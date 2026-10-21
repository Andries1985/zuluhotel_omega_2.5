# Patch Notes - v1.1.5
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: October 20, 2026**

---

Welcome to **Patch 1.1.5**, a small follow-up to 1.1.4. Mayors can finally step down. Empty keyrings can be sold. The personal powerhour no longer locks you out for part of the day a week after you used it. And a few Warrior for Hire heart and High Priest loopholes are closed. **If you are holding a warrior heart from before Patch 1.1.4, read the Warrior for Hire section.**

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

## Summary

- Mayors can resign from the townstone with "Resign and leave town"; the town can call a new election straight away.
- The personal powerhour weekly lock no longer comes back on the same weekday a week later; `.setph`, `.ph` and the Eon-Prism agree.
- Warrior hearts from before Patch 1.1.4 no longer revive; only a warrior's owner can hand in its heart; a High Priest memory revival costs one point of each stat per death, like a heart.
- The Eon-Prism works only from your backpack.
- Empty keyrings sell for 1 gold through "sell all" or "sell bag"; keyrings with keys on them are refused.

Thanks for playing Zuluhotel Omega 2.5.
