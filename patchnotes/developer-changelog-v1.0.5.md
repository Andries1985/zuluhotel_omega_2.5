# Developer Changelog - v1.0.5
**Range:** `1f06ebc` (origin/Patch-1.0.4) -> `e774ee3` (HEAD)  
**Branch:** Patch-1.0.5  
**Date:** 2026-05-29 -> 2026-06-03  
**Commits in range:** 5 (excluding merge commits)  
**Files changed:** 6 | +523 / -35

---

## Table of Contents

1. [Talisman ID Flow Reliability](#1-talisman-id-flow-reliability)
2. [Tamed AI and Master-Death Behavior](#2-tamed-ai-and-master-death-behavior)
3. [Animal Trainer and Stablemaster Safety Fixes](#3-animal-trainer-and-stablemaster-safety-fixes)
4. [Patchnotes/Launcher Maintenance Commits](#4-patchnoteslauncher-maintenance-commits)
5. [Commit Timeline](#5-commit-timeline)

---

## 1. Talisman ID Flow Reliability

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/talisman/include/talismanid.inc` | Talisman ID now returns success/failure correctly and emits clearer user feedback |
| `pkg/opt/talisman/talisman/use.src` | `#LastID` and talisman revision updates now only apply on successful ID use |

### Overview

This update makes talisman-based identification stateful and accurate, preventing cooldown/charge consumption behavior when identification does not actually happen.

### Notable Functional Changes

- `TalismanID(...)` now returns `0` on cancel/invalid/reserved/already-identified/no-op paths and returns real `identified` result on attempt paths.
- Added user feedback messages for key no-op/failure paths:
  - canceled targeting,
  - item currently reserved,
  - failed identification.
- Container ID path now returns whether at least one item was identified.
- `ItemID(...)` now returns the computed `identified` value (instead of always returning success).
- In `dotalismanid(...)`, both talisman revision increment and `#LastID` timestamp update are gated behind successful use.

### Expected Impact

- No false cooldown stamp (`#LastID`) when no valid identification occurs.
- No unnecessary talisman revision/charge progression on canceled/invalid actions.
- Better player-facing clarity on why an ID action did not proceed.

---

## 2. Tamed AI and Master-Death Behavior

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/tamed.src` | Added master-death state handling, safety guards, and follow speech fix |

### Overview

Tamed AI now transitions more predictably when the master dies and recovers cleanly after master revival, while avoiding speech-command edge-case failures.

### Notable Functional Changes

- Added `masterwasdead` state tracking.
- On master death:
  - pet enters all-stop style behavior,
  - autonomous self-defense stays enabled,
  - follow/guard state is cleared,
  - queued attack targets are cleared.
- On master revive:
  - same reset path is applied once to avoid stale pursuit/guarding state.
- Added `master` null guard before `master.npctemplate` checks.
- Fixed `all follow` dragonspeak handling so it speaks the correct follow target name in both "follow me" and targeted follow variants.
- Removed stray trailing semicolons in movement conditionals for cleaner control flow.

### Expected Impact

- Reduced pet desync/odd pursuit after master death.
- Cleaner re-entry into command-driven behavior after revive.
- Fewer follow-command messaging inconsistencies.

---

## 3. Animal Trainer and Stablemaster Safety Fixes

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/animaltrainer.src` | Added spawn/null checks, corrected return-item container calls, and target cancel guard |

### Overview

Trainer and stablemaster flows now fail more safely and return items correctly in edge cases.

### Notable Functional Changes

- Added null guard after `CreateNPCfromtemplate(...)` when trainer creates a pet from deed/item flow.
- Added null guard after ticket-based `CreateNPCFromTemplate(...)` in both ticket restore paths:
  - if pet creation fails, ticket is returned to player backpack and process exits safely.
- Corrected `MoveItemToContainer` argument order in multiple trainer return-gold/item paths.
- Added cancel guard in `stable(...)` when player cancels targeting a pet.
- Removed an accidental trailing semicolon in gold-check conditional.

### Expected Impact

- Lower risk of item/ticket loss on failed pet recreation.
- More consistent return behavior when trainer cannot process payment/training item.
- Fewer script-edge failures from null target/pet objects.

---

## 4. Patchnotes/Launcher Maintenance Commits

### Files Changed
| File | Change |
|------|--------|
| `patchnotes/launchernotes.md` | Added then corrected launcher-facing notes content |
| `patchnotes/patch-v1.0.0.md` | Updated during launcher-note prep commit sequence |

### Overview

Two documentation-only commits were included in range (`24935cc`, `de8f1f6`) to establish and then adjust launcher notes content.

---

## 5. Commit Timeline

1. `52a30ac` - Talisman Use Update  
2. `24935cc` - Launcher Notes  
3. `de8f1f6` - Launcher notes fix  
4. `cbfaeec` - Tamed Fixes Update  
5. `e774ee3` - Tamed fixes on master death

---

## Summary of Changes

Patch 1.0.5 is a behavior-correction and reliability patch focused on:

- Talisman ID success/failure correctness and cooldown/charge gating
- Tamed AI control-state resets during master death/revive transitions
- Trainer/stable safety improvements around pet creation and item return flows
- Launcher note/documentation maintenance commits

**Total Impact:**
- 6 files changed
- 523 lines added
- 35 lines removed
- 5 non-merge commits