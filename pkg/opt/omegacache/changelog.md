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

