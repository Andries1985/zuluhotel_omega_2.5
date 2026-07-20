# Patch Notes - v1.0.8
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: July 20, 2026**

---

Welcome to **Patch 1.0.8**. This update focuses on **player-run town administration**, **area-policy fixes**, **town NPC cleanup**, and a set of support-side script and balance updates.

---

## What Changed

## Townstones - Player-Run Town Management

### Player Impact

- Townstone status tools now surface more of the live town state, including treasury gold, population, upgrade and donation state, and player-town availability and purchase flags.
- Town member cleanup is more reliable, and town membership data now stays in sync when members are removed.
- Townstone creation and region conflict checks were tightened so townstone state is restored more consistently from the saved datafile.

---

## Areas - Castle Boundary Fixes

### Player Impact

- Area policy data was moved into per-realm datafiles, and the resolver now caches parsed area lines for lower server load.
- The area map was expanded and refreshed across all realms, including new shrines, dungeon entrances, and many more points of interest.
- Enter and leave text was updated alongside the area data, and the Lord British Castle and Lord Blackthornes Castle boundaries were corrected as part of that refresh.

---

## Holy Book - Remove Curse Update

### Player Impact

- Remove Curse now only accepts cursed weapons and armor as valid targets.
- Paladins now use a magery and magic resistance formula for the success chance instead of the older item-identification-only approach.
- The curse removal success and failure messaging is unchanged.

---

## Protection Effects - Longer Duration

### Player Impact

- Protection-style potion effects now last much longer than before.
- The effect strength is unchanged, but the duration was extended significantly.

---

## Skill Gain - High-Skill Tuning

### Player Impact

- Skill gain now correctly checks your base skill when worn gear pushes your effective skill to the cap, so skill-boosting items no longer block advancement.
- This fixes the edge case where capped effective skill could prevent gains even though your underlying skill still had room to improve.

---

## Town NPCs - Spawn Behavior Polish

### Player Impact

- Town NPCs and nobles now begin wandering immediately after spawn instead of waiting for their first movement cycle.
- This reduces the chance of freshly spawned town NPCs appearing idle.

---

## Staff and Support Tools

### Player Impact

- No direct gameplay change expected for regular players.
- Staff tools were updated for area policy management, go-location browsing, spawnpoint restarts, town member cleanup, and related command synopsis coverage.

---

## Maintenance and Data Updates

### Player Impact

- No direct gameplay change expected.
- Support data and scripts were refreshed for the new area and go-location workflow, and the backup script path was updated.

---

## Summary

- Townstone status and membership handling were expanded.
- Lord British Castle and Blackthornes area boundaries were corrected.
- Remove Curse now uses a better success formula and valid target filter.
- Protection effects now last longer.
- Skill gain handling was retuned for higher-skill edge cases.
- Town NPCs start wandering immediately after spawning.
- Staff support commands and data generators were refreshed.

Thanks for playing Zuluhotel Omega 2.5.