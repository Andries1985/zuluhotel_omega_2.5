// Shared player-vendor escrow helpers

use os;
use uo;
use util;
use datafile;
use storage;
use file;

const PLAYERVENDOR_ESCROW_DF_PREFIX := "merchantescrow_";
const ESCROW_STORAGE_NAME := "Merchant Escrow";
const MERCHANT_ESCROW_LOG_FILE := "::log/merchantescrow.log";

const ENTRY_ID_IDX := 1;
const ENTRY_OWNERSERIAL_IDX := 2;
const ENTRY_ESCROWNAME_IDX := 3;
const ENTRY_VENDORNAME_IDX := 4;
const ENTRY_VENDORSERIAL_IDX := 5;
const ENTRY_CREATED_IDX := 6;
const ENTRY_OWNERNAME_IDX := 7;

// Persistent troubleshooting log, independent of any live-console debug flags.
function MerchantEscrowLog(msg)
	LogToFile(MERCHANT_ESCROW_LOG_FILE, msg, LOG_DATETIME);
endfunction

function OpenEscrowStorage()
	var area := FindStorageArea(ESCROW_STORAGE_NAME);
	if(!area)
		area := CreateStorageArea(ESCROW_STORAGE_NAME);
	endif
	return area;
endfunction

// Percent-style escaping for the free-text fields (vendor/owner names) embedded
// in the "|"/"^"-delimited entry format, so a name containing either delimiter
// can't corrupt the encoding. Order matters: escape "%" first on encode (so it
// doesn't clobber the escapes just introduced for "|"/"^"), restore it last on
// decode (mirror order).
function EscrowEscapeField(s)
	s := CStr(s);
	s := StrReplace(s, "%", "%25");
	s := StrReplace(s, "|", "%7C");
	s := StrReplace(s, "^", "%5E");
	return s;
endfunction

function EscrowUnescapeField(s)
	s := CStr(s);
	s := StrReplace(s, "%7C", "|");
	s := StrReplace(s, "%5E", "^");
	s := StrReplace(s, "%25", "%");
	return s;
endfunction

function EncodeEscrowEntryFields(id, ownerserial, escrow_name, vendor_name, vendor_serial, created, owner_name)
	return CStr(id) + "|" + CStr(ownerserial) + "|" + CStr(escrow_name) + "|" + EscrowEscapeField(vendor_name) + "|" + CStr(vendor_serial) + "|" + CStr(created) + "|" + EscrowEscapeField(owner_name);
endfunction

function EncodeEscrowEntries(entries)
	var encoded := "";
	foreach entry in entries
		if(encoded)
			encoded := encoded + "^";
		endif
		encoded := encoded + EncodeEscrowEntryFields(EntryId(entry), EntryOwnerSerial(entry), EntryEscrowName(entry), EntryVendorName(entry), EntryVendorSerial(entry), EntryCreated(entry), EntryOwnerName(entry));
	endforeach
	return encoded;
endfunction

function DecodeEscrowEntries(entries_str)
	var entries := {};
	if(!entries_str)
		return entries;
	endif

	var parts := SplitWords(entries_str, "^");
	foreach part in parts
		if(part)
			var fields := SplitWords(part, "|");
			if(len(fields) >= 7)
				entries.append({
					fields[1],
					CInt(fields[2]),
					fields[3],
					EscrowUnescapeField(fields[4]),
					CInt(fields[5]),
					CInt(fields[6]),
					EscrowUnescapeField(fields[7])
				});
			endif
		endif
	endforeach

	return entries;
endfunction

function EntryId(entry)
	if(entry.id)
		return entry.id;
	endif
	if(len(entry) >= ENTRY_ID_IDX)
		return entry[ENTRY_ID_IDX];
	endif
	return "";
endfunction

function EntryOwnerSerial(entry)
	if(entry.ownerserial)
		return CInt(entry.ownerserial);
	endif
	if(len(entry) >= ENTRY_OWNERSERIAL_IDX)
		return CInt(entry[ENTRY_OWNERSERIAL_IDX]);
	endif
	return 0;
endfunction

function EntryEscrowName(entry)
	if(entry.escrowname)
		return entry.escrowname;
	endif
	if(len(entry) >= ENTRY_ESCROWNAME_IDX)
		return entry[ENTRY_ESCROWNAME_IDX];
	endif
	return "";
endfunction

function EntryVendorName(entry)
	if(entry.vendorname)
		return entry.vendorname;
	endif
	if(len(entry) >= ENTRY_VENDORNAME_IDX)
		return entry[ENTRY_VENDORNAME_IDX];
	endif
	return "Player Merchant";
endfunction

function EntryVendorSerial(entry)
	if(entry.vendorserial)
		return CInt(entry.vendorserial);
	endif
	if(len(entry) >= ENTRY_VENDORSERIAL_IDX)
		return CInt(entry[ENTRY_VENDORSERIAL_IDX]);
	endif
	return 0;
endfunction

function EntryCreated(entry)
	if(entry.created)
		return CInt(entry.created);
	endif
	if(len(entry) >= ENTRY_CREATED_IDX)
		return CInt(entry[ENTRY_CREATED_IDX]);
	endif
	return 0;
endfunction

function EntryOwnerName(entry)
	if(entry.ownername)
		return entry.ownername;
	endif
	if(len(entry) >= ENTRY_OWNERNAME_IDX and entry[ENTRY_OWNERNAME_IDX])
		return entry[ENTRY_OWNERNAME_IDX];
	endif
	return "Unknown Owner";
endfunction

function BuildLegacyEscrowDatafileName(player_obj)
	if(!player_obj)
		return "";
	endif

	var name := player_obj.name;
	var sanitized := StrReplace(name, " ", "_");
	return PLAYERVENDOR_ESCROW_DF_PREFIX + sanitized + "_" + player_obj.serial;
endfunction

function CopyLegacyEscrowEntriesForVendorKey(legacy_df, canonical_df, vendor_key)
	if(!legacy_df or !canonical_df or !vendor_key)
		return;
	endif

	var legacy_elem := legacy_df.FindElement(vendor_key);
	if(!legacy_elem)
		return;
	endif

	var legacy_entries := legacy_elem.GetProp("entries");
	if(!legacy_entries)
		return;
	endif

	var canonical_elem := canonical_df.FindElement(vendor_key);
	if(!canonical_elem)
		canonical_elem := canonical_df.CreateElement(vendor_key);
	endif

	if(canonical_elem and !canonical_elem.GetProp("entries"))
		canonical_elem.SetProp("entries", legacy_entries);
	endif
endfunction

function ResolveEscrowDatafileNameForSerial(player_obj, merchant_serial, migrate_to_canonical := 0)
	if(!player_obj)
		return "";
	endif

	var canonical_name := BuildEscrowDatafileName(player_obj);
	if(!canonical_name)
		return "";
	endif

	var legacy_name := BuildLegacyEscrowDatafileName(player_obj);
	if(!legacy_name or legacy_name == canonical_name)
		return canonical_name;
	endif

	var vendor_key := CStr(merchant_serial);
	var canonical_has_key := 0;
	var legacy_has_key := 0;

	var canonical_df := OpenDataFile(canonical_name);
	if(canonical_df)
		var canonical_elem := canonical_df.FindElement(vendor_key);
		if(canonical_elem and canonical_elem.GetProp("entries"))
			canonical_has_key := 1;
		endif
		UnloadDataFile(canonical_name);
	endif

	if(canonical_has_key)
		return canonical_name;
	endif

	var legacy_df := OpenDataFile(legacy_name);
	if(!legacy_df)
		return canonical_name;
	endif

	var legacy_elem := legacy_df.FindElement(vendor_key);
	if(legacy_elem and legacy_elem.GetProp("entries"))
		legacy_has_key := 1;
	endif

	if(legacy_has_key and migrate_to_canonical)
		canonical_df := OpenDataFile(canonical_name);
		if(!canonical_df)
			CreateDataFile(canonical_name);
			canonical_df := OpenDataFile(canonical_name);
		endif

		if(canonical_df)
			CopyLegacyEscrowEntriesForVendorKey(legacy_df, canonical_df, vendor_key);
			UnloadDataFile(canonical_name);
			UnloadDataFile(legacy_name);
			return canonical_name;
		endif
	endif

	UnloadDataFile(legacy_name);
	if(legacy_has_key)
		return legacy_name;
	endif

	return canonical_name;
endfunction

function BuildEscrowDatafileName(player_obj)
	if(!player_obj)
		return "";
	endif

	return BuildEscrowDatafileNameForSerial(player_obj.serial);
endfunction

// Takes a raw serial rather than a live object -- this must keep working even
// when the owning character can no longer be resolved (deleted/missing), so an
// escrow entry can still be filed and found later instead of silently dropped.
// Deliberately accepts a serial of 0 (a masterless merchant has no owner to
// key by) rather than rejecting it -- that still produces a valid, shared
// "merchantescrow_0" datafile that masterless-merchant entries can be indexed
// into (keyed per-merchant within it), instead of failing to build a name at
// all and leaving the escrow root permanently unindexed.
//
// Package-scoped (":playervendor:...") is mandatory -- see the identical fix
// on pkg/std/housing's BuildHouseEscrowDatafileNameForSerial for why a bare
// filespec is unsafe (it apparently resolves relative to the calling
// script's own package). This one happened to work by accident so far only
// because every current caller (playermerchant.src, escrow.src,
// pmescrowtest.src) lives inside pkg/systems/playervendor itself.
function BuildEscrowDatafileNameForSerial(owner_serial)
	return ":playervendor:" + PLAYERVENDOR_ESCROW_DF_PREFIX + CStr(owner_serial);
endfunction
