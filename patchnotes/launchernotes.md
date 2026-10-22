# Latest Changes
Always check Discord announcements for all the patch notes.

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
- **The spell and your jewelry don't add together.** You get whichever is higher. If you already have equal or better immunity from jewelry, casting it again does nothing; a weaker or equal spell immunity is replaced and its timer starts over.
- **Dispel and Mass Dispel now remove it**, like Bless or Protection. Logging out also ends it.
- **New buff icon** showing the spell's poison immunity and how long it has left. `.prots` and tooltips include it while it is active.
- **No more permanent immunity after a server restart.** A restart while the spell was active used to leave the immunity on for good.

---

## Dragon Skin - Reworked

### Player Impact

- **Dragon Skin is now a timed buff, not a song you keep playing.** Play it once and every party member and pet in range and in line of sight gets its protection. You are free to move straight away. It costs **20 stamina for each person protected**, and stops protecting more people when you run out.
- **Protection and duration depend on your bard level:**

| Bard level | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| Physical Protection | 1 | 2 | 3 | 5 | 7 | 9 |
| Duration (minutes) | 15 | 18 | 21 | 24 | 27 | 30 |

- **It no longer adds to your equipment.** You get whichever is higher, your best Physical Protection item or Dragon Skin. For example, level 6 gear with a level 6 bard used to give 12; it now gives 9.
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

## Summary

- Mayors can resign from the townstone with "Resign and leave town"; the town can call a new election straight away.
- The personal powerhour weekly lock no longer comes back on the same weekday a week later; `.setph`, `.ph` and the Eon-Prism agree.
- Warrior hearts from before Patch 1.1.4 no longer revive; only a warrior's owner can hand in its heart; a High Priest memory revival costs one point of each stat per death, like a heart.
- The Eon-Prism works only from your backpack.
- Empty keyrings sell for 1 gold through "sell all" or "sell bag"; keyrings with keys on them are refused.
- Sanctuary and Antidote no longer switch off your poison immunity jewelry when they end; Antidote protects its target; Dispel removes them.
- Dragon Skin is a timed buff (1/2/3/5/7/9 protection, 15-30 minutes, 20 stamina per person); it uses the higher of your gear and the song instead of adding; Dispel removes it; the stuck and stacking bugs are fixed.
- Reactive Armor can be dispelled and has a buff icon.
- Beastal Bond no longer leaves you frozen and invisible after a fumble; logging out ends the bond and puts you back where you started.

Thanks for playing Zuluhotel Omega 2.5.
