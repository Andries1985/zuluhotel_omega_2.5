# Developer Changelog - v1.0.8
**Range:** `9f8189f` (origin/Patch-1.0.7) -> `7c27772` (HEAD)  
**Branch:** Patch-1.0.8  
**Date:** 2026-06-20 -> 2026-07-23  
**Commits in range:** 10 (excluding merge commits)  
**Files changed:** 84 (+12970 / -1689)

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Townstones - Player-Run Town Administration and Membership Cleanup](#3-townstones---player-run-town-administration-and-membership-cleanup)
4. [Areas - Policy Engine Rewrite, Realm Sanitization, and Mask-Value Cache](#4-areas---policy-engine-rewrite-realm-sanitization-and-mask-value-cache)
5. [Go Locations - Indexed Config Generation and Range Fallback](#5-go-locations---indexed-config-generation-and-range-fallback)
6. [Spawnpoint - Restart Helpers and Region-Wide Reset Commands](#6-spawnpoint---restart-helpers-and-region-wide-reset-commands)
7. [Housing - Region-Based Placement Restrictions](#7-housing---region-based-placement-restrictions)
8. [Naming and Identity - Town-Suffix Exploit Closure and Validation Hardening](#8-naming-and-identity---town-suffix-exploit-closure-and-validation-hardening)
9. [Performance - Shutdown Time and Hot-Path Algorithm Fixes](#9-performance---shutdown-time-and-hot-path-algorithm-fixes)
10. [Crafting - Omega Cache Integration Fixes](#10-crafting---omega-cache-integration-fixes)
11. [Omega Cache - Potion Categorization Fix](#11-omega-cache---potion-categorization-fix)
12. [Gameplay Tuning and Script Fixes](#12-gameplay-tuning-and-script-fixes)
13. [Support Data and Tooling](#13-support-data-and-tooling)
14. [Staff Tooling - Character/Account Audit Panel and Name/Death/Poison/Note Tracking](#14-staff-tooling---characteraccount-audit-panel-and-namedeathpoisonnote-tracking)
15. [Exhaustive File-by-File Change List](#15-exhaustive-file-by-file-change-list)
16. [Risk and Regression Notes](#16-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.0.8 grew across two work windows. The first (through `a3c99f6`) was dominated by the datafile-backed area policy rewrite, the go-location indexing pass, and the player-run townstone/admin cleanup — see sections 3, 5, and 6 for that material, carried forward unchanged from the earlier draft of this document.

The second window (`7bdc099` through `b347427`) closed a live-reported name-collision exploit, fixed a duplicate-character bug it was related to, added region-based house placement restrictions, fixed a real performance regression in the areas package and in two hot commands, and completed Omega Cache integration for two crafting scripts that had been missed or left broken during the original cache rollout:

- A town-suffix name-collision exploit was closed: players could no longer hold both "X" and "X of Town" at once, town names are now blocked in freely-chosen names, and name comparisons are case-insensitive.
- A live 50-second rename-gump hang, and a resulting duplicate-character bug ("two Paladin Rahl of Occlo"), were root-caused to an O(accounts x 5) full `regions.cfg` re-parse per name check, and fixed with a proper cache.
- Staff rename paths (`.setprop name`, `.setname`, `.info`'s rename button) now run through the same name validation as player-initiated renames, scoped to player characters only.
- House placement now blocks city, dungeon, shrine, and graveyard regions by footprint (not just a single point check), replacing an older, narrower check.
- The area-policy mask lookup (`GetPolicyMask`) is now cached; it was previously hitting the datafile subsystem on every single area-policy check, including every AI tick for every mobile shard-wide.
- Two independent O(n^2) algorithms (`.go`'s location-reorder pass, and a shared multi-array sort used by `.gotomulti`/`.gotoboat`) were replaced with linear/O(n log n) equivalents after script-log evidence showed both tripping the runaway-script watchdog.
- The smithy hammer's Omega Cache targeting path was found to be dead code due to a bad merge and was rewritten to match every other crafting script's pattern; the crafter-boost smithy retort had never been integrated with the cache at all and now is.
- A gap in the Omega Cache's category map left 12 leveled potion objtypes (Greater Strength/Cure Potion tiers, etc.) falling into the "Other" bucket instead of "Potions."

A third window (`7c27772`) added a full staff-facing character/account audit tool and wired full name-change, death, poisoning, and account-note tracking into every script that touches them — see section 14. Two additional commits landed in this range (`b347427`, `232ee95`); `b347427`'s crafting/performance/potion-category work is already covered above (sections 9-11), and `232ee95` (a townstone treasury-race, election-cleanup, and townlist-bootstrap fix pass) is not detailed in this document.

---

## 2. Commit Timeline

| Commit | Date | Message |
|--------|------|---------|
| `32a2b3a` | 2026-06-20 | backup script path restore |
| `dbfbe79` | 2026-07-20 | Areas fix for lord british castle and blackthornes |
| `a3c99f6` | 2026-07-20 | Bunch of patches for player run towns |
| `7bdc099` | 2026-07-20 | Patch Notes and save datafile error |
| `065ed28` | 2026-07-20 | House placement not allowed in cities |
| `21bb91e` | 2026-07-23 | Name Change update |
| `09bf876` | 2026-07-23 | Areas Fix, and naming fixes |
| `b347427` | 2026-07-23 | Patch notes, and fixes for omega cache, as well as optimization for go gomulti and goboat (see sections 9-11) |
| `232ee95` | 2026-07-23 | Townstone fixes, Minstel and townsfolk fix (not detailed in this document) |
| `7c27772` | 2026-07-23 | New test panel up along with datafiles (see section 14) |

---

## 3. Townstones - Player-Run Town Administration and Membership Cleanup

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/townstones/textcmd/admin/createtownstone.src` | Creation flow tightened around existing live stones, townlist insertion, and state restoration from the datafile |
| `pkg/opt/townstones/textcmd/admin/removetownmember.src` | New targeted town-member removal command with election and poll cleanup plus datafile sync |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | Large admin gump expansion for treasury, upgrades, donations, availability, purchase state, member management, and deletion |

### Notable Functional Changes

- `townbankstatus` now surfaces and mutates more live town state:
  - treasury gold,
  - population,
  - upgrades enabled or disabled,
  - donations enabled or disabled,
  - player-town availability,
  - player-town purchased state,
  - purchase price,
  - townstone presence,
  - and region deletion when safe.
- Member management now removes citizens more carefully:
  - account membership is removed from the stone's citizen list,
  - per-character `town` properties are cleared,
  - town suffixes are stripped from character names,
  - population and vote counts are refreshed,
  - elections and polls are cleaned up if the removed member was involved.
- `removetownmember` is a direct targeted cleanup path for staff workflows.
- `createtownstone` now checks for active and live conflicts more aggressively before creating a new stone.
- Townstone state sync now persists the current stone serial, mayor data, population, election flags, poll flags, candidate list, vote totals, vote percentage, timer, and citizen list back into the datafile.

---

## 4. Areas - Policy Engine Rewrite, Realm Sanitization, and Mask-Value Cache

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/areas/include/areapolicy.inc` | New datafile-backed policy engine, parsed-line cache, global bypass masks, realm-name sanitization, and a mask-value cache |
| `pkg/opt/areas/include/areapolicy.inc.bak` | Backup copy of the pre-rewrite policy implementation |
| `pkg/opt/areas/textcmd/admin/areas.src` | Rewritten `.areas` admin gump for viewing and toggling area policy flags |
| `pkg/opt/areas/EnterAreaDelay.src` | Updated to use the new policy resolver path |
| `pkg/opt/areas/LeaveArea.src` | Updated to use the new policy resolver path |
| `pkg/opt/areas/areaban.src` | Updated to use the new policy resolver path |
| `pkg/opt/areas/areas.cfg` | Area definitions refreshed, including the Lord British Castle and Lord Blackthornes Castle fix |
| `scripts/include/areas.inc` | Shared area helpers updated for the new resolver model |
| `scripts/include/anchors.inc` | Anchor and lookup helpers updated to cooperate with the new area model |

### Notable Functional Changes

- The area policy layer now lives in a dedicated resolver backed by per-realm datafiles:
  - masks are stored per area id,
  - a world-catchall bypass mask is OR-merged from known global ids (`feluccawholeworld`, `wholeworld`, `britannia`, `trammelwholeworld`) — confirmed still a one-directional OR merge, so a whole-realm flag (e.g. setting Felucca or Britannia to RP) can force a policy bit on everywhere in that realm, but an individual area still can't opt back out of it since OR can't clear a bit,
  - parsed area lines are cached both per program and in a global property,
  - cache invalidation is fingerprinted by source line count.
- The admin `.areas` workflow now works as a single gump-driven editor for common policy flags such as guarded, no recall, no marking, anti-magic, forbidden, no looting, safe-area, no-PK, and RP-area behavior.
- `areas.cfg` includes the specific Lord British Castle and Lord Blackthornes Castle boundary fix from `dbfbe79`.
- The hot-path area checks now resolve through the new policy layer instead of repeatedly re-parsing config lines.
- **Realm-name sanitization (`7bdc099`):** every realm-taking function in `areapolicy.inc` (`GetRealmAreaLines`, `GetParsedRealmAreaLines`, `ResolveAreaMatchAtLocation`, `ResolvePolicyAtLocation`, `GetGlobalBypassMask`, `GetRealmPolicyDescriptor`) previously did a bare `Lower(CStr(realm))`. A new `SanitizePolicyRealm(realm)` now falls back to `"britannia"` whenever the incoming realm is blank or the literal string `"<uninitialized object>"`, fixing a datafile-save error that occurred when a realm value hadn't been initialized yet.
- **Mask-value cache (`09bf876`):** `GetPolicyMask(realm, area_id)` was hitting the datafile subsystem (`OpenDataFile` + `FindElement` + `GetProp`) on every single area-policy check, and `GetGlobalBypassMask()` calls it 4 more times per resolve for the world-catchall ids — up to 5 datafile round trips per check, on every AI tick for every mobile shard-wide. `GetPolicyMask` is now backed by a per-realm `dictionary` (`area_id -> mask`), cached both per-program and in a `GlobalProperty` (`zh.areapolicy.maskcache.<realm>`), using `.Exists()` rather than truthiness so a cached mask of `0` (the common case) isn't mistaken for "not cached." The cache is invalidated precisely on `SetPolicyMask()` writes (single area id) and wholesale on `PruneStaleRealmPolicies()` (whole-realm bulk delete). This mirrors the existing parsed-line cache and was verified against the ZH3.0 sibling repo's simpler (no world-catchall) implementation for reference before implementing.

---

## 5. Go Locations - Indexed Config Generation and Range Fallback

### Files Changed

| File | Change |
|------|--------|
| `config/golocs_by_id.cfg` | New generated index of go locations by region id, realm, type, range, and GoLoc metadata |
| `config/command_synopses.cfg` | Updated synopsis entry for `.go` and new supporting commands |
| `pythonscripts/generate_golocs_by_id.py` | New generator that builds the indexed go-location config from region data |
| `scripts/textcmd/coun/go.src` | `.go` rebuilt around the indexed go-location config, facet selection, type ordering, and range fallback |
| `regions/regions.cfg` | Massive region metadata expansion to feed the new go-location generator |

### Notable Functional Changes

- `.go` now reads `golocs_by_id.cfg` instead of the older ad hoc config path.
- Regions can now be resolved from either:
  - an explicit `GoLoc`, or
  - the center point of the configured `Range`.
- A `dnp` `GoLoc` value is treated as an intentional skip.
- `0,0,0` explicit coordinates are treated as a placeholder and are replaced from the range center when possible.
- Location ordering is normalized by type (`Jail`, `City`, `Shrine`, `Graveyard`, `Dungeon`, `POI`, `None`) before any leftover entries are appended. See section 9 for the algorithmic rewrite of this ordering pass.
- The generator script exists so the index can be rebuilt from `regions.cfg` instead of hand-maintaining the go list.

---

## 6. Spawnpoint - Restart Helpers and Region-Wide Reset Commands

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | Shared restart helper extracted for normal restart and max-fill modes |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpoint.src` | Restart command updated to use the shared helper and report next spawn timing |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | New region-wide restart and restartmax command that iterates spawnpoints by selected location ranges |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointmax.src` | Force-fill command updated to use the shared helper and report fill counts |
| `pkg/opt/spawnpoint/config/groups.cfg` | Spawnpoint group config adjusted to support the new region restart workflow |

### Notable Functional Changes

- `RestartSpawnPointWithMode(...)` now encapsulates the restart and max-fill logic:
  - normal restart clears active spawned objects and schedules the next spawn based on the configured frequency,
  - max-fill temporarily enables the group flag if needed, repeatedly checkpoints until the target max is reached, then restores the original group flag.
- `.restartspawnpoint` now reports the next spawn delay after the restart.
- `.restartspawnpointmax` now reports the number of spawned objects relative to the configured maximum.
- `.restartspawnpointarea` is a new command that:
  - lets staff choose a facet,
  - then choose a location from the `golocs_by_id.cfg` index,
  - then restarts every spawnpoint whose coordinates fall inside that location's configured ranges.

---

## 7. Housing - Region-Based Placement Restrictions

### Files Changed

| File | Change |
|------|--------|
| `pkg/std/housing/housedeed.src` | House placement now checked against region types by footprint; dungeon/system-teleporter proximity check extracted into a helper |

### Notable Functional Changes

- Replaced the old single-tile `CheckCity(character)==1` check (which only tested the placing character's own location) with `IsHousePlacementBlockedByRegionType(housetype, where, region_type)`, called once each for `"city"`, `"dungeon"`, `"shrine"`, and `"graveyard"`. This computes the house's full footprint via `GetHouseFootprintBounds(housetype, x, y)` (multi dimensions applied to the placement point) and rejects placement if that footprint overlaps any `regions.cfg` region of the matching `Type`, scoped to the placing character's realm.
- `NormalizeRegionType(region_type)` lowercases and validates the region-type string used for comparison against `regions.cfg`'s `Type` field.
- The old inline dungeon-item / system-teleporter proximity checks (`ListObjectsInBox` scans for objtype `0xa3c8` and `0x6200`) were extracted into `IsHousePlacementBlockedByNearbySpecialObjects(where)`, which returns the rejection message directly instead of duplicating the `ListObjectsInBox` scan and message send inline.
- Staff (`character.cmdlevel >= 4`) are unaffected — all of the above is still gated behind the existing `if (character.cmdlevel < 4)` check.

---

## 8. Naming and Identity - Town-Suffix Exploit Closure and Validation Hardening

### Files Changed

| File | Change |
|------|--------|
| `scripts/include/NameChecker.inc` | Town-suffix-aware duplicate detection, case-insensitive comparison, whitespace normalization, bad-spacing rejection, and caching |
| `pkg/opt/townstones/tstone.src` | `Citizenship()`/`CanselCityzenship()` now duplicate-check before applying a town-suffixed or stripped name |
| `scripts/misc/namechanger.src` | Passes `force_town_check := 1`; added "bad spacing" error message |
| `scripts/misc/oncreate.src` | Passes `force_town_check := 1` |
| `scripts/textcmd/admin/setname.src` | `.setname` now runs through `CheckName`, scoped to player characters only |
| `scripts/textcmd/gm/setprop.src` | `.setprop name` now runs through `CheckName`, scoped to player characters only |
| `scripts/textcmd/seer/info.src` | `.info`'s rename button now runs through `CheckName`, scoped to player characters only |

### Background

A live exploit was reported: a player could join a town as "X" (becoming "X of Town"), create a second character also named "X" on an alt, then leave the town (reverting the first character to bare "X"), ending up holding both "X" and "X of Town" simultaneously — and bypass case-sensitivity to also claim near-duplicates like "Bop"/"bop"/"BOP". Investigating this also surfaced a live duplicate-character bug (two characters both named "Paladin Rahl of Occlo") and a report of a ~50 second hang with stacked rename gumps after a related fix — both root-caused and fixed here.

### Notable Functional Changes

- **`GetKnownTownNames()`** — replaces the old static `bLTowns`-only blacklist scan with a merged list from three sources: the static `bLTowns` fallback, every `City=1` region in `regions.cfg` (covers player-run townstone regions), and `GetGlobalProperty("townlist")` (covers a townstone whose region entry was later removed). Cached per-program and in a `GlobalProperty` (`zh.namechecker.knowntownnames`); `InvalidateKnownTownNamesCache()` is exposed but not yet wired to any call site (regions.cfg is effectively static at runtime today).
- **`StripTownSuffix(name, town_names := 0)`** — strips a trailing `" of <Town>"` for any known town so name-uniqueness comparisons treat `"a pig"` and `"a pig of Trinsic"` as the same identity, closing the join/leave/alt exploit above. Accepts an optional pre-fetched `town_names` list to avoid recomputing it in a loop.
- **`NormalizeNameForCompare(name)`** — lowercases, collapses runs of interior spaces to one, and trims leading/trailing spaces before comparison.
- **`IsNameTaken(candidate_name, exclude_serial := 0)`** — scans up to 5 character slots per account, comparing `NormalizeNameForCompare(StripTownSuffix(name, town_names))`, case-insensitively, excluding `exclude_serial`. Replaces the old exact-string, case-sensitive comparison in `CheckName` (which never caught `"Bop"` vs `"bop"`).
- **`CheckName(name, who := 0, force_town_check := 0)`** — new `force_town_check` parameter: pass `1` whenever the player is actively choosing a brand-new name (character creation, the rename gump, staff rename tools) so town names are always blocked regardless of the player's current town membership; login/reconnect revalidation of an already-assigned, legitimately town-suffixed name leaves this `0`. Also added an unconditional bad-spacing rejection (`name[1] == " " || name[len(name)] == " " || find(name, "  ", 0)`) — leading/trailing/doubled spaces were technically "legal characters" under the old per-character validation loop, but let a base name like `"Rahl "` silently become `"Rahl  of Town"` once a town suffix was appended, evading exact-match duplicate detection. This check is unconditional (not gated by `force_town_check`), so it also self-heals an already-malformed stored name on next login revalidation — this is exactly how the shard's one existing malformed record (`"Paladin Rahl  of Occlo"`, confirmed via `data/pcs.txt` to be the only such record shard-wide) will get caught and forced through a rename.
- **`pkg/opt/townstones/tstone.src`**: `Citizenship(who, item)` now computes the candidate town-suffixed name and calls `IsNameTaken()` before any state mutation, aborting with a message if the resulting name is already in use, rather than mutating town membership and then blindly assigning a colliding name. `CanselCityzenship(who, item)` (leave-town) computes the stripped base name, and if `IsNameTaken()` finds it colliding, sets the character's name to `"AlreadyUsed"` and force-starts `misc/namechanger` instead of blocking the leave — citizen-list removal, population decrement, and the `town` property erasure all still proceed unconditionally, so the player isn't stuck in town membership limbo while they pick a new name.
- **Staff rename paths** now validate through the same `CheckName()` path as player-driven renames, all explicitly scoped to player characters (`what.acct` / `targ.acct` / `who.acct` checks) so NPC renaming by staff is untouched:
  - `.setname` (`scripts/textcmd/admin/setname.src`)
  - `.setprop name` (`scripts/textcmd/gm/setprop.src`)
  - `.info`'s rename button (`scripts/textcmd/seer/info.src`)
- **Performance root cause (the 50-second hang):** the original `IsNameTaken`/`StripTownSuffix` implementation called `GetKnownTownNames()` (a full `regions.cfg` re-parse) on every character-slot comparison. With 5,169 accounts x up to 5 slots, that was ~25,000 redundant full-config re-parses per `CheckName` call — the direct cause of the reported hang and stacked rename gumps. Fixed by hoisting `GetKnownTownNames()` to once per `IsNameTaken`/`CheckName` call (passed through as a parameter) and adding the cache described above.

---

## 9. Performance - Shutdown Time and Hot-Path Algorithm Fixes

### Background

Investigated a live report that "POL Cleanup" during shutdown takes noticeably longer since the areas-to-datafile migration, alongside a slow ~1 hour post-startup CPU stabilization period. `log/script.log` showed the runaway-script watchdog firing on `scripts/textcmd/coun/go.ecl` (3 times) and `pkg/opt/alryc/textcmd/test/gotomulti.ecl` (2 times) — both algorithmic, not areas-related. Note: `log/pol.log` does not capture the console-only cleanup messages, so the runaway scripts are strong circumstantial evidence and worthwhile independent fixes on their own merits, not a proven root cause of the shutdown-time regression specifically (the areas mask-value cache in section 4 is the more directly-implicated fix for that).

### Files Changed (uncommitted)

| File | Change |
|------|--------|
| `scripts/textcmd/coun/go.src` | `ReorderLocationsForDisplay()` rewritten from an O(n^2)*8 pass to a single O(n) bucketing pass |
| `scripts/include/string.inc` | `SortMultiArrayByIndex()` rewritten from an O(n^2) selection sort to an O(n log n) iterative bottom-up merge sort |

### Notable Functional Changes

- **`.go`'s `ReorderLocationsForDisplay()`**: previously called `AppendLocationsByType()` once per type bucket (Jail/City/Shrine/Graveyard/Dungeon/POI/None — 7 passes), each of which rescanned the entire, continuously-growing `ordered` output list via `LocationAlreadyAdded()` for every candidate to dedupe by `Id` or by `Name+X+Y+Z+R` — effectively O(n^2) x 8 passes total (7 typed + 1 final catch-all). With ~200 Britannia go-locations alone, this was heavy enough to trip the runaway-script watchdog. Rewritten as a single O(n) pass: build a `LocationDedupeKey(loc)` (prefers `"id:<Id>"`, falls back to `"xy:<Name>:<X>:<Y>:<Z>:<R>"`), dedupe against a `seen` dictionary once, bucket by `GetLocationTypeLabel()` into a `dictionary` of arrays, then emit known-type buckets in priority order followed by any custom/unrecognized-type entries in original encounter order. Output ordering is unchanged from the old implementation; only the algorithm changed. `AppendLocationsByType()` and `LocationAlreadyAdded()` were removed (confirmed via repo-wide grep that nothing else referenced them).
- **`SortMultiArrayByIndex(MultiArray, SubIndex)`** (`scripts/include/string.inc`): previously an O(n^2) nested-loop selection sort. Rewritten as an iterative bottom-up merge sort (O(n log n)) via a new `MergeRunsByIndex(MultiArray, low, mid, high, SubIndex)` helper, same ascending-by-`SubIndex` output ordering. This is a shared include used by both `pkg/opt/alryc/textcmd/test/gotomulti.src` (`SortHouseMultisByLastLogin`, sorting up to 418 multi records) and `gotoboat.src` (`SortBoatsByLastLogin`) — fixing it here benefits both call sites without touching either file. The merge-loop write-position variable was originally named `out`, which is a reserved word in EScript/POL; renamed to `write_idx`.

---

## 10. Crafting - Omega Cache Integration Fixes

### Files Changed (uncommitted)

| File | Change |
|------|--------|
| `pkg/std/blacksmithy/make_blacksmith_items.src` | `use_hammer` rewritten so Omega Cache targeting is reachable; dead/duplicate legacy code removed |
| `pkg/opt/crafterboost/make_crafter_boosts.src` | Full Omega Cache integration added (previously backpack-only) |

### Notable Functional Changes

- **Blacksmithy (`make_blacksmith_items.src`)**: `use_hammer` previously performed an unconditional `ReserveItem(use_on)` and an unconditional `IsInContainer(character.backpack, use_on)` check *before* any Omega Cache objtype check — unlike every other cache-integrated crafting script (`alchemy.src`, `tinkering.src`, `carpentry.src`, `make_cloth_items.src`, `cooking.src`, `cartography.src`, `inscription.src`), which all check for the cache objtype first. Since the Omega Cache container is a placed house item, it's never "in your backpack," so targeting the cache directly for a plain ingot craft failed with "That item has to be in your backpack." before ever reaching the cache-handling code lower in the file. The only path that worked was the bone-armor sub-flow (target a bone item from backpack first, then target the cache when prompted for ingots), because that second `Target()` call bypassed the broken top-level check. The file also had two near-duplicate copies of the bone/ingot crafting logic (apparent leftover from a bad merge when cache integration was added), including a fallthrough where "no anvil nearby" would silently retry the same target as a plain ingot craft. Rewrote `use_hammer` to check for the cache objtype first (matching the established pattern across the codebase), and factored the repeated ingot-targeting logic into `ResolveIngotTarget(character)` and the anvil-proximity check into `NearAnvil(character)`.
- **Crafter boosts (`make_crafter_boosts.src`)**: the smithy retort (crafting Oil/Alloy/Varnish/Compound/Recharge Powder from Brimstone/Bloodspawn/Daemon Bone/Bat Wing/Dragon's Blood plus an ingot, log, hide, or gem) had never been integrated with the Omega Cache — it didn't include `resourcemanager.inc` and used the plain backpack-only `FindSubstance`/`ConsumeSubstance` helpers throughout, so it would simply stop (insufficient-materials path) if a player ran out of the targeted material in their backpack, regardless of whether a nearby cache had more. Added `resourcemanager`/`omegacache_utils` includes and converted both the explicitly-targeted material and the implicit fixed reagent (which the player never targets directly) to `ResourceRequest`s:
  - The initial target and the Brimstone-combine second target now both check for `OMEGACACHE_OBJTYPE` and route through `SelectMaterialFromCache()`, matching the established pattern.
  - A new `MakeReagentRequest(who, objtype)` builds a backpack-first, cache-fallback `ResourceRequest` for the fixed reagent (mirrors `MakeBackpackRequest()`, but without needing a physical item reference, since the reagent is never targeted by the player).
  - `LeaseResource`/`ExtendResourceLease`/`ReleaseResourceLease`/`ConsumeResource`/`GetAvailableResource` now drive both materials through the crafting loop, matching `MakeBlacksmithItems`'s pattern; the loop's early-exit paths (missing empty flask, can't reach it, can't reserve it) now `break` instead of `return`, so `AutoLoop_finish()` always runs.
  - A small `ObjtypeStruct(objtype)` helper lets the existing item-based `IsIngot`/`IsLog`/`IsGem2` (which only ever read `.objtype` off their argument) be reused against a bare objtype from a cache selection, instead of duplicating their range logic locally.
  - Scope note: the empty-flask consumption for the Recharge Powder product line remains backpack-only (unchanged) — flasks were out of scope for this fix.

---

## 11. Omega Cache - Potion Categorization Fix

### Files Changed (uncommitted)

| File | Change |
|------|--------|
| `pkg/opt/omegacache/categories.cfg` | Added 12 missing leveled-potion objtypes to the `Potions` category |

### Notable Functional Changes

- `categories.cfg`'s `Potions` category had two adjacent documented ranges — "Mage-class potion variants (0xFF19-0xFF3F)" ending at `0xFF40`, and "AlchemyPlus potions (0xFF4E-0xFF95, 0xFFA2)" starting at `0xFF4E` — with a gap at `0xFF41`-`0xFF4D` that neither range covered. That gap is exactly where `pkg/std/alchemy/itemdesc.cfg`'s leveled potion tiers live: Strength Potion [Level 2] (`0xFF41`), Greater Strength Potion [Level 1-3] (`0xFF42`-`0xFF44`), Cure Potion [Level 1-3] (`0xFF45`-`0xFF47`), and Greater Cure Potion [Level 1-5] (`0xFF48`-`0xFF4C`). Since `BuildCategoryMap()` in `omegacache.inc` defaults any unmatched objtype straight to `"Other"`, these 12 potions were showing up in the wrong category bucket in the cache UI. Added all 12 objtypes to the `Potions` category, closing the gap. Confirmed via a full cross-reference of `pkg/std/alchemy/itemdesc.cfg` against `categories.cfg` that no other alchemy objtypes have a similar gap. Cosmetic UI-grouping fix only — no gameplay value changes.

---

## 12. Gameplay Tuning and Script Fixes

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/alchemyplus/newpotions.src` | Protection-style potion duration increased significantly |
| `pkg/opt/holybook/removecurse.src` | Remove Curse success chance changed; paladins now use magery and magic resistance for the roll |
| `pkg/opt/shilhook/shilhook.src` | Skill gain and difficulty checks refactored to use a separate gain skill and a clearer chance calculation |
| `pkg/packethooks/speech/receivespeechhook.src` | Unicode speech packet decoding now uses the native Unicode string path instead of the older CChrZ conversion |
| `pkg/std/treasuremap/treasure.cfg` | One buried treasure coordinate was adjusted |
| `pkg/opt/alryc/README.txt` | Stale README removed during the command and tooling reshuffle |
| `scripts/ai/noble.src` | Nobles now begin wandering immediately after spawn |
| `scripts/ai/person.src` | Generic town NPCs now begin wandering immediately after spawn |
| `scripts/ai/setup/criersetup.inc` | Crier setup now guards missing config more safely and defaults the flee point cleanly |
| `scripts/ai/townperson.src` | Town NPC startup now begins wandering immediately after spawn |
| `scripts/ai/townsfolk.inc` | Townfolk helper updated to stay aligned with the new spawn and movement behavior |
| `scripts/EquipTemplateValidation.src` | Boot-time equip-config validator deduplicated: removed a dead `else` branch, extracted a shared `ValidateEquipEntries()` used across Armor/Weapon/Equip instead of three near-identical inline blocks |

### Notable Functional Changes

- Protection effects now last much longer; the underlying strength is unchanged but the duration calculation was extended.
- Remove Curse is now more class-aware:
  - paladins use magery and magic resistance,
  - other casters still use the earlier item-identification-based path.
- Skill gain now uses a cleaner distinction between the skill used for the difficulty check and the skill used for gain calculations.
- The speech hook fix keeps staff speech logging working even when the packet contains Unicode speech data.
- Town NPCs no longer sit idle right after spawn; they start their wander cycle immediately.
- Crier setup became more defensive around missing NPC config entries.
- `EquipTemplateValidation.src` is a one-shot boot-time validator (~1,279 entries) — the cleanup is a code-quality/maintenance change, not a meaningful perf contributor on its own.

---

## 13. Support Data and Tooling

### Files Changed

| File | Change |
|------|--------|
| `ZHO-DataBackup.ps1` | Data backup destination moved to the Koofr path; the log backup line is now commented out |
| `config/command_synopses.cfg` | Synopsis coverage refreshed for the new staff commands and the updated `.areas`, `.go`, `.restartspawnpoint*`, `.playerruntowns`, `.whereat`, and related entries |
| `pkg/opt/alryc/textcmd/test/gotoboat.src` | New developer command to list boats and teleport to the selected tillerman |
| `pkg/opt/alryc/textcmd/test/gotomulti.src` | New developer command to list house multis and teleport to the selected sign |
| `pkg/opt/alryc/textcmd/test/makecheck.src` | New developer command to create a bank cheque |
| `pkg/opt/alryc/textcmd/test/whereat.src` | New developer command to inspect the location details of a selected mobile, item, or map tile |
| `pol.cfg` | Profiling and sysload watchers were enabled for better live diagnostics |

### Notable Functional Changes

- The backup script now writes to the Koofr-backed destination used by the shard.
- The command synopsis file now covers the new or updated admin and developer commands so they show up in the command browser and help surface.
- The new `alryc` test commands are staff and developer tools and are not intended to affect regular gameplay.

---

## 14. Staff Tooling - Character/Account Audit Panel and Name/Death/Poison/Note Tracking

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/admin/pkg.cfg` | New minimal package, created solely to give the new datafile a registered namespace (`data/ds/admin/`) |
| `pkg/opt/admin/include/adminpanel.inc` | New shared data layer: name/death/poisoning/account-note history recording, retrieval, and summary building |
| `pkg/opt/admin/textcmd/test/testadminpanel.src` | New `.testadminpanel` staff gump: account browsing and full character audit history |
| `config/cmds.cfg` | Added `DIR pkg/opt/admin/textcmd/test` under the `Test` cmdlevel block |
| `scripts/include/NameChecker.inc` | Added `NameCheckFailureReason(reason_code)` to translate a `CheckName()` rejection code into a human-readable reason |
| `scripts/textcmd/gm/setprop.src` | Fixed a trailing-space bug in the value-rebuild loop; `.setprop name` now shows the real rejection reason and records the change |
| `scripts/textcmd/admin/setname.src` | `.setname` now shows the real rejection reason and records the change |
| `scripts/textcmd/seer/info.src` | `.info`'s rename button and `fixnameguild` now show the real rejection reason (rename button) and record the change |
| `scripts/textcmd/gm/changename.src` | `.changename` now records the change |
| `scripts/textcmd/test/editcharacter.src` | `.editcharacter` now records the change |
| `pkg/commands/commands/gm/mobedit.src` | `mobedit`'s name field now records the change |
| `pkg/opt/spawnpoint/textcmd/admin/newmobedit.src` | `newmobedit`'s name field now records the change |
| `scripts/misc/namechanger.src` | The player rename gump now records the change |
| `scripts/items/racegate.src` | Race-gate name suffixing now records the change |
| `pkg/opt/roleplaying/rperstone.src` | RPer faction join (`[RPer]` suffix) now records the change |
| `scripts/textcmd/admin/removerper.src` | RPer faction removal (name restore) now records the change |
| `scripts/textcmd/admin/admin.src` | `admin.src`'s `fixnameguild` now records the change; the NOTES action now records an account note |
| `scripts/misc/chrdeath.src` | Character death now records a death entry (killer, killer's account, coordinates) |
| `scripts/include/dotempmods.inc` | `SetPoison()` now records a poisoning entry alongside the existing `PoisonedBy` cprop |
| `scripts/textcmd/coun/notes.src` | `.notes` now records an account note (Staff-initiated) |
| `pkg/opt/loot/antiloot.inc` | `AutoJail()` now records an account note (System-initiated) |
| `pkg/opt/spawnpoint/textcmd/admin/despawn.src` | Fixed `program` name (was still `forcespawn`, copy-pasted from that file); added a null/invalid-spawnpoint guard |
| `pkg/opt/spawnpoint/textcmd/admin/primespawn.src` | Same `program` name fix and null/invalid-spawnpoint guard as `despawn.src` |
| `pkg/opt/spawnpoint/textcmd/admin/forcespawn.src` | Added the same null/invalid-spawnpoint guard |
| `pkg/opt/spawnpoint/textcmd/admin/gotomobtype.src` | Removed a leftover debug `Print("stuff")` call |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | `ReorderLocationsForDisplay()` rewritten from the same O(n^2)*8-pass shape fixed for `.go` in section 9 to a single O(n) dedupe-and-bucket pass |

### Background

Building the audit panel required first finding every place in the codebase where a player character's name can actually change, so each one could be wired into the new tracking layer; that sweep turned up a live bug (`.setprop`'s trailing-space append, section 8 already covers the underlying `CheckName` bad-spacing rejection it was tripping) and several small pre-existing bugs in unrelated spawnpoint commands that were touched only incidentally while adding `RecordCharacterNameChange` calls to `newmobedit.src`'s neighbors.

### Notable Functional Changes

**New staff command `.testadminpanel` (Test cmdlevel):**
- Gump flow: choose "Account" -> pick a letter A-Z (5-column grid, dynamically sized to the letter count so it can't overflow) -> paged account list (8 per page, each row showing the account name as a button plus its IP and last login date) -> character list for that account -> character detail.
- `ShowCharacterList` (the account-landing screen) shows the account name, a "Notes" subtitle with the full word-wrapped latest account note (via the existing `GFWordWrap()` utility, so the gump is sized to the actual wrapped line count instead of truncating), an attribution line (`"- <initiator>, <timestamp>"` or `"- Unknown"`), a running count of stored note records, and then the account's characters as buttons.
- `ShowCharacterDetail` shows a "Character Information" title with "Names", "Killed By", and "Poisoned By" subtitled sections (10/5/5 most-recent-first respectively), each ending with a "more in datafile" total count.

**New shared data layer (`pkg/opt/admin/include/adminpanel.inc`):**
- One DataFile per account (`AdminPanelFilespec`, under `data/ds/admin/`), created in a dedicated new `admin` package purely because DataFile filespecs must resolve to a real, registered `pkg.cfg` package name.
- Name and death/poisoning history is keyed per character serial (a reserved `"_account_"` element key holds account-wide note history instead), so history stays attached to a character across renames rather than being keyed by name.
- Every recorded entry stores a timestamp (`FormatEpochTimestamp`), the recording script's name (`CurrentScriptName()`, via `GetProcess(GetPid()).name` - the same idiom `staff.inc`'s `LogCommand` already uses), and an initiator classification (`Staff`/`Player`/`System`).
- `TrimHistoryToLimit` caps every history array at 100 entries (`ADMINPANEL_MAX_HISTORY`), erasing from the front, applied on every write so none of the four tracked histories can grow unbounded.
- `RecordCharacterNameChange` only records when `mobile.isa(POLCLASS_MOBILE) && mobile.acct` - NPCs have no account and are never recorded, confirmed no hook fires on any NPC/template renaming path.
- The first time a character's name history is recorded, if history is empty, the character's pre-change name is bootstrapped in as an "unknown"-origin entry (blank script/initiator/timestamp) so a character that existed before this tracking was added isn't misattributed a fabricated origin.
- `RecordCharacterDeath` resolves the killer's account independently via `SystemFindObjectBySerial(killer_serial, SYSFIND_SEARCH_OFFLINE_MOBILES)` rather than depending on `chrdeath.src`'s existing online-only name resolution, so the killer's account still shows if they've since logged off.
- `RecordAccountNote`/`GetLatestNoteInfo`: the account's existing single `Notes` cprop remains the untouched source of truth for the note itself; a parallel `notes_history` array (also capped at 100) records every write with attribution. `GetLatestNoteInfo` cross-references the two by comparing note text and falls back to blank attribution if they don't match, so a note set through any not-yet-wired path is shown as `"Unknown"` rather than misattributed to the wrong staffer.

**Wiring - every character rename site found in the codebase now calls `RecordCharacterNameChange`, scoped to accounts only:**
- Staff-initiated: `.setprop name`, `.setname`, `.info`'s rename button, `.info`'s `fixnameguild`, `admin.src`'s `fixnameguild`, `.changename`, `.editcharacter`, `mobedit`/`newmobedit`, `.removerper`.
- Player-initiated: the rename gump (`namechanger.src`), race-gate name suffixing (`racegate.src`), and RPer faction join (`rperstone.src`, appending `" [RPer]"`).
- Account notes (`RecordAccountNote`): `.notes`, `.info`'s NOTES action, and `admin.src`'s NOTES action (all Staff-initiated); `antiloot.inc`'s `AutoJail()` (System-initiated, fires on repeat town-looting offenses).

**Bug fixes surfaced during the sweep:**
- `.setprop`'s value-rebuild loop appended a trailing space after every word, including the last (e.g. `"Paladin Rahl One "`), which silently tripped `CheckName`'s bad-spacing rejection while showing a generic "already in use" message that masked the real cause. Fixed the trailing space (`pval := pval[1, len(pval)-1];`) and added `NameCheckFailureReason()` so `.setprop`, `.setname`, and `.info`'s rename button all now show the actual rejection reason.
- `despawn.src` and `primespawn.src` both still had `program forcespawn(...)` as their program declaration (evidently copy-pasted from `forcespawn.src` when those two commands were created) instead of matching their own filenames; corrected, and a null/invalid-spawnpoint guard was added to both plus `forcespawn.src` itself.
- Removed a leftover debug `Print("stuff")` call in `gotomobtype.src`.
- `restartspawnpointarea.src` had its own copy of the same O(n^2)*8-pass location-reordering logic documented in section 9 for `.go`; rewritten to the identical single O(n) dedupe-then-bucket pattern (dedupe by `Key`, bucket by type label, emit known types in priority order, then unrecognized-type entries in original encounter order).

---

## 15. Exhaustive File-by-File Change List

All files changed in `Patch-1.0.7..HEAD`:

| File | Notes |
|------|-------|
| `ZHO-DataBackup.ps1` | Backup destination moved; log backup line commented out |
| `config/command_synopses.cfg` | Synopsis refresh for new and updated admin and developer commands |
| `config/golocs_by_id.cfg` | New generated go-location index by region id |
| `pkg/opt/alchemyplus/newpotions.src` | Protection duration extended |
| `pkg/opt/alryc/README.txt` | Removed stale README |
| `pkg/opt/alryc/textcmd/test/gotoboat.src` | New boat location and teleport developer tool |
| `pkg/opt/alryc/textcmd/test/gotomulti.src` | New multi location and teleport developer tool |
| `pkg/opt/alryc/textcmd/test/makecheck.src` | New cheque creation developer tool |
| `pkg/opt/alryc/textcmd/test/whereat.src` | New target-inspection developer tool |
| `pkg/opt/admin/pkg.cfg` | New package created to host the audit-panel datafile namespace |
| `pkg/opt/admin/include/adminpanel.inc` | New name/death/poison/note tracking data layer |
| `pkg/opt/admin/textcmd/test/testadminpanel.src` | New `.testadminpanel` staff audit gump |
| `pkg/opt/areas/EnterAreaDelay.src` | Switched to the new area-policy resolution flow |
| `pkg/opt/areas/LeaveArea.src` | Switched to the new area-policy resolution flow |
| `pkg/opt/areas/areaban.src` | Updated for the new area-policy path |
| `pkg/opt/areas/areas.cfg` | Area policy and region data refresh, including castle fixes |
| `pkg/opt/areas/include/areapolicy.inc` | Policy resolver/cache layer, plus realm sanitization and mask-value cache |
| `pkg/opt/areas/include/areapolicy.inc.bak` | Backup of the pre-rewrite policy layer |
| `pkg/opt/areas/textcmd/admin/areas.src` | Rewritten area policy admin gump and flag editor |
| `pkg/opt/crafterboost/make_crafter_boosts.src` | Full Omega Cache integration added |
| `pkg/opt/holybook/removecurse.src` | Revised Remove Curse chance logic |
| `pkg/opt/loot/antiloot.inc` | `AutoJail()` now records an account note (System-initiated) |
| `pkg/opt/omegacache/categories.cfg` | 12 leveled-potion objtypes added to the Potions category |
| `pkg/opt/roleplaying/rperstone.src` | RPer faction join now records a name change |
| `pkg/opt/shilhook/shilhook.src` | Skill gain and difficulty refactor |
| `pkg/opt/spawnpoint/config/groups.cfg` | Spawnpoint group config update |
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | Shared spawnpoint restart helper |
| `pkg/opt/spawnpoint/textcmd/admin/despawn.src` | Fixed `program` name mismatch, added null/invalid-spawnpoint guard |
| `pkg/opt/spawnpoint/textcmd/admin/forcespawn.src` | Added null/invalid-spawnpoint guard |
| `pkg/opt/spawnpoint/textcmd/admin/gotomobtype.src` | Removed leftover debug `Print("stuff")` call |
| `pkg/opt/spawnpoint/textcmd/admin/newmobedit.src` | Name field now records a name change |
| `pkg/opt/spawnpoint/textcmd/admin/primespawn.src` | Fixed `program` name mismatch, added null/invalid-spawnpoint guard |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpoint.src` | Restart command now uses the helper |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | New region-wide restart command; `ReorderLocationsForDisplay` also rewritten to O(n) |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointmax.src` | Force-fill restart command updated |
| `pkg/opt/townstones/textcmd/admin/createtownstone.src` | Creation workflow tightened and state restoration added |
| `pkg/opt/townstones/textcmd/admin/removetownmember.src` | New town member removal command |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | Major town status, member management, and runtime state expansion |
| `pkg/opt/townstones/tstone.src` | `Citizenship`/`CanselCityzenship` now duplicate-check before mutating a name |
| `pkg/packethooks/speech/receivespeechhook.src` | Unicode speech packet decode fix |
| `pkg/std/blacksmithy/make_blacksmith_items.src` | Omega Cache targeting fixed, dead code removed |
| `pkg/std/housing/housedeed.src` | Region-based city/dungeon/shrine/graveyard placement restrictions |
| `pkg/std/treasuremap/treasure.cfg` | One treasure location adjusted |
| `pkg/commands/commands/gm/mobedit.src` | Name field now records a name change |
| `pol.cfg` | Profiling and sysload watchers enabled |
| `pythonscripts/generate_golocs_by_id.py` | New generator for indexed go-location config |
| `regions/regions.cfg` | Large region metadata expansion and normalization |
| `scripts/EquipTemplateValidation.src` | Dead branch removed, shared validation helper extracted |
| `scripts/ai/noble.src` | Nobles now wander immediately after spawn |
| `scripts/ai/person.src` | Townsfolk now wander immediately after spawn |
| `scripts/ai/setup/criersetup.inc` | Safer crier config handling |
| `scripts/ai/townperson.src` | Town NPCs now wander immediately after spawn |
| `scripts/include/NameChecker.inc` | Town-suffix exploit closure, case-insensitive dedupe, bad-spacing rejection, caching; `NameCheckFailureReason()` added |
| `scripts/include/anchors.inc` | Anchor and lookup helpers updated for new area behavior |
| `scripts/include/areas.inc` | Area helper rewrite to match the new policy layer |
| `scripts/include/dotempmods.inc` | `SetPoison()` now records a poisoning entry |
| `scripts/include/string.inc` | `SortMultiArrayByIndex` rewritten to O(n log n) merge sort |
| `scripts/include/townsfolk.inc` | Townfolk helper alignment update |
| `scripts/items/racegate.src` | Race-gate name suffixing now records a name change |
| `scripts/misc/chrdeath.src` | Character death now records a death entry |
| `scripts/misc/namechanger.src` | `force_town_check := 1`, "bad spacing" error message; rename gump now records a name change |
| `scripts/misc/oncreate.src` | `force_town_check := 1` |
| `scripts/textcmd/admin/admin.src` | `fixnameguild` now records a name change; NOTES action now records an account note |
| `scripts/textcmd/admin/removerper.src` | RPer removal now records a name change |
| `scripts/textcmd/admin/setname.src` | Now runs through `CheckName`, PC-scoped; shows real rejection reason; records name changes |
| `scripts/textcmd/coun/go.src` | `ReorderLocationsForDisplay` rewritten to O(n); `.go` also rebuilt around indexed go locations and range fallback (earlier in the patch) |
| `scripts/textcmd/coun/notes.src` | `.notes` now records an account note |
| `scripts/textcmd/gm/changename.src` | `.changename` now records a name change |
| `scripts/textcmd/gm/setprop.src` | `.setprop name` now runs through `CheckName`, PC-scoped; trailing-space bug fixed; shows real rejection reason; records name changes |
| `scripts/textcmd/seer/info.src` | Rename button now runs through `CheckName`, PC-scoped, shows real rejection reason, and records name changes; `fixnameguild` and NOTES action also record |
| `scripts/textcmd/test/editcharacter.src` | Name field now records a name change |
| `config/cmds.cfg` | New `DIR` entry for `.testadminpanel` under the `Test` cmdlevel |

---

## 16. Risk and Regression Notes

1. `regions/regions.cfg` and `config/golocs_by_id.cfg` are now tightly coupled. Any future region edit should regenerate the go-location index rather than hand-editing the generated file.
2. The area-policy caches (parsed-line cache and the new mask-value cache) use global properties and, for the parsed-line cache, a line-count fingerprint. If `areas.cfg` changes shape, cache invalidation needs to remain intact or stale policy data can persist. The mask-value cache is invalidated precisely on `SetPolicyMask`/`PruneStaleRealmPolicies` writes, so it should stay correct as long as no other code path writes the `AREA_POLICY_MASK_PROP` datafile property directly.
3. Town member removal now touches citizen lists, population, and election or poll state. That makes the new cleanup path more complete, but it also raises the importance of testing member removal against live stones and offline accounts.
4. `RestartSpawnPointWithMode(...)` can checkpoint repeatedly during max-fill mode. That is intentional, but it means the new region-wide restart command should still be treated as a staff maintenance tool rather than a routine hot command.
5. `GetKnownTownNames()`'s cache has no automatic invalidation wired to `createtownstone`/region edits yet — a brand-new town's name won't be recognized as a "town name" for the free-name-choice block, nor will its residents' suffixes be stripped for dedupe purposes, until `InvalidateKnownTownNamesCache()` is called or the server restarts. Low practical risk today since new town creation is infrequent and staff-driven, but worth wiring up if that becomes a live pain point.
6. The town-suffix duplicate-name fixes and the house-placement region checks are both new *rejection* paths in previously-permissive flows — worth a live smoke test on a known player-run town (join/leave/rename) and a known city-adjacent building parcel before this goes fully live, since both change behavior at the boundary rather than just internals.
7. The blacksmithy and crafter-boost Omega Cache fixes change previously-broken or entirely-missing behavior into working behavior — this is a net-new capability for players (the failure mode was "doesn't work," not "we're removing something"), but should still get a quick live test: hammer-to-cache targeting for a plain ingot craft, bone-armor dual-material with the cache as either material, and a crafter-boost upgrade with the target material sourced from a cache.
8. The `.go` and shared-sort algorithmic rewrites are drop-in replacements with verified-identical output ordering (hand-traced on a small example for the merge sort; the `.go` reorder was verified against its old semantics directly). Risk is low, but both are hot, frequently-run staff commands, so a live spot check (a `.go` menu with a large realm, and `.gotomulti`/`.gotoboat` with the current multi/boat counts) is still worthwhile.
9. The potion-categorization fix is UI-only (Omega Cache category grouping) and carries no gameplay risk.
10. The audit panel's four history arrays (names, deaths, poisonings, account notes) hard-trim to 100 entries, erasing the oldest first. This is intentional to bound datafile growth, but it means very active characters/accounts will lose their oldest audit history over time rather than growing indefinitely — worth knowing before staff rely on it for long-term investigation.
11. `GetLatestNoteInfo()`'s attribution is derived by comparing the account's live `Notes` cprop text against the last `notes_history` entry; if a note is ever set through a path that isn't wired to `RecordAccountNote` (or was set before this feature existed), the panel will correctly show blank/"Unknown" attribution rather than a wrong name — by design, but staff should not read a blank attribution as "nobody knows," only as "not recorded through a tracked path."
12. `.testadminpanel` is read/write-audit-only and gated to the `Test` cmdlevel; it does not change any player-facing behavior. The only behavior changes with any player visibility from this section are the `.setprop`/`.setname`/`.info` rejection-message wording (now shows the real reason instead of a generic one) and the `despawn`/`primespawn`/`forcespawn` null-spawnpoint guard — both staff-tool-only surfaces.
13. This document's commit range (`9f8189f..7c27772`) includes two commits not detailed here: `b347427` (already covered by sections 9-11) and `232ee95` ("Townstone fixes, Minstel and townsfolk fix" — a townstone treasury-race-condition fix, election/mayor-removal cleanup, a candidate-list pairing bug fix, and a townlist-bootstrap self-heal on login). If player-facing patch notes are needed for `232ee95`, it should get its own review pass rather than being folded in here secondhand.
