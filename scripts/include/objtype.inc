/////////////////////////////////////////////////////////////////////////////
//
// include/objtype.inc - Object Type definitions
//
// TODO: Put some order in there, this is a true mess
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// 0x0000 to 0x3FFF - "Standard" objects
//
/////////////////////////////////////////////////////////////////////////////

const UOBJ_STONEWALL_NS := 0x0058;
const UOBJ_STONEWALL_EW := 0x0057;

const UOBJ_FISHSTEAK    := 0x097A;

const UOBJ_BOX          := 0x09aa;
const UOBJ_GOLD_COIN   := 0x0EED; // A gold coin

const UOBJ_FISH_START   := 0x09CC;
const UOBJ_FISH_END     := 0x09D0;

const UOBJ_SFISH_START  := 0x0DD6;
const UOBJ_SFISH_END    := 0x0DD9;

const UOBJ_HAIRS_END			:= 0x20d4;
const UOBJ_HAIRS_START			:= 0x203b;

const UOBJ_KINDLING     := 0x0DE1;
const UOBJ_CAMPFIRE     := 0x0DE3;   
const UOBJ_EMBERS       := 0x0DE9;       
const UOBJ_ASHES        := 0x0DEA;

const UOBJ_BANDAGE      := 0x0E21;
const UOBJ_BLANK_SCROLL := 0x0E34;

const UOBJ_BANKBOX      := 0x0E7C;

const UOBJ_DRUM         := 0x0E9C;
const UOBJ_TAMB         := 0x0E9D;
const UOBJ_HARP         := 0x0EB2;
const UOBJ_LUTE         := 0x0EB3;

const UOBJ_SPELLBOOK        := 0x0EFA;

const UOBJ_POISON_POTION    := 0x0F0A;
const UOBJ_EMPTY_BOTTLE     := 0x0F0E;

const UOBJ_GEM_START        := 0x0F0F;
const UOBJ_GEM_END          := 0x0f30;

const UOBJ_SHOVEL           := 0x0F39;

const UOBJ_XBOW             := 0x0F4F;

const UOBJ_BLACK_MOONGATE   := 0x1FD4;
const UOBJ_BLACK_MOONGATE2  := 0x7012;
const UOBJ_BLUE_MOONGATE    := 0x0F6C;
const UOBJ_BLUE_MOONGATE2   := 0x7010;
const UOBJ_RED_MOONGATE     := 0x0DDA;
const UOBJ_RED_MOONGATE2    := 0x7011;
const UOBJ_WHITE_MOONGATE   := 0x1FE7;
const UOBJ_WHITE_MOONGATE2  := 0x7013;

const UOBJ_CLAY		    := 0xee19;
const UOBJ_SAND		    := 0xee18;
const UOBJ_WOOL		    := 0x0df8;


const UOBJ_REAGENT_START    := 0x0f78;
const UOBJ_REAGENT_END	    := 0x0f91;

const UOBJ_BATWING	    := 0x0F78;
const UOBJ_BLACKMOOR	    := 0x0F79;
const UOBJ_BLACK_PEARL      := 0x0F7A;
const UOBJ_BLOOD_MOSS       := 0x0F7B;
const UOBJ_BLOODSPAWN	    := 0x0F7C;
const UOBJ_VIAL_OF_BLOOD    := 0x0F7D;
const UOBJ_BONE		    := 0x0F7E;
const UOBJ_BRIMSTONE        := 0x0F7F;
const UOBJ_DAEMON_BONE	    := 0x0F80;
const UOBJ_FERTILE_DIRT     := 0x0F81;
const UOBJ_DRAGONS_BLOOD    := 0x0F82;
const UOBJ_EXECUTIONERS_CAP := 0x0F83;
const UOBJ_GARLIC           := 0x0F84;
const UOBJ_GINSENG          := 0x0F85;
const UOBJ_MANDRAKE_ROOT    := 0x0F86;
const UOBJ_EYE_OF_NEWT	    := 0x0F87;
const UOBJ_NIGHTSHADE       := 0x0F88;
const UOBJ_OBSIDIAN         := 0x0F89;
const UOBJ_PIG_IRON         := 0x0F8A;
const UOBJ_PUMICE	    := 0x0F8B;
const UOBJ_SULPHUROUS_ASH   := 0x0F8C;
const UOBJ_SPIDER_SILK      := 0x0F8D;
const UOBJ_SERPENTS_SCALES  := 0x0F8E;
const UOBJ_VOLCANIC_ASH     := 0x0F8F;
const UOBJ_DEAD_WOOD	    := 0x0F90;
const UOBJ_WORMS_HEART	    := 0x0F91;


const UOBJ_SEWING_KIT   := 0x0F9D;
const UOBJ_CLOTH        := 0x0F95;         

const UOBJ_THREAD_START := 0x0FA0;            
const UOBJ_THREAD_END   := 0x0FA1;
const UOBJ_BOLT_START   := 0x0F95;
const UOBJ_BOLT_END     := 0x0F9C;
const UOBJ_MUTLI_CLOTH  := 0x1765;

const UOBJ_HIDE1	:= 0x1078;
const UOBJ_HIDE2	:= 0x1079;

const UOBJ_HIDES_START	:= 0x7020;
const UOBJ_HIDES_END	:= 0x7030; // Missing Orc, Terathan, Angel and Silverdragon

const UOBJ_ANVIL1       := 0x0FAF;
const UOBJ_ANVIL2       := 0x0FB0;
const UOBJ_SMALL_FORGE  := 0x0FB1;  // see also UOBJ_FORGE_START..UOBJ_FORGE_END

const UOBJ_COPPER_KEY   := 0x100E;
const UOBJ_GOLD_KEY     := 0x100F;
const UOBJ_IRON_KEY     := 0x1010;
const UOBJ_KEY_RING     := 0x1011;
const UOBJ_MAGIC_KEY    := 0x1012;
const UOBJ_RUSTY_IRON_KEY := 0x1013;

const UOBJ_CLOCK		:= 0x104c;
const UOBJ_CLOCK_FRAME		:= 0x104e;
const UOBJ_CLOCK_PARTS		:= 0x104f;
const UOBJ_AXLE_AND_GEARS	:= 0x1051;
const UOBJ_GEARS		:= 0x1053;
const UOBJ_HINGE		:= 0x1055;
const UOBJ_SEXTANT		:= 0x1057;
const UOBJ_SEXTANT_PARTS	:= 0x1059;
const UOBJ_AXLE			:= 0x105b;
const UOBJ_SPRINGS		:= 0x105d;

const UOBJ_KEG		:= 0x0E7F;
const UOBJ_BARREL	:= 0x0E77;
const UOBJ_BARREL_TAP	:= 0x1004;
const UOBJ_BARREL_LID	:= 0x1DB8;
const UOBJ_BARREL_STAVE	:= 0x1EB1;
const UOBJ_BARREL_HOOPS	:= 0x10E1;

const UOBJ_POTION_KEG	:= 0x1940;

const UOBJ_LOOM_START   := 0x105F;
const UOBJ_LOOM_END     := 0x1066;            

const UOBJ_DUMMYSWING_NS     := 0x1071;
const UOBJ_DUMMYFREEZE_NS    := 0x1070;
const UOBJ_DUMMYSWING_EW     := 0x1075;
const UOBJ_DUMMYFREEZE_EW    := 0x1074;

const UOBJ_BLOOD        := 0x122D;

const UOBJ_BOW          := 0x13B2;
const UOBJ_HEAVY_XBOW   := 0x13FD;

const UOBJ_MAP1         := 0x14EB;
const UOBJ_MAP2         := 0x14EC;
const UOBJ_ROLLED_MAP1  := 0x14ED;
const UOBJ_ROLLED_MAP2  := 0x14EE;

const UOBJ_HOUSE_DEED1  := 0x14ef;
const UOBJ_SHIP_DEED1   := 0x14F1;
const UOBJ_SHIP_DEED2   := 0x14F2;

const UOBJ_CHEST        := 0x0E40;

const UOBJ_FORGE_START  := 0x197A;  // see also UOBJ_SMALL_FORGE          
const UOBJ_FORGE_END    := 0x19A9;

const UOBJ_FEATHER      := 0x1BD1;
const UOBJ_SHAFTS       := 0x1BD4;
const UOBJ_LOGS         := 0x1BDD;
const UOBJ_LOGS_GODDESS	:= 0x1BDE;
const UOBJ_LOGS_START	:= 0x7300;
const UOBJ_LOGS_END	:= 0x7316;

const UOBJ_CLOTH_START	:= 0x175D;
const UOBJ_CLOTH_END	:= 0x1768;

const UOBJ_CARPET_START	:= 0x0AD1;
const UOBJ_CARPET_END	:= 0x0AD9;

const UOBJ_GLASS        := 0xee20;

const UOBJ_ARM          := 0x1CEC;
const UOBJ_LEG          := 0x1CE2;
const UOBJ_TRUNK        := 0x1CEE;
const UOBJ_HEART        := 0x1CEF;
const UOBJ_HEAD         := 0x1DAE;

const UOBJ_TINKER_TOOLS := 0x1EBC;


const UOBJ_RUNE			:= 0x1F14;
const UOBJ_GATE_SCROLL		:= 0x1f60;
const UOBJ_RECALL_SCROLL	:= 0x1f4c;
const UOBJ_CODEX_DAMNORUM	:= 0xa100;
const UOBJ_EARTH_BOOK		:= 0xb100;
const UOBJ_TOME_OF_ENCHANTMENT	:= 0xc100;
const UOBJ_RUNEBOOK		:= 0x7100;

const UOBJ_CORPSE       := 0x2006;

const UOBJ_PARAFIELD_NS   := 0x3967;
const UOBJ_PARAFIELD_EW   := 0x3979;
const UOBJ_POISONFIELD_NS := 0x3915;
const UOBJ_POISONFIELD_EW := 0x3920;
const UOBJ_EFIELD_EW      := 0x3955;
const UOBJ_EFIELD_NS      := 0x3946;
const UOBJ_FIREFIELD_NS   := 0x398c;
const UOBJ_FIREFIELD_EW   := 0x3996;

const GID_PLANK_EXTENDED_FACING_WEST   := 0x3ed5;
const GID_PLANK_RETRACTED_FACING_WEST  := 0x3eb1;
const GID_PLANK_EXTENDED_FACING_EAST   := 0x3ed4;
const GID_PLANK_RETRACTED_FACING_EAST  := 0x3eb2;
const GID_PLANK_EXTENDED_FACING_SOUTH  := 0x3e84;
const GID_PLANK_RETRACTED_FACING_SOUTH := 0x3e85;
const GID_PLANK_EXTENDED_FACING_NORTH  := 0x3e89;
const GID_PLANK_RETRACTED_FACING_NORTH := 0x3e8a;

// ick! these are fine for humans, but not true for items..
// these are more of a CID.
const UOBJ_HUMAN_MALE   := 0x190;
const UOBJ_HUMAN_FEMALE := 0x191;

/////////////////////////////////////////////////////////////////////////////
//
// 0x4000 to 0x4FFF - Multis
//
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
// 0x5000 to 0xEFFF - To Be Determined
//
/////////////////////////////////////////////////////////////////////////////

	//
	// TODO: reserve some of these ranges for weapons, armor, wands
	//       explicitly leave an area for end users to play with
	//

// recommendation: start these out somewhere like 0x6300, and reserve
// enough space for N (say, 64) colors of ore and ingots
// they don't all have to be defined yet, just reserved.
const UOBJ_ORE_START	    := 0x6009;
const UOBJ_IRON_ORE         := 0x6009;
const UOBJ_BRONZE_ORE       := 0x600a;  
const UOBJ_DULLCOPPER_ORE   := 0x600b;
const UOBJ_COPPER_ORE       := 0x600c;
const UOBJ_ONYX_ORE         := 0x600d;
const UOBJ_PYRITE_ORE       := 0x600e;
const UOBJ_MALACHITE_ORE    := 0x600f;
const UOBJ_AZURITE_ORE      := 0x6010;
const UOBJ_ORE_END	    := 0x6010;

// Added by GM Alcatraz - Zuluhotel.dynip.com
const UOBJ_ORE2_START	    := 0xc500;
const UOBJ_PLATINUM_ORE     := 0xc500;
const UOBJ_LAVAROCK_ORE     := 0xc501;
const UOBJ_MYSTIC_ORE       := 0xc502;
const UOBJ_ZULU_ORE         := 0xc503;
const UOBJ_SPIKE_ORE        := 0xc504;
const UOBJ_FRUITY_ORE       := 0xc505;
const UOBJ_GREYVIXEN_ORE    := 0xc506;
const UOBJ_SILVERROCK_ORE   := 0xc507;
const UOBJ_SPECTRAL_ORE     := 0xc508;
const UOBJ_UNDEAD_ORE       := 0xc509;
const UOBJ_DARKPAGAN_ORE    := 0xc50a;
const UOBJ_OLDBRITAIN_ORE   := 0xc50b;
const UOBJ_VIRGINITY_ORE    := 0xc50c;
const UOBJ_BLACKDWARF_ORE   := 0xc50d;
const UOBJ_REDELVEN_ORE     := 0xc50e;
const UOBJ_ORE2_END	    := 0xc50e;

const UOBJ_ORE3_START	    := 0xc530;
const UOBJ_DRIPSTONE_ORE    := 0xc533;
const UOBJ_EXECUTOR_ORE     := 0xc534;
const UOBJ_PEACHBLUE_ORE    := 0xc535;
const UOBJ_DESTRUCTION_ORE  := 0xc536;
const UOBJ_ANRA_ORE         := 0xc537;
const UOBJ_GODDESS_ORE      := 0xc538;
const UOBJ_CRYSTAL_ORE      := 0xc539;
const UOBJ_DOOM_ORE         := 0xc53a;
const UOBJ_SAPPHIRE_ORE     := 0xc53b;
const UOBJ_RUBY_ORE         := 0xc53c;
const UOBJ_DIAMOND_ORE      := 0xc53d;
const UOBJ_DAMASIN_ORE      := 0xc53e;
const UOBJ_KORGAN_ORE	    := 0Xc53f;
const UOBJ_AURORA_ORE	    := 0Xc540;
const UOBJ_ARASARN_ORE	    := 0Xc541;
const UOBJ_ORACLE_ORE	    := 0Xc542;
const UOBJ_VULCAN_ORE	    := 0Xc543;
const UOBJ_SHING_ORE	    := 0Xc544;
const UOBJ_LEVIATHAN_ORE    := 0Xc545;
const UOBJ_SANCTUARY_ORE    := 0Xc546;
const UOBJ_ORE3_END	    := 0xc546;
//
//
const UOBJ_GOLD_INGOT       := 0x1be9;
const UOBJ_INGOTS_START     := 0x6011;       
const UOBJ_IRON_INGOT       := 0x6011;
const UOBJ_BRONZE_INGOT     := 0x6012;  
const UOBJ_DULLCOPPER_INGOT := 0x6013;
const UOBJ_COPPER_INGOT     := 0x6014;
const UOBJ_ONYX_INGOT       := 0x6015;
const UOBJ_PYRITE_INGOT     := 0x6016;
const UOBJ_MALACHITE_INGOT  := 0x6017;
const UOBJ_AZURITE_INGOT    := 0x6018;
const UOBJ_INGOTS_END       := 0x6018;   
// Added by GM Alcatraz - Zuluhotel.dynip.com
const UOBJ_INGOTS2_START    := 0xc50f;
const UOBJ_PLATINUM_INGOT   := 0xc510;
const UOBJ_LAVAROCK_INGOT   := 0xc511;
const UOBJ_MYSTIC_INGOT     := 0xc512;
const UOBJ_ZULU_INGOT       := 0xc513;
const UOBJ_SPIKE_INGOT      := 0xc514;
const UOBJ_FRUITY_INGOT     := 0xc515;
const UOBJ_GREYVIXEN_INGOT  := 0xc516;
const UOBJ_SILVERROCK_INGOT := 0xc517;
const UOBJ_SPECTRAL_INGOT   := 0xc518;
const UOBJ_UNDEAD_INGOT     := 0xc519;
const UOBJ_DARKPAGAN_INGOT  := 0xc51a;
const UOBJ_OLDBRITAIN_INGOT := 0xc51b;
const UOBJ_VIRGINITY_INGOT  := 0xc51c;
const UOBJ_BLACKDWARF_INGOT := 0xc51d;
const UOBJ_REDELVEN_INGOT   := 0xc51e;
const UOBJ_DRIPSTONE_INGOT  := 0xc51f;
const UOBJ_EXECUTOR_INGOT   := 0xc520;
const UOBJ_PEACHBLUE_INGOT  := 0xc521;
const UOBJ_DESTRUCTION_INGOT := 0xc522;
const UOBJ_ANRA_INGOT       := 0xc523;
const UOBJ_GODDESS_INGOT    := 0xc524;
const UOBJ_CRYSTAL_INGOT    := 0xc525;
const UOBJ_DOOM_INGOT       := 0xc526;
const UOBJ_SAPPHIRE_INGOT   := 0xc527;
const UOBJ_RUBY_INGOT       := 0xc528;
const UOBJ_DIAMOND_INGOT    := 0xc529;
const UOBJ_DAMASIN_INGOT    := 0xc52a;
const UOBJ_KORGAN_INGOT     := 0Xc52b;
const UOBJ_AURORA_INGOT	    := 0Xc52c;
const UOBJ_ARASARN_INGOT    := 0Xc52d;
const UOBJ_ORACLE_INGOT     := 0Xc52e;
const UOBJ_VULCAN_INGOT	    := 0Xc52f;
const UOBJ_SHING_INGOT	    := 0Xc530;
const UOBJ_LEVIATHAN_INGOT  := 0Xc531;
const UOBJ_SANCTUARY_INGOT  := 0Xc532;
const UOBJ_INGOTS2_END      := 0xc532;
//


/////////////////////////////////////////////////////////////////////////////
//
// 0xF000 to 0xFFFF - Reserved by system
//
/////////////////////////////////////////////////////////////////////////////

	//
	// Ship Components
	//
const EXTOBJ_TILLERMAN       := 0xF010;
const EXTOBJ_PORT_PLANK      := 0xF011;
const EXTOBJ_STARBOARD_PLANK := 0xF012;
const EXTOBJ_HOLD            := 0xF013;

///////////////////
//  Containers
///////////////////

Const UOBJ_BACKPACK			:= 0x0E75;
Const UOBJ_BAG				:= 0x0E76;
Const UOBJ_POUCH			:= 0x0E79;
Const UOBJ_BASKET			:= 0x0990;
Const UOBJ_PICNIC_BASKET		:= 0x0E7A;
Const UOBJ_WOODEN_BOX_1		:= 0x0E7D;
Const UOBJ_WOODEN_BOX_2		:= 0x09AA;
Const UOBJ_TRASHBARREL		:= 0x7007;
const UOBJ_METALCHEST := 0x0E7C; // A Metal Chest
