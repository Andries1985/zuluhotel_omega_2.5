# Developer Changelog - v1.1.6
**Range:** `b640f5c` (merge of #353, the last 1.1.5 patchnotes state) -> working tree (this commit)
**Branch:** Patch-1.1.6 (branched from `origin/Test-Shard` at `b640f5c`)
**Date:** 2026-11-03 to 2026-11-23 (harvest tile audit 2026-11-03; tile lists picked and code written 2026-11-04; regen, karma/fame, pets, charm spells and review 2026-11-07; login limit hotfix 2026-11-08; areas package cleanup and live review fixes 2026-11-22; weapon elemental damage order and poison charges 2026-11-23)
**Commits in range:** 6 code commits on this branch (`8d85c3e` harvest, `1cf11dc` regen and the rest, `a7c3dfc` tree items, `af43cfb` the areas cleanup and live review fixes in sections 16-17, `89fa19e` `.pets`, and the combat fixes in section 18), plus the login limit hotfix `3f9d67b`, made on Patch-1.1.5 for live and merged into Test-Shard by PR #355 (section 15).
**Files changed:** harvest: 13 scripts and region configs, 1 new include (`harvesttiles.inc`); `1cf11dc`: 16 scripts and configs, 1 new include (`charm.inc`); `a7c3dfc`: 3 harvest scripts (section 7.7); `3f9d67b`: 3 account scripts; areas cleanup: 5 files in `pkg/opt/areas`, 1 deleted, plus `.gitignore` (section 16); live review fixes: 17 scripts and configs, 3 new commands (section 17); combat: 2 scripts (section 18); plus this release's 3 `patchnotes/` files. The full list is section 19.

> Scope was computed against `b640f5c`. `origin/Test-Shard` was still at `b640f5c` when this commit was made, so the whole delta is the two commits.
> Built with `ecompile -A` on 2026-11-22 with everything through section 17: 1,165 scripts, 0 errors (`.pets`, 17.6, built on its own afterwards: 0 errors, 0 warnings) (the `.src` files not built are in the disabled packages `astralfights`, `convertworld` and `timer`). The login hotfix (section 15) was also built and running on live since 11/08. Section 18 built with `ecompile -A` on 2026-11-23: 1,166 scripts, 0 errors. Not yet run in a client (section 20).

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [The Defect - Spoofed Target Graphics](#3-the-defect---spoofed-target-graphics)
4. [`harvesttiles.inc` - One Place for the Tile Lists](#4-harvesttilesinc---one-place-for-the-tile-lists)
5. [The Tile Lists](#5-the-tile-lists)
6. [Region Configs](#6-region-configs)
7. [Per-Script Changes](#7-per-script-changes) (7.7: tree items)
8. [Fishing - Magic Fish Above 129 Skill](#8-fishing---magic-fish-above-129-skill)
9. [Regeneration](#9-regeneration)
10. [Pets - Special Spell Mana and Cast Cooldown](#10-pets---special-spell-mana-and-cast-cooldown)
11. [Karma and Fame from the Template](#11-karma-and-fame-from-the-template)
12. [Control Undead and Spell Bind - `charm.inc`](#12-control-undead-and-spell-bind---charminc)
13. [Review of the Branch](#13-review-of-the-branch)
14. [`npcdesc.cfg` Rewrite](#14-npcdesccfg-rewrite)
15. [Login Limit per IP - No More In-World Kicks](#15-login-limit-per-ip---no-more-in-world-kicks)
16. [Areas Package Cleanup](#16-areas-package-cleanup)
17. [Live Review Fixes (2026-11-22)](#17-live-review-fixes-2026-11-22)
18. [Combat - Weapon Elemental Damage Order and Poison Charges](#18-combat---weapon-elemental-damage-order-and-poison-charges)
19. [Exhaustive File-by-File Change List](#19-exhaustive-file-by-file-change-list)
20. [Risk and Regression Notes](#20-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.6 is one theme: gathering skills stop trusting the client.

`TargetCoordinates()` returns whatever graphic the client says was clicked (`use_on.objtype`) without the core checking it. Every gathering script tested that graphic against a hard-coded tile list. A macro or a modified client could send a tree graphic for a grass square and get wood, send a water graphic anywhere and fish, and so on. The audit (2026-11-03) found the same pattern in lumberjacking, `bladed.src` kindling, mining, clay, sand, fishing, the fishing net, `fillpitcher.src`, `cleanbandages.src` and `seed.src`.

The fix reads the real statics (`ListStaticsAtLocation`) and land tile (`GetMapInfo`) at x,y in the caster's realm, and confirms the tile with the engine's own `GetHarvestDifficulty()`, which only accepts tiles listed in the matching `regions/*.cfg` that are really at that spot. Section 3 has the detail.

While doing that, every tile list was rebuilt tile by tile from the client art (`art.mul`) in the Harvest Tile Picker artifact and reviewed by Alryc (2026-11-04). The script predicates and the region configs had drifted apart: `wood.cfg` listed grasses and ferns, `fish.cfg` listed sand, `clay.cfg` listed cave and void tiles, and `ore.cfg` listed cave floor statics as land tiles. They now match exactly (section 5).

Section 8 is a small fishing fix found during review: `RandomFish()` left the fish type unset for skill rolls of 130 and above.

The second commit is a separate theme, built on the same branch:

- **Regeneration (section 9).** Poison and temporary mod expiry set HP regen to 100 hundredths a minute (1 HP/min) for good; unequipping an item subtracted regen that equipping never added; NPC template regen was never applied.
- **Pets (section 10).** Tamed creatures cast their free special spells without mana and, as casters, on every AI loop.
- **Karma and fame (section 11).** Kill values come from new `npcdesc.cfg` fields instead of a stats formula whose alignment typo made every NPC evil.
- **Control Undead and Spell Bind (section 12).** The freed creature kept the caster as its core master (so its attacks were the caster's in the repsys), and a reboot, a recast or a town release left a permanent pet.
- **Review (section 13)** of the whole branch, and the `npcdesc.cfg` rewrite that carries sections 9 and 11 (section 14).

A live hotfix rides along (section 15): kicking the third account on an IP after it entered the world stopped the server accepting new logins for about 150 s per kick, and on 2026-11-08 one player's auto-reconnecting clients kept that going for about three hours. Over-limit logins are now refused at the login server.

Section 16 is the parked cleanup list from the 2026-10-08 areas package review (the bug and performance items from that review shipped in 1.1.4, section 18 of its changelog). It fixes the inverted Wrong Level 3 box, so that level gets its no-recall/no-mark flags, and makes the world-wide flags work on every facet.

Section 17 is the round of fixes from the 2026-11-22 live log and save review. Section 18 (2026-11-23) comes from a player report: the Bow of the Air Element and the War Fork of the Water Element dropped the wielder out of war mode on a kill. Weapon elemental damage was dealt in the middle of the damage calculation, before the No-PK check, so the kill landed before the check and the check misread the destroyed NPC as a player. Elemental damage now lands after the checks. A poisoned weapon also lost one swing's damage when its charges ran out.

## 2. Commit Timeline

| Commit | Date | Subject | Sections |
|---|---|---|---|
| `8d85c3e` | 2026-11-07 | Harvest: check real tiles instead of client graphics, rebuilt tile lists, patch notes | 3-8 |
| `1cf11dc` | 2026-11-07 | Regen, pet mana, karma/fame from npcdesc, Control Undead / Spell Bind charm state, review fixes, patch notes | 9-14 |
| `3f9d67b` | 2026-11-08 | Refuse over-limit IP logins at the login server instead of kicking in world (on Patch-1.1.5; PR #355 into Test-Shard, PR #356 into Live-Shard) | 15 |
| `ed5d187` | 2026-11-08 | Patch notes for the login limit hotfix | - |
| `a7c3dfc` | 2026-11-08 | Lumberjacking and kindling: tree items on the ground give wood again (PR #358) | 7.7 |
| `ab229bd` | 2026-11-08 | Patch notes for the tree item fix (PR #359) | - |
| `af43cfb` | 2026-11-22 | Escrow restart fix and recovery, city wage, areas cleanup, live review fixes, patch notes (PR #359) | 16, 17 |
| `89fa19e` | 2026-11-22 | `.pets`, patch notes (PR #360) | 17.6 |
| *(this commit)* | 2026-11-23 | Weapon elemental damage applied after the No-PK checks (war mode drop), poison charge fix, patch notes | 18 |

---

## 3. The Defect - Spoofed Target Graphics

### 3.1 What the scripts trusted

- `lumberjack.src`: `IsTree(use_on.objtype)`. A target with no objtype (ground) fell back to a z-1/z+1/z-2/z+2 static search, but a target *with* an objtype was taken at its word.
- `bladed.src`: the same pattern for kindling.
- `mining.src`: `GetMapInfo(use_on.x, use_on.y)` without a realm (always Britannia's map), then `IsMinable(info.landtile, use_on.objtype)`; the cave floor statics `0x53B-0x553` were accepted from `use_on.objtype`.
- `fishing.src` / `fishing.inc`: `check_for_water(use_on)` tested `use_on.objtype` against water statics, then `GetHarvestDifficulty("fish", x, y, use_on.objtype)`.
- `fishingnet.src`: `IsWater(use_on)`, which accepted `use_on.objtype` when `use_on.item` was set and otherwise read Britannia's land tile.
- `fillpitcher.src`, `cleanbandages.src`: their own `check_for_water()` copies, testing `theobj.objtype` and `theobj.item.objtype`.
- `seed.src`: fertile dirt `0x31F4-0x31FB` accepted from `theobj.objtype`; land read without a realm.

### 3.2 Why `GetHarvestDifficulty` alone was not enough

`GetHarvestDifficulty(resource, x, y, tiletype, realm)` already rejects a tile that is not at x,y. The scripts never relied on it: they gated on their own predicate first and some called the engine with the land tile only (mining clay and sand) or without a realm. Fishing did call it with `use_on.objtype`, but treated a 0 roll as "no fish here".

### 3.3 The fix

Each script asks `FindHarvestTile(resource, x, y, realm)` (section 4), which only returns a tile that is both in the script-side list and accepted by the engine at that spot. The client's objtype is never read. Water for pitchers and bandages has no region config, so `IsWater(where, realm)` reads the real statics and land directly, and accepts a placed item only by its own `graphic` and only when it is not in a container. Wood does the same for tree items (planted and decoration trees) through `IsTreeItem()` (section 7.7).

---

## 4. `harvesttiles.inc` - One Place for the Tile Lists

New file `scripts/include/harvesttiles.inc`, included by `scripts/include/itemutil.inc` (so every existing `itemutil` user gets it) and directly by `fillpitcher.src`.

| Function | Purpose |
|---|---|
| `FindHarvestTile(resource, x, y, realm := 0)` | `"wood"`, `"ore"`, `"sand"`, `"clay"` or `"fish"`. Checks statics at x,y (`LIST_IGNORE_Z`, `ITEMS_IGNORE_MULTIS`) first, then the land tile. Returns the matching tile id, or 0. Realm defaults to `_DEFAULT_REALM`. |
| `HarvestTileAt(resource, x, y, tile, realm)` | Calls `GetHarvestDifficulty()` and tests `.errortext`, not the value: a difficulty roll of 0 is valid. `"Resource is depleted"` still counts as present, so the skill can say "none left" instead of "can't use that". |
| `HarvestTileListed(resource, tile, is_land)` | Dispatches to the list predicates. Wood is statics only; sand and clay are land only; ore and fish are both. |
| `IsTreeItem(use_on, realm := 0)` | Lumberjacking and kindling (`a7c3dfc`, section 7.7). The targeted `use_on.item` is on the ground (no container), in `realm`, and its own `graphic` passes `IsTree`. The server resolves the item, so the graphic is trusted. |
| `IsWater(where, realm := 0)` | Pitchers and bandages. A placed item not in a container whose `graphic` is a water static; a real water static at x,y; or water land at x,y. Replaces the old `itemutil.inc` `IsWater(theobj)`. |
| `IsTree`, `IsOreLand`, `IsOreStatic`, `IsSand`, `IsSwamp`, `IsFishLand`, `IsFishStatic`, `IsWaterLand`, `IsWaterStatic` | The tile lists (section 5). Each takes a tile id. |

The header comment maps each predicate to its region config and states that the two must agree.

Removed from `itemutil.inc`: `IsMinable(theobj, othertype)`, `IsSand(theobj)`, `IsSwamp(theobjtype)`, `IsTree(theobj)` and `IsWater(theobj)`. `IsSand`, `IsSwamp` and `IsTree` keep their names and one-argument shape in the new include; `IsMinable` has no callers left; `IsWater` gains an optional realm.

---

## 5. The Tile Lists

Picked in the Harvest Tile Picker artifact (db collection `picks`) on 2026-11-04. Claude proposed each list from the tile names and flags in the client data; Alryc accepted or changed it tile by tile.

### 5.1 Wood (`IsTree`, `wood.cfg` ObjType) - 197 statics

`0x0C95-0x0C96, 0x0C99-0x0C9E, 0x0CA8, 0x0CAA-0x0CAB, 0x0CC8-0x0CEA, 0x0CF3-0x0D03, 0x0D41-0x0DAB, 0x12B6-0x12C7, 0x1B1F-0x1B26, 0x1B7E`

- Added: coconut and date palms, yew `0x12B6-0x12C7`, leaves `0x1B1F-0x1B26`, Tuscany pine `0x1B7E`.
- Removed: the plants inside the old `0x0C99-0x0CEA` range (`0x0C9F-0x0CA7`, `0x0CA9`, `0x0CAC-0x0CC7`: grasses, ferns, rushes, weed, cattails, pampas, snake plant, ponytail palm, flowers).
- Yucca `0x0D37-0x0D38` stays out. It was in the old `IsTree` and refused by hand-coded checks in `lumberjack.src` and `bladed.src`; now it is simply not listed. Alryc reversed an early pick to keep it out.
- Rejected candidates: palm fronds `0x0587-0x0594`, `0x2183`; leaves `0x30C8-0x30D3`; the "tree" range `0x39A9-0x3AEE` (1,188 placed). Juniper bush `0x0CC8` is kept.

### 5.2 Ore (`IsOreLand` / `IsOreStatic`, `ore.cfg` LandTile / ObjType)

- Land (186): `0x00DE-0x00DF, 0x00E2-0x00E7, 0x00F4-0x00F7, 0x0104-0x0107, 0x0110-0x0113, 0x0122-0x0125, 0x01D3-0x01DA, 0x021F-0x0230, 0x0232-0x023B, 0x0245-0x026D, 0x02BC-0x02CB, 0x063B-0x063E, 0x06CD-0x06D1, 0x06DA-0x06DD, 0x06EB-0x06FE, 0x070D-0x0714, 0x071D-0x0720, 0x072B-0x0732, 0x073B-0x073E, 0x0749-0x0750, 0x0759-0x075C`
- Statics (24, cave floor): `0x053B-0x054F, 0x0551-0x0553`
- Added: cave `0x024A-0x026D` (`0x0250` moves here from clay), `0x02BC-0x02CB`, `0x063B-0x063E`, dirt `0x00DE-0x00DF`.
- Removed: forest `0x00F3` (the Occlo bank tile) and dirt `0x0075`.
- `ore.cfg` used to list `0x53B-0x53F` as `LandTile`; they are statics and are now `ObjType`.

### 5.3 Sand (`IsSand`, `sand.cfg` LandTile) - 157 land tiles

`0x0016-0x003A, 0x0044-0x004B, 0x011E-0x0121, 0x0126-0x012D, 0x0192, 0x01A8-0x01AB, 0x01B9-0x01D1, 0x0282-0x0285, 0x028A-0x0291, 0x0335-0x033C, 0x0342-0x0344, 0x034D, 0x034F-0x0354, 0x0359-0x035C, 0x03B7-0x03BE, 0x03C7-0x03CA, 0x064B-0x0652, 0x0657-0x065A, 0x0663-0x066A, 0x066F-0x0672`

Claude's suggestions accepted unchanged. The old ranges `0x0016-0x004B`, `0x0192-0x01AB`, `0x03B7-0x03CA` and `0x064B-0x0672` included non-sand transition tiles; the `0x0335-0x035C` sand tiles were missing.

### 5.4 Clay (`IsSwamp`, `clay.cfg` LandTile) - 396 land tiles

`0x3D65-0x3EF0` only.

- Removed: grass `0x0240-0x0243`, the void tile `0x0244`, cave `0x0250` (to ore), and `clay.cfg`'s cave tiles `0x024A-0x024F`.
- Dropping `0x0244` is a real fix: it lies under about 4.2 million dungeon cells, and because the old `mining.src` tested swamp before ore, mining a cave floor static over void land dug clay.

### 5.5 Fish (`IsFishLand` / `IsFishStatic`, `fish.cfg` LandTile / ObjType)

- Land (38): `0x004D-0x004F, 0x0051-0x0052, 0x0055-0x006C, 0x006E-0x006F, 0x00A8-0x00AB, 0x0136-0x0137, 0x3FF0`
- Statics (115): `0x1796-0x179C, 0x1FA4-0x1FAC, 0x1FAE-0x1FBB, 0x1FBD-0x1FC0, 0x1FC2-0x1FC5, 0x1FC7-0x1FCA, 0x2453-0x245D, 0x346E-0x3485, 0x3494-0x3498, 0x349A-0x34A4, 0x34A6-0x34AB, 0x34B8-0x34BB, 0x34BD-0x34C0, 0x34C2-0x34C5, 0x34C7-0x34CA`
- Alryc's changes: NoName land `0x4C`, `0x50`, `0x53`, `0x54`, `0x6D`, shoreline water statics `0x179D-0x17B2` and waves `0x1FA3`, `0x1FAD`, `0x1FBC`, `0x1FC1`, `0x1FC6` are not fishable. About 165,000 cells that were fishable become unfishable, mostly shore strips. `FISHING_RANGE` is 6, so players still reach open water from the shore.
- Also out: waterfalls, `0x1559`, fountain ripples, troughs, tubs, barrels, buckets, fountains, and `fish.cfg`'s old sand entries.

### 5.6 Water (`IsWaterLand` / `IsWaterStatic`, no config)

- Land (43): `0x004C-0x006F, 0x00A8-0x00AB, 0x0136-0x0137, 0x3FF0`
- Statics: the fish statics plus waterfalls, `0x1559`, the `0x179D-0x17B2` shoreline pieces, the waves left out of fishing, fountain pieces (`0x090D-0x0910`, `0x1731-0x175A`, `0x19C3-0x19EC`), troughs `0x0B41-0x0B44`, tub `0x0E7B`, bucket `0x0FFA`, barrel `0x154D`, and the `0x34D1-0x3526` water pieces.
- Alryc's changes: waterfalls, `0x1559` and `0x3FF0` count as water. Troughs, tubs, barrels, buckets and fountains were added as static candidates later and left at Claude's "water" suggestion. Before, they only counted as placed items.
- The old `IsWater` also accepted land `0x3FF1-0x3FF3`; only `0x3FF0` was picked.

---

## 6. Region Configs

| File | Change |
|---|---|
| `regions/wood.cfg` | Global `ObjType` list replaced with section 5.1 (it had duplicate entries for `0x0C9E`, `0x0CA8`, `0x0CAA`, `0x0CAB` and listed the plants). Range `0 0 5119 4095` -> `0 0 7167 4095`. "Keep in step" comment. |
| `regions/ore.cfg` | Global replaced with section 5.2: land as `LandTile`, cave floor as `ObjType`. Regions unchanged. |
| `regions/sand.cfg` | Global `LandTile` list replaced with section 5.3. Range extended to `0 0 7167 4095`. |
| `regions/clay.cfg` | Global `LandTile` list replaced with section 5.4. Range extended to `0 0 7167 4095`. |
| `regions/fish.cfg` | Global replaced with section 5.5 (`LandTile` + `ObjType`). |

The range extensions are cosmetic. In pol-core, a resource zone that no `Range` covers falls back to the *first* region in the file (`regions_[0]`). Wood, sand and clay's first region is "The Whole World", so the dungeon map past x 5120 (Occlo Mine included) was already harvestable; the new range only states it.

---

## 7. Per-Script Changes

### 7.1 `lumberjack.src`

- Yucca check replaced by `IsTreeItem(use_on, character.realm)` or `FindHarvestTile("wood", use_on.x, use_on.y, character.realm)` before the loop (the `IsTreeItem` half was added in `a7c3dfc`, section 7.7); failure: "You can't get any wood from that."
- The `IsTree(use_on.objtype)` branch and the z-search fallback (four `ListStaticsAtLocation` calls at z+1, z-1, z-2, z+2) are gone. `FindHarvestTile` ignores z, so clicking the ground under a tree still finds it.
- The unused `GetMapInfo()` in the loop and `ChopTree()`'s `info` parameter are removed.

### 7.2 `bladed.src`

- The tree branch is `IsTreeItem(use_on, user.realm) or (!use_on.item and !use_on.mobile and FindHarvestTile("wood", ...))`. A tree item on the ground gives kindling (`a7c3dfc`, section 7.7); any other item, and creatures, stay out of the map-tree check.
- The z-search fallback and `CarveTree()`'s yucca check are removed.

### 7.3 `mining.src`

- Before the loop: `FindHarvestTile` for `"ore"`, then `"clay"`, then `"sand"`. Ore first because cave floor statics sit on void or swamp-numbered land. Failure: "You can't mine or dig anything there."
- In the loop, a `case` on the resource found. `DigForClay()` / `DigForSand()` take the found tile and pass `character.realm` to `GetHarvestDifficulty()`. `MineForOre()` drops its `unused info` parameter.

### 7.4 Fishing

- `fishing.inc` `check_for_water(theobj, realm := 0)` is now `FindHarvestTile("fish", x, y, realm)`.
- `fishing.src` calls it after the cancel check (the old code called it before) with `character.realm`. The second `GetHarvestDifficulty()` block, which tested `use_on.objtype` and then Britannia's land tile and treated a roll of 0 as "can't get fish", is removed: the gate already confirmed the tile.
- The "There's not enough fish here." check now tests `HarvestResource()`'s result before `Random(fish_amount)+1`. The old check ran after it, when the amount could never be 0.
- `fishingnet.src` uses `check_for_water(use_on, character.realm)` instead of `IsWater(use_on)`, so nets follow the fishable list rather than the pitcher list.

### 7.5 `fillpitcher.src`, `cleanbandages.src`

Each local `check_for_water(character, theobj)` becomes `return IsWater(theobj, character.realm)`. `fillpitcher.src` includes `include/harvesttiles` directly.

### 7.6 `seed.src`

- Returns at once if the target is cancelled.
- `is_plantable(theobj, realm)`: fertile dirt `0x31F4-0x31FB` is accepted from a placed item (not in a container, by its `graphic`) or a real static at x,y; the land tile is read in the caster's realm.

### 7.7 Tree items (`a7c3dfc`, PR #358)

- **Found in testing:** a planted banana tree gave "You can't get any wood from that." `FindHarvestTile` only reads map statics and the land tile, so every tree that is an *item* was refused. That means Botanik trees grown from tree seeds (`pkg/opt/botanik/maketree.src`, carrying a `PlacedBy` CProp) and decoration trees.
- **Before 1.1.6** these gave wood: `IsTree(use_on.objtype)` matched the item, and `HarvestResource("wood", x, y, ...)` takes from the region pool without checking for a tile. The live save of 2026-11-08 has 2,891 tree items on the ground (trunks and leaves): 2,465 planted by players and 426 with no `PlacedBy`.
- **Fix:** `IsTreeItem(use_on, realm)` in `harvesttiles.inc` (section 4). `lumberjack.src` accepts `IsTreeItem` or `FindHarvestTile("wood")`; `bladed.src` sends a tree item to `CarveTree()` and keeps the map check for non-item, non-mobile targets.
- **Still safe from spoofing:** the client sends the item's serial and the server looks up the item, so its `graphic` is the server's. A macro claiming a tree graphic on bare ground still reaches only `FindHarvestTile`.
- **Unchanged:** wood comes from the region pool, as before 1.1.6 and as for map trees, so a tree item doesn't run dry by itself. The 2-tile distance check (and line of sight for kindling) still applies.
- Built with `ecompile -A`: 1,163 scripts, 0 errors.

---

## 8. Fishing - Magic Fish Above 129 Skill

`RandomFish()` in `fishing.inc` rolled `Random(skill) + 1` and set a fish type for rolls under 50, under 100 and under 130. A roll of 130 or more left `fishtype` uninitialized, so with 150 Fishing about 21 casts in 150 (1 in 7) passed an unset type to `createfish()` and gave nothing. The last branch is now a plain `else`: every roll of 100 or more gives a magic fish (`0xA368` + `Random(9)`). `fishingnet.src`'s magic fish roll uses the same function.

---

## 9. Regeneration

### 9.1 Units

`NewRegenRateFor<vital>` (`CPROP_NAME_PREFIX_REGEN_RATE` + `VITALID_*`) holds hundredths of a point per minute; `regen.src` (`pkg/opt/shilhook`) uses 1200 (12 a minute) when the CProp is absent, and the core clamps to 30000. The `npcdesc.cfg` CProps `BaseHpRegen`, `BaseManaRegen` and `BaseStaminaRegen` are in twelfths: the rate is the number x 12, so 100 = normal and 2500 = the cap.

### 9.2 The defects

- `dotempmods.inc` `BaseRegenRate(who)` returned the `baseregen` CProp, or **100** when it was missing (it always was for players). `processtempmod.src`, `processpersistedmod.src` and `UndoTheMod("poison")` wrote that straight into `SetHpRegenRate()`, so any temporary mod or poison ending set HP regen to 1 a minute and left it there. `CurePoison` multiplied by 12 and got 1200 by accident. The save had `NewRegenRateForlife i100` on 1,400 characters and 308 NPCs.
- `skilladvancerunequip.src` subtracted an item's `HpRegen`, `ManaRegen` and `StamRegen` from the wearer's rates, but `DoRegenMods()` in `skilladvancerequip.src` is commented out, so nothing was ever added. Both files also wrote `basemanaregen` into `basestamregen`.
- `modsetup.inc` applied template regen only when `Get<X>RegenRate(modnpc) == 100`, which is false for a normal NPC (1200). Only NPCs already stuck at 100 by the first defect got their template rate.

### 9.3 Helpers (`scripts/include/attributes.inc`)

| Function / const | Purpose |
|---|---|
| `DEFAULT_REGEN_RATE` (1200), `NPC_BASE_REGEN_DEFAULT` (100), `NPC_BASE_REGEN_MULTIPLIER` (12) | The units above |
| `NpcTemplateRegenBase(who, base_prop)` | The `Base<X>Regen` number from the NPC's npcdesc template, read live (`CProp BaseHpRegen i100` parsed with `SplitWords`), falling back to the NPC's own CProp if the template has none. Live, so template edits reach existing NPCs and tamed pets, which never respawn. |
| `NpcTemplateRegenRate(who, base_prop)` | That number (100 when missing or 0) x 12 |
| `NormalHpRegenRate(who)` | NPC: the template rate. Player: 1200 (item regen is not applied, section 9.2) |
| `SetVitalRegenRateOrDefault(who, vitalid, rate)` | Clamps to the core range; 1200 erases the CProp; skips the write when unchanged; then `RecalcVitalsIfOnline` |
| `RestoreHpRegenRate(who)` | `SetVitalRegenRateOrDefault(who, VITALID_LIFE, NormalHpRegenRate(who))` |

`attributes.inc` gains `use basic;` for `SplitWords`.

### 9.4 Call sites

- `dotempmods.inc`: `BaseRegenRate()` deleted; `CurePoison()` and `UndoTheMod("poison")` call `RestoreHpRegenRate()`.
- `processtempmod.src`, `processpersistedmod.src`: when not poisoned, `RestoreHpRegenRate(who)` (a no-op when the rate is already right). The stray `who.setPoisoned(0)` on an unpoisoned mobile is gone.
- `res.inc`: `SetHpRegenRate(who, 1200)` becomes `RestoreHpRegenRate(who)`, so an NPC gets its template rate back.
- `playermanager.src` (logon, inside the existing critical section after `CurePoison`): `RestoreHpRegenRate`, plus mana and stamina back to 1200. Nothing else changes a player's rates, so this repairs the 1,400 stuck characters and any damage from the unequip bug.
- `skilladvancerunequip.src`: the subtraction is replaced by a commented `//UndoRegenMods( who , it );` and a new uncalled `UndoRegenMods()` that mirrors `DoRegenMods()`; turn both on together. The `basestamregen` typo is fixed in both files.
- `modsetup.inc`: on every AI start, sets all three rates from `NpcTemplateRegenRate()`. HP is skipped while the NPC is poisoned (poison owns that rate and `CurePoison` restores it).

---

## 10. Pets - Special Spell Mana and Cast Cooldown

### 10.1 Mana (`scripts/include/npccast.inc`)

`castspell()` now starts with `NpcSpecialSpellManaID(spellname)`. If the caster `PaysForSpecialSpells()`, `ConsumeMana(caster, spellid)` must succeed or the cast is dropped. Wild NPCs still cast free.

| Special | Pays the mana of |
|---|---|
| `massvatten` | Sorcerer's Bane |
| `massgust` | Gust of Air |
| `masswyvern` | Wyvern Strike |
| `masskill` | Kill |
| `masscalllightning` | Call Lightning |
| `massicestrike` | Ice Strike |
| `massshiftingearth` | Shifting Earth |
| `massholybolt` | Holy Bolt |
| `masseld`, `champfire` | Meteor Swarm |
| `summondrag`, `summonskel`, `summonskelarmy`, `gluttons`, `superspider` | Summon Daemon |
| `summonsshiftingearth`, `summonsgustofair`, `summonsrisingfire`, `summonsicestrike`, `summonsspectretouch` | Shifting Earth, Gust of Air, Rising Fire, Ice Strike, Spectre's Touch (`SPELLID_SPECTER_TOUCH`) |

`PaysForSpecialSpells(caster)` is true only with a master (`.master` or the `master` CProp) and without the `summoned` or `EBSummons` CProp, so summoned creatures (the earth magic spirit elementals) stay free.

### 10.2 Cooldown (`scripts/ai/tamed.src`)

The caster branch of `Fight()` tested `nextbreath`, which only the breath branches advance, so a tamed caster cast on every loop. It now tests `#lastbreath` like `spellcombatevent.inc` (a 2 second cooldown).

---

## 11. Karma and Fame from the Template

### 11.1 Fields

Every active template has `alignment`, `Karma` (signed by alignment), `Fame`, `KarmaLoss` and `FameLoss` (section 14). The values are the Karma and Fame Table artifact defaults, seeded from `MagicItemLevel` or STR into the tiers 100 / 750 / 1,500 / 3,000 / 6,000 / 11,000 / 15,000.

### 11.2 `NpcTemplateKarmaFame(tmpl_name)` (`pkg/opt/karmafame/karmafame.inc`)

Strips a `:pkg:` prefix, reads `::npcdesc` and returns `struct {karma, fame, karmaloss, fameloss}`, or 0. Karma's sign follows alignment (evil negative, good positive, anything else 0) whatever sign the field was written with; fame is clamped to 0 or more. Adds `use basic;`.

### 11.3 `AffectKarmaAndFameForKill()`

- Returns at once for an NPC attacker (Warriors for Hire, monsters on the hit list). A pet's damage is already credited to its master.
- The own-pet, VetResed and guild checks are unchanged.
- The kill's template comes from the corpse's `npctemplate` CProp (or `.npctemplate`). With a template: if `KarmaLoss` or `FameLoss` is above 0, every attacker loses those amounts and gains nothing; otherwise fame and karma come from the template. A defender with no template (a player) uses `GetFame` / `GetKarma` as before.
- A neutral template (karma 0) gives fame only. The gain formulas (`FACTOR_FAME`, the karma steps, the `factor` split) are unchanged.

### 11.4 `modsetup.inc`

The old formula is deleted: stats x AR / 12, x1.2 for spell and breath scripts, equipment hitscript bonuses, regen and protection bonuses, a random +-10%, and an `alignmodnpcnt` typo that made every NPC's karma negative. On every AI start the `Karma` and `Fame` CProps are set from the template (when they differ) for the other code that reads them.

### 11.5 Knock-on effects

- `holybook` Wrath of God damage uses the karma difference between caster and target, and NPC karma changed (neutral NPCs are now 0, good ones positive).
- The chaos / newbie `multikillpcs` reanimate AIs only raise corpses with karma below -400: now evil ones only.

---

## 12. Control Undead and Spell Bind - `charm.inc`

### 12.1 The defects

- **Core master left behind (Control Undead).** The spell set the `master` CProp and the `tamed` script, and `tamed.src` then called `me.setmaster(master)`. When the spell ended it restored the script and erased the CProp but never called `setmaster(0)`. In pol-core, `NPC::repsys_on_attack()` (`repsys.cpp`) hands an attack by an NPC with a master to `master->repsys_on_attack()`, which says `*<master> is attacking you!*` to the defender and gives the master the aggressor timer, or the criminal flag against an innocent. The save had a `killpcs` spectre (`0x791979`) with no `master` CProp and `master 7620328` in its core data.
- **Timer only in the sleeping spell script (both spells).** A reboot during the charm killed the script, so nothing ever ended it: a permanent pet.
- **Recast on your own creature (Control Undead).** It saved `"tamed"` as the creature's old script, so the end restored `tamed`.
- **Town release (both).** `Release()`, and `OrderedFight()` in a safe area, call `ConfiscatePetForRestrictedRelease()`, which issued a confiscation ticket; the Animal Trainer rebuilds the creature from the ticket's template as a real pet.
- **Taking owned creatures.** Control Undead took anything with `Type Undead`, including another player's pet or animated dead, which then went wild for good when the spell ended. Spell Bind's owner contest only looked for online owners, and a taken pet went wild at the end.
- **Leftovers.** Control Undead never erased `charmed` (3 wild spectres in the save still had it). Spell Bind called `WarnManagerOfNpcDeath` twice, the second time after erasing `SpawnPoint`.

### 12.2 `scripts/include/charm.inc` (new)

CProp `Charm` = `struct {spell, ends_at, master, script, name, prev_master}`. `charmed` is still set to 1 for the code that reads it (no transfer, stabling or mounting; counts as a magic pet).

| Function | Purpose |
|---|---|
| `StartCharm(mob, caster, duration, spellname)` | Stores the struct (`prev_master` = the owner when the target was already a `tamed` pet), sets `charmed`, `master`, `setmaster(caster)` and the `tamed` script, and restarts the AI. Returns `ends_at`, which the spell keeps to recognise its own charm. |
| `IsSpellCharmed(mob)` | `Charm` or `charmed` is set. (Named so to avoid `damages.inc`'s `IsCharmed`.) |
| `CharmExpired(mob)` | Past `ends_at`; a pre-1.1.6 charm (no `Charm`) counts as expired. |
| `CharmPrevMaster(mob)` | The owner a taken pet goes back to (offline mobiles searched too), or 0. |
| `EndCharm(mob)` | Erases `Charm` and `charmed`. A taken pet gets its owner back (`master` CProp, `setmaster`, `tamed`). Anything else gets `master` erased, `setmaster(0)` and its own script: the stored one, a pre-1.1.6 Spell Bind script name, or the template's. Restores the stored name, messages caster and owner, restarts the AI. Returns the owner, or 0. |
| `ClearStaleCharmFlag(mob)` | A tameable undead (undead horse, mare, llama, dracoliche, armoured undead dragon) freed by the old Control Undead kept `charmed 1`. If it was later tamed for real (`PreviouslyTamed`), the flag is dropped instead of freeing the pet. |

### 12.3 `pkg/opt/necro/controlundead.src`

- Refuses non-NPCs, `iChampSpawn` creatures (champion spawns are `notame`, which the spell never checked), and anything that already has a master or a charm (`script == "tamed"`, a `master` CProp, `IsSpellCharmed`): "This creature is already under your control." / "That creature already has a master."
- On success: `WarnManagerOfNpcDeath` (the spawnpoint respawns), erase `SpawnPoint` and `Anchor`, `StartCharm(..., "controlundead")`, sleep, then `EndCharm` only if `Charm` still carries this cast's `ends_at` and caster.

### 12.4 `pkg/opt/necro/spellbind.src`

- `iChampSpawn` joins `untamable` as a refusal.
- Refuses an already charmed creature, and an NPC with a core master that isn't a pet (Warriors for Hire): "That creature's mind is already bound."
- The owner contest finds offline owners too (`SYSFIND_SEARCH_OFFLINE_MOBILES`).
- The spawnpoint is warned once, and only for a wild target.
- `StartCharm(..., "spellbind")`, sleep, then end only its own charm. If `EndCharm` returned the pet to its owner, the spell stops there; otherwise the old ending runs (`tametimer` +300, and the chance that the creature turns on the caster).

### 12.5 `scripts/ai/tamed.src`

- Includes `include/charm`. On AI start (after the NPC-master hand-off): `ClearStaleCharmFlag`, then `EndCharm` and return if `CharmExpired`. The same check runs at the top of every `MainAILoop()` pass, so a charm ends even if its spell script died.
- `Release()`: charmed creatures are handled first (a taken pet may carry `summoned`). In a no-PK, guarded or safe area, a charm with no owner to return to goes through `ConfiscatePetForRestrictedRelease()`; otherwise `EndCharm()`.
- `ConfiscatePetForRestrictedRelease()`: a charmed creature never gets a ticket. A taken pet goes back to its owner (`EndCharm`); anything else vanishes like a summon (smoke, moved to 5288 1176, killed).

### 12.6 `scripts/ai/setup/modsetup.inc`

On a wild AI start (`script != "tamed"`) of a creature still marked by a charm: `setmaster(0)`, and erase `Charm`, `charmed` and `master`. This frees the stale-master spectre and clears the 3 leftover flags. All 42 controllable undead templates use `killpcs`, `spellkillpcs` or `explosionkillpcs`, which all include modsetup.

---

## 13. Review of the Branch

A code review of `8d85c3e` plus the uncommitted work, after a clean full compile:

- **Fixed:** logon resets mana and stamina regen as well as HP (section 9.4); NPC regen is read live from npcdesc instead of the creation-time CProps (section 9.3); `processtempmod` / `processpersistedmod` no longer read the template twice; Spectre's Touch uses `SPELLID_SPECTER_TOUCH`; `scripts/items/pitcher.src` passes `character.realm` to `IsWater()` (the follow-up listed in the harvest notes).
- **Rejected:** "harvesting fails outside Britannia because `regions/*.cfg` has no `Realm`". In pol-core every realm's zone grid starts at region 0 (the `RegionGroupBase` constructor), so an unlisted realm falls back to the file's first region, and `ResourceDef::findmarker()` checks the tile in the realm passed. Passing `character.realm` to `GetHarvestDifficulty()` is right.
- **Kept on purpose:** the uncalled `UndoRegenMods()` (the pair to `DoRegenMods()`), and the literal `summoned` / `master` CProp checks in `PaysForSpecialSpells()` (pulling `damages.inc` into the NPC cast code for a rename was not worth it).

---

## 14. `npcdesc.cfg` Rewrite

Generated from the Karma and Fame Table defaults by a script that edits each active `NpcTemplate` in place (`#`-commented templates untouched, CRLF kept). A new header comment documents every field and the regen units.

- **Every active template (752)** has exactly one `alignment`, `Karma`, `Fame`, `KarmaLoss`, `FameLoss` and `CProp BaseHpRegen` / `BaseManaRegen` / `BaseStaminaRegen`. These replace the 136 old `Karma` / `Fame` fields that nothing read.
- **Alignment:** 495 evil (negative karma), 174 neutral (karma 0), 70 good with karma 0, 13 good with positive karma. 157 templates gained an explicit `neutral`, the core default, so no repsys change.
- **Fame:** 0 (73), 100 (34), 750 (211), 1,500 (105), 3,000 (100), 6,000 (103), 11,000 (112), 15,000 (12).
- **Losses (250 karma / 100 fame), 23 templates:** the killable townsfolk `townperson`, `person`, `artist`, `townguard`, `CastleGuard`, `CastleGuard2`, `questie`, `peasant`, `noblemale` and `noblefemale`, and the good creatures `beckon`, `forestlord`, `unicorn`, `kirin`, `dolphin`, `pixie`, `dryad`, `wisp`, `radiantwisp`, `mountainwisp`, `holywisp`, `humuc` and `fairy`.
- **Overrides of the table:** `warriorforhire` stays `neutral`, because `good` would make attacking someone's hired warrior a criminal act. `quest_target` stays `neutral` with no loss: it has `guardignore 1` and is spawned by staff for events, and the table's `good` + 250/100 would flag and fine whoever kills it.
- **Regen:** missing values became `i100`; 49 mana values above 2500 were clamped to 2500 (the core cap, so no change in behaviour). 126 templates regenerate HP faster than normal, 5 slower (the champion Spawns, `i20`), and 68 regenerate mana faster.
- The file has two `Rabbit` templates; both got the same values.

---

## 15. Login Limit per IP - No More In-World Kicks

Commit `3f9d67b`, made on Patch-1.1.5 as a live hotfix (PR #355 into Test-Shard, PR #356 into Live-Shard). It is documented here because it reaches Test-Shard together with 1.1.6.

### 15.1 The incident (live, 2026-11-08)

- `MaxIpsOnline 2` (`pkg/systems/accounts/config/settings.cfg`) was enforced in `pkg/systems/accounts/logon.src`: the third non-staff, non-`ExtraClient` character on an IP entered the world and was then `DisconnectClient`ed.
- After each of those kicks the server accepted no new connection from anyone for 150-153 s: 68 kicks out of 68 between 08:18 and 11:12 server time. Players already in the world kept playing, and sysload stayed at its normal 30-40.
- One player ran four accounts from one IP with clients that reconnect on their own. When a freeze ended, the queued connections were all accepted in the same second (about 30 per burst, logged as `disconnected (account unknown)`, 1,894 in total). The clients logged back in, the third was kicked, and the next freeze started.
- Everyone else could only get in during the one-second window at the end of each freeze. One player who dropped needed 15 minutes to get back in. A reboot did not help, because the clients came back and started the loop again.
- Earlier single freezes after a kick, from other players: 2026-10-23 (two, 150 s), 2026-10-25 (95 s), 2026-11-06 (180 s). Not every kick freezes; some players reconnected 3 s later.
- Why the core stops accepting is not known. `UseSingleThreadLogin=1` in `pol.cfg` handles every pre-login client in the listener thread, which fits, but it is unconfirmed. The fix avoids the in-world kick rather than the stall.

### 15.2 The fix (`pkg/systems/accounts/`)

| Where | Change |
|---|---|
| `hook/onLogin.src` `LoginHook` (0x80) | When `AcctHackChecks` returns `ACCT_LOGIN_OKAY`, and for a new auto-account before `CreateNewAccount`: `ACCT_IPLimitReached(username, connection.ip, 1, 1)`. Over the limit: `SysLog`, `DeniedMSG(connection, 6)` (0x82 code 5, "The IGR concurrency limit has been met") and `return 1`, so the core drops the packet and the character never enters the world. |
| `include/accounts.inc` `ACCT_IsIPLimitExempt(account)` | New. Exempt: `defaultcmdlevel`, the `ExtraClient` account prop, or any character in slots 1-7 with a cmdlevel (a staff character on an account whose default was never raised). |
| `include/accounts.inc` `ACCT_GetPendingIPLogins()` | New. Reads the global `#IPLimitPendingLogins` (`dictionary{ ip -> dictionary{ acct_name -> expiry } }`, `ReadGameClock` seconds) with expired entries dropped. |
| `include/accounts.inc` `ACCT_IPLimitReached(acct_name, ip, include_pending := 0, reserve := 0)` | New. Counts distinct accounts on the IP other than `acct_name`: characters in the world (not staff, not `ExtraClient`), plus pending logins when `include_pending`. Returns 1 when the count is `>= MaxIpsOnline`. With `reserve`, an allowed login is recorded as pending for `ACCT_IPLOGIN_PENDING_SECS` (120). No sleeps, because it runs inside the run-to-completion packet hook. |
| `include/accounts.inc` `CheckForMaxClientsOnline(who)` | Now `!ACCT_IPLimitReached(who.acctname, who.ip)`, counting characters in the world only, as before; staff skipped. The `SleepMS(10)` per online player is gone. `logon.src` and `reconnect.src` still use it as a backstop. |
| `logon.src` | The duplicated kick block is now one block. |
| `hook/onLogin.src` `DeniedMSG` | Wrote the reason byte at offset 2 of a 2-byte 0x82, so every denial went out as code 0 ("Incorrect name/password"). Now writes `reason - 1` at offset 1. |

### 15.3 Why pending logins count

At 08:18:56 two of the four accounts passed the login server in the same second, before either was in the world. Counting characters in the world alone would have let both through, and the backstop would have kicked one and frozen logins again. With pending logins counted, replaying that morning's login order gives two accounts in and two refused at the login screen, with no kicks.

### 15.4 Code review

A code review of the first version found the pending-login gap above, the unchecked auto-account path and the staff-character exemption. All three are fixed in `3f9d67b`. Left as they were: the startup `blockLogin` path still returns 0 after its denial, so it does not actually block (it never did); a wrong password gets the hook's 0x82 and then the core's own; a refused connection is left for the client to close.

---

## 16. Areas Package Cleanup

The parked (non-bug) list from the 2026-10-08 review of `pkg/opt/areas`, plus the one data fix kept in the plan.

### 16.1 Wrong Level 3 box (`areas.cfg`)

`Area 5784 5723 620 667 id=wronglevel3` had min_x above max_x, so the box never matched and the level fell through to `zuludungeonterritory`. min_x is now `5684`. No earlier line overlaps 5684-5723 x 620-667, so this is the first match there. It was the only inverted box in the file.

On the live save (2026-11-08) `wronglevel3` already has `Mask i10` (no recall to + no marking), like `wronglevel`, `wronglevel2` and `wronglevel4`. Those flags start applying on Wrong level 3 with this fix.

### 16.2 World-wide flags on every facet (`include/areapolicy.inc`)

- **Before:** `GetGlobalBypassMaskLocal()` OR-merged the masks of four hardcoded ids into every resolve: `feluccawholeworld`, `wholeworld`, `britannia` and `trammelwholeworld`. `wholeworld` and `britannia` are not in `areas.cfg` (and `PruneStaleRealmPolicies` deletes ids that aren't), so only the two whole-world lines worked. A flag on the `ilshenar`, `malas`, `tokuno` or `termur` world lines applied only to points no other area on that facet matched.
- **After:** a world catch-all is any area whose id ends in `wholeworld` (`IsWorldCatchallId()`). `areas.cfg` gains one such line at the end of each other facet, like Britannia's: `ilshenarwholeworld`, `malaswholeworld`, `tokunowholeworld` and `termurwholeworld`, each covering the whole map. The plain facet lines (`ilshenar`, `malas`, `tokuno`, `termur`) stay ordinary fallback areas, so a flag on them still covers only ground no other area claims, while a flag on the new Whole World line applies across the facet. (A first version treated an id equal to the realm name as the catch-all; the code review pointed out that this turned every flag on e.g. Malas into a facet-wide override, Luna and Umbra included, that smaller areas could no longer turn off.) `EnsureLocalParsedAreas()` collects them into `g_areapolicy_global_ids` when a script switches realm, and `GetGlobalBypassMaskLocal()` ORs only those, normally one lookup instead of four. `GetGlobalBypassMask()` now also calls `EnsureLocalParsedAreas()`.
- The four `AREA_POLICY_GLOBAL_ID_*` constants and the seeding of those ids in `RebuildPolicyMaskCache()` are removed: catch-alls are ordinary area ids and are already in the mask cache.
- **No change on live today:** the `ilshenar`, `malas`, `tokuno` and `termur` policy files are empty in the 2026-11-08 save, and Britannia / Britannia_alt resolve the same ids as before.

### 16.3 `.areas` parses each line once (`textcmd/admin/areas.src`)

- Area lines were parsed 4 times per command (`BuildCategoryDisplayOrder`, `BuildAreaSortKey`, `LoadAreaFlagArrays`, `SaveAreaFlagArrays`). `LoadAreasForRealm()` now fills `area_infos[]` with one `ParseAreaLine()` per line, and all four read it.
- `GetAreaName()`, which re-implemented the name parsing with weaker validation, is replaced by `GetAreaDisplayName(idx)` reading the parsed name. A line `ParseAreaLine()` rejects shows as `(invalid) <raw line>`.
- `EnsureAreaFlagArrays()` and its call are removed: `LoadAreaFlagArrays()` always fills every array to `areas.size()`.

### 16.4 Dead code

- `LeaveArea.src`, `EnterAreaDelay.src`: the commented-out datafile message blocks and `use datafile` are removed. The enter/leave text comes from `regions.cfg` `EnterText` / `LeaveText`, which a header comment now says.
- `EnterAreaDelay.src`: the unused `regionname` variable is removed, saving a `GetRegionName()` call on every region entry.
- `include/areapolicy.inc.bak` (an old 330-line copy) is deleted, and `*.bak` is added to `.gitignore`.

---

## 17. Live Review Fixes (2026-11-22)

From a review of the live logs (11/08-11/22, 1.1.5 live, no restart in that time) and the 11/22 save against the 1.1.5 and 1.1.4 notes. Fixes that checked out: the login IP limit (449 refusals at the login screen, no kicks, no freezes), kill credit, champion bags and tallies, the 1.1.5 log fixes, the areas cache, powerhour, WFH vitals and hearts, world saves (0-1 s).

### 17.1 Escrow packages lost on restart

- **Defect:** `SaveHouseEscrowPack()` (`houseescrow.inc`), `WFH_SaveToEscrow()` (`wfhescrow.inc`) and `SavePackToEscrow()` (`playermerchant.src`) renamed each storage root right after `CreateRootItemInStorageArea(area, escrow_name, ...)`, to `"<house label> Escrow"`, `"Warrior for Hire Escrow"` and `"Player Merchant Escrow - <vendor>"`.
- POL 100.2.0 files a storage root under its *name* when it loads `storage.txt` (`StorageArea::load_item` -> `insert_root_item`, `_items.insert({item->name(), item})`, case-insensitive map). After a restart every renamed root was filed under its display name: `FindRootItemInStorageArea(escrow_name)` failed, the claim said "That escrow package was not found." and deleted the index entry. Roots with the same display name collided on load; `insert` keeps the first, and the others were left out of the area and dropped at the next save.
- **Live evidence (11/22 save):** 24 house escrow index entries against 18 roots in "House Demolish Escrow", each display name exactly once. The WFH registry has 17 entries and 1 root ("Warrior for Hire Escrow"); the 11-08 morning save had 3.
- **Fix:** the renames are removed (also in `pmescrowtest.src`). The labels already live in CProps (`house_label`, `vendorname`), and nothing displays a root's name. `WFH_ESCROW_ROOT_NAME` is removed.

### 17.2 `.recoverescrow` (new, Administrator)

`scripts/textcmd/admin/recoverescrow.src`. Dry run by default; `.recoverescrow commit` applies. Report to the caller and `log/escrowrecovery.log`.

- Walks "House Demolish Escrow" and "Merchant Escrow" (`foreach root in StorageAreas()[name]`, `_root_iter` = the filing key), snapshot first.
- A root filed under a proper key (`HouseEscrow `, `PMEscrow `, `WFHEscrow `) whose name differs (renamed since the last boot): name set back to the key, so it survives the next restart.
- Any other root: a new root under a fresh key, CProps copied, contents moved (all moved back and the new root destroyed if any move fails), old root destroyed. Then the owner's index (`:housing:houseescrow_<owner>` element `<house>`, or `:playervendor:merchantescrow_<owner>` element `<vendor/corpse>`) is re-linked: the first entry with the same label whose package can't be found gets the new key; if none is left (a failed claim pruned it), a new entry is added. WFH also re-registers the sweeper row.
- `PMEscrowTest ` roots seeded by `.pmescrowtest` count as properly filed. In a dry run, an entry that one root would re-link is not offered to the next root and not reported as LOST, so the dry-run counts match what commit does.
- Index entries and WFH registry rows still pointing at nothing are reported as LOST: those packages were dropped by the collision and can't be recovered.
- **Order on go-live:** run `.recoverescrow commit` on the old server before shutting it down (it only needs its own `.ecl`), then again after the 1.1.6 boot, before players claim.

### 17.3 City vendor wage and the champion relic

`IsInCityRegion()` (`scripts/include/areas.inc`, 1.1.5) called `GetRegionString("regions", x, y, "Type", realm)`. That function reads *resource* regions (`gamestate.resourcedefs`: ore, wood, ...), so it returned "No resource by that name" every time and the check was never true. The 14 Ocllo vendors paid the 10,000 tier (333 gold a day) instead of the city tier (1,666), and the champion relic's "not in a city" refusal never fired.

The function is removed. `playermerchant.src` `CalculateDailyWage()` and `championrelic.src` call `CheckCity()` from `include/checkcity` (Type City ranges in `regions/regions.cfg`, realm-scoped), the same check house placement uses. It was not added to `areas.inc` because `go.src` defines its own `NormalizeRegionType()`.

Left as is: `NameChecker.inc` reads a `City` key that `regions.cfg` doesn't have, but the cached town list already holds every town (Papua, Delucia, Randorin, Empath Abbey and both castles come in through `townlist`).

### 17.4 Smaller fixes

| Where | Defect | Fix |
|---|---|---|
| `pkg/opt/guilds/include/guilds.inc` `UnEntwine()` | A missing entwined uniform sent "This is odd, your uniform is gone!, page staff!" to every player online (`Broadcast`), and returned before erasing `Entwined`, so every unequip on death repeated it (6 times for one character). | Props erased first; the message goes to the wearer only; the syslog line names the item. |
| `scripts/ai/killpcsclock.src` | `HALT_THRESHOLD` 24, but `Fight()` only chases inside 20 tiles: a target 20-24 tiles away ended the fight at once, `post_combat()` called `look_around()` again, until "exceeded maximum call depth" (2 in 14 days, Vortex Minions). | `HALT_THRESHOLD` 19. |
| `pkg/opt/warriorforhire/warrior.src` `DropEverything()` | `MoveObjectToLocation(..., 0)`: realm 0, so "<name> drop" never moved anything. | `me.realm`, `MOVEOBJECT_FORCELOCATION`. |
| `pkg/opt/warriorforhire/warriorforhire.src` | No realm on `CreateNpcFromTemplate`, so a deed used outside Felucca made the warrior in Felucca; `henchman` lookup without `CInt`. | `who.realm`; `CInt()`. |
| `scripts/ai/highpriest.src` | "give up warrior" and a heart hand-in passed an unset `henchman` to `SystemFindObjectBySerial` (debug.log, PC 2213). | `CInt()`. |
| `scripts/textcmd/player/guilds.src` applicants gump | Always builds `perpage+1` pages; pages past the last applicant read `list[startat+z]` out of range (debug.log PC 7691/7702). | Detail page only when `startat+z <= list.size()`. The code review also found that from page 2 on the rows were one applicant off from their detail pages (rows started at `startat`, details at `startat+1`), so Accept/Remove/Ban acted on the next applicant: rows are now `startat+1 .. startat+perpage`, and the next-page arrow shows only when more applicants remain. |
| `pkg/std/spells/earthquake.src` | No realm on `ListMobilesNearLocation`, `ListItemsNearLocation` and the four `ListItemsNearLocationOfType` calls, so outside Felucca it searched Felucca; a mobile gone mid-loop gave `z` Uninit. | Realms passed; skip a mobile whose `z` is no longer an integer. |
| `pkg/opt/loot/lootgump.src` | The loop read `corpse.realm` after the corpse decayed. | Break when `corpse.realm` is no longer a string. |
| `scripts/include/speech.inc` `process_text()` | `GetConfigString(elem, "default")` with an error `elem` for templates without a speech.cfg entry (merchant.ecl). | Only when `elem` is valid. |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | Read the bare `golocs_by_id` first, which looks in the package and logged "does not exist". | `::golocs_by_id` first. |

### 17.5 `.wherewarrior` (new, player)

`scripts/textcmd/player/wherewarrior.src`, like `.whereboat`. Finds the warrior from the player's `henchman` CProp, checks it is a live NPC whose `master` CProp is the player, and prints its x, y, z and facet; on the same facet also the distance, and in Felucca or Trammel the sextant position (`include/coords` `ToSextant`, which is centred on Britannia's map).

### 17.6 `.pets` (new, player)

`scripts/textcmd/player/pets.src`. Pets pay mana for specials since section 10, but the client only shows a pet's health: the core sends a mobile's mana and stamina (0xA2/0xA3) to that mobile's own client and its party (`send_mana_level`, `Party::on_mana_changed`), never to an owner. `.pets` lists every NPC within 20 tiles on the `tamed` AI whose `master` CProp is the caller (pets, summons, charmed creatures) with `GetHp`/`GetMaxHp`, `GetMana`/`GetMaxMana` and `GetStamina`/`GetMaxStamina`.

### 17.7 Not code: log noise

- `pol.cfg` `ShowWarningItem=1` (line 194) prints the "tried to drop item ... had not gotten an item" / equip warnings (about 3,000 in 14 days, players' dress and organiser agents). The "used out of sequence cursor" warnings (about 2,000, mostly one character) are `ShowWarningCursorSequence`, not listed in `pol.cfg` and so on by default. Setting either to 0 only silences the console/log line.
- The HTTP/SSH probes on the game port (56 in 14 days) are the Infrawatch scanner (`+https://infrawat.ch/` in the user agent), rotating through 69.5.169.0/24, 64.205.72.0/24, 77.67.26.0/24, 86.38.187.0/24, 200.138.46.0/24 and a few single addresses. 142.134.86.159 also logged "unexpected message" lines but is a player's IP.
- The `eleanor` account: 4,050 wrong passwords 11/08-11/12, one every 60 s from the IPs it also logs in from, a client reconnecting with an old saved password.

---

## 18. Combat - Weapon Elemental Damage Order and Poison Charges

All in `pkg/systems/combat/include/hitscriptinc.inc`, which every weapon hit script includes.

### 18.1 The war mode drop

Reported on 2026-11-23: the Bow of the Air Element and the War Fork of the Water Element take the wielder out of war mode after a kill. Both weapons are `ElementalDamage AIR:100` / `WATER:100`.

A hit script runs `RecalcDmg()` then `DealDamage()`. `RecalcPhysicalDmg()` worked out each element in the weapon's `ElementalDamage` CProp and dealt it on the spot with `ApplyElementalDamageNoResist()`; only the physical remainder went on to `DealDamage()`. A 100% elemental weapon therefore killed the defender inside `RecalcDmg()`.

The core destroys an NPC as soon as it dies (`NPC::on_death` ends with `destroy()`), and every method on a destroyed mobile returns an error (`ECharacterRefObjImp::call_polmethod_id`: "That object no longer exists"). `DealDamage()` then ran its No-PK check: `defender.isa(POLCLASS_NPC)` read the error as false, so `_any_npc` stayed 0, the pair looked like player against player, and with the attacker's `NOPKAREA` CProp set it called `attacker.setwarmode(0)`. On live the No-PK areas are Scourge Outlands, Ocllo / Ocllo Isle / Ocllo Mine, Randorin, Empath Abbey, the jail, the vendor markets, the wedding chapel and the staff areas.

Weapons whose damage is physical were never affected: their kill is the last line of `DealDamage()`, after the check. Every NPC is destroyed on death the same way; the bug was the order.

### 18.2 The fix

- New global `g_pending_elemental` (`{ {element_ID, amount}, ... }`). `RecalcPhysicalDmg()` resets it, computes the same amounts as before and stores them instead of dealing them. The physical remainder is unchanged.
- New `ApplyPendingElementalDamage(attacker, defender)`: deals the stored amounts once with `ApplyElementalDamageNoResist()` and clears them.
- `DealDamage()` calls it right after the No-PK check and the `LottoDrops` damage store, before the poison block. Before the poison block on purpose: a Cursed poisoned weapon swaps `defender` to the attacker there, and that swap never applied to elemental damage.

Amounts, resists and the target are the same as before; only the moment changes.

### 18.3 Who carries elemental damage

The five elemental-NPC weapons (`waterfork`, `bowofair`, `WarHammerofearth`, `Katanaoffire`, `Pitchforkofshadow`, made by `scripts/ai/water.src`, `air.src`, `earth.src`, `fire.src`, `shadow.src`) and every blacksmith weapon made from an ore with `ElemDamage` in `pkg/std/blacksmithy/blacksmithy.cfg` (several hundred in the 2026-11-22 live save). In that save every elemental weapon's hit script is `mainhit`, `slayerscript` or `lifedrainscript`; the first two always reach `DealDamage()`.

### 18.4 Paths that skip `DealDamage()`

Scripts that compute damage and then do not call `DealDamage()` no longer deal the elemental share on that path:

- `lifedrainscript.src` returns early when the defender has `InSafeArea`. On live only the Demonic Termites in Zulu Dungeon 6 (not a safe area) and one loose hammer carry `lifedrainscript` with elemental damage; a termite following a player into a safe area now deals nothing there, which is what a safe area means.
- The cursed branches of `piercingscript.src`, `voidscript.src` and `manadrainscript.src`, and Blackrock's summoned/animated branch. No live elemental weapon uses them (all 637 void weapons on live have no elemental damage), and only loot enchanting or `.newiteminfo` can change a weapon's hit script; loot never makes elemental weapons.
- The armor on-hit scripts `piercingonhit.src` and `raceresistonhit.src` call `RecalcDmg()` again when `basedamage == rawdamage` and used to deal the elemental share a second time. With armor on the wearer that equality only happens when the physical part is 0 (a 100% elemental weapon), and then the recomputed elemental is 0 too. No practical change.

### 18.5 Poison charges

In `DealDamage()`, a `SkillPoisoned` weapon within 14 tiles with `PoisonCharges` 0 erased both CProps and hit `return 0`, so that swing also lost its physical damage, the defender's reactive armor and the armor's on-hit script. Charges only drop when the target is not already poisoned, and the swing that used the last charge left the weapon at 0, so every poisoned weapon lost the next swing when it ran dry (610 poisoned weapons on live, one sitting at 0).

- 0 charges (saved at 0, or poisoned with none): erase both CProps, `IncRevision`, and skip only the poisoning.
- The swing that uses the last charge still poisons and erases both CProps at once, so a weapon never sits at 0 charges.
- Cursed poisoned weapons still swap the target to the wielder while they have charges (intended).

`scripts/ai/formationleader.src` created its poisoned weapon with `SkillPoisoned` 18 and no charges, so the poison was erased on its first swing; it now also gets `PoisonCharges` 100 like the other AIs. No template uses that AI.

---

## 19. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `scripts/include/harvesttiles.inc` | 4, 5, 7.7 | New - `FindHarvestTile()`, `HarvestTileAt()`, `HarvestTileListed()`, `IsTreeItem(use_on, realm)` (`a7c3dfc`), `IsWater(where, realm)`, and the list predicates `IsTree`, `IsOreLand`, `IsOreStatic`, `IsSand`, `IsSwamp`, `IsFishLand`, `IsFishStatic`, `IsWaterLand`, `IsWaterStatic` |
| `scripts/include/itemutil.inc` | 4 | Includes `include/harvesttiles`; `IsMinable`, `IsSand`, `IsSwamp`, `IsTree`, `IsWater` removed |
| `pkg/std/lumberjacking/lumberjack.src` | 7.1, 7.7 | `IsTreeItem` or `FindHarvestTile("wood")` gate; objtype branch, z-search fallback, yucca check and `info` removed |
| `scripts/items/bladed.src` | 7.2, 7.7 | Kindling branch: `IsTreeItem`, or `FindHarvestTile("wood")` on non-item, non-mobile targets; z-search and yucca check removed |
| `pkg/std/mining/mining.src` | 7.3 | Ore / clay / sand chosen once by `FindHarvestTile`; realm passed to `GetHarvestDifficulty`; `info` parameters removed |
| `pkg/std/fishing/fishing.inc` | 7.4, 8 | `check_for_water(theobj, realm)` via `FindHarvestTile("fish")`; `RandomFish()` last branch is `else` |
| `pkg/std/fishing/fishing.src` | 7.4 | Water check after cancel, with realm; second difficulty block removed; depleted check before the random amount |
| `pkg/std/fishing/fishingnet.src` | 7.4 | `check_for_water(use_on, character.realm)` |
| `pkg/std/cooking/fillpitcher.src` | 7.5 | `IsWater(theobj, character.realm)`; includes `include/harvesttiles` |
| `pkg/std/healing/cleanbandages.src` | 7.5 | `IsWater(theobj, character.realm)` |
| `pkg/opt/farming/seed.src` | 7.6 | Cancel check; real fertile dirt; realm on `GetMapInfo` |
| `regions/wood.cfg` | 5.1, 6 | New `ObjType` list; range to `7167 4095` |
| `regions/ore.cfg` | 5.2, 6 | New `LandTile` / `ObjType` lists |
| `regions/sand.cfg` | 5.3, 6 | New `LandTile` list; range to `7167 4095` |
| `regions/clay.cfg` | 5.4, 6 | New `LandTile` list; range to `7167 4095` |
| `regions/fish.cfg` | 5.5, 6 | New `LandTile` / `ObjType` lists |
| `scripts/include/attributes.inc` | 9.3 | `use basic`; regen constants; `NpcTemplateRegenBase`, `NpcTemplateRegenRate`, `NormalHpRegenRate`, `SetVitalRegenRateOrDefault`, `RestoreHpRegenRate` |
| `scripts/include/dotempmods.inc` | 9.4 | `BaseRegenRate()` removed; `CurePoison` and poison expiry restore the normal rate |
| `pkg/opt/summoning/processtempmod.src` | 9.4 | Restores the normal HP rate instead of 100 |
| `pkg/opt/summoning/processpersistedmod.src` | 9.4 | Same |
| `scripts/include/res.inc` | 9.4 | `RestoreHpRegenRate` on resurrection |
| `scripts/playermanager.src` | 9.4 | Logon resets HP, mana and stamina regen |
| `scripts/control/skilladvancerequip.src` | 9.4 | `basestamregen` typo in the dormant `DoRegenMods` |
| `scripts/control/skilladvancerunequip.src` | 9.4 | Regen subtraction off; uncalled `UndoRegenMods()` |
| `scripts/ai/setup/modsetup.inc` | 9.4, 11.4, 12.6 | Template regen on every AI start; karma formula replaced by template values; stale charm cleanup |
| `pkg/opt/karmafame/karmafame.inc` | 11.2, 11.3 | `NpcTemplateKarmaFame()`; kill awards from the template; NPC attackers skipped |
| `scripts/include/npccast.inc` | 10.1 | `NpcSpecialSpellManaID()`, `PaysForSpecialSpells()`; pets pay mana for specials |
| `scripts/ai/tamed.src` | 10.2, 12.5 | Caster cooldown; charm expiry on start and every loop; charmed release and confiscation handling |
| `scripts/include/charm.inc` | 12.2 | New - charm state for Control Undead and Spell Bind |
| `pkg/opt/necro/controlundead.src` | 12.3 | Refusals; `StartCharm` / `EndCharm` |
| `pkg/opt/necro/spellbind.src` | 12.4 | Refusals; offline owners; `StartCharm` / `EndCharm`; taken pets go back |
| `scripts/items/pitcher.src` | 13 | `IsWater(where, character.realm)` |
| `config/npcdesc.cfg` | 14 | Header; karma/fame fields and regen CProps on all 752 active templates |
| `pkg/systems/accounts/hook/onLogin.src` | 15.2 | IP limit checked in the 0x80 hook (existing and new auto-accounts), refused with 0x82 code 5; `DeniedMSG` writes the reason at offset 1 (`3f9d67b`) |
| `pkg/systems/accounts/include/accounts.inc` | 15.2 | `ACCT_IsIPLimitExempt`, `ACCT_GetPendingIPLogins`, `ACCT_IPLimitReached`; `CheckForMaxClientsOnline` rewritten on top of them (`3f9d67b`) |
| `pkg/systems/accounts/logon.src` | 15.2 | Kick block deduplicated (`3f9d67b`) |
| `pkg/opt/areas/areas.cfg` | 16.1, 16.2 | Wrong Level 3 min_x `5784` -> `5684`; Whole World lines for Ilshenar, Malas, Tokuno and Ter Mur |
| `pkg/opt/areas/include/areapolicy.inc` | 16.2 | World catch-alls are ids ending in `wholeworld` (`IsWorldCatchallId`, `g_areapolicy_global_ids`); hardcoded global ids and their cache seeding removed |
| `pkg/opt/areas/textcmd/admin/areas.src` | 16.3 | `area_infos[]` parsed once; `GetAreaDisplayName()` replaces `GetAreaName()`; `EnsureAreaFlagArrays()` removed |
| `pkg/opt/areas/LeaveArea.src` | 16.4 | Commented datafile block and `use datafile` removed |
| `pkg/opt/areas/EnterAreaDelay.src` | 16.4 | Commented datafile block, `use datafile` and unused `regionname` removed |
| `pkg/opt/areas/include/areapolicy.inc.bak` | 16.4 | Deleted |
| `.gitignore` | 16.4 | `*.bak` |
| `pkg/std/housing/houseescrow.inc` | 17.1 | Escrow root no longer renamed |
| `pkg/opt/warriorforhire/include/wfhescrow.inc` | 17.1 | Escrow root no longer renamed; `WFH_ESCROW_ROOT_NAME` removed |
| `pkg/systems/playervendor/playermerchant.src` | 17.1, 17.3 | Escrow root no longer renamed; daily wage uses `CheckCity()` |
| `pkg/systems/playervendor/commands/test/pmescrowtest.src` | 17.1 | Test roots no longer renamed |
| `scripts/textcmd/admin/recoverescrow.src` | 17.2 | New - `.recoverescrow [commit]` |
| `scripts/include/areas.inc` | 17.3 | Broken `IsInCityRegion()` removed |
| `pkg/opt/ArtifactSystem/championrelic.src` | 17.3 | City refusal uses `CheckCity()` |
| `pkg/opt/guilds/include/guilds.inc` | 17.4 | `UnEntwine()`: props erased first, message to the wearer only |
| `scripts/ai/killpcsclock.src` | 17.4 | `HALT_THRESHOLD` 24 -> 19 |
| `pkg/opt/warriorforhire/warrior.src` | 17.4 | `DropEverything()` passes the realm |
| `pkg/opt/warriorforhire/warriorforhire.src` | 17.4 | Deed creates the warrior in the player's realm; `CInt` on `henchman` |
| `scripts/ai/highpriest.src` | 17.4 | `CInt` on `henchman` lookups |
| `scripts/textcmd/player/guilds.src` | 17.4 | Applicant detail pages bounded by the list; row/detail off-by-one from page 2 fixed |
| `pkg/std/spells/earthquake.src` | 17.4 | Realm on all area searches; gone mobiles skipped |
| `pkg/opt/loot/lootgump.src` | 17.4 | Stops when the corpse is gone |
| `scripts/include/speech.inc` | 17.4 | No `GetConfigString` on a missing element |
| `pkg/opt/spawnpoint/textcmd/admin/restartspawnpointarea.src` | 17.4 | `::golocs_by_id` read first |
| `scripts/textcmd/player/wherewarrior.src` | 17.5 | New - `.wherewarrior` |
| `scripts/textcmd/player/pets.src` | 17.6 | New - `.pets` |
| `config/command_synopses.cfg` | 17.2, 17.5, 17.6 | Regenerated (`.recoverescrow`, `.wherewarrior`, `.pets`) |
| `pkg/systems/combat/include/hitscriptinc.inc` | 18.2, 18.5 | `g_pending_elemental`, `ApplyPendingElementalDamage()`; elemental damage dealt in `DealDamage()` after the No-PK check; empty poison charges no longer end the hit |
| `scripts/ai/formationleader.src` | 18.5 | Poisoned weapon gets `PoisonCharges` 100 |
| `patchnotes/developer-changelog-v1.1.6.md` | - | New - this file |
| `patchnotes/patch-v1.1.6.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.6 player notes |

---

## 20. Risk and Regression Notes

- **Build.** `attributes.inc`, `itemutil.inc` and `modsetup.inc` are included almost everywhere: deploy with a full `ecompile -A` (1,163 scripts, 0 errors at this commit).
- **Region configs are read at server start.** The new `wood.cfg`, `ore.cfg`, `sand.cfg`, `clay.cfg` and `fish.cfg` lists need a restart (or a resource reload) to match the scripts. Until then `GetHarvestDifficulty()` checks the old lists and `FindHarvestTile` can refuse newly added tiles.
- **Shore fishing is the visible change.** About 165,000 shoreline cells stop being fishable. Expect questions from fishers who stood on the beach and targeted the first water square; casting further out works.
- **Cave floor tiles `0x24A-0x26D` now give ore.** Some dungeon areas that gave nothing or clay now give ore. Ore regions in `ore.cfg` were not changed, so difficulty and ore type follow the existing regions.
- **Troughs, tubs, barrels, buckets and fountains as map statics** now fill pitchers. Alryc has not explicitly confirmed this part of the water list (it stands at Claude's suggestion).
- **Tree items still give wood and kindling** (section 7.7). Any tree item on the ground counts, planted or decoration, including decoration trees players may not expect to be choppable. They take from the region pool like before 1.1.6.
- **Multis are ignored** (`ITEMS_IGNORE_MULTIS`): trees and water that are part of a house or boat multi don't count.
- **Regen applies on the next AI start.** Existing NPCs and pets pick up template regen and the karma/fame CProps when their AI restarts (a reboot does it); kill awards read npcdesc live. Players are repaired at logon.
- **Fast-healing and fast-mana creatures.** 126 templates heal faster and 68 regain mana faster than before (their values were never applied). Bosses such as balrons and the Dragon King now regain mana at the core cap; watch fight lengths.
- **Pets with specials now run dry.** A pet stops casting its special when it is out of mana, and tamed casters wait 2 seconds between spells instead of casting every fight loop.
- **Karma shifts.** Killing neutral animals no longer raises karma; killing the 23 loss templates costs 250 karma and 100 fame per attacker on the hit list. Wrath of God damage and the reanimate AIs follow the new NPC karma (section 11.5).
- **Charmed creatures released in town vanish**, they are not confiscated; this also applies to Spell Bind. Players used to the ticket lose the creature (it was never theirs to keep). A pet taken with Spell Bind goes back to its owner instead.
- **Stale charm cleanup** runs on wild AI start and on pet AI start. A pre-1.1.6 charm with no end time is ended at once; a real pet carrying the old flag is kept if it has `PreviouslyTamed`.
- **Login limit per IP (section 15).** Live since 11/08: 449 over-limit logins refused in 14 days, no in-world kicks and no login freezes. Over-limit logins now stop at the login screen with "The IGR concurrency limit has been met". A login that passed the login screen holds its IP slot for up to 120 s, so cancelling at character select can get the same IP's other accounts refused for up to 2 minutes. Because `DeniedMSG` now sends real codes, a wrong password shows "credentials invalid" and the startup `blockLogin` shows "General IGR authentication failure". The in-world kick is still there as a backstop for clients that skip the login server or wait at character select for more than 120 s. On the test shard, check that a refusal is followed straight away by `disconnected (account unknown)` in pol.log, which means the client closed the socket. Players who run more than 2 accounts at once need `.extralogin` on the extra accounts.
- **Areas package (section 16).** `areapolicy.inc` is included by the AI and spell code that checks area flags, so deploy with a full `ecompile -A`. `areas.cfg` is parsed into the shared cache by the package start script, so the Wrong Level 3 box takes effect after a restart; from then on recall and marking are blocked on that level. The four new Whole World areas show up in `.areas ilshenar` / `malas` / `tokuno` / `termur` at the top of World; a flag set on them applies across that whole facet, towns included, the same way Felucca Whole World does on Britannia. The plain facet lines behave as before.
- **Escrow recovery order (section 17.2).** Run `.recoverescrow commit` on the old server just before the 1.1.6 restart and again right after it, before players claim. A claim on a package that hasn't been re-keyed yet still says "not found" and prunes its entry; the recovery then adds a fresh entry, so the package isn't lost, but the player sees the error once. Packages already dropped by name collisions can't be recovered; the LOST lines in `log/escrowrecovery.log` list them for refunds.
- **City wage (section 17.3).** Vendors in Type City regions start paying 1,666 gold a day instead of 333. On 11/22 the 14 Ocllo vendors were all paying the lower rate; 0x772204 held 10,000 gold and 0x771b7c 18,667, so their owners should be told.
- **Vortex Minions** now notice players at 19 tiles instead of 24.
- **Elemental damage order (section 18).** `hitscriptinc.inc` is included by every weapon hit script: deploy with a full `ecompile -A`. Damage per hit should not change; a kill with a 100% elemental weapon in a No-PK area no longer drops war mode. Elemental damage is now skipped on the few paths that never call `DealDamage()` (18.4); no live item uses them.
- **Poison charges (section 18.5).** A poisoned weapon's tooltip stops showing "0 charges": the poison disappears on the swing that uses the last charge.
- **Spell Bind debug line.** The spell still prints `difficulty: <n>` to the caster; it predates this patch and was left as is.
- **Not run in a client.** Worth testing on the test shard: a poison and a temporary mod ending (HP regen stays normal), a pet's special spell mana, a kill of a neutral animal and of a townsperson, Control Undead and Spell Bind running out, a reboot mid-charm, and a release in town; a kill with the Bow of the Air Element or the War Fork of the Water Element in a No-PK area (stays in war mode); a weapon poisoned with one charge hit twice (both swings do damage).
