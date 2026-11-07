# Developer Changelog - v1.1.6
**Range:** `b640f5c` (merge of #353, the last 1.1.5 patchnotes state) -> working tree (this commit)  
**Branch:** Patch-1.1.6 (branched from `origin/Test-Shard` at `b640f5c`)  
**Date:** 2026-11-03 to 2026-11-07 (harvest tile audit 2026-11-03; tile lists picked and code written 2026-11-04; notes 2026-11-07)  
**Commits in range:** 1 (this one).  
**Files changed:** 13 scripts and region configs modified, 1 new include (`scripts/include/harvesttiles.inc`), plus this release's 3 `patchnotes/` files. The full list is section 9.

> Scope was computed against `b640f5c`. `origin/Test-Shard` and `HEAD` were both at `b640f5c` when Patch-1.1.6 was cut, so the whole delta is this commit.
> Nothing in this patch was compiled before the commit. Run `ecompile` on the changed scripts before deploying (section 10).

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [The Defect - Spoofed Target Graphics](#3-the-defect---spoofed-target-graphics)
4. [`harvesttiles.inc` - One Place for the Tile Lists](#4-harvesttilesinc---one-place-for-the-tile-lists)
5. [The Tile Lists](#5-the-tile-lists)
6. [Region Configs](#6-region-configs)
7. [Per-Script Changes](#7-per-script-changes)
8. [Fishing - Magic Fish Above 129 Skill](#8-fishing---magic-fish-above-129-skill)
9. [Exhaustive File-by-File Change List](#9-exhaustive-file-by-file-change-list)
10. [Risk and Regression Notes](#10-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.6 is one theme: gathering skills stop trusting the client.

`TargetCoordinates()` returns whatever graphic the client says was clicked (`use_on.objtype`) without the core checking it. Every gathering script tested that graphic against a hard-coded tile list. A macro or a modified client could send a tree graphic for a grass square and get wood, send a water graphic anywhere and fish, and so on. The audit (2026-11-03) found the same pattern in lumberjacking, `bladed.src` kindling, mining, clay, sand, fishing, the fishing net, `fillpitcher.src`, `cleanbandages.src` and `seed.src`.

The fix reads the real statics (`ListStaticsAtLocation`) and land tile (`GetMapInfo`) at x,y in the caster's realm, and confirms the tile with the engine's own `GetHarvestDifficulty()`, which only accepts tiles listed in the matching `regions/*.cfg` that are really at that spot. Section 3 has the detail.

While doing that, every tile list was rebuilt tile by tile from the client art (`art.mul`) in the Harvest Tile Picker artifact and reviewed by Alryc (2026-11-04). The script predicates and the region configs had drifted apart: `wood.cfg` listed grasses and ferns, `fish.cfg` listed sand, `clay.cfg` listed cave and void tiles, and `ore.cfg` listed cave floor statics as land tiles. They now match exactly (section 5).

Section 8 is a small fishing fix found during review: `RandomFish()` left the fish type unset for skill rolls of 130 and above.

## 2. Commit Timeline

| Commit | Date | Subject | Sections |
|---|---|---|---|
| *(this commit)* | 2026-11-07 | Harvest: check real tiles instead of client graphics, rebuilt tile lists, patch notes | 3-8 |

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

Each script asks `FindHarvestTile(resource, x, y, realm)` (section 4), which only returns a tile that is both in the script-side list and accepted by the engine at that spot. The client's objtype is never read. Water for pitchers and bandages has no region config, so `IsWater(where, realm)` reads the real statics and land directly, and accepts a placed item only by its own `graphic` and only when it is not in a container.

---

## 4. `harvesttiles.inc` - One Place for the Tile Lists

New file `scripts/include/harvesttiles.inc`, included by `scripts/include/itemutil.inc` (so every existing `itemutil` user gets it) and directly by `fillpitcher.src`.

| Function | Purpose |
|---|---|
| `FindHarvestTile(resource, x, y, realm := 0)` | `"wood"`, `"ore"`, `"sand"`, `"clay"` or `"fish"`. Checks statics at x,y (`LIST_IGNORE_Z`, `ITEMS_IGNORE_MULTIS`) first, then the land tile. Returns the matching tile id, or 0. Realm defaults to `_DEFAULT_REALM`. |
| `HarvestTileAt(resource, x, y, tile, realm)` | Calls `GetHarvestDifficulty()` and tests `.errortext`, not the value: a difficulty roll of 0 is valid. `"Resource is depleted"` still counts as present, so the skill can say "none left" instead of "can't use that". |
| `HarvestTileListed(resource, tile, is_land)` | Dispatches to the list predicates. Wood is statics only; sand and clay are land only; ore and fish are both. |
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

- Yucca check replaced by `FindHarvestTile("wood", use_on.x, use_on.y, character.realm)` before the loop; failure: "You can't get any wood from that."
- The `IsTree(use_on.objtype)` branch and the z-search fallback (four `ListStaticsAtLocation` calls at z+1, z-1, z-2, z+2) are gone. `FindHarvestTile` ignores z, so clicking the ground under a tree still finds it.
- The unused `GetMapInfo()` in the loop and `ChopTree()`'s `info` parameter are removed.

### 7.2 `bladed.src`

- The tree branch is `!use_on.item and !use_on.mobile and FindHarvestTile("wood", ...)`. The item/mobile guard keeps a blade used on a tree-graphic item or a creature out of kindling.
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

---

## 8. Fishing - Magic Fish Above 129 Skill

`RandomFish()` in `fishing.inc` rolled `Random(skill) + 1` and set a fish type for rolls under 50, under 100 and under 130. A roll of 130 or more left `fishtype` uninitialized, so with 150 Fishing about 21 casts in 150 (1 in 7) passed an unset type to `createfish()` and gave nothing. The last branch is now a plain `else`: every roll of 100 or more gives a magic fish (`0xA368` + `Random(9)`). `fishingnet.src`'s magic fish roll uses the same function.

---

## 9. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `scripts/include/harvesttiles.inc` | 4, 5 | New - `FindHarvestTile()`, `HarvestTileAt()`, `HarvestTileListed()`, `IsWater(where, realm)`, and the list predicates `IsTree`, `IsOreLand`, `IsOreStatic`, `IsSand`, `IsSwamp`, `IsFishLand`, `IsFishStatic`, `IsWaterLand`, `IsWaterStatic` |
| `scripts/include/itemutil.inc` | 4 | Includes `include/harvesttiles`; `IsMinable`, `IsSand`, `IsSwamp`, `IsTree`, `IsWater` removed |
| `pkg/std/lumberjacking/lumberjack.src` | 7.1 | `FindHarvestTile("wood")` gate; objtype branch, z-search fallback, yucca check and `info` removed |
| `scripts/items/bladed.src` | 7.2 | Kindling branch uses `FindHarvestTile("wood")` on non-item, non-mobile targets; z-search and yucca check removed |
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
| `patchnotes/developer-changelog-v1.1.6.md` | - | New - this file |
| `patchnotes/patch-v1.1.6.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.6 player notes |

---

## 10. Risk and Regression Notes

- **Not compiled at commit time.** Every script that includes `itemutil.inc` picks up `harvesttiles.inc`, so a full `ecompile -A` is the safe build. Watch for a script that defined its own `IsTree`, `IsSand`, `IsSwamp` or `IsWater` (a duplicate function error); none was found by search.
- **`scripts/items/pitcher.src:52` still calls `IsWater(where)`** with no realm. It works (the realm defaults to Britannia) but reads the wrong map in other lands. A one-argument follow-up.
- **Region configs are read at server start.** The new `wood.cfg`, `ore.cfg`, `sand.cfg`, `clay.cfg` and `fish.cfg` lists need a restart (or a resource reload) to match the scripts. Until then `GetHarvestDifficulty()` checks the old lists and `FindHarvestTile` can refuse newly added tiles.
- **Shore fishing is the visible change.** About 165,000 shoreline cells stop being fishable. Expect questions from fishers who stood on the beach and targeted the first water square; casting further out works.
- **Cave floor tiles `0x24A-0x26D` now give ore.** Some dungeon areas that gave nothing or clay now give ore. Ore regions in `ore.cfg` were not changed, so difficulty and ore type follow the existing regions.
- **Troughs, tubs, barrels, buckets and fountains as map statics** now fill pitchers. Alryc has not explicitly confirmed this part of the water list (it stands at Claude's suggestion).
- **`bladed.src` kindling refuses items.** A tree placed as an item (house decoration) no longer gives kindling; before, its graphic matched `IsTree`.
- **Multis are ignored** (`ITEMS_IGNORE_MULTIS`): trees and water that are part of a house or boat multi don't count.
