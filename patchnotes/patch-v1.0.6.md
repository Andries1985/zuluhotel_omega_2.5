# Patch Notes - v1.0.6
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: June 25, 2026**

---

Welcome to **Patch 1.0.6**. This update introduces **Champion Relics** — a new type of artifact that lets you call down a champion boss — alongside a sweep of crafting and skill-gain bug fixes.

---

## What Changed

## 🏆 Champion Relics - New Artifact Items

Two new artifacts have been added to the Artifact System: **Champion Relics**.

### Player Impact

- Champion Relics can now drop from the Artifact Box.
- **Tier 1 Relic** summons a Tier 10 champion boss (Barracoon or Chaos God) when activated.
- **Tier 2 Relic** summons a Tier 11 champion boss (Rikktor or Nyx) when activated.
- Relics cannot be used in safe areas, guarded areas, or NOPK zones.
- The relic is destroyed on use.
- Relics have a **14-day expiry timer**. You can check remaining time in the item tooltip.
- Expired relics are automatically removed from storage by a background sweeper.

---

## 🗺️ Cartography - Resource and Skill Gain Fixes

Cartography had bugs where skill gain could occur or maps could be created even without the required materials.

### Player Impact

- All map tiers now correctly check you have enough resources **before** granting skill gain or creating a map.
- Blank maps must now be in your backpack to use — you can no longer drag-target one from a bank or container.
- No functional changes to map content, costs, or difficulty.

---

## 🔥 Camping - Kindling Source Fix

### Player Impact

- Kindling wood used for camping must be in your backpack or on the ground. Targeting wood inside a bank container is no longer allowed.

---

## ⚒️ Blacksmithy - Jewelry Crafting Fix

### Player Impact

- Making jewelry now correctly stops when you don't have enough gold, instead of proceeding to the skill check.

---

## 🐾 Tamed Pets - Overhang Following Fix

### Player Impact

- Tamed pets now follow correctly through tower and multi overhangs. Previously, pets could stall or drop follow state when the path crossed overhang geometry.

---

## 🏪 Player Merchant - Stock Announcement Fix

### Player Impact

- Hidden or concealed characters controlling a player merchant will no longer trigger a stock announcement event, preventing unintended reveals.

---

## 📋 Summary

✅ Champion Relics added — summon a boss from your artifact  
✅ Cartography resource checks corrected; blank maps must be in backpack  
✅ Camping kindling sourced only from backpack or ground  
✅ Jewelry crafting gold check now properly exits on failure  
✅ Tamed pet following fixed for tower/multi overhang geometry  
✅ Player merchant stock announcements suppressed while hidden/concealed

---

Thanks for playing Zuluhotel Omega 2.5.
