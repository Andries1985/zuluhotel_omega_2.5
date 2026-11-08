// npcspellnames.inc - every spell name an npc can cast, for staff tools like newmobedit.
//
// These are the names castspell() in npccast.inc accepts: its special cases (mass spells,
// summons, champfire) plus every name in GetSpellID(). The aliases meteorswarm and gustoair
// are left out in favour of meteor_swarm and gustofair. reactivearmor, earthportal and shapeshift
// are left out too: their scripts have no "#MOB" branch, so an npc can't cast them. songofbeckon
// is players only and isn't in GetSpellID(). Keep this in sync when a spell is added to npccast.inc.

function GetNpcSpellNames()

	return array{
		"abyssalflame", "agility", "angelicaura", "animate_dead", "antidote", "apocalypse",
		"archcure", "archprot", "astralstorm", "blade_spirit", "bless", "calllightning",
		"chainlightning", "champfire", "clumsy", "controlundead", "cunning", "cure",
		"curse", "darkness", "decayingray", "dispel", "dispel_field", "divinefury",
		"earthblessing", "earthquake", "earthspirit", "ebolt", "energy_field",
		"enlightenment", "explosion", "feeblemind", "fireball", "firefield", "flamespirit",
		"flamestrike", "gheal", "gluttons", "gustofair", "harm", "heal",
		"holybolt", "icestrike", "invisibility", "kill", "liche", "lightning",
		"magicarrow", "manadrain", "manavamp", "masscalllightning", "masscurse", "massdispel",
		"masseld", "massgust", "massholybolt", "massicestrike", "masskill", "massshiftingearth",
		"massvatten", "masswyvern", "meteor_swarm", "mindblast", "naturestouch", "nightsight",
		"owlsight", "parafield", "paralyze", "plague", "poison", "poisonfield",
		"polymorph", "protection", "reflect", "resurrect", "reveal",
		"risingfire", "sacrifice", "sanctuary", "seraphimswill", "shiftingearth",
		"songofair", "songofcloaking", "songofdefense", "songofdismissal", "songofearth",
		"songoffire", "songoffright", "songofglory", "songofhaste", "songoflife", "songoflight",
		"songofremedy", "songofsalvation", "songofsirens", "songofwater", "sorcerersbane", "spectretouch",
		"spellbind", "stormspirit", "strength", "summon_air", "summon_creature", "summon_daemon",
		"summon_earth", "summon_fire", "summon_spirit", "summon_water", "summondrag", "summonguardian",
		"summonmammals", "summonsgustofair", "summonsicestrike", "summonskel", "summonskelarmy", "summonsrisingfire",
		"summonsshiftingearth", "summonsspectretouch", "superspider", "teleport", "vortex", "wallofstone",
		"waterspirit", "weaken", "wraithbreath", "wraithform", "wrathofgod", "wyvernstrike"
	};

endfunction
