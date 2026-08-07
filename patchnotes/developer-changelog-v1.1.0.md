# Developer Changelog - v1.1.0
**Range:** `da73e02` (origin/Patch-1.0.9) -> `d1c5682` (HEAD)  
**Branch:** Patch-1.1.0  
**Date:** 2026-07-31 -> 2026-08-07  
**Commits in range:** 8 (excluding merge commits)  
**Files changed (committed):** 28 (+550 / -205)

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Tamed Pets - Ordered-Attack Area Gating and Packethook Hang Fix](#3-tamed-pets---ordered-attack-area-gating-and-packethook-hang-fix)
4. [Tamed Pets - AllCommand Self-Target Duplicate Fix](#4-tamed-pets---allcommand-self-target-duplicate-fix)
5. [Guaranteed-Kill Calls - HP Cap Bug Fix (Repo-Wide)](#5-guaranteed-kill-calls---hp-cap-bug-fix-repo-wide)
6. [Spawnpoints - Missing "Custom NPC" Despawn Case](#6-spawnpoints---missing-custom-npc-despawn-case)
7. [Staff Tools - Extra Login Management Commands](#7-staff-tools---extra-login-management-commands)
8. [Tamed Pets - Follow() Runaway-Loop Diagnostics and Hard Rate Floor](#8-tamed-pets---follow-runaway-loop-diagnostics-and-hard-rate-floor)
9. [Boss/SuperBoss/Champion Pet Confiscation - Extended to Boats, Champion Type Added](#9-bosssuperbosschampion-pet-confiscation---extended-to-boats-champion-type-added)
10. [Champion Relics - Blocked From Activation Inside Cities](#10-champion-relics---blocked-from-activation-inside-cities)
11. [Pet/Hireling Tooltip - Owner Name Property](#11-pethireling-tooltip---owner-name-property)
12. [Player Vendors - Duplicate IsInCityRegion() Definition Fix](#12-player-vendors---duplicate-isincityregion-definition-fix)
13. [Exhaustive File-by-File Change List](#13-exhaustive-file-by-file-change-list)
14. [Risk and Regression Notes](#14-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.0 is a small, focused patch on top of 1.0.9. It started with a tamed-pet ordered-attack safe-area fix (`b04437b`) followed by a broader refinement the next day (`e49d805`) that split safe-area and no-PK-area handling, fixed a packethook-driven hang in the pet "kill"/"attack" speech command, corrected a duplicate self-targeting bug in `AllCommand()`, and applied a repo-wide fix for a "guaranteed kill" idiom that silently failed against high-HP mobs; `deb83fc` then committed two new staff commands for the existing "extra login" account flag plus the first round of patch/launcher/dev notes. Three follow-up commits the same day (`39031bf`, `d71c555`, `e418d3b`) chased down and fixed a tamed-pet `Follow()` call-rate issue. Five days later, `4f7b633` bundled several unrelated small fixes/features together (champion relics blocked in cities, boss/superboss/champion pet confiscation extended to boats, an owner-name tooltip property for pets/hirelings, and further `Follow()` rate tuning), which introduced a duplicate-function-definition compile error fixed the next day by `d1c5682`. Two merge commits in range (`ceac651`, `e9a72fb`) only fold in already-released 1.0.8/1.0.9 content and carry no new 1.1.0 changes.

---

## 2. Commit Timeline

| Hash | Date | Message |
|---|---|---|
| `ceac651` | - | Merge pull request #304 from Andries1985/Patch-1.0.8 (no new content - already released) |
| `e9a72fb` | - | Merge pull request #305 from Andries1985/Patch-1.0.9 (no new content - already released) |
| `b04437b` | 2026-07-31 | Fix for tamed pets in safe areas |
| `e49d805` | 2026-08-01 | Updates for tamed, mobile kill and spawnpoint fix |
| `deb83fc` | 2026-08-02 | Patch 1.1.0 Notes and extra login fixes |
| `39031bf` | 2026-08-02 | Tamed AI Runaway script fix |
| `d71c555` | 2026-08-02 | Tamed Pet AI debug print to console |
| `e418d3b` | 2026-08-02 | More AI Tamed fix |
| `4f7b633` | 2026-08-06 | Patch update |
| `d1c5682` | 2026-08-07 | Definition error |

---

## 3. Tamed Pets - Ordered-Attack Area Gating and Packethook Hang Fix

### Files Changed

| File | Change |
|---|---|
| `scripts/ai/tamed.src` | New `OrderedFight()` / `ReturnDonatorMountToStone()` functions, `ProcessSpeech()` target-acquisition change |

### Overview

Ordering a tamed pet to attack (`"<petname> kill"`, `"<petname> attack"`, `"all kill"`, `"all attack"`) called `Fight(what)` directly, with no area check at the point of *ordering* an attack (area enforcement elsewhere only covered the pet's own autonomous aggression). `b04437b` introduced `OrderedFight()` as a gate in front of `Fight()`, refusing/confiscating the pet when the master, pet, or target was in a safe or no-PK area. `e49d805` then found this first pass had a second, more subtle problem: the target-acquisition call `AcquireTargetWithSpeechLock(TGTOPT_HARMFUL + TGTOPT_CHECK_LOS)` could hang forever. Per [[project-nopk-harmful-target-packethook]], `packethook.src`'s `OnTarget()` silently drops (`CORE_IGNORE`) any harmful-cursor click on a PC in a no-PK/safe area rather than letting the packet through - so if a master ordered an attack and then clicked a nearby player while in such an area, the underlying `Target()` call inside `AcquireTargetWithSpeechLock` never returned, permanently stuck with `awaitingtargetselection` still set.

### Notable Functional Changes

- `OrderedFight(atktarget)`: now computes `in_safe_area` and `in_nopk_area` separately (each still checking `me`, `atktarget`, and `master`) instead of one combined OR, plus `target_is_npc := atktarget.isa(POLCLASS_NPC)`.
  - Safe area: unchanged behavior - always blocks the order and confiscates the pet / returns a donator mount to its stone (with a charge deducted), regardless of target type.
  - No-PK area: **new**, narrower gate - only blocks the order when the target is *not* an NPC (i.e. a PC). Ordering a pet to fight a monster in a no-PK dungeon is allowed; only PC targets are refused. On refusal, the master gets `"You cannot order an attack on that here."`, `ignorespeechuntil` is reset to the current game clock (undoing the short post-target speech lockout `AcquireTargetWithSpeechLock` had already set, since no fight is actually starting), war mode is cleared, and the pet resumes `following := master` instead of sitting idle.
  - Both branches still fall through to `Fight(atktarget)` when neither gate applies.
- `ReturnDonatorMountToStone(removecharge := 0)`: extracted from the old inline block in `GoWild()` so `OrderedFight()`'s safe-area branch can reuse it with `removecharge=1` (deducts a stone charge) while `GoWild()` itself calls it with no charge removed, preserving prior behavior for the wild-release path.
- `ProcessSpeech()`: the pet-command target acquisition changed from `AcquireTargetWithSpeechLock(TGTOPT_HARMFUL + TGTOPT_CHECK_LOS)` to `AcquireTargetWithSpeechLock(TGTOPT_CHECK_LOS)` (harmful cursor removed). This lets the target-selection packet through in all cases so `Target()` actually returns; `OrderedFight()`'s own safe/no-PK checks now handle refusing an inappropriate PC target gracefully afterward instead of relying on the packethook to pre-filter (and hang if it does).
- Both call sites that invoked `Fight()` directly for player-issued kill/attack commands (`EVID_ALL_ATTACK_CMD` handling in `MainAILoop()`, and the direct kill/attack branch in `ProcessSpeech()`) now call `OrderedFight()` instead.

### Expected Impact

Ordering a pet to attack a player or monster while the pet, master, or target is in a safe area is refused and (for non-donator-mount pets) confiscates the pet, same as before. Ordering a pet to attack a *monster* while in a no-PK dungeon now works (previously blocked by the old combined check). Ordering a pet to attack a *player* while in a no-PK area is refused with a message instead of the command silently hanging the pet's speech-processing state forever.

---

## 4. Tamed Pets - AllCommand Self-Target Duplicate Fix

### Files Changed

| File | Change |
|---|---|
| `scripts/ai/tamed.src` | `AllCommand()` |

### Overview

`AllCommand()` sends a control event (e.g. an "all kill" order) to itself (`SendEvent(me, eve)`) and then loops every nearby mobile owned by the same master, re-sending the same event to each. The loop's ownership check (`GetObjProperty(mob, "master") == me.master.serial`) matches `me` as well as every sibling pet, so the calling pet received its own "all kill"/"all attack" event twice - once directly, once again from the loop.

### Notable Functional Changes

- The loop condition now requires `mob.serial != me.serial and GetObjProperty(mob, "master") == me.master.serial`, excluding the calling pet from the re-broadcast.

### Expected Impact

An "all kill"/"all attack" (or other `AllCommand`-routed) order now runs once per pet instead of twice for the pet that issued the broadcast, avoiding redundant `OrderedFight()`/target-acquisition calls stacking on the same pet.

---

## 5. Guaranteed-Kill Calls - HP Cap Bug Fix (Repo-Wide)

### Files Changed

| File | Change |
|---|---|
| `scripts/include/all.inc` | `KillMobile()` |
| `scripts/ai/main/assassinsleep.inc` | self-kill on `"killme"` |
| `scripts/ai/main/sleepmode.inc` | self-kill on `"killme"` |
| `scripts/include/anchors.inc` | `SendToJailAndRespawn()` |
| `pkg/opt/colorwars/cwars.src` | `RunCWars()`, `CleanArena()` (2 call sites) |
| `pkg/opt/colorwars/commands/gm/cleancw.src` | `CleanArena()` |
| `pkg/opt/versebook/ai_spirit_flock.src` | `RunCircle()` end-of-life cleanup |
| `pkg/opt/summoning/summoning.src` | summon duration expiry |
| `pkg/opt/summoning/npcsummoning.src` | summon duration expiry (2 call sites) |
| `pkg/opt/necro/animatedead.src` | spell-effect expiry |
| `pkg/std/spells/blade_spirit.src` | summon duration expiry |
| `pkg/std/spells/vortex.src` | summon duration expiry |
| `pkg/std/housing/utility.inc` | `KillConfiscatedPet()` |

### Overview

Per [[project-known-engine-constraints]] and [[project-spawnpoint-despawn-hp-cap-bug]], `ApplyRawDamage`/`ApplyTheDamage` silently no-op (no error, no effect) when the damage argument exceeds `USHRT_MAX` (65535). Every file above used the idiom `ApplyRawDamage(mobile, GetMaxHP(mobile)+n)` (or `ApplyTheDamage(...)` in `KillMobile`'s attributed-kill branch) purely to guarantee a kill regardless of current HP - self-destruct timers, summon/spell-duration expiry, arena/pet cleanup, "killme"-flagged sleep-mode NPCs. Against any mob with boosted max HP at or above ~65,533 (e.g. `CustomHitsLevel`-scaled superbosses), the "kill" silently did nothing and the mob was left alive at full health. Root cause was diagnosed from a live report of a Trinsic-area Soul Whisperer group spawnpoint (`CustomHitsLevel 8000000` = 80,000 HP) not despawning on restart.

### Notable Functional Changes

- Every listed call site now calls the native `x.kill()` method (or `mobile.kill(by_who)` for the attributed branch in `KillMobile`), which kills unconditionally regardless of current/max HP - the same idiom already used elsewhere in `scripts/ai/tamed.src` for releasing/killing tamed pets.
- `KillMobile()` in `scripts/include/all.inc` is the only one of these used by the spawnpoint package (`checkpoint.src`, `despawner.src`), confirmed via repo-wide grep before changing it, so this was the direct fix for the reported despawn bug.
- **Intentionally left unchanged:** `pkg/std/dundee/deathvortex.src`'s `ApplyRawDamage(who, GetMaxHP(who))` - this operates on player characters doing genuine partial damage-over-time (a dungeon trap effect, `GetMaxHP(who)/10` per tick), not a single guaranteed-kill call. Player HP won't realistically hit the 65535 cap, and switching this to `.kill()` would newly bypass invulnerability for staff/GMs testing the trap - a real player-facing behavior change, not a bugfix, so left alone pending an explicit ask.

### Expected Impact

High-HP spawned/summoned/tamed mobiles (colorwars combatants, spirit flock summons, blade spirit/vortex/animate-dead summons, confiscated pets, jailed characters' old mobiles, sleep-mode NPCs flagged for cleanup, and spawnpoint-tracked NPCs) now actually die when the script intends to guarantee their death, instead of silently surviving above the old 65535 damage cap. This directly fixes spawnpoints with very-high-HP NPCs (e.g. `CustomHitsLevel`-boosted superbosses) not despawning/respawning correctly.

---

## 6. Spawnpoints - Missing "Custom NPC" Despawn Case

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/spawnpoint/despawner.src` | Added `"Custom NPC"` case to the kill-branch |
| `pkg/opt/spawnpoint/destroypoint.src` | Added `"Custom NPC"` case to the kill-branch |

### Overview

Both scripts' `case( pt_data[1] )` switch over spawnpoint type only had a `"NPC"` / `"Group"` case falling through to the kill logic (`KillMobile()` in `despawner.src`, the inline `.kill()` sequence in `destroypoint.src`). Spawnpoints configured as type `"Custom NPC"` matched no case at all, so neither script ran any kill/cleanup logic for them - independent of the HP-cap bug in section 5.

### Notable Functional Changes

- `"Custom NPC"` now falls through to the same kill logic as `"NPC"` / `"Group"` in both `despawner.src` and `destroypoint.src`.
- `destroypoint.src`'s kill-branch also picked up the `.kill()` change from section 5 in the same edit (comment added in-line noting the HP-cap reason).

### Expected Impact

Destroying or despawning a "Custom NPC"-type spawnpoint now actually kills its currently-spawned NPC, matching the existing behavior for "NPC"/"Group" type spawnpoints - previously the old mob was left behind entirely, compounding the section 5 bug for any Custom NPC spawn that also had high max HP.

---

## 7. Staff Tools - Extra Login Management Commands

Committed in `deb83fc`, alongside the first round of this patch's notes files.

### Files Changed

| File | Change |
|---|---|
| `scripts/textcmd/test/listextralogin.src` | New |
| `scripts/textcmd/test/removeextralogin.src` | New |
| `config/command_synopses.cfg` | Regenerated via `pythonscripts/_gen_command_synopses_cfg.py` to add both new entries |

### Overview

The pre-existing `extralogin` command (`scripts/textcmd/test/extralogin.src`, unchanged this patch) grants an account's `ExtraClient` cprop, exempting that account's characters from the per-IP simultaneous-login cap (see `CheckForMaxClientsOnline()` in `pkg/systems/accounts/include/accounts.inc`). There was previously no way to check or revoke that flag short of a raw property-editing tool. Two new Developer-level (`CmdLevel 5`) commands were added, same directory/cmdlevel as `extralogin`.

### Notable Functional Changes

- `listextralogin`: targets a character, looks up `FindAccount(targ.acctname)`, and reports whether `ExtraClient` is set on that account - no other data (e.g. it does not report how many characters are currently logged in on that account; this was deliberately scoped down from an initially-proposed richer version).
- `removeextralogin`: targets a character and calls `acc.eraseprop("ExtraClient")` on their account (not on the character - the property lives on the account, matching where `extralogin` sets it), reporting success/failure to the staff member and notifying the target player.

### Expected Impact

Staff-facing only, no player-visible change beyond the notification message a targeted player receives when their extra login is revoked. Lets staff audit and reverse a previously one-way (`extralogin`-only) grant.

---

## 8. Tamed Pets - Follow() Runaway-Loop Diagnostics and Hard Rate Floor

### Files Changed

| File | Change |
|---|---|
| `scripts/ai/tamed.src` | `Follow()`, `MainAILoop()` - touched across `39031bf`, `d71c555`, `e418d3b`, and the `tamed.src` hunk of `4f7b633` |

### Overview

After the ordered-attack changes in sections 3-4 shipped, a live observation suggested a tamed pet's `Follow()` could be invoked far more often than the AI loop's nominal per-second cadence intends - a "runaway" follow loop. `39031bf` first tried bumping `MainAILoop()`'s `waittime` from `0` to `1` while following, but `e418d3b` reverted that back to `0` once it was clear the loop's whole-second `wait_for_event()` granularity wasn't the real lever, and instead added a hard `Sleepms()` floor directly inside `Follow()` itself so no call path (including any early-return branch) can re-enter `Follow()` with zero delay. `d71c555` added console-only diagnostics (a per-second call counter plus a warn cooldown, printed via `Print()`) to catch any pet whose `Follow()` rate exceeds the floor. `4f7b633` then tuned the floor down from `Sleepms(100)` to `Sleepms(25)` (raising the normal cadence ceiling from ~10/sec to ~40/sec) and raised the runaway-print threshold from 25 to 100 to match, since 25 calls/sec was apparently within normal range at the tighter floor and would have false-positived the diagnostic.

### Notable Functional Changes

- `Follow()`: now opens with a runaway-watch block - computes `followclock := ReadGameClock()`, tracks `followcalls` within a 1-second window (`followwindowstart`), and if the prior window's call count exceeded `FOLLOW_RUNAWAY_THRESHOLD` *and* at least `FOLLOW_RUNAWAY_WARN_COOLDOWN` (60s) has passed since the last warning, prints a console-only diagnostic line (pet name, hex serial, `npctemplate`, master name, calls/sec, position, whether standing on a multi, `guarding` state). This requires a new `use basicio;` import and is never sent to any client.
- `Follow()`: immediately after the runaway-watch block, unconditionally calls `Sleepms(25)` (`Sleepms(100)` as first introduced in `e418d3b`, tuned down in `4f7b633`) before any of the function's early-return branches, so every call is rate-limited to at most ~40/sec regardless of which branch it takes.
- `FOLLOW_RUNAWAY_THRESHOLD`: introduced at `20` (`d71c555`), raised to `25` (`e418d3b`, aligned to the then-100ms floor's ~10/sec ceiling), then to `100` (`4f7b633`, aligned to the new 25ms floor's ~40/sec ceiling).
- `MainAILoop()`: `waittime` while `(following) and (master)` was briefly changed from `0` to `1` (`39031bf`), then reverted to `0` (`e418d3b`) - net no change across the full range; superseded by the `Follow()`-level `Sleepms()` floor as the actual fix.

### Expected Impact

No intended player-visible behavior change under normal play - pets continue to follow at essentially the same cadence. Guards against a pathological case where something calls `Follow()` back-to-back with no delay (e.g. rapid re-entry from a branch not going through the normal per-second AI loop wait), which could otherwise spike CPU usage or cause visibly stuttery/erratic following movement. Diagnostic prints are server-console only, staff-visible, never sent to a player.

---

## 9. Boss/SuperBoss/Champion Pet Confiscation - Extended to Boats, Champion Type Added

### Files Changed

| File | Change |
|---|---|
| `pkg/std/housing/signcontrol.src` | `ConfiscateBossPet()` removed (relocated), Boss/SuperBoss check now also matches `Champion` |
| `pkg/std/housing/utility.inc` | `ConfiscateBossPet()` added (relocated from `signcontrol.src`), gains `locationnoun` parameter |
| `pkg/std/boat/boat.src` | New `CheckBoatForBossMobiles()`, called every 10s from `boat_script()`'s main loop |

### Overview

`ConfiscateBossPet()` previously lived only in `signcontrol.src`, firing when a Boss/SuperBoss-tagged tamed NPC entered a house via the sign's listener, with no boat equivalent. `4f7b633` moved it into the shared `pkg/std/housing/utility.inc` (next to the `KillConfiscatedPet()` it already calls), added a `Champion`-tagged check alongside `Boss`/`SuperBoss` at both the existing house call site and the new boat call site, and generalized its player-facing messages to name the location (`"house"` vs `"boat"`) via a new `locationnoun` parameter that defaults to `"house"`, preserving the old message text on the house path.

### Notable Functional Changes

- `ConfiscateBossPet(mobile, locationnoun := "house")` relocated from `signcontrol.src` to `housing/utility.inc` with unchanged core logic (find master online/offline, create a claim ticket in backpack then bank, fall back to killing outright if neither has space, message the master) except both `SendSysMessage()` calls now interpolate `locationnoun` instead of the literal word `"house"`.
- `signcontrol.src`'s `SignListener()`: the Boss/SuperBoss confiscation check now also matches `GetObjProperty(mobile, "Champion")`.
- `boat.src`: new `CheckBoatForBossMobiles()` iterates `boat.mobiles`, calling `ConfiscateBossPet(mob, "boat")` for any NPC on board flagged `Boss`, `SuperBoss`, or `Champion`. `boat_script()`'s main loop now runs this check every 10 seconds (`nextbosscheck`), the same cadence pattern as the existing sound/encounter timers. Requires new `use basic;` and `include "util/bank";` / `include ":housing:utility";` in `boat.src`.

### Expected Impact

A tamed Boss, SuperBoss, or (newly) Champion-tagged pet or hireling brought aboard a player boat is now confiscated the same way one brought into a player house already was - killed and replaced with a claim ticket (in the master's backpack or bank) redeemable at an Animal Trainer for a gold fine, checked roughly every 10 seconds while the boat script runs. Previously boats had no such check at all, and houses did not treat Champion-tagged pets as needing confiscation.

---

## 10. Champion Relics - Blocked From Activation Inside Cities

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/ArtifactSystem/championrelic.src` | New city-region check in `program championrelic()` |
| `scripts/include/areas.inc` | New `IsInCityRegion(who)` |

### Overview

Champion relic activation (`program championrelic`) had no location restriction. `4f7b633` added a check at the top of the activation flow refusing use inside a city region, backed by a new shared `IsInCityRegion(who)` helper added to `areas.inc`.

### Notable Functional Changes

- `championrelic.src`: `program championrelic(who, item)` now calls `IsInCityRegion(who)` immediately after its existing early-return checks; if true, sends `"You cannot activate this relic in a city."` (`RELIC_FONT_COLOR`) to `who` and returns `0` before rolling/consuming the relic.
- `areas.inc`: new `function IsInCityRegion(who)`, returning `CInt(GetRegionString("regions", who.x, who.y, "City", who.realm)) == 1` - the same region-string check pattern already used elsewhere in this file (e.g. `IsInRPArea()`).

### Expected Impact

Players can no longer activate a champion relic while standing inside a city region; they now get an explicit refusal message instead. No change to relic activation anywhere else.

---

## 11. Pet/Hireling Tooltip - Owner Name Property

### Files Changed

| File | Change |
|---|---|
| `pkg/packethooks/megacliloc/mobiledata.src` | New "Owner" property entry |

### Overview

The mega-cliloc mobile properties packethook (governs the hover/single-click property list for mobiles) had no entry showing a tamed pet or hireling's owner. `4f7b633` added one, keyed off the existing `"master"` obj property.

### Notable Functional Changes

- `mobiledata.src`: within the properties-building loop, now reads `GetObjProperty(xObject, "master")`; if set, resolves it via `SystemFindObjectBySerial(masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES)` (so it still resolves while the master is offline) and, if found, appends a property using cliloc `1070722` ("Owner: ~1_NAME~") with the master's name.

### Expected Impact

Examining/hovering a tamed pet or hireling now shows an "Owner: `<name>`" line in its property tooltip, for any mobile with a `master` obj property set - including pets confiscated per section 9 up until the moment they're removed, since that flow only affects boats/houses, not the tooltip itself.

---

## 12. Player Vendors - Duplicate IsInCityRegion() Definition Fix

### Files Changed

| File | Change |
|---|---|
| `pkg/systems/playervendor/playermerchant.src` | Removed local `IsInCityRegion()`, call site updated |

### Overview

`playermerchant.src` already had its own local, no-argument `IsInCityRegion()` (used by `CalculateDailyWage()` to decide between `MONTHLY_WAGE_GOLD` and `CITY_MONTHLY_WAGE_GOLD`), with a body identical to the one section 10 added globally to `areas.inc` in the same `4f7b633` commit. Two functions of the same name being visible in the same compiled scope produced a duplicate-definition compile error (the same class of collision as [[feedback_no_module_function_name_collision]], but between two script-defined functions rather than against a module builtin). `d1c5682` fixed it by deleting the local copy and switching the call site to the shared one.

### Notable Functional Changes

- `playermerchant.src`: removed `function IsInCityRegion()` (identical body to the one now in `areas.inc`).
- `CalculateDailyWage()`: `if(IsInCityRegion())` -> `if(IsInCityRegion(me))`, now calling the shared `areas.inc` version with an explicit `who` argument.

### Expected Impact

Compile-fix only - `CalculateDailyWage()` computes the identical result before and after (same region check, `me` passed implicitly before vs. explicitly now), so no gameplay change. Without this fix the package would not compile at all following `4f7b633`.

---

## 13. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `scripts/ai/tamed.src` | 3, 4 | `OrderedFight()`/`ReturnDonatorMountToStone()` added, safe/no-PK area split, packethook-hang fix in `ProcessSpeech()`, `AllCommand()` self-target fix |
| `scripts/include/all.inc` | 5 | `KillMobile()` -> `.kill()`/`.kill(by_who)` |
| `scripts/ai/main/assassinsleep.inc` | 5 | `"killme"` self-kill -> `.kill()` |
| `scripts/ai/main/sleepmode.inc` | 5 | `"killme"` self-kill -> `.kill()` |
| `scripts/include/anchors.inc` | 5 | `SendToJailAndRespawn()` -> `.kill()` |
| `pkg/opt/colorwars/cwars.src` | 5 | Two arena-cleanup call sites -> `.kill()` |
| `pkg/opt/colorwars/commands/gm/cleancw.src` | 5 | Arena cleanup -> `.kill()` |
| `pkg/opt/versebook/ai_spirit_flock.src` | 5 | Summon end-of-life -> `.kill()` |
| `pkg/opt/summoning/summoning.src` | 5 | Summon expiry -> `.kill()` |
| `pkg/opt/summoning/npcsummoning.src` | 5 | Summon expiry (2 sites) -> `.kill()` |
| `pkg/opt/necro/animatedead.src` | 5 | Spell expiry -> `.kill()` |
| `pkg/std/spells/blade_spirit.src` | 5 | Summon expiry -> `.kill()` |
| `pkg/std/spells/vortex.src` | 5 | Summon expiry -> `.kill()` |
| `pkg/std/housing/utility.inc` | 5 | `KillConfiscatedPet()` -> `.kill()` |
| `pkg/opt/spawnpoint/despawner.src` | 6 | `"Custom NPC"` case added |
| `pkg/opt/spawnpoint/destroypoint.src` | 5, 6 | `"Custom NPC"` case added; kill logic -> `.kill()` |
| `scripts/textcmd/test/listextralogin.src` | 7 | New |
| `scripts/textcmd/test/removeextralogin.src` | 7 | New |
| `config/command_synopses.cfg` | 7 | Regenerated for the two new commands |
| `patchnotes/developer-changelog-v1.1.0.md` | - | This file, first version committed in `deb83fc` |
| `patchnotes/patch-v1.1.0.md` | - | Player-facing notes, first version committed in `deb83fc` |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content in `deb83fc` |
| `pkg/opt/ArtifactSystem/championrelic.src` | 10 | City-region activation block added |
| `pkg/packethooks/megacliloc/mobiledata.src` | 11 | "Owner" property added |
| `pkg/std/boat/boat.src` | 9 | `CheckBoatForBossMobiles()` added, called every 10s |
| `pkg/std/housing/signcontrol.src` | 9 | `ConfiscateBossPet()` relocated out; `Champion` added to the confiscation check |
| `pkg/std/housing/utility.inc` | 9 | `ConfiscateBossPet()` relocated in, gains `locationnoun` param |
| `scripts/include/areas.inc` | 10 | New `IsInCityRegion(who)` |
| `pkg/systems/playervendor/playermerchant.src` | 12 | Duplicate local `IsInCityRegion()` removed; call site updated |

---

## 14. Risk and Regression Notes

- **No-PK ordered-attack gate** (section 3): now allows ordering a pet to attack monsters in no-PK areas, where the prior combined check blocked this too - confirm this matches intended design for no-PK dungeons (pets fighting monsters was presumably always intended to work there; only PC targeting is meant to be restricted).
- **Packethook hang fix** (section 3): removing `TGTOPT_HARMFUL` from the pet-command target acquisition means `packethook.src`'s own harmful-cursor PC rejection no longer runs for this code path at all - `OrderedFight()`'s explicit area/target checks are now the only gate. If any other harmful-cursor-specific behavior in `packethook.src` (beyond the no-PK/safe-area block) was relied upon for pet-command targeting, it no longer applies here.
- **`deathvortex.src` left unchanged** (section 5): confirmed intentional - this is genuine partial player damage-over-time, not a guaranteed-kill idiom; revisit only if explicitly asked to make the trap's final blow unconditional.
- **`Follow()` rate floor tuning** (section 8): the effective per-pet following cadence changed twice within this range (100ms -> 25ms floor) without a clearly documented root cause for the original runaway report - if pets still exhibit stuttery/erratic following in live testing, the diagnostic `Print()` added in `d71c555` is the tool to check first (server console, gated by `FOLLOW_RUNAWAY_WARN_COOLDOWN`) before tuning the constants further.
- **Boat boss-check cadence** (section 9): `CheckBoatForBossMobiles()` runs every 10 seconds while the boat script loop is alive - a Boss/SuperBoss/Champion pet could be aboard for up to ~10s before confiscation fires, unlike the house path which reacts to `SignListener()`'s move-in event immediately.
- **`IsInCityRegion()` now shared** (sections 10, 12): any other package with its own locally-defined `IsInCityRegion()`-named function (none found in this range beyond `playermerchant.src`) would hit the same duplicate-definition error `d1c5682` fixed - worth a repo-wide grep before merging any future branch that also touches `areas.inc`.
- **Extra-login commands** (section 7): committed in `deb83fc`; confirm `listextralogin.src`/`removeextralogin.src` have been compiled (not run automatically by this process).
