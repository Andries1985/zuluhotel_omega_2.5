# Developer Changelog - v1.1.3
**Range:** `92073b1` (patchnotes commit of v1.1.2) -> working tree  
**Branch:** Patch-1.1.3  
**Date:** 2026-10-06  
**Commits in range:** 1 non-boundary commit (`190321d` "Class info fix"); `ab3eb7c` (merge of Test-Shard) carries no new content - its diff against `a908285` is empty. The remainder of this release is still uncommitted in the working tree.  
**Files changed:** 2 committed (+251), 9 uncommitted code/config files, plus this release's own 3 `patchnotes/` files

> Scope was computed against `92073b1`, not `master`. `master` is well behind this branch - it still lacks every 1.1.2 merge - so `git log master..HEAD` reports dozens of already-shipped 1.1.2 commits as if they were new.

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [New Player Command - .classinfo](#3-new-player-command---classinfo)
4. [Civilian NPC AI - Town-Bound Enforcement Removed](#4-civilian-npc-ai---town-bound-enforcement-removed)
5. [Guild Package - Large Constant Arrays Now Built Lazily](#5-guild-package---large-constant-arrays-now-built-lazily)
6. [Carried Over From 1.1.2 - Custom-NPC Dressing Extracted (Undocumented There)](#6-carried-over-from-112---custom-npc-dressing-extracted-undocumented-there)
7. [Exhaustive File-by-File Change List](#7-exhaustive-file-by-file-change-list)
8. [Risk and Regression Notes](#8-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.3 is a small release with two pieces of real work plus one correction to 1.1.2's record.

`190321d` ("Class info fix") adds `.classinfo`, a read-only player command reporting the class-level math in `scripts/include/classes.inc`. The script as first written did not compile: it included a package that does not exist on this shard, and it called `IsFromThatClasse()` with an invented five-argument signature. Both are fixed in section 3, along with the command-registration steps the new command needed (`cmds.cfg` directory entry, `command_synopses.cfg` regeneration).

Section 4 removes the civilian town-bound system introduced in `c442d6c` ("Townsfolk now stay within the city they are spawned in.. if they leave, they get killed and will respawn. Can only be spawned in cities.") and extended by `a3c99f6` and `676adfb`. Townspeople, persons, nobles and minstrels are no longer teleported to jail and killed for leaving their home city, and are no longer killed at spawn for being spawned outside a city region. Movement reverts to the pre-`c442d6c` semantics: `drop_anchor(me)` plus a plain `Wander()`. The revert was deliberately scoped to movement only - the sayings/dress rework and the shared-include consolidation that `c442d6c` bundled alongside the bounds system are kept.

Section 5 ports a fix already made on ZH3.0: two large array literals declared at file scope in the guild package's `guildconstants.inc` were being constructed at the start of every script that includes it - including the equip and unequip control scripts, so once per item on every equip, unequip and world load - while only the `.guilds` command ever reads them. Both are now built lazily behind getters.

Section 6 records a change that shipped inside 1.1.2 but appears in neither 1.1.2's player notes nor its developer changelog: the custom-NPC dressing function was extracted out of `:spawnpoint:customnpc` into a standalone include, removing a large dependency chain from every NPC AI setup script. It rode along inside `e6a6be9`, a commit whose documented purpose was only the area-policy cache work.

## 2. Commit Timeline

| Commit | Subject | Sections |
|---|---|---|
| `ab3eb7c` | Merge pull request #330 from Andries1985/Test-Shard | - (no new content) |
| `190321d` | Class info fix | 3 |
| *(uncommitted)* | `.classinfo` compile fixes + synopsis regeneration | 3 |
| *(uncommitted)* | Civilian town-bound enforcement removal | 4 |
| *(uncommitted)* | Guild constant arrays built lazily (ZH3.0 port) | 5 |

---

## 3. New Player Command - .classinfo

**Files:** `pkg/opt/alryc/textcmd/player/classinfo.src` (new, 250 lines), `config/cmds.cfg`, `config/command_synopses.cfg`

A read-only report on the class-level math in `scripts/include/classes.inc`. It resolves the player's live class via `GetClasseIds()` / `IsFromThatClasse()`, then reports, in a gump:

- class name and current class level (`GetClasseName`, `IsFromThatClasse`)
- in-class and out-of-class point totals with percentages
- `PointsToNextClasseLevel()` - in-class points still needed for the next level, walking `(classe, total)` up together because class skills are a subset of the overall total
- `PointsToDropForNextLevel()` - out-of-class points that would have to come off instead, walking `total` down with `classe` fixed
- `PointsBeforeClasseDrop()` - how much out-of-class training fits before the level drops
- explicit "unreachable" branches for both routes, and a max-level (6) branch

`ClasseLevelFromCounts()` mirrors `IsFromThatClasse()`'s level decision purely from `(classe, total, number)`, which is what lets the three projection helpers replay the real rules against hypothetical totals.

### 3.1 Two authoring defects fixed before it compiled

**Nonexistent package include.** The file opened with `include ":mdgumps:gumps";`. There is no `mdgumps` package on this shard - `pkg/utils/mdgumps/` contains only empty directories (`commands/test`, `scripts/autoClose`, `scripts/yesNo`) with no `pkg.cfg` and no include files, and `classinfo.src` was the only file in the repository referencing that name. The real package is `gumps` (`pkg/utils/gumps`, `Name gumps`). Changed to `include ":gumps:gumps";`, matching the sibling convention in `pkg/opt/alryc/textcmd/test/gotoboat.src`. POL resolves `:gumps:gumps` to `pkg/utils/gumps/include/gumps.inc` through its `include/` fallback; both that and the explicit `:gumps:include/gumps` form are in use across the tree and both compile. All eight helpers the script uses - `GFCreateGump`, `GFPage`, `GFResizePic`, `GFTextLine`, `GFAddButton`, `GF_CLOSE_BTN`, `GFHTMLArea`, `GFSendGump` - resolve there with matching signatures.

**Invented function signature.** The script called `IsFromThatClasse( who, skills, forensicsallowed, mageryallowed, musicianshipallowed )`, producing four `Too many arguments passed. Expected 3, got 5.` errors. The real signature at `classes.inc:632` takes only `( who , classe_skills , forensicsallowed := 0 )`.

There is no magery or musicianship allowance anywhere in `classes.inc`. The script's `GetClasseFlags()` helper - which returned a three-element flag array and asserted that Crafter receives magery and musicianship allowances - was replaced with `ClasseAllowsForensics( classeid )`, returning 1 for `CLASSEID_RANGER` and 0 otherwise. That mirrors the only allowance any real predicate passes: `IsRanger()` at `classes.inc:615` is the sole caller supplying a non-default value. `ClasseCounts()` lost the same two phantom parameters.

### 3.2 Registration

`config/cmds.cfg` enumerates command directories explicitly. `pkg/opt/alryc/textcmd/test` was already listed under `CmdLevel Test`, but `textcmd/player` is a new directory in that package and was not listed under `CmdLevel Player` - the command would have compiled and still not existed in-game. Added `DIR pkg/opt/alryc/textcmd/player`.

`config/command_synopses.cfg` is generated by `pythonscripts/_gen_command_synopses_cfg.py` from `// Synopsis:` lines and had no `classinfo` entry, so `.help` would not have listed it. Regenerated: 340 synopses written, diff is exactly the seven-line `Command classinfo` block at `Level Player` / `CmdLevel 0`. That the generator derived `Player`/`0` independently confirms the `cmds.cfg` entry resolves.

### 3.3 Latent bug observed, deliberately not mirrored

In `IsFromThatClasse()`, `amount` is declared outside the skill loop and is only assigned in the non-forensics branch. When `forensicsallowed` is set and `i == SKILLID_FORENSICS`, `amount` keeps the previous iteration's value, which then leaks into `classe` via `if( i in classe_skills )`. This never fires with current callers: Ranger is the only caller passing 1 and its skill list does not contain `SKILLID_FORENSICS`, while Powerplayer does list forensics but passes 0. `classinfo.src`'s `ClasseCounts()` implements the clean semantics (forensics excluded from `total`, class skills summed from their own values) rather than reproducing the quirk.

---

## 4. Civilian NPC AI - Town-Bound Enforcement Removed

**Files:** `scripts/include/anchors.inc`, `scripts/include/townsfolk.inc`, `scripts/ai/townperson.src`, `scripts/ai/person.src`, `scripts/ai/noble.src`, `scripts/ai/minstrel.src`

### 4.1 What the system did

`c442d6c` added a city-confinement layer to the civilian AI, extended by `a3c99f6` (+195 lines in `anchors.inc`) and `676adfb`. Three entry points:

1. `InitTownBounds()` at script start - recorded the spawn point's region as `TownHomeX/Y/Z/Realm/Region` obj-properties, then called `SendToJailAndRespawn()` and returned 0 if the home location had no region name, or if that region was not a city (`RegionIsCity()`, accepting either `City 1` or `Type City`). An NPC spawned outside a city was therefore killed at spawn.
2. `WanderWithinTown()` - `EnforceTownBounds(0)`, door handling, `Wander()`, `EnforceTownBounds(0)` again.
3. `EnforceTownBounds(1)` inside `RunFromOpponent()`'s flee loop, once per iteration, so a civilian chased past its city border was killed mid-flee.

`SendToJailAndRespawn()` did `MoveObjectToLocation(me, DEFAULT_LOCATION_JAIL_X/Y/Z, "britannia", MOVEOBJECT_FORCELOCATION)` followed by `me.kill()`.

Worth recording: `EnforceTownBounds()` opened with an `if (IsInsideHomeTown()) return 1; else SendToJailAndRespawn(); return 0; endif`, which made everything below it unreachable - the `from_combat` check, the `MoveObjectToLocation` back to `TownHomeX/Y`, and the ten randomised retry placements were all dead code. In practice the system only ever jailed and killed; it never returned an NPC to its home point.

### 4.2 Removed from `anchors.inc`

`RegionIsCity`, `InitTownBounds`, `IsInsideHomeTown`, `EnforceTownBounds`, `WanderWithinTown`, `SendToJailAndRespawn`, plus the debug scaffolding that existed only to instrument them: `AnchorsDbg()`, `const ANCHORS_DEBUG`, the six `_dbg_*_calls` counters, and `const REGION_RESOURCE`. Verified by grep that nothing outside `anchors.inc` referenced any of them. The file goes from 374 to 89 lines.

**Retained:** `drop_anchor` (called by roughly thirty AI scripts and setup includes), and the door helpers `TryOpenNearbyNpcDoors`, `StepCivilianThroughDoorIfNeeded`, `IsDoorPushCivilianTemplate` and `FacingToMoveDir` - `townsfolk.inc`'s `FleeOpenDistance()` still calls the first two, and `StepCivilianThroughDoorIfNeeded()` depends on the other two. The top-of-file `use`/`include` lines were left exactly as they were, including `include "include/constants/locations"`, whose only in-file consumer was the jail constants: several scripts include `anchors.inc` and may be relying on it transitively, so pruning those lines was out of scope for this change.

### 4.3 `townsfolk.inc`

`RunFromOpponent()` no longer calls `EnforceTownBounds(1)` at the top of each flee iteration. The `survived` local is gone and the function returns 1 unconditionally; the return value is kept rather than changed to a bare `return`, so the existing `if (!RunFromOpponent( ... )) return; endif` call sites in all four AI scripts still compile untouched. The doc comment was rewritten to drop the "jails/kills if chased out of town" contract.

### 4.4 The four AI scripts

Each `InitTownBounds()` startup gate became `drop_anchor(me);`, and each `WanderWithinTown()` call site became the sequence `WanderWithinTown()` itself performed minus the two bounds checks - `TryOpenNearbyNpcDoors()`, `StepCivilianThroughDoorIfNeeded()`, `Wander()`.

- `townperson.src`, `person.src` - the pre-existing `//drop_anchor(me);` comment was uncommented; `person.src` also had a dead `//WanderWithinTown();` line at the top of its event loop, removed.
- `noble.src` - the `hasTownBounds` local is gone, and its wander guard `if (hasTownBounds and ReadGameClock() >= next_wander)` becomes `if (ReadGameClock() >= next_wander)`.
- `minstrel.src` - both wander sites, one at the top of the event loop and one on the `next_wander` timer.

`performer.src` was left alone: it includes `townsfolk.inc` for `singsongs`/`PlayMidi`/`PlaySFX` but never called any bounds function.

### 4.5 The anchor

`drop_anchor()` reads `dstart`/`psub` from `npcdesc.cfg` and calls `SetAnchor(me.x, me.y, dstart, psub)`, defaulting `psub` to 10. Every civilian template involved - `townperson`, `person`, `peasant`, `minstrel`, `performer`, `noblemale`, `noblefemale`, `quest_target` - sets `dstart 10` with no `psub`, so all of them anchor as `SetAnchor(x, y, 10, 10)`.

This is the pre-`c442d6c` behaviour and is what restoring `drop_anchor` means, but it is a real change in roam radius relative to the shipped 1.1.2 build: under the bounds system `drop_anchor` was commented out in `townperson.src` and `person.src`, so those two had no anchor at all and roamed their entire home city freely until the border killed them. They are now softly leashed to roughly ten tiles of their spawn point instead. Commenting `drop_anchor(me)` back out in any of the four scripts gives genuinely unbounded roaming.

### 4.6 Verification

All 89 scripts under `scripts/ai/` recompile with zero errors, which covers every direct includer of `anchors.inc` (62 files) and every `townsfolk.inc` consumer.

---

## 5. Guild Package - Large Constant Arrays Now Built Lazily

**Files:** `pkg/opt/guilds/include/guildconstants.inc`, `scripts/textcmd/player/guilds.src`

Ported from the equivalent ZH3.0 fix (`GetGuildColours()` / `BuildGuildColours()` in that shard's `guildconstants.inc`, dated 2026-09-25 in its own comment).

`guildconstants.inc` declared two large arrays at file scope:

- `var CLOTHING := { ... }` - 59 clothing objtypes
- `var COLOURS := { ... }` - 163 hues

A file-scope `var X := <initialiser>` in an include is executed at the **start of every script that includes the file**, not once globally. Both arrays were therefore constructed by all ten consumers of `guildconstants.inc`, while only `scripts/textcmd/player/guilds.src` - the `.guilds` command - ever reads either one.

The consumers, direct and transitive:

| Script | Reaches it via | Reads the arrays? |
|---|---|---|
| `scripts/control/skilladvancerequip.src` | `guilds.inc` | no |
| `scripts/control/skilladvancerunequip.src` | `guilds.inc` | no |
| `pkg/opt/guilds/guild_uniform.src` | `guilds.inc` | no |
| `pkg/opt/guilds/ondelete.src` | `guilds.inc` | no |
| `pkg/opt/versebook/versebook.src` | direct + `guilds.inc` | no |
| `pkg/opt/guilds/commands/player/c.src` | direct + `guildchat.inc` | no |
| `pkg/opt/guilds/commands/player/ca.src` | `guildchat.inc` | no |
| `pkg/opt/guilds/commands/player/co.src` | `guildchat.inc` | no |
| `pkg/opt/guilds/commands/test/changeguildownership.src` | direct | no |
| `scripts/textcmd/player/guilds.src` | direct | **yes** |

The first two are the expensive ones: `skilladvancerequip.src` and `skilladvancerunequip.src` are the equip/unequip control scripts, so both arrays were being built on **every item equip and every item unequip** - including once per equipped item for every character at world load - purely as include side effects, and then never touched.

Each array became a lazily-populated script global behind a getter, matching ZH3.0's shape:

```
var COLOURS := 0;

function GetGuildColours()
	if (!COLOURS)
		COLOURS := { ... };
	endif
	return COLOURS;
endfunction
```

`guilds.src` now takes a local at each use site - `var guildClothing := GetGuildClothing();` in `PurchaseUniform()` and `var guildColours := GetGuildColours();` in the colour picker - and the `.size()` call sites were repointed to those locals. The implicit `_item_iter` / `_colour_iter` foreach counters are unaffected. Scripts that only include the file for its `const`s now build neither array at all.

Difference from ZH3.0 worth recording: there, only `COLOURS` needed this, because by then it was computed by `BuildGuildColours()` (a 2000-iteration sweep of hues 1000-2999 minus exclusion bands, roughly 30k steps per script start) while `CLOTHING` was still a short literal left as a plain `var`. On 2.5 both are still plain literals, so both carry the same per-script construction cost and both were converted. 2.5 has no `BuildGuildColours()`, `GUILD_COLOUR_EXCLUDE_RANGES` or `GUILD_COLOUR_PER_PAGE` - its picker still uses the hand-listed 163-hue table, and this change deliberately does not port ZH3.0's wider hue range or its paged picker, only the lazy-initialisation fix.

All ten consumers recompile with zero errors.

---

## 6. Carried Over From 1.1.2 - Custom-NPC Dressing Extracted (Undocumented There)

**Files (all shipped in 1.1.2):** `scripts/ai/setup/dressCustom.inc` (new), `animalsetup.inc`, `archersetup.inc`, `criersetup.inc`, `killpcssetup.inc`, `sheepsetup.inc`, `spellsetup.inc`, `pkg/opt/shrink/Use_Shrink.src`

Found by diffing the full `7107d6d..97a7dcd` file list against the section 17 table in `developer-changelog-v1.1.2.md`: ten files changed in that range appear in neither that table nor `patch-v1.1.2.md`.

`DressNPCCustom()` - fifteen lines that copy a spawn point's stored items onto a spawned NPC, equipping them or routing `packitem`-flagged copies to the backpack - was extracted from `pkg/opt/spawnpoint/include/customnpc.inc` into a new standalone `scripts/ai/setup/dressCustom.inc`. The two bodies are byte-identical apart from the new file's own `use uo; use os;`. Seven call sites switched from `include ":spawnpoint:customnpc"` to `include "ai/setup/dressCustom"`.

The dependency avoided is the point. `customnpc.inc` is 411 lines and pulls in `:gumps:old-gumps` (1,415 lines) plus `include/attributes`, `include/client` and `include/constants/gumpids`, and declares `use cfgfile`, `use vitals` and `use attributes` - all of it compiled into every NPC AI setup script purely to reach one small function. AI setup runs per spawned NPC, so this is a per-NPC memory reduction across the whole shard, which places it with 1.1.2's area-policy work and the `.memdump` tool rather than as an isolated refactor.

It landed in `e6a6be9` ("Area policy memory update"), documented in 1.1.2's section 14 as only the area-policy mask/line cache simplification. `pkg/opt/spawnpoint/checkpoint.src` still calls `DressNPCCustom()` via `include ":spawnpoint:customnpc"`, so both definitions remain live; they are identical today, and any future edit must be applied to both or the duplicate removed.

Two loose ends checked and dismissed: `killpcssetup.inc` also gained `use util` and `include "include/randname"`, which look like a fix but are redundant - every AI script that includes `killpcssetup.inc` already includes `randname`, and the `RandomName()` call at its line 72 dates to the initial commit. `pkg/opt/ArtifactSystem/artifactbox.src` adds `UOBJ_EON_PRISM` to the two-week relic decay list; the substance is covered in 1.1.2's section 10 prose, only the file-table row was missing.

Also noted, not patch-notes material: `pkg/opt/spawnpoint/checkpoint.zip`, a 4.4 KB binary, was committed into the package tree by `d0220a8`, a commit otherwise about area-policy realm guards. It looks accidental and should be removed.

---

## 7. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `pkg/opt/alryc/textcmd/player/classinfo.src` | 3 | New - `.classinfo` player command; `mdgumps` include and `IsFromThatClasse` arity both corrected |
| `config/cmds.cfg` | 3 | `DIR pkg/opt/alryc/textcmd/player` added under `CmdLevel Player` |
| `config/command_synopses.cfg` | 3 | Regenerated; adds `Command classinfo` (`Player`, `CmdLevel 0`) |
| `scripts/include/anchors.inc` | 4 | Bounds/jail system and its debug scaffolding deleted; 374 -> 89 lines. `drop_anchor` and the four door helpers retained |
| `scripts/include/townsfolk.inc` | 4 | `RunFromOpponent()` no longer enforces town bounds; `survived` removed, returns 1 |
| `scripts/ai/townperson.src` | 4 | `InitTownBounds()` gate -> `drop_anchor(me)`; two `WanderWithinTown()` sites -> doors + `Wander()` |
| `scripts/ai/person.src` | 4 | Same, plus removal of a dead `//WanderWithinTown();` line |
| `scripts/ai/noble.src` | 4 | Same, plus `hasTownBounds` local and its wander-guard condition removed |
| `scripts/ai/minstrel.src` | 4 | Same, two wander sites |
| `pkg/opt/guilds/include/guildconstants.inc` | 5 | `CLOTHING` / `COLOURS` file-scope array literals replaced with lazy `GetGuildClothing()` / `GetGuildColours()` getters |
| `scripts/textcmd/player/guilds.src` | 5 | Colour picker and `PurchaseUniform()` take locals from the new getters |
| `patchnotes/developer-changelog-v1.1.3.md` | - | This file |
| `patchnotes/patch-v1.1.3.md` | - | Player-facing notes |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content |

Unchanged but relevant: `scripts/ai/performer.src` includes `townsfolk.inc` but never called the bounds functions, so it needed no edit.

---

## 8. Risk and Regression Notes

- **Civilians can now leave guarded territory entirely (section 4).** This is the intended change, but it is a genuine behavioural opening: townspeople, persons, nobles and minstrels may now wander out of their city, which means they can reach wilderness or dungeon approaches, draw monster aggression, and die without guard protection. Under the old system that was impossible by construction. Worth watching whether civilian populations near city edges thin out over time, since nothing respawns them faster to compensate.
- **Civilians spawned outside a city now survive (section 4.1).** `InitTownBounds()` used to kill them at spawn. Any spawn point that was placed outside a city region - deliberately or by accident - will now start producing persistent NPCs where it previously produced nothing. If the shard has such points, this may surface as unexpected civilian clusters.
- **Roam radius changed for `townperson` and `person` (section 4.5).** These two had `drop_anchor` commented out under the bounds system and so were unanchored; restoring the pre-system `drop_anchor(me)` applies `SetAnchor(x, y, 10, 10)` from their `dstart 10` template entries. Relative to the live 1.1.2 build they will visibly roam *less*, not more. If the goal is genuinely free roaming rather than pre-system parity, comment `drop_anchor(me)` back out in the four scripts.
- **Dead guard branches left in place (section 4.3).** `RunFromOpponent()` now always returns 1, so every `if (!RunFromOpponent( ... )) return; endif` in the four AI scripts is unreachable. Kept deliberately so the call sites did not need touching, but they are no longer doing anything and should not be read as live error handling.
- **Existing NPCs keep the old behaviour until they cycle.** AI scripts are per-NPC processes running compiled `.ecl`. Already-spawned civilians continue under the old bounds logic until they respawn or the server restarts, so a restart is the clean way to deploy this.
- **`anchors.inc` retains a now-unneeded include.** `include "include/constants/locations"` no longer has an in-file consumer. Left deliberately, since `anchors.inc` is included by 62 files that may resolve `DEFAULT_LOCATION_*` or other constants through it; removing it is a separate, testable cleanup.
- **`DressNPCCustom()` is defined twice (section 6).** `scripts/ai/setup/dressCustom.inc` and `pkg/opt/spawnpoint/include/customnpc.inc` hold identical copies, and `checkpoint.src` still reaches the latter. Any future change must touch both, or the duplicate should be removed and `checkpoint.src` repointed.
- **`.classinfo` calls a function with a side effect (section 3).** `IsFromThatClasse()` calls `unequipRestrictedItems(who)` in its `classe >= 3675` branch, and `GetLiveClasseId()` calls it once per class, so opening the gump can unequip class-restricted gear. This is not new behaviour introduced by the command - every routine class predicate (`IsWarrior()`, `IsMage()`, and so on) does the same thing throughout the codebase - but the script's header comment claims it "does not touch any properties", which is not strictly accurate. Either reword that comment or have the report replay the maths without calling the live predicate.
- **Guild array laziness is per-script, not global (section 5).** `GetGuildClothing()`/`GetGuildColours()` cache into a script global, so the array is built at most once per script instance - the `.guilds` command rebuilds it on each invocation rather than sharing one copy shard-wide. That is the same trade ZH3.0 made and is the right one here (only one command reads them), but it means this is a reduction in wasted work, not a shared cache. The guard is `if (!COLOURS)`, which is safe because both builders always return a non-empty array.
- **Not part of this patch, currently uncommitted:** `pkg/systems/accounts/config/uoclient.cfg` has the listener port changed from 2599 to 2598. If that is a local testing tweak it should not ship; if it is intended for live it is player-facing and needs its own patch-note entry, since players would have to reconnect on a different port.
