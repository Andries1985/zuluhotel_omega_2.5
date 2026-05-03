# Developer Changelog — v1.0.0
**Range:** `8b0de10` (Finalising plan for Omega Cache) → `HEAD` (`fab0488`)  
**Branch:** Test-Shard  
**Date:** 2026-05-02  
**Commits in range:** 292  
**Files changed:** 522 | +28,520 / -21,503

---

## Table of Contents

1. [Omega Cache — Core System](#1-omega-cache--core-system)
2. [Omega Cache — Crafting Integration](#2-omega-cache--crafting-integration)
3. [Omega Cache — Code Review & Bug Fixes](#3-omega-cache--code-review--bug-fixes)
4. [Omega Cache — Testing Feedback Rounds](#4-omega-cache--testing-feedback-rounds)
5. [Omega Cache — Gump Polish & Lease Fixes](#5-omega-cache--gump-polish--lease-fixes)
6. [Omega Cache — Post-RC Security & Data Integrity](#6-omega-cache--post-rc-security--data-integrity)
7. [Omega Cache — Post-Merge Hardening (v1.3)](#7-omega-cache--post-merge-hardening-v13)
8. [Talisman System](#8-talisman-system)
9. [Tracking System Overhaul](#9-tracking-system-overhaul)
10. [Tooltip / Cliloc Overhaul](#10-tooltip--cliloc-overhaul)
11. [Loot System](#11-loot-system)
12. [Tamed Pet AI & Pet Mechanics](#12-tamed-pet-ai--pet-mechanics)
13. [Banker NPC System](#13-banker-npc-system)
14. [Bard Skill / Song Rebalance](#14-bard-skill--song-rebalance)
15. [Begging Restrictions](#15-begging-restrictions)
16. [Housing & Secure Containers](#16-housing--secure-containers)
17. [Power Hour](#17-power-hour)
18. [Christmas Gifts System](#18-christmas-gifts-system)
19. [Crafting — Alchemy & AlchemyPlus](#19-crafting--alchemy--alchemyplus)
20. [Crafting — Blacksmithy / Tinkering / Tailoring / Carpentry](#20-crafting--blacksmithy--tinkering--tailoring--carpentry)
21. [Crafting — Crafter Boost / Recharge Flasks](#21-crafting--crafter-boost--recharge-flasks)
22. [Item Identification (ItemID)](#22-item-identification-itemid)
23. [Player Vendors / Merchant NPCs](#23-player-vendors--merchant-npcs)
24. [Spell Changes](#24-spell-changes)
25. [RPer / Class System](#25-rper--class-system)
26. [Skill Cap System](#26-skill-cap-system)
27. [Combat & Hit Scripts](#27-combat--hit-scripts)
28. [NPC Spawn / Townspeople](#28-npc-spawn--townspeople)
29. [Treasure Maps](#29-treasure-maps)
30. [Miscellaneous / Config Changes](#30-miscellaneous--config-changes)
31. [New Commands & GM Tools](#31-new-commands--gm-tools)
32. [Config Files Changed](#32-config-files-changed)

---

## 1. Omega Cache — Core System

**New Package:** `pkg/opt/omegacache/`

The Omega Cache is a new player-housing feature that provides a large-capacity, DataFile-backed crafting storage container. It replaces the old `homecollector.src` entirely.

### New Files Created

| File | Purpose |
|------|---------|
| `pkg/opt/omegacache/omegacache.src` | Entry point — intercepts double-click, opens gump |
| `pkg/opt/omegacache/omegacache.inc` | Core library — ~6,000 lines; all gump logic, deposit, withdraw, DataFile management, lease system, category handling |
| `pkg/opt/omegacache/placecache.src` | Placement deed handler — orientation selection (South/East), lazy slot init |
| `pkg/opt/omegacache/destroycache.src` | Destroys the cache container and cleans DataFile on remove |
| `pkg/opt/omegacache/cacheinsert.src` | `OnInsertScript` — intercepts drag-and-drop deposits, validates eligibility |
| `pkg/opt/omegacache/categories.cfg` | 1,436-line category map — objtype ranges → category names, icons, and icon colors |
| `pkg/opt/omegacache/blacklist.cfg` | Objtypes that must never enter the cache (non-stackable, exploit risk) |
| `pkg/opt/omegacache/stacking_ignore.cfg` | CProps ignored for cache key identity (e.g. `BackPackXYZ`, `IDed`, `#SecureRemove`, `fromLoot`) |
| `pkg/opt/omegacache/itemdesc.cfg` | Container definition: type `Container`, `Gump`, `OnInsertScript`, `DoubleclickRange 4`, `MaxItems/MaxSlots` |
| `scripts/textcmd/player/cache.src` | Player command `.cache` — list, withdraw, deposit, autodraw toggle, GM dump |

### Files Deleted / Replaced

| File | Reason |
|------|---------|
| `pkg/opt/omegacache/homecollector.src` | Removed — entire 4,372-line old system replaced |
| `pkg/opt/omegacache/collector.cfg` | Replaced by `categories.cfg` |
| `pkg/opt/omegacache/PLAN.md` | Moved to `pkg/opt/omegacache/docs/PLAN.md` |

### Core Architecture

- **DataFile-backed storage** keyed by house serial. Each item stored as a DataFile element with properties: `objtype`, `color`, `amount`, `quality`, `BaseName`, and any non-default CProps.
- **Key identity** built via `BuildItemKey` / `BuildDefaultKey` using `MD5Encrypt` of non-default properties (CProps not in `stacking_ignore.cfg`). Items with only ignored-CProp differences merge into the same entry.
- **Lease system** (`RL#` prefixed keys): `CreateLease`, `ExtendLease`, `ReleaseLease`, `GetLeasedAmount`. Prevents concurrent crafters from over-drawing the same stock. TTL-based; `BuildCategoryMap` sweeps expired leases on every gump open.
- **Category display**: config-ordered (Crafting → Mage → General → Special → Miscellaneous → Other). Items sorted alphabetically using `BaseName` when present.
- **Deposit All confirmation gump** (`YesNoVar`) before bulk deposits.
- **Drag-and-drop deposit** via `OnInsertScript` with `sleepms(50)` delay (avoids server crash from `DestroyItem` during transitional state).
- **Orientation selection** at placement uses standard UO `SelectMenuItem2` menu (South / East).
- **Housing integration**: cache slot counts (`numomegacache` / `maxnumomegacache`) displayed on house sign. Lazy-initialised on first deed placement.
- **House demolition**: `sign.src` explicitly searches `house.items` for `OMEGACACHE_OBJTYPE` before `DestroyMulti` and destroys them; `houseserial` CProp erased first to prevent `DestroyScript` from re-crediting slots.
- **Autodraw toggle**: player command `.cache autodraw` stores `omegacache_no_autodraw` CProp. Disables automatic fallback to cache during backpack crafting. Targeting cache directly still works.

---

## 2. Omega Cache — Crafting Integration

**New File:** `scripts/include/resourcemanager.inc`  
This is the central crafting-cache integration library. All 8 crafting scripts include it.

### ResourceRequest Struct

```
struct {
  key              // DataFile element key (0 = backpack-only)
  objtype
  color
  quality
  amount
  dataFileHandle   // handle to opened cache DataFile
  houseSerial      // for partial-consume logging
  leaseKey         // set by LeaseResource
  preferredSourceOrder  // array: {BACKPACK, OMEGA_CACHE} or {OMEGA_CACHE, BACKPACK}
}
```

### Key Functions in `resourcemanager.inc`

| Function | Behaviour |
|----------|-----------|
| `MakeBackpackRequest(who, item)` | Builds a backpack-first `ResourceRequest`; sets `dataFileHandle`/`houseSerial` from nearest accessible cache |
| `SelectMaterialFromCache(who)` | Opens cache material gump; returns cache-first `ResourceRequest` |
| `SelectMaterialFromList(who, access, valid_keys)` | Variant-aware material picker with pagination (12 items/page), alphabetical by `BaseName` |
| `GetAvailableResource(who, resourceRequest)` | Returns `{backpack, cache, total}` struct; respects autodraw toggle; excludes own lease from cache count |
| `ConsumeResource(who, byref resourceRequest, amount)` | Drains from backpack first then cache per `preferredSourceOrder`; detects partial-consume and emits structured `SysLog` |
| `LeaseResource(who, byref resourceRequest, amount, ttl)` | Creates/extends cache lease; skips if `key=0` (backpack-only); leases only the shortfall beyond backpack stock |
| `ExtendResourceLease(byref resourceRequest, who, amount, ttl)` | Extends TTL; supports late creation for autodraw paths; returns success when no lease exists (backpack-only OK) |
| `ReleaseResourceLease(byref resourceRequest)` | Deletes lease element; clears `leaseKey` on struct |
| `PromptBulkAmount(who, resourceRequest, max)` | Shows text-entry gump for bulk creation (shafts, arrows, bandages); only when cache is involved; retries on invalid input |
| `WithdrawItem(df, key, amount, exclude_lease_key)` | Caps withdrawal to `raw_qty - total_leased + own_lease`; protects concurrent crafters |

### Per-Script Integration

#### `pkg/std/blacksmithy/make_blacksmith_items.src`
- `SelectMaterialFromCache` cache entry point for ingots and bone materials
- `MakeBlacksmithItems(character, ingotRequest)` / `MakeBoneItems(character, ingotRequest, boneRequest)` — full lease lifecycle (LeaseResource → loop → ExtendResourceLease → ReleaseResourceLease)
- `CanMake`/`CanMakeBone` use `GetAvailableResource` instead of `.amount`
- `ReserveItem` moved after cache check (was before — locked container for all players)
- Availability re-check inside loop body before skill check (exploit prevention)

#### `pkg/std/tinkering/tinkering.src`
- Cache entry point via `OMEGACACHE_OBJTYPE` check before `ReserveItem`
- `HandleCacheMaterial` routes to correct path by objtype (logs→wood, ingots→metal, obsidian→totem, glass/clay, axle→complex, bottles→potion keg)
- `TryToMakeComplex` / `MakeTotem` / `TryToMakePotionKeg` all retrofitted with optional `resourceRequest` param (no parallel `FromCache` functions)
- `TryToMakeTalisman` rewritten: cache-aware availability (`GetAvailableResource`), PHC halving (125→63), consume-before-skill-check, removed internal `ReserveItem/ReleaseItem`, removed dead `needed_objtype` block
- All `SubtractAmount`/`DestroyItem` on materials replaced with `ConsumeResource` (gems, trap potions, potion keg bottles, obsidian golem)
- `SetTrap` supports cache targeting for trap materials

#### `pkg/std/tailoring/make_cloth_items.src`
- Full lease lifecycle for hides/cloth
- `MakeBackpackRequest` + `ConsumeResource` throughout; `PromptBulkAmount` for bandage bulk creation
- `scissors.src` — bandage multiplier path reverted to backpack-only (random 1-3x multiplier incompatible with exact amount prompt)

#### `pkg/std/carpentry/carpentry.src`
- Dual-material routing: global `logRequest` + `secondaryRequest` for logs and secondary (ingots/cloth/young oak)
- `MakeAndProcessMenu` supports cache for secondary log selection
- Dual independent leases; `ConsumeResource` for both materials
- `MakeYoungOakStaff` retrofitted (deleted `MakeYoungOakStaffFromCache` parallel function)
- `IsLogObjtype`/`IsIngotObjtype`/`IsClothObjtype` helpers added

#### `pkg/std/alchemy/alchemy.src`
- `CanMake`, `GetBottle`, `TryToMakePotion` all take optional `resourceRequest` / `parentRequest` params
- `GetBottle(conts, user, parentRequest)` — inherits `dataFileHandle` + `houseSerial` from parent; falls back to `FindAccessibleContainer`
- Lease lifecycle added to main loop; autodraw for bottle fallback
- Parallel `CanMakeFromCache`/`GetBottleFromCache`/`TryToMakePotionFromCache` functions deleted (merged back in)

#### `pkg/std/inscription/inscription.src`
- Global `scrollRequest` set on cache target; lease lifecycle in `CreateScroll` loop
- `ReleaseResourceLease` on ALL early exits (mana depleted, scrolls exhausted)

#### `pkg/std/cartography/cartography.src`
- Global `mapRequest` for blank maps; `ConsumeMap(who, blank, amount)` routes to `ConsumeResource` or `SubtractAmount`
- `MakeBackpackRequest` for autodraw on backpack-targeted maps

#### `pkg/std/cooking/cooking.src`
- `cooking_cache_df` + `cooking_cache_house_serial` module vars set on cache target
- `check_for_all_ingredients` falls back to cache via `GetStoredAmountByObjtype`
- `destroy_all_ingredients` consumes backpack-first, cache remainder via `ConsumeResource`
- Tracks `houseSerial` across all three resolution paths

#### `scripts/items/bladed.src` (Bowcraft/Carving)
- Full cache integration: `CarveLogsFromCache` (shafts/kindling/bows), `MakeArrowsFromCache` (fire/ice/thunder)
- `specialRequest` lease lifecycle for reagent materials on special bows
- Duplicate `AutoLoop_finish()` inside loop removed (replaced with `break`)
- `PromptBulkAmount` for shafts/kindling/arrow bulk creation

#### `scripts/items/fletch.src`
- Feathers support cache via `SelectMaterialFromCache` or autodraw
- Shafts converted to `GetAvailableResource`/`ConsumeResource` with autodraw
- `PromptBulkAmount` when either material is cache-involved
- Fixed `SelectMenuItem2` case sensitivity ("fletching" → "Fletching")
- Stack overflow fix: capped to 60,000 when cache total exceeds that

#### `pkg/std/cooking/grinding.src`
- `PromptBulkAmount` with adjusted batch consumption

---

## 3. Omega Cache — Code Review & Bug Fixes

**Key file:** `pkg/opt/omegacache/omegacache.inc`, `scripts/include/resourcemanager.inc`

### Critical Bugs Fixed (Code Review Pass)

1. **`GetStoredAmountByObjtype` missing `exclude_lease_key` param** — Caller's own lease was double-subtracted from availability; crafting loops broke after 1 iteration. Fixed: added `exclude_lease_key := 0` default param.

2. **Inscription missing lease release on early exits** — Orphaned leases blocked cache resources until TTL. Fixed: `ReleaseResourceLease` on every `return` path inside `CreateScroll`.

3. **Missing `quality` field on `ResourceRequest`** — Silently lost quality from cache items. Fixed: `quality` field added to all struct creation sites including inline structs in 5 crafting scripts.

4. **Missing default color fallback** — Items stored with default color returned `color=0`, breaking material color on crafted products. Fixed: fallback to `GetItemDescriptor(objtype).Color`.

5. **eScript `.+` operator quirk** — `LeaseResource` used `.+leaseKey` which silently fails if the member already exists. Fixed: plain `.leaseKey` assignment throughout.

6. **`ExtendResourceLease` returned 0 for backpack-only paths** — Caused all backpack-only crafting loops to abort after 1 iteration. Fixed: returns `1` (success) when no lease exists.

7. **`ReserveItem` on shared cache container** — Locked the container for all players. Fixed: cache check moved before `ReserveItem` in all crafting scripts.

8. **`MD5Encrypt("")` returns error in POL** — Empty strings passed to `BuildDefaultKey`/`BuildItemKey`. Fixed: sentinel `" "` (space) used instead of empty string.

9. **`MakeAndProcessMenu` byref corruption in Carpentry** — `cacheRequest` struct passed byref then overwritten with physical item. Fixed: copy variable passed instead.

10. **`ConsumeResource` partial-consume detection** — Added structured `SysLog` entry on partial failure: `[ConsumeResource] PARTIAL_CONSUME player_serial=... player_name=... objtype=... requested=... unfulfilled=... consumed_backpack=... consumed_cache=... color=... key=... house_serial=...`. Player sees generic "contact staff" message only.

---

## 4. Omega Cache — Testing Feedback Rounds

**v1.0 → v1.1 → v1.2 → v1.3 across ~140 commits**

### Gump & UI Fixes (omegacache.inc)
- Category ordering: config-defined order vs. dictionary key order
- Category button ID fix: used `display_count` (non-empty only) with `cat_display_order` mapping array
- Item list alphabetical sorting with `BaseName` when present
- `SpellID` and `BaseName` hidden from item variant suffix display
- Category icon colors: `IconColors` config section in `categories.cfg`
- Book categories use actual spellbook graphics/colors from `itemdesc.cfg`
- Withdrawal button layout: target column = blue dot (2362), backpack = golden triangle (2436/2437)
- Item name padding: x=68→75; category icon padding: x=55→70

### Container & Placement (placecache.src, itemdesc.cfg)
- Container graphic: `0x0E43` → `0x2DF4`, hue 2032
- Orientation selection replaced with standard UO `SelectMenuItem2` menu
- `placecache.src` lazy-init for slot counts on first placement

### Categories Config (categories.cfg) — Major Additions
- Missing items: `0x0C70` (Lettuce), `0x1727` (Dates), fish (`0x0DD6-0x0DD9`), tinkering components
- Cooking items: variants, bowls, pies, cakes, pizzas, bread, bacon, cheese, sausage, donuts, jerky
- Raw ingredients: dough, batter, flour, corn
- 68 AlchemyPlus potions (`0xFF4E-0xFF95`, `0xFFA2`)
- 8 talisman gems (`0x213F-0x2146`)
- 9 fishing shells, 10 verse book scrolls
- Candlemaking materials (beeswax, pot of wax, dipping stick)
- Bloody bandages, missing fish variants
- 37 mage-variant potions (`0xFF19-0xFF40`)
- Wheat sheaf; Raw food items moved from Food → RawFoodAndHerbs; category renamed

### Lease System Hardening
- Autodraw lease key resolution via prefix scan in `LeaseResource` when `key=0`
- Late lease creation in `ExtendResourceLease` for autodraw key resolution
- `ConsumeFromCache` refactored to take `ResourceRequest` byref — sets `resourceRequest.key` on resolution
- Lease creation validates unleased stock before creating (prevents over-reserving)
- `LeaseResource` and `ExtendResourceLease` now only lease `max(0, material - backpack_amount)` — shortfall only; recalculated each iteration

### stacking_ignore.cfg (new)
- CProps excluded from cache key identity: `BackPackXYZ`, `IDed`, `#SecureRemove`, `fromLoot`
- Items differing only by these CProps merge into same cache entry
- Ignored CProps stripped on deposit, not restored on withdrawal

### canstack.inc (`scripts/include/canstack.inc`)
- Added `item.inuse` check
- Added `stacking.cfg` `IgnoreCprops` filtering to match POL core `can_add_to_self()` behaviour

---

## 5. Omega Cache — Gump Polish & Lease Fixes

*(Milestone 2.3b — 2026-03-27)*

See section 4 for full list. Key items:

- **`RecreateItem` restores display name** via `SetName` when `BaseName` CProp present
- **`SelectMaterialFromList` sort fix** — replaced manual substring loop with `Find()` for `|||` separator; manual loop failed silently causing empty item lists on page 2+
- **Material selection category pagination** — 12 categories/page (was unbounded)
- **`WithdrawItem` lease-aware** — caps withdrawal to `raw_qty - total_leased + own_lease`

---

## 6. Omega Cache — Post-RC Security & Data Integrity

*(2026-04-11 → 2026-04-15)*

### `ValidateDepositTarget(who, access, tgt)` — new function in `omegacache.inc`
Centralised deposit gating. Checks:
- Player is in cache's house
- Target is accessible and within 2 tiles (via top-level world object for nested items)
- Target is in player's backpack or same house
- Secured container permissions checked by walking container chain for `USESCRIPTID_SECURE_CONTAINER`

### `RunOmegaCacheGump(who, access)` signature change
- Previously `(who, df)`. Now takes full `access` struct. `df` extracted internally.
- `DoDepositTargeting` and `DoDepositAll` also changed to take `access` instead of `df`

### `USESCRIPTID_SECURE_CONTAINER` constant
- Replaced string literal `":housing:securecont"` with constant
- Added include to `sign.src`, `signcontrol.src`, `ssign.src`

### Deposit All confirmation
- `DoDepositAll` shows `YesNoVar` gump before bulk deposit

### Drag-and-drop deposit (`cacheinsert.src`)
- `itemdesc.cfg` changed from `Item` to `Container` type
- `OnInsertScript` intercepts drops; validates via `DepositSingleItem`; returns ineligible items to backpack
- `sleepms(50)` delay avoids server crash (item in transitional state during drop)
- `DoubleclickRange 4` set (matches `.cache` command range)

---

## 7. Omega Cache — Post-Merge Hardening (v1.3)

*(2026-04-17)*

### AlchemyPlus Cache Integration (`pkg/opt/alchemyplus/alchemyplus.src`)
- Module-level `primary_cacheRequest` set when player targets cache container at burner
- `BuildReagentRequests`: builds one `ResourceRequest` per recipe reagent; PHC/mage bonus pre-adjusted; basicpot resolves to leveled variant from backpack then cache
- `CanMakePotion` + `destroy_all_reagents` accept pre-built `reagentRequests` array
- Container (flask/bottle) resolved via cache-aware `containerRequest`
- Lease lifecycle added to main loop; all `return` inside loop changed to `break` for clean release
- Duplicate `AutoLoop_finish()` inside loop removed
- Potion loop reordered: **create → consume container → find next** (was consume → find next → create — caused lost potions on last iteration)

### `houseSerial` Field Threading
- `ResourceRequest` struct gains `houseSerial` field
- `MakeBackpackRequest` and `SelectMaterialFromList` populate from `access.house.serial`
- `SelectMaterialFromList` signature changed from `(who, df, valid_keys)` → `(who, access, valid_keys)`
- Inline struct sites carry `houseSerial`: tinkering (`tempRequest`/`secondRequest`/`cacheBottleReq`), bladed (`cacheHideReq`), alchemy (`bottleReq`), cooking (`cacheRequest`)

### Omega Cache Token Count
- `fab0488`: Cache token limit set to **50 tokens**

---

## 8. Talisman System

**New Package:** `pkg/opt/talisman/`

### Files Created

| File | Purpose |
|------|---------|
| `pkg/opt/talisman/pkg.cfg` | Package registration |
| `pkg/opt/talisman/config/icp.cfg` | Item class properties for talisman gems |
| `pkg/opt/talisman/config/itemdesc.cfg` | Talisman item definitions (169 lines) |
| `pkg/opt/talisman/include/talismanid.inc` | Talisman identification logic (200 lines) |
| `pkg/opt/talisman/talisman/method.src` | Method hook |
| `pkg/opt/talisman/talisman/use.src` | Use hook — equip/apply talisman effect |

### `pkg/std/tinkering/tinkering.src`
- `TryToMakeTalisman` added — requires 125 iron ingots (63 under PHC) + talisman base
- Cache-aware: `MakeBackpackRequest` for backpack path, `SelectMaterialFromCache` for cache path; `GetAvailableResource` availability check
- Consume-before-skill-check pattern; no internal `ReserveItem/ReleaseItem`
- PHC halving: `if(GetGlobalProperty("PHC") || GetObjProperty(character, "#PPHC")) material := CInt(Ceil(material/2))`
- `tinker.cfg`: talisman recipes added (55 lines of new entries)

### `pkg/opt/talisman/include/talismanid.inc`
- `fix: honor caller talisman delay` — removed forced delay override so callers control timing
- `fix: remove duplicate ReserveItem call` in `TalismanID`
- Talisman ID review feedback addressed

### `pkg/opt/crafterboost/make_crafter_boosts.src`
- Updated to support talisman-related crafter boosts (107-line change)

### `pkg/opt/crafterboost/rechargeflask.src` (new, 88 lines)
- New script for recharging crafter flasks

### `scripts/include/itemutil.inc`
- Added talisman gem objtype constants and helper functions (60-line addition)

### `scripts/include/objtype.inc`
- Added ~49 new talisman and gem objtype constants

---

## 9. Tracking System Overhaul

**File:** `pkg/std/tracking/tracking.src` — ~200 line change

### What Changed
- **Config source changed**: `ReadConfigFile("tracking")` (old `tracking.cfg`) → `ReadConfigFile("::npcdesc")`. Tracking now reads NPC templates directly from `npcdesc.cfg`.
- **`mobile.graphic`-based lookup replaced** with `mobile.npctemplate`-based lookup. NPCs without a template are skipped.
- **Tracking menu categories** replaced with a dynamic type system. Old hardcoded: Animal / Monster / Person / Ethereal. New: 20+ types including Animated, Beholder, Champion, Daemon, Dragonkin, Elemental, Gargoyle, Giantkin, Human, Miscellaneous, Ophidian, Orc, Plant, Ratkin, Slime, Terathan, Troll, Undead — all with dedicated graphics icons.
- **Boss/Champion detection** normalised: any NPC with `Boss`, `SuperBoss`, `Champion`, or `LesserBoss` CProp tracks as "Champion" category.
- **`tracking.cfg` deleted** — 1,438 lines removed (this was the old config).
- **New helper functions added:**
  - `CategoryIndex(categories, value)` — finds category position
  - `NormalizeTrackingType(value)` — case-normalises type string from CProp
  - `GetTrackingTypeIcon(value)` — returns tile graphic for each type category
- Text above player on track start: `"You begin to track nearby creatures."`
- Tracking counter and timeout reset on each new track attempt

### `pkg/opt/shrink/textcmd/test/shrink.src`
- Shrink type mapping: naming consistency polish
- `createinbag` safety fix
- Tracking menu cancel index validation fix

---

## 10. Tooltip / Cliloc Overhaul

### `pkg/packethooks/megacliloc/itemdata.src`

- **Spawn chest protection** — `IsLockedSpawnPointChest(xObject)` check at top: returns empty props for locked spawn chests (no information leak before unlocking)
- **Talisman ID charges** — `idcharges` CProp displayed as `Charges: ~1_VAL~` (cliloc 1060741)
- **AR tooltip updated** — Armor with no `Coverage` config field shows `AR: actual (max_at_150)` format — e.g. `"32 (45)"` showing current effective AR vs. max potential. Armors with Coverage (zone-specific) show only current value.
- **DPS formula fixed** — Removed HP% damage reduction factor from `Calcdamage`. Previously weapons at low HP showed misleadingly low DPS in tooltip.
- **Can no longer ID locked spawn chests** — `itemdata.src` returns early before revealing any properties.

### `pkg/packethooks/megacliloc/mobiledata.src` — 112-line change

- **Staff tooltip for NPCs** — Consolidated and expanded. Now shows:
  - `Str: X HP: current/max`
  - `Int: X MP: current/max`
  - `Dex: X Stam: current/stam`
  - `Loot Grp: X Lvl: X Chance: X%` — pulled from `CustomLoot` CProp or `npcdesc.cfg`
  - All elemental resistances displayed if non-zero (Fire, Water, Earth, Air, Necro, Holy, Physical)

### `pkg/packethooks/packethook/packethook.src` — 47-line change
- Updated packethook registration and command level cliloc display

### Command Level Cliloc Changes
- `b674d56`, `f5b7a86`, `7c04901` — Multiple rounds of command level cliloc corrections for staff-accessible commands
- `config/command_synopses.cfg` — New file, 2,074 lines — full command synopsis documentation

---

## 11. Loot System

### `config/nlootgroup.cfg` — 575-line net change
- Loot levels restructured: proper level 10 tier added; all loot increments updated
- Shields moved to own loot functions with per-skill proc chances
- Dragon armor added to loot groups
- New clothing and guild clothes in enchantables loot group
- Nyx and Rikktor moved to **Tier 11** loot
- New items added with appropriate loot drops and restricted class groups

### `config/npcdesc.cfg` — 5,981-line change (large restructure)
- NPC template entries updated to use `Type` property consistently for tracking
- Loot group corrections and level assignments across many NPC types
- City/region data updated for townsfolk spawning

### `pkg/systems/combat/config/enchantableitems.cfg` — 119-line change
- Dragon armor added to enchantables
- New clothing items added

### `pkg/systems/combat/config/itemdesc.cfg` — 559-line change
- New items: dragon armor pieces, kimonos, kamishimo, new blacksmith items
- Talisman-related items added

### `pkg/opt/lootlottery/include/lootlottery.inc`
- 3-line addition supporting new loot tier

### `config/mrcspawn.cfg`
- 4-line update for spawn adjustments

---

## 12. Tamed Pet AI & Pet Mechanics

**Files:** `scripts/include/` AI-related files, `config/npcdesc.cfg`

### Boss/Super Boss Pet House Confiscation
- When a Boss or SuperBoss tamed pet enters a player house, it is **automatically confiscated**
- Multiple fix iterations (`384f241`, `c4ef1dc`, `612373b`)

### Tamed Pet Count Fix (`ba2ac09`)
- C5 players: can tame 2 boss pets + 1 mount
- C6 players: can tame 2 boss pets + 2 mounts
- Previously these counts were miscalculated

### Tamed AI Updates (`cd78bb2`, `0b21453`, `330b72b`, `ef1365b`, `a57f8bf`)
- Tamed casters maintain distance of 2–10 tiles from target
- First AI loop: automated defense triggers immediately for magic/summons/animated pets
- Release fix for mass spell targets on tamed pets
- Fix for tamed NPCs casting special mass spells (AOE restriction)
- ArmorZone inc errors fixed

### Poison Skill Threshold (`ba2ac09`)
- `poisoningskill` adjusted on specific NPC templates so tamable pets hit correct poison skill thresholds for AI decision points

### Tamed Magic/Summons/Animated pets
- On first loop in idle state, will now perform automated defense rather than waiting

---

## 13. Banker NPC System

**Multiple commits:** `db2fdc0`, `1c010bb`, `6aba1cb`, `3f3bc98`, `9599f1d`, `d27a7c4`, `c7d8f22`, `465d876`, `738bcb0`

### `scripts/include/` and banker-related scripts — Changes
- **Bankers now only accept cheques** to be given to them (not arbitrary items)
- **Cheque box validation** — only accepts numeric input
- **Deposit confirmation gump** — banker shows confirmation before processing deposit command
- **Banker speech queue fix** — race condition in speech processing resolved
- **Banker give item fix** — item transfer logic corrected
- **Removed bankers from begging list** — bankers are now excluded as begging targets
- **Player Merchants can give cheques to players**
- **Banker cheque, deposit, and withdraw functions** fully implemented (`db2fdc0`)
- **Warrior for hire healing fix on res** — healing triggered correctly after resurrection

---

## 14. Bard Skill / Song Rebalance

**Files:** `pkg/opt/songbook/songofdefense.src`, `pkg/opt/songbook/songofglory.src`, `pkg/opt/songbook/songofhaste.src`

### `songofdefense.src`
- `dmod` formula changed from `RandomInt(5) + peace/10 + spec*3` → `CInt(spec * 9)`, capped at 54
- Duration: 1800s → 3600s

### `songofglory.src`
- `dmod` changed to fixed tier table by spec level (0→0, 1→15, 2→30, 3→45, 4→60, 5→65, default→75)
- Added `CanMod(cast_near, "poly")` check before applying ebless
- Added Str and Int buff paths (was missing them in certain conditions)

### `songofhaste.src`
- Same fixed tier table: `dmod` by spec level (0→0, 1→15, 2→30, 3→45, 4→60, 5→65, default→75)
- Duration: 1800s → 3600s
- Bard buff normalization commit (`f588f97`) also fixed checkpoint infinite loop

### `pkg/opt/shilhook/omegaattack.inc`
- 30-line change; fire katana now correctly applies fire damage (`f6d8da2`)
- Taint buff changed to 75; Megos buff adds 5 minutes with no random roll

---

## 15. Begging Restrictions

**File:** `pkg/std/begging/begging.src` — 29-line change

### New Restrictions Added
- **Cannot beg while inside a multi (house)** — `character.multi` check added
- **Cannot beg from NPCs inside a multi** — `who.multi` check added
- **Expanded excluded NPC list**: now blocks begging from:
  - Warrior for hire
  - Player vendor
  - Healer
  - High Priest
  - Vanity Vendor
  - Architect
  - Title Master
  - Banker (added separately)
- Each excluded type now gives a **specific rejection message** rather than generic

---

## 16. Housing & Secure Containers

**Files:** `pkg/std/housing/sign.src` (329-line change), `pkg/std/housing/signcontrol.src` (69-line change), `pkg/std/housing/utility.inc` (35-line addition), `pkg/opt/statichousing/ssign.src`

### `sign.src`
- House sign gump updated to display **Omega Cache container count** (`numomegacache` / `maxnumomegacache`)
- Lazy initialisation: calls `AssignDefaultContainers` and `GetMaxProps` if slot data missing
- `USESCRIPTID_SECURE_CONTAINER` constant used instead of string literal
- Explicit `house.items` search for `OMEGACACHE_OBJTYPE` before `DestroyMulti` — destroys cache containers on demolition; erases `houseserial` CProp first to prevent slot re-credit

### `signcontrol.src`
- `USESCRIPTID_SECURE_CONTAINER` constant added

### `utility.inc`
- 35 new lines; helper functions for housing utility operations

### Death-on-container fix (`42306fe`)
- Possible fix for dying while standing on a container inside your house

### `d7f602`: Persons dress fix
- NPCs no longer drop equipped items on death

---

## 17. Power Hour

**File:** `pkg/opt/powerhour/powerhour.src` — 27-line change

### Sunday Bonus Power Hour Logic
- `randomPH := Random(3)+1`
- If `randomPH == 2` (Sunday bonus resource PH): second bonus PH chance = `Random(2000)` — **higher chance** (1/2 base) vs. normal `Random(10000)` (1/10 base)
- This makes Sunday bonus power hours significantly more likely to trigger a second PH

### `063be4e`: Power hour change
- If the Sunday bonus PH is "resource" type, the probability of hitting the 2nd bonus power hour is increased

---

## 18. Christmas Gifts System

**Files:** `pkg/opt/christmas/Christmasgifts.src` (64-line change), `pkg/opt/christmas/giftopen.src` (1,806-line change)

### `Christmasgifts.src`
- **Configurable cooldown**: `DEFAULT_GIFT_COOLDOWN := 24*3600`. Global property `ChristmasGiftCooldownSeconds` overrides default
- **Time remaining display**: rejection message now shows formatted time remaining (`FormatDuration`) instead of just "once per day"
- **New helper functions**: `GiftCooldownSeconds()`, `FormatDuration(seconds)`, `PluralS(value)`
- New GM commands: `7ba3787` — set speed at which players receive gifts; `9f4421e` — cooldown change

### `giftopen.src`
- **Staff bypass**: non-staff players get random open message (1-in-3 chance to wait); staff always open immediately
- **Massive expansion** — ~1,800 lines of new gift item possibilities, updated drop rates, reduced shroud chance, increased present roll chance
- Dragon armor and kimonos added to gift table

---

## 19. Crafting — Alchemy & AlchemyPlus

### `pkg/std/alchemy/alchemy.src` — 197-line change
- Cache integration (see section 2)
- `f0910e2`: Alchemy Changes (general balance fixes)
- `4d1d546`: Update (additional fixes)
- Flask autoloop bug fix (`057eb3c`)

### `pkg/std/alchemy/bluepotion.src` — 19-line change
### `pkg/std/alchemy/whitepotion.src` — 21-line change
- Potion recipe adjustments and deterministic tier mapping wired in

### `pkg/std/alchemy/itemdesc.cfg` — 8-line change
- Item definition updates for new/adjusted potions

### `pkg/opt/alchemyplus/alchemyplus.src` — 777-line change
- Full cache integration (see section 7)
- Potion loop reordered: create → consume container → find next
- Flask autoloop bug fixed: loop no longer loses potions on last iteration
- `v1.3` Testing Feedback corrections (`832acd1`, `f6a161b`, `0dbccb3`, `0177942`)

### `pkg/opt/alchemyplus/alchemyplus.cfg` — 44-line change
- Recipe/balance adjustments for AlchemyPlus potions
- Talisman (`0xffa3`) support hooks

### `pkg/opt/alchemyplus/itemdesc.cfg` — 93-line change
- New/updated item definitions for AlchemyPlus potions; strength values updated for deterministic tiers

### `pkg/opt/alchemyplus/newpotions.src` — 144-line change
### `pkg/opt/alchemyplus/potionbook.src` — 20-line change
- New potion types and potionbook updates

### Internal Patch Notes (new file)
- `pkg/std/alchemy/docs/PATCHNOTES-alchemyplus-balance-2026-04-25.md` — 309-line balance patch notes

---

## 19a. Potion Balance Overhaul (`f0910e2`, `f6d8da2`)

**Primary files:** `pkg/std/alchemy/bluepotion.src`, `pkg/std/alchemy/whitepotion.src`, `pkg/opt/alchemyplus/newpotions.src`, `scripts/include/dotempmods.inc`, `pkg/std/alchemy/alchemy.src`, `pkg/opt/alchemyplus/alchemyplus.src`

### Design Change: Deterministic Tiers
All stat-buff, taint, mego, and homeric potions converted from `RandomDiceStr()` outcomes to fixed deterministic values. Formula:
- `rank = base_tier + ByTrueMage(mage_level)` (min 1)
- `stat_gain = rank * 10 + 5`
- `duration = (rank + 1) * 480 seconds`
- Stat potion cooldown override: 2 seconds at consume time

### DEX / STR / INT Potions (`bluepotion.src`, `whitepotion.src`, `alchemy.src`)

All three stat lines follow the same tier table. Brewed variant selected by mage level via `ByTrueMage()` mapping:

| Potion | Mage Level | Gain | Duration |
|--------|-----------|------|----------|
| Lesser | Any | +5 | 8 min |
| Standard | Non-Mage | +15 | 16 min |
| Standard | M1 | +25 | 24 min |
| Standard | M2+ | +35 | 32 min (cap) |
| Greater | Non / M1 | +45 | 40 min |
| Greater | M2–3 | +55 | 48 min |
| Greater | M4–5 | +65 | 56 min |
| Greater | M6 | +75 | 64 min |

**INT Potion names:** Lesser = *Phandel's Fine Intellect*, Standard = *Fabulous*, Greater = *Fantastic*.

### Taint Transmutations (`newpotions.src` — `DoPolyEffect`)

Deterministic poly mod by effective strength tier (1–5):

| Tier | Poly Mod | AR (floor/2) |
|------|----------|--------------|
| 1 | +10 | +5 |
| 2 | +25 | +12 |
| 3 | +40 | +20 |
| 4 | +55 | +27 |
| 5 | **+75** | +37 |

- `f6d8da2`: Tier 5 (`default` case) raised from **+65 → +75**.
- AR is computed from poly in temp-mod processing: `floor(poly_mod / 2)`.

| Potion | Mage Level | Poly Mod | AR | Duration |
|--------|-----------|----------|----|----------|
| Taint Minor | Non | +10 | +5 | 12 min |
| Taint Minor | M1–6 | +25 | +12 | 24 min |
| Taint Major | Non / M1 | +40 | +20 | 36 min |
| Taint Major | M2–3 | +55 | +27 | 48 min |
| Taint Major | M4–6 | +75 | +37 | 60 min |

### Mego / AR Protection Potions (`newpotions.src` — `DoProtectionEffect`)
- `f6d8da2`: `mod_amount := RandomDiceStr(strength + "d2")` → `mod_amount := strength * 2` (deterministic)
- `f6d8da2`: `duration := strength * 15` → `duration := strength * 15 + 300` (+5 minutes per tier)

### Homeric Might (`newpotions.src`)

| Potion | Mage Level | Bless Mod | Duration |
|--------|-----------|-----------|----------|
| Homeric | Non | +15 | 12 min |
| Homeric | M1 | +30 | 24 min |
| Homeric | M2–6 | +45 | 36 min |
| Greater Homeric | Non / M1–2 | +45 | 36 min |
| Greater Homeric | M3–5 | +60 | 48 min |
| Greater Homeric | M6 | +75 | 60 min |

---

## 19b. Buff Stacking Enforcement (`f0910e2`, `f6d8da2`)

**File:** `scripts/include/dotempmods.inc` — 102-line change

### `TempModConflicts(existing_key, incoming_key)` — new function
Called by `AddToStatMods` before applying any temp-mod. Returns 1 (block) on conflict.

**Category definitions:**
| Category | Keys |
|----------|------|
| `stat` | `str`, `cstr`, `dex`, `cdex`, `int`, `cint` |
| `bless/poly` | `all`, `call`, `ebless`, `cebless`, `poly`, `cpoly` |
| `ar` | `ar`, `car` |
| `paralyze` | `p` |

**`f0910e2` (initial):** Blocked all cross-stat combinations — `TempModIsSingleStat()` treated STR, DEX, INT as one category.

**`f6d8da2` (fix):** Per-attribute separation — conflict only fires when same attribute conflicts with itself:
```
if( (TempModTouchesStrength(e) && TempModTouchesStrength(i)) ||
    (TempModTouchesDexterity(e) && TempModTouchesDexterity(i)) ||
    (TempModTouchesIntelligence(e) && TempModTouchesIntelligence(i)) )
    return 1;
```
Result: STR+DEX+INT can all be active simultaneously; same-stat double-buff still blocked.

**Allow/Block matrix (final state):**
| Combination | Result |
|-------------|--------|
| STR + DEX, STR + INT, DEX + INT | ALLOW |
| Same stat + same stat | BLOCK |
| stat + bless/poly | ALLOW |
| stat + ar | ALLOW |
| bless + ar | ALLOW |
| poly + ar | BLOCK |
| bless/poly + bless/poly (different) | BLOCK |
| poly + ar | BLOCK (poly contributes AR already) |
| ar + ar | BLOCK |
| paralyze + paralyze | BLOCK |

### Protection Spell AR Gating
**Files:** `pkg/std/spells/protection.src`, `pkg/std/spells/protection with timer.src`, `pkg/std/spells/archprot.src`

- Pre-application check added: if any active mod key is in `{ar, car, poly, cpoly}`, spell is blocked.
- Blocked cast sends message to caster. Arch Protection silently skips already-buffed targets.
- `DoProtectionEffect` in `newpotions.src` has same pre-check (blocks Mego if AR/Poly already active).

### Debug Command: `.showbuffcats` (`scripts/textcmd/test/showbuffcats.src` — new, 105 lines)
- Staff-level command; target any mobile.
- Outputs each active `#mod` key / category / amount / seconds remaining.
- Summary line lists occupied buff categories.
- Intended for verifying stacking enforcement in-game.

---

## 20. Crafting — Blacksmithy / Tinkering / Tailoring / Carpentry

### `pkg/std/blacksmithy/make_blacksmith_items.src` — 331-line change
- Cache integration throughout (see section 2)
- Tooltip fix: new blacksmith items that aren't exceptional no longer show incorrect tooltip
- Crafters can now make mage furniture using Blacksmithy instead of Magery (`2318b4a`)

### `pkg/std/blacksmithy/blacksmithy.cfg` — 45-line change
- New entries including `Name Iron` for iron ingot; dragon armor recipes

### `pkg/std/tinkering/tinkering.src` — 834-line change
- Full cache integration + talisman (see sections 2, 8)
- `a672db6`: Removed Magery requirement from making totems
- `a2855ea`: Totem fix — only crafters can make; cannot be spellbound; no magery required

### `pkg/std/tinkering/tinker.cfg` — 57-line change
- Talisman recipe entries; totem/misc updates

### `pkg/std/tailoring/make_cloth_items.src` — 184-line change
- Cache integration; `PromptBulkAmount` for bandages

### `pkg/std/tailoring/scissors.src` — 40-line change
- Backpack-only with overflow cap (random multiplier incompatible with bulk prompt)

### `pkg/std/tailoring/itemdesc.cfg` — 673-line change
- Kimono and kamishimo added; organised structure; new clothing items

### `pkg/std/carpentry/carpentry.src` — 284-line change
- Full cache integration; tents removed (`24c5cd5`)

### `pkg/std/carpentry/carpentry.cfg` — 4-line change

### `pkg/std/inscription/inscription.src` — 102-line change
- Cache integration; lease release on all exits

### `pkg/std/cartography/cartography.src` — 90-line change
- Cache integration; `makeNewmap` create-before-consume

### `pkg/std/mining/mining.src` — 188-line change
- Zulu ore additions; talisman gem mining hooks

---

## 21. Crafting — Crafter Boost / Recharge Flasks

### `pkg/opt/crafterboost/make_crafter_boosts.src` — 107-line change
- Talisman crafter boost support added

### `pkg/opt/crafterboost/rechargeflask.src` — new (88 lines)
- New recharge flask script

### `pkg/opt/crafterboost/crafterboost.cfg` — 10-line change
### `pkg/opt/crafterboost/itemdesc.cfg` — 11-line addition
- New crafter boost item definitions

---

## 22. Item Identification (ItemID)

**File:** `pkg/std/itemid/itemid.inc` — 182-line change

### Key Changes
- **ItemID logic consolidated** into `itemid.inc` — previously scattered
- **Spawn chests cannot be IDed** before they are unlocked (`fe1dabe`, `b0eb076`)
- **Merchant ID permanently locked out fix** — `merchantid` no longer gets permanently stuck
- **Reserve item fixes** — corrected reservation logic

---

## 23. Player Vendors / Merchant NPCs

### `b567304`
- **Player vendor barker** — vendors now announce when they were last restocked (if within the past week)
- **Thief poison bandage heal doubled**
- Kraken added to guardian list for treasure maps

---

## 24. Spell Changes

### `pkg/std/spells/protection.src` + `protection with timer.src`
- Protection spell timer added (10-line addition each)

### `pkg/std/spells/archprot.src` — 17-line change
- Arch protection updates

### `pkg/std/spells/dispel.src`
- 4-line addition

### `pkg/opt/holybook/enlightenment.src` — 10-line change
### `pkg/opt/holybook/seraphimswill.src` — 4-line change
- Holy book spell adjustments

### `f219605`: Magic Absorption
- Brought magic absorption in line with magic reflection when targeting self

### `pkg/std/dundee/lifecrystal.src` — 5-line change
- Life crystal (`ee9ef32`-series): **Resurrection crystals now actually resurrect** the player instead of just adding a CProp that was never read

### Elemental Weapons
- `f501897`: Elemental weapons from pentagrams now deal **100% elemental damage**
- `75b27dd`: Water, earth, air, and shadow element weapons changed to deal elemental damage

### Dual Planar
- `30233c7`: Dual planar no longer tells you how much damage you are doing

---

## 25. RPer / Class System

**Files:** `pkg/opt/roleplaying/rperstone.src` (26-line change), `pkg/opt/roleplaying/rper.inc` (101-line addition)

### `10e6a5d`: RPer update for new classes
- RPer system updated to handle all new class types correctly

### `0937f1f`: RPer equipment for Bladesinger
- Bladesinger-specific starting/RPer equipment set updated

### `pkg/opt/guilds/include/guildconstants.inc` — 19-line change
- Guild constant updates for new class support

---

## 26. Skill Cap System

**Files:** `pkg/opt/powerscrolls/transcendscroll.src` (33-line change), `pkg/opt/powerscrolls/powerscroll.src` (4-line change)

### `876e1ff`: Skill cap enforcement
- Every time a skill changes (gain, train, transcend scroll), checks equipped gear vs max cap
- Players cannot exceed their cap based on what they have equipped

### `75bd17b`: Capper fix
- Fixed capper to use same logic and account for equipment when determining effective cap

### New Commands (`23a7903`, `221626a`)
- `pkg/opt/powerscrolls/textcmd/test/raisecaps.src` — Raise chosen skill caps (moved from admin to test)
- `pkg/opt/powerscrolls/textcmd/test/raiseallchosencaps.src` — Raise all chosen caps (131 lines)
- `pkg/opt/powerscrolls/textcmd/test/lowerallchosencaps.src` — Lower all chosen caps (131 lines)
- `pkg/opt/powerscrolls/textcmd/test/lowercaps.src` — Lower caps (102 lines)

---

## 27. Combat & Hit Scripts

### `pkg/systems/combat/include/hitscriptinc.inc` — 116-line change
- Hit script include updates for elemental damage routing
- Fire katana elemental damage applied through hit scripts

### `pkg/systems/combat/dualplanarscript.src` — 4-line change
- Dual planar damage text removed

### `pkg/items/armor/include/armorZones.inc` — 35-line change
- ArmorZone logic fixes (was producing errors, fixed in `cd78bb2`)

### `pkg/systems/crafting/include/craftingfunctions.inc` — 35-line change
- `mat_quality` extraction: type-safe field access based on struct type
- Quality fallback from ResourceRequest to config
- Crafter make mage furniture with blacksmithy fix

### Exceptional Chance
- `27bec51`: Exceptional item chance **capped at 150** skill points (was uncapped)

### HP Spawn Fix
- `287d1f2`: NPCs now spawn with less HP fix (HP value corrected on spawn)

### Armor Stats Chance
- `e9f4d44`: Armor has more chance to get stats applied to it

---

## 28. NPC Spawn / Townspeople

---

## 28a. Spawn Chest System (New)

**Primary commits:** `5cc13fe` (Elven Chest + spawnpoint logic overhaul), `b015787` (Level 7 loot fix), `f588f97` (checkpoint infinite loop fix), `c3205ae` (lockpicking review feedback)

### `pkg/opt/spawnpoint/checkpoint.src` — 123-line change

#### Activation Gate
- Spawn chests **only generate** when `pt_data[5] == 300` (wander range field). Any other value aborts chest creation silently. This is the opt-in signal in spawn config.
- Uses `PROPID_CHEST_SPAWNPOINT_CHEST` (`"SpawnPointChest"`) CProp instead of the old `PROPID_CHEST_TREASURE_CHEST` (`"TreasureChest"`). These are now distinct systems.

#### Weighted Loot Tier Roll
Chest loot group is determined by a `RandomInt(1000)` roll mapped to groups 301–307:

| Roll Range | Loot Group | Chance |
|-----------|-----------|--------|
| 0–19 | 307 | 2% |
| 20–119 | 306 | 10% |
| 120–239 | 305 | 12% |
| 240–389 | 304 | 15% |
| 390–569 | 303 | 18% |
| 570–779 | 302 | 21% |
| 780–999 | 301 | 22% |

#### Magic Level & Lock Difficulty by Tier
| Loot Group | Magic Level | Magic Chance | Lock Difficulty |
|-----------|------------|-------------|----------------|
| 307 | 10 | 80–99% | 140–150 |
| 306 | 9 | 70–99% | 120–140 |
| 301–305 | 1–9 (random) | 30–99% | 50–120 |

- `PROPID_CHEST_SPAWN_LEVEL` (`"SpawnChestLevel"`) stores `lootgroup - 300` (1–7) for display on unlock message.
- Lock difficulty stored as `PROPID_CHEST_SPAWNPOINT_LOCK_DIFFICULTY` (`"SPLockPickDiff"`) — separate from the old `PROPID_CHEST_LOCK_DIFFICULTY`.
- Old random difficulty (`RandomInt(101)+50`) replaced by tiered system above.

#### Traps
- **All spawn chests are always trapped** (removed the old `if(!RandomInt(10))` 10% chance gate — now always fires).
- Trap type: `RandomInt(3)+1` (needle / poison / explosion).
- Trap strength: `magic_level + 1` (scales with chest tier).
- `PROPID_CHEST_TRAPPED_BY` set to `"Spawnpoint"` — allows traps.src to handle spawnpoint traps as a distinct case.
- Chest `usescript` set to `USESCRIPTID_TRAPPED_CONTAINER`.

#### Item Creation & Placement
- Chest created at `DEFAULT_LOCATION_ITEM_CREATION_*` coords (safe staging location) before being moved, preventing mid-creation world placement.
- `MoveObjectToLocation` now includes `realm` parameter (fixes cross-realm chest placement bug).
- Chest set `movable := 0`, `locked := 1` after placement.
- Chest serial appended to `PROPID_SPAWNPOINT_SPAWNED_OBJECTS` on the spawn point for cleanup tracking.
- `b015787`: `magic_chance` floor for groups 301–305 lowered from 50% to 30% (wider spread).

#### Infinite Loop Fix (`f588f97`)
- Checkpoint loop bug that could cause the spawn process to hang indefinitely — fixed.

### `pkg/opt/chests/lockpicking.src` — 99-line change

#### Thief-Only Spawn Chest Lock Picking (`PickTreasureChest` — new function)
- Spawn point chests (`PROPID_CHEST_SPAWNPOINT_CHEST`) are routed to a dedicated `PickTreasureChest()` function instead of the generic `LockPickTheThing()` path.
- **Only Thieves can pick spawn chests** — non-thieves (`CLASSEID_THIEF` CProp absent) receive: *"You are not a thief!"* and are blocked.
- Uses `PROPID_CHEST_SPAWNPOINT_LOCK_DIFFICULTY` for difficulty — tiered to chest loot level (50–150).
- **Enemy proximity check**: if any hostile is within 4 tiles, the pick attempt is blocked.
- On **success**: chest unlocked, lockpick consumed, level display message (*"You have unlocked a level X chest!"*), 5-minute self-destruct timer started via `misc/deleter`.
- On **failure**: standard "unable to pick" message; second skill check determines if lockpick breaks.
- `LastPickedBySerial` CProp set to picker's serial on unlock (audit trail).
- `b015787`: Disabled legacy `SpawnTheChest()` call path (now a stub returning 0) — old chest-spawning-from-lockpick flow replaced by spawnpoint system.

### `pkg/std/removetrap/removetrap.src` — 7-line change
- `EraseObjProperty(item, "trapped_by")` added on successful disarm — previously this CProp was left orphaned.

### `pkg/std/traps/traps.src` — 8-line change
- Trap type comparison changed from string (`"1"`, `"2"`, `"3"`) to integer (`1`, `2`, `3`) — was silently failing for spawnpoint-set traps (which store integers, not strings).

### `pkg/packethooks/megacliloc/itemdata.src`
- `IsLockedSpawnPointChest(xObject)` check at top of item tooltip handler.
- Returns empty props for any container with `SpawnPointChest` CProp that is currently locked — prevents players from reading loot level or item details before picking the chest.
- `IDCore_IsLockedSpawnPointChest()` in `scripts/include/itemid_core.inc` gates Item ID skill and Talisman ID on locked spawn chests — cannot be identified while locked.

### New PropID Constants (`scripts/include/constants/propids.inc`)
| Constant | String Value | Purpose |
|----------|-------------|--------|
| `PROPID_CHEST_SPAWNPOINT_CHEST` | `"SpawnPointChest"` | Marks chest as spawn point type |
| `PROPID_CHEST_SPAWNPOINT_LOCK_DIFFICULTY` | `"SPLockPickDiff"` | Tiered lock difficulty |
| `PROPID_CHEST_SPAWN_LEVEL` | `"SpawnChestLevel"` | Loot tier display (1–7) |

### `config/itemdesc.cfg`
- **Elven Chest** (`5cc13fe`): New container objtype added for use as the spawn chest graphic.

---

## 28b. NPC Spawn / Townspeople

### `c442d6c`: Townsfolk AI overhaul
- Townsfolk now stay within the city they were spawned in
- If they leave the city boundaries, they are killed and will respawn
- Can only be spawned in designated city regions
- Functions consolidated into a new `.inc` file

### `b662b52`: City updates in region config
- Region data updated so townsfolk know their spawn city boundaries

### `0a26c88`: NPC patrol behaviour
- Nobles and town people now walk around their area

### `3900f07`: Tracking fix for bosses + NPC restart script
- Boss NPC tracking fixed; restart script for NPC cleanup

### `ed3f499`: Test Wyrm color fix

---

## 29. Treasure Maps

**File:** `pkg/std/treasuremap/digtreasure.src` — 151-line change

### Personal Power Hour Honour
- `80295b91`-series (multiple iterations): Treasure map digging now honours the **personal power hour** (`#PPHC`) of the digger, not just the global PH
- Previous behaviour only checked global power hour

### Magic Chance Randomized
- `f9a303e`: Treasure map magic item chance is now randomized per dig

### Bard Loot Level 7
- `b015787`: Bards now have a chance at level 7 loot from treasure maps

### Spawn Chest Level Fix
- `b015787`: Spawn chests now correctly do level 7 loot

### `c570519`: Tmap fix — miscellaneous treasure map bug fix

### `pkg/std/treasuremap/guardians.cfg` — 3-line change
- Kraken added as possible guardian monster

### Loot Level 10 → Level 11
- Entire loot level chain updated: proper level 10 tier inserted; level 11 now exists

---

## 30. Miscellaneous / Config Changes

### Tents Removed
- `24c5cd5`: Tents removed from the game entirely

### Dragon Armor
- `64ad22b`, `a0d4bf6`: Dragon armor implemented — craftable via Blacksmithy; added to loot; kimono and kamishimo added in game and in loot

### Dupe Bag Script (`d5c3782`)
- New dupe bag script (test/GM tool)

### `pkg/opt/earth/shapeshift.src` — 26-line change
- Earth shapeshift updates

### `pkg/opt/earth/earthblessing.src` — 2-line change

### `pkg/opt/necro/spellbind.src` — 3-line change
- Cannot be spellbound if totem (tied to totem fix)

### `pkg/opt/farming/spinning.src` — 2-line change

### `pkg/std/removetrap/removetrap.src` — 7-line change
- Remove trap now works correctly with spawned chests

### `pkg/opt/chests/lockpicking.src` — 99-line change
- Lockpicking updated to work with spawned chests (Elven Chest support)
- Lockpicking review feedback addressed

### `pkg/opt/spawnpoint/checkpoint.src` — 123-line change
- Spawn point logic fixed to work with chest spawning
- Checkpoint infinite loop fixed (`f588f97`)

### `5cc13fe`: Elven Chest
- Elven Chest added for spawn chest system
- Traps and lockpicking integrated with spawned chests

### Pentagram Drop Fix
- `4fe6bf5`: Pentagram #9 drop table fixed

### Nyx and Rikktor
- `2f1894d`: Moved to Tier 11 loot table

---

## 31. New Commands & GM Tools

| Command | File | Level | Purpose |
|---------|------|-------|---------|
| `.cache list` | `scripts/textcmd/player/cache.src` | Player | List cache contents by category |
| `.cache withdraw` | same | Player | Withdraw items from cache |
| `.cache deposit` | same | Player | Target item to deposit |
| `.cache deposit all` | same | Player | Bulk deposit from backpack (confirmation gump) |
| `.cache autodraw` | same | Player | Toggle cache autodraw for crafting |
| `.cache dump` | same | GM (4+) | Raw DataFile dump for debugging |
| `raisecaps` | `textcmd/test/raisecaps.src` | Test | Raise chosen skill caps |
| `lowerallchosencaps` | `textcmd/test/lowerallchosencaps.src` | Test | Lower all chosen caps |
| `raiseallchosencaps` | `textcmd/test/raiseallchosencaps.src` | Test | Raise all chosen caps |
| `lowercaps` | `textcmd/test/lowercaps.src` | Test | Lower specific caps |
| `createinbag` | `textcmd/test/createinbag.src` | Test | Create item in bag (GM test tool) |
| `showcaps` | `textcmd/player/showcaps.src` | Player | Show current skill caps |
| `undressme` | `textcmd/player/undressme.src` | Player | Undress character |

### `d5c3782`: Dupe bag script (GM test tool)
### `ba2ac09`: New commands gump + initial test panel

---

## 32. Config Files Changed

| File | Change Summary |
|------|---------------|
| `config/command_synopses.cfg` | **New** — 2,074 lines of command synopses |
| `config/nlootgroup.cfg` | Loot tier restructure; shields, dragon armor, new items |
| `config/npcdesc.cfg` | NPC template updates; Type fields; city regions; loot groups |
| `config/itemdesc.cfg` | Dragon armor, kimono, kamishimo, talisman items |
| `config/menus.cfg` | 42-line change — menu updates |
| `config/equip.cfg` | 2-line change |
| `config/mrcspawn.cfg` | 4-line spawn config update |
| `config/stacking.cfg` | 1-line addition referencing `stacking_ignore.cfg` |
| `config/fileaccess.cfg` | 2-line removal |
| `pkg/systems/accounts/config/uoclient.cfg` | 2-line change |

---

*Generated from `git diff --stat 8b0de10..HEAD` and targeted per-file diffs.*  
*Anchor commit: `8b0de10` — "Finalising plan for Omega Cache"*  
*HEAD: `fab0488` — "Omega cache to 50 tokens"*
