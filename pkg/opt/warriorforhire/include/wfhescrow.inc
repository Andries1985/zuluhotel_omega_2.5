// wfhescrow.inc - Warrior for Hire gear escrow (Zuluhotel Omega 2.5 version of ZH3.0's include).
//
// When a hired warrior's corpse decays, whatever is still in it is bundled into a backpack and
// parked in the player-vendor "Merchant Escrow" storage area for 90 days. The package is indexed
// in the owner's escrow datafile under the vendor name "Warrior for Hire" (so .escrow hides it)
// and is recovered through the High Priest ("recover belongings", 20,000 gold). A package-wide
// registry datafile lists every live entry so escrowsweep.src can expire them across all owners.
//
// Differences from ZH3.0: entries are encoded and decoded with escrow.inc's shared helpers
// instead of hand-built strings; the per-owner index is keyed by serial
// (BuildEscrowDatafileNameForSerial) so it is written even when the owner cannot be resolved;
// and the registry filespec is package scoped - see BuildEscrowDatafileNameForSerial for why a
// bare filespec is unsafe. Expiry is measured on the wall clock (POLCORE().systime), never on
// ReadGameClock(), which resets at every restart.

use uo;
use os;
use polsys;
use storage;
use datafile;

include ":playervendor:include/escrow";
include "include/objtype";
include "include/random";
include "include/myutil";

const WFH_ESCROW_REGISTRY_DATAFILE := ":warriorforhire:wfhescrowregistry";
const WFH_ESCROW_EXPIRE_SECONDS := 7776000; // 90 days
const WFH_ESCROW_ROOT_NAME := "Warrior for Hire Escrow";

///////////////////////////////////////////////////////////////////////////
// Filing
///////////////////////////////////////////////////////////////////////////

// Parks payout_pack (a backpack holding the corpse's gear) in escrow for ownerserial.
// sourceserial is the corpse the gear came from; it keys the owner's per-vendor index.
// Returns 1 when the gear is safely in storage, 0 when it is still where it was.
function WFH_SaveToEscrow(payout_pack, ownerserial, sourceserial)

	ownerserial := CInt(ownerserial);
	sourceserial := CInt(sourceserial);

	var area := OpenEscrowStorage();
	if (!area)
		return 0;
	endif

	var escrow_name := "WFHEscrow " + CStr(ownerserial) + " " + CStr(ReadGameClock()) + " " + CStr(Random(99999));
	var escrow_root := CreateRootItemInStorageArea(area, escrow_name, UOBJ_BACKPACK);
	if (!escrow_root)
		return 0;
	endif

	var created := POLCORE().systime;

	escrow_root.name := WFH_ESCROW_ROOT_NAME;
	SetObjProperty(escrow_root, "ownerserial", ownerserial);
	SetObjProperty(escrow_root, "vendorserial", sourceserial);
	SetObjProperty(escrow_root, "vendorname", ESCROW_WFH_VENDOR_NAME);
	SetObjProperty(escrow_root, "created", created);

	if (!MoveItemToContainer(payout_pack, escrow_root))
		DestroyItem(escrow_root);
		DestroyRootItemInStorageArea(area, escrow_name);
		return 0;
	endif

	// Registered first and unconditionally: this is the only record the sweeper can find
	// "every entry across every owner" through, so it must exist even if indexing fails below.
	var entry_id := CStr(ownerserial) + "-" + CStr(created) + "-" + CStr(Random(100000));
	WFH_RegisterEscrowEntry(entry_id, ownerserial, escrow_name, created);

	var owner_name := "Unknown Owner";
	var owner_obj := SystemFindObjectBySerial(ownerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	if (owner_obj and owner_obj.name)
		owner_name := owner_obj.name;
	endif

	var df_name := BuildEscrowDatafileNameForSerial(ownerserial);
	var df := OpenDataFile(df_name);
	if (!df)
		CreateDataFile(df_name);
		df := OpenDataFile(df_name);
	endif
	if (!df)
		MerchantEscrowLog("[ERROR] WFH escrow: filed " + escrow_name + " for owner=" + ownerserial + " but could not open index " + df_name);
		return 1;
	endif

	var vendor_key := CStr(sourceserial);
	var vendor_elem := df.FindElement(vendor_key);
	if (!vendor_elem)
		vendor_elem := df.CreateElement(vendor_key);
	endif
	if (!vendor_elem)
		UnloadDataFile(df_name);
		MerchantEscrowLog("[ERROR] WFH escrow: filed " + escrow_name + " for owner=" + ownerserial + " but could not create index element " + vendor_key);
		return 1;
	endif

	var entries := DecodeEscrowEntries(vendor_elem.GetProp("entries"));
	entries.append({entry_id, ownerserial, escrow_name, ESCROW_WFH_VENDOR_NAME, sourceserial, created, owner_name});
	vendor_elem.SetProp("entries", EncodeEscrowEntries(entries));
	UnloadDataFile(df_name);

	MerchantEscrowLog("WFH escrow: filed " + escrow_name + " entry_id=" + entry_id + " owner=" + ownerserial + " (" + owner_name + ") corpse=" + sourceserial);
	return 1;

endfunction

///////////////////////////////////////////////////////////////////////////
// Registry (package-wide index used only by the sweeper)
///////////////////////////////////////////////////////////////////////////

function WFH_RegisterEscrowEntry(entry_id, ownerserial, escrow_name, created)

	var df := OpenDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
	if (!df)
		CreateDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
		df := OpenDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
		if (!df)
			return 0;
		endif
	endif

	var elem := df.FindElement(entry_id);
	if (!elem)
		elem := df.CreateElement(entry_id);
	endif
	if (!elem)
		UnloadDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
		return 0;
	endif

	elem.SetProp("ownerserial", CInt(ownerserial));
	elem.SetProp("escrow_name", CStr(escrow_name));
	elem.SetProp("created", CInt(created));

	UnloadDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
	return 1;

endfunction

function WFH_UnregisterEscrowEntry(entry_id)

	var df := OpenDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
	if (!df)
		return;
	endif

	df.DeleteElement(entry_id);
	UnloadDataFile(WFH_ESCROW_REGISTRY_DATAFILE);

endfunction

// Called daily by escrowsweep.src. Destroys every registry entry older than 90 real days,
// working straight off the registry's own escrow_name so entries whose owner can no longer
// be resolved are cleaned up too. Returns the number of entries expired.
function WFH_SweepExpiredEscrow()

	var df := OpenDataFile(WFH_ESCROW_REGISTRY_DATAFILE);
	if (!df)
		return 0;
	endif

	var now := POLCORE().systime;
	var expired := {}; // { entry_id, ownerserial, escrow_name }

	foreach entry_id in (df.Keys())
		var elem := df.FindElement(entry_id);
		if (!elem)
			continue;
		endif
		var created := CInt(elem.GetProp("created"));
		if (!created)
			continue;
		endif
		if ((now - created) >= WFH_ESCROW_EXPIRE_SECONDS)
			expired.append({CStr(entry_id), CInt(elem.GetProp("ownerserial")), CStr(elem.GetProp("escrow_name"))});
		endif
	endforeach

	// One open/delete-many/close pass for the registry instead of a reopen per entry.
	foreach row in expired
		df.DeleteElement(row[1]);
	endforeach
	UnloadDataFile(WFH_ESCROW_REGISTRY_DATAFILE);

	foreach row in expired
		WFH_DestroyEscrowContents(row[2], {row[1], row[2], row[3], ESCROW_WFH_VENDOR_NAME, 0, 0, ""});
		MerchantEscrowLog("WFH escrow: expired entry_id=" + row[1] + " owner=" + row[2] + " (" + row[3] + ")");
	endforeach

	return len(expired);

endfunction

///////////////////////////////////////////////////////////////////////////
// Per-owner access (High Priest)
///////////////////////////////////////////////////////////////////////////

// Every Warrior for Hire escrow entry indexed for ownerobj, oldest first, as decoded entry arrays.
function WFH_LoadEscrowEntriesForOwner(ownerobj)

	var entries := {};
	var df_name := BuildEscrowDatafileName(ownerobj);
	if (!df_name)
		return entries;
	endif

	var df := OpenDataFile(df_name);
	if (!df)
		return entries;
	endif

	foreach vendor_key in (df.Keys())
		var vendor_elem := df.FindElement(vendor_key);
		if (!vendor_elem)
			continue;
		endif
		foreach entry in (DecodeEscrowEntries(vendor_elem.GetProp("entries")))
			if (EntryVendorName(entry) == ESCROW_WFH_VENDOR_NAME)
				entries.append(entry);
			endif
		endforeach
	endforeach

	UnloadDataFile(df_name);
	return entries;

endfunction

// Moves the package's gear into who's backpack. Returns the number of items moved; the entry
// is only closed out once nothing is left in it, so a full backpack can come back for the rest.
function WFH_ClaimEscrowEntry(who, entry)

	var area := FindStorageArea(ESCROW_STORAGE_NAME);
	if (!area)
		return 0;
	endif

	var entry_id := EntryId(entry);
	var escrow_name := EntryEscrowName(entry);
	var escrowroot := FindRootItemInStorageArea(area, escrow_name);
	if (!escrowroot)
		// Storage item is gone: drop the dangling index entry so it stops being offered.
		WFH_RemoveEscrowEntryById(who.serial, entry_id);
		WFH_UnregisterEscrowEntry(entry_id);
		return 0;
	endif

	var moved := 0;
	foreach item in ListRootItemsInContainer(escrowroot)
		if (MoveItemToContainer(item, who.backpack))
			moved := moved + 1;
		else
			break;
		endif
	endforeach

	if (!len(ListRootItemsInContainer(escrowroot)))
		DestroyItem(escrowroot);
		DestroyRootItemInStorageArea(area, escrow_name);
		WFH_RemoveEscrowEntryById(who.serial, entry_id);
		WFH_UnregisterEscrowEntry(entry_id);
	endif

	MerchantEscrowLog("WFH escrow: claimant=" + who.serial + " claimed entry_id=" + entry_id + " moved=" + moved);
	return moved;

endfunction

// Destroys the storage item and the per-owner index entry only. The registry row is left to
// the caller: the sweeper removes its rows in one batch, WFH_DestroyEscrowEntry removes one.
function WFH_DestroyEscrowContents(ownerserial, entry)

	var area := FindStorageArea(ESCROW_STORAGE_NAME);
	if (area)
		var escrow_name := EntryEscrowName(entry);
		var escrowroot := FindRootItemInStorageArea(area, escrow_name);
		if (escrowroot)
			foreach item in EnumerateItemsInContainer(escrowroot)
				DestroyItem(item);
			endforeach
			DestroyItem(escrowroot);
			DestroyRootItemInStorageArea(area, escrow_name);
		endif
	endif

	WFH_RemoveEscrowEntryById(ownerserial, EntryId(entry));

endfunction

function WFH_DestroyEscrowEntry(ownerserial, entry)

	WFH_DestroyEscrowContents(ownerserial, entry);
	WFH_UnregisterEscrowEntry(EntryId(entry));
	MerchantEscrowLog("WFH escrow: destroyed entry_id=" + EntryId(entry) + " owner=" + ownerserial);

endfunction

// Removes one entry from the owner's per-vendor index, dropping the vendor element when empty.
function WFH_RemoveEscrowEntryById(ownerserial, entry_id)

	var df_name := BuildEscrowDatafileNameForSerial(CInt(ownerserial));
	var df := OpenDataFile(df_name);
	if (!df)
		return;
	endif

	foreach vendor_key in (df.Keys())
		var vendor_elem := df.FindElement(vendor_key);
		if (!vendor_elem)
			continue;
		endif
		var entries := DecodeEscrowEntries(vendor_elem.GetProp("entries"));
		if (!len(entries))
			continue;
		endif

		var kept := {};
		var found := 0;
		foreach entry in entries
			if (EntryId(entry) == entry_id)
				found := 1;
			else
				kept.append(entry);
			endif
		endforeach

		if (found)
			if (len(kept))
				vendor_elem.SetProp("entries", EncodeEscrowEntries(kept));
			else
				df.DeleteElement(vendor_key);
			endif
		endif
	endforeach

	UnloadDataFile(df_name);

endfunction
