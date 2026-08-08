# Developer Changelog - v1.1.1
**Range:** `f52370c` (origin/Patch-1.1.0) -> `7107d6d` (HEAD)  
**Branch:** Patch-1.1.1  
**Date:** 2026-08-07 -> 2026-08-08  
**Commits in range:** 2 (excluding the boundary merge commit, which carries no new 1.1.1 content)  
**Files changed (committed):** 48 (+3201 / -567)

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Stability - DataFile Handle Leak Fixes (Email + Area Policy)](#3-stability---datafile-handle-leak-fixes-email--area-policy)
4. [New Staff Tools - Load/Stress-Testing and Leak Diagnostics](#4-new-staff-tools---loadstress-testing-and-leak-diagnostics)
5. [CheckCity() Rewrite - Realm-Scoped, regions.cfg-Driven City Detection](#5-checkcity-rewrite---realm-scoped-regionscfg-driven-city-detection)
6. [New "Suburb" Region Type - Farmland/Outbuilding Reclassification](#6-new-suburb-region-type---farmlandoutbuilding-reclassification)
7. [House Placement - MultiID Resolution Fix and Region-Check Refactor](#7-house-placement---multiid-resolution-fix-and-region-check-refactor)
8. [House Placement - Per-Account House Limit (New, Cap 5)](#8-house-placement---per-account-house-limit-new-cap-5)
9. [NPC AI - Sleep-Mode Timing and Wake-Radius Tuning](#9-npc-ai---sleep-mode-timing-and-wake-radius-tuning)
10. [New House Escrow System - Staff Demolish-and-Escrow, Player Claim Command](#10-new-house-escrow-system---staff-demolish-and-escrow-player-claim-command)
11. [Player-Vendor Escrow - Shared Module Refactor and Orphaned-Merchant Fixes](#11-player-vendor-escrow---shared-module-refactor-and-orphaned-merchant-fixes)
12. [New Admin Command - Teleporter Serial Backfill Migration](#12-new-admin-command---teleporter-serial-backfill-migration)
13. [Tamed Pets - Follow() Runaway-Watch Diagnostics Removed](#13-tamed-pets---follow-runaway-watch-diagnostics-removed)
14. [CmdLevel Constants - Renumbered to Match the Actual Ladder](#14-cmdlevel-constants---renumbered-to-match-the-actual-ladder)
15. [Misc Small Fixes](#15-misc-small-fixes)
16. [Exhaustive File-by-File Change List](#16-exhaustive-file-by-file-change-list)
17. [Risk and Regression Notes](#17-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.1 is a large patch built from two commits. `6817955` ("Bunch of fixes to try and stop the shard crashing", 2026-08-07) is a stability pass: it plugs a class of DataFile handle leaks across the email and area-policy systems, adds five new staff-only load/stress-testing and leak-diagnostic tools, rewrites `CheckCity()` to be realm-scoped and driven by `regions.cfg` instead of a hardcoded, incomplete box list, splits farmland/outbuilding regions into a new "Suburb" type so they stop being treated as city, fixes a `GetMultiDimensions()` misuse in house placement that could let houses go undetected inside city/dungeon/shrine/graveyard regions, adds a new per-account house limit (5), and retunes NPC sleep-mode timing. `7107d6d` ("Patch 1.1.1 housing and teleporters fix / housing escrow / taming animals follow speeds", 2026-08-08) is a feature commit: it adds an entirely new staff "Demolish and Escrow" house teardown plus a player-facing `.houseescrow` claim command, refactors the pre-existing player-vendor escrow system into a shared module (fixing two paths that could orphan a merchant's inventory), rewrites house teleporter tracking to use an explicit serial list instead of an unreliable `house.items`-membership check, adds a one-time admin migration command to backfill that new teleporter list for already-placed houses, removes the 1.1.0-era `Follow()` runaway-diagnostic instrumentation (the underlying rate cap itself is untouched), and renumbers the `CMDLEVEL_*` named constants to match this shard's actual 6-level ladder (a phantom "Shard Mom" level and an off-by-one had existed in the constants file, though not in the engine-assigned levels themselves).

---

## 2. Commit Timeline

| Hash | Date | Message |
|---|---|---|
| `f52370c` | 2026-08-07 | Merge pull request #316 from Andries1985/Patch-1.1.0 (boundary - no new 1.1.1 content) |
| `6817955` | 2026-08-07 | Bunch of fixes to try and stop the shard crashing |
| `7107d6d` | 2026-08-08 | Patch 1.1.1 housing and teleporters fix / housing escrow / taming animals follow speeds |

---

## 3. Stability - DataFile Handle Leak Fixes (Email + Area Policy)

### Files Changed

| File | Change |
|---|---|
| `pkg/systems/email/chardelete.src` | `OnDelete()` - `UnloadDataFile()` on every exit path |
| `pkg/systems/email/commands/gm/inspectmail.src` | `ReadMail()` - `UnloadDataFile()` before all 5 return points |
| `pkg/systems/email/email.src` | `GetMail()` - `UnloadDataFile()` before both return points |
| `pkg/systems/email/logon.src` | `Logon()` - `UnloadDataFile()` before both return points |
| `pkg/systems/email/reconnect.src` | `Reconnect()` - same pattern as `logon.src` |
| `pkg/systems/email/webmail/webmail.src` | `GetInbox()` - `UnloadDataFile()` before final return |
| `pkg/opt/sysbook/start.src` | adds `UnloadDataFile("staticbooks")` at the end |
| `pkg/opt/areas/include/areapolicy.inc` | `GetPolicyMask()`, `SetPolicyMask()`, `PruneStaleRealmPolicies()` - `UnloadDataFile()` added on every exit path |

### Overview

Every function above opened a DataFile handle (`"Emails"`, `"AddressBooks"`, `"BlockLists"`, `"staticbooks"`, or a realm's area-policy datafile) and returned without calling `UnloadDataFile()` on at least one exit path - almost always an early-return/error branch, since the "happy path" return in most of these already unloaded correctly. Each open handle that's never unloaded holds a live OS file handle for the life of the process. Given how frequently `logon.src`/`reconnect.src` run (every character login/reconnect) and how often area-policy lookups happen (per [[project-known-engine-constraints]]-adjacent AI/movement code calling into `ResolvePolicyAtLocation()`), this is a plausible contributor to gradual handle exhaustion and an eventual crash under sustained uptime - the stated motivation for this commit.

### Notable Functional Changes

- No behavioral/logic change on any success path - every fix is purely adding a missing `UnloadDataFile()` call before a `return` that previously skipped it.
- `areapolicy.inc`'s `SetPolicyMask()` and `PruneStaleRealmPolicies()` each had multiple distinct early-return branches (`DFFindElement` failure, `GetRealmAreaLines` failure, `keys` failure) that independently needed the fix, not just one.

### Expected Impact

No player-visible change. Reduces (and per the new `dfleaktest`/`emailstressworker`/`areastressworker` diagnostics in section 4, is intended to be verifiable as having reduced) open-handle growth over long uptime, which is one plausible contributor to the crashes this commit is titled after.

---

## 4. New Staff Tools - Load/Stress-Testing and Leak Diagnostics

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/alryc/textcmd/test/dfleaktest.src` | New - `.dfleaktest` |
| `pkg/opt/alryc/textcmd/test/aimovementsim.src` | New - `.aimovementsim` |
| `pkg/opt/alryc/textcmd/test/shardstress.src` | New - `.shardstress` |
| `pkg/opt/alryc/stresstest/areastressworker.src` | New - spawned worker, not a direct command |
| `pkg/opt/alryc/stresstest/emailstressworker.src` | New - spawned worker, not a direct command |
| `pkg/opt/alryc/textcmd/test/housezoneaudit.src` | New - `.housezoneaudit` (see section 7) |
| `config/command_synopses.cfg` | Regenerated for `aimovementsim`, `dfleaktest`, `housezoneaudit`, `shardstress` |

### Overview

Five new Developer-only (CmdLevel 5) diagnostic tools, built specifically to reproduce and verify the fixes in this patch under sustained/concurrent load rather than a single serial test loop, since the live crash reports this patch responds to were plausibly load-dependent.

### Notable Functional Changes

- `dfleaktest`: runs 3000 iterations of open/lookup/unload against both the Britannia area-policy datafile and the `"Emails"` datafile (6000 total open/unload cycles), yielding every 100 iterations. Operator watches `pol.exe`'s Handles count in Task Manager before/after - flat count confirms the section 3 fix holds.
- `aimovementsim`: 20,000 calls to `ResolvePolicyAtLocation()` at random coordinates, batched 50 at a time with a 300ms pause between batches (~120s total), forcibly invalidating the policy-mask cache every 200 calls so it keeps hitting the real datafile-backed lookup instead of a warm cache.
- `shardstress`: spawns 20 `areastressworker` instances and 10 `emailstressworker` instances, each running for 15 minutes, to simulate sustained mixed player/AI load.
- `areastressworker` (spawned, not a command): loops for a given duration, invalidating the realm's policy-mask cache every 15 calls and calling `ResolvePolicyAtLocation()` at random coordinates - reproduces many concurrent instances independently warming/missing their own cache, closer to live conditions than one serial loop.
- `emailstressworker` (spawned, not a command): loops for a given duration doing open/lookup/unload cycles against the `"Emails"` datafile, simulating repeated login/reconnect churn.

### Expected Impact

Staff/developer-facing only. No player-visible change. Gives the team a repeatable way to load-test the area-policy cache and email-datafile paths and watch for handle growth going forward.

---

## 5. CheckCity() Rewrite - Realm-Scoped, regions.cfg-Driven City Detection

### Files Changed

| File | Change |
|---|---|
| `scripts/include/checkcity.inc` | `CheckCity()` rewritten; new shared helpers `NormalizeRegionType()`, `IsBoxOverlappingRegionType()`, `IsPointInRegionType()` |

### Overview

`CheckCity(item)` was previously a long `If/Elseif` chain of ~18 hardcoded x/y bounding boxes (Britain, Moonglow, Papua, Delucia, Jhelom, Yew, Empath Abbey, Minoc, Trinsic, Skara Brae, Magincia, Occlo, Buccaneers Den, Nujelm, Vesper, Cove, Wind) with no realm parameter at all, so it could false-positive across realms/facets sharing the same coordinates, and it never covered every city-type region actually defined in `regions.cfg` (Occlo Isle, for instance, matched nothing under the old hardcoded list, and was itself misclassified as `Type POI` rather than `Type City` in `regions.cfg` prior to section 6's fix).

### Notable Functional Changes

- `CheckCity(item)` is now a two-line wrapper around `IsPointInRegionType(item.x, item.y, item.realm, "city")`.
- New `IsBoxOverlappingRegionType(x1, y1, x2, y2, realm, region_type)`: reads `regions/regions.cfg`, filters by normalized region `Type` and (case-insensitively, only when the region specifies one) `Realm`, and returns the first region whose `Range` overlaps the given box, or `0` (fail-open, matching the old behavior when config was unavailable) if none match.
- New `IsPointInRegionType(x, y, realm, region_type)`: convenience wrapper calling the above with a degenerate zero-size box.
- New `NormalizeRegionType(region_type)`: lowercases/canonicalizes a region-type string against the known set `poi`, `city`, `suburb`, `dungeon`, `graveyard`, `jail`, `none` (adds `"suburb"`, see section 6).
- These three helpers are shared with `pkg/std/housing/housedeed.src` (section 7) and the new `pkg/opt/alryc/textcmd/test/housezoneaudit.src` (section 7), replacing three independent, duplicated implementations of essentially the same regions.cfg scan.

### Expected Impact

`CheckCity()` is now realm-scoped and matches every `Type=City` region actually defined in `regions.cfg`, not a hardcoded and incomplete subset. Every downstream consumer of `CheckCity()` (`pkg/std/fishing/fishingnet.src` - nets restricted outside cities; `pkg/opt/botanik/maketree.src` - trees blocked in cities; `pkg/std/alchemy/exploder.src` and `pkg/opt/GMItems/cains_exploder.src` - exploding-potion power reduced in cities; `scripts/control/spawnbookcase.src` - bookcase decay only in cities; `scripts/control/lockchests.src` and `pkg/std/provocation/provocation.src`'s `CheckCity()` calls are dead/commented-out code, unaffected) now evaluates against the corrected, realm-aware region data. Combined with section 6's reclassification, this changes which specific locations count as "city" for all of the above.

---

## 6. New "Suburb" Region Type - Farmland/Outbuilding Reclassification

### Files Changed

| File | Change |
|---|---|
| `regions/regions.cfg` | Occlo Isle `POI` -> `City`; 5 farmland/outbuilding region groups `City` -> `Suburb` |
| `pkg/opt/areas/areas.cfg` | Matching `cat=city` -> `cat=suburb` for the same 10 area entries |
| `config/golocs_by_id.cfg` | Matching `Type City` -> `Type Suburb` for the same GoLoc entries |
| `pkg/opt/areas/textcmd/admin/areas.src` | `CATEGORY_ORDER` gains `"suburb"` after `"city"` |
| `scripts/textcmd/coun/go.src` | `type_priority` gains `"Suburb"`; `NormalizeRegionType()` gains a `"suburb"` case |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | Same `type_priority`/normalize additions as `go.src` |

### Overview

`regions.cfg` reclassifies five previously `Type City` region groups - Yew Farmlands (4 ranges), Vesper Outbuildings (2 ranges), Minoc Farmlands (2 ranges), Britain Outer Farmlands, and Skara Brae Farmlands - to a new `Type Suburb`, and separately reclassifies Occlo Isle from `Type POI` to `Type City`. `pkg/opt/areas/areas.cfg` and `config/golocs_by_id.cfg` carry matching reclassifications for the same named areas/GoLocs. `areas.src`'s policy-resolution category order, and both `go.src`'s and `restartspawnpointarea.src`'s location-listing display order, are all updated to insert `"Suburb"`/`"suburb"` immediately after `"City"`/`"city"` so the new type sorts and resolves consistently with the others.

### Notable Functional Changes

- `areas.src`'s `CATEGORY_ORDER` ordering is load-bearing: `ResolveAreaKeyAtLocation()`/`ResolveAreaMatchAtLocation()` in `areapolicy.inc` walk this list in order and the first bounding-box match wins, so this insertion changes which policy applies at any coordinates where a city and a suburb region overlap.
- `go.src` and `restartspawnpointarea.src` each gain a `"Suburb"` display case in their respective location-normalization functions so suburb regions show a proper label instead of falling through to a generic/blank one.

### Expected Impact

Farmland and outbuilding zones around Yew, Vesper, Minoc, Britain, and Skara Brae are no longer treated as "city" by anything gated on `CheckCity()` or the `city` area category (section 5) - e.g. fishing nets, tree planting, exploding-potion power, and bookcase decay now behave as if those zones are wilderness, not town. Occlo Isle is now correctly recognized as a city region everywhere it wasn't before. The new "Suburb" type also now appears as its own category in `.go` location listings and the spawnpoint-area restart tool's location listings.

---

## 7. House Placement - MultiID Resolution Fix and Region-Check Refactor

### Files Changed

| File | Change |
|---|---|
| `pkg/std/housing/housedeed.src` | `GetHouseFootprintBounds()`, new `ResolveHouseMultiId()`, `IsHousePlacementBlockedByRegionType()` refactored, debug logging added |
| `pkg/opt/alryc/textcmd/test/housezoneaudit.src` | New - `.housezoneaudit` |
| `pkg/opt/townstones/textcmd/admin/createtownstone.src` | `GetRegionRange()` - `GetConfigStringArray` -> `SplitWords(CStr(GetConfigString(...)))` |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | Same one-line change as above |

### Overview

`GetHouseFootprintBounds(housetype, x, y)` called `GetMultiDimensions(housetype)` directly using the house item's objtype. `GetMultiDimensions()` actually needs the raw multi ID, not the item objtype - `itemdesc.cfg` maps every `"House <objtype>"` block to a separate, much smaller `MultiID` (e.g. House `0xA3EF` -> MultiID `0x3F`), and while `CreateMultiAtLocation()`/`TargetMultiPlacement()` already translate this internally, `GetMultiDimensions()` did not. This means footprint-bounds-dependent region checks (`IsHousePlacementBlockedByRegionType`, i.e. the city/dungeon/shrine/graveyard placement guard) could silently receive wrong or failed dimensions for any house whose objtype differs from its raw multi ID, potentially letting a house be placed inside a restricted region undetected.

### Notable Functional Changes

- New `ResolveHouseMultiId(housetype)`: looks up `:housing:itemdesc.cfg`'s `"House <housetype>"` block and returns its `MultiID`, falling back to the unresolved `housetype` if no block/field is found (with debug logging on each fallback).
- `GetHouseFootprintBounds()` now calls `GetMultiDimensions(ResolveHouseMultiId(housetype))` instead of `GetMultiDimensions(housetype)` directly.
- `IsHousePlacementBlockedByRegionType()` no longer inlines its own `regions.cfg` scan - it now calls the shared `IsBoxOverlappingRegionType()` from `checkcity.inc` (section 5).
- `NormalizeRegionType()` moved out of this file entirely into `checkcity.inc`.
- Debug `Print()` logging (prefixed `[housedeed][debug]`/`[houselimit][debug]`) added at several points in `Buildhouse()` and `IsHousePlacementBlockedByRegionType()` - console-only, staff-visible.
- New `.housezoneaudit` command (Developer, CmdLevel 5): scans every realm for placed houses (`ListMultisInBox` + `POLCLASS_HOUSE` filter), recomputes each house's footprint using the same `ResolveHouseMultiId()`/`GetHouseFootprintBounds()` logic as `housedeed.src` (deliberately duplicated to mirror placement-time checks exactly), and reports any house whose footprint overlaps a city/dungeon/shrine/graveyard region - i.e. an audit for houses that were placed illegally before this fix, or that became illegal after a region redefinition. Also includes a one-shot diagnostic trace (`RunRegionDiagnostics()`) built around a specific reported case (towers inside a "Lost Lands" dungeon region).
- `createtownstone.src` and `townbankstatus.src`'s `GetRegionRange()` both switch from `GetConfigStringArray(elem, "Range")` to `SplitWords(CStr(GetConfigString(elem, "Range")))` for parsing the same `Range` config line - consistent with the parsing approach used in the new `checkcity.inc` helpers, though no comment in the diff states a concrete bug this fixes in these two files specifically.

### Expected Impact

House placement's city/dungeon/shrine/graveyard region guard now correctly evaluates footprint bounds for every house type, including the ones whose item objtype differs from their raw multi ID (previously silently broken for those types). `.housezoneaudit` gives staff a way to find and clean up any already-placed houses that slipped through before this fix.

---

## 8. House Placement - Per-Account House Limit (New, Cap 5)

### Files Changed

| File | Change |
|---|---|
| `pkg/std/housing/utility.inc` | New `MAX_HOUSES_PER_ACCOUNT`, `GetAccountHouseCount()`, `IsAccountAtHouseLimit()` |
| `pkg/std/housing/housedeed.src` | `Buildhouse()` - new limit check |
| `pkg/std/housing/changeowner.src` | `ChangeHouseOwner()` - new limit check, plus a separate `owneracct`/`"Houses"` cprop sync bugfix |
| `pkg/std/housing/sign.src` | `ChangeHouseOwner()` - new limit check |
| `pkg/opt/statichousing/ssign.src` | `use_sign()` purchase flow and its own `ChangeHouseOwner()` - new limit check |

### Overview

New feature: an account may not own more than `MAX_HOUSES_PER_ACCOUNT` (5) houses at once. Not described in the commit as a crash fix - it's a scoped feature addition bundled into the same commit.

### Notable Functional Changes

- `GetAccountHouseCount(who)`: resolves the account via `FindAccount(who.acctname)` and sums the `"Houses"` obj-property array's size across all 5 of the account's character slots (`account.GetCharacter(1)` through `(5)`, including offline characters) - a live sum each call, not a maintained counter.
- `IsAccountAtHouseLimit(who)`: exempts staff (`who.cmdlevel >= 4`, matching `Buildhouse()`'s existing city/dungeon/shrine/graveyard staff-exemption threshold), otherwise denies if `GetAccountHouseCount(who) >= 5`.
- Wired into every house-acquisition path found in this range: new-deed placement (`housedeed.src`), ownership transfer via `changeowner.src` and `sign.src`'s in-game ownership-change flow, and static-housing sign purchase (`ssign.src`) - each blocks with "You already own the maximum of 5 houses on this account." before the acquisition completes.
- Separately, `changeowner.src`'s `ChangeHouseOwner()` had a pre-existing bug fixed in the same edit: it set `ownerserial` but never `owneracct`, and never called `AddHouseToCharacter()`/`RemoveHouseFromCharacter()` to keep the `"Houses"` cprop in sync on either the old or new owner - meaning a house transferred via this specific path previously went stale in both parties' house lists (and would have thrown off `GetAccountHouseCount()` for anyone who received a house this way). `sign.src`'s own `ChangeHouseOwner()` already did this correctly and was not affected by the bug.

### Expected Impact

An account cannot acquire a 6th house through deed placement, in-game ownership transfer, or static-housing purchase - existing accounts already over the cap are not retroactively affected (no house is removed), only further acquisition is blocked. Staff (Administrator and above) are exempt. The `changeowner.src` cprop-sync fix means houses transferred through that specific code path now correctly show up in the new owner's (and disappear from the old owner's) house list and account house count going forward.

---

## 9. NPC AI - Sleep-Mode Timing and Wake-Radius Tuning

### Files Changed

| File | Change |
|---|---|
| `scripts/ai/main/killpcsloop.inc` | New `NPC_SLEEP_AFTER_WANDERS := 150` (was a bare `60`) |
| `scripts/ai/main/sleepmode.inc` | New `NPC_SLEEP_WAKE_RADIUS := 20`; wake-on-entered-area radius unified to match |

### Overview

`killpcsloop.inc`'s main AI loop counted consecutive idle cycles (`wanders`, ~2s each via `wait_for_event(2)`) before attempting to put an NPC into sleep mode, previously triggering at 60 (~2 minutes). `sleepmode.inc` had two related but mismatched radius values: a proximity check before actually going dormant (20) and the radius used to re-enable `SYSEVENT_ENTEREDAREA` to wake the NPC back up (18).

### Notable Functional Changes

- `NPC_SLEEP_AFTER_WANDERS` raised from 60 to 150 - NPCs now need ~5 minutes of inactivity (up from ~2 minutes) before `sleepmode()` is even attempted. `sleepmode()` still does its own proximity check before actually going dormant.
- `NPC_SLEEP_WAKE_RADIUS` (20) now used for both the pre-sleep proximity check and the `EnableEvents(SYSEVENT_ENTEREDAREA, ...)` wake trigger (previously 18 for the latter) - both directions are now the same value: "if someone's close enough to prevent sleep, they're close enough to end it."
- The separate hiding-skill branch (stealthy/rogue-type NPCs, `GetEffectiveSkill(me, 21) > 0`) keeps its own short wake radius of 4, unchanged.

### Expected Impact

NPCs stay active longer before going dormant (5 minutes of no player interaction instead of 2), and the radius that wakes a dormant NPC back up is now consistent with the radius that would have kept it awake in the first place, removing a small dead zone (18-20 tiles) where an NPC could go to sleep with a player just barely outside the old wake trigger.

---

## 10. New House Escrow System - Staff Demolish-and-Escrow, Player Claim Command

### Files Changed

| File | Change |
|---|---|
| `pkg/std/housing/houseescrow.inc` | New (1206 lines) - core escrow logic |
| `scripts/textcmd/player/houseescrow.src` | New (405 lines) - `.houseescrow` player/staff command |
| `pkg/std/housing/sign.src` | New gump case 17 "Demolish and Escrow (For Staff Only)"; teleporter-placement rewrite (see also section 12); decay-timestamp field un-commented |
| `pkg/std/housing/signcontrol.src` | Dead decay-check block un-commented and fixed; `CleanupHouseTeleporters()` added to `Demolish()` |
| `pkg/std/housing/setup.inc` | New `HOUSE_DECAY_PERIOD` constant (placeholder, `DECAY` flag itself still 0) |
| `pkg/std/housing/utility.inc` | New `IsLocationInThisHouse()`, `CleanupHouseTeleporters()`, owner-last-login helpers, `ResolveHouseTypeDisplayName()` |
| `pkg/utils/gumps/include/yesNoSizable.inc` | `YesNoVar()` rewritten to also scale gump height to wrapped line count |
| `config/command_synopses.cfg` | New `.houseescrow` entry |
| `config/fileaccess.cfg` | New `.log` file-access grants for the `housing` and `playervendor` packages |

### Overview

A new staff-only "Demolish and Escrow" house teardown, explicitly modeled on but deliberately not sharing code with the pre-existing player-vendor escrow system (section 11) - a comment in `houseescrow.inc` states this is so the two systems "can't drift into each other." When staff use the new sign-gump option, the house's entire contents (secure containers, redeedable furniture groups, Omega Cache deposits, trash-can tokens, any player-merchant NPCs standing inside, and the resulting house deed itself) are swept into per-owner escrow storage instead of being destroyed, and any player it belonged to (or a staff-reassigned recipient) can later reclaim it with the new `.houseescrow` player command.

### Notable Functional Changes

- New sign gump option, case 17, gated `who.cmdlevel < CMDLEVEL_DEVELOPER` -> `"Staff only."` (i.e. requires the top cmdlevel, 5). Blocks first if any nearby item still has `outside == house.serial` set (owner must redeed outdoor furniture first). Two `YesNoVar` confirmations (a second, sterner one if the house has a `"GuildHouse"` flag). Writes an audit line to console (`[houseescrow][audit] ...`) before proceeding.
- The sign gump itself was widened (300 -> 360 height) to add an info section: Risk of Escrow (Yes/No, via `IsOwnerAccountAtRisk()`), Owner Last Seen (via `ResolveHouseOwnerLastLogin()`), House Type (via `ResolveHouseTypeDisplayName()`), and teleporter count (placed/total) - all informational display only in this patch, nothing auto-triggers escrow off the "at risk" flag.
- `DemolishAndEscrowHouse(who, house, sign)`: sets `SCRIPTOPT_NO_RUNAWAY` up front (large houses can trip the 500,000-instruction runaway-script threshold; mitigated with periodic `SleepMS(1)` yields). Resolves the owner (falling back through `owneracct`/sign's `lastownername`, logging `[ORPHANED]` and queuing a GM page via the same shared `gmpages` queue the player-vendor escrow system uses, if resolution fails entirely); creates the house's redeed deed via the same ~40-entry objtype->deed case table `sign.src`'s existing self-service demolish already used (extended with two new entries, `0xA3F1`/`0xA3F2`); destroys structural components; calls the new `CleanupHouseTeleporters()` (section 12); snapshots and classifies every item (secure containers, Omega Cache token, trash cans, redeed-group furniture, generic items) into escrow packs; drains the house's Omega Cache datafile wholesale; force-fires any player-merchant NPCs inside via a `pm_force_fire` event (routed into the *separate* player-vendor escrow system, not this one); removes the house from the owner's character record and destroys the multi; escrows the house deed itself as its own labeled pack.
- Escrow storage: a DataFile per owner (`:housing:houseescrow_<ownerserial>`, package-prefixed deliberately - a bare filespec resolves relative to the calling script's own package, so without the prefix `houseescrow.inc` and `houseescrow.src` would silently address two different datafiles despite an identical bare name), plus a `"House Demolish Escrow"` StorageArea holding the actual item packs (root backpacks tagged with `ownerserial`/`house_serial`/`house_label`/`created` cprops). Index entries are batch-appended (`AppendHouseEscrowIndexBatch`) for O(N) rather than O(N²) behavior - the comment cites a real house with 346 packs.
- `.houseescrow` (`scripts/textcmd/player/houseescrow.src`, Player level, CmdLevel 0): with no argument, shows the caller's own paginated escrow gump (5 entries/page) with "To Bank"/"To Backpack" claim buttons per entry. With a numeric argument and `cmdlevel >= 4`, shows a staff view of another character's escrow entries with a "Reassign to targeted character" button per row instead. Claiming (`ClaimHouseEscrowEntry`) snapshots root children before moving (not live iteration), reports if the bank/backpack destination is full, and prunes the datafile entry once fully drained (partial claims leave the remainder in escrow). Gump button IDs are page-relative row numbers reconstructed to an absolute entry index after the gump returns - explicitly to avoid an ID-collision bug that would otherwise occur past 1000 entries (same fix pattern as section 11's player-vendor escrow gump).
- `signcontrol.src`'s previously dead, commented-out house-decay check (`/* FIXME: ... decay is a string! */`) is un-commented and fixed with a `CInt()` cast; `sign.src` now actively refreshes the `"decay"` obj-property timestamp on every owner/cowner/staff sign click. Both remain dormant in practice since the `DECAY` feature flag itself is still `0`.
- `yesNoSizable.inc`'s `YesNoVar()` now estimates wrapped line count from prompt length and scales gump height accordingly (previously fixed-height, horizontal-only scaling) - needed for the new, longer house-demolish confirmation text.
- New `.log` file-access grants (`housing`, `playervendor` packages, `.log` extension) support new `HouseEscrowLog()`/`MerchantEscrowLog()` functions (persistent audit logs to `::log/houseescrow.log` and `::log/merchantescrow.log`, independent of the existing console-only debug print flags).

### Expected Impact

Staff now have a way to tear down and fully preserve a house's contents instead of the previous full-destroy behavior. Affected players (or a staff-designated recipient) can retrieve everything via `.houseescrow`. No existing house-management flow (self-service demolish, redeed, sign gump for owners/cowners) changes behavior beyond the teleporter-tracking rewrite covered in section 12.

---

## 11. Player-Vendor Escrow - Shared Module Refactor and Orphaned-Merchant Fixes

### Files Changed

| File | Change |
|---|---|
| `pkg/systems/playervendor/include/escrow.inc` | Becomes the shared/canonical escrow module |
| `pkg/systems/playervendor/commands/player/escrow.src` | Local duplicate code removed; pagination ID-collision fix |
| `pkg/systems/playervendor/commands/test/pmescrowtest.src` | Local duplicate code removed |
| `pkg/systems/playervendor/playermerchant.src` | Orphaned-merchant fixes; payout pack capacity raised; staff force-fire permission |

### Overview

This pre-existing escrow system (for cashed-out/closed player vendors) had its entry-encoding, storage-opening, and accessor logic duplicated independently across `escrow.src`, `pmescrowtest.src`, and `playermerchant.src`. This commit centralizes all of it into `escrow.inc` and, while doing so, fixes two paths that could leave a merchant's payout permanently orphaned (unclaimable).

### Notable Functional Changes

- `escrow.inc` gains: `MerchantEscrowLog()` (persistent log, independent of existing debug-print flags), `OpenEscrowStorage()`, `EscrowEscapeField`/`EscrowUnescapeField`, `EncodeEscrowEntryFields`/`EncodeEscrowEntries`/`DecodeEscrowEntries`, and accessor functions (`EntryId`, `EntryOwnerSerial`, `EntryEscrowName`, `EntryVendorName`, `EntryVendorSerial`, `EntryCreated`, `EntryOwnerName`) that check a struct-style field first before falling back to the legacy positional-array index.
- New `BuildEscrowDatafileNameForSerial(owner_serial)`, with the existing `BuildEscrowDatafileName(player_obj)` refactored to delegate to it - fixes the datafile name from a bare `PLAYERVENDOR_ESCROW_DF_PREFIX + serial` (no package prefix, previously "worked by accident" because every caller happened to already live inside `pkg/systems/playervendor`) to the package-scoped `":playervendor:..."` form, same class of fix as the house-escrow naming in section 10 but flagged here as latent (no confirmed live failure) rather than confirmed-active.
- `escrow.src`: pagination button IDs changed from absolute entry index to page-relative row number, reconstructed to an absolute index via `start_idx` after the gump returns, with added bounds checks - without this, a player with 1000+ escrow entries could push an absolute-index button ID into another button's numeric range and claim the wrong entry. `ClaimEscrowEntry` now snapshots via `ListRootItemsInContainer` instead of live-iterating while moving, and both it and `RemoveEscrowEntryById` gain `MerchantEscrowLog()` calls at each branch.
- `pmescrowtest.src`: pure deduplication (local `ENTRY_*_IDX` constants, accessor functions, `OpenEscrowStorage()`, and inline parse/encode logic all removed in favor of the shared module). One unstated side effect: this test tool's vendor-name fallback default changes from its own local `"[TEST] Player Merchant"` to the shared module's `"Player Merchant"`, since its local override was deleted along with the rest of the duplicate code.
- `playermerchant.src`: `PAYOUT_PACK_MAX_ROOT_ITEMS` 100 -> 225, `PAYOUT_PACK_MAX_WEIGHT` 40000 -> 60000 (matches the new house-escrow pack limits in section 10). The forced-fire event handler now also accepts `ev.source.cmdlevel >= 4` in addition to the merchant's actual master - required so `houseescrow.inc`'s `FireHouseMerchants()` can force-fire a merchant it doesn't own during a house demolish. `AppendEscrowIndex()` previously bailed out early without ever indexing the entry if the merchant had no `master` set (masterless), leaving the already-created escrow root permanently orphaned/unclaimable - it now proceeds regardless, logging `[ORPHANED]` and falling back to a raw-serial datafile name if the owner can't otherwise be resolved. Separately, `HandleMasterlessMerchantCleanup()` previously ran its own standalone teleport-and-kill sequence that never called `CashOut()` at all, destroying a masterless merchant's inventory/gold along with the NPC - it now delegates to `HandleMerchantClosure("masterless_cleanup", 1)`, which does call `CashOut()` before destroying the merchant's storage containers and running the kill sequence.

### Expected Impact

No change to normal vendor cash-out flow for merchants with a live master. Masterless merchants (owner deleted/unresolvable) are no longer silently destroyed with their inventory intact but unclaimable - their payout is now escrowed and locatable (flagged `[ORPHANED]` in the log) instead. The pagination fix prevents a wrong-entry claim for any player who accumulates 1000+ escrow entries. Payout packs can now hold more before a new pack is spun up.

---

## 12. New Admin Command - Teleporter Serial Backfill Migration

### Files Changed

| File | Change |
|---|---|
| `scripts/textcmd/admin/backfillteleporterserials.src` | New (100 lines) - `.backfillteleporterserials` |
| `pkg/std/housing/sign.src` | `AddTeleporter()` rewritten to use `TeleporterSerials` cprop instead of `house.items` membership |
| `pkg/std/housing/utility.inc` | New `CleanupHouseTeleporters()`, `IsLocationInThisHouse()` |
| `scripts/textcmd/admin/destroymulti.src` | Now calls `CleanupHouseTeleporters()` before destroying a house |

### Overview

House teleporter placement/removal previously validated against `house.items` membership - placement checked whether the newly created teleporter showed up in `house.items`, and removal counted matching-`teleNum` items directly within `house.items` (capped at 2 via a hardcoded counter). This commit introduces an explicit `TeleporterSerials` cprop list on each house as the source of truth, plus a one-time migration command to populate that list for teleporters placed before this change existed.

### Notable Functional Changes

- `sign.src`'s `AddTeleporter(who, house, num)`: placement now validates via the new `IsLocationInThisHouse(house, x, y, z, realm)` (an explicit spatial check against the house's own footprint via `ListMultisInBox`, replacing the old items-membership check) and, on success, appends the new teleporter's serial to `TeleporterSerials`. Removal now iterates the `TeleporterSerials` list (falling back to scanning `house.items` for untracked `0xA3CE` teleporters to backfill the list first if it's empty/missing), destroys matches, and writes back the remainder. Error message text changed from "That is not inside the building." to "You can only place teleporters inside this house." in both spots.
- New `CleanupHouseTeleporters(house)`: destroys every teleporter tracked in `TeleporterSerials`, plus sweeps `house.items` for any untracked pre-migration teleporters pointing at this house, then clears the cprop. Wired into `DemolishAndEscrowHouse()` (section 10), `sign.src`'s existing case-14 self-service demolish, `signcontrol.src`'s `Demolish()`, and now `scripts/textcmd/admin/destroymulti.src` as well.
- `.backfillteleporterserials` (Administrator, CmdLevel 4, idempotent - "safe to run more than once"): scans every realm world-wide by objtype (`0xA3CE`) rather than by proximity, specifically because a proximity/spatial search could miss a teleporter's paired partner if it's placed far away; resolves each teleporter's owning house via its `teleportserial` cprop and backfills that house's `TeleporterSerials` list. Orphaned teleporters (owning house no longer exists) are counted and reported but left untouched - explicitly deferred to "a separate cleanup pass."

### Expected Impact

Teleporter placement/removal is now driven by an explicit, house-scoped serial list instead of an unreliable items-membership check, and every path that destroys a house (demolish, redeed, escrow, admin destroymulti) now reliably cleans up its teleporters instead of potentially leaving them behind. Houses with teleporters placed before this patch need `.backfillteleporterserials` run once (already scheduled as part of this deployment) so their existing teleporters are tracked correctly going forward.

---

## 13. Tamed Pets - Follow() Runaway-Watch Diagnostics Removed

### Files Changed

| File | Change |
|---|---|
| `scripts/ai/tamed.src` | Removes 1.1.0's runaway-watch diagnostic block from `Follow()` |

### Overview

Patch 1.1.0 added a hard `Sleepms(25)` rate floor to `Follow()` (capping calls to ~40/sec) plus a console-only runaway-watch diagnostic that counted calls-per-second and printed a warning if a pet exceeded a threshold. This patch removes only the diagnostic instrumentation.

### Notable Functional Changes

- Removed: `FOLLOW_RUNAWAY_THRESHOLD`, `FOLLOW_RUNAWAY_WARN_COOLDOWN` constants; `followcalls`, `followwindowstart`, `lastfollowwarn` module-level vars; the per-call counting/windowing block and its `"[TamedAI runaway-watch] ..."` console print; the now-unused `use basicio;` import.
- **Not removed / unchanged:** the actual `Sleepms(25)` rate floor at the top of `Follow()` remains in place, applying before any early-return branch exactly as it did at the end of 1.1.0.

### Expected Impact

No gameplay/behavior change - pet follow speed and rate-limiting are identical to 1.1.0. This is a cleanup of console-only diagnostic logging that had served its purpose (confirming the 1.1.0 floor works) and is no longer needed.

---

## 14. CmdLevel Constants - Renumbered to Match the Actual Ladder

### Files Changed

| File | Change |
|---|---|
| `scripts/include/constants/cmdlevels.inc` | Removes phantom `CMDLEVEL_SHARD_MOM`; renumbers everything above Counselor |

### Overview

`config/cmds.cfg` defines exactly 6 `CmdLevel` blocks in order: Player, Coun, Seer, GM, Admin, Test. `cmdlevels.inc`'s named constants had a phantom `CMDLEVEL_SHARD_MOM := 2` that doesn't correspond to any real level, which pushed every constant above Counselor one level too high (`CMDLEVEL_SEER` was `3` instead of `2`, up through `CMDLEVEL_DEVELOPER` being `6` instead of the real top level, `5`).

### Notable Functional Changes

- `CMDLEVEL_SHARD_MOM` removed entirely. `CMDLEVEL_SEER` 3->2, `CMDLEVEL_GAME_MASTER` 4->3, `CMDLEVEL_ADMINISTRATOR` 5->4, `CMDLEVEL_DEVELOPER` 6->5.
- Only two symbol-name consumers of these constants exist repo-wide: `pkg/opt/spawnpoint/spawnpoint.src`'s case statement (pre-existing), and this patch's own new `who.cmdlevel < CMDLEVEL_DEVELOPER` check in `sign.src`'s case-17 escrow gump gate (section 10) - both reference by name and are correct as of this commit's renumbering.
- Not touched by this change: the large number of pre-existing raw numeric `who.cmdlevel >= 4` / `>= 5` comparisons found elsewhere in the codebase (e.g. `sign.src`'s existing owner/cowner checks). Those were already keyed to the engine's real, unaffected level numbering (`config/cmds.cfg` itself never changed), so their meaning is unchanged by this commit - only code that referenced the old, incorrect `CMDLEVEL_*` *names* was ever wrong.

### Expected Impact

No behavior change for the two confirmed symbol-name consumers (they now correctly reference the shard's real "Administrator"/"Developer" levels). No repo-wide audit of every raw-numeric cmdlevel check was performed as part of this commit - only the two confirmed name-based consumers were verified.

---

## 15. Misc Small Fixes

### Files Changed

| File | Change |
|---|---|
| `scripts/textcmd/seer/findboat.src` | Removes unused `include "include/account";` |
| `config/animxlate.cfg` | One new `Graphic 0x175` entry under `MobileType Monster` |
| `scripts/textcmd/test/editcharacter.src` | Color/hue clamp ceiling raised |

### Notable Functional Changes

- `findboat.src`: no functional code touched, only an unused include removed. No comment in the diff states why.
- `animxlate.cfg`: registers one additional monster graphic ID (`0x175`) for animation translation, alongside the pre-existing `0x176` entry. No further context in the diff about which creature this is for.
- `editcharacter.src`: `ClampInt(CInt(GFExtractData(result, ENTRY_COLOR)), 0, 10000)` -> `ClampInt(..., 0, 65535)` - the `.editcharacter` staff tool's hue/color clamp ceiling raised from 10000 to the full 16-bit hue range, so valid hues above 10000 are no longer incorrectly clamped down.

### Expected Impact

Staff-tool-only changes plus one unexplained dead-code removal; no player-facing effect.

---

## 16. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `config/animxlate.cfg` | 15 | One new monster graphic entry |
| `config/command_synopses.cfg` | 4, 10, 12 | Regenerated for new commands |
| `config/fileaccess.cfg` | 10 | New `.log` grants for housing/playervendor |
| `config/golocs_by_id.cfg` | 6 | City -> Suburb reclassification |
| `pkg/opt/alryc/stresstest/areastressworker.src` | 4 | New - spawned load worker |
| `pkg/opt/alryc/stresstest/emailstressworker.src` | 4 | New - spawned load worker |
| `pkg/opt/alryc/textcmd/test/aimovementsim.src` | 4 | New - `.aimovementsim` |
| `pkg/opt/alryc/textcmd/test/dfleaktest.src` | 4 | New - `.dfleaktest` |
| `pkg/opt/alryc/textcmd/test/housezoneaudit.src` | 7 | New - `.housezoneaudit` |
| `pkg/opt/alryc/textcmd/test/shardstress.src` | 4 | New - `.shardstress` |
| `pkg/opt/areas/areas.cfg` | 6 | City -> Suburb reclassification |
| `pkg/opt/areas/include/areapolicy.inc` | 3 | `UnloadDataFile()` leak fixes |
| `pkg/opt/areas/textcmd/admin/areas.src` | 6 | `CATEGORY_ORDER` gains "suburb" |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | 6 | Display ordering gains "Suburb" |
| `pkg/opt/statichousing/ssign.src` | 8 | House-limit check added |
| `pkg/opt/sysbook/start.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/opt/townstones/textcmd/admin/createtownstone.src` | 7 | `GetConfigStringArray` -> `SplitWords`/`GetConfigString` |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | 7 | Same as above |
| `pkg/std/housing/changeowner.src` | 8 | House-limit check + `owneracct`/cprop-sync bugfix |
| `pkg/std/housing/houseescrow.inc` | 10 | New - core house-escrow logic |
| `pkg/std/housing/housedeed.src` | 7, 8 | MultiID resolution fix, region-check refactor, house-limit check |
| `pkg/std/housing/setup.inc` | 10 | New `HOUSE_DECAY_PERIOD` constant (dormant) |
| `pkg/std/housing/sign.src` | 10, 12 | New escrow gump case, teleporter rewrite, decay timestamp refresh |
| `pkg/std/housing/signcontrol.src` | 10, 12 | Decay-check fix, `CleanupHouseTeleporters()` added |
| `pkg/std/housing/utility.inc` | 8, 10, 12 | House-limit helpers, escrow helpers, teleporter cleanup, owner-risk helpers |
| `pkg/systems/email/chardelete.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/email/commands/gm/inspectmail.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/email/email.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/email/logon.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/email/reconnect.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/email/webmail/webmail.src` | 3 | `UnloadDataFile()` leak fix |
| `pkg/systems/playervendor/commands/player/escrow.src` | 11 | Dedup + pagination ID-collision fix |
| `pkg/systems/playervendor/commands/test/pmescrowtest.src` | 11 | Dedup |
| `pkg/systems/playervendor/include/escrow.inc` | 11 | Becomes shared escrow module |
| `pkg/systems/playervendor/playermerchant.src` | 11 | Orphaned-merchant fixes, pack capacity, staff force-fire |
| `pkg/utils/gumps/include/yesNoSizable.inc` | 10 | `YesNoVar()` vertical scaling |
| `regions/regions.cfg` | 6 | City/Suburb reclassification, Occlo Isle -> City |
| `scripts/ai/main/killpcsloop.inc` | 9 | Sleep-after-wanders threshold 60 -> 150 |
| `scripts/ai/main/sleepmode.inc` | 9 | Wake radius unified to 20 |
| `scripts/ai/tamed.src` | 13 | Runaway-watch diagnostics removed |
| `scripts/include/checkcity.inc` | 5 | `CheckCity()` rewrite, new shared region helpers |
| `scripts/include/constants/cmdlevels.inc` | 14 | Renumbered to match actual ladder |
| `scripts/textcmd/admin/backfillteleporterserials.src` | 12 | New - `.backfillteleporterserials` |
| `scripts/textcmd/admin/destroymulti.src` | 12 | `CleanupHouseTeleporters()` added |
| `scripts/textcmd/coun/go.src` | 6 | Display ordering gains "Suburb" |
| `scripts/textcmd/player/houseescrow.src` | 10 | New - `.houseescrow` |
| `scripts/textcmd/seer/findboat.src` | 15 | Unused include removed |
| `scripts/textcmd/test/editcharacter.src` | 15 | Hue clamp ceiling raised |
| `patchnotes/developer-changelog-v1.1.1.md` | - | This file |
| `patchnotes/patch-v1.1.1.md` | - | Player-facing notes |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content |

---

## 17. Risk and Regression Notes

- **`CheckCity()`/region reclassification (sections 5-6):** any location logic gated on `CheckCity()` or the `city` area category now behaves differently for the five reclassified farmland/outbuilding zones (no longer "city") and Occlo Isle (now "city"). Confirmed active consumers: fishing nets, tree planting, exploding-potion power, bookcase decay. `lockchests.src` and `provocation.src`'s `CheckCity()` calls are dead/commented-out code and unaffected either way.
- **House placement MultiID fix (section 7):** `.housezoneaudit` should be run post-deploy to identify any already-placed houses that slipped into a restricted region under the old, broken `GetMultiDimensions()` call - this fix only prevents *new* violations, it does not retroactively move or flag existing ones automatically.
- **Per-account house limit (section 8):** only blocks new acquisition; accounts already over the 5-house cap are unaffected until they try to acquire another house. Worth confirming intended cap value (5) matches policy before this reaches players, since it's not adjustable without a code change (no config entry, just the `MAX_HOUSES_PER_ACCOUNT` constant).
- **Teleporter serial backfill (section 12):** `.backfillteleporterserials` needs to be run once, post-deploy, before the new `TeleporterSerials`-based removal logic in `sign.src` can be trusted for houses with pre-existing teleporters (the removal path does have a same-call fallback scan for untracked teleporters, but the backfill command is the deliberate one-time fix for the general case). Orphaned teleporters (house already destroyed) are reported but not cleaned up by this command - flagged as a separate follow-up.
- **Escrow storage growth (section 10):** house-escrow and player-vendor-escrow entries are only removed from their index once fully claimed - abandoned/orphaned entries (masterless merchants, unresolvable house owners) persist indefinitely in storage unless staff intervene via the reassign flow. No automatic expiry exists yet.
- **`pmescrowtest.src` vendor-name default (section 11):** this test tool's fallback vendor-name text silently changed from `"[TEST] Player Merchant"` to `"Player Merchant"` as an unstated side effect of removing its local duplicate code - purely a test-tool display string, but worth knowing if `.pmescrowtest` output is compared against old screenshots/docs.
- **CmdLevel renumbering (section 14):** only two symbol-name consumers were confirmed and verified correct; no repo-wide audit of raw-numeric `cmdlevel` comparisons was performed as part of this commit (none should be affected in principle, since `config/cmds.cfg`'s actual numbering never changed, but this wasn't exhaustively checked file-by-file).
- **`findboat.src` include removal (section 15):** no stated reason in the diff; if `.findboat` (Seer level) breaks in a way tied to account-related functionality, this removed include is the first thing to check.
