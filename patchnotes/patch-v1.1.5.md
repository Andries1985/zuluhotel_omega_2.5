# Patch Notes - v1.1.5
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: October 25, 2026**

---

Welcome to **Patch 1.1.5**, a small follow-up to 1.1.4. Mayors can finally step down. Empty keyrings can be sold. The personal powerhour no longer locks you out for part of the day a week after you used it. A few Warrior for Hire heart and High Priest loopholes are closed. And Sanctuary, Antidote and Dragon Skin are reworked: their protection can no longer get stuck or stack, it now depends only on the caster's class level, and Dispel removes it. Spellcasting monsters stop healing and blessing the players they fight, and every area spell a monster or pet casts now picks its targets the same way. Town NPCs become their own Townsfolk creature type. Warriors for Hire now fight the warrior you point them at, a new `.arrows` command counts your ammunition, and player vendors in cities finally pay the city wage. **If you are holding a warrior heart from before Patch 1.1.4, read the Warrior for Hire section.**

---

## What Changed

## Towns - Mayors Can Resign

### Player Impact

- **A mayor can now leave their town.** Until now the "give up your citizenship" button only appeared for ordinary citizens, so a mayor was stuck unless they lost an election or asked staff. The mayor's townstone menu now has **"Resign and leave town"**. After you confirm, you stop being mayor, you leave the town (your name loses its "of <town>"), and the whole shard is told you resigned.
- **The town can choose a new mayor straight away.** With no mayor, citizens can call an election at once instead of waiting out the two-month cooldown, as long as the town has at least 3 members. An election or poll that is already running carries on, and an election in progress will seat the next mayor.

---

## Personal Powerhour - No More Same-Day Lockout

### Player Impact

- **The weekly lock now lifts once and stays lifted.** Your personal powerhour resets at Sunday midnight GMT after you use it, or a full week later if you used it on a Sunday. A bug locked you out again on the same weekday the following week. For example, if you used it on a Wednesday at 15:00, you could start a new one on Sunday, Monday or Tuesday, but on Wednesday `.setph` refused you until 15:00 while `.ph` said you could start one "in less than a minute". `.setph`, `.ph` and the Eon-Prism now use the same reset time and always agree.

---

## Warrior for Hire - Hearts and the High Priest

### Player Impact

- **Hearts from before Patch 1.1.4 no longer work.** Since 1.1.4 every heart is tied to the death it came from. Older hearts were let through as a courtesy, but that also let a heart kept in a chest buy an extra life after any revival. Any heart from before 1.1.4 now crumbles when you hand it to the High Priest. If your warrior's most recent death left the priest a memory of it, "resurrect warrior" still works.
- **Hand your warrior's heart in yourself.** Only the character who owns the warrior can bring its heart to the High Priest. A heart handed in by anyone else, including another character on your own account, crumbles to dust ("This heart is bound to another soul").
- **A memory revival costs the same as a heart revival.** When the High Priest brings a warrior back from memory, it now loses one point of Strength, Intelligence and Dexterity for each death so far, never going below 10, just as with a heart. Before, the priest restored it at full stats.
- **Behind the scenes:** warriors use a little less server time in combat, and a rare case where damage between a warrior and a non-master could be halved as if they were sparring is fixed.

---

## Eon-Prism - Use It From Your Backpack

### Player Impact

- The Eon-Prism now only works from your own backpack (bags inside it count). Before, anyone standing nearby could use a prism lying on the floor or sitting in an open chest and spend it on themselves.

---

## Merchants - Empty Keyrings Sell for 1 Gold

### Player Impact

- **Merchants now buy empty keyrings for 1 gold each.** Keyrings are still kept on death, and that is what stopped merchants buying them. Use **"sell all"** and target a keyring to sell every empty one in your pack, or **"sell bag"** to sell a bag's contents. Keyrings will not show in the normal **"sell"** window, because the game never lists items you keep on death there.
- **A keyring with keys on it will not be sold.** Selling it would destroy the keys. The merchant tells you to "Take the keys off that keyring first". Use the keyring on itself to drop its keys into your pack, then sell it.

---

## Sanctuary and Antidote - Poison Immunity

### Player Impact

- **Your poison immunity jewelry no longer stops working when the spell runs out.** If you put on poison immunity jewelry while Sanctuary or Antidote was active, the spell ending took away all of your poison immunity, and you had to take the jewelry off and put it back on. The spell now ends cleanly and your jewelry keeps working.
- **Antidote now protects the person you cast it on.** Before, it cured your target but gave the poison immunity to you, the caster.
- **Immunity and duration now depend only on the caster's class level.** Before, they also depended on Magery, armor and Intelligence gear: durations ranged from about 3 to 16 minutes, Antidote could show levels like 20 (anything above 6 does nothing extra), and Sanctuary gave Powerplayers only level 1. Now every caster gets the same numbers for their class level, and unclassed casters count as level 1:

| Caster class level | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| Poison immunity | 1 | 2 | 3 | 4 | 5 | 6 |
| Duration (minutes:seconds) | 2:00 | 3:36 | 5:12 | 6:48 | 8:24 | 10:00 |

- **Level 6 blocks every poison in the game except a thief's**, the same as the best poison immunity jewelry.
- **Curing works as before.** Sanctuary always cures you; Antidote's cure still needs a successful skill check.
- **The spell and your jewelry don't add together.** You get whichever is higher. If you already have equal or better immunity from jewelry, casting it again does nothing; a weaker or equal spell immunity is replaced and its timer starts over.
- **Dispel and Mass Dispel now remove it**, like Bless or Protection. Logging out also ends it.
- **New buff icon** showing the spell's poison immunity and how long it has left. `.prots` and tooltips include it while it is active.
- **No more permanent immunity after a server restart.** A restart while the spell was active used to leave the immunity on for good.

---

## Dragon Skin - Reworked

### Player Impact

- **Dragon Skin is now a timed buff, not a song you keep playing.** Play it once and every party member and pet in range and in line of sight gets its protection. You are free to move straight away. It costs **20 stamina for each person protected**, and stops protecting more people when you run out.
- **Protection and duration depend on your bard level.** The duration is the same as Sanctuary and Antidote:

| Bard level | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| Physical Protection | 1 | 2 | 3 | 4 | 5 | 6 |
| Duration (minutes:seconds) | 2:00 | 3:36 | 5:12 | 6:48 | 8:24 | 10:00 |

- **It no longer adds to your equipment.** You get whichever is higher, your best Physical Protection item or Dragon Skin. For example, level 6 gear with a level 6 bard used to give 12; it now gives 6.
- **Someone already protected by a stronger Dragon Skin keeps theirs.** An equal or weaker one is replaced and its timer starts over.
- **Dispel and Mass Dispel remove it.** Logging out also ends it.
- **New buff icon** showing your Dragon Skin protection and how long it has left.
- **Bugs fixed:**
  - When the bard fumbled the song, everyone kept the protection permanently, and it could build up after server restarts.
  - People who walked out of range, died or hid kept the protection while the song played.
  - Changing equipment during the song could leave you with less protection than your gear gives.
- **Leftover protection is cleared.** When you log in, your Physical Protection and poison immunity are reset to what your equipment gives, which removes anything the old spells left behind.

---

## Reactive Armor

### Player Impact

- **Dispel and Mass Dispel now remove Reactive Armor.**
- **New buff icon** for Reactive Armor. It disappears when the armor triggers or wears off.
- A server restart no longer leaves Reactive Armor on until your next logout.

---

## Beastal Bond - No More Getting Stuck

### Player Impact

- **Fumbling while viewing your bonded creature no longer traps you.** You used to stay frozen and invisible at the creature's location, and "Leave Mind" did nothing. The song now ends and you are put back where you started playing.
- **Logging out or reconnecting ends the bond** and puts you back where you started playing. Before, you came back at the creature's location.
- Fixed a case where you were not moved back at all if the creature stood on the same row or column you started from.

---

## Buff Icons - Night Sight, Poison and Magic Reflection

### Player Impact

- **Night sight now shows a buff icon** with the time it has left, from Night Sight, Light of Day, Owl Sight, Song of Light and the night sight potion. Being blinded removes it, since blindness overrides night sight.
- **Poison now shows a "Poisoned" icon** with the time it has left. It disappears when you are cured or the poison wears off.
- **Magic Reflection now shows an icon** until it reflects a spell. It has no countdown, because the reflection lasts until it is used, and it comes back when you log in if your reflection is still up.

---

## Monsters, Pets and Area Spells

### Player Impact

- **Monsters no longer heal and bless you.** A spellcasting monster aimed every spell at the player it was fighting, so one that knew Heal, Cure, Bless, Protection, Reflect, Night Sight or another helpful spell cast it on you. It now casts helpful spells on itself.
- **Spellcasting monsters waste fewer casts.** A bug made part of every caster's attempts do nothing: half of them for a monster with one spell, a fifth for a monster with four. Every attempt now casts a spell, so casters hit a little more often.
- **A monster's area spells hit players and pets, not other monsters.** This covers Meteor Swarm, Chain Lightning, Earthquake and every other area spell or song a monster casts. Its area buffs, such as Arch Protection and Arch Cure, now only help other monsters. Before, a monster's area attack could hurt the monsters next to it, and its area buffs could land on you.
- **Boss mass attacks now hit pets too.** The special mass attacks that some bosses and champions use, such as the gusts, the mass ice strikes and the champion's elemental blast, used to hit only players. Pets in range now take them as well.
- **A pet's area spells follow your rules.** They never hit you, your party or your party's pets, and they follow the same faction, no-PK area and clan war rules as your own spells. A pet's area buffs help you, your party, players of your faction and other pets.
- **Town NPCs are left out.** Townsfolk, guards, merchants, healers, shrines and player vendors are never hit or buffed by a monster's or a pet's area spells. Before, a pet's area spell in town could hurt them, and a monster's buffs could help the guards fighting it. Friendly creatures such as pixies, wisps and unicorns count as town NPCs here too, so your pet's area spells won't hit them.
- **Monsters can sing.** Staff can now give monsters bard songs. A singing monster needs no instrument, and its song strength comes from its Musicianship. Song of Beckon stays players-only.

---

## Bards - Song of Sirens

### Player Impact

- **Running out of stamina now stops the song.** You got the "Out of Stamina!" message, but everyone in range was still paralyzed and you paid no stamina. Now the song fails.

---

## Town NPCs - New "Townsfolk" Creature Type

### Player Impact

- **Merchants, guards, townsfolk, nobles, healers, bankers, quest givers and Warriors for Hire are now the Townsfolk creature type** instead of Human.
- **Tracking has a new Townsfolk category**, so town NPCs no longer fill up the Human list.
- **Human slayer weapons and Human protection no longer count against them.** Both still work against human monsters such as brigands, and against players.
- **Bandages still heal them with Healing**, as before.

---

## Warrior for Hire - Attacking Another Warrior

### Player Impact

- **Your warrior now fights the warrior you tell it to attack.** If you told your warrior to attack another player's Warrior for Hire, it soon switched to that warrior's owner and chased them until you said stop. The other warrior did the same to you. Now the two warriors fight each other.
- **Your warrior no longer turns on you mid-fight** when one of your own area spells hits it.

---

## New Command - .arrows

### Player Impact

- **`.arrows` shows the ammunition you are carrying**, the way `.pagans` shows reagents. It counts arrows, fire arrows, ice arrows, bolts and thunder bolts in your backpack and the bags inside it.

---

## Player Vendors - City Wages

### Player Impact

- **Vendors in a city now pay the city wage.** Vendors were meant to cost more in a city, but a bug charged every vendor 10,000 gold a month. A vendor in a city now costs **50,000 gold a month** (about 1,666 a day). Everywhere else, including the Vendor Markets, it stays 10,000 (about 333 a day).
- **Wages come out of the gold your vendor holds**, from sales and from gold you hand it. If it runs short, the unpaid wages build up as debt, and after two months in debt the vendor leaves. Make sure your city vendors have enough gold.
- **Champion relics can no longer be used in a city.** The relic always said so, but the check never worked.

---

## Vortex Beast

### Player Impact

- **The Vortex Beast is now a Beholder.** Beholder slayer weapons do double damage to it, and Giantkin slayers no longer do. Its Vortex Minions are still Giantkin.

---

## Summary

- Mayors can resign from the townstone with "Resign and leave town"; the town can call a new election straight away.
- The personal powerhour weekly lock no longer comes back on the same weekday a week later; `.setph`, `.ph` and the Eon-Prism agree.
- Warrior hearts from before Patch 1.1.4 no longer revive; only a warrior's owner can hand in its heart; a High Priest memory revival costs one point of each stat per death, like a heart.
- The Eon-Prism works only from your backpack.
- Empty keyrings sell for 1 gold through "sell all" or "sell bag"; keyrings with keys on them are refused.
- Sanctuary and Antidote no longer switch off your poison immunity jewelry when they end; Antidote protects its target; immunity (1-6) and duration (2-10 minutes) follow the caster's class level only; Dispel removes them.
- Dragon Skin is a timed buff (protection 1-6 and 2-10 minutes by bard level, 20 stamina per person); it uses the higher of your gear and the song instead of adding; Dispel removes it; the stuck and stacking bugs are fixed.
- Reactive Armor can be dispelled and has a buff icon.
- Beastal Bond no longer leaves you frozen and invisible after a fumble; logging out ends the bond and puts you back where you started.
- Night sight, poison and Magic Reflection show buff icons.
- Monsters cast helpful spells on themselves instead of on you, and no longer waste part of their casts.
- A monster's area spells hit players and pets but not other monsters, and its buffs only help monsters; boss mass attacks now hit pets too.
- A pet's area spells follow your party, faction and no-PK rules; town NPCs and friendly creatures are never hit or buffed by an NPC's area spells.
- Staff can give monsters bard songs; Song of Beckon stays players-only.
- Song of Sirens fails when you run out of stamina instead of paralyzing everyone anyway.
- Town NPCs are the new Townsfolk creature type: their own Tracking category, and Human slayers and Human protection no longer count against them.
- Warriors for Hire told to attack another warrior fight that warrior instead of its owner.
- New `.arrows` command counts your arrows and bolts.
- Player vendors in a city now pay the city wage, 50,000 gold a month; elsewhere it stays 10,000. Champion relics can't be used in a city.
- The Vortex Beast is a Beholder for slayer weapons.

Thanks for playing Zuluhotel Omega 2.5.
