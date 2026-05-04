# Patch Notes — v1.0.1
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: May 4, 2026**

---

Welcome to **Patch 1.0.1** — a hotfix and balance patch addressing issues discovered after the v1.0.0 launch. This update includes fixes to the Omega Cache, housing, potions, combat, and the undead rise system, as well as two new travel destinations.

---

## 🏺 Omega Cache — Fixes & Adjustments

### Cost Reduced
The **Omega Cache deed** now costs **20 Vanity Tokens** to purchase, down from 50. The original cost was an error — this is the intended price.

### Cache Limits Rebalanced
Two house types were unintentionally restricted to fewer Omega Caches than their size warranted. These have been corrected:

| House Type | Old Limit | New Limit |
|------------|-----------|-----------|
| Mansion | 2 caches | 3 caches |
| Keep | 1 cache | 3 caches |

---

## 🏠 Housing — Sign Display Fix

The lockdown, secure container, and Omega Cache counts shown on your **house sign** are now always accurate.

Previously, these numbers could drift out of sync if items were moved by staff, or if data was restored after a crash. The sign now **recounts your actual storage usage live** every time you view it, so what you see is always correct.

---

## ⚗️ Dispel Potions — Consumption Fixed

**Dispel potions** were being applied correctly but were **not being consumed** from your pack. This is now fixed — using a Dispel Potion will properly remove one charge from the stack.

---

## ☠️ Rise System — Loot Preserved on Risen Corpses

When a creature was raised as a **risen corpse** using the Rise effect (undead necromancy / spell), any loot that had been transferred from the original corpse to the risen creature was being **destroyed** when the risen creature died, rather than left on its corpse for players to loot.

This has been fixed. Loot transferred to a risen corpse will now remain available on its body when it falls.

---

## ⚔️ Dragon Lord — Dexterity Reduced

The **Dragon Lord** was moving and attacking significantly faster than intended due to over-tuned dexterity values. Its agility has been reduced to bring it more in line with its intended threat level.

| Stat | Old | New |
|------|-----|-----|
| Base DEX | 140 | 130 |
| DEX Modifier | 500 | 300 |

---

## 🔧 NPC Bug Fixes

### Custom HP Mobs Healing Themselves
A bug was causing certain high-HP monsters with custom hit point values to **reset to full health** on every regen tick when damaged. This made them effectively unkillable in some situations. The faulty healing logic has been disabled.

---

## 🗺️ New Travel Destinations

Two new locations have been added to the `.go` travel list, accessible to all command levels:

- **Delucia** — The Lost Lands frontier town
- **Papua** — The Lost Lands port city

Staff can now use `.go` to teleport directly to either location.

---

## 🛠️ Staff Tool Improvements

### Go Command Rewritten
The `.go` command used by Counsellors and above has been fully rewritten for reliability and maintainability. Functionally it behaves the same — the changes are internal. A duplicate version of the command that existed at the Seer command level has been removed.

### NPC Tooltip Improvements
When staff hover over an NPC with a tooltip active, the loot information line is now displayed more cleanly — it will only show values that are actually configured on the NPC, rather than showing blank or error entries for NPCs with no loot config. The NPC's active script name is also now shown in the tooltip for quick identification.

---

*For full technical details, see the [Developer Changelog — v1.0.1](developer-changelog-v1.0.1.md).*
