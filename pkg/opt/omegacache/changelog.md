# Omega Cache - Changelog

> Entries are ordered oldest to newest (append-only).
> Each entry corresponds to a completed milestone from PLAN.md.

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

