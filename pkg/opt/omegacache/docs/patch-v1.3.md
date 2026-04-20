# Omega Cache - Patch v1.3

## Summary

This patch addresses issues introduced by the merged talisman crafting feature (`TryToMakeTalisman` in `pkg/std/tinkering/tinkering.src`). The initial implementation diverged from the established single-craft cache pattern in several ways: it bypassed the autodraw fallback when ingots were selected from the backpack, retained redundant `ReserveItem`/`ReleaseItem` calls, carried dead secondary-component validation code, and was missing the Crafting Power Hour half-material discount that all sibling tinkering functions apply. These have been corrected so talisman crafting honours cache autodraw, follows the consume-before-skill-check convention shared with `MakeTotem` and `TryToMakePotionKeg`, and applies the PHC discount consistently.

Alongside the talisman fixes, the wider resource-manager layer was hardened: `ConsumeResource` now detects and logs partial consumption (materials drained but the full request not satisfied — possible under concurrent lease/withdraw races) so support can diagnose rare material-loss reports; a `houseSerial` field is threaded through `ResourceRequest` so partial-consume logs identify which house's cache was involved without requiring expensive on-demand lookups; and `GetBottle` in alchemy was refactored to accept a full parent request instead of a raw `dataFileHandle`, eliminating lossy argument passing at two call sites.

### Talisman Crafting

The talisman crafting flow (`TryToMakeTalisman`) now:

- Honours **cache autodraw** on the manual ingot path. Previously the `ingot.amount < 125` gate rejected crafts when the player had partial ingots in the backpack and the rest in the cache, even though the request would have drawn from both. The check now uses `GetAvailableResource(...).total` and the `{BACKPACK, OMEGA_CACHE}` preference order built by `MakeBackpackRequest`.
- Applies the **Crafting Power Hour** (`PHC` / `#PPHC`) half-material discount. Under PHC the ingot requirement drops from 125 to 63, matching `MakeTotem` (100 → 50 obsidian) and `TryToMakePotionKeg` (10 → 5 bottles).
- **Consumes materials before the skill check** (legacy single-craft pattern from `MakeTotem` and `TryToMakePotionKeg`). This closes the duplication window that previously existed between availability check and consumption — the skill check can no longer yield control to a concurrent crafter after item creation.
- Drops the internal `ReserveItem`/`ReleaseItem` calls on the ingot stack. Matches `TryToMakeComplex` — the main `MakeTinkerItems` program's reservation plus script-end auto-release is sufficient.
- Drops the dead `needed_objtype` secondary-component check. With both current callers passing `0xffa3` (which always equals `use_on.objtype`), the block was unreachable.

### ConsumeResource partial-consume detection

`ConsumeResource` previously returned `0` with a generic "Resource consumption failed unexpectedly." message when the drain loop couldn't satisfy the full request despite the initial availability check passing. No audit trail was produced, and the player message was unhelpful for support. The function now tracks how much was drained from backpack vs. cache, emits a structured `SysLog` entry (player serial/name, objtype, requested amount, unfulfilled remainder, per-source consumed amounts, colour, cache key, house serial), and sends a generic player-facing message directing them to contact staff. Refund is NOT performed at this layer — the fix is diagnostic, not compensatory.

### `houseSerial` on ResourceRequest

The `ResourceRequest` struct now carries `houseSerial` alongside `dataFileHandle`. Both request factories (`MakeBackpackRequest`, `SelectMaterialFromList`/`SelectMaterialFromCache`) set it from `access.house.serial` (null-guarded), and every inline struct site that reaches `ConsumeResource` sets it — either by inheritance from a parent request (bladed, tinkering `secondRequest`/`tempRequest`) or by fresh resolution (tinkering `cacheBottleReq`). `SelectMaterialFromList`'s signature changed from `(who, df, valid_keys)` to `(who, access, valid_keys)` so the house can flow through naturally instead of being threaded as a separate parameter.

### `GetBottle` takes parent request

`GetBottle(conts, user, dataFileHandle)` → `GetBottle(conts, user, parentRequest)`. Callers previously extracted `regRequest.dataFileHandle` at the call site, discarding the rest of the context. They now pass `regRequest` directly. `GetBottle` inherits both `dataFileHandle` and `houseSerial` from the parent when provided, or falls back to a fresh `FindAccessibleContainer` resolution when not.

## Notable Points

### Talisman Crafting

- **Autodraw fix** (`pkg/std/tinkering/tinkering.src`): Replaced `ingot.amount < 125` with `GetAvailableResource(character, ingotRequest).total < material`. The `ingotRequest` is built via `MakeBackpackRequest` (backpack-first, cache fallback) when the player targets a backpack stack, and via `SelectMaterialFromCache` (cache-first) when the player targets the cache container.
- **PHC discount**: `var material := 125; if(GetGlobalProperty("PHC") || GetObjProperty(character, "#PPHC")) material := CInt(Ceil(material/2)); endif`. The talisman base (1 unit) is not halved — matches `TryToMakePotionKeg` where only the stackable material (bottles) is halved, not the single-unit parts.
- **Consume-before-check**: `_Play_Sound` → `ConsumeResource(base, 1)` → `ConsumeResource(ingot, material)` → `sendDiff` → `CheckSkill`. Failed consumption aborts before any item is created.
- **Existing stable / keg / totem flows untouched**: changes are isolated to `TryToMakeTalisman`.

### ConsumeResource

- **Partial-consume detection**: tracks `consumed_backpack` and `consumed_cache` via before/after snapshots of `remaining` in the drain loop. If `remaining > 0` after all sources are drained, logs the discrepancy.
- **SysLog format**: `[ConsumeResource] PARTIAL_CONSUME player_serial=... player_name=... objtype=... requested=... unfulfilled=... consumed_backpack=... consumed_cache=... color=... key=... house_serial=...`
- **Defensive read**: `houseSerial` is wrapped in `if(resourceRequest.houseSerial)` so a future inline struct that omits the field will log `house_serial=0` rather than breaking the log path.
- **Player message**: generic — `"A crafting error occurred and some of your materials could not be reclaimed. Please contact staff."` No objtype hex or internal IDs leak into the UX.
- **Not a refund**: materials drained before the failure stay consumed. Refund semantics are out of scope for this patch.

### `houseSerial` plumbing

- **Factories**: `MakeBackpackRequest` resolves from `access.house.serial`; `SelectMaterialFromList` takes full `access` struct and extracts both `df` and `houseSerial`.
- **Null-guards**: `if(access.house)` before `.serial` access — handles the GM-outside-house edge case without crashing.
- **Inline structs with `houseSerial` inheritance**: `tinkering.src` `tempRequest`/`secondRequest` (from `firstRequest`), `bladed.src` `cacheHideReq` (from `logRequest`).
- **Inline structs with fresh resolution**: `tinkering.src` `cacheBottleReq` (grabs from freshly resolved `access`).
- **Module-level tracking**: `cooking.src` carries `cooking_cache_house_serial` alongside `cooking_cache_df` across the three cache-resolution paths.
- **`houseSerial := 0` placeholders**: `bladed.src` `hideReq` (availability-only, doesn't reach `ConsumeResource`) has `houseSerial := 0` for struct shape consistency.

### `GetBottle` refactor

- **Signature**: `GetBottle(conts, user, parentRequest := 0)` — parent request replaces raw `dataFileHandle`.
- **Callers updated**: both sites in `TryToMakePotion` now pass `regRequest` directly (was `regRequest.dataFileHandle` and the temporary `bottleDf` variable).
- **Fallback preserved**: when `parentRequest` is 0 or lacks `dataFileHandle`, GetBottle resolves its own `access` via `FindAccessibleContainer`.

---

## Acceptance Testing Criteria

### Talisman Crafting

| Area | What to Test |
|------|-------------|
| **Backpack only** | Have 125+ ingots in backpack, 0 in cache. Tinker tool → talisman base → target backpack ingots. Verify talisman created, 125 ingots consumed from backpack. |
| **Cache only (autodraw off)** | 0 ingots in backpack, 125+ in cache, autodraw OFF. Target backpack ingots — should reject "You need at least 125 ingots" (autodraw disabled). |
| **Cache only (autodraw on)** | 0 ingots in backpack, 125+ in cache, autodraw ON. Target backpack ingots — reject (no ingots in backpack to target). Target cache — material gump opens, select ingots, talisman created, 125 consumed from cache. |
| **Split (autodraw on)** | 50 ingots in backpack, 100 in cache (total 150), autodraw ON. Target backpack ingots. Verify craft succeeds, 50 consumed from backpack + 75 from cache (= 125 total). **This previously failed with "You need at least 125 ingots".** |
| **Insufficient** | 50 ingots in backpack, 50 in cache, autodraw ON. Verify "You need at least 125 ingots" rejection. |
| **PHC active** | Activate Crafting Power Hour. Verify talisman requires only 63 ingots (was 125). Verify error message says "You need at least 63 ingots". |
| **PPHC active** | Activate personal `#PPHC`. Verify material drops to 63 for that player only. |
| **Skill failure** | With low tinkering skill, attempt talisman. Verify ingots are consumed (full loss) and no talisman is created. Matches legacy single-craft behaviour. |
| **Concurrent crafter** | Two players near the same cache, both targeting the same ingot stock. Verify no duplicate talismans created — the consume-before-check order means whoever's `ConsumeResource` runs second fails cleanly. |
| **Cache ingot selection** | Double-click tinker tool → target cache → material gump → pick iron ingots. Verify talisman color matches selected ingot type. |
| **Ingot colour propagation** | Use a non-iron ingot type (e.g., dull copper). Verify `theitem.color` matches the ingot's cache stored colour. |
| **Exceptional craft** | With high skill and crafter class, achieve an exceptional talisman. Verify quality boost, "CraftedBy" property, and "Exceptional" prefix in name. |

### ConsumeResource partial-consume

| Area | What to Test |
|------|-------------|
| **Normal consume** | Craft anything that uses `ConsumeResource`. Verify no `PARTIAL_CONSUME` log entries during normal operation. |
| **Forced partial (GM)** | As GM, manually drain cache stock between a player's `_Play_Sound` and `ConsumeResource` call (hard to time — alternative: use a test script). Verify log entry appears in syslog with correct player_name, objtype, amounts, and house_serial. |
| **Player message** | Under a partial-consume scenario, verify the player sees the generic "contact staff" message and NOT any technical details (no hex objtypes, no serials). |
| **house_serial in log** | Verify log line includes `house_serial=<serial>` with a non-zero value for a craft sourced from a cache. |
| **Defensive guard** | If a future inline struct omits `houseSerial` and reaches a partial-consume path, the log should emit `house_serial=0` rather than fail. (No direct test; audit future code additions.) |

### `GetBottle` / alchemy

| Area | What to Test |
|------|-------------|
| **Backpack bottles** | Craft a potion with bottles in backpack. Verify backpack bottle consumed first, craft succeeds. |
| **Cache bottles (reagent from cache)** | Reagents from cache, bottles in cache. Craft a potion. Verify bottle consumed from cache using same cache as reagents. |
| **Cache bottles (reagent from backpack)** | Reagents in backpack (parent request has `dataFileHandle` via autodraw), no bottles in backpack, bottles in cache. Verify autodraw pulls a bottle from cache. |
| **No bottles anywhere** | Empty backpack, empty cache of bottles. Craft potion. Verify graceful failure message, potion stored in mortar contents. |
| **Parent-request inheritance** | Trigger partial-consume on a bottle `ConsumeResource`. Verify syslog line has the correct `house_serial` (inherited from the parent reagent request). |

### Tinkering — `TryToMakeComplex` regression

Covers structs that now inherit `houseSerial`: `tempRequest`, `secondRequest`.

| Area | What to Test |
|------|-------------|
| **Axle + Gears from backpack** | Use tinker tool on axle in backpack (with gears also in backpack). Verify axle-and-gears created, 1 of each consumed. |
| **Axle + Gears via cache** | Target cache, select axle, then target gears (backpack or cache). Verify craft works, both materials consumed from the correct source. |
| **Clock Frame + Clock Parts** | Same but for clock parts → clock. |
| **Sextant Parts → Sextant** | Target sextant parts. Verify sextant created. |
| **Missing secondary component** | Target axle, have no gears in backpack or cache. Verify "You don't have the required component." |
| **Secondary from cache only** | Axle in backpack, gears only in cache (autodraw on). Verify complex craft succeeds with gears drawn from cache. |
| **Partial-consume logging** | Force a partial consume on the second component. Verify syslog line carries `house_serial` inherited from the first request. |

### Tinkering — `TryToMakePotionKeg` regression

Covers `cacheBottleReq` which now carries `houseSerial`.

| Area | What to Test |
|------|-------------|
| **All parts backpack** | Keg + tap + lid + 10 bottles all in backpack. Verify potion keg created. |
| **Bottles from cache** | Keg + tap + lid in backpack, 0 bottles in backpack, 10+ bottles in cache. Verify keg created, bottles consumed from cache. |
| **Insufficient bottles** | All parts present but fewer than 10 bottles between backpack + cache. Verify "You need at least 10 empty bottles." |
| **Partial-consume logging** | Force a partial consume on bottles. Verify syslog line shows `house_serial` for the cache the bottles came from. |

### Cooking regression

Covers `cacheRequest` inline struct and `cooking_cache_house_serial` tracking across three paths.

| Area | What to Test |
|------|-------------|
| **Backpack ingredient** | Double-click a raw food ingredient in backpack. Verify menu appears, craft a recipe — ingredients consumed from backpack. |
| **Cache targeting** | Double-click cooking tool → target cache. Verify menu appears, select recipe — ingredients consumed from cache. |
| **Autodraw split** | Some ingredients in backpack, some in cache (autodraw on). Verify recipe consumes from both. |
| **Partial-consume logging** | Force a partial consume on an ingredient. Verify syslog shows `house_serial` matching the cache (whether targeted directly or resolved via autodraw or via mainRequest). |

### Bowcraft / Bladed regression

Covers `cacheHideReq` which now inherits `houseSerial` from `logRequest`.

| Area | What to Test |
|------|-------------|
| **Fire bow (backpack hide)** | SA hide in backpack, logs in backpack. Craft fire bow. Verify both consumed from backpack. |
| **Fire bow (cache hide)** | SA hide in cache only, logs in backpack. Verify bow craftable via autodraw, hide consumed from cache. |
| **Ice/Thunder bow** | Same tests for Ice (SS) and Thunder (BP) hides. |
| **Shafts/kindling** | Carve logs — verify shafts/kindling crafted (no hide involved, simpler regression). |
| **Partial-consume logging** | Force a partial consume on a cache-sourced hide. Verify syslog shows `house_serial` (inherited from `logRequest`). |

---

## Minor items for consideration

These were identified during audit and are being tracked but not fixed in this patch:

- **Leading-space artefact in crafted item name** — If `blacksmithy.cfg` has no `Name` entry for the selected ingot objtype, `ingot_name` stays empty and the final talisman name becomes ` Talisman of Identification` (leading space). Guard the concatenation or skip the prefix when empty.
- **Double system message on consume failure** — `ConsumeResource` sends `"You don't have enough resources."` internally before returning 0; the talisman code then sends a second more specific message (`"You don't have enough ingots."`). The player sees both. `TryToMakePotionKeg:1081-1084` has the same pre-existing double-message pattern.
- **`ore_cfg` read per invocation** — `var ore_cfg := ReadConfigFile( ":blacksmithy:blacksmithy" );` runs on every call. `tinker_cfg` is already hoisted to module level at line 44. Move `ore_cfg` alongside it for consistency and to avoid repeated parsing.
- **Dead parameter `needed_objtype`** — After the v1.3 cleanup the parameter is still in the function signature and never read. Both callers pass `0xffa3`. Either remove from the signature or leave a `// reserved for future secondary-component recipes` comment to justify its retention.
- **Dead local `ingot`** — After building `ingotRequest` (line 1235 or line 1209), the `ingot` variable is unused. Minor clutter, not a bug.
- **Redundant `ingot_quality` fallback** — Line 1182 initialises `ingot_quality := 1`. Lines 1262-1264 re-default to 1 after the `ore_cfg` block, but the outer `if( ore_cfg[...].Quality )` already filters the falsy case. The fallback is effectively dead.

---

## Files Modified

### Talisman Crafting
- `pkg/std/tinkering/tinkering.src` — `TryToMakeTalisman`: cache-aware availability check, PHC halving, consume-before-skill-check reorder, removed internal `ReserveItem`/`ReleaseItem`, removed dead `needed_objtype` block

### ConsumeResource hardening
- `scripts/include/resourcemanager.inc` — `use os` added for `SysLog`; `ConsumeResource` tracks per-source consumption, emits structured `PARTIAL_CONSUME` log on partial failure, sends generic player message

### `houseSerial` plumbing
- `scripts/include/resourcemanager.inc` — `MakeBackpackRequest` / `SelectMaterialFromList` output `houseSerial`; `SelectMaterialFromList` signature changed to take `access` struct
- `pkg/std/tinkering/tinkering.src` — `tempRequest` / `secondRequest` inherit `houseSerial` from `firstRequest`; `cacheBottleReq` resolves from freshly looked-up `access`
- `pkg/std/alchemy/alchemy.src` — `bottleReq` inherits `houseSerial` from `parentRequest` or resolves fresh
- `pkg/std/cooking/cooking.src` — `cooking_cache_house_serial` module var tracked across three resolution paths; `cacheRequest` inline struct carries `houseSerial`
- `scripts/items/bladed.src` — `cacheHideReq` inherits `houseSerial` from `logRequest`; `hideReq` adds `houseSerial := 0` for struct shape consistency

### `GetBottle` refactor
- `pkg/std/alchemy/alchemy.src` — `GetBottle(conts, user, dataFileHandle)` → `GetBottle(conts, user, parentRequest)`; both callers in `TryToMakePotion` updated to pass `regRequest` directly
