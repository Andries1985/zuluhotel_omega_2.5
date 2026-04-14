# Omega Cache - Changelog

> Entries are ordered oldest to newest (append-only).
> Each entry corresponds to a completed milestone from PLAN.md.

## Release Candidate — Phase 1 + Phase 2 (2026-04-11)

**Scope:** Core storage (Phase 1) and crafting integration (Phase 2).
**Base commit:** `662569f2b2f79c9667642f06e7bc45ae931db763`

**Phase 1 — Core Omega Cache:**
- Milestones 1.1-1.5: DataFile storage, housing integration, deposit/withdraw, gump UI, text commands, bug fixes & polish.

**Phase 2 — Crafting Integration:**
- Milestones 2.1-2.3: Resource manager with lease system, all 9 crafting skills integrated (blacksmithy, tinkering, tailoring, carpentry, alchemy, bowcraft/fletching, cooking, inscription, cartography), code review & bug fixes, testing & polish.

**Not in this release:** Phase 3 (Loadout System), Artisan's Hammer of Signus.

**Outstanding test:** #38 (two players concurrent access with lease protection) — requires two active players. Low risk due to POL cooperative multitasking and lease-aware `WithdrawItem`.

---

## Milestone 1.1 — Foundation (2026-03-24)

### Summary

Non-player-facing foundation for the Omega Cache feature. Establishes the core libraries, data layer, access control, and item definitions that all subsequent milestones depend on.

**Files created:**
- `scripts/include/canstack.inc` — Shared `CanStack()` function extracted from packethook, with added usescript/equipscript/snoopscript checks (stricter than POL core)
- `pkg/opt/omegacache/omegacache.inc` — Complete data layer rewrite (replaces 4400-line old include). Contains: `BuildItemKey()`, `BuildDefaultKey()`, `GetNonDefaultProperties()`, `OpenHouseStore()`, `CloseHouseStore()`, `DepositItem()`, `WithdrawItem()`, `GetStoredAmount()`, `GetStoredAmountByObjtype()`, `GetAllStored()`, `IsStoreEmpty()`, `IsEligibleForStorage()`, `FindAccessibleContainer()`
- `pkg/opt/omegacache/blacklist.cfg` — Blacklist with Gold Ingot (0x1BE9) and Zulu Coin (0x3B9A)

**Files modified:**
- `pkg/packethooks/packethook/packethook.src` — Removed inline `CanStack()`, added `include "include/canstack"`
- `pkg/opt/omegacache/itemdesc.cfg` — Rewritten from Container to Item type, removed lock/gump/container properties, added deed definition (0xDF0B)

### Verification Steps

**Functional:**
1. Verify shard starts without errors (itemdesc.cfg and blacklist.cfg parse correctly)
2. Verify `packethook.src` still compiles — `CanStack()` is now included from shared location
3. Verify stack merging still works in-game (drop one stack onto another) — confirms `CanStack()` move didn't break packethook

**Integration:**
4. Create a test item via GM, call `BuildItemKey()` — verify it returns `"0x<objtype>|<32-char-hex>"` format
5. Create two identical items, verify `BuildItemKey()` produces the same key for both
6. Create a standard item and verify `BuildItemKey()` matches `BuildDefaultKey(objtype)` for the same objtype
7. Modify an item's color (GM command), verify `BuildItemKey()` produces a different key
8. Call `OpenHouseStore()` on a test cabinet with `houseserial` CProp — verify DataFile is created in `data/ds/omegacache/`
9. Call `DepositItem()` and verify element appears in DataFile with correct qty, objtype, weight
10. Call `WithdrawItem()` and verify qty decrements, element deletes at 0
11. Call `IsEligibleForStorage()` on a stackable item — returns 1
12. Call `IsEligibleForStorage()` on a non-stackable item — returns 0
13. Call `IsEligibleForStorage()` on a blacklisted objtype (0x1BE9) — returns 0
14. Place an Omega Cache item near a house owner, call `FindAccessibleContainer()` — verify it returns the struct with container, house, df
15. Call `FindAccessibleContainer()` as a non-friend — verify it returns 0

**Feature:**
- No player-facing features in this milestone. All testing is via GM commands or test scripts.

---

## Milestone 1.2 — Housing & Placement (2026-03-24)

### Summary

Player-facing milestone. Players can place and remove Omega Caches in houses via deeds and the House Management gump. House sign displays cache count.

**Files created:**
- `scripts/include/omegacache_utils.inc` — Shared DataFile utilities (`OpenOmegaCacheStore`, `CloseOmegaCacheStore`, `IsOmegaCacheEmpty`, `OMEGACACHE_OBJTYPE`, `OMEGACACHE_DEED_OBJTYPE`)
- `pkg/opt/omegacache/placecache.src` — Deed placement script (house/ownership/limit validation, targeting, creation)
- `pkg/opt/omegacache/destroycache.src` — DestroyScript safety net (blocks if items stored, re-credits slot)

**Files modified:**
- `pkg/opt/omegacache/omegacache.inc` — Refactored to use shared utils (removed duplicated DataFile logic, `OMEGACACHE_OBJTYPE` const, `IsStoreEmpty`, `CloseHouseStore`)
- `pkg/std/housing/sign.src`:
  - Added `include "include/client"` and `include "include/omegacache_utils"`
  - `AssignDefaultContainers()`: Added `numomegacache` for all 37 house types (1-3 based on house size)
  - `GetMaxProps()`: Added `maxnumomegacache` for all 37 house types
  - Lazy-init: Added checks for missing `numomegacache`/`maxnumomegacache`
  - House sign info page: Added "Number of Omega Caches: used/max" display (data[11]/data[12])
  - House Management gump: Added "Remove Omega Cache" button for both player and GM layouts
  - `HouseFunctionRemoveOmegaCache()`: New function — target, validate, empty check, destroy, re-credit
  - Demolition (case 14): Added yellow warning if cache non-empty, DataFile cleanup after demolition

### Verification Steps

**Functional:**
1. Start shard — verify no compile/parse errors from modified files
2. Open house sign on an existing house — verify lazy-init creates `numomegacache`/`maxnumomegacache` properties
3. Verify house sign info page shows "Number of Omega Caches: 0/1" (or similar based on house type)
4. GM creates Omega Cache Deed (`0xDF0B`) — verify item appears with correct name/graphic
5. Player double-clicks deed inside owned house — verify cache is placed, deed destroyed, slot decremented
6. Player double-clicks deed outside a house — verify "You must be inside a house" message
7. Player double-clicks deed in someone else's house — verify ownership rejection
8. Place max caches, try to place another — verify "no more slots" message
9. Open House Management — verify "Remove Omega Cache" button appears
10. Target cache with Remove — verify "Omega Cache removed" and slot re-credited

**Integration:**
11. Seed test data into cache DataFile via GM, try to remove cache — verify "must empty" block
12. Seed test data, try house demolition — verify yellow warning message appears before YesNo
13. Demolish house with cache data — verify DataFile is cleaned up (check data/ds/omegacache/)
14. Verify DestroyScript blocks destruction when items stored (GM tries `.remove` on cache)
15. Verify DestroyScript allows destruction and re-credits slot when cache is empty

**Feature:**
16. Full cycle: place deed → cache appears → sign shows count → remove via House Management → sign updates
17. Multiple caches in one house — verify all share same storage pool (same house_serial DataFile)
18. Verify existing housing features still work (lockdowns, secures, teleporters) — no regressions from layout changes

---

## Milestone 1.3 — Deposit & Withdraw (2026-03-24)

### Summary

Core deposit and withdrawal logic. The feature is functionally complete — players can store and retrieve items. Gump UI comes in Milestone 1.4.

**Files created:**
- `pkg/opt/omegacache/omegacache.src` — Main use-script for cache interaction. Contains: `DepositSingleItem()`, `DepositFromContainer()`, `DoDepositTargeting()`, `DoDepositAll()`, `DoWithdraw()`, `PromptDestination()`

**Files modified:**
- `pkg/opt/omegacache/omegacache.inc` — Added: `RecreateItem()` (restores item from DataFile element properties including color, graphic, quality, flags, scripts, CProps), `ReCreditItem()` (undo debit on failure), `GetMaxWithdrawableByWeight()` (full parent chain weight walk)

### Key Design Decisions

- **Create-first-then-debit**: Items are created in the destination before debiting the DataFile. If creation fails, no debit happened — prevents item loss. The inverse (debit-first) would risk items disappearing if creation fails.
- **Deposit builds item list first**: `DepositFromContainer()` enumerates all eligible items into an array before destroying any, avoiding iteration-during-modification issues.
- **Stack limit per item type**: Uses `GetItemDescriptor(objtype).StackLimit` when available, falls back to 60000.
- **Weight validation walks full parent chain**: `GetMaxWithdrawableByWeight()` checks available weight at every container level up to the root.

### Verification Steps

**Functional:**
1. Double-click a placed Omega Cache — verify "Omega Cache is operational" message (placeholder, gump in 1.4)
2. Call `DepositSingleItem()` with a stackable item — verify item destroyed, DataFile element created with correct qty/objtype/weight
3. Call `DepositSingleItem()` with a non-stackable item — verify "cannot be stored" rejection
4. Call `DepositSingleItem()` with a blacklisted item — verify rejection
5. Call `DepositFromContainer()` on a bag of mixed items — verify eligible items deposited, summary shows correct counts, ineligible items remain with skip count
6. Call `DoDepositAll()` — verify all eligible items from backpack deposited
7. Deposit same item type twice — verify qty accumulates on same DataFile element

**Integration:**
8. Call `DoWithdraw()` for 100 iron ingots — verify item appears in destination with correct objtype/amount
9. Withdraw an item that had non-default color — verify recreated item has the correct color
10. Withdraw an item that had CProps — verify CProps restored on recreated item
11. Withdraw 120,000 of an item (stack_limit=60000) — verify 2 stacks created
12. Withdraw into a container near its weight limit — verify partial withdrawal with message
13. Withdraw into a full container (max_items reached) — verify "container is full" message
14. `PromptDestination()` — target a bag, verify items go there. ESC, verify items go to backpack.
15. Withdraw all of an item — verify DataFile element is deleted

**Feature:**
16. Full deposit-withdraw cycle: deposit 500 iron ingots → verify DataFile → withdraw 200 → verify 300 remain → withdraw 300 → verify element deleted
17. Deposit items with CProps (e.g., special potion) → withdraw → verify CProps intact
18. Deposit a recolored item → withdraw → verify color preserved

---

## Milestone 1.4 — Gump & Commands (2026-03-24)

### Summary

Full player-facing UI and macro command support. The feature is now complete for end-to-end player use.

**Files created:**
- `scripts/textcmd/player/cache.src` — `.cache` command with subcommands: `deposit`, `deposit target`, `list`, `list <category>`, `withdraw <amount>`, and no-arg (open gump)

**Files modified:**
- `pkg/opt/omegacache/omegacache.src` — Full rewrite with gump UI. Uses struct-style gump API (`:gumps:gumps` + `:gumps:gumps_ex`). Contains: `BuildCategoryMap()`, `ShowCategoryMenu()`, `ShowItemList()`, main gump loop with re-send pattern for deferred category loading. Paginated item list with prev/next buttons.
- `pkg/opt/omegacache/omegacache.inc` — Added shared functions: `LoadCategoryLookup()` (centralised category/objtype/icon lookup using integer keys), `GetCategoryObjtypes()`, `GetItemDisplayName()`, `GetObjtypeFromKey()`. Moved deposit/withdraw/prompt functions here from omegacache.src to eliminate duplication with cache.src.

### Key Design Decisions

- **Struct gump API** (`:gumps:gumps` with `GFCreateGump`/`GFSendGump`) — the newer, complete API where all functions take a gump struct by reference. Used over the legacy global-variable API (`:gumps:old-gumps`).
- **Re-send pattern** for category navigation: clicking a category closes the gump and re-sends with that category's items. Only the viewed category's elements are read from DataFile.
- **Pagination** with `GFPage` for item lists exceeding 14 items per page. Prev/Next buttons and page indicator on each page. Back/Deposit buttons on page 0 (visible on all pages).
- **Button ID scheme**: Categories = 100+index, Take buttons = 1000+index, fixed IDs for Deposit Item (1), Deposit All (2), Back (3). Text entry IDs match button IDs for `GFExtractData` lookup.
- **Integer-based objtype comparison**: All category lookups use `CInt()` to avoid `Hex()` format mismatches (uppercase, leading zeros). `LoadCategoryLookup()` stores `CInt(objtype) -> category_name`.
- **No code duplication**: All deposit/withdraw/prompt logic lives in `omegacache.inc`. Both `omegacache.src` (gump) and `cache.src` (command) call the same shared functions.
- **Per-operation privilege checks**: Gump open requires VIEW_SECURE, deposit requires ADD_TO_SECURE, withdrawal requires REMOVE_FROM_SECURE. Checked at each operation, not just at gump open.
- **Config function correction**: Uses `ListConfigElemProps(elem)` for property names from config elements, not `GetConfigStringKeys()` which is for top-level section keys.

### Verification Steps

**Functional:**
1. Double-click placed Omega Cache — verify gump opens with category menu
2. Verify categories show correct item counts from DataFile
3. Verify empty categories are hidden (not shown)
4. Click a category — verify item list shows with tile icons, names, quantities
5. Category with >14 items — verify pagination with Prev/Next buttons and page indicator
6. Enter amount in text field, click Take — verify destination prompt, then items withdrawn
7. Click "Deposit Item" — verify targeting loop starts, items deposited, gump re-opens with updated counts
8. Click "Deposit All" — verify all backpack items deposited, gump re-opens
9. Click "Back" from item list — verify returns to category menu
10. Close gump (right-click) — verify script exits cleanly

**Commands:**
11. `.cache` — verify gump opens (same as double-click)
12. `.cache deposit` — verify all backpack items deposited, no gump
13. `.cache deposit target` — verify targeting loop, no gump
14. `.cache list` — verify category summary with counts
15. `.cache list reagents` — verify item list for specific category
16. `.cache list unknowncategory` — verify error message
17. `.cache withdraw 100` — verify target prompt, then withdrawal to backpack
18. `.cache withdraw` (no amount) — verify usage message
19. `.cache` away from a cache — verify "must be near" message

**Permissions:**
20. Friend with VIEW_SECURE only — verify can open gump, cannot deposit or withdraw
21. Friend with ADD_TO_SECURE — verify can deposit
22. Friend with REMOVE_FROM_SECURE — verify can withdraw
23. `.cache deposit` without ADD_TO_SECURE privilege — verify rejection

**Integration:**
24. Deposit via gump → withdraw via `.cache withdraw` — verify data consistency
25. Deposit via `.cache deposit` → view in gump — verify categories updated
26. Variant items (non-default CProps) — verify variant display in item list shows CProp info
27. Multiple categories with items — verify navigation between categories works
28. `.cache list` → `.cache list <category>` — verify counts match between summary and detail

**Macro:**
29. `.cache deposit` then `.cache withdraw 500` in sequence — verify both work without gump interaction
30. `.cache list` output — verify parseable text format
31. Verify all command output messages are clear text (parseable by macro tools)

---

## Milestone 1.5 — Bug Fixes & Polish (2026-03-25)

### Summary

Stabilisation pass addressing bugs found during testing. Major refactors to gump architecture, access control, container lifecycle, and house management integration.

**Bug fixes:**

1. **`.cache` command failed to open gump** — `start_script` from a textcmd could not send gumps to the player. Fix: moved all gump code (constants, `BuildCategoryMap`, `ShowCategoryMenu`, `ShowItemList`, main loop) into `omegacache.inc` as `RunOmegaCacheGump()`. Both `omegacache.src` (double-click) and `cache.src` (`.cache` command) now call it directly inline, matching the pattern used by `.reags` and `.showcaps`.

2. **Deposit message showed duplicate stack count** — `DepositSingleItem` used `item.desc` which includes the stack amount (e.g., "14 Radiant Nimbus Diamond Ingots"), then prepended `FormatNumber(amt)` again. Fix: switched to `GetItemDisplayName(item.objtype)`.

3. **Container removal blocked by Z-level mismatch** — `ListItemsNearLocation` searched from house sign coordinates (z=3) but containers were at z=49. Both `sign.src` and `destroycache.src` failed to find containers, causing false "must empty" blocks and double slot re-crediting. Fix: switched all container counting to `house.items` iteration which covers all Z levels.

4. **Double slot re-crediting on removal** — Both `sign.src` (HouseFunctionRemoveOmegaCache) and `destroycache.src` (DestroyScript) incremented `numomegacache`. Fix: removed re-credit from `sign.src`; DestroyScript is the single source of truth.

5. **Could not remove non-last containers while items stored** — The empty check blocked removal of ANY container if the shared DataFile had items. Fix: count containers via `house.items`; only block removal of the last one.

6. **Outside house access** — Players could access the cache from outside the house. Fix: added `who.multi` check and house serial match in `FindAccessibleContainer()` (GMs exempt).

7. **Multiple gump instances** — Players could open multiple cache gumps simultaneously. Fix: added `#omegacache_open` temp CProp guard with 10-minute timeout, cleared on gump close.

**Enhancements:**

- **Redeed on removal** — Removing a container via House Management now returns a deed to the player's backpack instead of just destroying it.
- **Recount Omega Caches** — New GM-only button in House Management gump. Counts containers via `house.items`, resets `numomegacache` and `maxnumomegacache` to correct values. Follows the same pattern as "Count Teleporters".
- **`.nukeserial` admin command** — New admin textcmd to destroy items by serial number, with optional `resetcache` flag to recalculate house omega cache slots.
- **Placement orientation TODO** — Added note in `placecache.src` for future directional graphic support (East/South selection like secure containers).

**Files modified:**
- `pkg/opt/omegacache/omegacache.inc` — Added `:gumps:gumps` and `:gumps:gumps_ex` includes. Moved all gump code here: constants, `BuildCategoryMap()`, `ShowCategoryMenu()`, `ShowItemList()`, `RunOmegaCacheGump()`. Fixed `DepositSingleItem` to use `GetItemDisplayName`. Added `who.multi` + house serial match checks in `FindAccessibleContainer()`. Added `#omegacache_open` duplicate gump guard.
- `pkg/opt/omegacache/omegacache.src` — Stripped to minimal: access check, `detach()`, `RunOmegaCacheGump()` call.
- `scripts/textcmd/player/cache.src` — Replaced `start_script` with direct `RunOmegaCacheGump()` call.
- `pkg/opt/omegacache/destroycache.src` — Switched container counting to `house.items`. Only blocks destruction of last container when items stored.
- `pkg/opt/omegacache/placecache.src` — Added TODO note for directional graphic orientation selection.
- `pkg/std/housing/sign.src` — Fixed container counting to use `house.items`. Removed double slot re-credit. Added redeed on removal. Added "Recount Cache Containers" GM button and `RecountOmegaCaches()` function. Renamed menu labels to "Cache Container".

**Files created:**
- `scripts/textcmd/admin/nukeserial.src` — Admin command to destroy items by serial, with omega cache slot recalculation.

---

## Milestone 2.1 — Resource Manager & Lease System (2026-03-25)

### Summary

Centralised resource management and lease system for crafting integration. Creates `scripts/include/resourcemanager.inc` and adds lease functions to `pkg/opt/omegacache/omegacache.inc`. All crafting skills use these to consume materials from both backpack and Omega Cache transparently, with leases preventing concurrent consumption conflicts.

**Files created:**
- `scripts/include/resourcemanager.inc` — Centralised resource lookup, consumption, and lease wrappers with `ResourceRequest` pattern

**Files modified:**
- `pkg/opt/omegacache/omegacache.inc` — Added resource lease system (`CreateLease`, `ExtendLease`, `ReleaseLease`, `GetLeasedAmount`). Made `GetStoredAmount` and `GetStoredAmountByObjtype` lease-aware. Added RL# key filtering to `BuildCategoryMap`, `ShowItemList`, `GetAllStored`.
- `pkg/systems/crafting/include/craftingfunctions.inc` — Made `ApplyMaterialProperties` backwards-compatible with `ResourceRequest` structs (reads `.objtype`/`.color` from struct or `.objtype`/`.Color` from physical item).
- `pkg/std/blacksmithy/make_blacksmith_items.src` — First craft script integration. Full `ResourceRequest` pattern with cache targeting, lease lifecycle in AutoLoop, dual-material bone armor support.

### ResourceRequest Struct

```
struct{
    objtype              // CInt objtype of the material
    key                  // full DataFile key "0xABCD|hash" (0 if backpack-sourced)
    color                // material color for crafting product properties
    preferredSourceOrder // array{ OMEGA_CACHE, BACKPACK } or array{ BACKPACK, OMEGA_CACHE }
    dataFileHandle       // DataFile handle (0 if no cache available)
    leaseKey             // lease key string (0 if no active lease)
}
```

- Built by `MakeBackpackRequest(who, item)` (backpack-first) or `SelectMaterialFromCache(who)` (cache-first via gump)
- `leaseKey` is set by `LeaseResource()` and read by all consumption/availability functions
- Passed `byref` to `LeaseResource` and `ReleaseResourceLease` which mutate `leaseKey`

### Resource Lease System

Leases reserve a specific quantity of a cache resource for a crafting execution, preventing concurrent consumers from depleting each other's materials.

**Lease key format:** `RL#<item_key>|<who.serial>_<GetPid()>`
- Example: `RL#0x1BF2|d41d8cd98f00b204|1073742185_4527`
- Stored as DataFile elements with `quantity` and `expiry` properties
- TTL: 60 seconds default, extended each crafting loop iteration

**Lease lifecycle in crafting:**
1. Before loop: `LeaseResource(who, resourceRequest, material)` — creates lease, sets `resourceRequest.leaseKey`
2. Each iteration: consume from unleased portion, then `ExtendResourceLease(resourceRequest)` — extends TTL if enough stock remains, else deletes lease and breaks loop
3. Loop ends: `ReleaseResourceLease(resourceRequest)` — deletes lease, clears `leaseKey`

**Lease-aware functions:**
- `GetStoredAmount(df, key, exclude_lease_key)` — returns `qty - leased`, optionally excluding caller's own lease
- `GetStoredAmountByObjtype(df, objtype, exclude_lease_key)` — same, across all keys matching objtype
- `GetLeasedAmount(df, item_key, exclude_lease_key)` — sums active leases, cleans expired ones, optionally excludes one
- `ConsumeFromCache` — caps withdrawal to unleased amount per key
- `ExtendLease` — validates item still has enough stock (qty >= total_leased) before extending; deletes lease if insufficient

**Why not ReserveItem:** `ReserveItem` locks an entire physical item or container — too coarse. Leases are quantity-specific, key-specific, TTL-based, and allow multiple concurrent consumers on different materials or quantities.

### Key Design Decisions

- **Variant-aware**: Crafting scripts read `material.Color` to set product color via `ApplyMaterialProperties()`. The `SelectMaterialFromCache` gump shows each variant as a separate row with colored tile icons. `ConsumeFromCache` debits the specific key first, then falls back to other keys matching the objtype prefix.
- **preferredSourceOrder**: An ordered array (`array{ OMEGA_CACHE, BACKPACK }` or `array{ BACKPACK, OMEGA_CACHE }`) that `ConsumeResource` iterates to determine depletion order. Extensible for future sources.
- **No special cases**: Every material type (ingots, bottles, scrolls, reagents, food) goes through the same functions. No "utility" vs "primary" distinction.
- **Own-lease exclusion**: When checking availability or consuming, the caller's own lease is excluded via `exclude_lease_key` so the caller can consume their reserved portion.
- **Expired lease cleanup**: `GetLeasedAmount` opportunistically deletes expired leases during scans.

### Functions

**resourcemanager.inc:**

| Function | Purpose |
|---|---|
| `GetAvailableResource(who, objtype, df, exclude_lease_key)` | Query total available across backpack + cache (lease-aware) |
| `ConsumeResource(who, resourceRequest, amount)` | Consume following `preferredSourceOrder`, excludes own lease |
| `ConsumeFromBackpack(backpack_items, amount)` | Deplete physical items via `SubtractAmount` |
| `ConsumeFromCache(df, key, objtype, amount, exclude_lease_key)` | Debit DataFile qty from unleased portion |
| `MakeBackpackRequest(who, item)` | Build backpack-first `ResourceRequest` from targeted physical item |
| `SelectMaterialFromCache(who)` | Variant-aware selection gump, returns cache-first `ResourceRequest` |
| `LeaseResource(who, byref resourceRequest, amount, ttl)` | Create lease, set `resourceRequest.leaseKey` |
| `ExtendResourceLease(resourceRequest, ttl)` | Extend lease TTL if stock sufficient |
| `ReleaseResourceLease(byref resourceRequest)` | Delete lease, clear `leaseKey` |

**omegacache.inc (new):**

| Function | Purpose |
|---|---|
| `CreateLease(df, item_key, who, quantity, ttl)` | Create lease DataFile element, return lease key |
| `ExtendLease(df, lease_key, ttl)` | Extend TTL if item has sufficient stock, else delete |
| `ReleaseLease(df, lease_key)` | Delete lease element |
| `GetLeasedAmount(df, item_key, exclude_lease_key)` | Sum active leases, clean expired, optionally exclude one |

### Blacksmithy Integration (first craft script)

- **Entry point**: Cache targeting branch added — `SelectMaterialFromCache` for ingots and/or bone
- **`MakeBlacksmithItems(character, ingotRequest)`**: Takes `ResourceRequest`, leases before loop, extends each iteration, releases on exit
- **`MakeBoneItems(character, ingotRequest, boneRequest)`**: Dual `ResourceRequest`, independent leases for ingots and bone
- **`CanMake`/`CanMakeBone`**: Use `GetAvailableResource` instead of `ingots.amount`
- **`ApplyMaterialProperties`**: Receives `ResourceRequest` struct (backwards-compatible)
- **Helper functions**: `IsIngotObjtype(objtype)`, `IsBoneObjtype(objtype)` for cache selection validation

### Tinkering Integration (Milestone 2.2)

Tinkering is the most complex crafting skill — 4 material types (wood, metal, glass, clay), complex multi-component recipes, secondary components (gems, springs, hinges), and special paths (totem, potion keg, traps, lockable).

**Changes to `pkg/std/tinkering/tinkering.src`:**

- **Cache entry point**: Added `OMEGACACHE_OBJTYPE` check before accessibility/movable validation. `SelectMaterialFromCache` opens variant selection gump, then `HandleCacheMaterial` routes by objtype to the correct crafting path.
- **`MakeAndProcessMenu`/`CanMake`**: Now accept `ResourceRequest` instead of physical item. `CanMake` uses `GetAvailableResource` for availability checks.
- **`TryToMakeItem` (main loop)**: Full lease lifecycle — `LeaseResource` before loop, `GetAvailableResource` in condition, `ConsumeResource` for consumption, `ExtendResourceLease` each iteration, `ReleaseResourceLease` on exit. Gem targeting unchanged (physical targeting each loop iteration).
- **New cache-specific functions**:
  - `HandleCacheMaterial` — Routes cache selection to correct path based on objtype (logs→wood menu, ingots→metal menu, axle→complex, obsidian→totem, glass/clay→respective menus, bottles→potion keg)
  - `TryToMakeComplexFromCache` — Complex item crafting (axle+gears, clock, sextant) consuming from cache. Second component auto-resolved from cache/backpack or via second `SelectMaterialFromCache` gump (for axle_and_gears→springs/hinge selection)
  - `MakeTotemFromCache` — Obsidian totem consuming 100 units from cache
  - `TryToMakePotionKegFromCache` — Bottles from cache, other parts (keg/tap/lid) from backpack only (non-stackable)
  - `IsLogObjtype`/`IsIngotObjtype` — Objtype-range checks for routing
- **Unchanged paths**: `SetTrap` (potions not stackable in cache context), `TryToMakeAContainerLockable` (keys not stackable)
- **Gem cache support**: Jewelry crafting (0x1085-0x108a, 0x1535) allows targeting cache for gems — opens `SelectMaterialFromCache` gump per iteration. Falls back to physical `SubtractAmount` for backpack gems.

### Tailoring Integration (Milestone 2.2)

Single material (hides or cloth). Same pattern as blacksmithy.

**Changes to `pkg/std/tailoring/make_cloth_items.src`:**
- Added `include "include/resourcemanager"` and cache entry point
- `MakeAndProcessMenu`/`CanMake` accept ResourceRequest, use `GetAvailableResource`
- `TryToMakeItem` uses full lease lifecycle (LeaseResource/ExtendResourceLease/ReleaseResourceLease)
- Bandage special case uses `GetAvailableResource` for "consume all" amount
- `resource.color`/`resource.objtype` replaced with `resourceRequest.color`/`resourceRequest.objtype`

### Carpentry Integration (Milestone 2.2)

Dual material (logs + optional ingots/cloth). Most complex integration due to global `use_with` pattern.

**Changes to `pkg/std/carpentry/carpentry.src`:**
- Added `include "include/resourcemanager"` and cache entry point with routing for logs/ingots/cloth/young oak
- Global `logRequest` and `secondaryRequest` track ResourceRequests for primary and secondary materials
- `MakeAndProcessMenu` supports cache targeting for the secondary log selection (when ingots/cloth selected first)
- `CanMake` checks `GetAvailableResource` for both primary and secondary
- `TryToCreateItem` uses dual independent leases, `ConsumeResource` for both materials in success/failure paths
- Color inheritance updated for cache-sourced materials
- `MakeYoungOakStaffFromCache` added for cache path
- `IsLogObjtype`/`IsIngotObjtype`/`IsClothObjtype` helpers added

### Alchemy Integration (Milestone 2.2)

Unique consumption pattern — reagents consumed inside `CanMake()` before skill check.

**Changes to `pkg/std/alchemy/alchemy.src`:**
- Separate cache functions: `CanMakeFromCache`, `TryToMakePotionFromCache`, `GetBottleFromCache`, `IsReagent_ObjType`
- Original backpack path completely untouched — cache path is parallel
- `GetBottleFromCache` checks backpack first, then cache for empty bottles
- Lease lifecycle in `TryToMakePotionFromCache` loop

### Bowcraft/Fletching Integration (Milestone 2.2)

Simple dual material (shafts + feathers), no loop.

**Changes to `scripts/items/fletch.src`:**
- Both shafts and feathers can be sourced from cache via `SelectMaterialFromCache`
- `GetAvailableResource` replaces `GetAmount`/`.amount` checks
- `ConsumeResource` replaces `SubtractAmount` for both materials
- No leases needed (single craft, no loop)

### Cooking Integration (Milestone 2.2)

Multi-ingredient recipe system with dictionary-based lookups.

**Changes to `pkg/std/cooking/cooking.src`:**
- Global `cooking_cache_df` set when player targets cache container
- `check_for_all_ingredients`: checks cache via `GetStoredAmountByObjtype` as fallback when backpack insufficient
- `destroy_all_ingredients`: consumes from backpack first, cache remainder via inline `ConsumeResource`
- Recipe filter bypassed when targeting cache (`original_ingredient_objtype == 0`) — shows all available recipes
- Special materials (water, milk, cheese) remain backpack-only

### Inscription Integration (Milestone 2.2)

Blank scroll loop with mana gating.

**Changes to `pkg/std/inscription/inscription.src`:**
- Global `scrollRequest` set when player targets cache and selects blank scrolls
- `CreateScroll` loop uses `GetAvailableResource`/`ConsumeResource` when `scrollRequest` is set
- Lease lifecycle: `LeaseResource` before loop, `ExtendResourceLease` each iteration, `ReleaseResourceLease` on exit
- Enchanting and recharging paths unchanged (gem targeting is physical)

### Cartography Integration (Milestone 2.2)

Blank maps with variable consumption (1 for simple, 10 for complex).

**Changes to `pkg/std/cartography/cartography.src`:**
- Global `mapRequest` set when player targets cache and selects blank maps
- Unified `ConsumeMap(who, blank, amount)` function routes to `ConsumeResource` or `SubtractAmount`
- `makeNewmap` uses `GetAvailableResource` for batch material validation when cache-sourced
- No leases needed (single craft, no loop)

---

## Milestone 2.2 — Code Review & Bug Fixes (2026-03-25)

### Summary

Three-pass code review of the entire crafting integration. Found and fixed critical bugs, structural inconsistencies, and missing fields. No regressions detected in backpack-only crafting paths.

### Critical Bugs Fixed

1. **`GetStoredAmountByObjtype` missing `exclude_lease_key` parameter** — Function accepted only 2 params but `resourcemanager.inc` called it with 3. The caller's own lease was double-subtracted from availability, causing crafting loops to break early. Fixed by adding `exclude_lease_key := 0` default parameter and passing it through to `GetLeasedAmount`.

2. **Inscription missing lease release on early exits** — `CreateScroll` loop created a lease before the loop, but early returns (mana depleted, blank scrolls exhausted) did not release it. Orphaned leases blocked cache resources until TTL expiry (60s). Fixed by adding `ReleaseResourceLease(scrollRequest)` before each early return inside the loop.

3. **Missing `quality` field on ResourceRequest struct** — The struct was created without quality in `MakeBackpackRequest`, `SelectMaterialFromCache`, and 3 inline struct creations (alchemy bottles, tinkering second component, cooking ingredients). Quality from cache items was silently lost. Fixed by adding `quality` field to all struct creation sites.

4. **Missing default color fallback in `SelectMaterialFromCache`** — When an item's color matched the itemdesc default, it wasn't stored in the DataFile. The returned ResourceRequest had `color=0`, causing crafted products to lose their material color (e.g., New Zulu ingots). Fixed by falling back to `GetItemDescriptor(objtype).Color` when no stored color exists.

### Structural Fixes

5. **`ApplyMaterialProperties` type-unsafe quality access** — Accessed `material.quality` (lowercase) which works for ResourceRequest structs but would fail for physical items (which use `.Quality` uppercase). Fixed by extracting `mat_quality` with case-correct field access based on struct type detection, matching the existing `mat_color` pattern.

6. **Carpentry `MakeAndProcessMenu` byref corruption** — When ingots/cloth selected from cache first, the `cacheRequest` struct was passed byref as `use_on`, then overwritten with a physical item inside the function. Fixed by passing a `cache_use_on` copy variable instead.

7. **Tinkering complex items cache key=0** — `TryToMakeComplexFromCache` built a second component ResourceRequest with `key := 0` for cache-sourced items, forcing `ConsumeFromCache` to use prefix scan (could consume wrong variant). Fixed by scanning DataFile keys to find the actual matching key.

8. **Quality not read from cache items** — All crafting entry points read quality from skill config only, ignoring quality stored on cache items. Fixed by checking `resourceRequest.quality` first (from DataFile), falling back to config when 0. Applied to tinkering, tailoring, carpentry entry points and `ApplyMaterialProperties`.

### Minor Fixes

9. **Explicit `include "include/omegacache_utils"` added** — All 8 crafting scripts relied on transitive include for `OMEGACACHE_OBJTYPE`. Added explicit includes to: tinkering, tailoring, carpentry, alchemy, bowcraft, cooking, inscription, cartography.

10. **Tinkering gem cache support added** — Jewelry crafting gem targeting now supports targeting the cache container, opening `SelectMaterialFromCache` to select gem variant. Uses `ConsumeResource` for cache gems, `SubtractAmount` for backpack gems.

11. **Gold ingot color in tinkering complex path** — `TryToMakeComplexFromCache` didn't check for `UOBJ_GOLD_INGOT` special case. Fixed with explicit `GOLD_COLOR` assignment.

12. **Dead code removed** — `GetRessourceName`/`GetresourceName` functions removed from tinkering, tailoring, and carpentry (no callers after conversion to `ConsumeResource`).

### Confirmed Not Bugs (Investigated)

- **Carpentry dual-material cache flow**: `MakeAndProcessMenu` DOES prompt for logs when ingots/cloth selected first (line 693-695). Both materials can come from cache. Not a bug.
- **Tinkering builder mark in complex path**: Backpack path has `ToggleBuildMark` check commented out (always sets CraftedBy). Cache path matches. Not a bug.
- **Lease quantity not tracking consumption**: By design — lease reserves fixed per-iteration cost. `ExtendLease` validates stock sufficiency each iteration, catching depletion. Correct for fixed-cost-per-iteration crafting.
- **Container destroyed during crafting**: DataFile handle remains valid (keyed by house serial, not container serial). Container is just an access point. Not a practical issue.
- **Cooking `create_extra_returns` loop condition**: Pre-existing bug (`==` vs `<=`), not introduced by Omega Cache changes.

### Files Modified

- `pkg/opt/omegacache/omegacache.inc` — Added `exclude_lease_key` to `GetStoredAmountByObjtype`
- `scripts/include/resourcemanager.inc` — Added `quality` field to ResourceRequest in `MakeBackpackRequest` and `SelectMaterialFromCache`. Added default color fallback via `GetItemDescriptor`.
- `pkg/systems/crafting/include/craftingfunctions.inc` — Added `mat_quality` extraction with type-safe field access. Quality fallback from ResourceRequest to config.
- `pkg/std/tinkering/tinkering.src` — Quality/color/gem cache fixes. Removed dead code. Added explicit include.
- `pkg/std/tailoring/make_cloth_items.src` — Quality fallback. Removed dead code. Added explicit include.
- `pkg/std/carpentry/carpentry.src` — Byref fix, quality fallback. Removed dead code. Added explicit include.
- `pkg/std/alchemy/alchemy.src` — Quality field on inline struct. Added explicit include.
- `pkg/std/cooking/cooking.src` — Quality field on inline struct. Added explicit include.
- `pkg/std/inscription/inscription.src` — Lease release on early exits. Added explicit include.
- `pkg/std/cartography/cartography.src` — Added explicit include.
- `scripts/items/fletch.src` — Added explicit include.

### Lessons Learned

- **Struct fields must be consistent**: When adding a field to a struct (like `quality`), ALL creation sites must be updated — including inline structs in crafting scripts, not just the builders in `resourcemanager.inc`.
- **Default property fallback is essential**: Items deposited with default properties (color, quality) don't store those properties in the DataFile. The ResourceRequest must fall back to `GetItemDescriptor` or skill config to avoid losing material properties on crafted products.
- **Lease release on ALL exit paths**: Any function that creates a lease must release it on every possible exit — including error returns, mana checks, and loop breaks. A lease cleanup audit should be part of every crafting integration.
- **byref parameters with struct types**: Passing a struct byref to a function that reassigns the parameter will corrupt the original struct. Pass a copy when the function may overwrite the parameter.
- **Review agents produce false positives**: Automated review found 4 false positives out of 12 issues in the third pass. Manual verification of each finding is essential before acting on it.

---

## Milestone 2.3 — Testing & Bug Fixes (2026-03-27)

### Summary

Live testing of blacksmithy crafting integration revealed multiple bugs across the lease system, resource consumption, and concurrent access. All crafting scripts updated with fixes.

### Critical Bugs Fixed

1. **`.+` operator doesn't overwrite existing struct members** — `LeaseResource` used `resourceRequest.+leaseKey := lease_key` which silently failed because `leaseKey` already existed on the struct (initialized to `0` by `SelectMaterialFromCache`). Lease key was always `0`, preventing lease creation, extension, and release. Fixed by using plain `.leaseKey` assignment. This is an eScript language quirk: `.+` adds NEW members only; use `.` for existing members.

2. **`ExtendResourceLease` broke backpack-only loops** — Returned `0` when `leaseKey` was `0` (no lease). All crafting loops had `if(!ExtendResourceLease(...)) break;` which aborted after 1 iteration on backpack path. Fixed by returning `1` (success) when no lease exists — backpack-only path doesn't need a lease.

3. **`ReserveItem` on cache container blocked concurrent access** — `ReserveItem(use_on)` was called before the `OMEGACACHE_OBJTYPE` check in blacksmithy. When one player targeted the cache, it locked the container item, preventing other players from using it. Fixed by moving the cache check before `ReserveItem` in all crafting scripts.

4. **Crafting exploit: resource depletion bypass** — `ConsumeResource` was called after item creation without checking the return value. A player could open the crafting menu (which snapshot-checks availability), remove materials from backpack, then craft — consuming less than the full material cost. Fixed by adding `GetAvailableResource` re-check inside the loop body before the skill check/item creation in all crafting scripts.

5. **`MD5Encrypt("")` returns error in POL** — `BuildDefaultKey` and `BuildItemKey` passed empty strings to `MD5Encrypt` for items with no non-default properties. POL rejects empty strings, producing `error{ errortext = "String is empty" }` as part of the key. Fixed by using `" "` (space) as sentinel for "no properties". Existing data unaffected (all deposited items had `weight_multiplier_mod=1` as a non-default property).

6. **`LeaseResource` created leases for backpack-only requests** — `MakeBackpackRequest` sets `key := 0` but `dataFileHandle` was set (nearby cache found). `LeaseResource` tried to lease with `key=0`, fell through to `BuildDefaultKey` which failed (see #5). Fixed by returning early from `LeaseResource` when `key=0` — backpack-first requests don't need cache leases.

### Features Added

7. **`.cache dump` GM command** — Raw DataFile dump showing all keys, properties, and lease status. Restricted to `cmdlevel >= 4`. Essential for debugging lease accumulation and data integrity.

8. **`.cache autodraw` toggle** — Player command to disable automatic cache fallback when crafting from backpack. Stores `omegacache_no_autodraw` CProp on the character. When set, `MakeBackpackRequest` skips cache lookup entirely. Targeting the cache container directly still works.

9. **Expired lease cleanup in `BuildCategoryMap`** — Every gump open now sweeps all `RL#` keys and deletes expired leases. Catches orphaned leases from crashed scripts, server restarts, or broken key formats.

10. **Lease key filtering in `.cache list`** — Both `DoListCategories` and `DoListCategory` now skip `RL#` keys, preventing lease entries from appearing as stored items.

### Refactoring

11. **`GetAvailableResource` signature simplified** — Changed from `(who, objtype, dataFileHandle, exclude_lease_key)` to `(who, resourceRequest)`. All 30 call sites across 8 crafting scripts updated. Eliminates verbose field extraction at every call site.

12. **Availability check before create** — All crafting scripts with loops now re-check `GetAvailableResource` at the top of each iteration, before the skill check. If insufficient, sends a message and breaks. `ConsumeResource` remains after item creation (avoids consuming on failed `CreateItemInBackpack`).

### Debug Noise Removed

- `IsEligibleForStorage` rejection messages (fired for every non-stackable item during deposit-all)
- `FindAccessibleContainer: no accessible cache found` (fired on every `GetAvailableResource` call when out of range)

### Files Modified

- `scripts/include/resourcemanager.inc` — `GetAvailableResource` signature, `LeaseResource` `.+` fix and early return, `ExtendResourceLease` no-lease success, autodraw check in `MakeBackpackRequest`
- `pkg/opt/omegacache/omegacache.inc` — `BuildItemKey`/`BuildDefaultKey` empty string fix, `BuildCategoryMap` lease cleanup, debug spam removal
- `scripts/textcmd/player/cache.src` — `.cache dump`, `.cache autodraw`, lease key filtering in list commands
- `pkg/std/blacksmithy/make_blacksmith_items.src` — ReserveItem skip, availability check, `_debug_who` for debugging
- `pkg/std/tailoring/make_cloth_items.src` — ReserveItem skip, availability check, `GetAvailableResource` signature update
- `pkg/std/carpentry/carpentry.src` — Availability check, `GetAvailableResource` signature update
- `pkg/std/tinkering/tinkering.src` — ReserveItem guard, availability check, `GetAvailableResource` signature update
- `pkg/std/alchemy/alchemy.src` — ReserveItem guard, availability check, `ConsumeResource` reorder in cache path, `GetAvailableResource` signature update
- `pkg/std/inscription/inscription.src` — `GetAvailableResource` signature update
- `pkg/std/cartography/cartography.src` — `GetAvailableResource` signature update
- `scripts/items/fletch.src` — `GetAvailableResource` signature update

### Lessons Learned

- **eScript `.+` operator**: Only adds NEW struct members. If the member already exists (e.g., initialized to `0` in a struct literal), `.+` silently does nothing. Always use plain `.` for assignment to existing members.
- **No-op functions must return success, not failure**: When a function's purpose is optional (e.g., extending a lease that doesn't exist), returning failure causes callers to abort. Return success for "nothing to do" cases.
- **`ReserveItem` on shared furniture**: Physical items used as access points (like the cache container) must NOT be reserved — it blocks concurrent users. Use data-level locking (leases) instead.
- **Verify consumption before creation, but consume after**: Check availability before the skill check to prevent exploits, but consume after item creation to avoid material loss on failed creation.
- **`ReadGameClock` may not be monotonic across restarts**: Lease expiry values can become "in the future" after server restart if the clock resets. The `BuildCategoryMap` sweep handles this by cleaning expired leases on gump open, but leases with future expiry values persist until their host item is accessed.

### Known Issues

- **Observed once, unreproducible: stale `#omegacache_open` CProp**: Player denied access, then granted access, could not open cache ("You already have the Omega Cache open"). The `#omegacache_open` CProp has a 600-second TTL safety net. Access denial happens before `RunOmegaCacheGump` and should never set the CProp. May have been a script crash between set (line 1473) and erase (line 1594). Self-resolves after 10 minutes.
- **Lease created on last loop iteration**: `ExtendResourceLease` creates a lease for the "next" iteration at the end of each loop. On the final iteration, this lease is immediately released when the while condition fails. No practical impact (lease lives for a fraction of a second) but is wasteful. Fix would require peeking at `AutoLoop_more()` remaining count without consuming it.
- **Pre-existing: Missing itemdesc.cfg entries entirely**: Some items (e.g., Cloth `0x1765`, Axle `0x105b`, Springs `0x105d`, Clock Parts `0x104f`) have no `itemdesc.cfg` entry at all. The cache gump shows the hex objtype (e.g., "0x1765") instead of a name. These items cannot be `.create`'d either. Not caused by Omega Cache.
- **Pre-existing: Missing `Desc` fields in itemdesc.cfg**: Some items (EmptyBottle, Blankscroll, Axleandgears, etc.) have only a `Name` field in `itemdesc.cfg` with no `Desc`. The cache gump shows the raw `Name` (e.g., "EmptyBottle") instead of a readable display name (e.g., "an Empty Bottle"). The in-game tooltip uses POL's built-in tile data which has the readable name, but `GetItemDisplayName` can only read `Desc`/`Name` from `itemdesc.cfg`. Fix: add `Desc` fields to affected items. Not caused by Omega Cache.
- **Pre-existing: Missing itemdesc entries for crafting components**: Axle (`0x105b`), Springs (`0x105d`), Clock Parts (`0x104f`), and Young Oak Logs (`0xBA2A`) have no `itemdesc.cfg` entries. These items cannot be created via `.create` or crafting. Tinkering complex item chains and carpentry young oak staff are untestable. Not caused by Omega Cache.
- **DataFiles persist on disk after house demolition**: POL has no `DeleteDataFile` function. Files in `data/ds/omegacache/` accumulate for demolished houses (elements deleted but file remains). The `.N.txt` suffix is POL's versioning — each world save creates a new version number. `UnloadDataFile` may discard unsaved changes (removed `CloseOmegaCacheStore` call after cleanup to ensure deletions persist). A server startup script to scan for orphaned files (house serial no longer exists) would be the cleanest solution. Not a functional issue — empty files are small and harmless.
- **Pre-existing: Cartography shows all map types regardless of materials**: The cartography menu is a static `config/menus.cfg` menu — always shows all map types (local, regional, world, canvas). No `CanMake` filtering by available blank maps. Player can select canvas world map (needs 10) with only 1 blank map — fails at consumption with "not enough materials". Not caused by Omega Cache.
- **Pre-existing: Crafted items with different materials auto-stack incorrectly**: POL core's `CreateItemInBackpack` auto-stacks items by objtype BEFORE the crafting script sets CProps/color/name. Two items of the same objtype but different materials (e.g., Iron Sextant Parts + Lavarock Sextant Parts) merge into one stack, adopting the newer item's properties and destroying the older one. This is not caused by Omega Cache — it's a pre-existing issue in the crafting scripts' use of `CreateItemInBackpack`. Fix would require creating items via `CreateItemAtLocation` (no auto-stack) then moving to container, or setting properties on `product_desc` before creation. Affects tinkering complex items and potentially other crafts.

---

## Milestone 2.3b — Gump Polish & Lease Fixes (2026-03-27)

### Summary

Continued testing and polish of the Omega Cache gump, lease system, category handling, and container placement.

### Lease System Fixes

1. **Autodraw lease key resolution** — `LeaseResource` now resolves cache key via prefix scan when `key=0` (autodraw/backpack-first path). Previously returned without creating a lease, leaving cache resources unprotected during crafting loops.

2. **Late lease creation in `ExtendResourceLease`** — When `ConsumeFromCache` resolves a key during autodraw fallback (first cache consumption), `ExtendResourceLease` now creates a lease on the resolved key for subsequent iterations. Signature changed to `(byref resourceRequest, who := 0, amount := 0, ttl)`.

3. **`ConsumeFromCache` refactored to take `ResourceRequest` byref** — Sets `resourceRequest.key` directly when resolved via prefix scan, eliminating the intermediate `cache_result` struct and copying in the caller.

4. **`ConsumeResource` takes `ResourceRequest` byref** — Allows `ConsumeFromCache` to update `.key` on the struct for the autodraw key resolution flow.

5. **Lease creation validates unleased stock** — Both `LeaseResource` and `ExtendResourceLease` now check `GetStoredAmount(df, key) >= amount` before creating a lease. Prevents leases that would push total leased amount beyond available stock.

### Gump Improvements

6. **Category ordering** — Categories now display in config-defined order (Crafting → Mage → General → Special → Miscellaneous → Other) instead of dictionary key order. Uses `cat_names` array from `LoadCategoryLookup`, with "Other" appended at the end.

7. **Category button fix** — Button IDs now use `display_count` (non-empty categories only) with a `cat_display_order` mapping array. Previously used `cat_index` which included empty categories, causing page 2+ categories to open wrong lists.

8. **Item list alphabetical sorting** — Items within a category are sorted alphabetically by name using string-concat sort (`name + "|||" + key`). Previous struct-based `.sort(1)` did not work correctly in eScript.

9. **SpellID hidden from item names** — Scroll items no longer show `[SpellID=xxx]` suffix in the gump. The SpellID CProp is skipped in the variant display loop.

10. **Category icon colors** — New `IconColors` config section provides hue values for category icons. Book categories (Codex Damnorum, Earth Book, Holy Book, Song Book) now use their actual spellbook graphics and colors from `itemdesc.cfg`.

11. **Withdrawal button layout** — Target column uses blue dot button (2362), backpack column uses golden triangle button (2436/2437). Header row shows target icon (0x0E79) and backpack icon (0x0E75) instead of text.

12. **Item name padding** — Increased spacing between tile icon and item name (x=68 → x=75) to prevent overlap with larger icons like ores.

13. **Category icon padding** — Increased spacing between category tile icon and text (x=55 → x=70) to prevent overlap.

### Container & Placement

14. **Container graphic updated** — `0x0E43` → `0x2DF4` with hue 2032 in `itemdesc.cfg`.

15. **Orientation selection on placement** — `placecache.src` now shows a gump with South (`0x2DF4`) and East (`0x2DF3`) orientation previews. Player selects facing before container is placed. Closing the gump cancels placement.

### Categories Config

16. **Missing item categorizations fixed** — Added `0x1079` (Hide variant) to Hides, `0x1727` (Dates) to Food, `0x0DD6-0x0DD9` (fishing fish) to Food, tinkering components (`0x1051`, `0x1053`, `0x1055`, `0x104E`, `0x1059`, `0x105D`) to Miscellaneous.

17. **"Other" category icon** — Uses `0x0FA7` as fallback icon for uncategorized items.

18. **SpecialItems icon** — Updated to `0x3679`.

### Tinkering Fixes (2026-04-07)

19. **Complex item cache targeting** — Fixed `TryToMakeComplexFromCache` missing `preferredSourceOrder` on temp ResourceRequest struct, causing `GetAvailableResource` to fail silently. Added error message for backpack path when second component missing (was silent return).

20. **Axle+Gears → cache for springs/hinge** — Previously blocked with "Use the cache directly" message. Now opens `SelectMaterialFromCache` and routes to correct complex path based on selected component type.

21. **Lease shortfall calculation** — `LeaseResource` and `ExtendResourceLease` now only lease `max(0, material - backpack_amount)` instead of full material cost. Prevents over-reserving cache stock when player has partial materials in backpack. Lease quantity recalculated each iteration as backpack stock changes.

22. **`RecreateItem` restores display name** — When withdrawing items with a `BaseName` CProp, `RecreateItem` now calls `SetName` to restore the item's display name. Previously only restored the CProp but not the actual name, causing casing mismatches (e.g., "sextant Parts" vs "Sextant Parts") and failed auto-stacking.

23. **BaseName used as display name in gumps** — Both main cache gump and material selection gump now check `cprop_BaseName` before falling back to `GetItemDisplayName`. Items like "Ice Rock Lockpick" show their crafted name instead of "Lockpick [BaseName=Ice Rock Lockpick]". `BaseName` also excluded from variant suffix display alongside `SpellID`.

24. **Item sort uses BaseName** — Both gumps sort by `BaseName` (when present) instead of default itemdesc name. Fixes sort order for crafted variants (e.g., "Ice Rock Lockpick" sorts under I, not L for "Lockpick").

25. **Lettuce categorized** — Added `0x0C70` (Lettuce) to Food category. Was appearing as UNCATEGORIZED in debug output.

26. **Orientation selection uses standard UO menu** — `placecache.src` replaced custom gump with `CreateMenu`/`AddMenuItem`/`SelectMenuItem2`, matching the native orientation menu used by chairs and other furniture.

27. **Material selection category pagination** — `SelectMaterialCategory` now paginates with 12 categories per page (was unbounded, overflowing on 22+ categories).

28. **Material selection category ordering** — Uses config-defined order from `LoadCategoryLookup` instead of dictionary key order.

### Tinkering Full Cache Model (2026-04-07)

29. **All `SubtractAmount` converted to `ConsumeResource`** — Gem jewelry, trap items (2x consumption), potion keg bottles, obsidian golem. All now use `MakeBackpackRequest` + `ConsumeResource` with autodraw cache fallback.

30. **All `.amount` checks converted to `GetAvailableResource`** — Potion keg bottle availability checks (both entry paths) now count backpack + cache stock.

31. **`TryToMakeComplex` cache fallback for second component** — When the player targets a backpack component (e.g., gears) but the second component (e.g., axle) is only in cache, the function now falls through to `TryToMakeComplexFromCache` instead of failing silently. Availability check via `GetAvailableResource` before crafting.

32. **`TryToMakeComplex` uses `ConsumeResource` for both components** — Replaced `DestroyItem(use_on)` and `DestroyItem(have_it)` with `ConsumeResource` calls. Color inheritance uses `firstRequest.color` with gold ingot special case.

33. **`MakeTotem` cache-aware** — `GetAvailableResource` for obsidian check (was `it.amount`), `ConsumeResource` for consumption (was `SubtractAmount`). Autodraw enabled — 20 obsidian in backpack + 80 in cache = 100 needed.

34. **`SelectMaterialFromList` sort fix** — Replaced manual substring loop with `Find()` for `|||` separator extraction. Manual loop failed silently, causing empty item lists on page 2+ categories.

35. **`SetTrap` cache targeting** — `SetTrap` now supports targeting the Omega Cache container for trap materials. Opens `SelectMaterialFromCache`, player picks a poison/explosion potion or bolt. Trap type and strength read from config by objtype (`GetItemDescriptor` for graphic, `:alchemy:itemdesc` for Strength). Both consumption calls (1 before skill check + 1 on success) use `ConsumeResource` with cache support.

36. **`TryToMakePotionKeg` bottles appended to parts** — When bottles were found and amount-checked via `GetAvailableResource`, they were not appended to the `parts` array for consumption. Fixed by adding `parts.append(have_it)` after the amount check.

37. **Debug logging removed** — Removed investigation-specific debug lines from `make_blacksmith_items.src` (`_debug_who`, `After LeaseResource`, `Before ReleaseResourceLease`) and `resourcemanager.inc` (`SelectMaterialCategory` button mapping debug).

### Files Modified

- `pkg/opt/omegacache/omegacache.inc` — Category ordering, button mapping, icon colors, item sorting with BaseName, SpellID/BaseName filter, withdrawal button layout, padding fixes, `RecreateItem` SetName, lease cleanup debug
- `pkg/opt/omegacache/categories.cfg` — Missing items (lettuce, dates, fish, hides, gourds), IconColors section, updated book graphics
- `pkg/opt/omegacache/itemdesc.cfg` — Container graphic `0x2DF4`, color 2032
- `pkg/opt/omegacache/placecache.src` — Standard UO orientation menu
- `scripts/include/resourcemanager.inc` — `ConsumeFromCache` byref refactor, `ConsumeResource` byref, `LeaseResource` shortfall calculation with backpack enumeration, `ExtendResourceLease` shortfall recalculation and late creation, unleased stock validation, item sort with BaseName and `Find()` fix, category pagination and ordering
- `pkg/std/blacksmithy/make_blacksmith_items.src` — `ExtendResourceLease` call site updated with who/amount, `orename` fallback to `GetItemDisplayName`
- `pkg/std/blacksmithy/blacksmithy.cfg` — Added `Name Iron` for iron ingot entry
- `pkg/std/tinkering/tinkering.src` — Full cache model: all `SubtractAmount`/`.amount`/`DestroyItem` on materials converted to `ConsumeResource`/`GetAvailableResource`. Complex item cache fallback for second component. Obsidian golem, gem jewelry, traps, potion keg bottles all cache-aware.
- `scripts/items/fletch.src` — Removed cache branch for shafts (always backpack, double-clicked). Shafts consumed via direct `SubtractAmount` on targeted stack. Feathers support cache via `SelectMaterialFromCache` or autodraw fallback via `GetAvailableResource`. Fixed `SelectMenuItem2` case sensitivity ("fletching" → "Fletching"). Fixed stack overflow when `num_to_make` exceeded 60000 (cache total). Capped to 60000.
- `scripts/items/bladed.src` — Full cache integration for bowcraft/knife. Cache entry point at `use_blade` targeting. New functions: `CarveLogsFromCache` (shafts/kindling create-all capped at 60k, bows/crossbows with lease loop), `CanMakeFromCache` (availability via `GetAvailableResource`), `MakeArrowsFromCache` (fire/ice/thunder arrows from cache, both arrows and reagents from cache, capped at 60k). Special bows (fire/ice/thunder) consume reagents from backpack (non-stackable hides). All `SubtractAmount` replaced with `ConsumeResource`.
- All 5 other crafting scripts — `ExtendResourceLease` call sites updated with who/amount

### Bulk Amount Prompt (2026-04-08)

38. **`PromptBulkAmount` centralised function** — Added to `resourcemanager.inc`. Prompts player for desired amount when creating bulk items (shafts, kindling, arrows, bandages, etc). Only shows gump when cache is involved (`resourceRequest.dataFileHandle` is set). Backpack-only paths return max available capped to 60k silently (preserving original behaviour). Skips gump when max=1. Retries with error message when input exceeds maximum.

39. **Fire/Ice/Thunder bow loop fix** — When reagent sourced from cache, `special` was unset (0), causing the `if(special and special.amount > 9)` loop guard to fail and abort crafting immediately. Fixed by replacing `special` checks with `specialRequest` + `GetAvailableResource` which covers both backpack and cache. Renamed `special` → `specialBackpackItem` to clarify it's only the physical backpack lock item. `makespecial` used for all logic checks instead.

40. **`WithdrawItem` lease-aware** — `WithdrawItem` now enforces lease protection at the data layer. Caps withdrawal to `raw_qty - total_leased + own_lease`. Accepts optional `exclude_lease_key` parameter (4th param, defaults to 0). `ConsumeFromCache` passes caller's lease key through. Manual gump withdrawals (no lease) respect all active leases, preventing players from withdrawing into another player's crafting reservation. Previously only relied on upstream checks in `ConsumeFromCache`.

41. **Fletching shafts autodraw** — `fletch.src` now supports cache autodraw for shafts (was backpack-only). `shaft_count` uses `GetAvailableResource` instead of `shafts.amount`. `SubtractAmount(shafts, ...)` replaced with `ConsumeResource(character, shaftRequest, ...)`. `PromptBulkAmount` shows if either shafts or feathers involve cache.

42. **Scissors reverted** — `scissors.src` left as original backpack-only behaviour. Random bandage multiplier (1-3x cloth) incompatible with exact amount prompting. Capped at 60k.

43. **Replaced hardcoded 60k caps** — All 6 bulk creation locations now use `PromptBulkAmount`:
- `bladed.src` — shafts/kindling creation and special arrows (fire/ice/thunder)
- `fletch.src` — arrows/bolts from fletching
- `make_cloth_items.src` — bandages from sewing kit (prompts for bandage count, multiplies by 4 for cloth)
- `scissors.src` — bandages from scissors on cloth (backpack-only, no gump, replaced 60k overflow loop)
- `grinding.src` — ground items (backpack-only, no gump, adjusts batch consumption)

44. **`PromptBulkAmount` cancel fix** — `SendTextEntryGump` returns 0 on cancel, same as `CInt("abc")` or `CInt("0")`. Fixed by checking raw return before `CInt`: cancel (falsy non-string) returns 0 immediately, "0"/"-1"/text shows retry message, only over-max retries.

45. **Alchemy backpack path retrofitted** — `TryToMakePotion` now accepts optional `regRequest` param. When provided (cache path), uses it; otherwise builds from physical `reg` item via `MakeBackpackRequest`. `CanMake` accepts optional `regRequest` — uses `GetAvailableResource` + `ConsumeResource` instead of `reg.amount` + `SubtractAmount`. `GetBottle` now checks cache as fallback when no bottles in backpack. Lease lifecycle added to `TryToMakePotion` loop. `ReserveItem(reg)` removed for cache path.

46. **Duplicate `FromCache` functions eliminated** — Removed all parallel cache-specific functions that duplicated existing logic. Existing functions retrofitted with optional `resourceRequest` parameter instead:
- **Alchemy**: Deleted `CanMakeFromCache`, `GetBottleFromCache`, `TryToMakePotionFromCache`. Merged into `CanMake`, `GetBottle`, `TryToMakePotion`.
- **Tinkering**: Deleted `MakeTotemFromCache`, `TryToMakePotionKegFromCache`, `TryToMakeComplexFromCache`. Merged into `MakeTotem`, `TryToMakePotionKeg`, `TryToMakeComplex`.
- **Carpentry**: Deleted `MakeYoungOakStaffFromCache`. Merged into `MakeYoungOakStaff`.

### Files Modified

- `scripts/include/resourcemanager.inc` — added `PromptBulkAmount` function, cancel/retry fix
- `scripts/items/bladed.src` — 2 locations replaced with `PromptBulkAmount`, `specialBackpackItem` rename, fire bow cache reagent fix
- `scripts/items/fletch.src` — `PromptBulkAmount`, shafts autodraw via `GetAvailableResource` + `ConsumeResource`
- `pkg/std/tailoring/make_cloth_items.src` — `PromptBulkAmount` for bandages
- `pkg/std/tailoring/scissors.src` — reverted to original with 60k cap
- `pkg/std/cooking/grinding.src` — `PromptBulkAmount` with adjusted batch consumption
- `pkg/std/alchemy/alchemy.src` — `CanMake`/`GetBottle`/`TryToMakePotion` retrofitted with optional cache params. Deleted 3 duplicate functions. Lease lifecycle added.
- `pkg/std/tinkering/tinkering.src` — `MakeTotem`/`TryToMakePotionKeg`/`TryToMakeComplex` retrofitted with optional cache params. Deleted 3 duplicate functions. `use_on.color` guarded for cache path.
- `pkg/std/carpentry/carpentry.src` — `MakeYoungOakStaff` retrofitted. Deleted `MakeYoungOakStaffFromCache`.
- `pkg/opt/omegacache/omegacache.inc` — `WithdrawItem` lease-aware with `exclude_lease_key` param
- `pkg/opt/omegacache/categories.cfg` — Wheat sheaf added to RawFoodAndHerbs. Raw food items moved from Food. Category renamed RawHerbs → RawFoodAndHerbs. Added 37 mage-variant potions (0xFF19-0xFF40) to Potions.

### Bug Fixes (2026-04-10)

47. **`TryToMakePotionKeg` bottle handling** — Bottles were being added to `parts` array AND consumed via `bottleRequest`, causing double-consumption or `DestroyItem` on the physical bottle before `ConsumeResource` could use it. Fixed: bottles never added to `parts` (excluded at entry and in loop). All bottle consumption goes through `bottleRequest` exclusively. Integrity check adjusted to expect `len(parts) - 1` when `bottleRequest` is set.

48. **`TryToMakePotionKeg` cache bottle fallback in loop** — When targeting keg/tap/lid and no bottles in backpack, the loop's cache fallback now sets `bottleRequest` instead of appending a struct to `parts`. Consistent bottle handling across all paths.

49. **Inscription backpack autodraw** — `scrollRequest` was only set when targeting cache. Backpack blank scrolls had no autodraw fallback. Fixed: `scrollRequest := MakeBackpackRequest(character, item)` when targeting blank scrolls from backpack. All paths now use `GetAvailableResource`/`ConsumeResource` via `scrollRequest`.

50. **Alchemy menu autodraw** — `CanMake` in menu building didn't pass `regRequest`, so only backpack reagents counted for recipe availability. With 1 reagent in backpack and 120k in cache, recipes needing 4+ didn't show. Fixed: `backpackRegRequest` built before menu, passed to `CanMake` and `TryToMakePotion`.

51. **`GetBottle` autodraw check** — Inline bottle cache request used `preferredSourceOrder := array{ OMEGA_CACHE }` which bypassed the autodraw toggle check (only checks `[1] == BACKPACK`). Changed to `array{ BACKPACK, OMEGA_CACHE }` so the autodraw toggle is respected.

52. **eScript named args** — `CanMake(user, potion_type, 0, mage, usereg := 0, cacheRequest)` failed: unnamed args cannot follow named args. Fixed by naming all trailing args: `regRequest := cacheRequest`.

### Features (2026-04-11)

53. **Drag-and-drop deposit** — Changed `itemdesc.cfg` from `Item` to `Container` type. Added `OnInsertScript` (`cacheinsert.src`) that intercepts items dropped onto the cache container. Deposits eligible stackable items via `DepositSingleItem`, returns ineligible/non-permitted items to backpack. `Script omegacache` overrides default container open on double-click. `Gump`, `MaxItems`, `MaxSlots` set to prevent use as a real container. Initial `CanInsertScript` approach caused server crash (assertion failure — item in transitional state during drop, `DestroyItem` failed). Switched to `OnInsertScript` with `sleepms(50)` delay.

54. **`DoubleclickRange 4`** — Cache container double-click range increased from default (~2 tiles) to 4, matching `.cache` command range.

55. **`placecache.src` lazy-init** — Cache slots (`numomegacache`/`maxnumomegacache`) now initialized on first deed placement if not yet set. Previously required opening house sign first.

56. **House demolition destroys cache containers** — Explicit `house.items` search for `OMEGACACHE_OBJTYPE` before `DestroyMulti`. Containers were outside `ListItemsNearLocation` range from house sign. `EraseObjProperty("houseserial")` before destroy prevents `DestroyScript` from re-crediting slots on a demolished house.

### Release Audit Fixes (2026-04-11)

57. **Missing `specialRequest` lease in bowcraft** — Cache-sourced fire/ice/thunder bow reagents (SA/SS/BP) had no lease protection in the crafting loop. Added `LeaseResource`/`ExtendResourceLease`/`ReleaseResourceLease` for `specialRequest` matching the `logRequest` pattern.

58. **Duplicate `AutoLoop_finish()` in bowcraft** — `AutoLoop_finish()` called inside the while loop (else branch) AND after the loop. Replaced inner call with `break` to exit loop normally. Single `AutoLoop_finish()` after endwhile.

### Post-RC Security & Data Integrity (2026-04-15)

59. **Deposit target validation** — New centralised `ValidateDepositTarget(who, access, tgt)` function gates all deposit operations. Checks: player is in cache's house, target is accessible and within 2 tiles (via top-level world object for nested items), target is in player's backpack or in the same house, secured container permissions checked via `IsFriend(who, house, REMOVE_FROM_SECURE)` by walking from the target itself up the container chain for `usescript == USESCRIPTID_SECURE_CONTAINER`. The `movable` check was moved to `IsEligibleForStorage` (item-level) so locked-down non-secure containers can be targeted for depositing their contents. Prevents cross-house theft via cache deposits.

60. **`RunOmegaCacheGump` signature refactored** — Changed from `(who, df)` to `(who, access)`. `df` extracted internally. `DoDepositTargeting` and `DoDepositAll` also changed to take `access` instead of `df`. Eliminates redundant `FindAccessibleContainer` calls on deposit actions.

61. **`stacking_ignore.cfg`** — New config file defining CProps excluded from cache key identity: `BackPackXYZ`, `IDed`, `#SecureRemove`, `fromLoot`. Items differing only by these CProps merge into same cache entry. `BaseName` and `foodvalue` deliberately preserved. Ignored CProps stripped on deposit, not restored on withdrawal.

62. **`CanStack` updated** — Added `item.inuse` check and `stacking.cfg` ignored CProps filtering to match POL core's `can_add_to_self()` behaviour. Reads `config/stacking.cfg` `IgnoreCprops` and filters before comparing.

63. **`USESCRIPTID_SECURE_CONTAINER` constant** — Replaced string literal `":housing:securecont"` with constant in `sign.src`, `signcontrol.src`, `ssign.src`. Include added to each file.

64. **Category additions** — ~160 new item mappings: cooking items (variants, bowls, pies, cakes, pizzas, bread, bacon, cheese, sausage, donuts, jerky), raw ingredients (dough, batter, flour, corn), 68 AlchemyPlus potions (`0xFF4E`-`0xFF95`, `0xFFA2`), 8 talisman gems (`0x213F`-`0x2146`), 9 fishing shells, 10 verse book scrolls, candlemaking materials (beeswax, pot of wax, dipping stick), bloody bandages, missing fish variants (`0x09CC`, `0xA370`).

### Files Modified

- `pkg/opt/omegacache/omegacache.inc` — `ValidateDepositTarget()`, `RunOmegaCacheGump(who, access)` signature, deposit function signatures, `GetNonDefaultProperties` reads `stacking_ignore.cfg`
- `pkg/opt/omegacache/omegacache.src` — Updated `RunOmegaCacheGump` call to pass `access`
- `pkg/opt/omegacache/stacking_ignore.cfg` — New file
- `pkg/opt/omegacache/categories.cfg` — ~160 new item mappings
- `pkg/opt/omegacache/cacheinsert.src` — New file. `OnInsertScript` for drag-and-drop deposit.
- `pkg/opt/omegacache/itemdesc.cfg` — `Item` → `Container`, added `Gump`, `MinX/MaxX/MinY/MaxY`, `MaxItems`, `MaxSlots`, `OnInsertScript`, `DoubleclickRange`
- `pkg/opt/omegacache/placecache.src` — Lazy-init for `numomegacache`/`maxnumomegacache`
- `scripts/textcmd/player/cache.src` — Updated deposit/gump calls to pass `access`
- `scripts/include/canstack.inc` — `inuse` check, `stacking.cfg` filtering
- `scripts/items/bladed.src` — Added specialRequest lease lifecycle, fixed duplicate AutoLoop_finish
- `config/stacking.cfg` — Comment referencing `stacking_ignore.cfg`
- `pkg/std/housing/sign.src` — `USESCRIPTID_SECURE_CONTAINER` constant, cache container destruction via `house.items`
- `pkg/std/housing/signcontrol.src` — `USESCRIPTID_SECURE_CONTAINER` constant
- `pkg/opt/statichousing/ssign.src` — `USESCRIPTID_SECURE_CONTAINER` constant
- `pkg/std/cooking/cooking.src` — Autodraw for backpack path. Cache resolved once at entry. Lease lifecycle.
- `pkg/std/cartography/cartography.src` — `MakeBackpackRequest` for autodraw. `makeNewmap` create-before-consume.

