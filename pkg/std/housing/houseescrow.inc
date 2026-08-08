// houseescrow.inc -- Staff "Demolish and Escrow" core logic.
//
// Dedicated, independent escrow storage/claim system for house-demolish
// contents (secures, redeedable furniture, Omega Cache contents, everything
// else, and the resulting house deed). Modeled on the player-merchant escrow
// pattern (pkg/systems/playervendor/include/escrow.inc) but deliberately
// self-contained -- no cross-package include into pkg/systems/playervendor
// for storage/pack logic, so the two systems can't drift into each other.

use os;
use uo;
use storage;
use datafile;
use file;
use basic;
use polsys;

include "include/objtype";
include "include/myutil";
include "include/constants/usescriptids";
include "include/omegacache_utils";
include "utility";

const HOUSE_ESCROW_STORAGE_NAME := "House Demolish Escrow";
const HOUSE_ESCROW_DF_PREFIX := "houseescrow_";
const HOUSE_ESCROW_PACK_OBJTYPE := UOBJ_BACKPACK;
const HOUSE_ESCROW_PACK_MAX_ROOT_ITEMS := 225;
const HOUSE_ESCROW_PACK_MAX_WEIGHT := 60000;
const HOUSE_ESCROW_LOG_FILE := "::log/houseescrow.log";

const HOUSE_ENTRY_ID_IDX := 1;
const HOUSE_ENTRY_OWNERSERIAL_IDX := 2;
const HOUSE_ENTRY_ESCROWNAME_IDX := 3;
const HOUSE_ENTRY_LABEL_IDX := 4;
const HOUSE_ENTRY_HOUSESERIAL_IDX := 5;
const HOUSE_ENTRY_CREATED_IDX := 6;
const HOUSE_ENTRY_OWNERNAME_IDX := 7;

////////////////////////////////////////////////////////////////////////////
// Logging -- persistent troubleshooting record, independent of any live
// console debug flags.
////////////////////////////////////////////////////////////////////////////
function HouseEscrowLog(msg)
	LogToFile(HOUSE_ESCROW_LOG_FILE, msg, LOG_DATETIME);
endfunction

// Reuses the shard-global "gmpages" GlobalProperty queue that
// playermerchant.src's QueueGMPageReport already writes to and the
// counselor-level .page tooling already reads -- a plain GlobalProperty, not
// owned by any one package, so replicating the small append here is correct
// rather than adding a cross-package include just for this.
function QueueHouseEscrowGMPage(subject_serial, message)
	var gmpages := GetGlobalProperty("gmpages");
	if(!gmpages)
		gmpages := {};
	endif
	gmpages.append(subject_serial);
	gmpages.append("[HouseEscrow] " + message);
	SetGlobalProperty("gmpages", gmpages);
endfunction

////////////////////////////////////////////////////////////////////////////
// Storage area / per-owner DataFile index
////////////////////////////////////////////////////////////////////////////
function OpenHouseEscrowStorage()
	var area := FindStorageArea(HOUSE_ESCROW_STORAGE_NAME);
	if(!area)
		area := CreateStorageArea(HOUSE_ESCROW_STORAGE_NAME);
	endif
	return area;
endfunction

// Takes a raw serial, not a character object -- this must keep working even
// when the owning character can no longer be resolved, so an escrow entry can
// still be filed and found later instead of silently dropped.
//
// Package-scoped (":housing:...") is mandatory here, not cosmetic -- a bare
// filespec apparently resolves relative to whichever package the calling
// script was compiled into, so pkg/std/housing/houseescrow.inc (this file)
// and scripts/textcmd/player/houseescrow.src (the claim command, a
// different package) were silently writing to/reading from two different
// datafiles despite using the identical bare name string. Confirmed via
// this repo's only other cross-file DataFile usage (pkg/opt/townstones'
// ":townstone:townstone_data", shared verbatim across multiple files) --
// nothing in this codebase successfully shares a DataFile across files
// without an explicit package prefix.
function BuildHouseEscrowDatafileNameForSerial(owner_serial)
	if(!owner_serial)
		return "";
	endif
	return ":housing:" + HOUSE_ESCROW_DF_PREFIX + CStr(owner_serial);
endfunction

// Percent-style escaping for the free-text fields (house label / owner name)
// embedded in the "|"/"^"-delimited entry format. Escape "%" first on encode,
// restore it last on decode.
function HouseEscrowEscapeField(s)
	s := CStr(s);
	s := StrReplace(s, "%", "%25");
	s := StrReplace(s, "|", "%7C");
	s := StrReplace(s, "^", "%5E");
	return s;
endfunction

function HouseEscrowUnescapeField(s)
	s := CStr(s);
	s := StrReplace(s, "%7C", "|");
	s := StrReplace(s, "%5E", "^");
	s := StrReplace(s, "%25", "%");
	return s;
endfunction

function HouseEntryId(entry)
	if(len(entry) >= HOUSE_ENTRY_ID_IDX)
		return entry[HOUSE_ENTRY_ID_IDX];
	endif
	return "";
endfunction

function HouseEntryOwnerSerial(entry)
	if(len(entry) >= HOUSE_ENTRY_OWNERSERIAL_IDX)
		return CInt(entry[HOUSE_ENTRY_OWNERSERIAL_IDX]);
	endif
	return 0;
endfunction

function HouseEntryEscrowName(entry)
	if(len(entry) >= HOUSE_ENTRY_ESCROWNAME_IDX)
		return entry[HOUSE_ENTRY_ESCROWNAME_IDX];
	endif
	return "";
endfunction

function HouseEntryLabel(entry)
	if(len(entry) >= HOUSE_ENTRY_LABEL_IDX)
		return entry[HOUSE_ENTRY_LABEL_IDX];
	endif
	return "House";
endfunction

function HouseEntryHouseSerial(entry)
	if(len(entry) >= HOUSE_ENTRY_HOUSESERIAL_IDX)
		return CInt(entry[HOUSE_ENTRY_HOUSESERIAL_IDX]);
	endif
	return 0;
endfunction

function HouseEntryCreated(entry)
	if(len(entry) >= HOUSE_ENTRY_CREATED_IDX)
		return CInt(entry[HOUSE_ENTRY_CREATED_IDX]);
	endif
	return 0;
endfunction

function HouseEntryOwnerName(entry)
	if(len(entry) >= HOUSE_ENTRY_OWNERNAME_IDX and entry[HOUSE_ENTRY_OWNERNAME_IDX])
		return entry[HOUSE_ENTRY_OWNERNAME_IDX];
	endif
	return "Unknown Owner";
endfunction

function EncodeHouseEscrowEntryFields(id, ownerserial, escrow_name, house_label, house_serial, created, owner_name)
	return CStr(id) + "|" + CStr(ownerserial) + "|" + CStr(escrow_name) + "|" + HouseEscrowEscapeField(house_label) + "|" + CStr(house_serial) + "|" + CStr(created) + "|" + HouseEscrowEscapeField(owner_name);
endfunction

function EncodeHouseEscrowEntries(entries)
	var encoded := "";
	foreach entry in entries
		if(encoded)
			encoded := encoded + "^";
		endif
		encoded := encoded + EncodeHouseEscrowEntryFields(HouseEntryId(entry), HouseEntryOwnerSerial(entry), HouseEntryEscrowName(entry), HouseEntryLabel(entry), HouseEntryHouseSerial(entry), HouseEntryCreated(entry), HouseEntryOwnerName(entry));
	endforeach
	return encoded;
endfunction

function DecodeHouseEscrowEntries(entries_str)
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
					HouseEscrowUnescapeField(fields[4]),
					CInt(fields[5]),
					CInt(fields[6]),
					HouseEscrowUnescapeField(fields[7])
				});
			endif
		endif
	endforeach

	return entries;
endfunction

// Read-modify-write of the per-owner datafile's per-house element. No
// sleep/wait_for_event happens anywhere between OpenDataFile and
// UnloadDataFile in this function, which keeps it atomic under POL's
// cooperative script scheduler -- do not add one here without re-adding real
// synchronization.
function AppendHouseEscrowIndex(ownerserial, house_serial, escrow_name, house_label, owner_name)
	var df_name := BuildHouseEscrowDatafileNameForSerial(ownerserial);
	if(!df_name)
		HouseEscrowLog("[ERROR] AppendHouseEscrowIndex: failed to build datafile name for owner=" + ownerserial + " house=" + house_serial);
		return "";
	endif

	var df := OpenDataFile(df_name);
	if(!df)
		CreateDataFile(df_name);
		df := OpenDataFile(df_name);
		if(!df)
			HouseEscrowLog("[ERROR] AppendHouseEscrowIndex: failed to open/create datafile " + df_name);
			return "";
		endif
	endif

	var house_key := CStr(house_serial);
	var house_elem := df.FindElement(house_key);
	if(!house_elem)
		house_elem := df.CreateElement(house_key);
	endif
	if(!house_elem)
		HouseEscrowLog("[ERROR] AppendHouseEscrowIndex: failed to create/find element " + house_key + " in " + df_name);
		UnloadDataFile(df_name);
		return "";
	endif

	var entry_id := ownerserial + "-" + ReadGameClock() + "-" + Random(100000);

	var entries := DecodeHouseEscrowEntries(house_elem.GetProp("entries"));
	entries.append({
		entry_id,
		CInt(ownerserial),
		escrow_name,
		house_label,
		CInt(house_serial),
		ReadGameClock(),
		owner_name
	});
	house_elem.SetProp("entries", EncodeHouseEscrowEntries(entries));
	UnloadDataFile(df_name);

	HouseEscrowLog("Indexed entry_id=" + entry_id + " owner=" + ownerserial + " (" + owner_name + ") house=" + house_serial + " (" + house_label + ") escrow_name=" + escrow_name + " df=" + df_name);
	return entry_id;
endfunction

// Creates the escrow root and moves the pack into storage, but does NOT
// index it -- returns the escrow_name string on success ("" on failure) so
// the caller can batch-index every pack for a house in one shot via
// AppendHouseEscrowIndexBatch instead of one datafile
// open/decode/append/encode/save cycle per pack. A house with hundreds of
// packs (a heavily-loaded fortress, many full secures) doing that per-pack
// was O(N^2) in the entries list size -- 346 packs in one real test.
function SaveHouseEscrowPack(hpack, ownerserial, house_serial, house_label)
	var area := OpenHouseEscrowStorage();
	if(!area)
		HouseEscrowLog("[ERROR] SaveHouseEscrowPack: escrow storage area unavailable for house=" + house_serial + " owner=" + ownerserial);
		return "";
	endif

	var escrow_name := "HouseEscrow " + ownerserial + " " + house_serial + " " + ReadGameClock() + " " + Random(99999);
	var escrow_root := CreateRootItemInStorageArea(area, escrow_name, UOBJ_BACKPACK);
	if(!escrow_root)
		HouseEscrowLog("[ERROR] SaveHouseEscrowPack: failed to create escrow root " + escrow_name + " for house=" + house_serial);
		return "";
	endif

	escrow_root.name := house_label + " Escrow";
	SetObjProperty(escrow_root, "ownerserial", CInt(ownerserial));
	SetObjProperty(escrow_root, "house_serial", CInt(house_serial));
	SetObjProperty(escrow_root, "house_label", house_label);
	SetObjProperty(escrow_root, "created", ReadGameClock());

	if(!MoveItemToContainer(hpack, escrow_root))
		HouseEscrowLog("[ERROR] SaveHouseEscrowPack: failed to move pack into escrow root " + escrow_name + " for house=" + house_serial);
		DestroyRootItemInStorageArea(area, escrow_name);
		return "";
	endif

	HouseEscrowLog("Saved pack to escrow root " + escrow_name + " for house=" + house_serial + " owner=" + ownerserial);
	return escrow_name;
endfunction

// Indexes every saved pack for one house in a single
// open/decode/append-all/encode/save cycle -- see the comment on
// SaveHouseEscrowPack above for why this replaced indexing one pack at a
// time.
function AppendHouseEscrowIndexBatch(ownerserial, house_serial, house_label, owner_name, byref escrow_names)
	if(!len(escrow_names))
		return;
	endif

	var df_name := BuildHouseEscrowDatafileNameForSerial(ownerserial);
	if(!df_name)
		HouseEscrowLog("[ERROR] AppendHouseEscrowIndexBatch: failed to build datafile name for owner=" + ownerserial + " house=" + house_serial);
		return;
	endif

	var df := OpenDataFile(df_name);
	if(!df)
		CreateDataFile(df_name);
		df := OpenDataFile(df_name);
		if(!df)
			HouseEscrowLog("[ERROR] AppendHouseEscrowIndexBatch: failed to open/create datafile " + df_name);
			return;
		endif
	endif

	var house_key := CStr(house_serial);
	var house_elem := df.FindElement(house_key);
	if(!house_elem)
		house_elem := df.CreateElement(house_key);
	endif
	if(!house_elem)
		HouseEscrowLog("[ERROR] AppendHouseEscrowIndexBatch: failed to create/find element " + house_key + " in " + df_name);
		UnloadDataFile(df_name);
		return;
	endif

	var entries := DecodeHouseEscrowEntries(house_elem.GetProp("entries"));
	foreach escrow_name in escrow_names
		var entry_id := ownerserial + "-" + ReadGameClock() + "-" + Random(100000);
		entries.append({
			entry_id,
			CInt(ownerserial),
			escrow_name,
			house_label,
			CInt(house_serial),
			ReadGameClock(),
			owner_name
		});
	endforeach
	house_elem.SetProp("entries", EncodeHouseEscrowEntries(entries));
	UnloadDataFile(df_name);

	HouseEscrowLog("AppendHouseEscrowIndexBatch: owner=" + ownerserial + " (" + owner_name + ") house=" + house_serial + " (" + house_label + ") indexed " + len(escrow_names) + " pack(s), df=" + df_name + ", entries_in_element_now=" + len(entries));
endfunction

////////////////////////////////////////////////////////////////////////////
// Weight-safe pack builder -- independent copy of playermerchant.src's
// CreatePayoutPack/CanPlaceInPayoutPack/MoveItemToPayoutPacks. No ground-drop
// fallback: packs can always be spun up fresh.
////////////////////////////////////////////////////////////////////////////
function CreateHouseEscrowPack(label, realm)
	// Staging packs must be created in the SAME realm as the house being
	// swept -- MoveItemToContainer moving a world item into a container that
	// lives in a different realm silently fails (returns 0, no exception),
	// which is exactly what caused mass "could not be moved into escrow"
	// failures across every item type on a house outside the default
	// "britannia" realm (this shard has 5: britannia, britannia_alt,
	// ilshenar, malas, tokuno -- see config/gorealm.cfg). Once a pack is
	// later moved into the storage area (SaveHouseEscrowPack), realm stops
	// mattering -- storage-area roots aren't world-realm-bound -- so only
	// this initial staging creation needs to match.
	var hpack := CreateItemAtLocation(5288, 1176, 0, HOUSE_ESCROW_PACK_OBJTYPE, 1, realm);
	if(hpack and !hpack.errortext)
		hpack.name := label + " escrow";
		return hpack;
	endif
	return 0;
endfunction

function CanPlaceInHouseEscrowPack(hpack, item)
	var roots := ListRootItemsInContainer(hpack);
	if(len(roots) >= HOUSE_ESCROW_PACK_MAX_ROOT_ITEMS)
		return 0;
	endif
	var estweight := CInt(hpack.weight) + CInt(item.weight);
	if(estweight > HOUSE_ESCROW_PACK_MAX_WEIGHT)
		return 0;
	endif
	return 1;
endfunction

function MoveItemToHouseEscrowPacks(byref packs, byref item, label, realm, source := "unknown")
	// Defensive guard against a stale/bad item reference (seen in practice
	// from live-iteration hazards upstream, e.g. a container listing walked
	// while being drained) -- without this, item.movable below throws and
	// the failure gets logged as an unhelpful "<uninitialized object>"
	// instead of a clear, greppable message. `source` identifies which
	// call site handed us the bad reference, since that's the one piece
	// of information the item itself can no longer tell us.
	if(!item or item.errortext)
		HouseEscrowLog("[ERROR] MoveItemToHouseEscrowPacks: received an invalid/stale item reference (source=" + source + ") for label=" + label + " -- skipped, nothing to move.");
		return 0;
	endif

	// Locked-down/secured house items have movable=0 -- MoveItemToContainer
	// silently fails (returns 0, no error) on a non-movable item without
	// this, which is exactly what left hundreds of empty packs behind: every
	// locked item failed to move into every existing pack, fell through to
	// a freshly created pack, failed there too for the same reason, and got
	// left behind as an empty, still-indexed escrow entry. Same pattern
	// sign.src's own "recover lost items" flow already uses.
	if(item.movable == 0)
		item.movable := 1;
	endif

	for i := 1 to len(packs)
		if(CanPlaceInHouseEscrowPack(packs[i], item) and MoveItemToContainer(item, packs[i]))
			return 1;
		endif
	endfor

	var new_pack := CreateHouseEscrowPack(label, realm);
	if(!new_pack)
		HouseEscrowLog("[ERROR] MoveItemToHouseEscrowPacks: failed to create a new escrow pack for label=" + label);
		return 0;
	endif

	if(MoveItemToContainer(item, new_pack))
		packs.append(new_pack);
		return 1;
	endif

	// Move failed even into a brand-new, empty pack -- don't leave that pack
	// behind as a phantom empty escrow entry (it was never added to `packs`
	// above, so it won't get indexed), destroy it, and log loudly with
	// enough detail to actually chase down why this specific item won't
	// move (item.movable is already cleared above, so this is something
	// else -- e.g. a cursed/non-container-safe item, or a genuine engine
	// container-capacity limit).
	HouseEscrowLog("[ERROR] MoveItemToHouseEscrowPacks: failed to move item " + item.serial + " (objtype=" + Hex(item.objtype) + ", name='" + item.name + "', realm=" + item.realm + ", movable=" + item.movable + ") into a freshly created pack (realm=" + new_pack.realm + ") for label=" + label + " -- item left un-escrowed");
	DestroyItem(new_pack);
	return 0;
endfunction

// Variant for Omega Cache recreation, where the weight of the item(s) about
// to be created is known before they physically exist.
function CanFitWeightInHouseEscrowPack(hpack, added_weight)
	var roots := ListRootItemsInContainer(hpack);
	if(len(roots) >= HOUSE_ESCROW_PACK_MAX_ROOT_ITEMS)
		return 0;
	endif
	var estweight := CDbl(hpack.weight) + CDbl(added_weight);
	if(estweight > HOUSE_ESCROW_PACK_MAX_WEIGHT)
		return 0;
	endif
	return 1;
endfunction

function FindOrCreateHouseEscrowPackForWeight(byref packs, added_weight, label, realm)
	for i := 1 to len(packs)
		if(CanFitWeightInHouseEscrowPack(packs[i], added_weight))
			return packs[i];
		endif
	endfor

	var new_pack := CreateHouseEscrowPack(label, realm);
	if(!new_pack)
		return 0;
	endif
	packs.append(new_pack);
	return new_pack;
endfunction

////////////////////////////////////////////////////////////////////////////
// Bucket helpers
////////////////////////////////////////////////////////////////////////////
// Returns the count of child items that could NOT be moved into escrow
// (left behind in the secure, which is destroyed regardless -- see
// MoveItemToHouseEscrowPacks for why a move can fail and what gets logged).
function DrainSecureContainerToPacks(secure_item, byref packs, label, realm)
	// MoveItemToContainer invokes the source container's CanRemoveScript
	// (scripts/control/can_remove_container.src) same as a player drag --
	// confirmed the hard way: every single child of a real secure was
	// failing to move regardless of movable/realm, while everything else
	// escrowed fine. That script's SecureRemove() gates removal behind
	// IsCowner(who, sign)/friend permissions keyed off the container's
	// "houseserial" cprop, and our headless move has no live player "who"
	// to satisfy it -- it fails closed for every item, every time. Erase
	// the tag before draining so CanRemoveScript's houseserial check comes
	// up falsy and just allows it, same trick the Omega Cache branch above
	// already uses (EraseObjProperty before DestroyItem) to sidestep this
	// exact permission gate. Safe here since secure_item is destroyed
	// outright at the end of this function regardless.
	EraseObjProperty(secure_item, "houseserial");

	// Snapshot children into a local array BEFORE moving any of them out --
	// iterating ListRootItemsInContainer(secure_item) live while removing
	// from that same container mid-loop is exactly the live-iteration
	// hazard already known elsewhere in this codebase (escrow.src's
	// DestroyContainerAndContents snapshots first for the same reason). A
	// handful of items always at the tail of a large drain reads back as
	// <uninitialized object> without this -- consistent with later entries
	// in a live container listing going stale as earlier ones are removed.
	var children_snapshot := {};
	var bad_at_collection := 0;
	foreach child in ListRootItemsInContainer(secure_item)
		if(!child or child.errortext)
			bad_at_collection := bad_at_collection + 1;
			continue;
		endif
		children_snapshot.append(child);
	endforeach
	if(bad_at_collection)
		HouseEscrowLog("[ERROR] DrainSecureContainerToPacks: ListRootItemsInContainer(" + secure_item.serial + ") returned " + bad_at_collection + " already-bad entry(ies) at collection time (before any moves happened) -- likely pre-existing corrupted item data on this house, not something this sweep caused.");
	endif

	var yield_counter := 0;
	var failed := 0;
	foreach child in children_snapshot
		if(!MoveItemToHouseEscrowPacks(packs, child, label, realm, "secure_child"))
			failed := failed + 1;
		endif

		// yield check -- see the SCRIPTOPT_NO_RUNAWAY comment on DemolishAndEscrowHouse.
		yield_counter := yield_counter + 1;
		if(yield_counter >= 25)
			yield_counter := 0;
			SleepMS(1);
		endif
	endforeach
	DestroyItem(secure_item);
	return failed;
endfunction

// Trimmed duplicate of pkg/opt/omegacache/omegacache.inc's RecreateItem() --
// Omega Cache deposits are DataFile records, not physical items, so draining
// them into escrow means reconstructing real items from the stored
// properties. omegacache.inc itself is heavy (pulls in the full gump
// library) and pkg/std/housing deliberately doesn't depend on it; this is a
// small, deliberate duplication of just the reconstruction logic.
function RecreateOmegaCacheItemForEscrow(container, elem, amount)
	var objtype := CInt(elem.GetProp("objtype"));
	var newitem := CreateItemInContainer(container, objtype, amount);
	if(!newitem or newitem.errortext)
		return 0;
	endif

	var propnames := elem.PropNames();
	foreach pname in propnames
		var val := elem.GetProp(pname);

		if(pname == "qty" or pname == "objtype" or pname == "weight")
			continue;
		endif

		if(pname == "color")
			newitem.color := CInt(val);
		elseif(pname == "graphic")
			newitem.graphic := CInt(val);
		elseif(pname == "quality")
			newitem.quality := CDbl(val);
		elseif(pname == "usescript")
			newitem.usescript := CStr(val);
		elseif(pname == "equipscript")
			newitem.equipscript := CStr(val);
		elseif(pname == "snoopscript")
			newitem.snoopscript := CStr(val);
		elseif(pname == "newbie")
			newitem.newbie := CInt(val);
		elseif(pname == "insured")
			newitem.insured := CInt(val);
		elseif(pname == "cursed")
			newitem.cursed := CInt(val);
		elseif(pname == "weight_multiplier_mod")
			newitem.weight_multiplier_mod := CDbl(val);
		elseif(pname[1,6] == "cprop_")
			var cprop_name := pname[7, len(pname)];
			SetObjProperty(newitem, cprop_name, val);
		endif
	endforeach

	var base_name := GetObjProperty(newitem, "BaseName");
	if(base_name and base_name != "")
		SetName(newitem, CStr(base_name));
	endif

	return newitem;
endfunction

function DrainOmegaCacheStoreToPacks(house_serial, byref packs, label, realm)
	var df := OpenOmegaCacheStore(house_serial);
	if(!df)
		return;
	endif

	HouseEscrowLog("DrainOmegaCacheStoreToPacks: house=" + house_serial + " keys=" + len(df.Keys()));
	var yield_counter := 0;

	foreach key in (df.Keys())
		if(CStr(key)[1,3] == "RL#")
			continue;
		endif

		var elem := df.FindElement(key);
		if(!elem)
			continue;
		endif

		var qty := CInt(elem.GetProp("qty"));
		if(qty <= 0)
			continue;
		endif

		var objtype := CInt(elem.GetProp("objtype"));
		var item_weight := CDbl(elem.GetProp("weight"));
		var desc_info := GetItemDescriptor(objtype);
		var stack_limit := 60000;
		if(desc_info and desc_info.StackLimit)
			stack_limit := CInt(desc_info.StackLimit);
		endif

		var remaining := qty;
		while(remaining > 0)
			var this_stack := remaining;
			if(this_stack > stack_limit)
				this_stack := stack_limit;
			endif

			// A stack's total weight must fit inside one (possibly brand new,
			// empty) pack -- subdivide further by weight if needed so we never
			// ask for more than a fresh pack could ever hold.
			if(item_weight > 0)
				var max_by_pack_weight := CInt(HOUSE_ESCROW_PACK_MAX_WEIGHT / item_weight);
				if(max_by_pack_weight < 1)
					max_by_pack_weight := 1;
				endif
				if(this_stack > max_by_pack_weight)
					this_stack := max_by_pack_weight;
				endif
			endif

			var hpack := FindOrCreateHouseEscrowPackForWeight(packs, item_weight * this_stack, label, realm);
			if(!hpack)
				HouseEscrowLog("[ERROR] DrainOmegaCacheStoreToPacks: could not obtain a pack for house=" + house_serial + " objtype=" + Hex(objtype) + " qty=" + this_stack);
				break;
			endif

			var newitem := RecreateOmegaCacheItemForEscrow(hpack, elem, this_stack);
			if(!newitem)
				HouseEscrowLog("[ERROR] DrainOmegaCacheStoreToPacks: failed to recreate objtype=" + Hex(objtype) + " qty=" + this_stack + " for house=" + house_serial);
				break;
			endif

			remaining := remaining - this_stack;

			// yield check -- see the SCRIPTOPT_NO_RUNAWAY comment on DemolishAndEscrowHouse.
			yield_counter := yield_counter + 1;
			if(yield_counter >= 10)
				yield_counter := 0;
				SleepMS(1);
			endif
		endwhile
	endforeach

	CloseOmegaCacheStore(house_serial);
endfunction

// Handles one item's redeed-group membership if it has one. Returns 1 if the
// item was a redeed-group member (handled and its whole group marked
// processed), 0 if it isn't part of a redeed group at all (falls through to
// the generic bucket).
function TryBulkRedeedItem(item, byref processed, byref packs, label, realm)
	var from_deed := GetObjProperty(item, "FromDeed");
	var created_serials := GetObjProperty(item, "ItemsCreatedSerials");
	if(!from_deed or !created_serials)
		return 0;
	endif

	var group_items := {};
	foreach s in created_serials
		processed[CInt(s)] := 1;
		var part := SystemFindObjectBySerial(CInt(s));
		if(part)
			group_items.append(part);
		endif
	endforeach

	if(!len(group_items))
		return 1;
	endif

	var itemname, itemqual, lockable, is_excep, the_color, CraftedBy;
	foreach member in group_items
		if(member.objtype != UOBJ_COPPER_KEY)
			if(!itemname)
				itemname := GetObjProperty(member, "BaseName");
			endif
			if(!itemqual)
				itemqual := member.quality;
			endif
			if(!lockable)
				lockable := GetObjProperty(member, "lockable");
			endif
			if(!is_excep)
				is_excep := GetObjProperty(member, "IsExceptional");
			endif
			if(!CraftedBy)
				CraftedBy := GetObjProperty(member, "CraftedBy");
			endif
			if(!the_color)
				the_color := member.color;
			endif
		endif

		if(member.isa(POLCLASS_CONTAINER))
			// Same live-iteration hazard as DrainSecureContainerToPacks --
			// snapshot the children before moving any of them out from
			// under this same loop.
			var member_children_snapshot := {};
			var member_bad_at_collection := 0;
			foreach child in ListRootItemsInContainer(member)
				if(!child or child.errortext)
					member_bad_at_collection := member_bad_at_collection + 1;
					continue;
				endif
				member_children_snapshot.append(child);
			endforeach
			if(member_bad_at_collection)
				HouseEscrowLog("[ERROR] TryBulkRedeedItem: ListRootItemsInContainer(" + member.serial + ") returned " + member_bad_at_collection + " already-bad entry(ies) at collection time (before any moves happened) -- likely pre-existing corrupted item data on this house, not something this sweep caused.");
			endif
			foreach child in member_children_snapshot
				MoveItemToHouseEscrowPacks(packs, child, label, realm, "redeed_member_child");
			endforeach
		endif

		member.movable := 1;
		DestroyItem(member);
	endforeach

	var staged_deed := CreateItemAtLocation(5288, 1176, 0, CInt(from_deed), 1, realm);
	if(staged_deed and !staged_deed.errortext)
		SetObjProperty(staged_deed, "ItemBaseName", itemname);
		SetObjProperty(staged_deed, "ItemQuality", itemqual);
		if(itemname)
			SetName(staged_deed, "deed for : " + itemname);
		endif
		if(lockable)
			SetObjProperty(staged_deed, "ItemIsLockable", 1);
		endif
		if(is_excep)
			SetObjProperty(staged_deed, "ItemIsExceptional", 1);
			if(CraftedBy)
				SetObjProperty(staged_deed, "CraftedBy", CraftedBy);
			endif
		endif
		if(the_color)
			staged_deed.color := the_color;
		endif
		MoveItemToHouseEscrowPacks(packs, staged_deed, label, realm, "redeed_staged_deed");
	else
		HouseEscrowLog("[ERROR] TryBulkRedeedItem: failed to create replacement deed objtype=" + Hex(CInt(from_deed)) + " for group starting at item=" + item.serial);
	endif

	return 1;
endfunction

// Fires any player merchant standing in the house via the existing
// pm_force_fire event mechanism (pkg/systems/playervendor/playermerchant.src,
// same path pmfireme.src already uses), routing the payout into the
// existing, separate "Merchant Escrow" system -- firing a merchant is
// independent of the house-demolish escrow.
function FireHouseMerchants(house, who)
	var fired := 0;
	foreach mobile in (house.mobiles)
		if(mobile.isa(POLCLASS_NPC) and (mobile.script == "playermerchant" or mobile.script == ":playervendor:playermerchant"))
			if(mobile.process)
				var ev := struct;
				ev.type := "pm_force_fire";
				ev.source := who;
				ev.reason := "house_demolish_escrow";
				ev.force_escrow := 1;
				mobile.process.sendevent(ev);
				HouseEscrowLog("FireHouseMerchants: sent pm_force_fire to merchant " + mobile.serial + " (" + mobile.name + ") in house " + house.serial + ", triggered by staff " + who.serial);
				fired := fired + 1;
			endif
		endif
	endforeach
	return fired;
endfunction

// Same deed-objtype table as sign.src's existing case-14 demolish flow.
function CreateHouseDeedForType(house_objtype)
	var deed_objtype := 0;
	case(house_objtype)
		0x6064: deed_objtype := 0x6019;
		0x6066: deed_objtype := 0x601A;
		0x6068: deed_objtype := 0x6231;
		0x606A: deed_objtype := 0x601B;
		0x606C: deed_objtype := 0x601C;
		0x606E: deed_objtype := 0x601D;
		0x6070: deed_objtype := 0x601E;
		0x6072: deed_objtype := 0x601F;
		0x6074: deed_objtype := 0x6020;
		0x6076: deed_objtype := 0x6021;
		0x6078: deed_objtype := 0x6022;
		0x607A: deed_objtype := 0x6023;
		0x607C: deed_objtype := 0x6024;
		0x607E: deed_objtype := 0x6025;
		0x608C: deed_objtype := 0x6026;
		0x608D: deed_objtype := 0x6230;
		0x60A0: deed_objtype := 0x6227;
		0x60A2: deed_objtype := 0x6228;
		0x6098: deed_objtype := 0x6229;
		0x609C: deed_objtype := 0x622A;
		0x609A: deed_objtype := 0x622B;
		0x609E: deed_objtype := 0x622C;
		0x6096: deed_objtype := 0x622D;
		0x6388: deed_objtype := 0x622E;
		0x6BB8: deed_objtype := 0x622F;
		0xA3E0: deed_objtype := 0x6233;
		0xA3E1: deed_objtype := 0x6234;
		0xA3E2: deed_objtype := 0x6235;
		0xA3E3: deed_objtype := 0x6236;
		0xA3E4: deed_objtype := 0x6237;
		0xA3E5: deed_objtype := 0x6238;
		0xA3E6: deed_objtype := 0x6239;
		0xA3E7: deed_objtype := 0x623D;
		0xA3E8: deed_objtype := 0x623E;
		0xA3E9: deed_objtype := 0x623F;
		0xA3EA: deed_objtype := 0x6240;
		0xA3EB: deed_objtype := 0x6241;
		0xA3EC: deed_objtype := 0x6243;
		0xA3ED: deed_objtype := 0x6244;
		0xA3EE: deed_objtype := 0x6245;
		0xA3EF: deed_objtype := 0x6246;
		0xA3F0: deed_objtype := 0x6247;
		0xA3F1: deed_objtype := 0x6248;
		0xA3F2: deed_objtype := 0x6249;
	endcase
	return deed_objtype;
endfunction

////////////////////////////////////////////////////////////////////////////
// Orchestration entry point
////////////////////////////////////////////////////////////////////////////
function DemolishAndEscrowHouse(who, house, sign)
	// A big house (guild houses especially -- higher lockdown/secure caps)
	// can mean thousands of items to sweep, each needing several property
	// reads/container ops. That's real risk of tripping pol.cfg's
	// RunawayScriptThreshold (500000 instructions without sleeping) partway
	// through -- which would mean items already destroyed/moved out of the
	// house, but execution never reaching Step 8/9 below to actually save
	// anything to escrow. SCRIPTOPT_NO_RUNAWAY is a backstop; the periodic
	// SleepMS() yields sprinkled through the loops below (search "yield
	// check") are the primary fix, since they also keep this from
	// monopolizing the whole (single-threaded, cooperative) scheduler for a
	// huge house instead of just disabling the safety check outright.
	Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);

	var result := struct;
	result.ok := 0;
	result.escrowed_pack_count := 0;
	result.owner_name := "Unknown Owner";
	result.owner_resolved := 0;
	result.items_left_behind := 0;
	result.errmsg := "";

	if(!house)
		result.errmsg := "House not found.";
		return result;
	endif

	// Cached up front and used for the rest of this function instead of
	// house.serial/house.objtype/house.realm -- DestroyMulti(house) runs
	// partway through, and property access on an already-destroyed multi
	// isn't something to rely on for the logging/escrow calls that happen
	// afterward. house_realm also feeds every CreateItemAtLocation/
	// CreateHouseEscrowPack call below, so escrow packs get created in the
	// SAME realm as the house being swept -- see CreateHouseEscrowPack.
	var house_serial := house.serial;
	var house_objtype := house.objtype;
	var house_realm := house.realm;

	// Step 1: resolve the owner. Never abort solely because the owner can't
	// be found -- that's exactly the case this feature exists for.
	var raw_ownerserial_prop := GetObjProperty(house, "ownerserial");
	var ownerserial := CInt(raw_ownerserial_prop);

	var owner := SystemFindObjectBySerial(ownerserial);
	if(!owner)
		owner := SystemFindObjectBySerial(ownerserial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	endif

	var owner_name;
	if(owner)
		owner_name := owner.name;
		result.owner_resolved := 1;
	else
		owner_name := GetObjProperty(house, "owneracct");
		if(!owner_name)
			owner_name := GetObjProperty(sign, "lastownername");
		endif
		if(!owner_name)
			owner_name := "Unknown Owner";
		endif
		result.owner_resolved := 0;
		HouseEscrowLog("[ORPHANED] DemolishAndEscrowHouse: owner serial " + ownerserial + " could not be resolved for house " + house_serial + "; filing escrow under that serial anyway, name fallback=" + owner_name);
		QueueHouseEscrowGMPage(ownerserial, "House " + house_serial + " demolished-and-escrowed by " + who.name + "(" + who.serial + ") but owner serial " + ownerserial + " could not be resolved to a character. Escrow filed under that serial; use .houseescrow " + ownerserial + " as staff to inspect/reassign.");
	endif
	result.owner_name := owner_name;

	var house_label := GetObjProperty(house, "MultiType");
	if(!house_label or house_label == "")
		house_label := ResolveHouseTypeDisplayName(house_objtype);
	endif

	HouseEscrowLog("DemolishAndEscrowHouse: starting house=" + house_serial + " owner=" + ownerserial + " (" + owner_name + ") resolved=" + result.owner_resolved + " type=" + house_label + " triggered_by=" + who.serial);

	// Step 2: create the house deed while house.objtype is still valid. Held
	// aside and escrowed last, as its own separately-labeled pack.
	var deed_objtype := CreateHouseDeedForType(house_objtype);
	var house_deed := 0;
	if(deed_objtype)
		house_deed := CreateItemAtLocation(5288, 1176, 0, deed_objtype, 1, house_realm);
	endif

	// Step 3: destroy structural components -- unchanged from the existing
	// case-14 demolish flow (sign.src), never escrowed. Must happen before
	// the house.items snapshot below.
	var addonitem, founditem;
	addonitem := CInt(GetObjProperty(house, "component1"));
	if(addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
	addonitem := CInt(GetObjProperty(house, "component2"));
	if(addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
	addonitem := CInt(GetObjProperty(house, "component3"));
	if(addonitem) DestroyItem(SystemFindObjectBySerial(addonitem)); endif
	addonitem := CInt(GetObjProperty(house, "builtdeed"));
	if(addonitem)
		founditem := DestroyItem(SystemFindObjectBySerial(addonitem));
		if(founditem <> 1) founditem := DestroyItem(SystemFindObjectBySerial(addonitem, 1)); endif
		if(founditem <> 1) DestroyItem(SystemFindObjectBySerial(addonitem, 2)); endif
	endif

	if(house_objtype == 0x608D)
		foreach item in ListItemsNearLocation(sign.x, sign.y, 0, 8)
			if((item.objtype == 0x177d) or (item.objtype == 0xb41) or (item.objtype == 0xb42))
				DestroyItem(item);
			endif
		endforeach
	endif

	// Destroy every teleporter this house has ever placed via the canonical
	// CProp-tracked cleanup (utility.inc) -- catches untracked/legacy
	// teleporters too, more reliably than a plain "teleportserial" sweep of
	// house.items. Runs before the snapshot below so teleporters never enter
	// the classification pass at all.
	CleanupHouseTeleporters(house);

	// Step 4: snapshot house.items before any further destruction.
	// house.items/ListItemsNearLocation can apparently hand back a small
	// number of already-bad/error entries directly (confirmed in practice:
	// traced a batch of "invalid item reference" failures at the generic
	// bucket all the way back to here -- item.serial on a bad entry
	// silently reads falsy instead of throwing, in EScript's duck-typed
	// struct access, so nothing before this point ever caught it). Guard
	// here so bad entries are identified and dropped at the source instead
	// of silently riding along in items_snapshot for hundreds of items
	// before finally surfacing downstream as an unhelpful failure.
	// items_snapshot_serials caches each item's serial AT SNAPSHOT TIME,
	// parallel-indexed with items_snapshot. The classification loop below
	// uses these cached values (not a live item.serial re-read) for its
	// "already processed"/"is the sign" checks -- see the comment there for
	// why a live re-read isn't safe.
	var items_snapshot := {};
	var items_snapshot_serials := {};
	var snapshot_serials := dictionary;
	var snapshot_bad_count := 0;
	foreach item in (house.items)
		if(!item or item.errortext)
			snapshot_bad_count := snapshot_bad_count + 1;
			continue;
		endif
		items_snapshot.append(item);
		items_snapshot_serials.append(item.serial);
		snapshot_serials[item.serial] := 1;
	endforeach

	// Some house-linked items (e.g. secures placed on a tent, validated via
	// InsideTent() rather than house.items membership -- see
	// pkg/std/housing/sign.src's own HouseFunctionSecure) never actually
	// register as house.items members. The existing case-14 self-service
	// demolish flow compensates with this same proximity sweep so those
	// items still get handled instead of surviving DestroyMulti as orphans;
	// dedup by serial against what's already in the snapshot.
	var proximity_z := 0;
	repeat
		foreach item in ListItemsNearLocation(sign.x, sign.y, proximity_z, 20)
			if(!item or item.errortext)
				snapshot_bad_count := snapshot_bad_count + 1;
				continue;
			endif
			if(!snapshot_serials[item.serial] and (item.getprop("houseserial") == house_serial or item.getprop("f")))
				items_snapshot.append(item);
				items_snapshot_serials.append(item.serial);
				snapshot_serials[item.serial] := 1;
			endif
		endforeach
		proximity_z := proximity_z + 20;
	until(proximity_z >= 60);

	HouseEscrowLog("DemolishAndEscrowHouse: house=" + house_serial + " items_snapshot size=" + len(items_snapshot) + " bad_entries_dropped_at_snapshot=" + snapshot_bad_count);

	// Step 5: single classification pass over the snapshot.
	var packs := {};
	var processed := dictionary;
	var secures_drained := 0;
	var omegacaches_destroyed := 0;
	var redeed_groups := 0;
	var generic_moved := 0;
	var trashcans_redeeded := 0;
	var items_left_behind := 0;
	var items_skipped_nested := 0;
	var yield_counter := 0;
	var item_index := 0;

	foreach item in items_snapshot
		item_index := item_index + 1;
		// Cached at Step 4, when this item was confirmed valid -- NOT a live
		// item.serial re-read. Confirmed root cause of a set of "invalid/
		// stale item reference" failures: TryBulkRedeedItem can destroy
		// OTHER items via ItemsCreatedSerials (a whole redeed group), not
		// just the one it was called on. If one of those grouped items also
		// has its own separate entry later in items_snapshot (a genuinely
		// standalone top-level item sharing the group's tag), it's already
		// destroyed by the time its own turn comes up here -- and a live
		// item.serial read on an already-destroyed handle reads back falsy
		// instead of the real serial, so "processed[item.serial]" misses
		// its own key and the dangling item limps through every remaining
		// check instead of being cleanly skipped.
		var cached_serial := items_snapshot_serials[item_index];

		// yield check -- see the SCRIPTOPT_NO_RUNAWAY comment above.
		yield_counter := yield_counter + 1;
		if(yield_counter >= 25)
			yield_counter := 0;
			SleepMS(1);
		endif
		if(item_index % 100 == 0)
			HouseEscrowLog("DemolishAndEscrowHouse: house=" + house_serial + " progress " + item_index + "/" + len(items_snapshot));
		endif

		if(cached_serial == sign.serial)
			continue;
		endif
		if(processed[cached_serial])
			continue;
		endif

		// Belt-and-suspenders: if this item was destroyed as a side effect
		// of something else (e.g. it just wasn't caught above for some
		// other reason), catch it here with a clear, traceable message
		// tied to its real serial instead of letting it silently limp
		// through the branches below and surface as an unhelpful
		// "<uninitialized object>" failure deep inside the generic bucket.
		if(!item or item.errortext)
			HouseEscrowLog("[ERROR] DemolishAndEscrowHouse: item " + cached_serial + " went stale between snapshot and classification (likely destroyed as a side effect of an earlier redeed group sharing its ItemsCreatedSerials) -- skipped.");
			continue;
		endif

		// Items nested inside another item (contents of a secure, a
		// wardrobe, a redeed-placed cabinet, etc.) get carried along
		// automatically when that parent container is itself moved or
		// drained -- DrainSecureContainerToPacks/TryBulkRedeedItem each do
		// their own independent ListRootItemsInContainer lookup for their
		// container's direct children, and a plain container move via
		// MoveItemToContainer already brings its contents with it. If
		// house.items also lists those same nested items individually
		// (recursively), processing them AGAIN here -- often after their
		// parent has already been relocated into an escrow pack inside the
		// storage area -- is what caused mass "could not be moved into
		// escrow" failures (a real, confirmed case: 343 clothing items
		// inside one container). Only handle true top-level items here.
		if(item.container)
			items_skipped_nested := items_skipped_nested + 1;
			continue;
		endif

		if(item.usescript == USESCRIPTID_SECURE_CONTAINER)
			items_left_behind := items_left_behind + DrainSecureContainerToPacks(item, packs, house_label, house_realm);
			secures_drained := secures_drained + 1;
			continue;
		endif

		if(item.objtype == OMEGACACHE_OBJTYPE)
			EraseObjProperty(item, "houseserial");
			DestroyItem(item);
			omegacaches_destroyed := omegacaches_destroyed + 1;
			continue;
		endif

		// Trash cans have no FromDeed/ItemsCreatedSerials (same as
		// scripts/textcmd/player/redeed.src's own special case for this
		// objtype) so TryBulkRedeedItem below would never catch them --
		// destroy and hand back the reusable trash-can token (0x7009) the
		// same way the player-facing .redeed command does, instead of
		// escrowing the physical trash can itself.
		if(item.objtype == 0x7007)
			DestroyItem(item);
			var trash_token := CreateItemAtLocation(5288, 1176, 0, 0x7009, 1, house_realm);
			if(trash_token and !trash_token.errortext)
				MoveItemToHouseEscrowPacks(packs, trash_token, house_label, house_realm, "trashcan_token");
			endif
			trashcans_redeeded := trashcans_redeeded + 1;
			continue;
		endif

		if(TryBulkRedeedItem(item, processed, packs, house_label, house_realm))
			redeed_groups := redeed_groups + 1;
			continue;
		endif

		if(MoveItemToHouseEscrowPacks(packs, item, house_label, house_realm, "generic_toplevel"))
			generic_moved := generic_moved + 1;
		else
			items_left_behind := items_left_behind + 1;
		endif
	endforeach

	HouseEscrowLog("DemolishAndEscrowHouse: house=" + house_serial + " swept secures=" + secures_drained + " omegacaches=" + omegacaches_destroyed + " redeed_groups=" + redeed_groups + " trashcans=" + trashcans_redeeded + " generic_items=" + generic_moved + " skipped_nested=" + items_skipped_nested + " items_left_behind=" + items_left_behind + " packs_so_far=" + len(packs));
	result.items_left_behind := items_left_behind;

	// Step 6: drain the Omega Cache DataFile wholesale, then wipe it exactly
	// as the existing player-facing demolish flow does. DrainOmegaCacheStoreToPacks
	// closes its own handle before returning, so re-opening here for the wipe
	// doesn't stack a second, never-released reference on top of it.
	DrainOmegaCacheStoreToPacks(house_serial, packs, house_label, house_realm);

	var ocdf := OpenOmegaCacheStore(house_serial);
	if(ocdf)
		foreach ockey in (ocdf.Keys())
			ocdf.DeleteElement(ockey);
		endforeach
		CloseOmegaCacheStore(house_serial);
	endif

	HouseEscrowLog("DemolishAndEscrowHouse: house=" + house_serial + " omega cache drain complete, packs_so_far=" + len(packs));

	// Step 7: fire merchants, house-list bookkeeping, then destroy the multi.
	var fired := FireHouseMerchants(house, who);

	if(result.owner_resolved)
		RemoveHouseFromCharacter(owner, house_serial);
	endif

	DestroyMulti(house);
	HouseEscrowLog("DemolishAndEscrowHouse: house=" + house_serial + " multi destroyed, proceeding to escrow flush -- " + len(packs) + " content pack(s) pending, deed_created=" + (house_deed and !house_deed.errortext));

	// Step 8: escrow the house deed as its own separately-labeled pack.
	var deed_escrow_names := {};
	if(house_deed and !house_deed.errortext)
		var deed_packs := {};
		MoveItemToHouseEscrowPacks(deed_packs, house_deed, house_label + " deed", house_realm, "house_deed");
		foreach hpack in deed_packs
			var deed_escrow_name := SaveHouseEscrowPack(hpack, ownerserial, house_serial, house_label + " deed");
			if(deed_escrow_name)
				deed_escrow_names.append(deed_escrow_name);
				result.escrowed_pack_count := result.escrowed_pack_count + 1;
			endif
		endforeach
	else
		HouseEscrowLog("[ERROR] DemolishAndEscrowHouse: no house deed was created for objtype=" + Hex(house_objtype) + ", house=" + house_serial);
	endif
	AppendHouseEscrowIndexBatch(ownerserial, house_serial, house_label + " deed", owner_name, deed_escrow_names);

	// Step 9: flush every accumulated content pack, then index them all in
	// one batch (see SaveHouseEscrowPack/AppendHouseEscrowIndexBatch above).
	var content_escrow_names := {};
	foreach hpack in packs
		var content_escrow_name := SaveHouseEscrowPack(hpack, ownerserial, house_serial, house_label);
		if(content_escrow_name)
			content_escrow_names.append(content_escrow_name);
			result.escrowed_pack_count := result.escrowed_pack_count + 1;
		endif
	endforeach
	AppendHouseEscrowIndexBatch(ownerserial, house_serial, house_label, owner_name, content_escrow_names);

	result.ok := 1;
	HouseEscrowLog("DemolishAndEscrowHouse: completed house=" + house_serial + " owner=" + ownerserial + " (" + owner_name + ") resolved=" + result.owner_resolved + " packs=" + result.escrowed_pack_count + " merchants_fired=" + fired);

	return result;
endfunction
