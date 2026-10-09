# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## Server-Wide Powerhour - Fixes

### Player Impact

- **A server-wide powerhour no longer gets stuck on after a restart.** Until now, if the server restarted while a server-wide powerhour was running, the powerhour's bonus stayed switched on with nothing left to switch it off - sometimes for up to a week, until the next weekly powerhour cleaned it up. During that time the Eon-Prism also refused to work, because it will not run while a server-wide powerhour is active. The powerhour's end time is now saved with the world, so a restart simply resumes it and it ends when it should. If one happens to be running when this patch goes live, it will be given a fresh full hour from the restart and then end normally.
- **The "second powerhour" bonus now follows the rule it was supposed to.** When the weekly powerhour ends there is a chance the Omega gods grant a second one. That chance was meant to be 50% after a half-resources powerhour and 10% after the other two, but because of a bug the game was rolling against a random type instead of the one that had actually run, so every week got the same blended odds. It is now 50% on half-resources weeks and 10% on hunting or double-skillgain weeks. Across many weeks the overall number of bonus hours stays about the same - they just land where they were meant to.
- **Only the weekly powerhour can earn a bonus hour.** Powerhours started by staff (see below) and bonus hours themselves never spawn a follow-up, so there is still at most one bonus hour a week.
- **`.ph` tells you more.** While a server-wide powerhour is running it now shows how long is left on it, and during the two-minute warning before one starts it says a powerhour is about to start. The countdown to the next scheduled powerhour now skips zero parts (so "3 hours and 12 minutes" rather than "0 days, 3 hours, 12 minutes and 0 seconds").
- **The weekly schedule can now be changed by staff.** Nothing moves unless staff change it - the powerhour stays on Sunday at 19:00 GMT as before. If it is ever moved, `.ph` will show the new time, and any change will be announced.

---

## Personal Powerhour - Fixes

### Player Impact

- **No more being locked out of `.setph`.** If you opened the `.setph` window and pressed OK without picking one of the three powerhours, nothing started but the game still thought the window was open, and every later `.setph` said "You have the gump window open already!" until you logged out and back in. Pressing OK with nothing selected now just tells you to pick one, and you can try again straight away.
- **A staff reset or Eon-Prism no longer cuts a fresh powerhour short.** If your personal powerhour was reset by staff or by an Eon-Prism and you then started a new one, the old hour's timer was still ticking in the background and would end your *new* powerhour early when it ran out. The old timer now recognises that the powerhour it was watching is gone and leaves the new one alone.
- **`.ph`'s "you can start a new personal powerhour in ..." countdown is accurate for Sunday starts.** If you used your personal powerhour on a Sunday, the countdown could be up to a day short, telling you that you could start again at Sunday midnight when in fact the lock lasts a full week from when you used it. It now shows the real time.
- **Wording tidy-up.** `.setph` and `.ph` now use the same names for the three types everywhere: Hunting, Half-resources and Double skillgain.
- The Eon-Prism works exactly as before: it clears your weekly personal powerhour lock as long as you are not running a personal powerhour, a server-wide powerhour is not active, and you are not already free to start one anyway.

---

## Warrior for Hire - Brought In Line With ZH3.0

### Player Impact

- **Ten revivals instead of three.** Your Warrior for Hire can now be brought back from each of its first ten deaths, either with the heart that drops when it dies or through the High Priest's memory of it (see below). Both use up one of the ten. The eleventh death is final: no heart, no memory, and you'll need to hire a new warrior. A heart revival costs the warrior one point of Strength, Intelligence and Dexterity for each death so far.
- **The heart finds you even when you're offline.** If your warrior dies while you're logged out, the heart goes to your bank box instead of being lost.
- **A lost heart doesn't mean a lost warrior.** Every time your warrior dies, the High Priest remembers it. Say **"resurrect warrior"** to him and, for 10,000 gold, he rebuilds them from that memory with the same name, stats and skills. This is the same revival the heart would have given, not an extra one: the heart crumbles and the death still counts toward the ten. It only works if you don't already have a living warrior, and only for the most recent death. A heart kept back from an earlier death crumbles if you try to use it.
- **Lost gear goes into safekeeping.** When your warrior's corpse decays, whatever was still on it is bundled up and held for 90 days. This happens after every death, including the final one. Say **"recover belongings"** to the High Priest to buy it back for 20,000 gold, or ask him to dispose of it. These packages do not show in `.escrow`; the priest is the only way to get them.
- **Letting a warrior go is final.** **"give up warrior"** at the High Priest, or **"<name> release"** to the warrior itself, ends them for good rather than leaving a masterless warrior wandering about, and clears the priest's memory of them too. Their gear still goes into safekeeping and can be recovered from the priest for 90 days.
- **New command: "<name> status"** opens a window with your warrior's stats, hits, armour and protections, skills, worn equipment and how many deaths they have left. Your warrior keeps fighting, healing and following while the window is open. Staff can open it on any warrior, so they can see what you see when you report a problem.
- **Warriors now learn from you.** A new hire starts as a modest recruit (75 Strength, 50 Intelligence, 50 Dexterity, 50 in each fighting skill, 100 Healing) rather than a fully trained veteran, and gains skill while fighting at your side toward your own skill levels, up to 130. Stats keep growing hourly as before. Warriors you already own keep the stats and skills they have.
- **Hit points follow Strength, buffs included.** As for a player, a warrior's maximum hit points equal its Strength, its mana its Intelligence and its stamina its Dexterity. A Bless or the Staff of Nagash raises them for as long as the buff lasts.
- **"stop" works like it does for pets.** **"<name> stop"** or **"all stop"** ends the fight and stops your warrior guarding anyone. It keeps following you and only fights back if something attacks it. Tell it to guard or attack to put it back to work. Before, "stop" in the middle of a fight left it guarding you, so it went straight back in, and "stop" out of a fight made it stop following you.
- **Every bow and crossbow now counts as a ranged weapon.** Your warrior only recognised the plain bows, crossbows, heavy crossbows, fire bows and icebows. With anything else - the Dreams Bow, composite bows, yumis, repeating and thunder crossbows, the boss bows - it ran up and tried to melee. It now stands off and shoots with any of them, as long as it carries the right ammunition.
- **The Staff of Nagash buffs Warriors for Hire** in range, as it already did players and tamed pets.
- **Warriors' bandages grow with their Healing skill.** A warrior's bandage heal used to grow with its Strength, then broke completely at 200 Strength, counting buffs - a blessed warrior could stop healing altogether. It now grows with the warrior's Healing skill instead: normal size at 100 Healing, about 40% bigger at 130. Buffs no longer affect it. Bandages taken out of its pack while it is in the middle of applying one no longer heal it from wherever they ended up.
- **Warriors drink every heal potion, one at a time.** When badly hurt, a warrior drinks the weakest heal it is carrying: Heal and Greater Heal potions of any true-mage level first, then a Tamla. Before, it only recognised plain Heal and Greater Heal potions - a true mage's leveled ones and Tamlas just sat in its pack - and it drank a whole stack for a single heal. Heal and Greater Heal potions now heal it exactly what they heal a player; a Tamla heals it fully, and a true mage's two-dose Tamla leaves a plain Tamla behind. Poisoned potions are left alone.
- **A warrior that dies while buffed no longer comes back with the buff built in.** Its heart and the priest's memory recorded its stats with any Bless or other buff included, and a revival made those its permanent stats - so it came back stronger than it was and could be blessed again on top. They now record its real stats. A heart or memory saved before this patch keeps whatever it recorded.
- **Sparring is safer.** A warrior and its own master do half damage to each other.
- **No more mounts.** Warriors no longer ride; the ride, mount, dismount and transfer commands get a polite refusal. A warrior already on a mount keeps it until staff remove it.
- **Companion's Second Wind** is a new vanity shop item (10 vanity points) that resets your living warrior's death count to zero.
- Smaller fixes: a warrior told to follow a staff member now correctly refuses; "<name> showpack" works from three tiles away and tells you when you're too far; a warrior's protections from gear are capped one tier below the maximum (85); reviving a warrior from its heart now correctly records the new warrior as yours (before, the deed and the priest could insist you already had a henchman while pointing at the dead one); and handing the priest a heart while you already have a living warrior now gives the heart back, where before he kept it and stopped responding to everyone until a restart.
- Everything above takes effect for existing warriors on the next server restart.

---

## Town Guards - Innocent Pets No Longer Caught Up

### Player Impact

- When a guard was called, every tamed creature the guard checked *after* a criminal's pet was also treated as belonging to that criminal and got a guard of its own. Only the criminal's own pets are targeted now.

---

## Areas - Guards, Safe Zones and Area Rules

### Player Impact

- **Area rules cost the server far less to check.** Every time the game asks whether a spot is guarded, safe, no-PK, anti-magic or recall-blocked - which happens for every monster on every AI tick, every guard call, every region you walk into and every recall or gate - it used to re-read the area list and unpack two large stored tables. It now keeps those in memory and only refreshes when staff actually change something. One of the steadier background loads on the server is gone.
- **Staff area changes apply reliably.** Two bugs could leave the server enforcing old area rules. A change to an area's boundaries or name in the configuration could be ignored forever, even across restarts. And a brief hiccup reading the area settings during a world save could mark an area as having no rules at all - Britain with no guards, for instance - until staff next saved the area editor. Both are fixed: the area list is rebuilt on every server start, and a failed read is never remembered.
- **Calling the guards records the right caller.** When guards were called on a criminal, the record of who called them was found by looking the caller up by name among everyone online. Two characters with the same name could get the wrong account recorded, and a caller who logged out at that moment recorded nothing useful. It now records the caller directly.
- Staff: the `.areas` editor only writes the areas you changed and no longer prints a line for every area to the server console on Save.

---

## High Priest - Making Amends Costs At Least 1,000 Gold

### Player Impact

- If you have upset the High Priest, repairing the relationship costs your class level times 2,500 gold, never less than 1,000 and never more than 60,000, the same bounds his curse-removal price uses. Until now a bug meant a player with no class could buy forgiveness for a single coin, and asking him about your "relationship" told you so. Both now quote and charge the real price. Anyone with a class level pays exactly what they did before.

---

## Tracking - One Window Instead Of Two Menus

### Player Impact

- Using Tracking now opens a single window: categories down the left, the creatures in the chosen category down the right, with page buttons on both sides. You can switch between categories freely without using the skill again. Before, you got one category menu, then one creature menu, and had to start over to look at a different category.
- Picking a creature works exactly as it did: you get its direction every few seconds until you lose it.
- Tracking other players still works; "Players" is one of the categories.
- The old menus showed a small picture next to each category and creature. The new window shows names only.
- Behind the scenes the skill no longer throws away and re-reads the server's creature definitions every time it is used, which was a small but needless load on every use.

---

## Houses - Old Runes Can No Longer Drop You Into Someone Else's House

### Player Impact

- If you marked a rune on an upper floor of a house that was later demolished, and someone else then placed a house there, that rune could recall or gate you straight onto the floor of the new house even though you had no access to it. The house check looked for a house at the rune's height, found nothing that high, and let you through; the landing then put you on the floor below. Recall, gate, the Earth Portal and runebooks now check the house footprint regardless of height, so only the owner, co-owners and friends with permission can arrive inside.
- A rune or runebook entry that points into a house you may not enter now crumbles (or is removed from the book) when you try to use it, the same way runes to forbidden areas already do.
- Castle courtyards and static houses now count as "inside the house" for recalling, gating and marking. Owners and friends with the right permission are unaffected, and characters on the same account as a static house's owner can now travel in too (that was always meant to work and didn't).
- Teleport can no longer land you inside a house footprint you have no access to, and monsters can no longer teleport in either.
- If a recall destination is blocked you now get a message instead of silently staying put.
- Fixed on the way: gating to a static house checked the wrong permission, and the Earth Portal could leave you stuck "casting" after a refused target.

---

## Classes - Your Class Bonus Now Covers All Your Skills

### Player Impact

- **Every class skill now gets the class bonus.** A classed character is meant to gain faster, and to get a second chance on a failed skill check, on all eight of their class skills. Because of a long-standing bug, any skill that also belongs to another class was left out. Warriors only got it on Healing and Wrestling; Paladins only on Parry and Macefighting; Bladesingers, Mystic Archers and Rangers on half of theirs. Bards, Crafters, Mages and Thieves were unaffected. From this patch the bonus and the second chance apply to all eight skills of whatever class you are.
- **Powerplayers get their bonus a level sooner, and it keeps growing.** Before, Powerplayers gained 1.1x faster at level 3, 1.2x at level 4 and 1.3x at level 5, and nothing at levels 1, 2 or 6. Now it is 1.1x at level 2, 1.2x at level 3, 1.3x at level 4, 1.4x at level 5 and 1.5x at level 6, on every skill. Powerplayers do not get the second-chance roll; that stays a specialist perk.
- **Your class now shapes your stats.** Each class gains one or two stats a little faster and one or two a little slower, scaling with class level, from the skills you use. Warriors and Crafters: Strength up, Intelligence down. Mages: Intelligence up, Strength and Dexterity down. Bards: Dexterity and Intelligence up, Strength down. Bladesingers and Rangers: Dexterity up, Strength down. Mystic Archers: Dexterity and Intelligence up, Strength down. Thieves: Dexterity up, Strength and Intelligence down. Paladins: Intelligence up, Dexterity down. Powerplayers are neutral. Stat caps are unchanged and nothing already gained is touched.
- **If you somehow qualify for two classes, you are treated as the one you are strongest in** for prices, bonuses and gear rules. This is rare; it needs two closely related classes and a very narrow build.
- **Faster combat.** Every hit and every damaging spell used to pause while the game re-checked all your worn gear for class-illegal items. That check now only runs when your class is recalculated: at login, on the class commands, and on the ten-minute class recheck that already runs for everyone online. Expect combat to feel snappier.

---

## Staff Tools

### Player Impact

- Administrators have a new `.phadmin` window that can start a server-wide powerhour of a chosen type (always with the usual two-minute warning first), extend, shorten or end the one that is running, and change the day and time of the weekly powerhour. If staff change how long a running powerhour has left, the new end time is announced to everyone. These are all things that used to need a server restart or a code change, so expect server-wide powerhours to show up around events more often.
- The `.resetph` staff command added in Patch 1.1.2 for untangling a stuck personal powerhour was never actually reachable in-game because of a missing configuration line. It works now. If you were ever told "we'll reset it" and nothing happened, that was why.
- Developers have `.setwfhdamage`, which tunes how much damage Warriors for Hire deal to and take from ordinary monsters and from bosses. Everything is at 1.0 (no change) until staff decide otherwise.
- Developers have `.maxwfh`, which brings a targeted Warrior for Hire's fighting skills to the 130 cap and raises its strength, intelligence and dexterity to its owner's (or to values staff give), then refills it to its new full hit points. It is meant for fixing a warrior that lost progress, not for handing out free veterans.
- Seers and above have `.speedwalk`, which sets a run-speed modifier from 0 to 4 (no argument toggles it) and is restored every time they log in. It was written for this shard in April 2025 but never made it to live; this is ZH3.0's working version.
- Developers have `.classbonusinfo`, which targets a character and lists their classes, primary class, stat multipliers and every skill that carries a class bonus. Use it to check the class changes above on live characters.

---

## Summary

- Server-wide powerhours survive restarts properly and always end on time, instead of occasionally staying switched on for up to a week.
- The bonus second powerhour is now 50% likely after a half-resources week and 10% after the others, as originally intended; the long-run number of bonus hours is unchanged.
- `.ph` shows time remaining on an active server-wide powerhour and warns when one is about to start.
- `.setph` no longer locks you out if you press OK without choosing, a reset no longer ends your next powerhour early, and the Sunday countdown is correct.
- Staff can start, adjust, end and reschedule server-wide powerhours with `.phadmin`; `.resetph` now actually works.
- Warrior for Hire: ten revivals in total, by heart or by the High Priest's memory (10,000 gold), then gone for good; the heart reaches your bank box when you're offline; the priest returns a dead or released warrior's gear (20,000 gold); a "status" window, skills that grow toward yours, hit points that rise with Strength buffs, "stop" that ends guarding, every bow recognised, no mounts, and a vanity item that resets the death count.
- Calling the guards no longer also targets innocent pets scanned after a criminal's pet.
- Area rule checks (guards, safe zones, no-PK, anti-magic, recall blocks) no longer re-read the config on every check; staff area edits apply reliably after a restart; guard calls record the right caller.
- Repairing your relationship with the High Priest now costs at least 1,000 gold; a classless player could previously do it for one coin.
- Tracking opens one window with categories and creatures side by side and page buttons, instead of two successive menus. Player tracking is kept; the little icons are gone.
- Staff: `.speedwalk` (Seer and above) sets a run-speed modifier that is kept across logins.
- Runes marked in a demolished house can no longer drop you into the house that replaced it; refused runes crumble; courtyards and static houses now count as inside the house.
- Class bonuses now apply to all eight of your class skills (Warriors and Paladins had them on only two); Powerplayers get their skill-gain bonus from level 2, up to 1.5x at level 6; each class now grows some stats faster and others slower; combat no longer re-checks your gear on every hit.

Thanks for playing Zuluhotel Omega 2.5.
