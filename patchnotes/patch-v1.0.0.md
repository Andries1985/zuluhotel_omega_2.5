# Patch Notes — v1.0.0
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: May 2, 2026**

---

Welcome to **v1.0.0** — the largest update to Zuluhotel Omega in years. This patch introduces the **Omega Cache**, a complete overhaul of the crafting material storage system, alongside significant improvements to tracking, loot, combat, tamed pets, housing, bard songs, and much more.

---

## 🏺 Omega Cache

The **Omega Cache** is a brand-new craftable container you can place in your house. It stores an enormous quantity of crafting materials in a single location — no more spreading ingots, hides, logs, and reagents across dozens of chests.

### How It Works
- Place the Omega Cache deed inside your house. You'll be prompted to choose an orientation (South or East facing).
- Double-click the container to open the cache gump, which displays all stored materials organised by category (Crafting, Mage, Food, General, etc.).
- Use the **Deposit** button to target individual items from your backpack, or **Deposit All** to sweep all eligible items from your backpack at once (you'll be asked to confirm first).
- Use the **Withdraw** buttons to take items back — either directly to your cursor to place in the world, or straight to your backpack.
- You can also **drag and drop** items directly onto the cache container to deposit them.

### Crafting Directly from the Cache
All major crafting skills can now **pull materials directly from your Omega Cache** without moving anything to your backpack first:

(Example list, not exhaustive)
| Skill | Supported Materials |
|-------|-------------------|
| Blacksmithy | Ingots, Bone |
| Tinkering | Ingots, Logs, Glass, Clay, Obsidian, Bottles, Gems, Potions (traps) |
| Tailoring | Hides, Cloth |
| Carpentry | Logs, Ingots, Cloth |
| Alchemy | Reagents, Empty Bottles |
| AlchemyPlus | Reagents, Flasks, Bottles |
| Inscription | Blank Scrolls |
| Cartography | Blank Maps |
| Cooking | Ingredients |
| Bowcraft/Fletching | Logs (shafts), Feathers, Arrow components |

**Autodraw** — If you have some materials in your backpack and some in the cache, the system will automatically use backpack stock first, then draw the remainder from the cache. You can disable this with `.cache autodraw` if you prefer strict backpack-only crafting.

**Bulk crafting** — When making large quantities of stackable items (shafts, arrows, bandages, etc.) and your cache is involved, you'll be prompted for how many you want to make.

### Commands
- `.cache list` — Browse and withdraw items from your cache
- `.cache deposit` — Target an item to deposit
- `.cache deposit all` — Deposit all eligible items from your backpack
- `.cache autodraw` — Toggle automatic cache fallback during crafting

### Notes
- The cache stores **stackable items only**. Non-stackable items (weapons, armour, etc.) cannot be stored.
- Only the **house owner and friends** can deposit into or withdraw from the cache.
- Number of caches allowed depends on the house.  Max 3 caches.

---

## 🏹 Tracking Overhaul

The Tracking skill has been completely overhauled.

- **More creature categories** — Tracking now shows 20+ specific creature types: Animals, Animated, Beholder, Champion, Daemon, Dragonkin, Elemental, Ethereal, Gargoyle, Giantkin, Human, Ophidian, Orc, Plant, Ratkin, Slime, Terathan, Troll, Undead, and more.
- **Bosses tracked as Champions** — Any boss, super boss, lesser boss, or champion NPC shows up under the "Champion" category, making it easier to hunt down powerful enemies.
- **Visual icons** — Each category has a unique tile graphic icon in the tracking menu.
- A message now displays when you begin tracking: *"You begin to track nearby creatures."*
- Each new track attempt resets the counter and timeout cleanly.

---

## 🗡️ Talisman System

**Talismans** are a new craftable item type made by Tinkers, requiring items from alchemy, mining and hunting.

- Talisman gems can now be mined. There are 8 new gems to be found.
- Talisman crafting also requires a glowing brain, which can be found hunting and carving up corpses.
- Alchemists can combine the brain with 1 of each of the new gems and a flask to create a Flask of Crystallized Intelligence.
- Tinkers then can tinker that flask with an ore of your choosing to create the talisman.
- Once create you can equip the talisman of ID and use it to id items/corpses/bags etc, using a charge for each item ID'd.
    - *Note* The type of ore you use is just for color.  The amount of charges the Talisman has is based on crafter level.
- Crafters can also create recharge flasks with the Smith of Retort to recharge your Talisman of ID.
    - You can create these with some of the new gems and some Dragon's Blood.

---

## ⚔️ Combat & Weapons

### Elemental Weapons
- **Elemental weapons from Pentagrams** now deal **100% elemental damage**.
- Water, Earth, Air, Fire, and Shadow elemental weapons now correctly apply their elemental damage type.
    - **Fire Katana** now deals fire damage.

### Armor
- Armor now has a better chance to receive stat bonuses when it drops.
- Armor tooltip now shows **actual AR (AR at your skill level)** alongside **maximum potential AR (at 150 skill)**, e.g. `32 (45)`. This helps you understand how much better armour will get as your skills improve.

### Exceptional Crafting
- Exceptional item chance is now **capped at 150 skill points**. Previously it scaled without limit.

### Dual Planar
- Dual Planar no longer displays how much damage you are doing to the target in chat.

### Magic Absorption
- Magic absorption now behaves the same as magic reflection when targeting yourself.
 - Should be able to dispel yourself while wearing blackrock.

### Thief Healing
 - Doubled thief healing from poison bandages.

---

## 🎵 Bard Songs — Rebalance

Bard buffs have been normalised across all songs. Buffs are now based on your **spec level** (how many specialisation points you have invested in Bardic songs) with fixed tiers:

| Spec Level | Buff Modifier |
|-----------|--------------|
| 0 | +0 |
| 1 | +15 |
| 2 | +30 |
| 3 | +45 |
| 4 | +60 |
| 5 | +65 |
| 6+ | +75 |

- **Song of Defense** and **Song of Haste** duration increased from **30 minutes to 60 minutes**.
- Song of Glory now also checks if the target can receive the polymorph effect before applying the ebless buff.
- Bard buff application was previously unreliable in some edge cases — this is now fixed.
- An infinite loop bug in the spawn point checkpoint was fixed (affected bards near certain spawn areas).

---

## 🐾 Tamed Pets

### Pet Count
- **Ranger C5 players**: can have 2 boss pets + 1 mount
- **Ranger C6 players**: can have 2 boss pets + 2 mounts

### Boss/Super Boss Pets & Houses
- If a Boss or Super Boss tamed pet **enters a player house**, it will be automatically confiscated (instead of killed). Boss pets are not permitted inside houses.
    - This will also happen if your pets go wild inside of a safe zone.
    - You will get a ticket you can redeem at a stablemaster.

### AI Improvements
- Caster and Poisoner AI has been fixed.  Mobs that can be tamed that can cast spells will now do so.  Mobs that can poison will now do so again.
- Tamed casters now maintain a distance of **2–10 tiles** from their target, making them more effective in combat.
- Tamed pets (magic users, summons, animated) will now begin automated defense immediately on their first AI loop rather than waiting.
- Fixed a bug where tamed NPCs could cast special mass/AOE spells they shouldn't.
- Fixed a bug where mass spell targeting incorrectly affected tamed pets that had been released.
- Pets now will no longer switch targets when being told to kill.
- Pets now will auto follow you after a combat loop is over.

---

## 🏦 Banker NPCs

Bankers now have a full suite of banking commands you can use by talking to them:

| Command | Function |
|---------|----------|
| **DEPOSIT** | Deposits cheques into your bank box |
| **WITHDRAW** | Withdraws cheques from your bank box |
| **CHECK** / **CHEQUE** | Asks the banker for a cheque... Also works with player vendors to give you your gold they have collected|

### Changes
- Bankers now **only accept cheques** — you can no longer hand bankers arbitrary items.
- **Deposit confirmation** — the banker will ask you to confirm before processing a deposit command.
- The cheque box now only accepts numeric input (no letters or symbols).
- Player merchants can now give cheques to players.
- Various speech queue and item transfer bugs fixed.
- Bankers are excluded from the begging system.

---

## 🏠 Housing

- The house sign now shows how many **Omega Cache containers** you have placed vs. your maximum allowed.
- Fixed a possible crash/death bug that could occur when dying while standing on a container inside your house.
- House demolition now properly destroys any Omega Cache containers placed in the house.
- Secure container permissions are more robustly checked during cache deposit operations.
- Allows you to move your omega cache as long as it is empty, or you have more than one in the house.

---

## ⚡ Power Hour

- **Sunday Bonus Power Hour**: if the bonus power hour type is "Resource", there is now a significantly higher chance of triggering a second bonus power hour (approximately 1-in-2 instead of 1-in-10).

---

## 🎁 Christmas Gifts

- **Configurable cooldown** — the gift cooldown can now be adjusted server-side (default remains 24 hours).
- When you try to claim a gift too early, the rejection message now shows **exactly how much time remains** (e.g. "3 hours, 22 minutes").
- Gift opening messages are now shown correctly.
- The gift table has been significantly expanded with new items, adjusted drop rates and increased present roll chance on mobs that can drop them.

---

## 🧪 Alchemy & AlchemyPlus

### Vanilla Alchemy
- Full Omega Cache integration — reagents and bottles can come from cache.
- Flask autoloop bug fixed: the loop no longer consumes a bottle without producing a potion on the last iteration.

### AlchemyPlus
- Full Omega Cache integration — double-click the alchemist's burner and target your Omega Cache to select a primary reagent from cache.
- **Fixed critical bug**: potions were being lost on the last crafting loop iteration because the bottle was consumed before the potion was created. This has been corrected — the potion is always created before the bottle is consumed.
- AlchemyPlus error messages improved.

---

## 🔨 Crafting — Tinkering

- **Totems** — Only crafters can make them. Cannot be spellbound. Magery is no longer required.
- **Talisman crafting** added to tinkering (see Talisman section above).

---

## 🗺️ Treasure Maps

- Treasure map digging now honours **Personal Power Hour** (`#PPHC`) — if you have your personal power hour active, it applies to your dig.
- The magic item chance per chest is now **randomised** per dig rather than fixed.
- Spawn chests now correctly generate **Level 7 loot**.
- Bards now have a chance at **Level 7 loot** from treasure maps.
- Kraken added as a possible guardian monster.

---

## 🏘️ NPC & Townspeople

- **Townsfolk now stay in their city.** If they wander outside their city's boundaries, they are killed and will respawn at their correct spawn point. They can only spawn in designated city regions.
- Nobles and townspeople now patrol their area (walking around).
- Begging system expanded — you can no longer beg while inside a house, or from NPCs that are inside a house. You also cannot beg from: High Priests, Architects, Vanity Vendors, Bankers, Healers.

---

## 📦 Loot System

- **Loot tier overhaul**: a proper Level 10 tier has been added; all loot increments updated accordingly.
- **Nyx and Rikktor** moved to **Tier 11** loot.
- Shields now have their own loot functions with per-skill proc chances (different shield procs depending on relevant combat skill).
- Dragon armor, kimonos, kamishimo, and new clothing items added to the loot table and enchantable item lists.
- New items added with appropriate restricted class groups and drop locations.
- Legendary items have been reintroduced with corrected drop rates.

---

## 🔍 Item Tooltips

- **Weapon DPS** in tooltips is now accurate — previously, the tooltip incorrectly reduced DPS based on item HP%, causing weapons at low durability to show misleadingly low values.
- **Talisman charges** now show on the tooltip.
- **Spawn chests** show no property information until they are unlocked — you can no longer read loot details off a locked spawn chest.
- NPC stat tooltips (visible to staff) now show current HP, MP, Stamina alongside maximums, plus loot group info and elemental resistances.

---

## 🌿 Resurrection Crystals

- Resurrection crystals now **actually resurrect the player** when used. Previously they only set a CProp that was never read — they effectively did nothing. This is now fixed.

---

## 🗡️ Skill Caps

- Skill caps are now enforced every time you gain, train, or use a transcend scroll. Your equipped gear is taken into account — you cannot exceed your effective cap based on what you have equipped.
- The cap enforcement logic now uses the same rules as the capper system.

---

## 💬 Commands

### New Player Commands
- `.cache list` / `.cache deposit` / `.cache deposit all` / `.cache autodraw` — Omega Cache management
- `showcaps` — Show your current skill caps
- `undressme` — Undress your character
- `.commands` — Updated to show a synopsis of what the command does

---

## 🎭 Role Player (RPer) — New Classes

Three new classes are now available when you use the **RPer Stone** to begin your roleplaying journey. The stone's selection menu has been expanded to include:

---

### ⚔️ Paladin
A holy warrior clad in full plate, equally at home with blade or mace, and capable of supporting their allies with magic.

**Skills:** Swordsmanship, Mace Fighting, Tactics, Parry, Magery, Evaluate Intelligence, Meditation, Magic Resistance

**Starting Equipment:**
- Enchanted Katana *(+20 damage modifier)*
- Enchanted Mace *(+20 damage modifier)*
- Full Platemail Armour set
- 200 Bandages, 10 Greater Heal Potions, 5 Greater Cure Potions
- 2,000 Gold Coins

**Restrictions:** Cannot wear leather, studded leather, bone, or ringmail armour. Can equip shields.

---

### 🏹 Mystic Archer
A wanderer of the wilds who combines archery with the magic of the sea and sky. Part hunter, part mage — self-sufficient and deadly at range.

**Skills:** Archery, Magery, Evaluate Intelligence, Meditation, Magic Resistance, Fishing, Cooking, Tracking

**Starting Equipment:**
- Enchanted Bow *(+20 damage modifier)*
- 500 Arrows
- Full Leather Armour set
- Fishing Pole, 100 Kindling, 100 Raw Fish Steaks
- Assorted Magic Fish (10 each of 5 varieties)
- Full reagent stock (60 of each reagent)
- 2,000 Gold Coins

**Restrictions:** Cannot wear platemail armour or use shields.

---

### 🎵 Bladesinger
A deadly fusion of blade and song — a duelist who fights with finesse and controls the battlefield with bardic magic.

**Skills:** Swordsmanship, Fencing, Tactics, Anatomy, Musicianship, Provocation, Peacemaking, Enticement

**Starting Equipment:**
- Enchanted Daisho *(+20 damage modifier)*
- Chainmail Coif, Leggings & Tunic
- Platemail Arms, Gorget & Gloves
- Leather Boots
- Lute
- 20 Greater Heal Potions
- 4,000 Gold Coins

**Restrictions:** Can only wear leather and studded leather armour (plus the chainmail/plate pieces provided). Can equip shields.

---

## 🐛 Bug Fixes

- Fixed an issue where players could not mount certain mounts if they weren't a donator mount graphic.
- Fixed a naming issue for shields.
- Fixed ArmorZone calculation errors that could produce incorrect armor zone lookups.
- Fixed tracker map reset not clearing correctly between uses.
- Pentagram #9 drop table corrected.
- Tents have been removed from the game.
- HP spawning with less HP (NPCs no longer spawn with incorrect HP values).
- Various compilation errors fixed.
- Fixed Warrior for Hire healing bandage spam after being ressed.

---

*Thank you for playing Zuluhotel Omega. This update represents months of development work. Please report any bugs to staff.*
