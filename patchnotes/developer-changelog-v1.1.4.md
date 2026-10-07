# Developer Changelog - v1.1.4
**Range:** `3631ada` (patchnotes commit of v1.1.3) -> working tree  
**Branch:** Patch-1.1.4  
**Date:** 2026-10-07  
**Commits in range:** 0 non-boundary commits. `927916d` (merge of Patch-1.1.3 into master) carries no new content - `git diff 3631ada..HEAD` is empty. The whole release is the uncommitted working tree.  
**Files changed:** 29 modified code/config files, 2 renamed into a new package and rewritten, 17 new files, plus this release's own 3 `patchnotes/` files

> Scope was computed against `3631ada`, not `master`. `master` only caught up with 1.1.3 via `927916d`; anything older than that is already shipped.

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Powerhour Package - Shared Include and Scheduler Rewrite](#3-powerhour-package---shared-include-and-scheduler-rewrite)
4. [New Admin Command - .phadmin](#4-new-admin-command---phadmin)
5. [Player Commands - .setph and .ph Fixes](#5-player-commands---setph-and-ph-fixes)
6. [.resetph Registered - Unreachable Since 1.1.2](#6-resetph-registered---unreachable-since-112)
7. [Eon-Prism - Moved Onto the Shared Helpers](#7-eon-prism---moved-onto-the-shared-helpers)
8. [Reviewed, Not Changed - Hunting Powerhour Loot Count Condition](#8-reviewed-not-changed---hunting-powerhour-loot-count-condition)
9. [Warrior for Hire - Package Move and AI Port From ZH3.0](#9-warrior-for-hire---package-move-and-ai-port-from-zh30)
10. [Warrior for Hire - Heart, Backup and High Priest Revival](#10-warrior-for-hire---heart-backup-and-high-priest-revival)
11. [Warrior for Hire - Gear Escrow](#11-warrior-for-hire---gear-escrow)
12. [Warrior for Hire - Combat, Protection Cap, Guards and GM Tools](#12-warrior-for-hire---combat-protection-cap-guards-and-gm-tools)
13. [Tracking - Gump Menu From ZH3.0](#13-tracking---gump-menu-from-zh30)
14. [Staff Speedwalk - Added From ZH3.0](#14-staff-speedwalk---added-from-zh30)
15. [Houses - Travel Checks Use the Footprint, Not the Rune's Height](#15-houses---travel-checks-use-the-footprint-not-the-runes-height)
16. [Classes - Per-Skill Bonus Lookup, Powerplayer Curve, Stat Affinity, Primary Class, Equipment Sweep](#16-classes---per-skill-bonus-lookup-powerplayer-curve-stat-affinity-primary-class-equipment-sweep)
17. [Code Review Follow-ups](#17-code-review-follow-ups)
18. [Exhaustive File-by-File Change List](#18-exhaustive-file-by-file-change-list)
19. [Risk and Regression Notes](#19-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.4 has three topics: a code review of `pkg/opt/powerhour` with the two staff features that came out of it (sections 3-8), a port of ZH3.0's Warrior for Hire work onto this shard's systems (sections 9-12), ZH3.0's single-window tracking menu (section 13), the Seer `.speedwalk` command (section 14), ZH3.0's house-travel fix (section 15), and a class-system review that started from ZH3.0's finding about skills shared between classes (section 16).

The review found two real defects in the server-wide scheduler (`powerhour.src`), both rooted in state that lived in script locals while the type flag lived in a persisted global property. A restart mid-powerhour left the flag on with nothing left to turn it off, so the flag stayed set until the next weekly powerhour's cleanup erased it, up to a week later. And the "second chance" bonus roll compared against a random type re-rolled every minute rather than the type that had actually run, so the rule introduced in `063be4e` ("if sunday bonus PH is resource, more likely to hit the 2nd bonus power hour") never applied. Section 3 rewrites the scheduler around a small set of persisted global properties and a new shared include, `pkg/opt/powerhour/include/powerhour.inc`, which every script in the package (and the Eon-Prism) now reads its constants and helpers from.

Section 4 adds `.phadmin`, an Administrator-level gump that starts a chosen powerhour on demand, edits or ends the active one's remaining time, and edits the weekly schedule that was previously a hard-coded Sunday 19:00 GMT constant duplicated in two files.

Section 5 fixes a player-facing lockout in `.setph` (pressing OK with nothing selected left the "gump open" flag set until relog), a stale-timer bug where an old personal powerhour's background script could end a freshly started one, and an up-to-24-hour error in `.ph`'s "next eligible" countdown for Sunday users.

Section 6 is a registration fix: `.resetph`, shipped in 1.1.2 with a full changelog entry, was never callable because its `textcmd/test` directory was not listed in `config/cmds.cfg`.

Section 7 covers the Eon-Prism, which already existed from 1.1.2 and is the "artifact that resets your personal powerhour". It was not duplicated; it was moved onto the shared eligibility helper so it cannot drift from `.setph`/`.ph` again.

Section 8 records one finding deliberately left for a design decision: an inverted condition in `scripts/include/starteqp.inc` that controls whether hunting-powerhour loot stacks are doubled.

Sections 9-12 bring over the Warrior for Hire work ZH3.0 did between 2026-09-12 and 2026-09-30: the mercenary moved into its own `pkg/opt/warriorforhire` package, lost its masterless-recruit and mount code, gained persisted stat/skill mirrors, skill growth, a status gump and vital ceilings, and its death became recoverable in three ways instead of one - a heart with eleven lives that reaches an offline owner's bank box, a `WFHBackup` the High Priest can rebuild a warrior from for 10,000 gold, and a 90-day escrow of the corpse's gear the High Priest returns for 20,000. ZH3.0 and 2.5 have drifted apart underneath that code (realms, include paths, the vitals system, the throwing skill, the escrow helpers), so each piece was re-based on 2.5's own mechanisms rather than copied; section 9.5 lists what was deliberately not ported.

Section 13 ports ZH3.0's tracking menu: the two classic client menus the skill used here become one paged gump that stays open while the player switches categories. 2.5's player tracking, which ZH3.0 never had, is kept as a category, and the skill no longer throws away the cached NPC definitions file on every use.

Section 14 adds the Seer `.speedwalk` command. It was written for 2.5 in April 2025 but only on side branches that never reached this line; the version here is ZH3.0's August fix, which sends the client packet directly instead of through a helper package this shard does not have.

Section 15 ports ZH3.0's house-travel fix: a rune marked on an upper floor of a demolished house could recall or gate its holder into the stranger's house placed there later, because every travel script looked for a house at the rune's height. A new `scripts/include/housetravel.inc` finds the house by footprint at any height, on placed houses, courtyards and static houses alike, and a refused rune is destroyed as runes to forbidden areas already are.

Section 16 is the class review. ZH3.0 found that the per-skill class lookup returned the first class in list order that listed a skill, so any class whose skills also appear in an earlier class lost its gain bonus and second roll on them: Warriors on six of eight skills, Paladins on six, Powerplayers on 48 of 49. 2.5 has the same code. The lookup now follows the classes the character holds; the Powerplayer moves to a skill-wide small curve with no second roll; stat affinity, written years ago and never wired in, now applies to every class with the table decided in this session; every level-based rule, item legality included, judges a dual-class character by its strongest class; and the illegal-equipment sweep that ran inside the class bonus functions on every hit and spell is gone.

## 2. Commit Timeline

| Commit | Subject | Sections |
|---|---|---|
| `927916d` | Merge pull request #332 from Andries1985/Patch-1.1.3 | - (no new content) |
| *(uncommitted)* | Powerhour shared include + scheduler rewrite | 3 |
| *(uncommitted)* | `.phadmin` admin gump, `cmds.cfg` Admin entry, synopsis regeneration | 4 |
| *(uncommitted)* | `.setph` / `.ph` fixes | 5 |
| *(uncommitted)* | `.resetph` directory registered under `CmdLevel Test` | 6 |
| *(uncommitted)* | `eonprism.src` onto shared helpers | 7 |
| *(uncommitted)* | Warrior for Hire package move, AI port, migration shim | 9 |
| *(uncommitted)* | Heart/backup rework, High Priest revival and escrow services, Companion's Second Wind | 10 |
| *(uncommitted)* | Gear escrow include, sweeper, corpse-decay hand-off, `.escrow` filtering | 11 |
| *(uncommitted)* | Damage moderation, protection cap, `callguards` reset, `.setwfhdamage`, synopsis regeneration | 12 |
| *(uncommitted)* | Tracking gump menu (ZH3.0 port), player tracking kept, `npcdesc` unload removed | 13 |
| *(uncommitted)* | `.speedwalk` (Seer), packet include, login restore, synopsis regeneration | 14 |
| *(uncommitted)* | House travel by footprint: `housetravel.inc`, Recall/Gate/Mark/Teleport/Earth Portal, runebook | 15 |
| *(uncommitted)* | Class lookup, Powerplayer curve, stat affinity, primary class, sweep removal, `.classbonusinfo` | 16 |

---

## 3. Powerhour Package - Shared Include and Scheduler Rewrite

**Files:** `pkg/opt/powerhour/include/powerhour.inc` (new, 426 lines), `pkg/opt/powerhour/powerhour.src` (62 -> 140 lines)

### 3.1 What the old scheduler did

`powerhour.src` was a `while(1)` loop with `Sleep(60)` at the bottom. Every iteration it rolled `randomPH := Random(3)+1`, then:

- if `((systime - starttime) % week) > (week - 120)` and nothing was `started`, broadcast the 2-minute warning, `sleep(120)`, set `started := ReadGameClock()`, and set one of the global properties `PHH` / `PHC` / `PHS` (hunting / half-resources / double skillgain) according to that iteration's `randomPH`;
- if `started` and an hour had passed on the game clock, broadcast the end, erase all three flags, and roll for a bonus: `Random(2000) < 1000` (50%) if `randomPH == 2`, else `Random(10000) < 1000` (10%). A successful roll set a local `secChance`, which the next iteration consumed by starting another random powerhour.

`starttime` was the literal `1543172400` (2018-11-25 19:00 GMT, a Sunday), declared separately in `powerhour.src` and in `textcmd/player/ph.src`.

### 3.2 The two defects

**Stuck flag after a restart.** `PHH`/`PHC`/`PHS` are ordinary global properties and are saved with the world. `started` was a local. After a restart mid-powerhour the flag was still set but `started` was 0, and the end branch is gated on `started`, so nothing ever cleared it. The flag stayed on - every crafting, loot and skill-gain consumer kept honouring it - until the next weekly powerhour's end branch did its blanket `EraseGlobalProperty` of all three, up to a week later. The Eon-Prism refuses to work while a server-wide powerhour is active, so a stuck flag also disabled that item for the same window.

**Bonus roll conditioned on the wrong value.** `randomPH` was re-rolled at the top of every loop iteration, including the iteration that ran the end branch an hour later. The `if(randomPH == 2)` test there therefore looked at that minute's fresh roll, not the type that had started. The realised outcome was a flat 1/3 x 50% + 2/3 x 10% = 23.3% bonus chance on every week regardless of type, rather than 50% on half-resources weeks and 10% otherwise. (The long-run average is identical; the conditioning was simply never applied.)

Two smaller items in the same file: `secChance` was also a local and so a granted-but-not-yet-started bonus was lost to a restart; and the script used `sleep()` and `Sleep()` with only `use uo;`, reaching the `os` module transitively through `include/random`.

### 3.3 New state model

All server-wide state now lives in global properties, documented at the top of `powerhour.inc`:

| Property | Meaning |
|---|---|
| `PHH` / `PHC` / `PHS` | The active type flag. **Unchanged** - every consumer in `pkg/std/*`, `scripts/include/skillpoints.inc`, `scripts/include/starteqp.inc`, `scripts/items/bladed.src`, `pkg/opt/alchemyplus`, `pkg/opt/crafterboost` and `pkg/std/treasuremap` keeps working untouched. |
| `PH_EndTime` | `systime` at which the active powerhour ends. |
| `PH_Source` | `"scheduled"`, `"bonus"` or `"admin"` - what started the active one. |
| `PH_Pending` | Type currently inside its 2-minute warning. |
| `PH_Request` | Type an admin asked to start; consumed by the scheduler. |
| `PH_SecondChance` | 1 while a bonus powerhour has been granted and is waiting to start. |
| `PH_StartTime` | Weekly schedule anchor. Absent means `PH_DEFAULT_START_TIME` (`1543172400`), i.e. the old constant - **no behaviour change until an admin edits the schedule**. |
| `PH_SchedulerPID` | pid of the running scheduler script. |

Personal-powerhour state is unchanged in shape (`#PPHH`/`#PPHC`/`#PPHS` unsaved, `pph_use_time`/`pph_use_weekday` saved, `#SettingPH` unsaved) but is now accessed only through helpers.

### 3.4 The include

`powerhour.inc` is `use uo; use os; use basic;` and is included as `:powerhour:include/powerhour` (the package name from `pkg.cfg` is `powerhour`). It provides:

- constants: `PH_MINUTE/HOUR/DAY/WEEK`, `PH_DURATION` (3600), `PH_WARNING_SECONDS` (120), `PH_TICK_SECONDS` (60), the three `PH_TYPE_*` ids, every property name above as a `PH_GPROP_*` / `PPH_PROP_*` constant, the broadcast hue 1176 and the two sysmessage hues (2601 / 2595) the package already used;
- type helpers: `PH_TypeName()` ("Hunting" / "Half-resources" / "Double skillgain"), `PH_GlobalProp()`, `PPH_Prop()`, `PH_IsValidType()`;
- server-wide state: `PH_ActiveType()`, `PH_EndTime()`, `PH_SecondsRemaining(now)`, `PH_PendingType()`, `PH_RequestedType()`, `PH_BonusPending()`, `PH_ClearActive()`;
- schedule: `PH_GetStartTime()`, `PH_WeekdayOf(secs)` (the existing `(secs/86400 + 4) % 7`, 0 = Sunday), `PH_WeekdayName()`, `PH_GetSchedule(byref weekday, hour, minute)`, `PH_SetSchedule(weekday, hour, minute)`, `PH_ScheduleString()` ("Sunday 19:00 GMT"), `PH_SecondsUntilScheduled(now)`, `PH_InWarningWindow(now)`, `PH_FormatDuration(secs, show_seconds := 0)`;
- scheduler process: `PH_FindScheduler()`, `PH_EnsureScheduler()`, `PH_NudgeScheduler()`;
- personal: `PPH_ActiveType()`, `PPH_UseTime()`, `PPH_UseWeekday()`, `PPH_SecondsRemaining()`, `PPH_IsEligible()`, `PPH_SecondsUntilEligible()`, `PPH_Clear()`.

`PH_SetSchedule()` rebuilds the anchor as `PH_REFERENCE_SUNDAY + weekday*86400 + hour*3600 + minute*60`, where `PH_REFERENCE_SUNDAY` is `1543104000` (2018-11-25 00:00 GMT, the Sunday midnight the original constant sits on). `PH_GetSchedule()` is its inverse. The trigger test is `PH_SecondsUntilScheduled(now) <= 120`, the same 120-second window the old `elapsed > week - 120` test described, with an inclusive edge.

`PPH_IsEligible()` keeps the exact rule the three callers shared - `weekday_now < use_weekday || now > use_time + week` - but reads both properties through `CInt()`. The old code compared the raw `GetObjProperty()` result against integers, which for a character with no `pph_use_time` yet meant comparing `error` with an integer and relying on the engine's cross-type ordering to come out as "eligible". It did in practice, but it was never a guaranteed result.

`PH_FindScheduler()` resolves `PH_SchedulerPID` with `GetProcess()` and then checks the process name contains `powerhour/powerhour` (or the backslash form), so a pid reused by an unrelated script after a restart is not mistaken for the scheduler.

### 3.5 The scheduler loop

```
program powerhour()
	existing := PH_FindScheduler(); if(existing && existing.pid != GetPid()) return 0;
	SetGlobalProperty("PH_SchedulerPID", GetPid());
	RecoverAfterRestart();
	while(1)
		if(active)      -> EndPowerhour(active) once systime >= PH_EndTime
		elseif(request) -> StartPowerhour(request, "admin")
		elseif(bonus)   -> StartPowerhour(Random(3)+1, "bonus")
		elseif(window)  -> StartPowerhour(Random(3)+1, "scheduled")
		WaitForTick();  -> Wait_For_Event(60), draining any queued wake events
	endwhile
```

- **Single instance.** `start.src` (run at boot) and `.phadmin` (when it finds no live scheduler) can both start the script; the guard at the top makes the second one exit.
- **`RecoverAfterRestart()`.** A type flag with no `PH_EndTime` - which is exactly what a pre-1.1.4 world save looks like if it was taken mid-powerhour, and also the old stuck-flag state - is given a full hour from now rather than being cut off or left on. A `PH_Pending` left over from a restart during the warning is converted into a `PH_Request`, so the powerhour players were promised still happens (re-broadcasting the warning first).
- **`StartPowerhour(type, source)`.** Sets `PH_Pending`, broadcasts the warning ("A powerhour will start in 2 minutes!" for admin starts, the original "A random powerhour will start in 2 minutes!" otherwise), sleeps 120, then sets the flag, `PH_EndTime := systime + 3600`, `PH_Source`, clears `PH_Pending` and broadcasts "<Type> powerhour has started!". Messages are the originals.
- **`EndPowerhour(type)`.** Broadcasts "<Type> powerhour has ended!", `PH_ClearActive()`, then rolls for a bonus **only if `PH_Source` was `"scheduled"`**: `Random(100) < 50` for half-resources, `< 10` otherwise, using the type that actually ran. A bonus or admin-started powerhour never spawns a follow-up, which preserves the old at-most-one-bonus-per-week behaviour and keeps staff-started hours from producing surprises.
- **Event-driven tick.** `Wait_For_Event(60)` replaces `Sleep(60)`. `.phadmin` sends a `ph_wake` event after every change so the scheduler reacts immediately instead of up to a minute later; the drain loop discards any extras.

`include/random`'s `Random()` (the shard's own LCG) is still what the scheduler rolls with.

---

## 4. New Admin Command - .phadmin

**Files:** `pkg/opt/powerhour/textcmd/admin/phadmin.src` (new, 323 lines), `config/cmds.cfg`, `config/command_synopses.cfg`

Administrator-level (`CmdLevel 4`). Built on `:gumps:include/gumps` and `gumps_ex` (`GFCreateGump`, `GFResizePic`, `GFTextLine`, `GFRadioButton`, `GFTextEntry`, `GFAddButton`, `GFSendGump`, `GFExtractData`) with the same art the areas editor uses (9250 background, 9200 panels, 4005/4006 and 4017/4018 buttons, 208/209 radios). Logs via `:staff:include/staff`'s `LogCommand()` on open and once per action.

On open it checks `PH_FindScheduler()`; if the scheduler is dead it starts it via `PH_EnsureScheduler()` and tells the admin, or refuses if even that fails. It then loops: send the gump, act on the button, redraw, until Close or the gump's X.

The gump:

- **Status.** Active type and `PH_FormatDuration()` of the time remaining, or "Starting shortly" during the warning, or "Start requested" while a request waits for the scheduler, or "No server-wide powerhour is active." Below: the schedule as `PH_ScheduleString()` and the countdown to it, the server clock in GMT (so the admin can relate the schedule to "now"), and either a "bonus powerhour pending" line or a reminder that powerhours last an hour and the weekly one picks a random type.
- **Start a powerhour now.** Three radios (ids 11/12/13) and a Start button (id 1). Shown only when nothing is active, pending or requested; otherwise replaced by a "wait for the current one to finish" line. `HandleStart()` re-validates both the selection and the state, writes `PH_Request`, nudges the scheduler, and erases the request again if the nudge fails. The 2-minute warning is always broadcast.
- **Active powerhour time.** Shown only while one is active. A 4-character text entry (id 21) prefilled with the minutes remaining (rounded up), an Apply button (id 2) and an End now button (id 3). `HandleApplyTime()` accepts whole numbers 1-1440 (`PHADMIN_MAX_MINUTES`), sets `PH_EndTime := systime + minutes*60`, nudges, logs, and broadcasts "The <Type> powerhour will now end in <duration>!" so players are not surprised by the `.ph` countdown jumping. `HandleEndNow()` sets `PH_EndTime := systime` and nudges; the scheduler then ends it through the normal path with the normal "has ended" broadcast, which is why the command does not clear the flags itself.
- **Weekly schedule (GMT).** Three text entries (ids 22/23/24; weekday 0-6 with 0 = Sunday, hour 0-23, minute 0-59) prefilled from `PH_GetSchedule()`, and a Save button (id 4). `HandleSaveSchedule()` requires all three to be whole numbers (`IsWholeNumber()` - tolerates surrounding whitespace, rejects signs, blanks and multiple words), delegates range checking to `PH_SetSchedule()`, nudges the scheduler (a new moment inside the next 2 minutes fires right away), logs old -> new, and reports the new schedule and countdown.
- **Footer.** Refresh (id 5) redraws; Close (id 6) exits.

Text-entry values come back from `SendDialogGump()` as `"<id>: <text>"` and are read through `GFExtractData()`; a missing entry extracts to `error`, which `IsWholeNumber()` rejects, so an action button can never read a stale or absent field as 0.

### 4.1 Registration

`config/cmds.cfg` gained `DIR pkg/opt/powerhour/textcmd/admin` under `CmdLevel Admin`, placed alphabetically between the `karmafame` and `powerscrolls` entries. `config/command_synopses.cfg` was regenerated with `pythonscripts/_gen_command_synopses_cfg.py` (341 synopses, up from 340); the diff is the new seven-line `Command phadmin` block at `Level Administrator` / `CmdLevel 4` plus the reworded `.setph` synopsis (section 5). The generator deriving `Administrator`/`4` on its own confirms the directory resolves.

---

## 5. Player Commands - .setph and .ph Fixes

**Files:** `pkg/opt/powerhour/textcmd/player/setph.src` (144 -> 134 lines), `pkg/opt/powerhour/textcmd/player/ph.src` (64 -> 35 lines)

### 5.1 `.setph` - lockout after OK with no selection

The gump's result was read as `foreach key in (gumpdata.keys) playerchoice := key; endforeach` - "whichever key sorts last", relying on the radio id (100/200/300) sorting after the button key 0. With no radio selected the only key is 0, `playerchoice` stayed 0, the `case` matched nothing, and `activateph()` - the only place that erased `#SettingPH` - never ran. Every later `.setph` then hit "You have the gump window open already!" until the player relogged (`scripts/misc/logon.src` and `reconnect.src` erase the property). 

Now `#SettingPH` is set immediately before `SendDialogGump()` and erased immediately after it returns, whatever happened; the radios are checked explicitly (`gumpdata[SETPH_RADIO_HUNT]` etc.); and the no-selection case tells the player to pick one. Because the gump can sit open indefinitely, the active and eligibility checks are repeated after it returns before anything is set.

### 5.2 `.setph` - stale timer ending a fresh powerhour

`activateph()` sets the flags, `Sleep(3600)`s inside the command script, then erases all three flags. If staff `.resetph` the player (or they used an Eon-Prism) during that hour and then started a new personal powerhour, the old sleeping script still woke an hour after the *first* start and erased the flags of the *second* one, ending it early with the wrong "has ended" message. `activateph()` now remembers the `pph_use_time` it stamped and returns without touching anything if the property no longer matches. It also only sends "has ended" if the flag is still set, so a powerhour already dropped by logoff ends silently as before. The hour is still a `Sleep()` in the command script, as it was.

The three "Half-Resources Powerhour" / "Double-Skillgain Powerhour" wordings in `.setph` became "Half-resources" / "Double skillgain" via `PH_TypeName()`, matching `.ph` and the broadcasts. The synopsis line was reworded from "Activate your hunting power hour bonus" (it was never hunting-only) to "Start your weekly personal powerhour (hunting, half-resources or double skillgain)".

### 5.3 `.ph` - Sunday countdown and new information

The "you can start a new personal powerhour in ..." countdown computed `days_to_reset := (7 - use_weekday) % 7`, mapping 0 to 7, and reported the next Sunday 00:00 GMT. For a powerhour used on a Sunday the eligibility rule's `weekday_now < use_weekday` branch can never fire (`weekday_now < 0`), so the real reset is `use_time + 604800`, i.e. the following Sunday at the same clock time - up to 24 hours later than reported. `PPH_SecondsUntilEligible()` handles the Sunday case explicitly.

The server-wide section now also says "It ends in <duration>." while one is active (previously only *that* one was active was reported) and "A server-wide <Type> powerhour is about to start!" during the 2-minute warning. The "next chance" line uses `PH_FormatDuration(..., 1)`, which prints the same days/hours/minutes/seconds as before but drops zero components. The weekly anchor is read from `PH_GetStartTime()`, so `.ph` follows whatever `.phadmin` sets.

---

## 6. .resetph Registered - Unreachable Since 1.1.2

**Files:** `config/cmds.cfg`, `pkg/opt/powerhour/textcmd/test/resetph.src` (31 lines, rewritten onto the helpers)

1.1.2 added `pkg/opt/powerhour/textcmd/test/resetph.src` (section 9 of that changelog) but never added its directory to `config/cmds.cfg`. The core only auto-discovers package command folders named `commands/<level>/` (which is why `pkg/opt/ArtifactSystem/commands/admin` works with no entry); `textcmd/<level>/` folders are found only through explicit `DIR` lines, and every other such folder in the tree has one - `pkg/opt/powerhour/textcmd/test` was the single exception. `.resetph` compiled, was listed in `command_synopses.cfg`, and did not exist in-game. Added `DIR pkg/opt/powerhour/textcmd/test` under `CmdLevel Test`.

The script itself now calls `PPH_Clear()` for the property wipe and, if the target had a personal powerhour running, tells the staffer that the reset ended it - the 1.1.2 risk note flagged that silent early end.

---

## 7. Eon-Prism - Moved Onto the Shared Helpers

**File:** `pkg/opt/ArtifactSystem/eonprism.src` (62 -> 48 lines)

The request for "an artifact that resets your personal powerhour" is already met by the Eon-Prism (`item 0x792F`, 1.1.2 section 10): it clears the weekly lock when the player is neither running a personal powerhour nor already eligible, and refuses during a server-wide powerhour. Nothing new was added.

The file dropped its private `PH_DAY`/`PH_WEEK` constants, `use math`, and its hand-copied eligibility block in favour of `PPH_ActiveType()`, `PH_ActiveType()`, `PPH_IsEligible()` and `PPH_Clear()` from `:powerhour:include/powerhour`. The messages, the flavour phrases, `SFX_SPELL_RESURRECTION`, and the load-bearing `ReleaseItem()`-then-`DestroyItem()` order are untouched. This introduces a compile-time dependency from the `artifactsystem` package on the `powerhour` package; both are `Enabled 1`. The one behavioural nuance is the `CInt()` coercion noted in section 3.4, which only matters for a character with no `pph_use_time` property - who is told, as before, that they do not need the prism.

---

## 8. Reviewed, Not Changed - Hunting Powerhour Loot Count Condition

**File:** `scripts/include/starteqp.inc`, `CreateFromStackString()`, line 1557 - **not modified**

Three loot paths in `starteqp.inc` double a chance-drop's chance (capped at 100) when `GetGlobalProperty("PHH") || GetObjProperty(killer, "#PPHH")`. The third, `CreateFromStackString()`, additionally decides the stack *count*:

```
if(!GetGlobalProperty("PHH") || !GetObjProperty(killer, "#PPHH"))
	item := CreateItemInContainer( who , objname , count );
else
	item := CreateItemInContainer( who , objname , count*2 );
endif
```

`!A || !B` is true unless both are set, so the doubled count only ever happens when a server-wide *and* a personal hunting powerhour are active at the same time. The surrounding structure ("neither -> normal, otherwise double") reads as an intended `!A && !B`. The line is unchanged since the initial import (`f8450b9`, 2022-09-07), so this has been the live behaviour for the shard's whole history. Correcting it would double stack sizes from this path during every hunting powerhour, a loot-economy change rather than a code fix, so it is left for a deliberate decision. The one-line fix is `||` -> `&&` at that line.

---

## 9. Warrior for Hire - Package Move and AI Port From ZH3.0

**Files:** `scripts/ai/warrior.src` -> `pkg/opt/warriorforhire/warrior.src` (renamed and rewritten, 1624 -> 1510 lines), `scripts/items/warriorforhire.src` -> `pkg/opt/warriorforhire/warriorforhire.src` (renamed), new `pkg/opt/warriorforhire/{pkg.cfg,itemdesc.cfg,include/wfhcommon.inc,include/wfhvitals.inc}`, `scripts/ai/warrior.src` (new, migration shim), `config/npcdesc.cfg`, `config/itemdesc.cfg`

**Source.** ZH3.0's Warrior for Hire work landed in four commits on that repository - `2e2b24a` ("Bunch of fixes", 2026-09-12: package move, escrow, heart rework, High Priest revival, GM tools), `ec26d19` ("More updates", 2026-09-13: escrow registry and sweeper), `cdd3ef2` ("Fable 5.1 fixes", 2026-09-24: the `Follow()` member-name fix) and `e0ff379` ("Patch Notes", 2026-09-30: vital ceilings) - and is written up in that repository's `developer-changelog-v3.1.1.md` section 15 and `developer-changelog-v3.1.3.md` section 10. The checkout compared against is `D:\Zuluhotel2\zuluhotel_omega_3` (branch Patch-3.1.3).

**Baseline.** Before any of that, 2.5's `scripts/ai/warrior.src` differed from ZH3.0's pre-move copy by 52 lines, every one of them a realm argument ZH3.0 adds to location calls, its `:mdgumps:` and `NPCBackpacks` include paths, its four-argument `check_speech()`/`TakeItem()` signatures, or 2.5's own April 2026 healing-on-resurrect fix (the `me.dead`/`who.dead` guards and 16-second self-heal cooldown in `ApplyHealing()`). The port therefore applies ZH3.0's hunks onto 2.5's file and keeps all of those 2.5-side differences.

### 9.1 What moved and what points at it

- The AI and the deed script now live in `pkg/opt/warriorforhire` (package name `warriorforhire`). The `warriorforhire` template in `config/npcdesc.cfg` has `script :warriorforhire:warrior`; deed `Item 0xa399` left `config/itemdesc.cfg` (a pointer comment remains) for the package's own `itemdesc.cfg` with `Script :warriorforhire:warriorforhire`. Colour 1160 is kept (ZH3.0 uses 2669). `config/mrcspawn.cfg` sells the deed by objtype and needed no change.
- **Migration shim.** POL saves each NPC's script name: `data/npcs.txt` holds ten live warriors with `script warrior`, which after the move would resolve to nothing and leave them without an AI. `scripts/ai/warrior.src` is now a 17-line script that sets `me.script := ":warriorforhire:warrior"` and calls `RestartScript(me)`, so each of the ten migrates itself the first time it starts after the restart. New warriors never run it.

### 9.2 Behaviour removed (as on ZH3.0)

- **Masterless recruitment.** `AskToJoin()`, `JoinThem()`, `sleepmode()`, `ProcessMasterlessEvents()` and the masterless branches of `WarriorMovement()` are gone; a warrior only ever exists with an owner (deed, heart or High Priest). Speech is always enabled at range 12.
- **Mounts.** `MountHorse()` and the `ride`/`mount`/`dismount`/`transfer` commands are gone; each now answers "I'm afraid I'm not comfortable with mounts, master."
- **Guard-distance break-off.** `CloseDistance()` no longer abandons a fight to run back to the guarded target when that target is more than 8 tiles away (the old `gd` branches). ZH3.0 removed this with the rest; kept for parity.
- **The bored-and-offline hide.** A warrior whose owner is offline no longer hides itself; it just drops war mode.

### 9.3 Behaviour added

- **`SyncStatusMirror()`** replaces the five unconditional `SetObjProperty()` calls at the top of every loop iteration with write-on-change, and additionally mirrors the nine tracked skills (Anatomy, Parry, Healing, Tactics, Archery, Swordsmanship, Macefighting, Fencing, Wrestling) into a `Skills` dictionary CProp. The heart and the High Priest's backup read these off the corpse (section 10).
- **`CheckOwnerModWipe()`** calls `WipeMods()` whenever the owner's online state flips and every 30-60 minutes besides.
- **`GainSkill()`**, called from `fight()` beside `GainStat()`: every 5 minutes of fighting, each tracked skill below the master's own value has a `(Random(140)+1) > skill` chance to gain 0.1, never past 130.0 (`WFH_SKILL_CAP_TENTHS`).
- **`<name> status`** opens a gump (`:gumps:include/gumps`) with stats, hits, armour and protections, the nine skills, worn equipment by layer (backpack, hair and beard filtered out) and "Deaths survived: n / 10".
- **`SetMeUp()`** (first run only): a hire is a fixed 75/50/50 recruit with the nine skills at 50 (Healing 100), always dressed by `::/misc/dressme weakwarrior`, with its vitals stamped (9.4). Before, it rolled 20-60 per stat and set every listed skill to 130; and because the deed sets `master` only after the template script has already run `SetMeUp()`, a deed-bought warrior always took the "masterless" 130-skill branch anyway.
- **`Follow()`:** `following.cmd`/`master.cmd` became `.cmdlevel` (`.cmd` is not a member, so the staff-follow guard never fired); the `+5000`/`-5000` dexterity-mod sprint around `RunToward()` is gone.
- **`OpenMyPack()`** works from 3 tiles instead of 1 and says so when the owner is too far.
- **`Release()`** erases the owner's `henchman` and `WFHBackup` and calls `AbandonWarrior()` (`wfhcommon.inc`: flags `AbandonedByOwner` and `guardkill`, then kills) instead of turning the warrior loose as a masterless NPC.
- `GetLiveMaster()` factors the master lookup out of `GainStat()`, which now also re-stamps vitals and calls `SyncStatusMirror()`.

### 9.4 Vitals on this shard

ZH3.0's `wfhvitals.inc` sets HP = 2 x base Str, Mana = base Int, Stam = base Dex through its attributes package's `SetNpcVitalOverride()`. 2.5 has no such package: its NPC maximums come from `pkg/opt/shilhook/regen.src`, where an NPC's life maximum is Strength x 100 unless a `CustomHitsLevel` CProp (in hundredths) overrides it, with `CustomManaLevel`/`CustomStaminaLevel` likewise. The 2.5 `wfhvitals.inc` therefore stamps those three CProps from the base stats and calls `RecalcVitals()` (`vitals.em`) so the engine re-reads them; `SetWfhVitalCeilingsAndFill()` also tops the vitals up. It runs on hire, on every hourly `GainStat()` and on both High Priest restore paths. A fresh hire has 150 hits; before, max HP equalled Strength.

### 9.5 Deliberately not ported

- Throwing (skill 57, `ATTRIBUTEID_THROWING`) and ZH3.0's `HasThrowing()`/`HasRanged()`: this shard has no throwing skill. `HasBow()` stays the ranged check and the tracked-skill list has nine entries, not ten.
- The `HITS`/`MANA`/`STAM`/`RunSpeed` template fields ZH3.0 added: `regen.src` reads but never uses the first three, and `RunSpeed` belongs to ZH3.0's movement rework.
- ZH3.0's other changes in the same files that are not about the warrior: the `hitscriptinc.inc` class-damage rework, the snooping and animal-trainer changes, the `mainloop*` move-delay refactor.

---

## 10. Warrior for Hire - Heart, Backup and High Priest Revival

**Files:** `scripts/misc/death.src`, `scripts/ai/highpriest.src`, `pkg/opt/warriorforhire/resetwfhdeaths.src` (new), `pkg/opt/warriorforhire/itemdesc.cfg`

### 10.1 `death.src`

The heart block now:

- skips a corpse flagged `AbandonedByOwner` entirely (no heart, no backup; the gear stays on the corpse);
- allows ten revivals instead of three (`resnum < 11`, was `< 4`);
- resolves the owner with `SYSFIND_SEARCH_OFFLINE_MOBILES` when they are not online and puts the heart in their bank box (`FindBankBox()` from `util/bank`) instead of creating a heart nobody receives;
- copies the `Skills` mirror onto the heart;
- writes a `WFHBackup` dictionary (Str, Int, Dex, Name, Sex, resnum, Skills) onto the owner on every death, which is what lets the High Priest rebuild a warrior whose heart has disintegrated.

2.5's own `mastersrl == error` guard and heart colour 1172 are kept (ZH3.0 uses 2750).

### 10.2 `highpriest.src`

Three new speech commands, matched case-insensitively and inserted after `boost` in the not-upset branch:

- **"resurrect warrior"** - if the player has no living henchman and has a `WFHBackup`: `YesNo()` confirm, `spendgold(10000)`, then a new `warriorforhire` is created at the relocation point, given the backup's stats, vitals, name, gender, `resnum` and skills (`SetBaseSkillBaseValue()` per tracked skill), has its template gear destroyed, is bound as the player's `henchman`, restarted and moved to the player. If creation fails the gold is refunded (`CreateItemInBackpack(player, "goldcoin", 10000)`, the item name `merchant.src` already uses).
- **"give up warrior"** - confirm, then `AbandonWarrior()` on a living one; either way clears `henchman` and `WFHBackup`.
- **"recover belongings"** - offers the oldest Warrior for Hire escrow package (section 11) for 20,000 gold via `WFH_ClaimEscrowEntry()`, or disposes of it on request via `WFH_DestroyEscrowEntry()`; refunds if nothing could be moved.

The existing heart-revival path (`SYSEVENT_ITEM_GIVEN`) changed in four places: the stat loss is a flat `resnum-1` instead of `Random(resnum)*5` (0 to 5 points per past death, rolled separately per stat); the restored warrior gets `SetWfhVitalCeilingsAndFill()`; its tracked skills are restored from the heart's `Skills`; and the owner's `henchman` CProp is set to the new serial. That last one fixes a 2.5 bug: a heart revival never updated `henchman`, so the deed's "You already have a henchman" check and the High Priest's own check kept looking at the dead warrior's serial.

### 10.3 Companion's Second Wind

`resetwfhdeaths.src` + `Item 0x7930` (`wfhdeathreset`, graphic 0x2256, hue 1153, `VanityCost 10`): sets the user's living warrior's `resnum` to 0 and mirrors that into `WFHBackup`. `pkg/opt/vanityshop/vanityshop.src` reads `:*:itemdesc` for `VanityCost`, so it appears in the vanity shop without further registration. ZH3.0 uses objtype `0x4000BC`; this shard has no other objtype above `0x1FFFF`, so the item took the next free slot after the Eon-Prism (`0x792F`) in the gap the 1.1.2 changelog documents as unused.

### 10.4 Forgiveness price clamp (not a warrior change, same file)

`CheckWhyHeGave()` compared the forgiveness donation against the raw `getlevel*2500` and only then clamped `getlevel` (not the price) to 1000-60000, so a classless player was forgiven for any coin and the clamp never did anything. ZH3.0 fixed this on 2026-09-23 by clamping the price before the comparison. Ported, with one addition ZH3.0 does not need: 2.5's June 2026 "relationship" command quoted the same unclamped number (and said "any amount of gold" for a classless player), so both places now go through a new `PriestForgivenessFine(classlevel)` helper - class level x 2500, floored to 1000 and capped at 60000, the remove-curse bounds. Effect: a classless player now owes 1000 gold to repair the relationship instead of 1; everyone with a class pays what they already did.

---

## 11. Warrior for Hire - Gear Escrow

**Files:** `pkg/opt/warriorforhire/include/wfhescrow.inc` (new, 343 lines), `pkg/opt/warriorforhire/wfhescrowsave.src` (new), `pkg/opt/warriorforhire/escrowsweep.src` (new), `pkg/opt/warriorforhire/start.src` (new), `scripts/control/corpsedecay.src`, `pkg/systems/playervendor/commands/player/escrow.src`

**Flow.** When `ProcessNpcCorpseDecaying()` reaches a `warriorforhire` corpse that is not `AbandonedByOwner`, has a `master` and still holds root items, it bundles those items into a backpack created at the corpse and starts `:warriorforhire:wfhescrowsave` with `{payout, ownerserial, corpse.serial}` before destroying the corpse. That script calls `WFH_SaveToEscrow()`, which creates a root backpack in the player-vendor "Merchant Escrow" storage area, moves the payout into it, registers the entry in a package-wide registry datafile (`:warriorforhire:wfhescrowregistry`: entry id -> owner serial, escrow name, created time) and indexes it in the owner's escrow datafile under vendor name "Warrior for Hire" and vendor key = corpse serial. `escrowsweep.src` (started by `start.src`) runs `WFH_SweepExpiredEscrow()` once a day, destroying everything older than 90 wall-clock days straight off the registry. The High Priest claims or disposes of entries (section 10.2). `.escrow` filters "Warrior for Hire" entries out of its listing (`LoadEscrowEntriesForSerial()`) and refuses to claim one (`ClaimEscrowEntry()` guard), so they only come back through the priest and his fee.

**Adapted to 2.5's escrow include.** ZH3.0's include hand-builds the `|`/`^`-delimited index strings; 2.5's `pkg/systems/playervendor/include/escrow.inc` already has `EncodeEscrowEntries()`/`DecodeEscrowEntries()` with field escaping, the `Entry*()` accessors and `BuildEscrowDatafileNameForSerial()`, so the 2.5 include is written on those. Three consequences: the per-owner index is written even when the owner cannot be resolved (serial-keyed filespec, which ZH3.0's `BuildEscrowDatafileName(owner_obj)` could not do); the registry filespec is package-scoped (`:warriorforhire:...`) per the warning on `BuildEscrowDatafileNameForSerial`; and every filing, claim, destroy and expiry is logged to `merchantescrow.log` through `MerchantEscrowLog()`.

**Two other deliberate differences.** ZH3.0 includes the whole escrow include in `corpsedecay.src`, i.e. in every per-corpse decay process; here the decay script only does the cheap bundle-and-hand-off and the datafile work runs once in `wfhescrowsave.src`. And where ZH3.0 destroys the payout if filing fails, 2.5 leaves the pack on the ground where the corpse was and logs an error, so a filing failure can never delete gear.

---

## 12. Warrior for Hire - Combat, Protection Cap, Guards and GM Tools

**Files:** `pkg/systems/combat/include/hitscriptinc.inc`, `scripts/control/skilladvancerequip.src`, `scripts/control/skilladvancerunequip.src`, `pkg/opt/areas/callguards.src`, `pkg/opt/warriorforhire/textcmd/test/setwfhdamage.src` (new), `config/cmds.cfg`, `config/command_synopses.cfg`

- **Damage moderation.** `WFHModifyDamage()` runs first thing in `DealDamage()`, so it covers the astral, on-hit and plain paths alike: a warrior hitting its own master does half damage (sparring); a warrior hitting an NPC is scaled by `WFHDamageToNPCs`, or `WFHDamageToBosses` for a `Boss`/`SuperBoss`; an NPC hitting a warrior by `WFHDamageFromNPCs`/`WFHDamageFromBosses`. All four are global properties defaulting to 1.0 (`WFHDamageMultiplier()`), so nothing changes until staff set them. Only the two WFH functions and the one call were taken from ZH3.0's `hitscriptinc.inc`; its class-damage rework in the same commit was not.
- **`.setwfhdamage`** (Developer, `CmdLevel 5`): a four-field gump for those multipliers, built on `:gumps:include/gumps`/`gumps_ex` (ZH3.0's `:mdgumps:` package is empty here, as 1.1.3 section 3.1 found) and logged with `LogCommand()`. `config/cmds.cfg` gained `DIR pkg/opt/warriorforhire/textcmd/test` under `CmdLevel Test`, and `command_synopses.cfg` was regenerated (343 entries; this release adds `phadmin`, `setwfhdamage` and `speedwalk`).
- **Protection cap.** `DoImmunity()` and `UndoImmunity()` cap a warrior's `NecroProtection` and the five elemental/holy protections at 85 instead of 95 on both equip and unequip (the unequip side so a lower-but-still-above-85 remaining item cannot recompute back over the cap).
- **`callguards.src`.** `crimMaster` was declared once per call and never reset inside the mobile loop, so every tamed creature scanned after a criminal owner's pet also got a guard. Reset at the top of the loop. Not warrior-specific, but it sits in the same loop as the `WarriorForHire` guard-ignore check and the fix was in ZH3.0's warrior commit.

---

## 13. Tracking - Gump Menu From ZH3.0

**Files:** `pkg/std/tracking/tracking.src` (312 -> 377 lines)

ZH3.0's tracking script was itself derived from 2.5's in May 2026 (`df69926`). In August (`68e3a51`) it replaced the two classic client menus - "Select a category", then "Select a creature" - with a single gump, and in September (`cdd3ef2`) it dropped the `unloadconfigfile("::npcdesc")` the skill ran on every use. Both are now here.

- **One window instead of two menus.** Left column: the categories that actually have something in range, in a fixed alphabetical order; right column: the creatures in the chosen category. Both columns page ten at a time (More/Back, Next Page/Prev Page), and the window stays open while the player switches categories, so one successful skill check can be browsed instead of re-rolled. Picking a creature starts the same 12-report tracking loop as before. The frame is the OSI crafting-menu art (background 5054, tiled panels 2624, arrow buttons 4005/4007 and 4014/4016, exit 4017/4019) with the crafting section removed, built on `:gumps:include/gumps` (`GFCreateGump`, `GFResizePic`, `GFPicTiled`, `GFAddAlphaRegion`, `GFTextLine`, `GFAddButton`, `GFSendGump`) where ZH3.0 includes its `:mdgumps:gumps_ex`; the helper signatures are identical.
- **Single classification pass.** Every in-range mobile is bucketed once (`category_mobiles[type]` holding indices into a flat `critter_ids` list) instead of the old code's second pass per chosen category, which re-ran the boss-tier skill gates and `FindConfigElem()` for every mobile. The gates are unchanged: Lesser Boss at 100 tracking, Boss at 125, Super Boss at 140, Champion at 150, all listed under "Champion"; a boss the player is not yet skilled enough for is simply absent.
- **Player tracking kept.** ZH3.0 never tracked players, so its gump has no "Players" category. 2.5 did (`IsTrackablePlayer()`: no template, has an account), so "Players" is a 21st fixed category here, between Plant and Ratkin, with the same inclusion rule as before.
- **Icons gone.** The old menus showed a tile icon per category and per creature (`GetTrackingTypeIcon()`); the gump shows names only, as on ZH3.0. The icon table and `CategoryIndex()` went with the menus.
- **Diagnostics.** A creature whose template is missing from `config/npcdesc.cfg`, or whose template has no `Type`, now tells the tracker once per template ("An untrackable creature is nearby ... please report") instead of being skipped silently. All 752 templates in `config/npcdesc.cfg` have a `Type` and this shard has no package-level `npcdesc.cfg`, so the message cannot appear until someone adds a template without one.
- **No more config unload.** `unloadconfigfile("::npcdesc")` dropped the cached 1.15 MB NPC definitions file on every use of the skill and forced a re-read, while every running NPC script kept the old copy alive beside it. Removed, as on ZH3.0.

---

## 14. Staff Speedwalk - Added From ZH3.0

**Files:** `pkg/opt/alryc/include/speedwalk.inc` (new), `scripts/textcmd/seer/speedwalk.src` (new), `pkg/systems/accounts/logon.src`, `config/command_synopses.cfg`

2.5's live line has never had `.speedwalk`. It was written on 2025-04-19 (`d197dc0`, "Added speedboost for seer+ and fixed the staff robe colors") together with a `pkg/utils/objClassMethods` package whose `Character.SpeedWalk()` method sent the packet and a restore-on-login hook in the accounts package, but that commit exists only on the remote work branches (`2025-May-June-Work`, `Fall-2025-Work`, `Winter-2025-2026-Work`, `Beta-Shard`, `misc-fixes`) and is not an ancestor of this branch. ZH3.0 did carry it and hit the other failure: its `objClassMethods` package is disabled, so the method call silently did nothing until `7a3fb13` ("Speedwalk fix", 2026-08-19) moved the packet send into a plain include. That version is what 2.5 gets, with no helper package at all:

- **`SendSpeedWalk(mobile, toggle)`** (`:alryc:include/speedwalk`) builds packet `0xBF`, subcommand `0x26`, with the 0-4 modifier and sends it. The include carries `use polsys` itself for `CreatePacket()`.
- **`.speedwalk`** (Seer, `CmdLevel 2`, found through the already-registered `scripts/textcmd/seer`): no argument toggles between 1 and off; an argument 0-4 sets that modifier. The value is stored on the character as the saved `SpeedWalk` CProp.
- **`pkg/systems/accounts/logon.src`** re-sends the stored value for `cmdlevel >= 2` on every login, because the modifier is client state and is lost on reconnect. This is the accounts package's own logon hook (the engine runs every package's `logon.ecl` on login); it otherwise still does only the Fantasia-era max-clients check and `VerifyStaffOnline()`. `command_synopses.cfg` was regenerated (343 entries).

---

## 15. Houses - Travel Checks Use the Footprint, Not the Rune's Height

**Files:** `scripts/include/housetravel.inc` (new), `pkg/std/spells/recall.src`, `pkg/std/spells/gate.src`, `pkg/std/spells/mark.src`, `pkg/std/spells/teleport.src`, `pkg/opt/earth/earthportal.src`, `pkg/std/runebook/customspells.inc`, `pkg/std/runebook/runebook.src`

**The bug** (ZH3.0 `68a5c45`, 2026-10-02, their changelog 3.1.3 section 19; the same code is live here). Every travel script tested the destination with `GetStandingHeight( tox, toy, toz ).multi`. The engine answers that with the lowest standable surface at or above the given z, while `MoveObjectToLocation()` puts the mobile on the nearest surface within +7 above or any distance below. A rune marked on an upper floor of a house that was later demolished, with a stranger's smaller house placed on the spot, found "no house" at the rune's z (nothing of the new house is that high), passed the owner/friend test by default, and then dropped its holder onto the new house's floor. Static houses had the same hole from the other side: `StaticHouseSerialAtLocation()` looks for the floor plate with range 0 at the rune's z, so an upper-floor rune missed it. And a castle courtyard tile has no house piece under it at all, so it never counted as "in a house" for Recall, Gate or Mark.

**The helper.** `scripts/include/housetravel.inc` is ZH3.0's include rebuilt on this shard's housing:

- `FindHouseAtSpot( x, y, z, realm, scan_static := 1 )`: (1) any house multi whose footprint extent covers x,y at any height, via `ListMultisInBox( x, y, -128, x, y, 127 )`, the call `pkg/std/housing/utility.inc`'s `IsLocationInThisHouse()` already relies on for exactly this; (2) the engine's old test at that z, so steps, porches and boats behave as before; (3) a static house's sign, found through its floor plate on that tile with z ignored. ZH3.0 reads footprint boxes off its house signs instead; `pkg/std/housing` keeps no such data.
- `CanTravelIntoHouse( who, house, priv )`: `IsCowner()`, or a character on the account a static house's sign records in `owneracct`, or `IsFriend()` with the matching permission (`RECALL_TO`, `GATE_TO`, `GATE_FROM`). The account rule is what `customspells.inc` already meant to apply to static houses, but it compared `"OwnerAcct"` (wrong case, never set) and so never passed; `pkg/opt/statichousing/ssign.src` itself treats same-account characters as owners.
- `RemoveRunebookEntryAt( book, x, y, z )` and `DestroyRefusedRune( caster, cast_on, house, tox, toy, toz )`: a refused loose rune crumbles, a refused runebook entry (and a matching default location) is removed from the book, nothing happens for a boat. The same treatment forbidden areas already give a rune.

**The scripts.** Recall, Gate, Earth Portal and the runebook's `CustomRecall()`/`CustomGate()` run both the origin and the destination through the helper, destroy or remove a refused rune, and report a failed move ("Something blocks that location.") instead of failing silently. Mark uses the helper instead of `caster.multi`. Teleport refuses a destination inside a house footprint the caster may not enter ("You cannot teleport there.") after its existing supporting-multi test; NPC casts skip the static-plate scan. `CustomRecall()`/`CustomGate()` return `TRAVEL_REFUSED_BY_HOUSE` (-1) for a refused destination, and the three runebook callers remove that entry through a new `RemoveRefusedEntry()` in `runebook.src`. Each script's separate static-house block is gone, folded into the helper.

Two pre-existing slips fixed on the way: Gate and Earth Portal tested the *destination* static house with `GATE_FROM` instead of `GATE_TO`; Earth Portal left `#Casting` set on five early exits and said "You can't gate from to this house."

**Not ported:** ZH3.0's `traveldebug.inc` console tracing (temporary there), the realm argument on every call (single realm here), and the runic atlas objtypes.

---

## 16. Classes - Per-Skill Bonus Lookup, Powerplayer Curve, Stat Affinity, Primary Class, Equipment Sweep

**Files:** `scripts/include/classes.inc`, `scripts/include/skillpoints.inc`, `pkg/opt/alryc/textcmd/test/classbonusinfo.src` (new), `config/command_synopses.cfg`

### 16.1 The per-skill lookup (ZH3.0 `9164261`, their changelog 3.1.3 section 22)

`GetClasseIdForSkill( skillid )` took only the skill and returned the first class in `GetClasseIds()` order whose list contained it; `IsSpecialisedIn()` then read the character's level in that class. Since nine of the ten classes share skills with a class earlier in the list, a classed character got the class skill-gain multiplier and the failed-check second roll only on skills no earlier class also lists. Computed from 2.5's own lists: Bard, Crafter, Mage and Thief were whole; Bladesinger, Mystic Archer and Ranger lost four of eight; Paladin and Warrior lost six of eight; the Powerplayer lost 48 of 49. The three consumers are the gain multiplier in `skillpoints.inc` and the second roll and success-level bump in `pkg/opt/shilhook/shilhook.src` (the success-level function has no caller on either shard; it is dead code and was left alone). The direct `ClasseBonus( who, CLASSEID_X )` calls in combat, spells and crafting name their class and were never affected.

The lookup now takes the character, walks the classes they hold and returns the highest-level one that lists the skill, 0 if none; `IsSpecialisedIn()` returns an explicit 0. The Powerplayer is excluded by design (16.2). Unlike ZH3.0 no Thief special case is needed: 2.5's Thief list still contains Detecting Hidden and Snooping, and there is no Throwing here.

### 16.2 Powerplayer

Decided 2026-10-07: no second roll, and the skill-wide bonus follows the "small" curve already defined in `classes.inc` (1 + 0.15 per level: 1.15 at level 1 to 1.9 at level 6) on every skill, replacing the 1.1 / 1.2 / 1.3 table that only applied at levels 3 to 5. It reads the Powerplayer level directly rather than through `GetClasseLevel()`. A Powerplayer can never hold a second class (16.4), so nothing stacks.

### 16.3 Stat affinity

`HaveStatAffinity()`, `HaveStatDifficulty()` and `GetStatPointsMultiplier()` existed in `classes.inc` with nothing calling them. `AwardPoints()` now passes each stat advancement's dice amount through a new `ClassStatGainAmount()`: multiplied by the class bonus (1 + 0.25 per level) for an affinity stat, divided by it for a difficulty stat, rounded half up, never below 1. Raw stat points are applied against the tenth's cost as a probability, so this scales the chance of gaining 0.1 by the same factor at every stat level; caps are untouched. The tables, as decided:

| Class | Faster | Slower |
|---|---|---|
| Warrior, Crafter | Strength | Intelligence |
| Mage | Intelligence | Strength, Dexterity |
| Bard | Dexterity, Intelligence | Strength |
| Bladesinger | Dexterity | Strength |
| Mystic Archer | Dexterity, Intelligence | Strength |
| Ranger | Dexterity | Strength |
| Thief | Dexterity | Strength, Intelligence |
| Paladin | Intelligence | Dexterity |
| Powerplayer | nothing | nothing |

Both tables use a new `HighestHeldClasseLevel()` instead of the first-match walk. Where a character's classes disagree on a stat the higher level wins, a tie favouring the affinity.

### 16.4 Primary class

`AssignClasse()` stamps a level for every class the character qualifies for, so two can be held. The qualifying rule (eight class skills totalling at least 600 and at least 60% of all skill points) means two classes sharing no skills can never coexist and a Powerplayer is always alone; only the sibling pairs sharing four skills can, and only on a build with little trained outside those twelve skills. The save file bears this out: of 3,533 characters, one holds two classes (Ranger 3, Mystic Archer 1). `GetClass()` took the first held class in list order, so that character was a level 1 Mystic Archer to all 31 level-based callers and, through `IsProhibitedByClasse()`'s if/else chain in the same order, was judged by the Mystic Archer item rules. New `GetPrimaryClasseId()` returns the held class with the highest level, ties keeping list order so single-class characters are unchanged; `GetClass()` and the legality chain use it. The class weapon lists were left as primary-class only: Thief, Bard and Mage can never pair with each other and their possible partners have no list, so a union could never differ.

### 16.5 Equipment sweep

`ClasseBonus()`, `ClasseBonusBySkillId()` and `IsFromThatClasse()` each ran `unequipRestrictedItems()` before doing their job: every worn item, two 5 ms sleeps each, then the legality rule. `ClasseBonus` is called up to six times per hit and 22 times per spell damage calculation, and `IsFromThatClasse` is behind every `IsThief()`/`IsMage()`/`IsBard()` predicate in the hit scripts, so every melee hit and damaging spell paid roughly 100 to 200 ms inside the hit script. Equip time is already guarded by `skilladvancerequip.src`, and class levels only change inside `AssignClasse()`, which now sweeps once after stamping every class instead of once per qualifying class. `AssignClasse()` runs at login, from the class commands and stones, and every ten minutes for every online classed character from `pkg/opt/summoning/checkclasse.src`, so the sweep still happens on that cadence. The sweep was removed from the three functions; the throttled safety-net variant was considered and dropped (decided 2026-10-07).

### 16.6 Verification tool

`.classbonusinfo` (Developer): targets a character and prints the classes held, the primary class and level, the Powerplayer multiplier if any, the three stat multipliers, and every skill that carries a per-skill class bonus with the class, level and multiplier. Synopses regenerated (344 entries). All 138 scripts that include `classes.inc` compile with `ecompile -w` at 0 errors, 0 warnings.

---

## 17. Code Review Follow-ups

A review pass over the whole working tree (2026-10-07) produced ten findings. Seven were confirmed and fixed; three were judged acceptable and left. Every affected script recompiles at 0 errors.

**Fixed:**

- **Sleep inside a critical section** (`scripts/misc/death.src`). The offline-owner heart path called `util/bank`'s `FindBankBox()`, which sleeps up to a second, from inside `npcdeath`'s `set_critical(1)` block, which would have stalled every script on the shard for that long on each such death. A local `FindBankBoxNoSleep()` does the same World Bank lookup without the sleep.
- **Unbounded memory rebuilds** (`death.src`, `scripts/ai/highpriest.src`). The `WFHBackup` snapshot was written only on deaths that dropped a heart and never consumed, so after the heart disintegrated the same stale memory could be rebuilt for 10,000 gold indefinitely, and a heart in the bank could be revived on top of a memory-rebuilt body. Now the backup is written on every death, including the disintegrating one, the rebuild erases it (the living warrior writes a fresh one on its next death), and the rebuild destroys any heart for that warrior in the player's backpack and bank box ("The old heart crumbles to dust."). Net rule: ten free heart revivals, then each further life costs 10,000 gold at the priest, one per death.
- **High Priest frozen on an unanswered prompt** (`highpriest.src`). The four new `YesNo()` dialogues ran with no timeout inside the priest's single event loop, so one player walking away from the gump would stop him serving anyone else. They now pass `WFH_PROMPT_TIMEOUT` (30 s); an ignored gump closes as "no".
- **Courtyard tiles were still uncovered** (`scripts/include/housetravel.inc`). `ListMultisInBox` is documented (`core-changes.txt`) as listing multis that have a piece inside the box, so the single-tile box the port used could not see a castle courtyard, which is exactly what the fix claimed to cover. The helper now scans a house-sized box (34 tiles) around the spot and tests each house's `.footprint` rectangle, the core member that gives the world-coordinate extent. A core without that member falls through to the old tests.
- **Account rule reached placed houses** (`housetravel.inc`). Placed houses carry `owneracct` too (`housedeed.src`, `changeowner.src`), so the same-account allowance would have let any alt recall, gate and mark inside a placed house without being a friend. It now applies only when the house object is a static sign, not a multi, which is what was decided.
- **`.setwfhdamage` accepted anything** (`setwfhdamage.src`). `CDbl()` of "abc" or "1,5" silently stored 0.0 (read back as 1.0) and a negative value made a warrior heal bosses. Every field must now be a plain number between 0.1 and 10 or nothing is saved.
- **`.speedwalk` validation was dead** (`speedwalk.src`). `CInt()` never returns an error object, so "abc" became "toggle off" and "-3" went straight into the packet and was stored for re-send on every login. The argument must now be a single digit 0-4.

**Also tightened:**

- The "Warrior for Hire" vendor name that `wfhescrow.inc` writes and `escrow.src` filters on is now one constant, `ESCROW_WFH_VENDOR_NAME`, in `pkg/systems/playervendor/include/escrow.inc`, so the filter cannot drift from the writer.
- `wfhescrowsave.src`: if filing into escrow fails, the payout pack goes to the owner's bank box instead of being left on the ground where anyone could loot it; only if that also fails does it stay put, and the log says which.

**Left as is:**

- `RemoveRunebookEntryAt()` in `housetravel.inc` duplicates runebook bookkeeping that `runebook.src` owns. True, and the same shape ZH3.0 ships; the entry layout has not changed in years and the two live in the same patch. Noted for the day the runebook storage format changes.
- The one compile warning across the whole tree, an unused `from_charge` parameter in `customspells.inc`, predates this patch.

---

## 18. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `pkg/opt/powerhour/include/powerhour.inc` | 3 | New - shared constants, state/schedule/personal helpers, scheduler process helpers |
| `pkg/opt/powerhour/powerhour.src` | 3 | Rewritten - persisted end time/source/pending/request/bonus state, restart recovery, event-driven tick, single-instance guard, bonus roll keyed to the type that ran |
| `pkg/opt/powerhour/textcmd/admin/phadmin.src` | 4 | New - `.phadmin` admin gump (start now / edit or end active / edit weekly schedule) |
| `pkg/opt/powerhour/textcmd/player/setph.src` | 5 | `#SettingPH` cleared on gump close; explicit radio checks; no-selection message; post-gump re-validation; `activateph()` ownership check; wording via `PH_TypeName()`; synopsis reworded |
| `pkg/opt/powerhour/textcmd/player/ph.src` | 5 | Onto the helpers; Sunday countdown fix; shows active remaining time and the warning state; schedule read from `PH_GetStartTime()` |
| `pkg/opt/powerhour/textcmd/test/resetph.src` | 6 | Onto `PPH_Clear()`; reports when the reset ended a running personal powerhour |
| `pkg/opt/ArtifactSystem/eonprism.src` | 7 | Onto the shared helpers; private constants and `use math` removed |
| `config/cmds.cfg` | 4, 6, 12 | `DIR pkg/opt/powerhour/textcmd/admin` under `CmdLevel Admin`; `DIR pkg/opt/powerhour/textcmd/test` and `DIR pkg/opt/warriorforhire/textcmd/test` under `CmdLevel Test` |
| `config/command_synopses.cfg` | 4, 5, 12, 14, 16 | Regenerated (344 entries): adds `Command phadmin` (`Administrator`, `CmdLevel 4`), `Command setwfhdamage` and `Command classbonusinfo` (`Developer`, `CmdLevel 5`) and `Command speedwalk` (`Seer`, `CmdLevel 2`); `setph` synopsis reworded |
| `pkg/opt/warriorforhire/warrior.src` | 9 | Moved from `scripts/ai/warrior.src` and ported: masterless/mount code removed; status mirror, mod wipe, skill growth, status gump, vitals, abandon-on-release |
| `pkg/opt/warriorforhire/warriorforhire.src` | 9 | Moved from `scripts/items/warriorforhire.src` (deed) |
| `pkg/opt/warriorforhire/pkg.cfg` | 9 | New package |
| `pkg/opt/warriorforhire/itemdesc.cfg` | 9, 10 | New - deed `0xa399` (moved here) and `0x7930` Companion's Second Wind |
| `pkg/opt/warriorforhire/include/wfhcommon.inc` | 9 | New - `AbandonWarrior()` |
| `pkg/opt/warriorforhire/include/wfhvitals.inc` | 9 | New - vital ceilings via `Custom*Level` CProps + `RecalcVitals()` |
| `scripts/ai/warrior.src` | 9 | New - migration shim for NPCs saved with `script warrior` |
| `config/npcdesc.cfg` | 9 | `warriorforhire` template script -> `:warriorforhire:warrior` |
| `config/itemdesc.cfg` | 9 | Deed `0xa399` removed (pointer comment left) |
| `scripts/misc/death.src` | 10, 17 | Heart: 11 lives, offline owner -> bank box (non-sleeping lookup), `Skills` on heart, `WFHBackup` on every death, `AbandonedByOwner` skip |
| `scripts/ai/highpriest.src` | 10 | "resurrect warrior" / "give up warrior" / "recover belongings"; heart revival restores skills and vitals, flat stat loss, sets `henchman`; forgiveness price clamped to 1000-60000 via new `PriestForgivenessFine()` (also used by the "relationship" reply) |
| `pkg/opt/warriorforhire/resetwfhdeaths.src` | 10 | New - Companion's Second Wind use script |
| `pkg/opt/warriorforhire/include/wfhescrow.inc` | 11 | New - escrow filing, registry, sweep, claim, destroy |
| `pkg/opt/warriorforhire/wfhescrowsave.src` | 11 | New - files a decayed corpse's bundled gear |
| `pkg/opt/warriorforhire/escrowsweep.src` | 11 | New - daily 90-day expiry |
| `pkg/opt/warriorforhire/start.src` | 11 | New - starts the escrow sweeper |
| `scripts/control/corpsedecay.src` | 11 | Bundles a warrior corpse's gear and hands off to `wfhescrowsave` |
| `pkg/systems/playervendor/commands/player/escrow.src` | 11 | Hides and refuses "Warrior for Hire" entries |
| `pkg/systems/combat/include/hitscriptinc.inc` | 12 | `WFHDamageMultiplier()` / `WFHModifyDamage()` at the top of `DealDamage()` |
| `scripts/control/skilladvancerequip.src` | 12 | 85 cap on necro/elemental protections for warriors |
| `scripts/control/skilladvancerunequip.src` | 12 | Same cap on the unequip recomputation |
| `pkg/opt/areas/callguards.src` | 12 | `crimMaster` reset per scanned mobile |
| `pkg/opt/warriorforhire/textcmd/test/setwfhdamage.src` | 12 | New - `.setwfhdamage` |
| `pkg/std/tracking/tracking.src` | 13 | Two classic menus -> one paged gump (ZH3.0 port); single classification pass; "Players" category kept; icon table removed; `unloadconfigfile("::npcdesc")` removed |
| `pkg/opt/alryc/include/speedwalk.inc` | 14 | New - `SendSpeedWalk()` packet helper (`0xBF`/`0x26`) |
| `scripts/textcmd/seer/speedwalk.src` | 14 | New - `.speedwalk` (Seer): toggle or set run-speed modifier 0-4, stored as `SpeedWalk` |
| `pkg/systems/accounts/logon.src` | 14 | Re-sends the stored `SpeedWalk` modifier for `cmdlevel >= 2` on login |
| `scripts/include/housetravel.inc` | 15, 17 | New - `FindHouseAtSpot()` (footprint rectangle via `.footprint` over a 34-tile scan, then the old height test, then static plates), `CanTravelIntoHouse()` (same-account rule for static signs only), `RemoveRunebookEntryAt()`, `DestroyRefusedRune()` |
| `pkg/systems/playervendor/include/escrow.inc` | 17 | `ESCROW_WFH_VENDOR_NAME` shared by the escrow writer and the `.escrow` filter |
| `pkg/std/spells/recall.src` | 15 | Origin and destination through the helper; refused rune destroyed; failed move reported; static-house blocks folded in |
| `pkg/std/spells/gate.src` | 15 | Same; destination static check now `GATE_TO` (was `GATE_FROM`) |
| `pkg/std/spells/mark.src` | 15 | Helper instead of `caster.multi`; static block folded in |
| `pkg/std/spells/teleport.src` | 15 | Destination inside a house footprint refused unless owner/friend; NPC casts skip the static scan |
| `pkg/opt/earth/earthportal.src` | 15 | Same as Gate; `#Casting` cleared on every early exit; "gate from to" message fixed |
| `pkg/std/runebook/customspells.inc` | 15 | `CustomRecall()`/`CustomGate()` through the helper; `TRAVEL_REFUSED_BY_HOUSE` (-1); failed move reported |
| `pkg/std/runebook/runebook.src` | 15 | Three travel callers handle `TRAVEL_REFUSED_BY_HOUSE` via new `RemoveRefusedEntry()` |
| `scripts/include/classes.inc` | 16 | Who-aware `GetClasseIdForSkill()`; explicit 0 from `IsSpecialisedIn()`; stat tables rewritten with `HighestHeldClasseLevel()`; `GetStatPointsMultiplier()` conflict rule; `GetPrimaryClasseId()` behind `GetClass()` and `IsProhibitedByClasse()`; sweep removed from `ClasseBonus()`, `ClasseBonusBySkillId()`, `IsFromThatClasse()`; `AssignClasse()` sweeps once |
| `scripts/include/skillpoints.inc` | 16 | Powerplayer small curve replaces the 1.1/1.2/1.3 table; stat advancements through new `ClassStatGainAmount()` |
| `pkg/opt/alryc/textcmd/test/classbonusinfo.src` | 16 | New - `.classbonusinfo` verification report |
| `patchnotes/developer-changelog-v1.1.4.md` | - | This file |
| `patchnotes/patch-v1.1.4.md` | - | Player-facing notes |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content |

Unchanged but relevant: `config/mrcspawn.cfg` (still sells deed `0xa399`, whose objtype did not change), `scripts/ai/combat/warriorcombatevent.inc` (only `helppcs.src` includes it; nothing in the warrior package does), `pkg/opt/powerhour/start.src` (still `start_script("powerhour")`), every `PHH`/`PHC`/`PHS`/`#PPH*` consumer listed in section 3.3, `scripts/misc/logoff.src` (still drops the personal flags on logoff), `scripts/misc/logon.src` / `reconnect.src` (still erase `#SettingPH`), and `pkg/opt/ArtifactSystem/artifactbox.src` (Eon-Prism decay registration from 1.1.2).

Every changed or new script compiles with `ecompile -w` at 0 errors, 0 warnings: the powerhour set, the whole warriorforhire package, the migration shim, every consumer edited in sections 10-12, and two hit scripts that include `hitscriptinc.inc`.

---

## 19. Risk and Regression Notes

- **Deploy with a server restart, not a hot reload (section 3).** The old scheduler is a long-lived process running the old `.ecl`. If the new files are compiled onto a live server, the old loop keeps running with its old logic and never writes `PH_SchedulerPID`; the first `.phadmin` would then find no scheduler, start a *second* one from the new code, and the two could each start powerhours. Either restart the server or kill the old `powerhour` process before using `.phadmin`.
- **A powerhour active at upgrade time gets a full extra hour (section 3.5).** `RecoverAfterRestart()` cannot know when a flag with no `PH_EndTime` was set, so it grants an hour from boot. That is one generous hour, once, and also what finally ends any flag that is currently stuck from the old bug.
- **Bonus odds now actually depend on the type (section 3.2).** Half-resources weeks move from an effective 23% to 50%; hunting and skill weeks from 23% to 10%. The long-run average is unchanged. Admin-started and bonus powerhours never roll.
- **The schedule is now data.** Once an admin saves a schedule, `PH_StartTime` is set and the compiled default is no longer consulted. There is no "reset to default" button; typing `0` / `19` / `0` restores the original Sunday 19:00 GMT. Changing the schedule to a moment that has just passed means the next powerhour is a week away, which the gump's countdown shows immediately.
- **`.phadmin`'s Apply broadcasts (section 4).** Changing the remaining time announces the new end to the whole shard. End now goes through the normal "has ended" broadcast. Starting one always broadcasts the 2-minute warning; there is no silent or instant start.
- **Gump layout was compiled, not eyeballed.** No client was run for this change. Coordinates follow the areas editor's conventions and the panels are sized to their content, but hues and positions may need a nudge after a first look in-game. Text entries use hue 900 on the 9200 panels.
- **Message changes players may notice (section 5).** `.setph`'s "Half-Resources"/"Double-Skillgain" became "Half-resources"/"Double skillgain"; `.ph` gained two lines and drops zero components from its countdown; the Eon-Prism and `.ph` now agree exactly on eligibility, including the Sunday case.
- **`.setph` no longer uses `#SettingPH` as a lock for the whole hour.** It is set only while the gump is open. The active-powerhour check at the top of the command is what prevents a second start during the hour, as it always effectively was; `logon.src`/`reconnect.src` erasing the property on login remains harmless.
- **Cross-package include (section 7).** `artifactsystem` now compiles against `:powerhour:include/powerhour`. Disabling the `powerhour` package would break `eonprism.src`'s compile; neither package has ever been disabled.
- **New global properties.** `PH_EndTime`, `PH_Source`, `PH_Pending`, `PH_Request`, `PH_SecondChance`, `PH_StartTime`, `PH_SchedulerPID` are new names in the global property store. Anything that enumerates global properties will see them.
- **Section 8 is a pending decision, not a fix.** The hunting-loot count condition in `starteqp.inc` is unchanged; if it is corrected later it needs its own patch-note line because it changes drop quantities.

Warrior for Hire (sections 9-12):

- **Restart required; the ten live warriors migrate themselves (section 9.1).** They keep their current stats and skills (`SetMeUp()` only runs on a `<random>` name), so an existing warrior stays the 130-skill veteran it was and simply gains the new behaviours. Its maximum HP changes only when the ceilings are first stamped, i.e. on its first hourly `GainStat()` tick while fighting with its owner online, at which point it jumps from Strength to 2 x Strength.
- **Already-mounted warriors stay mounted.** The dismount command now refuses and `RemoveIt()` never removes the mount layer, so a warrior that is riding today keeps the mount item until staff remove it.
- **Masterless warriors go inert.** A warrior recruited by talking under the old system (no `master`) has no code path left: it idles in the bored branch and reacts to nothing. If any exist they should be removed by staff.
- **New hires are weaker on day one (section 9.3).** 75/50/50 and 50 in each fighting skill instead of 130s is intended (ZH3.0 parity, offset by skill growth) but players used to the old hires will notice.
- **Guard break-off removed (9.2).** A guarding warrior now follows a fight wherever it goes instead of returning to the guarded target when more than 8 tiles from it.
- **Escrow shares the Merchant Escrow storage area and datafiles.** `.escrow` hides the entries, but any staff tool that enumerates that area will see "Warrior for Hire Escrow" roots. The daily sweeper only exists once `start.src` has run, i.e. after the restart.
- **Warriors now carry `CustomHitsLevel`.** `modsetup.inc` tops an NPC with that CProp up to full HP at AI start (harmless here). `pkg/std/housing/utility.inc`'s pet-fine code reads the same CProp in display units rather than hundredths; it only ever runs on confiscated pets, but the unit mismatch is worth knowing about if that CProp is set on anything else.
- **Damage multipliers default to 1.0.** No combat change until `.setwfhdamage` is used; the half-damage-to-own-master rule is always on.
- **Objtype `0x7930`, not ZH3.0's `0x4000BC`.** Any cross-shard tooling keyed on the Companion's Second Wind objtype must map it.
- **Gump layouts were compiled, not opened** (status gump, `.setwfhdamage`): same caveat as `.phadmin`.
- **High Priest forgiveness now costs classless players 1000 gold (10.4).** Previously a single coin repaired the relationship for anyone without a class, and the "relationship" command said as much. Players with a class level pay exactly what they did before.

Tracking (section 13):

- **No more icons in the tracking menu.** The gump lists names only; the per-category and per-creature tile icons of the old client menus are gone, as on ZH3.0.
- **Using Tracking no longer refreshes `npcdesc`.** Anyone who used the skill as a cheap way to pick up a `config/npcdesc.cfg` edit without a restart loses that side effect. It was never intended and cost a 1.15 MB re-read on every use.
- **Gump layout compiled, not opened here.** The layout is ZH3.0's verbatim and matches the screenshot it was requested from, but it was not opened in a client on this shard.

Speedwalk (section 14):

- **Staff only, by command level.** Players cannot reach the command, and the login restore fires only for `cmdlevel >= 2`. A staff member demoted below Seer keeps the saved `SpeedWalk` CProp but stops receiving the packet, which is the safe direction.
- **Client-side only.** The packet asks the client to move faster; nothing server-side is told about it. Any future server-side speed check must exempt characters carrying `SpeedWalk`.
- **Not tested in a client here.** The packet bytes are byte-for-byte ZH3.0's, where the command is confirmed working since 3.0.8.

Houses (section 15):

- **Refused runes are destroyed.** A rune or runebook entry pointing into a house its holder may not enter now crumbles / is removed on use, as ZH3.0 decided and as forbidden areas already do here. Players holding old runes to houses they lost access to lose them the first time they try; nothing goes without a message.
- **Courtyards and static houses now count.** Recall and Gate cast from a castle courtyard or inside a static house need the same access as from inside any house, and a visitor can no longer mark there.
- **Front steps.** The footprint extent is the multi's bounding rectangle, so a stranger's rune on a house's steps or porch is refused where the old height test may have let it through. Owners, co-owners and friends with the permission are unaffected.
- **Same-account access to static houses is now real.** The runebook path meant to allow it and never did (wrong property case). Alts of a static house's buyer can now recall and gate in; placed houses are unchanged.
- **NPCs no longer teleport into houses.** Monster Teleport casts fail the footprint test (no ownership), which is the safe direction.
- **Failed moves now say so.** A recall whose spot is blocked reports instead of ending silently after reagents and mana were spent; the outcome is unchanged, only visible now.
- **Not tested in a client here.** ZH3.0 confirmed the cause and the fix in game on 2026-10-01; this port compiles clean but was not walked through on this shard.

Classes (section 16):

- **Skill gain speeds up for six classes.** Bladesingers, Mystic Archers and Rangers now gain and succeed as a class on eight skills instead of four; Paladins and Warriors on eight instead of two. That is the intended design finally applied, but it is a real increase in training speed and check reliability on those skills from the moment the patch lands.
- **Powerplayers change shape.** Levels 1, 2 and 6 gain a bonus they never had (1.15, 1.3, 1.9); levels 3 to 5 go from 1.1 / 1.2 / 1.3 to 1.45 / 1.6 / 1.75. No Powerplayer ever had the second roll, so nothing is taken away.
- **Stats now drift by class.** Every classed character gains their affinity stat faster and their difficulty stat slower from today's skill use; nothing retroactive. Mages are the only class slowed on two stats. Stat caps are unchanged.
- **One live character changes class identity.** The Ranger 3 / Mystic Archer 1 is a Ranger to every rule now, item rules included: Strength gear the Mystic Archer rule stripped is legal for him again.
- **Mid-session illegal gear is caught within ten minutes, not instantly.** `pkg/opt/summoning/checkclasse.src`, started at boot, walks every online character every 600 seconds and reruns `AssignClasse()` for anyone holding a class, which sweeps. Before this patch the sweep also fired on the character's next hit or spell; now the ten-minute loop, login and the class commands are the only sweeps. (The hourly capper only enforces stat and skill caps; it never swept equipment.)
- **`IsFromThatClasse()` is a pure computation now.** Anything that relied on calling `IsWarrior()` and friends to strip gear as a side effect no longer gets that; `AssignClasse()` is the only sweep.
- **Not tested in a client here.** Compiles clean across all 138 includers; `.classbonusinfo` exists so the per-skill result can be checked on live characters before and after.
