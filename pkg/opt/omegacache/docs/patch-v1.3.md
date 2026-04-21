# Omega Cache - Patch v1.3

## Summary

This patch addresses issues introduced by the merged talisman crafting feature (`TryToMakeTalisman` in `pkg/std/tinkering/tinkering.src`). The initial implementation diverged from the established single-craft cache pattern in several ways: it bypassed the autodraw fallback when ingots were selected from the backpack, retained redundant `ReserveItem`/`ReleaseItem` calls, carried dead secondary-component validation code, and was missing the Crafting Power Hour half-material discount that all sibling tinkering functions apply. These have been corrected so talisman crafting honours cache autodraw, follows the consume-before-skill-check convention shared with `MakeTotem` and `TryToMakePotionKeg`, and applies the PHC discount consistently.

Alongside the talisman fixes, the wider resource-manager layer was hardened: `ConsumeResource` now detects and logs partial consumption (materials drained but the full request not satisfied — possible under concurrent lease/withdraw races) so support can diagnose rare material-loss reports; a `houseSerial` field is threaded through `ResourceRequest` so partial-consume logs identify which house's cache was involved without requiring expensive on-demand lookups; and `GetBottle` in alchemy was refactored to accept a full parent request instead of a raw `dataFileHandle`, eliminating lossy argument passing at two call sites.

One AlchemyPlus bug fix: the potion loop had a long-standing bug where consuming the last flask/bottle aborted the current iteration with "You run out of flasks" / "You run out of bottles" *before* the potion was created, causing material loss without the expected output. The loop now creates the potion first and searches for the next container afterwards — matching the pattern already used in the sibling `alchemyplus toad.src`. (Note: a stackability fix for the flask/brain items was investigated and reverted — server-side stacking worked but the UO client wouldn't render the drag-split UI without a client-side `tiledata.mul` patch, which is out of scope for this patch.)

**Follow-up (next patch):** AlchemyPlus is not cache-integrated — the reagent/flask/brain pipeline still uses direct `FindItemInContainer` / `SubtractAmount` calls without `MakeBackpackRequest` / `ConsumeResource` / autodraw support. A separate Phase-2-style integration is planned next to mirror what was done for vanilla alchemy (`pkg/std/alchemy/alchemy.src`).

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

### AlchemyPlus — "run out of flasks/bottles" bug

- **Cause**: the main potion loop in `alchemyplus.src:183-226` consumed the container (flask or bottle) **before** creating the potion, then searched for the *next* iteration's container and aborted on failure — losing the current iteration's potion output.
- **Fix**: reorder to `CreateItemInContainer` first, then `SubtractAmount`, then search for next-iteration container. Pattern mirrors `alchemyplus toad.src` which already did this correctly.
- **Same fix covers both bottles and flasks** — the unified post-create block uses a `needs_container` flag plus `next_objtype` / `material_name` for the two cases.

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

### AlchemyPlus Loop Fix

| Area | What to Test |
|------|-------------|
| **AlchemyPlus craft — one flask** | Have exactly 1 flask and reagents. Craft one flask potion. Verify the potion IS created AND the "You run out of flasks." message fires after creation. **Previously the flask was consumed with no potion produced.** |
| **AlchemyPlus craft — multiple flasks** | Have 5 flasks and reagents for 5 crafts. Verify all 5 potions produced, loop terminates on the 6th iteration with "run out" message. |
| **AlchemyPlus craft — one bottle** | Have exactly 1 empty bottle and reagents. Craft one potion. Verify potion created, "run out of bottles" after. |
| **AlchemyPlus craft — multiple bottles** | 5 bottles + reagents. Verify 5 potions produced. |

### AlchemyPlus Cache Integration (Phase 1-3)

Tonight the alchemyplus crafting flow (`pkg/opt/alchemyplus/alchemyplus.src`) was retrofitted to use the Omega Cache resource manager. This is a substantial change to the craft lifecycle. Tests below should cover every code path touched.

#### Setup & targeting

| Area | What to Test |
|------|-------------|
| **Backpack-only regression** | Empty the cache (or move away). Craft any AlchemyPlus recipe with reagents in backpack. Verify recipe works exactly as before. |
| **Cache target — primary reagent** | Double-click burner, target the cache container. Material-selection gump appears. Pick a reagent (e.g. ginseng). Menu of recipes that use ginseng appears. Pick one, craft. Verify the primary reagent drains from the cache. |
| **Cache target — not-a-reagent item** | Double-click burner, target cache, pick a non-reagent (e.g. iron ingot). Verify: `"That's not a reagent I know how to use."` and crafting aborts. |
| **Cache target — basicreg bypass** | Double-click burner, target cache, pick a basic reagent (ginseng/mandrake). Verify: `"You must use a mortar to make basic potions."` Same rule as targeting a basicreg in backpack. |
| **Backpack target (legacy)** | Double-click burner, target a reagent in backpack. Menu of recipes appears as before. Pick one, craft. Verify behaviour identical to pre-patch. |
| **Burner-as-target / lastmade** | Craft something via menu so `lastmade` is set. Double-click burner, target the burner itself. Verify: last recipe re-triggered, crafts correctly. Verify `"success"` bypass path still works. |
| **Autodraw disabled** | Run `.cache autodraw` to disable. Backpack has half the reagents, cache has the rest. Verify recipe rejected (autodraw off → cache not used). Re-enable, verify success. |

#### Material sourcing

| Area | What to Test |
|------|-------------|
| **All reagents in backpack** | Standard legacy case. Craft completes, reagents consumed from backpack. |
| **All reagents in cache + autodraw** | Backpack empty of reagents. Cache has all. Craft completes, reagents drain from cache. |
| **Split — some backpack, some cache** | Mix: half the reagents backpack-only, half cache-only. Autodraw on. Verify craft completes; each reagent drains from its respective source. |
| **Split — same reagent both sources** | Ginseng: 2 in backpack, 8 in cache. Craft 5 potions (5 ginseng each = 25 needed). Verify: 10 drained from backpack first (across all iterations), then 15 from cache. Backpack-first order honoured. |
| **Insufficient — fails cleanly** | Start with fewer reagents than any recipe needs. Verify menu shows no recipes (or fewer); if player picks one, `"You can no longer make that potion."` mid-loop. |

#### Flask & Brain (talisman recipe — itemtype 40)

Note: the flask/brain/talisman-gem items are NOT stackable and therefore cannot be stored in the Omega Cache. Tests here verify graceful backpack-only behaviour for these items while the stackable reagents (wyrmheart, daemonbone, dragonblood, obsidian) can still be cache-sourced via autodraw.

| Area | What to Test |
|------|-------------|
| **All reagents + flask + brain in backpack** | Craft the Flask of Crystallized Intelligence. All 12 reagents consumed from backpack, empty flask consumed, filled flask created. |
| **Stackable reagents in cache** | Wyrmheart (`0x0F91`), daemonbone (`0x0F80`), dragonblood (`0x0F82`), obsidian (`0x0F89`) only in cache; brain, empty flask, and talisman gems in backpack. Craft. Verify stackable reagents drain from cache; brain/flask/gems drain from backpack. |
| **Brain missing everywhere** | Empty flask + all other reagents present; no brain anywhere. Verify recipe not craftable, availability check fails cleanly. |
| **Empty flask missing** | All reagents present; no empty flask in backpack. Verify: `"You need an empty flask to make that potion."` — flask path does not Target() fallback (preserved legacy). |
| **Basicpot reagent resolved to leveled variant** | Craft a recipe with `reagent 0xDC03 4` (Greater Heal). Player has 4 leveled Greater Heal potions (objtype `0xff1c`+). Verify: `BuildReagentRequests` resolves `0xDC03` → `0xff1c`, `CanMakePotion` passes, `destroy_all_reagents` consumes `0xff1c`. Same test but with the leveled variants in the CACHE instead (leveled potions ARE stackable) — verify resolution also works via cache probe. |

#### Lease + concurrency

| Area | What to Test |
|------|-------------|
| **Lease release — normal loop completion** | Craft enough for 5 iterations, run 5 of 5. Verify loop exits cleanly, no lingering leases. Open cache as another player to confirm nothing reserved. |
| **Lease release — `CanMakePotion` fails mid-loop** | Start crafting. Mid-loop, have a GM consume remaining backpack stock. Next iteration's `CanMakePotion` returns 0. Loop breaks via `break` (not `return`). Verify leases released. |
| **Lease release — skill failure path** | With low skill, craft many potions. Skill fails on some iterations. Verify loop continues on failures and releases leases at end. |
| **Lease release — death** | Craft long loop. Have character take fatal damage mid-loop. `NOT character.dead` fails, loop breaks. Verify leases released (cache not permanently reserved). |
| **Lease release — container runs out** | Have exactly 1 bottle for a recipe needing 1 bottle per iteration. Craft. Create first iteration, `ConsumeResource` drains to 0, `GetAvailableResource < 1` → break. Verify leases released. |
| **Concurrent crafters — same cache** | Two players near same cache, same reagent. Player A starts crafting with lease. Player B attempts same — should see reduced availability (own lease excluded, but Player A's lease subtracted). Verify no double-consumption. |
| **Menu filter performance** | Open recipe menu as high-skill alchemist with 30+ recipes viable. Observe whether `CanMakePotion` × N × reagents produces a perceptible lag. Should complete within 1-2 seconds. |

#### Container (flask/bottle) edge cases

| Area | What to Test |
|------|-------------|
| **Single flask, single craft** | Exactly 1 empty flask in backpack, reagents for 1 craft. Verify flask consumed, filled flask created, loop exits with `"You run out of flasks."` after creation. **This was v1.3's fix — regression check.** |
| **Multiple flasks, partial batch** | 5 flasks, reagents for only 3 potions. Verify 3 filled flasks created, loop exits after 3rd when reagents run out. 2 empty flasks remain. |
| **Flask in cache-only path (stackable containers only — e.g. if future flasks become stackable)** | This path exists in code for forward compatibility. Current non-stackable flasks can't be in cache, so `containerRequest` never reaches this branch for flasks today. Test this branch by temporarily making `0x0F0E` (empty bottle) cache-stored and crafting a bottle-based potion with empty backpack. |
| **Flask target fallback (bottles only)** | For a non-flask recipe (itemtype != 40), no bottles in backpack or cache. Verify: `"Select an empty bottle to make the potion in."` prompt appears (legacy path preserved). |
| **Flask target fallback (flask)** | For flask recipe, no flask anywhere. Verify: `"You need an empty flask to make that potion."` and abort (legacy behaviour preserved — no Target() prompt for flasks). |

#### `primary_cacheRequest` mutation fix

| Area | What to Test |
|------|-------------|
| **Menu filter doesn't mutate `primary_cacheRequest`** | Target cache, pick ginseng (or any reagent used by many recipes). Menu-filter pass calls `CanMakePotion` for 40 recipes. Verify: no compile error on `.+`, no stale-amount bugs when player then picks a potion. The fix copies fields into fresh structs rather than mutating the module-level var. |
| **Multiple menu-filter-then-craft cycles** | Target cache, pick ginseng, craft recipe A. Without closing the burner, re-trigger crafting (new invocation). Target cache again, pick a different reagent. Verify second invocation works correctly (module-level vars reset per program instance). |

#### Miscellaneous regression

| Area | What to Test |
|------|-------------|
| **Mortar requirement still enforced** | Attempt a mortar-requiring recipe without a mortar in backpack. Verify: rejected at `CanMakePotion` level. |
| **Specialist check still enforced** | Non-Mage player attempts a specialist potion (e.g. Flask of Crystallized Intelligence). Verify: rejected. |
| **Skill gate unchanged** | Player with alchemy skill just below recipe skill (within 10 pts). Verify: recipe appears in menu (legacy allows -10). Below -10, recipe not shown. |
| **Reagent reservation still blocks external mutation** | Start crafting. Have another player or packet hook try to move the reagent stack mid-craft. Verify: blocked (`ReserveItem` held by our script). |
| **Potion produced in correct mage level** | As mage, craft a leveled potion (e.g. Phandel's Fine Intellect). Verify: output objtype matches the mage-level mapping in `ReturnTruePotion`. |
| **Container cache lease extended correctly** | Craft from cache flasks for 10 iterations. Verify: ExtendResourceLease on container succeeds for all 10. If it fails (manually expire), loop breaks cleanly. |

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

### AlchemyPlus loop fix
- `pkg/opt/alchemyplus/alchemyplus.src` — Reordered the main potion loop to create the potion BEFORE consuming the container. Previously `SubtractAmount` → `FindItemInContainer` → abort (before `CreateItemInContainer`) caused the current iteration's potion to be lost when the player had exactly 1 flask/bottle. New order matches the sibling `alchemyplus toad.src` pattern.

---

## Follow-up work (next patch)

- **AlchemyPlus cache integration.** The flask/brain/reagent pipeline in `alchemyplus.src` still uses direct `FindItemInContainer` / `SubtractAmount` calls. Needs the same Phase-2-style refactor applied to `pkg/std/alchemy/alchemy.src` — `MakeBackpackRequest` / `GetAvailableResource` / `ConsumeResource` with autodraw support so reagents and containers can be pulled from the Omega Cache.
