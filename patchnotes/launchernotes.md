# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## Townstones - Player-Run Town Management

### Player Impact

- Townstone status tools now surface more of the live town state, including treasury gold, population, upgrade and donation state, and player-town availability and purchase flags.
- Town member cleanup is more reliable, and town membership data now stays in sync when members are removed.
- Townstone creation and region conflict checks were tightened so townstone state is restored more consistently from the saved datafile.

---

## Name Changes - Exploit Closed and Fixes

### Player Impact

- Closed an exploit where a player could end up holding both a plain name (like "Pig") and a town-suffixed version of it ("Pig of Trinsic") at the same time by juggling alt characters while joining and leaving a town.
- Name matching is now case-insensitive - "Bop", "bop", and "BOP" are treated as the same name and can no longer all exist at once.
- New characters, and anyone using the rename gump, can no longer pick a name that contains a town name.
- Fixed a bug that let a name end up with a leading/trailing/double space, which could silently create duplicate-looking character names.
- Fixed the cause of a reported ~50 second delay and stacked rename gumps when changing your name.
- Leaving a town with a name that's already taken by someone else no longer traps you - you'll be prompted to pick a new name instead.
- Staff renaming tools now check names the same way player renames do, so a player can no longer end up with an invalid or duplicate name through a staff tool.

---

## Housing - Placement Restrictions

### Player Impact

- House placement now checks your entire house footprint (not just where you're standing) against city, dungeon, shrine, and graveyard zones, so houses can no longer be placed overlapping those areas.

---

## Areas - Castle Boundary Fixes and Performance

### Player Impact

- Area policy data was moved into per-realm datafiles, and the resolver now caches parsed area lines and policy flags for lower server load.
- The area map was expanded and refreshed across all realms, including new shrines, dungeon entrances, and many more points of interest.
- Enter and leave text was updated alongside the area data, and the Lord British Castle and Lord Blackthornes Castle boundaries were corrected as part of that refresh.

---

## Crafting - Omega Cache Fixes

### Player Impact

- The smithy hammer can now correctly pull ingots (and bone, for bone armor) directly from your Omega Cache - this was supposed to work before but was broken.
- The smithy retort (used to craft crafting-power upgrades) can now also pull materials from your Omega Cache if you run out in your backpack.
- Fixed several leveled Strength and Cure Potions (and their Greater versions) showing up under "Other" in the Omega Cache instead of "Potions."

---

## Holy Book - Remove Curse Update

### Player Impact

- Remove Curse now only accepts cursed weapons and armor as valid targets.
- Paladins now use a magery and magic resistance formula for the success chance instead of the older item-identification-only approach.

---

## Protection Effects - Longer Duration

### Player Impact

- Protection-style potion effects now last much longer than before.
- The effect strength is unchanged, but the duration was extended significantly.

---

## Skill Gain - High-Skill Tuning

### Player Impact

- Skill gain now correctly checks your base skill when worn gear pushes your effective skill to the cap, so skill-boosting items no longer block advancement.

---

## Town NPCs - Spawn Behavior Polish

### Player Impact

- Town NPCs and nobles now begin wandering immediately after spawn instead of waiting for their first movement cycle.

---

## Performance

### Player Impact

- Fixed two commands that were heavy enough to occasionally slow the server - no player-facing behavior change, just faster and lighter on the server.

---

## Staff and Support Tools

### Player Impact

- No direct gameplay change expected for regular players.
- Staff tools were updated for area policy management, go-location browsing, spawnpoint restarts, town member cleanup, and related command coverage.

---

## Summary

- Townstone status and membership handling were expanded.
- A town-suffix name-collision exploit was closed, and several related naming bugs were fixed.
- House placement now respects city, dungeon, shrine, and graveyard boundaries.
- Lord British Castle and Blackthornes area boundaries were corrected.
- The smithy hammer and smithy retort now correctly use the Omega Cache.
- A handful of leveled potions were moved into the correct Omega Cache category.
- Remove Curse now uses a better success formula and valid target filter.
- Protection effects now last longer.
- Skill gain handling was retuned for higher-skill edge cases.
- Town NPCs start wandering immediately after spawning.
- Two hot commands were made significantly faster.
- Staff support commands and data generators were refreshed.

Thanks for playing Zuluhotel Omega 2.5.
