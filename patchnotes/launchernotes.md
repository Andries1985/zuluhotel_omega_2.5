# Latest Changes
Always check Discord announcements for all the patch notes.

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

## 📘 Runebook - Recall Scroll Overflow Fix

### Player Impact

- Runebooks now recharge correctly when a scroll stack is larger than remaining charges.
- Example: dropping 2 recall scrolls on a runebook missing 1 charge now recharges the book by 1 as expected.
- Any extra scrolls are returned to backpack when possible.
- If backpack is full, extras are dropped to the ground when possible.
- If backpack return and ground drop both fail, only the excess scrolls are destroyed and recharge still succeeds.

---

## 🛠️ Internal - Script API Compatibility Cleanup

### Player Impact

- No gameplay change expected.
- Deprecated `MoveItemToLocation` script calls were migrated to `MoveObjectToLocation` in affected scripts for modern POL compatibility.

---

## ⛪ High Priest - Relationship Cost Prompt

### Player Impact

- If your relationship with the High Priest is upset, you can now say **relationship** to see exactly how much gold is required to repair it.
- The required donation scales with your class level (`class level * 2500`).
- If no class level is detected, the priest now gives a fallback hint that any donation amount can begin repairing the relationship.

---

## ⚔️ Dual Planar - Protection Gate Removed

### Player Impact

- The protection/immunity gate on Dual Planar's on-hit effect was removed.
- This means previously protected targets are no longer skipped by that specific immunity branch.

---

## 💎 Loot Tables - More Life Crystal Coverage

### Player Impact

- `lifecrystal` was added to additional entries within the Junk loot group.
- You should now see broader life crystal availability from junk-group loot rolls.

---

## 📋 Summary

✅ Champion Relics added — summon a boss from your artifact  
✅ Cartography resource checks corrected; blank maps must be in backpack  
✅ Camping kindling sourced only from backpack or ground  
✅ Jewelry crafting gold check now properly exits on failure  
✅ Tamed pet following fixed for tower/multi overhang geometry  
✅ Player merchant stock announcements suppressed while hidden/concealed  
✅ Runebook recall-scroll overflow now recharges safely and handles extras correctly  
✅ Deprecated item-move API calls migrated for core compatibility  
✅ High Priest now reports relationship repair gold requirement  
✅ Dual Planar immunity/protection gate removed on hit logic  
✅ Life crystal appears in more Junk loot entries

---

Thanks for playing Zuluhotel Omega 2.5.
