// Shared player-vendor escrow helpers

use os;
use uo;
use util;
use datafile;

const PLAYERVENDOR_ESCROW_DF_PREFIX := "merchantescrow_";

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

	return PLAYERVENDOR_ESCROW_DF_PREFIX + player_obj.serial;
endfunction
