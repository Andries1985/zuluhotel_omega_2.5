// resourcemanager.inc — Centralised resource lookup and consumption
//
// Provides cache-aware resource management for crafting scripts.
// All crafting skills use these functions instead of direct SubtractAmount
// calls, enabling transparent consumption from both backpack and Omega Cache.
//
// Used by: all crafting scripts (blacksmithy, tailoring, carpentry, alchemy,
// tinkering, bowcraft, cooking, inscription, cartography).

use uo;
use math;

include "include/omegacache_utils";
include ":omegacache:omegacache";

// ============================================================
// Constants
// ============================================================

const OMEGA_CACHE := 1;
const BACKPACK    := 2;

// Gump constants for SelectMaterialFromCache
const RM_TEXT_HUE           := 242;
const RM_TEXT_HUE_HIGHLIGHT := 53;
const RM_TEXT_HUE_QTY       := 55;
const RM_BTN_SELECT_BASE    := 100;   // 100 + item_index
const RM_ITEMS_PER_PAGE     := 10;

// ============================================================
// Resource Availability
// ============================================================

// Returns total available amount of an objtype across backpack and cache.
// Pass dataFileHandle if already resolved, or 0 to auto-detect.
function GetAvailableResource(who, objtype, dataFileHandle := 0, exclude_lease_key := 0)
	objtype := CInt(objtype);

	// Enumerate backpack
	var backpack_amount := 0;
	var backpack_items := array{};
	foreach thing in EnumerateItemsInContainer(who.backpack)
		if(CInt(thing.objtype) == objtype)
			backpack_amount := backpack_amount + thing.amount;
			backpack_items.append(thing);
		endif
	endforeach

	// Resolve cache if not provided
	if(!dataFileHandle)
		var access := FindAccessibleContainer(who, array{REMOVE_FROM_SECURE});
		if(access)
			dataFileHandle := access.df;
		endif
	endif

	var cache_amount := 0;
	if(dataFileHandle)
		cache_amount := GetStoredAmountByObjtype(dataFileHandle, objtype, exclude_lease_key);
	endif

	return struct{
		total          := backpack_amount + cache_amount,
		backpack       := backpack_amount,
		cache          := cache_amount,
		backpack_items := backpack_items,
		dataFileHandle := dataFileHandle
	};
endfunction

// ============================================================
// Consumption
// ============================================================

// Consumes amount of a resource following the preferredSourceOrder in the ResourceRequest.
// Uses resourceRequest.leaseKey to exclude the caller's own lease from availability checks.
// Returns 1 on success, 0 on failure (insufficient resources).
function ConsumeResource(who, resourceRequest, amount)
	var res := GetAvailableResource(who, resourceRequest.objtype, resourceRequest.dataFileHandle, resourceRequest.leaseKey);
	if(res.total < amount)
		SendSysMessage(who, "You don't have enough resources.");
		return 0;
	endif

	var remaining := amount;
	foreach source in (resourceRequest.preferredSourceOrder)
		if(remaining <= 0)
			break;
		endif
		if(source == BACKPACK)
			remaining := ConsumeFromBackpack(res.backpack_items, remaining);
		elseif(source == OMEGA_CACHE and res.dataFileHandle)
			var consumed := ConsumeFromCache(res.dataFileHandle, resourceRequest.key, resourceRequest.objtype, remaining, resourceRequest.leaseKey);
			remaining := remaining - consumed;
		endif
	endforeach

	if(remaining > 0)
		SendSysMessage(who, "Resource consumption failed unexpectedly.");
		return 0;
	endif

	return 1;
endfunction

// Consumes from physical backpack items via SubtractAmount.
// Returns remaining amount (0 = fully consumed).
function ConsumeFromBackpack(backpack_items, amount)
	var remaining := amount;
	foreach item in backpack_items
		if(remaining <= 0)
			break;
		endif
		var take := Min(remaining, item.amount);
		if(SubtractAmount(item, take))
			remaining := remaining - take;
		endif
	endforeach
	return remaining;
endfunction

// Consumes from cache DataFile — debits qty without creating physical items.
// Lease-aware: only consumes from the unleased portion of each key.
// If key is provided, tries that key first (variant-specific consumption).
// Falls back to other keys matching the objtype prefix.
// Returns total amount consumed.
function ConsumeFromCache(dataFileHandle, key, objtype, amount, exclude_lease_key := 0)
	var remaining := amount;
	var consumed := 0;

	// Try the specific key first (if provided)
	if(key)
		var available := GetStoredAmount(dataFileHandle, key, exclude_lease_key);
		if(available > 0)
			var take := Min(remaining, available);
			var taken := WithdrawItem(dataFileHandle, key, take);
			consumed := consumed + taken;
			remaining := remaining - taken;
		endif
	endif

	// If still remaining, scan for other keys matching this objtype
	if(remaining > 0)
		var prefix := Hex(CInt(objtype)) + "|";
		var prefix_len := len(prefix);
		foreach df_key in (dataFileHandle.Keys())
			if(remaining <= 0)
				break;
			endif
			// Skip lease keys and the key we already tried
			if(df_key[1, 3] == "RL#")
				continue;
			endif
			if(key and df_key == key)
				continue;
			endif
			if(df_key[1, prefix_len] == prefix)
				var available := GetStoredAmount(dataFileHandle, df_key, exclude_lease_key);
				if(available > 0)
					var take := Min(remaining, available);
					var taken := WithdrawItem(dataFileHandle, df_key, take);
					consumed := consumed + taken;
					remaining := remaining - taken;
				endif
			endif
		endforeach
	endif

	return consumed;
endfunction

// ============================================================
// Lease Wrappers
// ============================================================

// Creates a lease on cache resources for the given ResourceRequest.
// Sets resourceRequest.leaseKey on success.
// Only leases if the request has a cache dataFileHandle.
// Returns 1 on success, 0 if no lease needed/possible.
function LeaseResource(who, byref resourceRequest, amount, ttl := LEASE_DEFAULT_TTL)
	if(!resourceRequest.dataFileHandle)
		resourceRequest.leaseKey := 0;
		return 0;
	endif
	// Only lease if we have a specific cache key to lease against.
	// Backpack-first requests (MakeBackpackRequest) have key=0 — no lease needed
	// since physical items are consumed directly via SubtractAmount.
	var item_key := resourceRequest.key;
	if(!item_key)
		resourceRequest.leaseKey := 0;
		return 0;
	endif
	var lease_key := CreateLease(resourceRequest.dataFileHandle, item_key, who, amount, ttl);
	resourceRequest.leaseKey := lease_key;
	return lease_key;
endfunction

// Extends the lease on the ResourceRequest. Returns 1 on success, 0 if lease expired/deleted.
// Returns 1 (success) when no lease exists — backpack-only path doesn't need a lease.
function ExtendResourceLease(resourceRequest, ttl := LEASE_DEFAULT_TTL)
	if(!resourceRequest.leaseKey)
		return 1;  // No lease to extend — not an error
	endif
	if(!resourceRequest.dataFileHandle)
		return 1;
	endif
	return ExtendLease(resourceRequest.dataFileHandle, resourceRequest.leaseKey, ttl);
endfunction

// Releases the lease on the ResourceRequest. Safe to call if no lease exists.
function ReleaseResourceLease(byref resourceRequest)
	OmegaCacheDebug("ReleaseResourceLease: leaseKey=" + resourceRequest.leaseKey + " df=" + resourceRequest.dataFileHandle);
	if(!resourceRequest.dataFileHandle or !resourceRequest.leaseKey)
		OmegaCacheDebug("ReleaseResourceLease: skipped (no df or no leaseKey)");
		return;
	endif
	ReleaseLease(resourceRequest.dataFileHandle, resourceRequest.leaseKey);
	resourceRequest.leaseKey := 0;
endfunction

// ============================================================
// ResourceRequest Builders
// ============================================================

// Builds a ResourceRequest from a targeted physical item (backpack-first).
// Checks for a nearby cache for fallback consumption.
function MakeBackpackRequest(who, item)
	var dataFileHandle := 0;
	var access := FindAccessibleContainer(who, array{REMOVE_FROM_SECURE});
	if(access)
		dataFileHandle := access.df;
	endif

	return struct{
		objtype              := CInt(item.objtype),
		key                  := 0,
		color                := item.color,
		quality              := CDbl(item.quality),
		preferredSourceOrder := array{ BACKPACK, OMEGA_CACHE },
		dataFileHandle       := dataFileHandle,
		leaseKey             := 0
	};
endfunction

// Opens a material selection gump from the cache. Player picks a variant.
// Reserves the cache container to prevent concurrent modification.
// Returns a ResourceRequest with cache-first priority, or 0 on cancel.
function SelectMaterialFromCache(who)
	var access := FindAccessibleContainer(who, array{REMOVE_FROM_SECURE});
	if(!access)
		SendSysMessage(who, "You must be near an Omega Cache Container you have access to.");
		return 0;
	endif

	var df := access.df;
	var all_keys := df.Keys();
	if(!all_keys or all_keys.size() == 0)
		SendSysMessage(who, "The Omega Cache is empty.");
		return 0;
	endif

	// Build list of keys with available qty > 0 (skip leases, lease-aware)
	var valid_keys := array{};
	foreach key in all_keys
		if(key[1, 3] == "RL#")
			continue;
		endif
		var available := GetStoredAmount(df, key);
		if(available > 0)
			valid_keys.append(key);
		endif
	endforeach

	if(valid_keys.size() == 0)
		SendSysMessage(who, "The Omega Cache is empty.");
		return 0;
	endif

	// Sort by item name for consistent display
	var sort_list := array{};
	foreach key in valid_keys
		var ot := CInt(GetObjtypeFromKey(key));
		var name := Lower(GetItemDisplayName(ot));
		sort_list.append(struct{ sort_name := name, sort_key := key });
	endforeach
	sort_list.sort(1);
	valid_keys := array{};
	foreach entry in sort_list
		valid_keys.append(entry.sort_key);
	endforeach

	// Build selection gump
	var gump := GFCreateGump(25, 25);

	GFPage(gump, 0);
	GFResizePic(gump, 0, 0, 9270, 500, 520);
	GFResizePic(gump, 10, 10, 9350, 480, 500);

	var title := "Select Material";
	GFTextLine(gump, (500 - len(title) * 8) / 2, 15, RM_TEXT_HUE_HIGHLIGHT, title);

	// Column headers on page 0
	GFTextLine(gump, 68, 42, RM_TEXT_HUE, "Material");
	GFTextLine(gump, 340, 42, RM_TEXT_HUE, "Available");
	GFTextLine(gump, 440, 42, RM_TEXT_HUE, "Select");

	var total_items := valid_keys.size();
	var total_pages := 1;
	if(total_items > RM_ITEMS_PER_PAGE)
		total_pages := CInt(Ceil(CDbl(total_items) / CDbl(RM_ITEMS_PER_PAGE)));
	endif

	var page_num := 0;
	var item_index := 0;
	var row_height := 35;

	foreach key in valid_keys
		item_index := item_index + 1;
		var page_item := ((item_index - 1) % RM_ITEMS_PER_PAGE) + 1;

		if(page_item == 1)
			page_num := page_num + 1;
			GFPage(gump, page_num);

			// Page navigation
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
				var nav_x := (500 - group_w) / 2;
				if(has_prev)
					GFAddButton(gump, nav_x, 485, 9766, 9767, GF_PAGE_BTN, page_num - 1);
					GFTextLine(gump, nav_x + 20, 485, RM_TEXT_HUE, "Prev");
					nav_x := nav_x + 70;
				endif
				GFTextLine(gump, nav_x, 485, RM_TEXT_HUE, page_label);
				nav_x := nav_x + label_w + 10;
				if(has_next)
					GFAddButton(gump, nav_x, 485, 9762, 9763, GF_PAGE_BTN, page_num + 1);
					GFTextLine(gump, nav_x + 20, 485, RM_TEXT_HUE, "Next");
				endif
			endif
		endif

		var row_y := 65 + ((page_item - 1) * row_height);

		// Row background
		GFResizePic(gump, 18, row_y - 2, 9350, 465, 32);

		var elem := df.FindElement(key);
		var qty := GetStoredAmount(df, key);
		var objtype := CInt(elem.GetProp("objtype"));
		var desc_info := GetItemDescriptor(objtype);

		// Item name with variant indicator
		var item_name := GetItemDisplayName(objtype);
		var propnames := elem.PropNames();
		foreach pname in propnames
			if(pname[1,6] == "cprop_")
				var cprop_name := pname[7, len(pname)];
				var cprop_val := elem.GetProp(pname);
				item_name := item_name + " [" + cprop_name + "=" + CStr(cprop_val) + "]";
				break;
			endif
		endforeach

		// Tile icon with color
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

		// Name and quantity
		GFTextCrop(gump, 68, row_y + 7, 265, 20, RM_TEXT_HUE, item_name);
		GFTextLine(gump, 340, row_y + 7, RM_TEXT_HUE_QTY, FormatNumber(qty));

		// Select button
		var btn_id := RM_BTN_SELECT_BASE + item_index;
		GFAddButton(gump, 445, row_y + 7, 4005, 4007, GF_CLOSE_BTN, btn_id);
	endforeach

	if(total_items == 0)
		GFPage(gump, 1);
		GFTextLine(gump, 170, 250, RM_TEXT_HUE, "No materials available.");
	endif

	var result := GFSendGump(who, gump);
	if(!result)
		return 0;
	endif

	var btn := CInt(result[0]);
	if(btn < RM_BTN_SELECT_BASE)
		return 0;  // Closed or cancelled
	endif

	var selected_index := btn - RM_BTN_SELECT_BASE;
	if(selected_index < 1 or selected_index > valid_keys.size())
		return 0;
	endif

	var selected_key := valid_keys[selected_index];
	var selected_elem := df.FindElement(selected_key);
	if(!selected_elem)
		SendSysMessage(who, "That item is no longer available.");
		return 0;
	endif

	var selected_objtype := CInt(selected_elem.GetProp("objtype"));
	var selected_color := 0;
	if(selected_elem.GetProp("color"))
		selected_color := CInt(selected_elem.GetProp("color"));
	else
		// Fall back to default color from item descriptor
		var desc_info := GetItemDescriptor(selected_objtype);
		if(desc_info and desc_info.Color)
			selected_color := CInt(desc_info.Color);
		endif
	endif
	var selected_quality := 0;
	if(selected_elem.GetProp("quality"))
		selected_quality := CDbl(selected_elem.GetProp("quality"));
	endif

	return struct{
		objtype              := selected_objtype,
		key                  := selected_key,
		color                := selected_color,
		quality              := selected_quality,
		preferredSourceOrder := array{ OMEGA_CACHE, BACKPACK },
		dataFileHandle       := df,
		leaseKey             := 0
	};
endfunction
