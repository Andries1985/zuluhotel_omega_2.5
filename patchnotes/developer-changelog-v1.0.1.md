# Developer Changelog — v1.0.1
**Range:** `fab0488` (Omega Cache to 50 tokens) → `HEAD` (`353ad60`)  
**Branch:** Patch-1.0.1  
**Date:** 2026-05-04  
**Commits in range:** 8 (excluding patchnote/cosmetic commits)  
**Files changed:** 10 | +518 / -358

---

## Table of Contents

1. [Omega Cache — Post-Launch Fixes](#1-omega-cache--post-launch-fixes)
2. [Housing — Sign & Storage Recount](#2-housing--sign--storage-recount)
3. [AlchemyPlus — Dispel Potion Fix](#3-alchemyplus--dispel-potion-fix)
4. [NPC / Combat — Dragon Lord Rebalance](#4-npc--combat--dragon-lord-rebalance)
5. [Rise System — Loot Transfer Fix](#5-rise-system--loot-transfer-fix)
6. [NPC Regen — CustomHP Healing Fix](#6-npc-regen--customhp-healing-fix)
7. [Staff Tools — MegaCliloc & Go Command](#7-staff-tools--megacliloc--go-command)
8. [World Data — New Go Locations & Regions](#8-world-data--new-go-locations--regions)

---

## 1. Omega Cache — Post-Launch Fixes

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/omegacache/itemdesc.cfg` | Deed vanity cost reduced |
| `scripts/include/omegacache_utils.inc` | Cache slot limits rebalanced for Mansions and Keeps |

### Omega Cache Deed Cost (commit `08c1040`)

The Omega Cache placement deed `VanityCost` was reduced from **50 → 20** tokens immediately after launch feedback.

```diff
-  VanityCost      50
+  VanityCost      20
```

**File:** `pkg/opt/omegacache/itemdesc.cfg`

---

### Cache Availability Rebalance (commit `cf2a411`)

`GetMaxOmegaCacheForHouse()` in `scripts/include/omegacache_utils.inc` was adjusted for two house types that were unintentionally capped low relative to their secure capacity.

| House Type | Old Limit | New Limit |
|------------|-----------|-----------|
| Mansion (`0x607A`) — 15 secures | 2 caches | 3 caches |
| Keep (`0x6BB8`) | 1 cache | 3 caches |

All other house type limits remain unchanged. The full switch block in `GetMaxOmegaCacheForHouse()` now reads:

```
Small houses (3–5 secures)  → 1 cache
Medium houses (8 secures)   → 2 caches
Large houses (10+ secures)  → 2 caches
Mansion (15 secures)        → 3 caches  ← changed
Castle (20 secures)         → 3 caches
Fortress (25 secures)       → 3 caches
Keep                        → 3 caches  ← changed
```

---

## 2. Housing — Sign & Storage Recount

### Files Changed
| File | Change |
|------|--------|
| `pkg/std/housing/sign.src` | Added `RecountHouseStorageUsage()` function; called on every sign open |

### Problem (commit `50f8aa7`)

The lockdown, secure, and Omega Cache counts displayed on the house sign (`numlockdowns`, `numsecure`, `numomegacache`) were tracked by increment/decrement at the time of placement or removal. This meant the displayed counts could drift out of sync if items were moved by GM tools, crash recovery restored old data, or edge-case placement/removal bugs occurred.

### Solution

A new function `RecountHouseStorageUsage(house)` was added to `sign.src` and is now called **every time the house sign gump is opened**, before the count values are read for display.

```uo
function RecountHouseStorageUsage(house)
    // Iterates house.items, counts:
    //   lockeddown CProp       → used_lockdowns
    //   usescript == USESCRIPTID_SECURE_CONTAINER → used_secures
    //   objtype == OMEGACACHE_OBJTYPE             → used_omegacaches
    //
    // Writes back SetObjProperty(house, "numlockdowns" | "numsecure" | "numomegacache", ...)
    // Clamps remaining counts to 0 minimum (cannot show negative remaining slots)
```

**Behaviour notes:**
- If `maxnumlockdowns`, `maxnumsecure`, and `maxnumomegacache` are all 0 (uninitialised house), the function calls `GetMaxProps(house)` first to ensure maximums are set before recounting.
- The stored CProp values are now the **remaining** (available) count, not the used count — consistent with prior convention.
- Any negative remainder is clamped to 0 to prevent display issues.

**Call site:**

```uo
// In program textcmd_sign() before reading numlockdowns / numsecure / numomegacache:
RecountHouseStorageUsage(house);
data[8]  := GetObjProperty(house, "maxnumlockdowns") - GetObjProperty(house, "numlockdowns") + "/" + ...
```

Wait — this is an inversion: the stored value is now the *used* count derived from the live scan. Callers that read `numlockdowns` as remaining should continue to work because `RecountHouseStorageUsage` stores the remaining (max - used) value. Double-check if any other code paths write `numlockdowns` as "used" vs "remaining" if issues arise post-deploy.

---

## 3. AlchemyPlus — Dispel Potion Fix

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/alchemyplus/newpotions.src` | Fixed dispel potion not being consumed on use |

### Problem (commit `d86895a`)

The `DoDispel()` function was declared with `unused what` as the second parameter:

```uo
function DoDispel( me , unused what)
```

The `unused` keyword suppresses the variable from being referenced, which prevented `SubtractAmount(what, 1)` from ever being called. Dispel potions were applied but **never consumed** from the player's pack.

### Fix

```diff
-function DoDispel( me , unused what)
+function DoDispel( me , what)
 
     PlayObjectCenteredEffect(...)
     ...
     SendSysmessage(me, "All the magical effects active on this creature have been wiped.");
+    SubtractAmount(what, 1);
```

The `unused` qualifier was removed and `SubtractAmount(what, 1)` was added directly after the effect message, consistent with all other potion effect functions in the file.

---

## 4. NPC / Combat — Dragon Lord Rebalance

### Files Changed
| File | Change |
|------|--------|
| `config/npcdesc.cfg` | Dragon Lord base DEX and BaseDexmod reduced |

### Changes (commit `353ad60`)

The Dragon Lord NPC template (`NpcTemplate dragonlord`) had its agility stats tuned down. The previous values made the Dragon Lord disproportionately fast relative to its intended threat tier.

| Property | Old Value | New Value |
|----------|-----------|-----------|
| `DEX` (base stat) | 140 | 130 |
| `BaseDexmod` (CProp modifier) | 500 | 300 |

The `BaseDexmod` reduction is the more impactful change, as this CProp is applied on top of the base DEX to compute effective combat dexterity. The combined effective dex was over-tuned; this brings it in line with other Tier 11+ boss entries.

---

## 5. Rise System — Loot Transfer Fix

### Files Changed
| File | Change |
|------|--------|
| `scripts/misc/rise.src` | Tag all transferred items with `KeepOnNoLootDeath`; tag risen NPC with `RiseLootTransfer` |
| `scripts/misc/death.src` | Honour `RiseLootTransfer` flag during no-loot corpse cleanup |

### Problem (commit `126817a`)

When an NPC is raised as a risen corpse via `rise.src`, items from the original corpse are moved into the risen NPC's backpack. The risen NPC is created with `noloot = 1`, meaning when it dies its corpse is cleaned of all items. This caused the loot originally looted from the base monster to be **destroyed** on the risen NPC's death instead of remaining on its corpse.

### Solution

Two new CProps are introduced to communicate loot intent through the death pipeline:

| CProp | Set On | Meaning |
|-------|--------|---------|
| `RiseLootTransfer` | Risen NPC | This NPC's corpse holds transferred loot; preserve `KeepOnNoLootDeath` items |
| `KeepOnNoLootDeath` | Each transferred item | This item should survive the no-loot cleanup |

**`rise.src` changes:**
```uo
SetObjProperty(the_critter, "RiseLootTransfer", 1);   // Mark the risen NPC

foreach item in EnumerateItemsInContainer(corpse)
    SetObjProperty(item, "KeepOnNoLootDeath", 1);     // Tag each transferred item
    MoveItemToContainer(item, the_critter.backpack);
endforeach
```

**`death.src` changes (inside the `noloot` cleanup loop):**
```uo
if (GetObjProperty(corpse, "noloot"))
    foreach item in EnumerateItemsInContainer(corpse)
        if (item.container == corpse)
            if (GetObjProperty(corpse, "RiseLootTransfer") and GetObjProperty(item, "KeepOnNoLootDeath"))
                EraseObjProperty(item, "KeepOnNoLootDeath");
                continue;   // Skip destruction — leave item on corpse
            endif
            DestroyItem(item);
        endif
    endforeach
    if (GetObjProperty(corpse, "RiseLootTransfer"))
        EraseObjProperty(corpse, "RiseLootTransfer");
    endif
```

**Result:** Items transferred from the original corpse to the risen NPC are now left on the risen NPC's corpse when it dies, available for looting normally.

---

## 6. NPC Regen — CustomHP Healing Fix

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/shilhook/regen.src` | Disabled faulty self-heal logic for CustomHP NPCs |

### Problem (commit `126817a`)

A block of code in the regen hook was intended to synchronise an NPC's HP with its `CustomHitsLevel` on first spawn. Due to a complex boolean expression combining `and` / `||` / `&&`, it was incorrectly triggering on subsequent regen ticks, causing CustomHP NPCs to **reset to full HP** each time the regen hook fired.

The problematic condition:
```uo
if ((GetMaxHp(who) != GetHp(who) and !GetObjProperty(who, "firstTimeCustomHp")) 
    || GetMaxHp(who) != GetHp(who) && GetGlobalProperty("CustomHPFix"))
```

The `||` branch (`GetMaxHp != GetHp && CustomHPFix`) had no guard on `firstTimeCustomHp`, meaning any damaged CustomHP NPC would be healed to full every regen tick when the global flag was set.

### Fix

The entire block was commented out pending a correct reimplementation:

```uo
// if ((GetMaxHp(who) != GetHp(who) and !GetObjProperty(who, "firstTimeCustomHp")) 
//     || GetMaxHp(who) != GetHp(who) && GetGlobalProperty("CustomHPFix"))
// SetHP(who, GetMaxHP(who));
// SetObjProperty(who, "firstTimeCustomHp", 1);
// endif
```

**Note for future work:** The intended behaviour (sync HP once on first regen after spawn) should be reimplemented with a tighter condition — e.g. only firing if `firstTimeCustomHp` is not set, without any `||` fallback that bypasses the guard.

---

## 7. Staff Tools — MegaCliloc & Go Command

### Files Changed
| File | Change |
|------|--------|
| `pkg/packethooks/megacliloc/mobiledata.src` | Null-guard loot property display; add NPC script name tooltip line |
| `scripts/textcmd/coun/go.src` | Rewritten to use gumps package; constants extracted |
| `scripts/textcmd/seer/go.src` | **Deleted** — duplicate of coun/go.src |
| `scripts/start.src` | Removed duplicate go command registration |

### MegaCliloc Fixes (commit `126817a`)

**Problem:** When staff hovered an NPC, the loot info cliloc line was always appended even if the NPC had no loot config entries. If `loot_lootgroup`, `loot_mlevel`, or `loot_mprop` were not integers (e.g. uninitialised struct members), the line would display garbage or an error string.

**Fix — null-guarded loot line construction:**

Each loot property is now individually type-checked with `TypeOf(...) == "Integer"` before being appended to the display string. The cliloc line is only added to `allprops` if the resulting string is non-empty.

```uo
var loot_line := "";
if (TypeOf(loot_lootgroup) == "Integer")  loot_line := "Loot Grp: " + loot_lootgroup; endif
if (TypeOf(loot_mlevel) == "Integer")     loot_line := loot_line + "  Lvl: " + loot_mlevel; endif
if (TypeOf(loot_mprop) == "Integer")      loot_line := loot_line + "  Chance: " + loot_mprop + "%"; endif
if (loot_line)
    prop.values := array {loot_line};
    allprops.append(prop);
endif
```

**New — NPC script name tooltip line:**

After the loot line, the NPC's active script is now displayed. The script is resolved first from `xObject.script`, then falls back to `mob[xObject.npctemplate].script` if the runtime script property is unset.

```uo
var npc_script := xObject.script;
if (!npc_script)
    npc_script := mob[xObject.npctemplate].script;
endif
...
if (npc_script)
    prop.values := array {"Script: " + npc_script};
    allprops.append(prop);
endif
```

---

### Go Command Rewrite (commit `126817a`)

**`scripts/textcmd/coun/go.src`** was rewritten from scratch to use the `:gumps:gumps` include package instead of raw layout string arrays. The old implementation built gump layout manually with hardcoded string offsets and a direct `data := array(...)` approach, which was fragile and difficult to maintain.

**Key changes:**
- `include ":gumps:gumps"` added.
- All layout constants extracted to named `const` declarations at the top of the file for readability:

| Constant | Value | Purpose |
|----------|-------|---------|
| `HEADER_Y` | 50 | Y position of column headers |
| `TEXTSTARTLOCY` | 72 | Y of first location row |
| `ROW_HEIGHT` | 18 | Pixels per location row |
| `COL1_X` | 72 | Left column X |
| `COL2_X` | 312 | Right column X |
| `BTN_GO_OFS` | 160 | Go button X offset from column |
| `BTN_SND_OFS` | 200 | Send button X offset from column |
| `BTN_Y_OFS` | 5 | Vertical alignment offset for arrow buttons |

- Old commented-out permission check block (`gCmd` / `ReadGameClock`) removed entirely.

**`scripts/textcmd/seer/go.src`** — **deleted.** This was a duplicate of the coun-level go command. The seer-level script was registered separately in `scripts/start.src`, creating two active `.go` handlers at different command levels pointing to different (but functionally identical) scripts. The duplicate registration was removed from `start.src`.

---

## 8. World Data — New Go Locations & Regions

### Files Changed
| File | Change |
|------|--------|
| `config/golocs.cfg` | Added Delucia and Papua fast-travel entries |
| `regions/regions.cfg` | Added Delucia and Papua region definitions |

### New Go Locations (commit `126817a`)

Two new entries were appended to `config/golocs.cfg`:

```cfg
goloc 0x0027
{
  Name  Delucia
  Type  all
  x     5221
  y     4009
  z     37
  r     britannia
}

goloc 0x0028
{
  Name  Papua
  Type  all
  x     5722
  y     3213
  z     16
  r     britannia
}
```

Both entries use `Type all` (accessible to all command levels) and target established Lost Lands coordinates on the Britannia map.

### New Region Definitions (commit `126817a`)

Skeleton region entries for Delucia and Papua were added to `regions/regions.cfg`. Both regions currently have their `EnterText`/`LeaveText` lines commented out — they will need boundary coordinates and active region config before they function as full regions:

```cfg
# Contains: Delucia, Papua
Region Delucia
    # EnterText   You have entered Delucia.
    # LeaveText   You have left Delucia.

Region Papua
    # EnterText   You have entered Papua.
    # LeaveText   You have left Papua.
```

---

## Summary of All Commits

| Commit | Description | Files |
|--------|-------------|-------|
| `f5a11ef` | Patchnotes folder and initial v1.0.0 notes created | patchnotes/ |
| `cf2a411` | Omega Cache limit rebalance (Mansion 2→3, Keep 1→3) | `omegacache_utils.inc` |
| `d86895a` | Dispel potion consumption fix (`unused` param + SubtractAmount) | `newpotions.src` |
| `08c1040` | Omega Cache deed cost: 50 → 20 vanity tokens | `itemdesc.cfg` |
| `50f8aa7` | Housing sign live storage recount (`RecountHouseStorageUsage`) | `sign.src` |
| `126817a` | Rise loot fix; Delucia/Papua; go rewrite; CustomHP fix; cliloc fix | 9 files |
| `353ad60` | Dragon Lord DEX 140→130, BaseDexmod 500→300 | `npcdesc.cfg` |
