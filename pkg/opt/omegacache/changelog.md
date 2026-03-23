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

