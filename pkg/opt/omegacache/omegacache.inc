// omegacache.inc — Omega Cache data layer
//
// Core functions for item identity, DataFile storage, access control,
// and eligibility checking. Used by all Omega Cache scripts.

use uo;
use os;
use cfgfile;
use polsys;
use basic;
use math;

include "include/canstack";
include "include/client";
include "include/omegacache_utils";
include ":housing:utility";

// ============================================================
// Constants
// ============================================================

const OMEGACACHE_DEBUG_HUE := 53;

// Module-level debug target. Set by calling scripts before operations
// so that helper functions can send debug messages to the player.
var _debug_who := 0;

function OmegaCacheDebug(msg)
	if(DEBUG_MODE and _debug_who)
		SendSysMessage(_debug_who, "[OmegaCache] " + msg, 3, OMEGACACHE_DEBUG_HUE);
	endif
endfunction

// ============================================================
// Utility
// ============================================================

// Formats a number with thousand separators (e.g., 300000 -> "300,000").
function FormatNumber(num)
	var s := CStr(CInt(num));
	var result := "";
	var count := 0;
	var i := len(s);
	while(i > 0)
		if(count > 0 and (count % 3) == 0)
			result := "," + result;
		endif
		result := s[i] + result;
		count := count + 1;
		i := i - 1;
	endwhile
	return result;
endfunction

// Parses a string as a base-10 integer, stripping leading zeros
// to prevent CInt from interpreting "010000" as hex/octal.
function ParseDecimal(s)
	s := CStr(s);
	// Strip leading zeros
	while(len(s) > 1 and s[1] == "0")
		s := s[2, len(s)];
	endwhile
	return CInt(s);
endfunction

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

// Reads categories.cfg and builds lookups:
//   cat_names: array of category names
//   objtype_to_cat: dictionary of CInt(objtype) -> category_name
//   cat_icons: dictionary of category_name -> icon tile id
// Returns struct{ cat_names, objtype_to_cat, cat_icons }
function LoadCategoryLookup()
	var cat_cfg := ReadConfigFile(":omegacache:categories");
	var categories_elem := FindConfigElem(cat_cfg, "Categories");
	var icons_elem := FindConfigElem(cat_cfg, "Icons");
	var cat_names := array{};
	var objtype_to_cat := dictionary;
	var cat_icons := dictionary;

	if(categories_elem)
		cat_names := ListConfigElemProps(categories_elem);
	endif

	OmegaCacheDebug("LoadCategoryLookup: cat_names count=" + cat_names.size());
	foreach cat_name in cat_names
		var cat_elem := FindConfigElem(cat_cfg, cat_name);
		if(cat_elem)
			var objtypes := ListConfigElemProps(cat_elem);
			OmegaCacheDebug("LoadCategoryLookup: " + cat_name + " has " + objtypes.size() + " objtypes");
			foreach ot in objtypes
				OmegaCacheDebug("  ot=" + ot + " CInt=" + CInt(ot));
				objtype_to_cat[CInt(ot)] := cat_name;
			endforeach
		endif
		if(icons_elem)
			cat_icons[cat_name] := CInt(GetConfigString(icons_elem, cat_name));
		endif
	endforeach

	return struct{ cat_names := cat_names, objtype_to_cat := objtype_to_cat, cat_icons := cat_icons };
endfunction

// Returns the set of objtypes (as CInt keys) for a given category name.
// Returns dictionary of CInt(objtype) -> 1, or empty dictionary if not found.
function GetCategoryObjtypes(cat_name)
	var cat_cfg := ReadConfigFile(":omegacache:categories");
	var cat_elem := FindConfigElem(cat_cfg, cat_name);
	var result := dictionary;
	if(cat_elem)
		var objtypes := ListConfigElemProps(cat_elem);
		foreach ot in objtypes
			result[CInt(ot)] := 1;
		endforeach
	endif
	return result;
endfunction

// Returns a display name for an objtype, using itemdesc Desc or Name.
// Falls back to Hex(objtype) if no descriptor found.
// Optionally accepts a pre-fetched descriptor to avoid redundant lookups.
function GetItemDisplayName(objtype, desc_info := 0)
	if(!desc_info)
		desc_info := GetItemDescriptor(objtype);
	endif
	var name := "";
	if(desc_info)
		if(desc_info.Desc and desc_info.Desc != "")
			name := desc_info.Desc;
		elseif(desc_info.Name and desc_info.Name != "")
			name := desc_info.Name;
		endif
	endif
	if(name == "")
		return Hex(objtype);
	endif
	// Strip all tiledata pluralization placeholders "%s"
	var pos := Find(name, "%s", 1);
	while(pos > 0)
		var before := "";
		var after := "";
		if(pos > 1)
			before := name[1, pos - 1];
		endif
		if(pos + 2 <= len(name))
			after := name[pos + 2, len(name)];
		endif
		name := before + after;
		pos := Find(name, "%s", 1);
	endwhile
	// Trim trailing/leading whitespace
	while(len(name) > 0 and name[len(name)] == " ")
		name := name[1, len(name) - 1];
	endwhile
	while(len(name) > 0 and name[1] == " ")
		name := name[2, len(name)];
	endwhile
	return name;
endfunction

// Extracts the objtype string prefix from a DataFile key ("0xABCD|hash" -> "0xABCD").
function GetObjtypeFromKey(key)
	var pipe_pos := Find(key, "|", 1);
	if(pipe_pos > 0)
		return key[1, pipe_pos - 1];
	endif
	return key;
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
	OmegaCacheDebug("BuildItemKey: objtype=" + Hex(item.objtype) + " canonical='" + canonical + "' key=" + key);
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
// Thin wrapper — reads houseserial from cabinet CProp, delegates to shared utils.
// Returns the DataFile handle, or 0 on failure.
function OpenHouseStore(cabinet)
	var house_serial := GetObjProperty(cabinet, "houseserial");
	if(!house_serial)
		return 0;
	endif

	var df := OpenOmegaCacheStore(house_serial);
	OmegaCacheDebug("OpenHouseStore: opened for house serial " + house_serial);
	return df;
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

	OmegaCacheDebug("DepositItem: key=" + key + " added=" + item.amount + " total=" + (current_qty + item.amount));
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

	OmegaCacheDebug("WithdrawItem: key=" + key + " requested=" + amount + " withdrawn=" + take + " remaining=" + (available - take));
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

// IsStoreEmpty — use IsOmegaCacheEmpty(house_serial) from omegacache_utils.inc

// ============================================================
// Item Recreation (Withdrawal)
// ============================================================

// Recreates an item in a container from DataFile element properties.
// Only sets non-default properties stored on the element.
// Returns the created item, or error on failure.
function RecreateItem(container, elem, amount)
	var objtype := CInt(elem.GetProp("objtype"));
	var newitem := CreateItemInContainer(container, objtype, amount);
	if(!newitem)
		OmegaCacheDebug("RecreateItem: FAILED to create " + Hex(objtype) + " x" + amount + " - returned 0");
		return newitem;
	endif
	if(newitem.errortext)
		OmegaCacheDebug("RecreateItem: FAILED to create " + Hex(objtype) + " x" + amount + " - " + newitem.errortext);
		return newitem;
	endif

	// Restore non-default properties from element
	var propnames := elem.PropNames();
	foreach pname in propnames
		var val := elem.GetProp(pname);

		// Skip metadata properties
		if(pname == "qty" or pname == "objtype" or pname == "weight")
			continue;
		endif

		// Item properties compared against itemdesc
		if(pname == "color")
			newitem.color := CInt(val);
		elseif(pname == "graphic")
			newitem.graphic := CInt(val);
		elseif(pname == "quality")
			newitem.quality := CDbl(val);

		// Script overrides
		elseif(pname == "usescript")
			newitem.usescript := CStr(val);
		elseif(pname == "equipscript")
			newitem.equipscript := CStr(val);
		elseif(pname == "snoopscript")
			newitem.snoopscript := CStr(val);

		// Flags
		elseif(pname == "newbie")
			newitem.newbie := CInt(val);
		elseif(pname == "insured")
			newitem.insured := CInt(val);
		elseif(pname == "cursed")
			newitem.cursed := CInt(val);
		elseif(pname == "weight_multiplier_mod")
			newitem.weight_multiplier_mod := CDbl(val);

		// CProps — prefix "cprop_"
		elseif(pname[1,6] == "cprop_")
			var cprop_name := pname[7, len(pname)];
			SetObjProperty(newitem, cprop_name, val);
		endif
	endforeach

	OmegaCacheDebug("RecreateItem: created " + Hex(objtype) + " x" + amount + " serial=" + newitem.serial);
	return newitem;
endfunction

// Re-credits quantity to an existing element without needing a physical item.
// Used when item creation fails during withdrawal to undo the debit.
function ReCreditItem(df, key, amount)
	var elem := df.FindElement(key);
	if(!elem)
		// Element was deleted by WithdrawItem — can't re-credit without full properties.
		// This shouldn't happen if we create-first-then-debit.
		OmegaCacheDebug("ReCreditItem: WARNING - element not found for key=" + key);
		return 0;
	endif
	var current := CInt(elem.GetProp("qty"));
	elem.SetProp("qty", current + amount);
	OmegaCacheDebug("ReCreditItem: key=" + key + " re-credited " + amount + " new_total=" + (current + amount));
	return current + amount;
endfunction

// ============================================================
// Weight / Capacity Validation
// ============================================================

// Calculates the maximum amount withdrawable by weight, checking
// the full container parent chain. Returns the constrained amount.
function GetMaxWithdrawableByWeight(destination, item_weight, requested_amount)
	if(item_weight <= 0)
		return requested_amount;
	endif

	var max_by_weight := requested_amount;
	var container := destination;

	while(container)
		if(container.max_weight > 0)
			var available_weight := CDbl(container.max_weight) - CDbl(container.weight);
			var fits := CInt(available_weight / item_weight);
			if(fits < max_by_weight)
				max_by_weight := fits;
			endif
		endif
		container := container.container;
	endwhile

	if(max_by_weight < 0)
		max_by_weight := 0;
	endif

	return max_by_weight;
endfunction

// ============================================================
// Validation
// ============================================================

var _omegacache_blacklist := 0;

// Checks if an item is eligible for Omega Cache storage.
// Must be stackable and not on the blacklist.
function IsEligibleForStorage(item)
	if(!item.stackable)
		OmegaCacheDebug("IsEligibleForStorage: " + Hex(item.objtype) + " rejected - not stackable");
		return 0;
	endif

	// Lazy-load blacklist
	if(!_omegacache_blacklist)
		_omegacache_blacklist := ReadConfigFile(":omegacache:blacklist");
	endif

	if(_omegacache_blacklist)
		if(FindConfigElem(_omegacache_blacklist, item.objtype))
			OmegaCacheDebug("IsEligibleForStorage: " + Hex(item.objtype) + " rejected - blacklisted");
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
	_debug_who := who;
	OmegaCacheDebug("FindAccessibleContainer: searching near " + who.name + " at " + who.x + "," + who.y + "," + who.z);
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

	OmegaCacheDebug("FindAccessibleContainer: no accessible cache found for " + who.name);
	return 0;
endfunction

// ============================================================
// Deposit Operations
// ============================================================

// Deposits a single item into the cache.
function DepositSingleItem(who, df, item)
	if(!IsEligibleForStorage(item))
		SendSysMessage(who, "That item cannot be stored.");
		return 0;
	endif

	var key := BuildItemKey(item);
	var desc := item.desc;
	var amt := item.amount;
	DepositItem(df, key, item);
	DestroyItem(item);
	SendSysMessage(who, "Deposited: " + FormatNumber(amt) + " " + desc);
	return 1;
endfunction

// Deposits all eligible items from a container.
function DepositFromContainer(who, df, container)
	var deposited := dictionary;
	var skipped := 0;
	var total_deposited := 0;

	var items_to_deposit := array{};
	foreach thing in EnumerateItemsInContainer(container)
		if(!thing.isa(POLCLASS_CONTAINER))
			if(IsEligibleForStorage(thing))
				items_to_deposit.append(thing);
			else
				skipped := skipped + 1;
			endif
		endif
	endforeach

	foreach thing in items_to_deposit
		var key := BuildItemKey(thing);
		var desc := thing.desc;
		var amt := thing.amount;

		DepositItem(df, key, thing);
		DestroyItem(thing);

		if(!deposited[desc])
			deposited[desc] := amt;
		else
			deposited[desc] := deposited[desc] + amt;
		endif
		total_deposited := total_deposited + amt;
	endforeach

	if(total_deposited > 0)
		var parts := array{};
		foreach desc in (deposited.keys())
			parts.append(FormatNumber(deposited[desc]) + " " + desc);
		endforeach
		var summary := "";
		var i := 1;
		foreach part in parts
			if(i > 1)
				summary := summary + ", ";
			endif
			summary := summary + part;
			i := i + 1;
		endforeach
		SendSysMessage(who, "Deposited: " + summary);
	else
		SendSysMessage(who, "No eligible items to deposit.");
	endif

	if(skipped > 0)
		SendSysMessage(who, CStr(skipped) + " item(s) could not be stored.");
	endif

	return total_deposited;
endfunction

// Deposit with targeting loop.
function DoDepositTargeting(who, df)
	var tgt;
	while(1)
		SendSysMessage(who, "Target an item or container to deposit. Press ESC to stop.");
		tgt := Target(who);
		if(!tgt)
			break;
		endif

		if(tgt.isa(POLCLASS_CONTAINER))
			DepositFromContainer(who, df, tgt);
		else
			DepositSingleItem(who, df, tgt);
		endif
	endwhile
endfunction

// Deposit all eligible items from backpack.
function DoDepositAll(who, df)
	SendSysMessage(who, "Depositing all eligible items from your backpack...");
	var total := DepositFromContainer(who, df, who.backpack);
	if(total == 0)
		SendSysMessage(who, "No eligible items found in your backpack.");
	endif
	return total;
endfunction

// ============================================================
// Withdraw Operations
// ============================================================

// Withdraws items from the cache to a destination container.
function DoWithdraw(who, df, key, amount, destination)
	_debug_who := who;

	var elem := df.FindElement(key);
	if(!elem)
		SendSysMessage(who, "That item is no longer in the cache.");
		return 0;
	endif

	var available := CInt(elem.GetProp("qty"));
	if(amount <= 0)
		SendSysMessage(who, "Invalid amount.");
		return 0;
	endif
	if(amount > available)
		amount := available;
		SendSysMessage(who, "Only " + FormatNumber(available) + " available. Withdrawing all.");
	endif

	var objtype := CInt(elem.GetProp("objtype"));
	var desc_info := GetItemDescriptor(objtype);
	var item_weight := CDbl(elem.GetProp("weight"));

	var stack_limit := 60000;
	if(desc_info and desc_info.StackLimit)
		stack_limit := CInt(desc_info.StackLimit);
	endif

	var max_by_weight := GetMaxWithdrawableByWeight(destination, item_weight, amount);
	if(max_by_weight < amount)
		if(max_by_weight <= 0)
			SendSysMessage(who, "The destination cannot hold any more weight.");
			return 0;
		endif
		SendSysMessage(who, "Weight limit: can only withdraw " + FormatNumber(max_by_weight) + " (requested " + FormatNumber(amount) + ").");
		amount := max_by_weight;
	endif

	var stacks_needed := CInt(Ceil(CDbl(amount) / CDbl(stack_limit)));

	if(destination.max_items > 0)
		var current_items := 0;
		foreach thing in EnumerateItemsInContainer(destination, ENUMERATE_ROOT_ONLY)
			current_items := current_items + 1;
		endforeach
		var slots_available := destination.max_items - current_items;
		if(stacks_needed > slots_available)
			var max_stacks := slots_available;
			if(max_stacks <= 0)
				SendSysMessage(who, "The destination container is full.");
				return 0;
			endif
			amount := max_stacks * stack_limit;
			if(amount > available)
				amount := available;
			endif
			stacks_needed := max_stacks;
			SendSysMessage(who, "Container slot limit: withdrawing " + FormatNumber(amount) + " (" + stacks_needed + " stack(s)).");
		endif
	endif

	var total_withdrawn := 0;
	var remaining := amount;

	while(remaining > 0)
		var this_stack := remaining;
		if(this_stack > stack_limit)
			this_stack := stack_limit;
		endif

		var newitem := RecreateItem(destination, elem, this_stack);
		if(!newitem or (newitem and newitem.errortext))
			SendSysMessage(who, "Failed to create item. Stopping withdrawal.");
			break;
		endif

		WithdrawItem(df, key, this_stack);

		total_withdrawn := total_withdrawn + this_stack;
		remaining := remaining - this_stack;

		elem := df.FindElement(key);
		if(!elem and remaining > 0)
			SendSysMessage(who, "Cache depleted unexpectedly. Stopping.");
			break;
		endif
	endwhile

	if(total_withdrawn > 0)
		var item_name := GetItemDisplayName(objtype, desc_info);
		var stacks_created := CInt(Ceil(CDbl(total_withdrawn) / CDbl(stack_limit)));
		if(stacks_created > 1)
			SendSysMessage(who, "Withdrawn: " + FormatNumber(total_withdrawn) + " " + item_name + " (" + stacks_created + " stacks).");
		else
			SendSysMessage(who, "Withdrawn: " + FormatNumber(total_withdrawn) + " " + item_name + ".");
		endif
	endif

	return total_withdrawn;
endfunction

// Prompts for a destination container, defaulting to backpack on ESC.
function PromptDestination(who)
	SendSysMessage(who, "Target a container to receive items, or press ESC for your backpack.");
	var dest := Target(who);
	if(!dest)
		dest := who.backpack;
		SendSysMessage(who, "Using your backpack.");
	elseif(!dest.isa(POLCLASS_CONTAINER))
		SendSysMessage(who, "That is not a container. Using your backpack.");
		dest := who.backpack;
	endif
	return dest;
endfunction
