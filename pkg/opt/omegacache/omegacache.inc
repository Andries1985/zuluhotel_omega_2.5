// omegacache.inc — Omega Cache data layer
//
// Core functions for item identity, DataFile storage, access control,
// and eligibility checking. Used by all Omega Cache scripts.

use uo;
use os;
use cfgfile;
use polsys;
use basic;

include "include/canstack";
include "include/client";
include ":housing:utility";

// ============================================================
// Constants
// ============================================================

const OMEGACACHE_OBJTYPE := 0xDF0A;

// ============================================================
// Key Generation
// ============================================================

// Collects all properties explicitly set on an item that differ
// from itemdesc defaults or fixed defaults. Returns a dictionary
// of name -> value. Used by both BuildItemKey (for hashing) and
// DepositItem (for storing on the DataFile element).
function GetNonDefaultProperties(item)
	var desc := GetItemDescriptor(item.objtype);
	var props := dictionary;

	// Properties compared against itemdesc defaults
	if(item.graphic != desc.Graphic)
		props["graphic"] := item.graphic;
	endif
	if(item.color != desc.Color)
		props["color"] := item.color;
	endif
	if(CDbl(item.quality) != CDbl(desc.Quality))
		props["quality"] := item.quality;
	endif

	// Script overrides — empty string means "no per-instance override"
	if(item.usescript != "")
		props["usescript"] := item.usescript;
	endif
	if(item.equipscript != "")
		props["equipscript"] := item.equipscript;
	endif
	if(item.snoopscript != "")
		props["snoopscript"] := item.snoopscript;
	endif

	// Properties with fixed defaults (0)
	if(item.newbie)
		props["newbie"] := 1;
	endif
	if(item.insured)
		props["insured"] := 1;
	endif
	if(item.cursed)
		props["cursed"] := 1;
	endif
	if(item.weight_multiplier_mod != 0)
		props["weight_multiplier_mod"] := item.weight_multiplier_mod;
	endif

	// CProps — always per-instance
	var cprop_names := GetObjPropertyNames(item);
	foreach propname in cprop_names
		props["cprop_" + propname] := GetObjProperty(item, propname);
	endforeach

	return props;
endfunction

// Returns "objtype|md5hash" for an item.
// Hash is computed from only non-default properties.
// Items with no overrides produce MD5("") — a constant.
function BuildItemKey(item)
	var props := GetNonDefaultProperties(item);

	// Sort property names alphabetically and build canonical string
	var names := props.keys();
	names.sort();

	var parts := array{};
	foreach name in names
		parts.append(name + "=" + CStr(props[name]));
	endforeach

	var canonical := "";
	var i := 1;
	foreach part in parts
		if(i > 1)
			canonical := canonical + "|";
		endif
		canonical := canonical + part;
		i := i + 1;
	endforeach

	var key := Hex(item.objtype) + "|" + MD5Encrypt(canonical);
	if(DEBUG_MODE)
		Print("[OmegaCache] BuildItemKey: objtype=" + Hex(item.objtype) + " canonical='" + canonical + "' key=" + key);
	endif
	return key;
endfunction

// Returns "objtype|md5hash" for a standard item with no overrides.
// Equivalent to: Hex(objtype) + "|" + MD5Encrypt("")
function BuildDefaultKey(objtype)
	return Hex(objtype) + "|" + MD5Encrypt("");
endfunction

// ============================================================
// DataFile Management
// ============================================================

// Opens (or creates) the house's Omega Cache DataFile.
// Returns the DataFile handle, or 0 on failure.
function OpenHouseStore(cabinet)
	var house_serial := GetObjProperty(cabinet, "houseserial");
	if(!house_serial)
		return 0;
	endif

	var dfname := ":omegacache:house_" + house_serial;
	var df := OpenDataFile(dfname);
	if(!df)
		df := CreateDataFile(dfname, DF_KEYTYPE_STRING);
		if(DEBUG_MODE)
			Print("[OmegaCache] OpenHouseStore: Created new DataFile " + dfname);
		endif
	endif
	return df;
endfunction

// Unloads the house's DataFile from memory.
function CloseHouseStore(house_serial)
	UnloadDataFile(":omegacache:house_" + house_serial);
endfunction

// ============================================================
// Transactions
// ============================================================

// Deposit an item into the cache. Adds item.amount to the element's qty.
// On first deposit for this key, stores weight and non-default properties.
// Returns the new total quantity.
function DepositItem(df, key, item)
	var elem := df.FindElement(key);
	if(!elem)
		elem := df.CreateElement(key);
	endif

	var current_qty := CInt(elem.GetProp("qty"));
	elem.SetProp("qty", current_qty + item.amount);

	// Store metadata on first deposit for this key
	if(current_qty == 0)
		elem.SetProp("objtype", item.objtype);
		elem.SetProp("weight", GetItemDescriptor(item.objtype).Weight);

		// Store non-default properties for recreation on withdrawal
		var props := GetNonDefaultProperties(item);
		foreach name in (props.keys())
			elem.SetProp(name, props[name]);
		endforeach
	endif

	if(DEBUG_MODE)
		Print("[OmegaCache] DepositItem: key=" + key + " added=" + item.amount + " total=" + (current_qty + item.amount));
	endif
	return current_qty + item.amount;
endfunction

// Withdraw an amount from the cache. Returns the actual amount withdrawn.
// Deletes the element if qty reaches 0.
function WithdrawItem(df, key, amount)
	var elem := df.FindElement(key);
	if(!elem)
		return 0;
	endif

	var available := CInt(elem.GetProp("qty"));
	var take := amount;
	if(take > available)
		take := available;
	endif

	if(take >= available)
		df.DeleteElement(key);
	else
		elem.SetProp("qty", available - take);
	endif

	if(DEBUG_MODE)
		Print("[OmegaCache] WithdrawItem: key=" + key + " requested=" + amount + " withdrawn=" + take + " remaining=" + (available - take));
	endif
	return take;
endfunction

// Query the stored quantity for a specific key.
function GetStoredAmount(df, key)
	var elem := df.FindElement(key);
	if(!elem)
		return 0;
	endif
	return CInt(elem.GetProp("qty"));
endfunction

// Sum qty across all keys matching an objtype prefix.
// Used by crafting which only knows the objtype, not the full key.
function GetStoredAmountByObjtype(df, objtype)
	var prefix := Hex(objtype) + "|";
	var prefix_len := len(prefix);
	var total := 0;
	foreach key in (df.Keys())
		if(key[1, prefix_len] == prefix)
			var elem := df.FindElement(key);
			if(elem)
				total := total + CInt(elem.GetProp("qty"));
			endif
		endif
	endforeach
	return total;
endfunction

// Returns all keys in the DataFile.
function GetAllStored(df)
	return df.Keys();
endfunction

// Returns 1 if no items are stored, 0 otherwise.
function IsStoreEmpty(df)
	var keys := df.Keys();
	return (keys.size() == 0);
endfunction

// ============================================================
// Validation
// ============================================================

var _omegacache_blacklist := 0;

// Checks if an item is eligible for Omega Cache storage.
// Must be stackable and not on the blacklist.
function IsEligibleForStorage(item)
	if(!item.stackable)
		if(DEBUG_MODE)
			Print("[OmegaCache] IsEligibleForStorage: " + Hex(item.objtype) + " rejected — not stackable");
		endif
		return 0;
	endif

	// Lazy-load blacklist
	if(!_omegacache_blacklist)
		_omegacache_blacklist := ReadConfigFile(":omegacache:blacklist");
	endif

	if(_omegacache_blacklist)
		if(FindConfigElem(_omegacache_blacklist, item.objtype))
			if(DEBUG_MODE)
				Print("[OmegaCache] IsEligibleForStorage: " + Hex(item.objtype) + " rejected — blacklisted");
			endif
			return 0;
		endif
	endif

	return 1;
endfunction

// ============================================================
// Access Control
// ============================================================

// Finds an Omega Cache item within interaction range of the player
// and validates house access privileges.
// Returns struct{ container, house, df } or 0.
function FindAccessibleContainer(who, required_privileges)
	if(DEBUG_MODE)
		Print("[OmegaCache] FindAccessibleContainer: searching near " + who.name + " at " + who.x + "," + who.y + "," + who.z);
	endif
	var nearby := ListItemsNearLocation(who.x, who.y, who.z, 4, who.realm);
	foreach item in nearby
		if(item.objtype == OMEGACACHE_OBJTYPE)
			var house_serial := GetObjProperty(item, "houseserial");
			if(!house_serial)
				continue;
			endif

			var house := SystemFindObjectBySerial(CInt(house_serial));
			if(!house)
				continue;
			endif

			// Owner — always passes
			if(GetObjProperty(house, "ownerserial") == who.serial)
				return struct{ container := item, house := house, df := OpenHouseStore(item) };
			endif

			// Co-owner — always passes
			if(IsCowner(who, house))
				return struct{ container := item, house := house, df := OpenHouseStore(item) };
			endif

			// GM+ — always passes
			if(who.cmdlevel >= 4)
				return struct{ container := item, house := house, df := OpenHouseStore(item) };
			endif

			// Friend — check required privileges
			var friends := GetObjProperty(house, "Friends");
			if(friends)
				foreach friend in friends
					if(friend[1] == who.serial)
						var has_all := 1;
						foreach priv in required_privileges
							if(!friend[priv])
								has_all := 0;
								break;
							endif
						endforeach
						if(has_all)
							return struct{ container := item, house := house, df := OpenHouseStore(item) };
						endif
					endif
				endforeach
			endif
		endif
	endforeach

	if(DEBUG_MODE)
		Print("[OmegaCache] FindAccessibleContainer: no accessible cache found for " + who.name);
	endif
	return 0;
endfunction
