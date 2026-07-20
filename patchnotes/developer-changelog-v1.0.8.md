# Developer Changelog - v1.0.8
**Range:** `9f8189f` (origin/Patch-1.0.7) -> `a3c99f6` (HEAD)  
**Branch:** Patch-1.0.8  
**Date:** 2026-06-20 -> 2026-07-20  
**Commits in range:** 3 (excluding merge commits)  
**Files changed:** 40 | +10208 / -1253

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Townstones - Player-Run Town Administration and Membership Cleanup](#3-townstones---player-run-town-administration-and-membership-cleanup)
4. [Areas - Policy Engine Rewrite and Castle Boundary Fixes](#4-areas---policy-engine-rewrite-and-castle-boundary-fixes)
5. [Go Locations - Indexed Config Generation and Range Fallback](#5-go-locations---indexed-config-generation-and-range-fallback)
6. [Spawnpoint - Restart Helpers and Region-Wide Reset Commands](#6-spawnpoint---restart-helpers-and-region-wide-reset-commands)
7. [Gameplay Tuning and Script Fixes](#7-gameplay-tuning-and-script-fixes)
8. [Support Data and Tooling](#8-support-data-and-tooling)
9. [Exhaustive File-by-File Change List](#9-exhaustive-file-by-file-change-list)
10. [Risk and Regression Notes](#10-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.0.8 is dominated by three big surfaces:

- a datafile-backed area policy rewrite with cache and invalidation support,
- a go-location indexing pass driven by a generated `golocs_by_id.cfg`,
- and a substantial player-run townstone/admin cleanup pass with better persistence and membership handling.

It also includes spawnpoint restart tooling, new staff test commands, and gameplay tuning in the remove-curse, protection, and skill-gain paths.

Large insertion volume comes mostly from generated and expanded data assets:

- `regions/regions.cfg` was expanded heavily to carry the metadata needed for the new go-location index.
- `config/golocs_by_id.cfg` was generated as the new indexed source used by `.go` and spawnpoint region selection.
- `pkg/opt/areas/include/areapolicy.inc` is a new policy resolver layer with cache and persistence helpers.

---

## 2. Commit Timeline

| Commit | Date | Message |
|--------|------|---------|
| `32a2b3a` | 2026-06-20 | backup script path restore |
| `dbfbe79` | 2026-07-20 | Areas fix for lord british castle and blackthornes |
| `a3c99f6` | 2026-07-20 | Bunch of patches for player run towns |

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

## 4. Areas - Policy Engine Rewrite and Castle Boundary Fixes

### Files Changed

| File | Change |
|------|--------|
| `pkg/opt/areas/include/areapolicy.inc` | New datafile-backed policy engine, parsed-line cache, global bypass masks, and prune and invalidate helpers |
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
  - a world-catchall bypass mask is OR-merged from known global ids,
  - parsed area lines are cached both per program and in a global property,
  - cache invalidation is fingerprinted by source line count.
- The admin `.areas` workflow now works as a single gump-driven editor for common policy flags such as guarded, no recall, no marking, anti-magic, forbidden, no looting, safe-area, no-PK, and RP-area behavior.
- `areas.cfg` includes the specific Lord British Castle and Lord Blackthornes Castle boundary fix from `dbfbe79`.
- The hot-path area checks now resolve through the new policy layer instead of repeatedly re-parsing config lines.

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
- Location ordering is normalized by type (`Jail`, `City`, `Shrine`, `Graveyard`, `Dungeon`, `POI`, `None`) before any leftover entries are appended.
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

## 7. Gameplay Tuning and Script Fixes

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

### Notable Functional Changes

- Protection effects now last much longer; the underlying strength is unchanged but the duration calculation was extended.
- Remove Curse is now more class-aware:
  - paladins use magery and magic resistance,
  - other casters still use the earlier item-identification-based path.
- Skill gain now uses a cleaner distinction between the skill used for the difficulty check and the skill used for gain calculations.
- The speech hook fix keeps staff speech logging working even when the packet contains Unicode speech data.
- Town NPCs no longer sit idle right after spawn; they start their wander cycle immediately.
- Crier setup became more defensive around missing NPC config entries.

---

## 8. Support Data and Tooling

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

## 9. Exhaustive File-by-File Change List

All files changed in `Patch-1.0.7..Patch-1.0.8`:

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
| `pkg/opt/areas/EnterAreaDelay.src` | Switched to the new area-policy resolution flow |
| `pkg/opt/areas/LeaveArea.src` | Switched to the new area-policy resolution flow |
| `pkg/opt/areas/areaban.src` | Updated for the new area-policy path |
| `pkg/opt/areas/areas.cfg` | Area policy and region data refresh, including castle fixes |
| `pkg/opt/areas/include/areapolicy.inc` | New policy resolver and cache layer |
| `pkg/opt/areas/include/areapolicy.inc.bak` | Backup of the pre-rewrite policy layer |
| `pkg/opt/areas/textcmd/admin/areas.src` | Rewritten area policy admin gump and flag editor |
| `pkg/opt/holybook/removecurse.src` | Revised Remove Curse chance logic |
| `pkg/opt/shilhook/shilhook.src` | Skill gain and difficulty refactor |
| `pkg/opt/spawnpoint/config/groups.cfg` | Spawnpoint group config update |
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | Shared spawnpoint restart helper |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpoint.src` | Restart command now uses the helper |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | New region-wide restart command |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointmax.src` | Force-fill restart command updated |
| `pkg/opt/townstones/textcmd/admin/createtownstone.src` | Creation workflow tightened and state restoration added |
| `pkg/opt/townstones/textcmd/admin/removetownmember.src` | New town member removal command |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | Major town status, member management, and runtime state expansion |
| `pkg/packethooks/speech/receivespeechhook.src` | Unicode speech packet decode fix |
| `pkg/std/treasuremap/treasure.cfg` | One treasure location adjusted |
| `pol.cfg` | Profiling and sysload watchers enabled |
| `pythonscripts/generate_golocs_by_id.py` | New generator for indexed go-location config |
| `regions/regions.cfg` | Large region metadata expansion and normalization |
| `scripts/ai/noble.src` | Nobles now wander immediately after spawn |
| `scripts/ai/person.src` | Townsfolk now wander immediately after spawn |
| `scripts/ai/setup/criersetup.inc` | Safer crier config handling |
| `scripts/ai/townperson.src` | Town NPCs now wander immediately after spawn |
| `scripts/include/anchors.inc` | Anchor and lookup helpers updated for new area behavior |
| `scripts/include/areas.inc` | Area helper rewrite to match the new policy layer |
| `scripts/include/townsfolk.inc` | Townfolk helper alignment update |
| `scripts/textcmd/coun/go.src` | `.go` rebuilt around indexed go locations and range fallback |

---

## 10. Risk and Regression Notes

1. `regions/regions.cfg` and `config/golocs_by_id.cfg` are now tightly coupled. Any future region edit should regenerate the go-location index rather than hand-editing the generated file.
2. The new area policy cache uses global properties and a line-count fingerprint. If `areas.cfg` changes shape, cache invalidation needs to remain intact or stale policy data can persist.
3. Town member removal now touches citizen lists, population, and election or poll state. That makes the new cleanup path more complete, but it also raises the importance of testing member removal against live stones and offline accounts.
4. `RestartSpawnPointWithMode(...)` can checkpoint repeatedly during max-fill mode. That is intentional, but it means the new region-wide restart command should still be treated as a staff maintenance tool rather than a routine hot command.
