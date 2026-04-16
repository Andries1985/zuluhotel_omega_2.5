// omegacache.inc — Omega Cache data layer and gump UI
//
// Core functions for item identity, DataFile storage, access control,
// eligibility checking, and gump display. Used by all Omega Cache scripts.

use uo;
use os;
use cfgfile;
use polsys;
use basic;
use math;

include "include/canstack";
include "include/client";
include "include/constants/usescriptids";
include "include/omegacache_utils";
include ":housing:utility";
include ":gumps:gumps";
include ":gumps:gumps_ex";

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
	if(len(s) == 0)
		return 0;
	endif
	// Validate all characters are digits
	var i := 1;
	while(i <= len(s))
		var ch := s[i];
		if(ch < "0" or ch > "9")
			return 0;
		endif
		i := i + 1;
	endwhile
	// Strip leading zeros
	while(len(s) > 1 and s[1] == "0")
		s := s[2, len(s)];
	endwhile
	return CInt(s);
endfunction

// Converts PascalCase to space-separated display name.
// e.g., "SpellbookScrolls" -> "Spellbook Scrolls"
//       "ClothAndFibre" -> "Cloth And Fibre"
//       "RawHerbs" -> "Raw Herbs"
function FormatCategoryName(name)
	var result := "";
	var i := 1;
	while(i <= len(name))
		var ch := name[i];
		// Insert space before uppercase letter (except at start)
		if(i > 1 and ch >= "A" and ch <= "Z")
			result := result + " " + ch;
		else
			result := result + ch;
		endif
		i := i + 1;
	endwhile
	return result;
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

	if(!desc)
		return props;
	endif

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

	// CProps — per-instance, skip ignored CProps from stacking_ignore.cfg
	var ignore_cfg := ReadConfigFile(":omegacache:stacking_ignore");
	var ignore_elem := FindConfigElem(ignore_cfg, "IgnoreCProps");
	var ignore_list := dictionary;
	if(ignore_elem)
		foreach propname in (ListConfigElemProps(ignore_elem))
			ignore_list[propname] := 1;
		endforeach
	endif

	var cprop_names := GetObjPropertyNames(item);
	foreach propname in cprop_names
		if(!ignore_list[propname])
			props["cprop_" + propname] := GetObjProperty(item, propname);
		endif
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
	var icon_colors_elem := FindConfigElem(cat_cfg, "IconColors");
	var cat_names := array{};
	var objtype_to_cat := dictionary;
	var cat_icons := dictionary;
	var cat_icon_colors := dictionary;

	if(categories_elem)
		cat_names := ListConfigElemProps(categories_elem);
	endif

	foreach cat_name in cat_names
		var cat_elem := FindConfigElem(cat_cfg, cat_name);
		if(cat_elem)
			var objtypes := ListConfigElemProps(cat_elem);
			foreach ot in objtypes
				objtype_to_cat[CInt(ot)] := cat_name;
			endforeach
		endif
		if(icons_elem)
			cat_icons[cat_name] := CInt(GetConfigString(icons_elem, cat_name));
		endif
		if(icon_colors_elem)
			var clr := CInt(GetConfigString(icon_colors_elem, cat_name));
			if(clr)
				cat_icon_colors[cat_name] := clr;
			endif
		endif
	endforeach

	return struct{ cat_names := cat_names, objtype_to_cat := objtype_to_cat, cat_icons := cat_icons, cat_icon_colors := cat_icon_colors };
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
	// Strip all tiledata pluralization placeholders ("%s", "%es", lone "%")
	var pos := Find(name, "%", 1);
	while(pos > 0)
		var before := "";
		var after := "";
		if(pos > 1)
			before := name[1, pos - 1];
		endif
		// Determine how many chars the placeholder spans
		var skip := 1; // lone "%"
		if(pos + 2 <= len(name) and name[pos + 1, pos + 2] == "es")
			skip := 3; // "%es"
		elseif(pos + 1 <= len(name) and name[pos + 1] == "s")
			skip := 2; // "%s"
		endif
		if(pos + skip <= len(name))
			after := name[pos + skip, len(name)];
		endif
		name := before + after;
		pos := Find(name, "%", 1);
	endwhile
	// Trim trailing/leading whitespace
	while(len(name) > 0 and name[len(name)] == " ")
		name := name[1, len(name) - 1];
	endwhile
	while(len(name) > 0 and name[1] == " ")
		name := name[2, len(name)];
	endwhile
	// Capitalize first letter of each word
	var result := "";
	var cap_next := 1;
	var ci := 1;
	while(ci <= len(name))
		var ch := name[ci];
		if(ch == " ")
			cap_next := 1;
			result := result + ch;
		elseif(cap_next)
			result := result + Upper(ch);
			cap_next := 0;
		else
			result := result + ch;
		endif
		ci := ci + 1;
	endwhile
	return result;
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

	// POL's MD5Encrypt rejects empty strings — use a space as sentinel for "no properties"
	if(!canonical or canonical == "")
		canonical := " ";
	endif

	var key := Hex(item.objtype) + "|" + MD5Encrypt(canonical);
	OmegaCacheDebug("BuildItemKey: objtype=" + Hex(item.objtype) + " canonical='" + canonical + "' key=" + key);
	return key;
endfunction

// Returns "objtype|md5hash" for a standard item with no overrides.
// Uses a space character for MD5 since POL's MD5Encrypt rejects empty strings.
// This must match what BuildItemKey produces for items with no non-default properties.
function BuildDefaultKey(objtype)
	return Hex(objtype) + "|" + MD5Encrypt(" ");
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
	OmegaCacheDebug("OpenHouseStore: house_serial=" + house_serial + " df=" + df);
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
		var dep_desc := GetItemDescriptor(item.objtype);
		if(dep_desc)
			elem.SetProp("weight", dep_desc.Weight);
		endif

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
function WithdrawItem(df, key, amount, exclude_lease_key := 0)
	var elem := df.FindElement(key);
	if(!elem)
		return 0;
	endif

	var raw_qty := CInt(elem.GetProp("qty"));
	var leased := GetLeasedAmount(df, key, exclude_lease_key);
	var available := raw_qty - leased;
	if(available < 0)
		available := 0;
	endif

	var take := amount;
	if(take > available)
		take := available;
	endif

	if(take <= 0)
		return 0;
	endif

	var new_qty := raw_qty - take;
	if(new_qty <= 0)
		df.DeleteElement(key);
	else
		elem.SetProp("qty", new_qty);
	endif

	OmegaCacheDebug("WithdrawItem: key=" + key + " requested=" + amount + " withdrawn=" + take + " remaining=" + new_qty);
	return take;
endfunction

// Query the stored quantity for a specific key (lease-aware).
// Pass exclude_lease_key to exclude a specific lease from the calculation
// (e.g., the caller's own lease, so they can consume their reserved portion).
function GetStoredAmount(df, key, exclude_lease_key := 0)
	var elem := df.FindElement(key);
	if(!elem)
		return 0;
	endif
	var qty := CInt(elem.GetProp("qty"));
	var leased := GetLeasedAmount(df, key, exclude_lease_key);
	var available := qty - leased;
	if(available < 0)
		available := 0;
	endif
	return available;
endfunction

// Sum qty across all keys matching an objtype prefix (lease-aware).
// Skips RL# lease keys. Used by crafting which only knows the objtype.
function GetStoredAmountByObjtype(df, objtype, exclude_lease_key := 0)
	var prefix := Hex(objtype) + "|";
	var prefix_len := len(prefix);
	var total := 0;
	foreach key in (df.Keys())
		// Skip lease keys
		if(key[1, 3] == "RL#")
			continue;
		endif
		if(key[1, prefix_len] == prefix)
			var elem := df.FindElement(key);
			if(elem)
				var qty := CInt(elem.GetProp("qty"));
				var leased := GetLeasedAmount(df, key, exclude_lease_key);
				var available := qty - leased;
				if(available > 0)
					total := total + available;
				endif
			endif
		endif
	endforeach
	return total;
endfunction

// Returns all item keys in the DataFile (excludes lease keys).
function GetAllStored(df)
	var keys := df.Keys();
	var item_keys := array{};
	foreach key in keys
		if(key[1, 3] != "RL#")
			item_keys.append(key);
		endif
	endforeach
	return item_keys;
endfunction

// ============================================================
// Resource Leases
// ============================================================

const LEASE_PREFIX := "RL#";
const LEASE_DEFAULT_TTL := 60;

// Creates a lease reserving a quantity of a specific item key.
// Returns the lease key string.
function CreateLease(df, item_key, who, quantity, ttl := LEASE_DEFAULT_TTL)
	var lease_id := CStr(who.serial) + "_" + CStr(GetPid());
	var lease_key := LEASE_PREFIX + item_key + "|" + lease_id;

	var elem := df.CreateElement(lease_key);
	if(!elem)
		// Element may already exist (re-lease)
		elem := df.FindElement(lease_key);
		if(!elem)
			OmegaCacheDebug("CreateLease: FAILED to create element for " + lease_key);
			return 0;
		endif
	endif

	elem.SetProp("quantity", quantity);
	elem.SetProp("expiry", ReadGameClock() + ttl);
	OmegaCacheDebug("CreateLease: " + lease_key + " qty=" + quantity + " ttl=" + ttl);
	return lease_key;
endfunction

// Extends an existing lease's TTL if the item still has enough quantity
// (minus all other leases) to support this lease. If not, deletes the
// lease and returns 0.
function ExtendLease(df, lease_key, ttl := LEASE_DEFAULT_TTL)
	var elem := df.FindElement(lease_key);
	if(!elem)
		OmegaCacheDebug("ExtendLease: lease not found (expired?) " + lease_key);
		return 0;
	endif

	var lease_qty := CInt(elem.GetProp("quantity"));

	// Extract the item key from the lease key: RL#<item_key>|<lease_id>
	// Item key contains one |, so find the second | after RL#
	var after_prefix := lease_key[4, len(lease_key)];  // strip "RL#"
	// Find the last | to separate item_key from lease_id
	var last_pipe := 0;
	var i := len(after_prefix);
	while(i > 0)
		if(after_prefix[i] == "|")
			last_pipe := i;
			break;
		endif
		i := i - 1;
	endwhile

	if(last_pipe > 0)
		var item_key := after_prefix[1, last_pipe - 1];
		var item_elem := df.FindElement(item_key);
		if(item_elem)
			var raw_qty := CInt(item_elem.GetProp("qty"));
			var total_leased := GetLeasedAmount(df, item_key);
			// Available = raw qty - all leases (including this one)
			// This lease is already counted in total_leased, so available
			// for *other* use is raw_qty - total_leased. For this lease
			// to be valid, raw_qty - (total_leased - lease_qty) >= lease_qty
			// i.e., raw_qty - total_leased + lease_qty >= lease_qty
			// i.e., raw_qty >= total_leased
			if(raw_qty < total_leased)
				// Not enough stock to sustain this lease
				df.DeleteElement(lease_key);
				OmegaCacheDebug("ExtendLease: insufficient stock, deleted " + lease_key);
				return 0;
			endif
		else
			// Item no longer exists
			df.DeleteElement(lease_key);
			OmegaCacheDebug("ExtendLease: item gone, deleted " + lease_key);
			return 0;
		endif
	endif

	elem.SetProp("expiry", ReadGameClock() + ttl);
	return 1;
endfunction

// Releases (deletes) a lease.
function ReleaseLease(df, lease_key)
	if(lease_key)
		df.DeleteElement(lease_key);
		OmegaCacheDebug("ReleaseLease: " + lease_key);
	endif
endfunction

// Returns the total leased amount for a specific item key.
// Cleans up expired leases as it scans.
// Pass exclude_lease_key to exclude a specific lease (e.g., the caller's own).
function GetLeasedAmount(df, item_key, exclude_lease_key := 0)
	var lease_prefix := LEASE_PREFIX + item_key + "|";
	var lease_prefix_len := len(lease_prefix);
	var total := 0;
	var now := ReadGameClock();

	foreach key in (df.Keys())
		if(key[1, lease_prefix_len] == lease_prefix)
			// Skip the excluded lease
			if(exclude_lease_key and key == exclude_lease_key)
				continue;
			endif
			var elem := df.FindElement(key);
			if(elem)
				var expiry := CInt(elem.GetProp("expiry"));
				if(expiry < now)
					// Expired — clean up
					df.DeleteElement(key);
					OmegaCacheDebug("GetLeasedAmount: cleaned expired lease " + key);
				else
					total := total + CInt(elem.GetProp("quantity"));
				endif
			endif
		endif
	endforeach

	return total;
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

	// If BaseName CProp was restored, set the item's display name to match
	var base_name := GetObjProperty(newitem, "BaseName");
	if(base_name and base_name != "")
		SetName(newitem, CStr(base_name));
	endif

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
		return 0;
	endif
	if(!item.movable)
		return 0;
	endif

	// Lazy-load blacklist
	if(!_omegacache_blacklist)
		_omegacache_blacklist := ReadConfigFile(":omegacache:blacklist");
	endif

	if(_omegacache_blacklist)
		if(FindConfigElem(_omegacache_blacklist, item.objtype))
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
	// Must be inside a house
	if(!who.multi and who.cmdlevel < 4)
		return 0;
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

			// Must be inside the same house the cache belongs to
			if(who.multi and who.multi.serial != house.serial and who.cmdlevel < 4)
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
	var amt := item.amount;
	var name := GetItemDisplayName(item.objtype);
	DepositItem(df, key, item);
	DestroyItem(item);
	SendSysMessage(who, "Deposited: " + FormatNumber(amt) + " " + name);
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
		var display_name := GetItemDisplayName(thing.objtype);
		var amt := thing.amount;

		DepositItem(df, key, thing);
		DestroyItem(thing);

		if(!deposited[display_name])
			deposited[display_name] := amt;
		else
			deposited[display_name] := deposited[display_name] + amt;
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
function DoDepositTargeting(who, access)
	var df := access.df;
	var tgt;
	while(1)
		SendSysMessage(who, "Target an item or container to deposit. Press ESC to stop.");
		tgt := Target(who);
		if(!tgt)
			break;
		endif

		if(!ValidateDepositTarget(who, access, tgt))
			continue;
		endif

		if(tgt.isa(POLCLASS_CONTAINER))
			DepositFromContainer(who, df, tgt);
		else
			DepositSingleItem(who, df, tgt);
		endif
	endwhile
endfunction

// Deposit all eligible items from backpack.
function DoDepositAll(who, access)
	if(!ValidateDepositTarget(who, access, who.backpack))
		return 0;
	endif
	if(!ConfirmDepositAll(who))
		SendSysMessage(who, "Deposit cancelled.");
		return 0;
	endif
	var df := access.df;
	SendSysMessage(who, "Depositing all eligible items from your backpack...");
	var total := DepositFromContainer(who, df, who.backpack);
	if(total == 0)
		SendSysMessage(who, "No eligible items found in your backpack.");
	endif
	return total;
endfunction

const BTN_CONFIRM_DEPOSIT_ALL := 1;

function ConfirmDepositAll(who)
	var w := 350;
	var h := 150;
	var gx := (800 - w) / 2;
	var gy := (600 - h) / 2;
	var gump := GFCreateGump(gx, gy);
	GFPage(gump, 0);
	GFResizePic(gump, 0, 0, 9270, w, h);
	GFResizePic(gump, 10, 10, 9350, w - 20, h - 20);

	var line1 := "Deposit All";
	var line2 := "This will deposit all stackable items";
	var line3 := "from your backpack into the cache.";
	GFTextLine(gump, (w - len(line1) * 8) / 2, 18, TEXT_HUE_HIGHLIGHT, line1);
	GFTextLine(gump, (w - len(line2) * 8) / 2, 50, TEXT_HUE, line2);
	GFTextLine(gump, (w - len(line3) * 8) / 2, 70, TEXT_HUE, line3);

	var btn_area := 195;
	var btn_start := (w - btn_area) / 2;
	GFAddButton(gump, btn_start, h - 40, 4005, 4007, GF_CLOSE_BTN, BTN_CONFIRM_DEPOSIT_ALL);
	GFTextLine(gump, btn_start + 35, h - 40, TEXT_HUE_DEPOSIT, "Confirm");
	GFAddButton(gump, btn_start + 130, h - 40, 4017, 4019, GF_CLOSE_BTN, 0);
	GFTextLine(gump, btn_start + 165, h - 40, TEXT_HUE, "Cancel");

	var result := GFSendGump(who, gump);
	return result[0];
endfunction

// Validates that a deposit target is allowed.
// Returns 1 if valid, 0 if rejected (sends message to player).
function ValidateDepositTarget(who, access, tgt)
	// If no access struct, skip validation (backwards compatibility)
	if(!access)
		return 1;
	endif

	// Player must be in the cache's house
	if(!who.multi or who.multi.serial != access.house.serial)
		SendSysMessage(who, "You must be inside the house to deposit items.");
		return 0;
	endif

	// Target is the player's backpack — always valid if player is in house
	if(tgt.serial == who.backpack.serial)
		return 1;
	endif

	// Must be accessible and within range
	if(!Accessible(who, tgt))
		SendSysMessage(who, "You can't reach that.");
		return 0;
	endif
	// Range check — find the top-level world object (walk up containers)
	var world_obj := tgt;
	while(world_obj.container)
		world_obj := world_obj.container;
	endwhile
	if(Distance(who, world_obj) > 2)
		SendSysMessage(who, "That is too far away.");
		return 0;
	endif

	// Walk up the container chain — item must trace back to
	// player's backpack or be within the cache's house
	var check := tgt.container;
	while(check)
		if(check.serial == who.backpack.serial)
			return 1;
		endif
		check := check.container;
	endwhile

	// Not in backpack — check if item is within the house
	var in_house := 0;
	check := tgt;
	while(check)
		if(check.multi and check.multi.serial == access.house.serial)
			in_house := 1;
			break;
		endif
		check := check.container;
	endwhile

	if(!in_house)
		SendSysMessage(who, "You can only deposit items from your backpack or from within this house.");
		return 0;
	endif

	// Check if the target itself or any parent is a secured container
	check := tgt;
	while(check)
		if(check.usescript == USESCRIPTID_SECURE_CONTAINER)
			// Inside a secured container — check access
			var owner_serial := GetObjProperty(access.house, "ownerserial");
			if(who.serial != owner_serial and !IsCowner(who, access.house))
				if(!IsFriend(who, access.house, REMOVE_FROM_SECURE))
					SendSysMessage(who, "You don't have permission to remove items from that container.");
					return 0;
				endif
			endif
			break;
		endif
		check := check.container;
	endwhile

	return 1;
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

// Prompts for a destination container. Returns 0 on cancel.
function PromptDestination(who)
	SendSysMessage(who, "Target a container to receive items, or press ESC to cancel.");
	var dest := Target(who);
	if(!dest)
		SendSysMessage(who, "Cancelled.");
		return 0;
	elseif(!dest.isa(POLCLASS_CONTAINER))
		SendSysMessage(who, "That is not a container.");
		return 0;
	endif
	return dest;
endfunction

// ============================================================
// Gump Constants
// ============================================================

const TEXT_HUE := 242;
const TEXT_HUE_HIGHLIGHT := 53;
const TEXT_HUE_QTY := 55;
const TEXT_HUE_WITHDRAW := 16;  // Purple — taking items out
const TEXT_HUE_DEPOSIT := 67;   // Green-ish — putting items in

// Button IDs — start at 10 to avoid conflict with right-click close (0)
const BTN_DEPOSIT_ITEM := 10;
const BTN_DEPOSIT_ALL := 11;
const BTN_BACK := 12;
const BTN_WITHDRAW := 13;
const BTN_CATEGORY_BASE := 100;    // 100 + category_index
const BTN_TAKE_BASE := 1000;       // 1000 + item_index (target destination)
const BTN_TAKE_BP_BASE := 1500;    // 1500 + item_index (direct to backpack)
const TXT_ENTRY_BASE := 2000;      // 2000 + item_index (text entry IDs, separate from buttons)

// ============================================================
// Category Map Builder
// ============================================================

function BuildCategoryMap(df, lookup)
	var keys := df.Keys();
	var cat_map := dictionary;

	var cat_names := lookup.cat_names;
	var objtype_to_cat := lookup.objtype_to_cat;

	// Initialize empty arrays for all categories
	foreach cat_name in cat_names
		cat_map[cat_name] := array{};
	endforeach

	// Classify each stored key — clean expired leases, skip active leases
	var now := ReadGameClock();
	foreach key in keys
		if(key[1, 3] == "RL#")
			var elem := df.FindElement(key);
			if(elem)
				var expiry := CInt(elem.GetProp("expiry"));
				if(expiry and expiry < now)
					df.DeleteElement(key);
					OmegaCacheDebug("BuildCategoryMap: cleaned expired lease " + key);
				endif
			endif
			continue;
		endif
		var objtype_int := CInt(GetObjtypeFromKey(key));
		var cat_name := objtype_to_cat[objtype_int];
		if(!cat_name)
			OmegaCacheDebug("BuildCategoryMap: UNCATEGORIZED objtype=" + Hex(objtype_int) + " int=" + objtype_int + " key=" + key);
			cat_name := "Other";
			if(!cat_map["Other"])
				cat_map["Other"] := array{};
			endif
		endif
		cat_map[cat_name].append(key);
	endforeach

	return cat_map;
endfunction

// ============================================================
// Gump: Category Menu
// ============================================================

function ShowCategoryMenu(who, category_map, cat_icons, ordered_cat_names := 0, byref out_cat_order := 0, cat_icon_colors := 0)
	var gump := GFCreateGump(25, 25);

	// Page 0 — always visible (background, title, bottom buttons)
	GFPage(gump, 0);
	GFResizePic(gump, 0, 0, 9270, 490, 550);
	GFResizePic(gump, 10, 10, 9350, 470, 530);
	var title := "Omega Cache";
	GFTextLine(gump, (490 - len(title) * 8) / 2, 15, TEXT_HUE_HIGHLIGHT, title);

	// Deposit buttons (always visible)
	GFAddButton(gump, 30, 510, 4011, 4013, GF_CLOSE_BTN, BTN_DEPOSIT_ITEM);
	GFTextLine(gump, 65, 510, TEXT_HUE_DEPOSIT, "Deposit Item");
	GFAddButton(gump, 370, 510, 4011, 4013, GF_CLOSE_BTN, BTN_DEPOSIT_ALL);
	GFTextLine(gump, 405, 510, TEXT_HUE_DEPOSIT, "Deposit All");

	// Card grid: 2 columns x 5 rows = 10 per page
	var cards_per_page := 10;
	var card_w := 215;
	var card_h := 80;
	var row_start := 48;
	var row_spacing := 85;

	// Use config-defined order if available, else dictionary key order
	var cat_names;
	if(ordered_cat_names)
		cat_names := ordered_cat_names;
		if(category_map["Other"] and category_map["Other"].size() > 0)
			cat_names.append("Other");
		endif
	else
		cat_names := category_map.keys();
	endif

	// Build display-order mapping (button index -> category name)
	out_cat_order := array{};
	var total_display := 0;
	foreach cat_name in cat_names
		if(category_map[cat_name].size() > 0)
			total_display := total_display + 1;
		endif
	endforeach

	var total_pages := 1;
	if(total_display > cards_per_page)
		total_pages := CInt(Ceil(CDbl(total_display) / CDbl(cards_per_page)));
	endif

	var display_count := 0;
	var page_num := 0;

	foreach cat_name in cat_names
		var items := category_map[cat_name];
		if(!items or items.size() <= 0)
			continue;
		endif

		display_count := display_count + 1;
		out_cat_order.append(cat_name);
		var page_item := ((display_count - 1) % cards_per_page) + 1;

		// Start new gump page
		if(page_item == 1)
			page_num := page_num + 1;
			GFPage(gump, page_num);

			// Page nav — balanced centering based on which buttons are visible
			if(total_pages > 1)
				var has_prev := (page_num > 1);
				var has_next := (page_num < total_pages);
				var page_label := "Page " + page_num + "/" + total_pages;
				var label_w := len(page_label) * 8;

				var group_w := label_w;
				if(has_prev)
					group_w := group_w + 70;
				endif
				if(has_next)
					group_w := group_w + 70;
				endif

				var nav_x := (490 - group_w) / 2;

				if(has_prev)
					GFAddButton(gump, nav_x, 485, 9766, 9767, GF_PAGE_BTN, page_num - 1);
					GFTextLine(gump, nav_x + 20, 485, TEXT_HUE, "Prev");
					nav_x := nav_x + 70;
				endif

				GFTextLine(gump, nav_x, 485, TEXT_HUE, page_label);
				nav_x := nav_x + label_w + 10;

				if(has_next)
					GFAddButton(gump, nav_x, 485, 9762, 9763, GF_PAGE_BTN, page_num + 1);
					GFTextLine(gump, nav_x + 20, 485, TEXT_HUE, "Next");
				endif
			endif
		endif

		// Card position: 2 columns, 5 rows
		var col := (page_item - 1) % 2;
		var row := CInt((page_item - 1) / 2);
		var cx;
		if(col == 0)
			cx := 22;
		else
			cx := 252;
		endif
		var cy := row_start + (row * row_spacing);

		// Card panel
		GFResizePic(gump, cx, cy, 9350, card_w, card_h);

		// Category icon
		var icon := cat_icons[cat_name];
		if(!icon and cat_name == "Other")
			icon := 0x0FA7;
		endif
		if(icon)
			var icon_color := 0;
			if(cat_icon_colors)
				icon_color := CInt(cat_icon_colors[cat_name]);
			endif
			if(icon_color)
				GFTilePic(gump, cx + 8, cy + 18, icon, icon_color);
			else
				GFTilePic(gump, cx + 8, cy + 18, icon);
			endif
		endif

		// Category name and item count
		var count := items.size();
		GFTextLine(gump, cx + 70, cy + 15, TEXT_HUE, FormatCategoryName(cat_name));
		GFTextLine(gump, cx + 70, cy + 38, TEXT_HUE_QTY, CStr(count) + " type(s)");

		// Open category button
		var btn_id := BTN_CATEGORY_BASE + display_count;
		GFAddButton(gump, cx + 180, cy + 28, 4005, 4007, GF_CLOSE_BTN, btn_id);
	endforeach

	if(total_display == 0)
		GFPage(gump, 1);
		GFTextLine(gump, 170, 250, TEXT_HUE, "No items stored.");
	endif

	return GFSendGump(who, gump);
endfunction

// ============================================================
// Gump: Item List (per category)
// ============================================================

function ShowItemList(who, df, category_name, category_map, byref out_keys)
	var keys := category_map[category_name];
	if(!keys)
		keys := array{};
	endif

	var gump := GFCreateGump(25, 25);

	// Page 0 — always visible (background, title, bottom buttons)
	GFPage(gump, 0);
	GFResizePic(gump, 0, 0, 9270, 600, 520);
	GFResizePic(gump, 10, 10, 9350, 580, 500);

	var title := "Omega Cache - " + FormatCategoryName(category_name);
	GFTextLine(gump, (600 - len(title) * 8) / 2, 15, TEXT_HUE_HIGHLIGHT, title);

	// Bottom buttons (always visible): Back | Deposit | Withdraw (right-aligned)
	GFAddButton(gump, 30, 480, 4014, 4016, GF_CLOSE_BTN, BTN_BACK);
	GFTextLine(gump, 65, 480, TEXT_HUE, "Back");
	GFAddButton(gump, 240, 480, 4011, 4013, GF_CLOSE_BTN, BTN_DEPOSIT_ITEM);
	GFTextLine(gump, 275, 480, TEXT_HUE_DEPOSIT, "Deposit");
	GFAddButton(gump, 485, 480, 4017, 4019, GF_CLOSE_BTN, BTN_WITHDRAW);
	GFTextLine(gump, 520, 480, TEXT_HUE_WITHDRAW, "Withdraw");

	// Filter to only keys with available qty > 0 (lease-aware)
	var display_keys := array{};
	foreach key in keys
		if(key[1, 3] == "RL#")
			continue;
		endif
		var available := GetStoredAmount(df, key);
		if(available > 0)
			display_keys.append(key);
		endif
	endforeach
	// Sort display keys alphabetically by item name
	var name_key_pairs := array{};
	foreach key in display_keys
		var ot := CInt(GetObjtypeFromKey(key));
		var elem := df.FindElement(key);
		var base_name := elem.GetProp("cprop_BaseName");
		var name;
		if(base_name and base_name != "")
			name := Lower(CStr(base_name));
		else
			name := Lower(GetItemDisplayName(ot));
		endif
		// Trim leading/trailing spaces for consistent sort
		while(name[1] == " ")
			name := name[2, len(name)];
		endwhile
		name_key_pairs.append(name + "|||" + key);
	endforeach
	name_key_pairs.sort();
	display_keys := array{};
	foreach pair in name_key_pairs
		var sep := Find(pair, "|||", 1);
		display_keys.append(pair[sep + 3, len(pair)]);
	endforeach

	out_keys := display_keys;

	// Pagination — taller rows, fewer items per page
	var items_per_page := 11;
	var row_height := 35;
	var total_items := display_keys.size();
	var total_pages := 1;
	if(total_items > items_per_page)
		total_pages := CInt(Ceil(CDbl(total_items) / CDbl(items_per_page)));
	endif

	var page_num := 0;
	var item_index := 0;

	foreach key in display_keys
		item_index := item_index + 1;

		var page_item := ((item_index - 1) % items_per_page) + 1;
		if(page_item == 1)
			page_num := page_num + 1;
			GFPage(gump, page_num);

			// Column headers
			GFTextLine(gump, 75, 42, TEXT_HUE, "Item");
			GFTextLine(gump, 290, 42, TEXT_HUE, "Stored");
			GFTextLine(gump, 400, 42, TEXT_HUE, "Amount");
			GFTilePic(gump, 505, 32, 0x0E79);   // Target icon header
			GFTilePic(gump, 548, 28, 0x0E75);   // Backpack icon header

			// Page nav — balanced centering based on which buttons are visible
			if(total_pages > 1)
				var has_prev := (page_num > 1);
				var has_next := (page_num < total_pages);
				var page_label := "Page " + page_num + "/" + total_pages;
				var label_w := len(page_label) * 8;

				// Total width of nav group: [Prev 60] [gap 10] [label] [gap 10] [Next 60]
				var group_w := label_w;
				if(has_prev)
					group_w := group_w + 70;  // button + text + gap
				endif
				if(has_next)
					group_w := group_w + 70;
				endif

				var nav_x := (600 - group_w) / 2;

				if(has_prev)
					GFAddButton(gump, nav_x, 455, 9766, 9767, GF_PAGE_BTN, page_num - 1);
					GFTextLine(gump, nav_x + 20, 455, TEXT_HUE, "Prev");
					nav_x := nav_x + 70;
				endif

				GFTextLine(gump, nav_x, 455, TEXT_HUE, page_label);
				nav_x := nav_x + label_w + 10;

				if(has_next)
					GFAddButton(gump, nav_x, 455, 9762, 9763, GF_PAGE_BTN, page_num + 1);
					GFTextLine(gump, nav_x + 20, 455, TEXT_HUE, "Next");
				endif
			endif
		endif

		var row_y := 65 + ((page_item - 1) * row_height);

		// Row card panel (matches category card style)
		GFResizePic(gump, 18, row_y - 2, 9350, 565, 32);

		var elem := df.FindElement(key);
		var qty := GetStoredAmount(df, key);
		var objtype := CInt(elem.GetProp("objtype"));
		var desc_info := GetItemDescriptor(objtype);

		// Item name — use BaseName CProp if stored, otherwise default display name
		var base_name := elem.GetProp("cprop_BaseName");
		var item_name;
		if(base_name and base_name != "")
			item_name := CStr(base_name);
		else
			item_name := GetItemDisplayName(objtype);
		endif

		// Variant display — show distinguishing CProps (skip known internal ones)
		var propnames := elem.PropNames();
		foreach pname in propnames
			if(pname[1,6] == "cprop_")
				var cprop_name := pname[7, len(pname)];
				if(cprop_name == "SpellID" or cprop_name == "BaseName")
					continue;
				endif
				var cprop_val := elem.GetProp(pname);
				item_name := item_name + " [" + cprop_name + "=" + CStr(cprop_val) + "]";
				break;
			endif
		endforeach

		// Tile icon
		var graphic := objtype;
		if(desc_info and desc_info.Graphic)
			graphic := CInt(desc_info.Graphic);
		endif
		var color := 0;
		if(elem.GetProp("color"))
			color := CInt(elem.GetProp("color"));
		elseif(desc_info and desc_info.Color)
			color := CInt(desc_info.Color);
		endif
		GFTilePic(gump, 22, row_y, graphic, color);

		// Item name
		GFTextCrop(gump, 75, row_y + 7, 208, 20, TEXT_HUE, item_name);

		// Quantity
		GFTextLine(gump, 290, row_y + 7, TEXT_HUE_QTY, FormatNumber(qty));

		// Text entry with inset background (wide enough for 999,999,999)
		var txt_id := TXT_ENTRY_BASE + item_index;
		var btn_id := BTN_TAKE_BASE + item_index;
		GFResizePic(gump, 397, row_y + 4, 5054, 100, 22);
		GFTextEntry(gump, 400, row_y + 7, 95, 20, TEXT_HUE, "", txt_id);

		// Take to target button (blue dot)
		GFAddButton(gump, 518, row_y + 7, 2362, 2362, GF_CLOSE_BTN, btn_id);
		// Take to backpack button (golden down triangle)
		var bp_btn_id := BTN_TAKE_BP_BASE + item_index;
		GFAddButton(gump, 558, row_y + 8, 2436, 2437, GF_CLOSE_BTN, bp_btn_id);
	endforeach

	if(total_items == 0)
		GFPage(gump, 1);
		var empty_msg := "No items in this category.";
		GFTextLine(gump, (600 - len(empty_msg) * 8) / 2, 250, TEXT_HUE, empty_msg);
	endif

	return GFSendGump(who, gump);
endfunction

// ============================================================
// Main Gump Loop
// ============================================================

// Runs the interactive gump loop for browsing/depositing/withdrawing.
// Called by both the use-script (double-click) and the .cache text command.
function RunOmegaCacheGump(who, access)
	var df := access.df;
	// Block multiple gump instances
	if(CInt(GetObjProperty(who, "#omegacache_open")) > ReadGameClock())
		SendSysMessage(who, "You already have the Omega Cache open.");
		return;
	endif
	SetObjProperty(who, "#omegacache_open", ReadGameClock() + 600);

	var view := "categories";
	var category_map := dictionary;
	var current_keys := array{};
	var cat_lookup := LoadCategoryLookup();

	category_map := BuildCategoryMap(df, cat_lookup);

	var cat_display_order := array{};

	while(1)
		var result;
		if(view == "categories")
			category_map := BuildCategoryMap(df, cat_lookup);
			result := ShowCategoryMenu(who, category_map, cat_lookup.cat_icons, cat_lookup.cat_names, cat_display_order, cat_lookup.cat_icon_colors);
		else
			result := ShowItemList(who, df, view, category_map, current_keys);
		endif

		if(!result)
			break;
		endif

		var btn := CInt(result[0]);
		if(btn < BTN_DEPOSIT_ITEM or btn >= TXT_ENTRY_BASE)
			if(view == "categories")
				break;
			else
				view := "categories";
				continue;
			endif
		endif

		if(btn == BTN_DEPOSIT_ITEM)
			DoDepositTargeting(who, access);

		elseif(btn == BTN_DEPOSIT_ALL)
			DoDepositAll(who, access);

		elseif(btn == BTN_BACK)
			view := "categories";

		elseif(btn == BTN_WITHDRAW)
			if(!FindAccessibleContainer(who, array{REMOVE_FROM_SECURE}))
				SendSysMessage(who, "You don't have permission to withdraw items.");
			else
				var dest := 0;
				var withdrew_any := 0;
				var item_index := 0;
				foreach key in current_keys
					item_index := item_index + 1;
					var txt_id := TXT_ENTRY_BASE + item_index;
					var amount_str := GFExtractData(result, txt_id);
					var amount := ParseDecimal(amount_str);
					if(amount > 0)
						if(!dest)
							dest := PromptDestination(who);
							if(!dest)
								break;
							endif
						endif
						DoWithdraw(who, df, key, amount, dest);
						withdrew_any := 1;
					endif
				endforeach
				if(!withdrew_any)
					SendSysMessage(who, "Enter amounts to withdraw.");
				endif
			endif

		elseif(btn >= BTN_CATEGORY_BASE and btn < BTN_TAKE_BASE)
			var cat_index := btn - BTN_CATEGORY_BASE;
			if(cat_index >= 1 and cat_index <= cat_display_order.size())
				view := cat_display_order[cat_index];
			endif

		elseif(btn >= BTN_TAKE_BASE)
			if(!FindAccessibleContainer(who, array{REMOVE_FROM_SECURE}))
				SendSysMessage(who, "You don't have permission to withdraw items.");
			else
				var to_backpack := (btn >= BTN_TAKE_BP_BASE);
				var item_index;
				if(to_backpack)
					item_index := btn - BTN_TAKE_BP_BASE;
				else
					item_index := btn - BTN_TAKE_BASE;
				endif
				if(item_index >= 1 and item_index <= current_keys.size())
					var key := current_keys[item_index];
					var txt_id := TXT_ENTRY_BASE + item_index;
					var amount_str := GFExtractData(result, txt_id);
					var amount := ParseDecimal(amount_str);
					if(amount > 0)
						var dest;
						if(to_backpack)
							dest := who.backpack;
						else
							dest := PromptDestination(who);
						endif
						if(dest)
							DoWithdraw(who, df, key, amount, dest);
						endif
					else
						SendSysMessage(who, "Enter an amount to withdraw.");
					endif
				endif
			endif
		endif

		sleepms(200);
	endwhile

	EraseObjProperty(who, "#omegacache_open");
endfunction
