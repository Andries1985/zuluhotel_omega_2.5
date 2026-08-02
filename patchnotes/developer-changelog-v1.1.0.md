# Developer Changelog - v1.1.0
**Range:** `da73e02` (origin/Patch-1.0.9) -> `e49d805` (HEAD)  
**Branch:** Patch-1.1.0  
**Date:** 2026-07-31 -> 2026-08-02  
**Commits in range:** 2 (excluding merge commits)  
**Files changed (committed):** 16 (+106 / -42)  
**Pending (uncommitted at time of writing):** 3 files (+~150 / -0, new staff commands + regenerated synopses config)

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Tamed Pets - Ordered-Attack Area Gating and Packethook Hang Fix](#3-tamed-pets---ordered-attack-area-gating-and-packethook-hang-fix)
4. [Tamed Pets - AllCommand Self-Target Duplicate Fix](#4-tamed-pets---allcommand-self-target-duplicate-fix)
5. [Guaranteed-Kill Calls - HP Cap Bug Fix (Repo-Wide)](#5-guaranteed-kill-calls---hp-cap-bug-fix-repo-wide)
6. [Spawnpoints - Missing "Custom NPC" Despawn Case](#6-spawnpoints---missing-custom-npc-despawn-case)
7. [Staff Tools - Extra Login Management Commands](#7-staff-tools---extra-login-management-commands)
8. [Exhaustive File-by-File Change List](#8-exhaustive-file-by-file-change-list)
9. [Risk and Regression Notes](#9-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.0 is a small, focused patch on top of 1.0.9. Two substantive commits: an initial tamed-pet ordered-attack safe-area fix (`b04437b`) followed by a broader refinement the next day (`e49d805`) that split safe-area and no-PK-area handling, fixed a packethook-driven hang in the pet "kill"/"attack" speech command, corrected a duplicate self-targeting bug in `AllCommand()`, and applied a repo-wide fix for a "guaranteed kill" idiom that silently failed against high-HP mobs. Two merge commits in range (`ceac651`, `e9a72fb`) only fold in already-released 1.0.8/1.0.9 content and carry no new 1.1.0 changes. Also pending at time of writing (not yet committed): two new staff commands for managing the existing "extra login" account flag (`listextralogin`, `removeextralogin`), alongside a regenerated `command_synopses.cfg`.

---

## 2. Commit Timeline

| Hash | Date | Message |
|---|---|---|
| `ceac651` | - | Merge pull request #304 from Andries1985/Patch-1.0.8 (no new content - already released) |
| `e9a72fb` | - | Merge pull request #305 from Andries1985/Patch-1.0.9 (no new content - already released) |
| `b04437b` | 2026-07-31 | Fix for tamed pets in safe areas |
| `e49d805` | 2026-08-01 | Updates for tamed, mobile kill and spawnpoint fix |
| *(pending)* | 2026-08-02 | Extra login list/remove staff commands + synopses regeneration - not yet committed |

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

**Not yet committed at time of writing.**

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

## 8. Exhaustive File-by-File Change List

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
| `scripts/textcmd/test/listextralogin.src` | 7 | New (pending commit) |
| `scripts/textcmd/test/removeextralogin.src` | 7 | New (pending commit) |
| `config/command_synopses.cfg` | 7 | Regenerated for the two new commands (pending commit) |

---

## 9. Risk and Regression Notes

- **No-PK ordered-attack gate** (section 3): now allows ordering a pet to attack monsters in no-PK areas, where the prior combined check blocked this too - confirm this matches intended design for no-PK dungeons (pets fighting monsters was presumably always intended to work there; only PC targeting is meant to be restricted).
- **Packethook hang fix** (section 3): removing `TGTOPT_HARMFUL` from the pet-command target acquisition means `packethook.src`'s own harmful-cursor PC rejection no longer runs for this code path at all - `OrderedFight()`'s explicit area/target checks are now the only gate. If any other harmful-cursor-specific behavior in `packethook.src` (beyond the no-PK/safe-area block) was relied upon for pet-command targeting, it no longer applies here.
- **`deathvortex.src` left unchanged** (section 5): confirmed intentional - this is genuine partial player damage-over-time, not a guaranteed-kill idiom; revisit only if explicitly asked to make the trap's final blow unconditional.
- **Extra-login commands pending commit** (section 7): `listextralogin`/`removeextralogin` and the regenerated `config/command_synopses.cfg` were not committed as of this writing - confirm they're committed before this branch is released, and compile the two new `.src` files (not run automatically by this process).
