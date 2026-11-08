# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## Gathering - Only What Is Really There

### Player Impact

- **The game checks the real ground, not what your client reports.** When you target a tree, a rock, sand, a swamp or water, the server now looks at the map and the objects really at that spot. A spot that has nothing to gather says so: "You can't get any wood from that." or "You can't mine or dig anything there."
- **This covers every gathering skill.** Lumberjacking, carving kindling with a blade, mining, digging clay and sand, fishing with a pole or a net, filling jugs and pitchers, cleaning bandages and planting seeds.
- **Other lands work properly.** Mining, digging, fishing, seed planting and filling an empty pitcher outside Britannia read the map of the land you are standing in. Before, some of them read the Britannia map at the same coordinates.
- **Clicking the ground at the foot of a tree still works.** You don't have to hit the trunk exactly: anything gatherable in the square you click counts.

---

## Lumberjacking and Kindling

### Player Impact

- **More trees give wood:** coconut and date palms, yew trees, some leafy tree tops and the Tuscany pine.
- **Plants that aren't trees no longer give wood.** Grasses, ferns, rushes, weeds, cattails, pampas grass, snake plants, ponytail palms and flowers were on the old list. The juniper bush still gives wood.
- **Yucca still gives no wood.**
- **Trees you plant still give wood**, and so do decoration trees. Chopping a tree grown from a tree seed, or carving kindling from it, works as before.
- Carving kindling with a blade uses the same tree list.

---

## Mining, Clay and Sand

### Player Impact

- **More cave floors give ore.** All the dungeon cave floor tiles now count, along with a few mountain and dirt tiles that were missing.
- **Dungeon floors give ore, not clay.** On some dungeon floors your pickaxe dug clay instead of ore. Those spots now give ore.
- **The forest tile at the Occlo bank no longer gives ore.**
- **Clay comes only from swamp.** Some grass and cave tiles used to give clay. They no longer do.
- **The sand list matches real sand.** A few beach tiles that are sand were missing and now work; a few that aren't sand no longer do.
- When a spot has more than one resource, ore is checked first, then clay, then sand.

---

## Fishing

### Player Impact

- **You can only fish in real water.** Fishing with a pole or a net checks for real water at the spot.
- **The thin strip of water along the shoreline no longer counts as fishable.** You can still cast up to 6 tiles out from the shore into open water.
- **Waterfalls, fountains, troughs, tubs, barrels and buckets are not fishing spots.**
- **High-skill fishers no longer reel in nothing.** With more than 129 Fishing, about 1 cast in 7 at 150 skill landed no fish at all. Those casts now give a magic fish, the same as rolls of 100-129.
- **A fished-out spot says "There's not enough fish here."** Before, a spot could also refuse you now and then even when it still had fish.

---

## Water for Pitchers and Bandages

### Player Impact

- **More kinds of water work.** Waterfalls, fountains, troughs, tubs, barrels and buckets that are part of the map now fill pitchers and clean bandages, not only ones placed as items.
- **Water inside a container doesn't count.** A trough or barrel has to be out in the world.

---

## Farming

### Player Impact

- **Seeds only plant in real fertile dirt or farmland.** Fertile dirt placed as an item, or part of the map, still works.
- **Cancelling the seed target now simply cancels.**

---

## Health, Mana and Stamina Regeneration

### Player Impact

- **Your health regeneration no longer gets stuck at 1 point a minute.** When poison or a temporary spell effect wore off, health regeneration could drop to 1 point a minute and stay there. About 1,400 characters were stuck like this. It now goes back to normal, and logging in fixes any character that is still stuck.
- **Taking off an item no longer lowers your regeneration.** Items with health, mana or stamina regeneration were never adding it when worn, but taking one off still took it away, for good. That is fixed, and logging in puts mana and stamina regeneration back to normal too. Wearing such an item still doesn't change your regeneration.
- **Creatures regenerate at their own rates.** Every creature has health, mana and stamina regeneration rates, but they were almost never applied. They are now:
  - 126 creatures heal faster than normal, among them the Tainted Shrines, Gluttons, Chief Paroxysmus, Mounds of Maggots and Kappas.
  - 68 creatures get mana back faster, among them balrons, golden dragons, fallen angels, the Dragon King and the Soul Whisperers.
  - The five champion Spawns (of the Earth, Storm, Fire, Water and Dead) heal slower than normal.
  - Pets keep their own creature's rates.

---

## Pets

### Player Impact

- **Pets pay mana for their special spells.** A tamed creature with a special spell (the water dragon's mass spells, mass Gust of Air, Champfire, the summoning specials and others) now pays the mana of the normal spell it copies. Before, pets cast them for free, without end. Summoned creatures, such as the earth magic spirit elementals, still cast them for free.
- **Tamed spellcasters wait between spells.** Wild casters wait 2 seconds between spells, but a tamed caster could cast on every step of its fight. Tamed casters now wait 2 seconds too.

---

## Karma and Fame

### Player Impact

- **Each creature has a set karma and fame value.** What a kill is worth no longer comes from a formula based on the creature's stats, armour and weapons. Every creature now has its own value, in steps of 100, 750, 1,500, 3,000, 6,000, 11,000 and 15,000.
- **Evil creatures give karma; neutral creatures give fame only.** Before, a typo made every creature count as evil, so killing a deer or a rabbit raised your karma. Neutral animals and creatures now give fame but no karma.
- **Killing townsfolk or good creatures costs you.** Killing a townsperson, peasant, noble, castle guard and the like, or a good creature, takes 250 karma and 100 fame and gives nothing.
- **How much you gain is unchanged.** You still gain more from creatures far above your own karma and fame and less as you catch up, and a kill shared by several players is split between them.
- Only players gain karma and fame from kills: hired warriors and other creatures that join a fight no longer do.

---

## Control Undead and Spell Bind

### Player Impact

- **A freed undead no longer attacks in your name.** When Control Undead ran out, the creature stayed linked to you. Anyone it attacked saw "*Your Name* is attacking you!", and you could be flagged as the aggressor, or as a criminal against an innocent. Freed creatures are now fully wild again, and those already roaming the world are fixed when they next wake.
- **Control ends on time, even across a server restart.** Before, a restart while a creature was under Control Undead or Spell Bind left it as your pet for good. Casting Control Undead again on your own creature had the same effect. The end time is now kept on the creature.
- **Releasing a controlled or bound creature in town makes it vanish.** Before, it was confiscated like a pet, and the Animal Trainer gave it back as a real pet. The same happens if you order it to attack in a safe area. A pet you took from another player with Spell Bind goes back to its owner instead.
- **When Spell Bind ends on someone's pet, it goes back to its owner.** Before, it went wild and the owner lost it.
- **Pets of players who are offline are protected too.** Spell Bind now checks their owner's taming or magery as well, the same as for players who are online.
- **These can no longer be taken:** a creature that already has a master (someone's pet, summon or animated dead), a creature already under Control Undead or Spell Bind, a Warrior for Hire, and champion spawn creatures.
- When a spell takes a creature from a spawn, the spawn makes a new one, as before.

---

## Logging In - Too Many Accounts From One Connection

### Player Impact

- **Logging in no longer freezes for everyone.** You can have 2 accounts online from one connection. Before, a third one still got into the world and was then kicked. Each kick stopped the server letting anyone log in for about 2½ minutes. Game clients that reconnect on their own kept repeating it, and on November 8 almost nobody could log in for about three hours.
- **The extra account is now refused at the login screen**, with the message "The IGR concurrency limit has been met". Nobody is kicked, and nobody else has to wait.
- **The limit itself is unchanged:** 2 accounts at once per connection. Staff and accounts that staff have given an extra login are not limited. Logging the same account back in after a disconnect still works.
- Once an account passes the login screen, it keeps its place for up to 2 minutes while you choose a character. If you cancel there, your other accounts on the same connection can be refused for up to 2 minutes.
- Login error messages now give the real reason. Before, every refusal said "Incorrect name/password".

---

## Summary

- Every gathering skill (lumberjacking, kindling, mining, clay, sand, fishing, nets, pitchers, bandages, seeds) checks what is really at the spot instead of trusting the client, and reads the right map outside Britannia.
- Wood: palms, yews, some leaves and the Tuscany pine added; grasses, ferns, flowers and other plants removed; yucca stays out; planted and decoration trees still give wood.
- Mining: more cave floors give ore; dungeon floors give ore instead of clay; the Occlo bank forest tile gives nothing; clay only from swamp; sand list corrected.
- Fishing: only real fishable water; shoreline strips, waterfalls, fountains and troughs are out; casts above 129 skill no longer come up empty.
- Pitchers and bandages accept waterfalls, fountains, troughs, tubs, barrels and buckets on the map.
- Seeds plant only in real fertile dirt or farmland.
- Health regeneration no longer gets stuck at 1 a minute, and taking off items no longer lowers regeneration; logging in repairs both.
- Creatures regenerate health and mana at their own rates.
- Pets pay mana for special spells, and tamed casters wait 2 seconds between spells.
- Every creature has a set karma and fame value: evil gives karma, neutral gives fame only, townsfolk and good creatures cost 250 karma and 100 fame.
- Control Undead and Spell Bind: freed creatures no longer attack in your name, control ends on time even after a restart, no more permanent pets from town releases, stolen pets go back to their owner, and champion spawns can't be taken.
- A third account from one connection is refused at the login screen instead of being kicked from the world, which froze logins for everyone for about 2½ minutes each time.

Thanks for playing Zuluhotel Omega 2.5.
