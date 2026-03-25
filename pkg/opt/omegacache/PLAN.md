# Omega Cache - Planning Document

> Package: `pkg/opt/omegacache`

## Overview

The Omega Cache is a house furniture item that provides virtual, unlimited-capacity storage for stackable resources. Players deposit items which are destroyed and recorded as quantities in a DataFile. On withdrawal, items are recreated from the stored quantity. The primary goal is to **reduce server item count** by eliminating thousands of physical resource stacks from player houses.

Omega Caches are tied to the house, not to individual cabinet items. A house may have multiple physical cabinets, but they all share the same storage pool (the house's DataFile). This aligns with how Secure Containers work — the house defines the limit, and cabinets are placed via deeds.

Original author: DeiviD (deivid@neverlands.org), package renamed from `resourcesscollector` → `storagecabinet` → `omegacache`.

---

## Current State

### What Exists

- **`homecollector.src`** (~2000+ lines): Main script. Opens a multi-page gump with hardcoded category tabs (Reagents, Gems, Resources, Food, Money, Potions, Ammo, Scrolls, Runes). Handles withdrawal via text entry fields per item row.
- **`omegacache.inc`** (~4400 lines): Contains ~300 individual getter functions (one per item type returning `CInt(GetObjProperty(item, "X"))`), a massive `store_collection_item()` case statement for deposits, and the `rescollect()` targeting function.
- **`categories.cfg`**: Defines all item categories, their objtypes, display names, and gump icons.
- **`itemdesc.cfg`**: Defines the physical cache furniture item (`0xDF0A`, graphic 42789). The player-visible Name and Desc are configurable here — the item can be renamed at any time without code changes.
- **`blacklist.cfg`**: Items excluded from storage even if stackable.
- **`pkg.cfg`**: Package definition.

### How It Currently Works

**Storage Model**: CProps on the cabinet item. Each item type is a separate CProp (e.g., `SetObjProperty(cabinet, "Ginseng", 500)`). Up to ~300 CProps on a single item.

**Deposit**: Player clicks "Add Item To Drawer", targets an item or container. Script checks objtype against a hardcoded whitelist, then adds the item's amount to the corresponding CProp and (presumably) destroys the item.

**Withdrawal**: Player navigates to a category page, types a quantity into a text field, clicks the category take button. Script reads the text entry, validates amount, calls `CreateItemInBackpack()`, and decrements the CProp.

### Known Issues

1. **DestroyItem may be missing** from the deposit flow — needs verification. If absent, this is a duplication exploit.
2. **CProps at scale**: 300+ CProps on a single item is unusual for POL. Bloats item serialization on every world save.
3. **Deposit limited to 50 items** when targeting a container — arbitrary cap with no player feedback about skipped items.
4. **No fall-through protection** between case blocks in the withdrawal switch — may cause unintended multi-category withdrawals depending on eScript case semantics.
5. **No backpack weight/item count validation** on withdrawal — commented out, never implemented.
6. **Special potions (0x7059)** share an objtype and differ by `itemtype` CProp — deposit/withdrawal likely collapses them into one bucket.
7. **Runes and scrolls** have CProps (SpellID, enchantment data) that would be lost when stored as simple counts.
8. **Massive code duplication**: The `.inc` file has a dedicated function per item type. The deposit case statement repeats the same 6-line pattern ~300 times. The gump builder hardcodes every row.
9. **No data-driven design**: Adding a new item requires changes in `categories.cfg`, `omegacache.inc` (getter function + deposit case + whitelist entry), and `homecollector.src` (gump row + withdrawal case).

---

## Proposed Architecture

### Storage Model: DataFile

Replace CProps with POL's DataFile system. Storage is per-house, not per-cabinet. All cabinets in a house share the same DataFile, keyed by the house serial.

```
// Structure: data/ds/omegacache/house_<houseserial>.txt
// Each element key = objtype (as string)
// Each element has property "qty" = stored amount

var house_serial := GetObjProperty(cabinet, "houseserial");
var df := OpenDataFile(":omegacache:house_" + house_serial);
var key := BuildItemKey(item);  // "0x6011|d41d8cd9..." (objtype|hash, always)
var elem := df.FindElement(key);
var qty := CInt(elem.GetProp("qty"));
```

**Why DataFile over CProps:**
- Zero impact on item serialization — cabinet item stays lightweight
- Data lives in separate files under `data/ds/omegacache/`
- Enumerable via `df.Keys()` — can build gumps dynamically
- Can be unloaded from memory with `UnloadDataFile()` when not in use
- Human-readable on disk for debugging
- Naturally shared across all cabinets in a house

**Why not Storage Areas:**
- Storage Areas store real Item objects — does not reduce item count (defeats the primary goal)
- Subject to container weight/item limits
- Higher memory and world-save overhead

**Why not SQL:**
- Not compiled into the current build (requires `HAVE_MYSQL` flag)
- Not used anywhere else in the shard
- External dependency (MySQL server) for a simple key-value store

### Concurrency

Not a concern. POL uses single-threaded cooperative multitasking for script execution. No two scripts execute simultaneously. The only consideration is logical: validate stored quantities at transaction time (not at gump-open time) since another player may have withdrawn between gump open and button press.

### Item Eligibility: Stackability Rule

An item is eligible for storage if it passes the same checks as our `CanStack()` function. This function currently lives in `pkg/packethooks/packethook/packethook.src` (lines 25-67) but must be **moved to `scripts/include/canstack.inc`** as a shared include — eScript cannot include across packages. The packethook script will be updated to include from the new shared location. `CanStack()` mirrors POL core's `Item::can_add_to_self()` and checks:

- `objtype` match
- `stackable` flag (from tiledata `FLAG_STACKABLE`)
- `graphic` match
- `color` match
- `newbie` / `insured` / `cursed` flags match
- `quality` match
- `weight_multiplier_mod` match
- `usescript` / `equipscript` / `snoopscript` match (stricter than POL core — see below)
- **All CProps must match**

**Note on scripts**: POL's core `can_add_to_self()` does NOT check `usescript`, `equipscript`, or `snoopscript`. Our `CanStack()` is intentionally **stricter** — items with different per-instance script overrides produce different hashes and are stored separately. This prevents loss of script overrides during deposit/withdrawal. The scripts are accessible via `item.usescript`, `item.equipscript`, `item.snoopscript` and are stored on the DataFile element for restoration.

POL's stacking guarantee means: **if two items can stack, they are identical in every meaningful property.** The `BuildItemKey()` function captures item identity as an `"objtype|md5hash"` key by hashing only properties that differ from itemdesc defaults. Only non-default properties are stored on the DataFile element for restoration on withdrawal — recreated items inherit defaults naturally from `CreateItemInContainer()`. Items with the same objtype but different overrides produce different hashes and are stored as separate entries.

`IsEligibleForStorage()` checks that the item is stackable and not on the blacklist. Any stackable item is accepted — the key/hash system handles identity and property preservation automatically.

### Blacklist

Items that are stackable but should NOT be stored are defined in `pkg/opt/omegacache/blacklist.cfg`:

```
// blacklist.cfg — items excluded from Omega Cache storage even if stackable.
//
// Format: one element per blacklisted objtype.
// "Reason" property documents why the item is blacklisted.

Blacklist 0x1BE9
{
    Reason  Gold ingot - not a real item on this shard (graphic used by clay)
}

// Add more as needed:
// Blacklist 0xABCD
// {
//     Reason  Quest token - should not be bulk-stored
// }
```

Loaded once at script start via `ReadConfigFile(":omegacache:blacklist")`. The `IsEligibleForStorage()` function checks `item.stackable` first, then rejects if the objtype has a matching element in the blacklist config. Everything else is accepted.

The blacklist starts minimal — items are added only when a specific reason arises (e.g., an item that happens to be stackable but shouldn't be bulk-stored).

---

## Housing Integration

### Alignment with Secure Containers

Omega Caches follow the same patterns as Secure Containers (`pkg/std/housing/`):

| Aspect | Secure Containers | Omega Caches |
|---|---|---|
| **Limit per house** | `maxnumsecure` on house sign (1-25 depending on house type) | `maxnumomegacache` on house sign (TBD per house type) |
| **Counter** | `numsecure` (available slots remaining) | `numomegacache` (same pattern) |
| **Placement** | Voice command creates container at location | **Deed** placed by player — house enforces limit |
| **Access control** | Owner, Co-owner, Friends with privilege flags | Same system, same privilege checks |
| **House demolition** | All secure containers and contents destroyed | Block destruction if items are stored in the house DataFile |

### House Sign Display

The house sign gump (`pkg/std/housing/sign.src`) currently displays lockdowns and secures on the info page:

```
// sign.src line 158-159
data[8]  := maxnumlockdowns - numlockdowns + "/" + maxnumlockdowns;   // "94/100"
data[10] := maxnumsecure - numsecure + "/" + maxnumsecure;            // "3/6"
```

Add a new line for Omega Caches in the same format:

```
// New: "Number of Omega Caches:"
// Display: used/max  (e.g., "1/2")
data[N] := maxnumomegacache - numomegacache + "/" + maxnumomegacache;
```

This requires changes to the sign gump layout and data arrays in `sign.src`.

### Changes to Housing Scripts

The following functions in `pkg/std/housing/sign.src` must be extended:

**`AssignDefaultContainers(house)`** (line 418) — Add `numomegacache` per house type:
```
// Example: alongside existing numsecure assignments
0x6074: SetObjProperty(house, "numlockdowns", 400);
        SetObjProperty(house, "numsecure", 4);
        SetObjProperty(house, "numomegacache", 1);  // NEW
```

**`GetMaxProps(house)`** (line 1446) — Add `maxnumomegacache` per house type:
```
// Example: alongside existing maxnumsecure assignments
0x6074: SetObjProperty(house, "maxnumlockdowns", 400);
        SetObjProperty(house, "maxnumsecure", 4);
        SetObjProperty(house, "maxnumomegacache", 1);  // NEW
```

**Lazy-init check** (lines 148-156) — Extend to detect missing `numomegacache`:
```
// Existing check triggers AssignDefaultContainers if numlockdowns or numsecure missing
// Extend: also trigger if numomegacache is missing
if (GetObjProperty(house, "numomegacache") == error)
    AssignDefaultContainers(house);
endif
if (!GetObjProperty(house, "maxnumomegacache"))
    GetMaxProps(house);
endif
```

The lazy-init pattern means existing houses will automatically get the new properties the first time a player opens their house sign after the update — **no separate migration script needed**. The `AssignDefaultContainers` and `GetMaxProps` functions are already called as a fallback when properties are missing. Adding the Omega Cache properties to these functions gives us migration for free.

### Cabinet Limits Per House Type

TBD — suggested starting values (conservative, can be increased):

| House Size | Secures | Suggested Cabinets |
|---|---|---|
| Small (0x6064-0x606E, 0x6070-0x6072) | [ ] 1 | 1 |
| Medium (0x6074, 0x608D) | 3-4 | [ ] 1 |
| Large (0x6076, 0x6078, 0x608C) | [ ] 6 | [ ] 2 |
| Mansion (0x607A) | [ ] 15 | [ ] 2 |
| Castle (0x607C) | [ ] 20 | [ ] 3 |
| Fortress (0x607E) | [ ] 25 | [ ] 3 |
| Keep (0x6BB8) | [ ] 1 | 1 |
| Custom multi-houses (0xA3E0-0xA3F0) | 3-22 | 1-3 (scale with secures) |

Since all cabinets share one storage pool, the limit mainly controls how many physical access points the player can place, not storage capacity.

### Access Control

Reuse the existing house friend/privilege system from `pkg/std/housing/utility.inc`:

```
// Privilege constants (from utility.inc)
const VIEW_SECURE := 2;        // Can view secure containers
const ADD_TO_SECURE := 3;      // Can add items to secure containers
const REMOVE_FROM_SECURE := 4; // Can take items from secure containers
const CO_OWNER := 8;           // Co-owner (all permissions)
```

Access checks for Omega Caches:
- **Open/view cabinet**: Owner, Co-owner, GM+, or friend with `VIEW_SECURE` privilege
- **Deposit items**: Owner, Co-owner, GM+, or friend with `ADD_TO_SECURE` privilege
- **Withdraw items**: Owner, Co-owner, GM+, or friend with `REMOVE_FROM_SECURE` privilege

This reuses the existing permission system — no new privilege indices needed.

### Cabinet Placement via Deed

1. Player acquires a Omega Cache Deed (crafted, purchased, or GM-granted).
2. Player uses the deed inside their house.
3. Script checks:
   - Player is inside a house they own or co-own
   - House has not reached `maxnumomegacache` limit
   - Target location is valid (inside house bounds)
4. Cabinet item (`0xDF0A`) is created at the target location.
5. Cabinet gets `houseserial` CProp linking it to the house.
6. House's `numomegacache` counter is decremented.
7. Deed is destroyed.

### Cabinet Destruction

- **Destruction is blocked** if the house's DataFile contains any stored items.
- Player must withdraw all items before removing a cabinet.
- If the house has multiple cabinets and items remain, individual cabinets can only be removed down to a minimum of 1 (the last cabinet cannot be removed while items exist).

### House Demolition

Items in the DataFile are lost on demolition, same as secure container contents. The demolition flow in `sign.src` (case 14, line 210) currently:

1. Checks for outside furniture (lines 211-216)
2. `YesNo(who, "Destroy everything inside?")` (line 217)
3. Checks ownership / GM level
4. Checks guild house
5. Creates deed in backpack, destroys all house items, destroys multi

**Before** the existing `YesNo(who, "Destroy everything inside?")` on line 217, send a warning system message if the DataFile is non-empty. No extra modal — just a visible warning alongside the existing confirmation:

```
// After outside furniture check, before existing YesNo
var df := OpenDataFile(":omegacache:house_" + house.serial);
if (df and !IsStoreEmpty(df))
    SendSysMessage(who, "WARNING: Your Omega Caches still contain items. These will be permanently lost!", 3, 53);  // yellow hue
endif
// Existing flow continues: YesNo(who, "Destroy everything inside?")
```

After demolition completes, clean up the DataFile (delete all elements, unload).

---

## Phase 1: Simple Stackable Resources

### Scope

Categories defined in `categories.cfg`. Full item listings per category are in the config file. Summary:

| Category | Group | Count |
|---|---|---|
| Cloth & Fibre | Crafting | [ ] 4 |
| Hides | Crafting | [ ] 18 |
| Ingots | Crafting | [ ] 34 |
| Logs | Crafting | [ ] 26 |
| Mining Materials | Crafting | [ ] 2 |
| Ores | Crafting | [ ] 34 |
| Seeds | Crafting | [ ] 11 |
| Codex Damnorum Scrolls | Mage | [ ] 16 |
| Earth Book Scrolls | Mage | [ ] 16 |
| Holy Book Scrolls | Mage | [ ] 16 |
| Potions | Mage | [ ] 23 |
| Song Book Scrolls | Mage | [ ] 20 |
| Spellbook Scrolls | Mage | 64 |
| Ammo | General | [ ] 7 |
| Food | General | ~42 |
| Gems | General | [ ] 9 |
| Money | General | [ ] 1 |
| Raw Herbs | General | [ ] 4 |
| Reagents | General | [ ] 26 |
| Special Items | Special | [ ] 5 |
| Miscellaneous | Misc | [ ] 6 |

**Blacklisted items** (in `blacklist.cfg`): Gold Ingot (`0x1BE9` — not a real item, graphic used by clay), Zulu Coin (`0x3B9A` — doesn't exist).

Any stackable item not in a defined category and not blacklisted is displayed under "Miscellaneous / Other" in the gump.

**Note on stackability verification**: Most items have been confirmed stackable via itemdesc or in-game behaviour. Since `IsEligibleForStorage()` checks `item.stackable` at runtime (from tiledata flags), any non-stackable item in `categories.cfg` would simply be rejected at deposit — no bug, just an unused category entry. Full tiledata verification of all items is a low-priority follow-up.

### 1.1 Data Layer

Create a shared include (`omegacache.inc` rewrite) with these core functions:

```
// Key generation
BuildItemKey(item)               // Returns "objtype|md5hash" — always includes hash.
                                 // Collects only properties explicitly set on the item
                                 // (differing from itemdesc defaults): color, graphic,
                                 // quality, flags, weight_multiplier_mod, scripts, + all
                                 // CProps. Sorts alphabetically, builds canonical string,
                                 // MD5Encrypt()s it. Items with no overrides hash to
                                 // MD5("") — a constant. Items with any override produce
                                 // a different hash. Hash matches what the element stores.

BuildDefaultKey(objtype)         // Returns "objtype|md5hash" where md5hash is the hash
                                 // of an empty canonical string (no overrides).
                                 // Equivalent to: objtype + "|" + MD5Encrypt("")
                                 // Used by crafting for fast-path lookups when no physical
                                 // item is available to call BuildItemKey() on.

// Cabinet DataFile management
OpenHouseStore(cabinet)          // Reads houseserial from cabinet, returns DataFile handle.
                                 // Creates the DataFile if it doesn't exist yet.
CloseHouseStore(house_serial)    // Unloads DataFile from memory

// Transactions — all keyed by "objtype|hash"
DepositItem(df, key, item)       // Add item.amount to qty. On first deposit for this key,
                                 // stores per-unit weight (via GetItemDescriptor(item.objtype).Weight)
                                 // and only non-default properties on the element for recreation.
                                 // Properties matching itemdesc defaults are omitted.
WithdrawItem(df, key, amount)    // Subtract quantity, returns amount actually withdrawn
GetStoredAmount(df, key)         // Query current quantity
GetStoredAmountByObjtype(df, objtype)  // Sum qty across all keys matching objtype prefix.
                                       // Used by crafting which only knows objtype.
GetAllStored(df)                 // Returns df.Keys() for enumeration
IsStoreEmpty(df)                 // Returns 1 if no items stored (for destruction check)

// Validation
IsEligibleForStorage(item)       // Checks: stackable flag set, NOT on blacklist.
                                 // Uses CanStack() logic to validate item identity.
                                 // Any stackable item is eligible unless blacklisted.
                                 // Blacklist loaded from :omegacache:blacklist.cfg
```

### 1.2 Deposit Mechanism

**Current problem**: Targets one item at a time, container deposit capped at 50, massive case statement.

**New approach**:

1. Player clicks "Add Item to Omega Cache" button on gump.
2. Access check: player must have `ADD_TO_SECURE` privilege (or be owner/co-owner).
3. Targeting cursor opens. Player targets an item or container.
4. If single item: validate with `IsEligibleForStorage()`, store `objtype -> amount`, destroy item.
5. If container: `EnumerateItemsInContainer()` with **no arbitrary limit**. For each eligible item, deposit and destroy. Track totals per type. Report summary to player: "Deposited: 500 Iron Ingots, 200 Ginseng, 12 Diamond."
6. Skip ineligible items with a summary: "3 items could not be stored."
7. Loop: re-open targeting cursor until player presses ESC.
8. **Deposit-all shortcut**: A separate button (and `.cache deposit` command) that deposits all eligible items from the player's backpack and all child containers in one action. Player is warned in the prompt: "This will deposit all eligible items from your backpack and all bags within it." Uses the same logic as container deposit but targets `who.backpack` directly.

**Data-driven deposit** — no case statement needed:
```
// Pseudocode
var df := OpenHouseStore(cabinet);
foreach thing in EnumerateItemsInContainer(target_container)
    if (IsEligibleForStorage(thing))
        var key := BuildItemKey(thing);  // "objtype|hash"
        DepositItem(df, key, thing);    // reads thing.amount, stores properties on first deposit
        DestroyItem(thing);
    endif
endforeach
```

Eligibility is driven by `CanStack()` logic at runtime — any stackable item not on the blacklist can be stored. Items not listed in `categories.cfg` categories are displayed under "Miscellaneous / Other".

### 1.3 Withdrawal Mechanism

**Current problems**: No weight check, no multi-stack support, hardcoded per-category, single destination (backpack only).

**New approach**:

1. Player opens gump, browses categories (built dynamically from `categories.cfg` + `df.Keys()` to show only items actually in stock).
2. Player enters a quantity and clicks withdraw.
3. Access check: player must have `REMOVE_FROM_SECURE` privilege (or be owner/co-owner).
4. **Destination container**: Player targets a container to receive items. Default prompt suggests backpack, but any accessible container is valid.
5. **Validation before creating any items**:
   - Check requested amount <= stored amount
   - Calculate how many stacks are needed: `CInt(Ceil(CDbl(amount) / CDbl(stack_limit)))` where `stack_limit` defaults to 60000
   - Check destination container has room for that many new item slots (`max_items - current_items >= stacks_needed`)
   - **Weight check on full container hierarchy**: Walk from destination container up through all parent containers to the root, checking that each level can hold the additional weight. The tightest constraint wins.
   - If validation fails: report the error with the maximum amount that *can* be withdrawn. Do not create any items.
6. **Create items — decrement per stack, not in bulk**:
   - For each stack to create:
     - Decrement the DataFile by the stack amount
     - Create the item in the destination container
     - If creation fails: re-credit the DataFile for this stack's amount, stop, report what was successfully withdrawn
   - This per-stack approach prevents duplication (no items created without a matching DataFile debit) and prevents loss (failed creates are re-credited immediately).
7. **Report**: "Withdrawn: 120,000 Iron Ingots (2 stacks of 60,000) to your backpack."

**Weight calculation across container hierarchy**:
```
// Pseudocode
function GetMaxWithdrawableByWeight(destination, elem, requested_amount)
    var item_weight := CDbl(elem.GetProp("weight"));  // per-unit weight via GetItemDescriptor()
    if (item_weight <= 0)
        return requested_amount;  // weightless items
    endif

    var max_by_weight := requested_amount;
    var container := destination;

    // Walk up the full parent chain
    while (container)
        var available_weight := CDbl(container.max_weight) - CDbl(container.weight);
        var fits := CInt(available_weight / item_weight);  // truncates down — intentional
        if (fits < max_by_weight)
            max_by_weight := fits;
        endif
        container := container.container;  // parent container
    endwhile

    return max_by_weight;
endfunction
```

### 1.4 Gump Redesign

**Current problem**: Every gump row is hardcoded — adding an item means editing 3+ places.

**New approach**: Build gump pages dynamically from `categories.cfg`:

**Parsing note**: `categories.cfg` uses objtypes as property names with empty string values (e.g., property name `"0x0F85"`, value `""`). To read items in a category, use `ListConfigElemProps(elem)` to get all property names (objtypes) from the section element, then iterate. Do not use `GetConfigString()` — the values are empty.

1. Read categories and their items from `categories.cfg`
2. For each category, query DataFile for stored quantities
3. Only show items that have stock > 0 (or show all with "0" for empty)
4. Each row: icon (from itemdesc graphic) | item name (from itemdesc) | stored quantity | text entry field | take button
5. Category pages generated in a loop, not hardcoded
6. "Add Item to Omega Cache" button (replaces "Add Item To Drawer")
7. "Deposit All" button for backpack + child containers

**Note**: Keep the current visual style for now. Schedule a follow-up step to evaluate and potentially redesign the gump layout after the core functionality is working.

### 1.5 Item Definitions (`itemdesc.cfg`)

The current `itemdesc.cfg` defines the cache as a `Container` with lock scripts and container dimensions. This needs to be reworked — the Omega Cache is not a physical container that players open. It's a furniture item with a use-script that opens a gump.

**Current definition (to be replaced):**
```
Container 0xdf0a    // Wrong — should be Item, not Container
{
    ControlScript ::lockchests  // Wrong — not a lockable chest
    Gump 0                      // Wrong — no container gump needed
    MinX/MaxX/MinY/MaxY         // Wrong — no container dimensions needed
    movable 1                   // Wrong — should be immovable once placed
}
```

**Revised Omega Cache item:**
```
Item 0xDF0A
{
    Name            OmegaCacheContainer
    Desc            Omega Crate of Holding
    Graphic         0x0E43
    Weight          250
    Movable         0
    Script          omegacache
    DestroyScript   :omegacache:destroycache
    cprop   tooltips_note   sAn resource cache infused with Omega energy able to store vast amounts of items
}
```

- `Item` not `Container` — no physical container UI
- Keep existing Name/Desc (player-visible, configurable)
- Keep tooltips_note CProp for client tooltip display
- `Movable 0` — placed via deed, secured to house
- `Script omegacache` — opens the cache gump on use (replaces `homecollector`)
- `DestroyScript` — custom script to block destruction if items are stored, re-credits `numomegacache` on house
- Removed: `ControlScript`, `Gump`, `MinX/MaxX/MinY/MaxY` (not a container)

**Omega Cache Deed:**
```
Item 0xDF0B
{
    Name            OmegaCacheContainerDeed
    Desc            An Omega Crate of Holding Deed
    Graphic         0x14F0
    Weight          1
    Movable         1
    Script          :omegacache:placecache
}
```

- Objtype `0xDF0B` — TBD, needs to be confirmed as unused on the shard
- Graphic `0x14F0` — standard deed graphic (same as house deeds)
- `Script :omegacache:placecache` — handles placement flow (house check, limit check, targeting, creation)

**Note**: The Name and Desc fields are player-visible and can be changed at any time without code changes. The objtype for the deed needs verification against existing shard items to avoid collisions.

### 1.6 Cabinet Lifecycle

- **Deed creation**: Cabinet Deeds can be crafted (carpentry or tinkering TBD) or GM-granted. Deed item defined in `itemdesc.cfg`.
- **Placement**: Player uses deed inside their house. Script validates house ownership/co-ownership, checks `numomegacache` limit, creates cabinet item at target location with `houseserial` CProp. Deed is destroyed.
- **First use**: `OpenDataFile()` is called. If the DataFile doesn't exist for this house yet, `CreateDataFile()` initialises it.
- **Destruction**: Only the last container is blocked from removal if the house's DataFile contains stored items. Non-last containers can be freely removed and return a deed to the player's backpack. The DestroyScript counts containers via `house.items` and is the single source of truth for slot re-crediting. A "Recount Cache Containers" GM tool in House Management can fix slot counts if they get out of sync.
- **House demolition**: Same rules as Secure Containers — all cabinets and stored items are lost. Add a warning during demolition if the DataFile is non-empty: "Warning: Your Omega Caches contain items that will be lost."

---

## Phase 2: Crafting Integration

### Goal

Allow crafting scripts to consume resources directly from a nearby Omega Cache — without needing to withdraw first. All crafting skills are included: blacksmithy, tailoring, carpentry, alchemy, tinkering, bowcraft, cooking, inscription, and cartography.

### Design Principles

1. **Centralise, don't replicate.** Resource availability checking and consumption logic lives in a single shared include (`scripts/include/resourcemanager.inc`). Crafting scripts call centralised functions instead of each implementing their own cache logic.
2. **Uniform treatment.** Every material — whether it's ingots, bottles, blank scrolls, reagents, or food ingredients — goes through the same consumption function. There are no special cases for "utility" vs "primary" materials.
3. **Smart consumption, not interception.** Crafting scripts replace their `SubtractAmount` and amount-check calls with cache-aware equivalents. The decision logic is built into the consumption function, not bolted on around existing calls.
4. **Smelting/meltdown excluded.** Output from smelting ore or melting down items goes to the backpack as normal — no auto-deposit to cache.

### Current Crafting Pattern

All crafting scripts follow the same pattern:

1. Player uses crafting tool → prompted to **target** material
2. Player targets a **physical item** (e.g., ingots, hides, logs)
3. The targeted item's **objtype determines the material type** — difficulty, quality, color, name prefix, elemental properties are all read from crafting config keyed by objtype
4. Script validates amount: `item.amount >= material_cost`
5. Script consumes: `SubtractAmount(item, material_cost)`
6. Some crafts (carpentry, blacksmithy) require **multiple targeting steps** — e.g., target ingots, then target logs

There is no centralised resource consumption function — each craft calls `SubtractAmount()` directly on physical item references. Material type identification uses range-based helpers like `IsIngot()`, `IsHide()`, `IsLog()` in `scripts/include/itemutil.inc`.

### Approach: Cache Container as Target

When a crafting script prompts "What would you like to use that on?", the player can **target the Omega Cache container** instead of a physical item. The crafting script detects the target is an Omega Cache (by objtype), validates access, and opens a **material selection gump**.

**Material selection flow:**
1. Player uses crafting tool
2. "What would you like to use that on?"
3. Player targets the **Omega Cache container** (the physical furniture item)
4. Script detects `target.objtype == OMEGACACHE_OBJTYPE`
5. Access validation (same `FindAccessibleContainer` rules)
6. Material selection gump opens — player picks a material type
7. Script returns a `ResourceRequest` struct with the selected objtype and cache-first source order
8. Crafting script validates the selection (e.g., blacksmithy checks `IsIngot(objtype)`)
9. For multi-material crafts (carpentry), the player can target the cache again for the second material — each material gets its own `ResourceRequest` independently

**Targeting happens once** at the start of crafting. The AutoLoop then depletes from the selected source on each cycle using the `ResourceRequest` set during targeting.

### ResourceRequest and Depletion Priority

A `ResourceRequest` is a struct set during material targeting that controls what to consume and in what order:

**Player targeted the Omega Cache container** → source order: `array{ OMEGA_CACHE, BACKPACK }`
**Player targeted a physical item** → source order: `array{ BACKPACK, OMEGA_CACHE }`

This applies to every material uniformly. The consumption function iterates the source order array, depleting from the first source until exhausted, then moving to the next.

```
// Source constants
const OMEGA_CACHE := 1;
const BACKPACK := 2;

// ResourceRequest struct — set during targeting, used throughout the crafting loop
struct{
    objtype                // the material objtype selected
    preferredSourceOrder   // array{ OMEGA_CACHE, BACKPACK } or array{ BACKPACK, OMEGA_CACHE }
    dataFileHandle         // DataFile handle (or 0 if no cache available)
}
```

### Resource Manager Include

Create a shared include that centralises all resource lookup and consumption:

```
// scripts/include/resourcemanager.inc

const OMEGA_CACHE := 1;
const BACKPACK := 2;

// Get total available amount across all sources
function GetAvailableResource(who, objtype, dataFileHandle := 0)
    var backpack_amount := 0;
    var backpack_items := array{};
    foreach thing in EnumerateItemsInContainer(who.backpack)
        if (thing.objtype == objtype)
            backpack_amount := backpack_amount + thing.amount;
            backpack_items.append(thing);
        endif
    endforeach

    var cache_amount := 0;
    if (!dataFileHandle)
        var access := FindAccessibleContainer(who, array{REMOVE_FROM_SECURE});
        if (access)
            dataFileHandle := access.df;
        endif
    endif
    if (dataFileHandle)
        cache_amount := GetStoredAmountByObjtype(dataFileHandle, objtype);
    endif

    return struct{ total := backpack_amount + cache_amount,
                   backpack := backpack_amount,
                   cache := cache_amount,
                   backpack_items := backpack_items,
                   dataFileHandle := dataFileHandle };
endfunction

// Consume amount following the source order in ResourceRequest
function ConsumeResource(who, resourceRequest, amount)
    var res := GetAvailableResource(who, resourceRequest.objtype, resourceRequest.dataFileHandle);
    if (res.total < amount)
        return error{ errortext := "Insufficient resources" };
    endif

    var remaining := amount;
    foreach source in (resourceRequest.preferredSourceOrder)
        if (remaining <= 0) break; endif
        if (source == BACKPACK)
            remaining := ConsumeFromBackpack(res.backpack_items, remaining);
        elseif (source == OMEGA_CACHE and res.dataFileHandle)
            var consumed := ConsumeFromCache(res.dataFileHandle, resourceRequest.objtype, remaining);
            remaining := remaining - consumed;
        endif
    endforeach

    return 1;
endfunction

// Consume from physical backpack items via SubtractAmount
function ConsumeFromBackpack(backpack_items, amount)
    var remaining := amount;
    foreach stack in backpack_items
        if (remaining <= 0) break; endif
        var take := remaining;
        if (take > stack.amount) take := stack.amount; endif
        SubtractAmount(stack, take);
        remaining := remaining - take;
    endforeach
    return remaining;
endfunction

// Consume from cache DataFile — debits qty without creating physical items
function ConsumeFromCache(dataFileHandle, objtype, amount)
    // Fast path: try default key (no overrides)
    // Fallback: prefix scan for objtype matches
    // Debit via WithdrawItem() which decrements qty / deletes element at 0
    // Returns actual amount consumed
endfunction

// Build a ResourceRequest from targeting a physical item (backpack-first)
function MakeBackpackRequest(who, objtype)
    var dataFileHandle := 0;
    var access := FindAccessibleContainer(who, array{REMOVE_FROM_SECURE});
    if (access)
        dataFileHandle := access.df;
    endif
    return struct{ objtype := objtype,
                   preferredSourceOrder := array{ BACKPACK, OMEGA_CACHE },
                   dataFileHandle := dataFileHandle };
endfunction

// Select material from cache via gump — returns ResourceRequest (cache-first)
function SelectMaterialFromCache(who)
    // Validate access via FindAccessibleContainer
    // Open material selection gump (unfiltered categories)
    // Player picks a material type
    // Return struct{ objtype, preferredSourceOrder := array{ OMEGA_CACHE, BACKPACK }, dataFileHandle }
    // or 0 on cancel
endfunction
```

### Integration with Existing Crafts

Each crafting script replaces its direct `SubtractAmount` and amount-check calls with the centralised resource manager functions:

**Before:**
```
var use_on := Target(who);
if (IsIngot(use_on))
    var material_cost := GetCost(recipe);
    if (use_on.amount < material_cost)
        SendSysMessage(who, "Not enough ingots.");
        return;
    endif
    SubtractAmount(use_on, material_cost);
```

**After:**
```
include "include/resourcemanager";
// ...
var use_on := Target(who);
var resourceRequest;

if (use_on.objtype == OMEGACACHE_OBJTYPE)
    resourceRequest := SelectMaterialFromCache(who);
    if (!resourceRequest) return; endif
elseif (IsIngot(use_on))
    resourceRequest := MakeBackpackRequest(who, use_on.objtype);
else
    SendSysMessage(who, "You can't use that.");
    return;
endif

// Material properties from crafting config (unchanged)
var orediff := smith_cfg[resourceRequest.objtype].Difficulty;
var orename := smith_cfg[resourceRequest.objtype].Name;

// Check availability (replaces use_on.amount >= material)
var available := GetAvailableResource(who, resourceRequest.objtype, resourceRequest.dataFileHandle);
if (available.total < material_cost)
    SendSysMessage(who, "Not enough ingots.");
    return;
endif

// Consume (replaces SubtractAmount(use_on, material_cost))
ConsumeResource(who, resourceRequest, material_cost);
```

For multi-material crafts (carpentry, blacksmithy bone armor), each targeting step produces its own `ResourceRequest`. The player can independently choose cache or physical item for each material.

### Affected Scripts

| Script | Primary Materials | Secondary Materials | Dual Material |
|---|---|---|---|
| `pkg/std/blacksmithy/make_blacksmith_items.src` | Ingots | Bone (bone armor) | Yes |
| `pkg/std/tailoring/make_cloth_items.src` | Hides, Cloth | — | No |
| `pkg/std/carpentry/carpentry.src` | Logs | Ingots, Cloth | Yes |
| `pkg/std/alchemy/alchemy.src` | Reagents | Bottles | No |
| `pkg/std/tinkering/tinkering.src` | Metal/Components | — | No |
| `pkg/std/bowcraft/` | Logs, Shafts, Feathers | — | No |
| `pkg/std/cooking/cooking.src` | Food items | Water, Milk, Cheese | No |
| `pkg/std/inscription/inscription.src` | Blank Scrolls | Gems (recharge) | No |
| `pkg/std/cartography/cartography.src` | Blank Maps | — | No |

**Excluded:** `blacksmithy/blacksmithy.src` (smelting), `blacksmithy/meltdown.src` (recycling) — these produce materials, not consume them for crafting.

### Considerations

- **Unfiltered gump**: The material selection gump shows all categories — it does not filter by craft type. The crafting script validates the selection after the fact. This keeps the gump generic and reusable.
- **Cooking complexity**: Cooking uses a recipe system with multiple ingredients per recipe. Each ingredient goes through the same `ConsumeResource` function — the recipe handler calls it once per ingredient type.
- **Failure material loss**: On crafting failure, all crafts destroy a percentage of materials (75 - skill/2, max 50%). The lost amount is consumed via the same `ConsumeResource` call with the same `ResourceRequest` — if the material came from cache, the loss is debited from the cache.
- **AutoLoop integration**: The `ResourceRequest` set during targeting persists for the entire batch crafting loop. Each iteration calls `GetAvailableResource` to check availability and `ConsumeResource` to debit.
- **Feedback**: When resources are consumed from the cache, inform the player: "Used 50 iron ingots from your Omega Cache."
- **Proximity**: `FindAccessibleContainer` (from Phase 1) handles all access validation — must be inside the house, within range, with required privileges.

### Shared Access Check

All features that interact with the Omega Cache store (direct use, crafting, loadouts) use the same access validation via `FindAccessibleContainer()` in `omegacache.inc`. This was established in Phase 1 and includes `who.multi` house membership check, house serial match, and privilege validation.

---

## Phase 3: Loadout System

### Overview

Inspired by the `.arm` command pattern (`scripts/textcmd/player/arm.src`), loadouts let players define target item quantities for a container and sync it with the Omega Cache in one action. Use cases: restocking a reagent pouch before PvP, topping up a quiver with arrows, returning surplus materials after a crafting session.

### Storage Model: DataFile Per Character

Each character gets a DataFile for their loadout definitions:

```
// Structure: data/ds/omegacache/loadouts_<charserial>.txt
//
// Element key: slot number ("1" through "10")
// Properties:
//   name              = "Reagent Pouch"          (player-assigned name)
//   container_serial  = 12345678                 (target container in backpack)
//   item_<objtype|hash> = 500                    (target qty, keyed same as cabinet store)
//
// Example element "1":
//   name = "PvP Reagents"
//   container_serial = 8042156
//   item_0x0F85|d41d8cd9... = 500    (500 Ginseng — simple item, constant hash)
//   item_0x0F7A|d41d8cd9... = 500    (500 Black Pearl)
//   item_0x0F8C|d41d8cd9... = 300    (300 Sulphurous Ash)
```

**Why DataFile over CProps**: A single loadout can contain dozens of item types with individual quantities. Nested arrays in a CProp become unwieldy and hard to inspect. DataFile gives us named properties per slot, human-readable on disk, and consistent with the cabinet storage model.

### Player Flow

**Creating a loadout** (`.loadout` → gump):
1. Player clicks "Create Loadout"
2. Player targets a container in their backpack (pouch, bag, quiver, etc.)
3. Player names the loadout (text entry)
4. Loadout is created with no items — slot is saved with container serial and name

**Adding items to a loadout** (from gump):
1. Player selects a loadout slot, clicks "Add Item"
2. Targeting cursor opens — player targets an item (in backpack, in the container, anywhere accessible)
3. Script reads the item's objtype (or compound key for complex items), records it with the item's current stack amount as the default target quantity
4. Item appears in the loadout gump with an editable quantity field
5. Player can target additional items — cursor loops until ESC

**Editing a loadout** (from gump):
- All items in the loadout are displayed with their current target quantity in editable text fields
- Player can change any quantity directly in the gump
- Each item row has a "Remove" button to delete it from the loadout
- "Save" button commits all quantity changes at once
- "Change Container" button to re-target a different container

**Applying a loadout** (`.loadout 1` or gump button):
1. Access check: `FindAccessibleContainer(who, array{ADD_TO_SECURE, REMOVE_FROM_SECURE})` — player needs both deposit and withdraw privileges since sync goes both ways
2. Find the loadout's target container by serial in player's backpack
3. Build a key-to-amount map of the container's current contents: for each item in the container, call `BuildItemKey(item)` and sum amounts by key. This matches by full `"objtype|hash"` key, not just objtype — a recolored variant won't be counted toward a default variant's target.
4. For each key in the loadout definition:
   - Compare current amount (from the key map) against target amount
   - If below target: withdraw the deficit from the cabinet DataFile, create items in the container
   - If above target: find matching items in container by key, subtract the surplus, deposit into the cabinet DataFile
5. Respect container weight/item limits — if the container can't hold the full loadout, fill what fits and warn
6. Items in the container that are **not** in the loadout definition are left untouched
7. Report summary: "Loadout 'PvP Reagents' applied: +300 Ginseng, +150 Black Pearl, -50 Sulphurous Ash returned to storage."

**Saving from baseline** (shortcut for initial setup):
1. Player fills a container with exactly the amounts they want
2. Opens loadout gump, creates a new loadout pointing to that container
3. Clicks "Save from Current Contents"
4. Script reads all eligible items in the container, calls `BuildItemKey(item)` for each, records `key -> amount` as the loadout definition
5. This is the fastest way to set up a loadout — arrange once, save, restock forever

### Gump Design

Similar layout to `.arm`:
- List of loadout slots (up to 10), showing: slot number, name, container name, item count
- Radio select for slot operations
- Buttons: Create, Edit, Remove, Apply, Apply All
- **Edit view**: shows all items in the selected loadout with:
  - Item icon + name
  - Editable quantity text field
  - Remove button per row
  - "Add Item" button (opens targeting cursor)
  - "Save from Current Contents" button
  - "Change Container" button
  - "Save" button to commit changes
- Pagination if needed (5 slots per page)
- "Apply All" button runs all loadouts in sequence

### Not Limited to Simple Stackables

Loadouts can reference any item that exists in the cabinet store. The loadout stores the exact same `"objtype|hash"` key the cabinet DataFile uses. The loadout system doesn't care whether the item has CProps or not — it just matches keys between the loadout definition and the cabinet store.

### Lost Container Handling

If a loadout's target container is destroyed, lost, or traded, `SystemFindObjectBySerial()` will fail when applying the loadout. Behaviour:
- Send error: "Loadout 'PvP Reagents' failed: container not found. Use `.loadout` to update the container."
- Do not auto-clear or modify the loadout definition — the player may have simply left the container at home.

### Command Registration

- `.loadout` — opens the loadout management gump
- `.loadout 1` through `.loadout 10` — applies the specified loadout slot
- `.loadout all` — applies all defined loadouts in sequence
- Registered as a player-level command in `config/cmds.cfg`

---

## Key Format and Item Identity

### Compound Keys Using CanStack() Properties + MD5 Hash

The `CanStack()` function (moved to `scripts/include/canstack.inc`) defines exactly which properties make two items "the same." Every item stored in the cabinet uses a key format that captures the full stacking identity: `"objtype|md5hash"`.

**Key generation** (`BuildItemKey(item)`):
1. Collect only properties that are **explicitly set on the item** (differ from itemdesc defaults): color, graphic, quality, flags (newbie, insured, cursed), weight_multiplier_mod, scripts (usescript, equipscript, snoopscript). Compare each against `GetItemDescriptor(item.objtype)` — omit if matching.
2. Collect all CProps (these are always per-instance, never inherited from itemdesc).
3. Sort all collected property names alphabetically.
4. Build a canonical string from only the non-default properties: e.g., `"cprop_itemtype=23"` (a special potion with one CProp override) or `""` (a standard iron ingot with no overrides).
5. Hash it: `MD5Encrypt(canonical_string)` (built-in, `polsys.em`). An empty string produces a constant hash.
6. DataFile element key: `"0x7059|a3f2b8c1d4e5..."` (objtype + "|" + MD5)

Standard items with no overrides all produce the same constant hash (MD5 of empty string). Items with any non-default property produce a different hash. The hash captures **what's special about this item**, not its full state — matching exactly what the element stores.

`BuildDefaultKey(objtype)` is simply `objtype + "|" + MD5Encrypt("")` — no need to read itemdesc at all.

**Why MD5 hash for the key**:
- Fixed-length, consistent key regardless of CProp count or value complexity
- No delimiter escaping issues (CProp values could contain any character)
- Two items with the same hash are guaranteed identical (same sorted inputs = same hash)
- `MD5Encrypt()` is a POL built-in (`polsys.em`) — no external dependencies
- Output is **lowercase hex** (32 chars, e.g., `"098f6bcd4621d373cade4e832627b4f6"`)
- DataFile keys are case-insensitive in POL, and `MD5_Compare()` uses case-insensitive comparison — no case sensitivity risk

### DataFile Element Structure

The hash is only for lookup/identity. The DataFile element stores the objtype, weight, and **only properties that differ from itemdesc defaults**. Properties that match the itemdesc are omitted — the recreated item inherits them naturally from `CreateItemInContainer()`. This ensures cached items behave identically to non-cached items (inheriting from itemdesc, not carrying per-instance overrides).

```
// DataFile element key: "0x7059|a3f2b8c1d4e5..."
//
// Properties (always present):
//   qty = 50
//   objtype = 0x7059
//   weight = 0.25                (per-unit weight via GetItemDescriptor(objtype).Weight)
//
// Properties compared against itemdesc (omitted if matching GetItemDescriptor(objtype)):
//   color = 0x123                (omitted if matches itemdesc Color)
//   graphic = 0x1234             (omitted if matches itemdesc Graphic / objtype)
//   quality = 2.0                (omitted if matches itemdesc Quality)
//   usescript = customscript     (omitted if matches itemdesc Script)
//   equipscript = customequip    (omitted if matches itemdesc EquipScript)
//   snoopscript = customsnoop    (omitted if matches itemdesc SnoopScript)
//
// Properties with fixed defaults (omitted if at default value):
//   newbie = 1                   (default: 0)
//   insured = 1                  (default: 0)
//   cursed = 1                   (default: 0)
//   weight_multiplier_mod = 2    (default: 0)
//
// CProps (always per-instance, stored if present):
//   cprop_itemtype = 23
//   cprop_SpellID = i81
```

**Important**: Both the hash and the element store only non-default values — they are always in sync. On withdrawal:
1. `CreateItemInContainer(container, objtype, amount)` — item inherits all itemdesc defaults
2. Only set properties that are explicitly stored on the element (non-defaults)
3. Restore each CProp from stored `cprop_*` properties

This means a standard iron ingot's element only has `qty`, `objtype`, and `weight` — no overrides needed. A recolored or script-overridden item stores the specific differences.

### Gump Display Strategy

When building the gump, each item row needs: name, icon, color, quantity, and category. Here's how each is resolved from the key format `"objtype|hash"`:

| Data Needed | Source | Requires Reading Element? |
|---|---|---|
| Category (for grouping) | Parse objtype prefix from key → look up in `categories.cfg`. Items not in any category go to "Miscellaneous / Other". | No |
| Item name | Parse objtype prefix → look up in itemdesc for base name. For variants (different hash for same objtype), read element CProps for specifics (e.g., "Invisibility Potion" vs "Special Potion") | Only for variant display |
| Icon/graphic | Parse objtype prefix → look up in itemdesc | No |
| Color/hue | Read from element if stored (non-default), otherwise itemdesc default | Only for non-default color |
| Quantity | `elem.GetProp("qty")` | Yes (always) |

**Deferred loading by category**:

1. `df.Keys()` — get all keys (lightweight, just key strings)
2. Parse objtype prefix from each key → look up category in `categories.cfg` → build `category -> array of keys` mapping. Keys whose objtype is not in any category go to "Miscellaneous / Other".
3. Show category menu with item type counts per category (`len(keys_array)`) — **no element reads yet**
4. Player selects a category → **only then** read `qty` (and CProp display data for variants) for keys in that category

This keeps gump opening cheap. Element reads are deferred to the category the player actually views.

---

## Macro-Friendly Design

All repeatable actions must be executable via text commands with **no gump interaction**. Gumps are for setup and browsing only.

### Commands

| Command | Action | Gump? |
|---|---|---|
| `.cache` | Open the Omega Cache browse/withdraw gump | Yes |
| `.cache deposit` | Deposit all eligible items from backpack + child containers | No |
| `.cache deposit` (with target) | Target a container (deposits all eligible contents) or a single stackable item | Target only |
| `.cache withdraw <amount>` | Target an item of the type to withdraw, then specify amount. Item is used for type identification only (not consumed). If you have no instance of the item to target, use the `.cache` gump instead. Prompt: "Target an item of the type you wish to withdraw." | Target only |
| `.cache list` | List all categories with item type counts and total quantities | No |
| `.cache list <category>` | List items in a specific category with quantities | No |
| `.loadout` | Open loadout management gump | Yes |
| `.loadout 1-10` | Apply a specific loadout | No |
| `.loadout all` | Apply all defined loadouts in sequence | No |

All commands registered as player-level in `config/cmds.cfg`.

**Design principle**: A macro can chain `.loadout all` after recalling home, or `.cache deposit` after a farming run, with zero gump interaction. Commands print clear success/failure messages for macro tools to parse.

**Access check applies to commands too** — all commands require `FindAccessibleContainer()` proximity check.

---

## Implementation Plan

Each phase is broken into milestones that deliver a testable, functional increment. Milestones within a phase are sequential — each builds on the previous. Phases can be delivered independently.

**Tracking**: Tasks are marked with `[ ]` (pending) or `[x]` (complete) as work progresses.

**Logging**: All functions include debug logging wrapped in `if(DEBUG_MODE)` blocks using `SendSysMessage` to the player's in-game output (not `Print()` to server console). Functions that don't have direct access to a player reference use a module-level `_debug_who` variable, set at the start of each operation by the calling script. Debug output uses hue 53 (yellow) for visibility.

**Changelog**: At the end of every milestone, update `pkg/opt/omegacache/changelog.md` with:
- A PR-style summary of what was done in that milestone
- Verification steps for manual functional, integration, and feature testing
- Entries are ordered newest at bottom (append-only)

---

### Phase 1: Core Omega Cache

#### Milestone 1.1 — Foundation
Non-player-facing. Builds the core libraries and item definitions that all subsequent milestones depend on.

| # | Task | Deliverable |
|---|---|---|
| [x] 1 | Move `CanStack()` to `scripts/include/canstack.inc` | Shared include, `packethook.src` updated to use it |
| [x] 2 | Data layer (`omegacache.inc` rewrite) | `BuildItemKey()`, `BuildDefaultKey()`, `OpenHouseStore()`, `DepositItem()`, `WithdrawItem()`, `GetStoredAmount()`, `GetStoredAmountByObjtype()`, `IsStoreEmpty()`, `IsEligibleForStorage()` |
| [x] 3 | Shared access check | `FindAccessibleContainer()` function |
| [x] 4 | Item definitions | Rework `itemdesc.cfg` (Item not Container), add deed definition, verify deed objtype `0xDF0B` confirmed available |
| [x] 5 | Blacklist | `blacklist.cfg` with Gold Ingot and Zulu Coin, format verified |

**Testable**: Data layer functions can be exercised via GM commands or test scripts — create items, build keys, deposit/withdraw from DataFile, verify element contents on disk.

#### Milestone 1.2 — Housing & Placement
Player-facing. Players can place and remove Omega Caches in houses.

| # | Task | Deliverable |
|---|---|---|
| [x] 6 | Housing integration | Extend `AssignDefaultContainers()` + `GetMaxProps()` with `numomegacache`/`maxnumomegacache`, lazy-init check in `sign.src` |
| [x] 7 | House sign display | "Number of Omega Caches: used/max" line in sign gump |
| [x] 8 | Access control | Privilege checks (VIEW_SECURE, ADD_TO_SECURE, REMOVE_FROM_SECURE) integrated into `FindAccessibleContainer()` — completed in Milestone 1.1 |
| [x] 9 | Deed placement script | `placecache.src` — house check, limit check, targeting, cabinet creation with `houseserial` CProp |
| [x] 10 | Destruction/removal | `destroycache.src` (DestroyScript safety net) + "Remove Omega Cache" button in House Management gump |
| [x] 11 | House demolition warning | Yellow system message before existing YesNo, DataFile cleanup after demolition |

**Testable**: GM creates a deed, player places it in a house, house sign shows count, destruction is blocked while items exist (seed test data via GM), demolition warning fires.

#### Milestone 1.3 — Deposit & Withdraw
Player-facing. The core feature is usable — players can store and retrieve items.

| # | Task | Deliverable |
|---|---|---|
| [x] 12 | Deposit — single item | `DepositSingleItem()` — validate, build key, deposit, destroy, report |
| [x] 13 | Deposit — container | `DepositFromContainer()` — enumerate, deposit each, summary report with skip count |
| [x] 14 | Deposit — deposit-all | `DoDepositAll()` — targets backpack, same container logic |
| [x] 15 | Withdrawal — basic | `DoWithdraw()` — create-first-then-debit, per-stack loop, `RecreateItem()` for property restoration |
| [x] 16 | Withdrawal — destination targeting | `PromptDestination()` — target container or ESC for backpack |
| [x] 17 | Withdrawal — weight validation | `GetMaxWithdrawableByWeight()` — full parent chain walk, item slot check |
| [x] 18 | Withdrawal — multi-stack | Stack loop in `DoWithdraw()` using `GetItemDescriptor().StackLimit` |

**Testable**: Deposit items via gump and verify DataFile on disk. Withdraw items and verify they appear in the correct container with correct properties. Test weight limits, multi-stack, partial failures.

#### Milestone 1.4 — Gump & Commands
Player-facing. Full UI and macro support.

| # | Task | Deliverable |
|---|---|---|
| [x] 19 | Gump — category menu | `ShowCategoryMenu()` — dynamic from `categories.cfg` + `df.Keys()`, two-column layout, icons, item counts, "Other" for uncategorized |
| [x] 20 | Gump — item display | `ShowItemList()` — per-category rows with tile icon, name, variant CProps, quantity, text entry, take button |
| [x] 21 | Gump — deposit buttons | "Deposit Item" and "Deposit All" buttons on both category menu and item list |
| [x] 22 | `.cache` command | `scripts/textcmd/player/cache.src` — calls `RunOmegaCacheGump()` directly (inline, not via `start_script`) |
| [x] 23 | `.cache deposit` command | `.cache deposit` (all from backpack) and `.cache deposit target` (targeting loop) |
| [x] 24 | `.cache withdraw` command | `.cache withdraw <amount>` — target item type, withdraw to backpack |

**Testable**: Full player flow — open gump, browse categories, deposit via gump and commands, withdraw via gump and commands. Verify macro-friendliness (`.cache deposit` + `.cache withdraw` with no gump interaction).

#### Milestone 1.5 — Bug Fixes & Polish (2026-03-25)
Stabilisation pass. Bug fixes and enhancements found during testing.

| # | Task | Deliverable |
|---|---|---|
| [x] 25 | `.cache` gump fix | Moved gump code to `.inc` as `RunOmegaCacheGump()`, called inline instead of via `start_script` |
| [x] 26 | Deposit message fix | Use `GetItemDisplayName()` instead of `item.desc` to avoid duplicate stack count |
| [x] 27 | Container counting fix | Switch from `ListItemsNearLocation` to `house.items` for reliable multi-Z-level search |
| [x] 28 | Slot re-credit fix | Single source of truth in `destroycache.src`, removed duplicate in `sign.src` |
| [x] 29 | Non-last container removal | Allow removal of non-last containers when items stored; only block last one |
| [x] 30 | Outside house access block | `who.multi` check + house serial match in `FindAccessibleContainer()` |
| [x] 31 | Duplicate gump prevention | `#omegacache_open` temp CProp guard with timeout |
| [x] 32 | Redeed on removal | Return deed to backpack instead of destroying container outright |
| [x] 33 | Recount GM tool | "Recount Cache Containers" button in House Management (GM-only) |
| [x] 34 | `.nukeserial` admin command | Destroy items by serial with optional cache slot recalculation |
| [ ] 35 | Access control testing | Outside house, through walls, friend permissions (VIEW/ADD/REMOVE), GM bypass |
| [ ] 36 | Redeed on removal | Verify deed returned to backpack on removal, blocked if backpack full |
| [ ] 37 | House demolition with items | Verify warning shown, DataFile cleaned up after demolition |
| [ ] 38 | Two players same cache | Concurrent access — both browsing, one deposits while other withdraws, verify data consistency |
| [ ] 39 | Multiple house types | Verify `who.multi` check works across different house types (small, large, castle, custom multi-houses) |
| [ ] 40 | Orientation selection | TODO: directional graphic placement (East/South) if using chest-style graphic |

**Testable**: Core bug fixes verified. Remaining: access control edge cases, permission matrix, house demolition, concurrent access, multi-house-type validation, orientation selection if graphic changes.

---

### Phase 2: Crafting Integration

#### Milestone 2.1 — Resource Manager & Lease System
| # | Task | Deliverable |
|---|---|---|
| [x] 41 | `resourcemanager.inc` | `GetAvailableResource()`, `ConsumeResource()`, `ConsumeFromBackpack()`, `ConsumeFromCache()`, `MakeBackpackRequest()` — centralised resource lookup and consumption with `ResourceRequest` pattern |
| [x] 42 | `SelectMaterialFromCache()` | Variant-aware material selection gump with colored tile icons — returns `ResourceRequest` struct with key, color, and cache-first source order |
| [x] 43 | `ConsumeFromCache()` | Lease-aware — caps withdrawal to unleased portion per key, excludes caller's own lease |
| [x] 44 | Lease system in `omegacache.inc` | `CreateLease`, `ExtendLease` (validates stock), `ReleaseLease`, `GetLeasedAmount` (cleans expired). Lease key format: `RL#<item_key>\|<serial>_<pid>` |
| [x] 45 | Lease-aware data layer | `GetStoredAmount` and `GetStoredAmountByObjtype` subtract leased amounts (with `exclude_lease_key`). `BuildCategoryMap`, `ShowItemList`, `GetAllStored` filter `RL#` keys. |
| [x] 46 | Lease wrappers in `resourcemanager.inc` | `LeaseResource` (sets `resourceRequest.leaseKey` via byref), `ExtendResourceLease`, `ReleaseResourceLease`. `leaseKey` lives on `ResourceRequest` struct. |
| [x] 47 | `ApplyMaterialProperties` compatibility | Backwards-compatible with `ResourceRequest` struct — reads `.objtype`/`.color` or `.objtype`/`.Color` based on struct type detection |

**Testable**: Call `GetAvailableResource()` and `ConsumeResource()` from test scripts. Verify backpack-first and cache-first consumption. Verify fallback between sources. Verify leases prevent concurrent over-consumption. Verify expired lease cleanup.

#### Milestone 2.2 — Craft Script Modifications
| # | Task | Deliverable |
|---|---|---|
| [x] 48 | Blacksmithy | Full `ResourceRequest` + lease integration. Cache targeting, dual material (ingots + bone), `IsIngotObjtype`/`IsBoneObjtype` helpers. |
| [ ] 49 | Tailoring | Same pattern (hides/cloth). Single material, color applied to product. |
| [ ] 50 | Carpentry | Dual material (logs + ingots/cloth), each with independent `ResourceRequest` and lease. Complex color inheritance. |
| [ ] 51 | Alchemy | Reagents + bottles. Consumption during `CanMake()` uses same centralised function. |
| [ ] 52 | Tinkering | Metal/components + gems. Multiple target steps, color/quality applied. |
| [ ] 53 | Bowcraft | Shafts + feathers. Simple Min() consumption, no AutoLoop. |
| [ ] 54 | Cooking | Multi-ingredient recipe system. `destroy_all_ingredients()` calls `ConsumeResource` per ingredient. |
| [ ] 55 | Inscription | Blank scrolls (loop) + gems (recharge loop). |
| [ ] 56 | Cartography | Blank maps. Asymmetric consumption (failure vs success). |

**Testable**: For each craft: target cache to select material, craft item, verify depletion from cache. Target physical item, craft until backpack depleted, verify fallback to cache. Craft away from cache — verify backpack-only still works. Verify leases created/extended/released correctly during AutoLoop.

#### Milestone 2.3 — Crafting Testing
| # | Task | Deliverable |
|---|---|---|
| [ ] 57 | Mixed source crafting | Craft with some materials in backpack, some in cache. Verify correct depletion order per `ResourceRequest`. |
| [ ] 58 | Dual material crafts | Carpentry/blacksmithy: one material from cache, other from backpack. Verify independent `ResourceRequest` and leases per material. |
| [ ] 59 | Failure material loss | Craft and fail — verify percentage loss debited from correct source based on `ResourceRequest`. |
| [ ] 60 | AutoLoop batch with leases | Batch craft 20+ items. Verify lease extends each iteration, released on exit. Verify another player sees reduced availability during lease. |
| [ ] 61 | Lease expiry | Simulate long idle — verify lease expires, another player can consume. Verify expired lease cleaned up. |
| [ ] 62 | No cache nearby | Craft away from any cache — verify all crafts still work exactly as before (pure backpack, no leases). |
| [ ] 63 | Edge cases | Insufficient total resources, cache with variant items, concurrent crafters on same resource |

---

### Phase 3: Loadout System

#### Milestone 3.1 — Loadout Data Layer
| # | Task | Deliverable |
|---|---|---|
| [ ] 59 | Loadout DataFile | Per-character DataFile at `data/ds/omegacache/loadouts_<charserial>.txt` |
| [ ] 60 | Slot CRUD | Create slot (name, container serial), delete slot, update slot properties |
| [ ] 61 | Item management | Add item to slot (by full key), remove item from slot, update quantity |

**Testable**: Create/edit/delete loadout slots via test scripts, verify DataFile contents on disk.

#### Milestone 3.2 — Loadout Gump
| # | Task | Deliverable |
|---|---|---|
| [ ] 62 | Slot list view | List of 10 slots with name, container name, item count. Radio select, pagination. |
| [ ] 63 | Create flow | Target container, name the loadout, save empty slot |
| [ ] 64 | Edit view | All items in slot with icon, name, editable quantity, remove button per row |
| [ ] 65 | Add item | Targeting cursor, `BuildItemKey()`, record with current amount as default |
| [ ] 66 | Save from baseline | Read all eligible items in container, record `key -> amount` for each |
| [ ] 67 | Change container | Re-target a different container |
| [ ] 68 | Save button | Commit all quantity changes from text fields |

**Testable**: Full gump flow — create loadout, add items, edit quantities, save from baseline, change container.

#### Milestone 3.3 — Loadout Apply
| # | Task | Deliverable |
|---|---|---|
| [ ] 69 | Apply logic | Build key-to-amount map of container, compare against loadout definition, withdraw deficits, deposit surpluses |
| [ ] 70 | Container validation | Weight/item limits, partial fill with warning |
| [ ] 71 | Lost container handling | Error message if container not found |
| [ ] 72 | Summary report | "+300 Ginseng, -50 Sulphurous Ash returned to storage" |

**Testable**: Apply loadout with items above/below/at target. Verify container contents match definition. Test with missing container, full container, weight-limited container.

#### Milestone 3.4 — Loadout Commands & Testing
| # | Task | Deliverable |
|---|---|---|
| [ ] 73 | `.loadout` command | Opens gump, registered as player-level command |
| [ ] 74 | `.loadout 1-10` command | Apply specific slot (no gump) |
| [ ] 75 | `.loadout all` command | Apply all defined slots in sequence |
| [ ] 76 | Integration testing | Apply after farming run, apply after PvP death, apply with empty cache, apply all with mixed slots |
| [ ] 59 | Macro flow testing | `.loadout all` chains, verify clean output messages |

---

## Decisions Log

| Question | Decision |
|---|---|
| Cabinet destruction policy | Block destruction if items are stored |
| House demolition | Same rules as Secure Containers — items are lost, yellow system message warning |
| Multiple cabinets per house | Yes — house defines limit, all cabinets share one storage pool |
| Deposit-all shortcut | Yes — deposits from backpack and all child containers |
| Cabinet storage limit | Unlimited |
| Gump style | Keep current style for now, review later |
| Migration | Not needed — lazy-init in `AssignDefaultContainers`/`GetMaxProps` handles existing houses automatically |
| Placement method | Deed-based (not house add menu), house enforces limit |
| Access control | Reuse Secure Container privileges (VIEW/ADD/REMOVE_FROM_SECURE) |
| Storage scope | Per-house, not per-cabinet |
| Access check | Proximity to a physical cabinet + house privilege check (not just "in a house") — shared across all features |
| Loadout storage | DataFile per character, not CProps |
| Loadout container | Per-loadout, not global |
| Loadout item entry | Target-based (not typing), editable quantities in gump |
| Loadout item scope | Any item in the cabinet store, not limited to simple stackables |
| Loadout slots | Up to 10 per character |
| Loadout container loss | Error message, do not auto-clear — player updates via gump |
| Item eligibility | Blacklist (not whitelist) — any stackable item eligible unless blacklisted |
| Uncategorized items | Displayed under "Miscellaneous / Other" in gump |
| CanStack() location | Move to `scripts/include/canstack.inc` (shared include) |
| Weight storage | Per-unit weight via `GetItemDescriptor(objtype).Weight`, stored as element property |
| Macro support | All repeatable actions via text commands, no gump required |
| Complex items as separate phase | Removed — compound keys and property serialization built into Phase 1 from day one |


## Prompts

```
OK, redo a review of the current implementation, the current shard files and the plan and deliver critique and short comings in our plan or the full implementation as it stands. Check that the plan, implementation and changelog is consistent. Check for incorrect assumptions, logic errors, bugs, type castings etc. Also, look for repeated logic in this module, the modal and the textcmd `.cache` and make sure you don't have additional logic repeating which should be centralized
```

```
Ok, now with Milestone 1.2. Draw up a detailed plan on how you want to implement this and what you want to do here and propose it for approval
```
