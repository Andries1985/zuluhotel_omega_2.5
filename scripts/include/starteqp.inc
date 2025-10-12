///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// New Loot System: Previously loot worked on distinct cut offs.  This made modifying loot distribution
// impossible.  Although the 1 benefit of it was that it was dynamic. Meaning you could set a magicitemlevel
// HIGH and it would adjust to that.
//
// This new system uses 10 preset levels, all existing npcs use 1 - 9 magiclevel anyway.
//
// It's all been changed to a chance % drop system.  Meaning that it IS possible to get a blackrock weapon
// from a level 1 loot.  Or it IS possible to get a +1 ele prot from level 10 loot.  The chances of these extremes happeniing
// are about 1 in 1000000.  Everything else in between has a chance of happening, so killing any monster 
// has the 'exciting' chance of it being a good item.
//
// This isn't a 'raise' in loot levels, it's a more even distribution system.  
//
// The actual % for each level may need tweaking. It's all commented so it's easy to find, but annoying to do.
// There are a LOT of them to determine loot.
//
// A big factor in this working will be correct spawning too. Highest npc out of a dungeon, recallable to should be a 
// UF or frost wisp.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

use cfgfile;
use os;

include "include/client";
include "include/random";
include "include/attributes";
//
//  See starteqp.cfg for a description of what we expect in the config file.
//

const WEAPON 	:= 1;
const ARMOR 	:= 2;
const SHIELD 	:= 3;
const CLOTHING 	:= 4;
const MISC 	:= 5;
const PROF 	:= 6;
const MAX_COLOR	:= 1169;

var availspells := array;
var availeffects := array;
var availgreaters := array;
var availspellonhit := array;
var availeffectonhit := array;
var availgreateronhit := array;

var magicspawnchance 	:= 0;
var magiclevel 		:= 0;
var basechance 		:= 0;
var baselevel 		:= 0;

var starteqp_cfg;
var magiccfg;
var hitscriptcfg;
var onhitcfg;

var unique_items_created := {};
var corpsetype;
var lootgroupglob;

set_script_option( SCRIPTOPT_NO_RUNAWAY, 1 );

function MakeLoot( corpse , lootgroup := 0, magic_chance_default := 0, magic_level_default := 0 )

	starteqp_cfg := ReadConfigFile( "::nlootgroup" );
	
	if( lootgroup )
		if ( (lootgroup >= 5) and (lootgroup <= 9) )
			magicspawnchance := lootgroup * 10;
			magiclevel := lootgroup - 3;
			basechance := magicspawnchance;
			baselevel  := magiclevel;
		endif

		if ( (lootgroup == 100))
			magicspawnchance := (lootgroup - 91 ) * 10;
			magiclevel := lootgroup - 91;
			basechance := magicspawnchance;
			baselevel  := magiclevel;
		endif

		if ( (lootgroup >= 101) and (lootgroup <= 105) )
			magicspawnchance := (lootgroup - 96 ) * 10;
			magiclevel := (lootgroup - 96);
			basechance := magicspawnchance;
			baselevel  := magiclevel;
		endif

		//spawnchests are lootgroups 109 through 119
		if ( (lootgroup >= 109) and (lootgroup <= 119) )
			magicspawnchance := lootgroup - 75;
			magiclevel := Cint((lootgroup - 109) / 3); 
			basechance := magicspawnchance;
			baselevel  := magiclevel;
		endif
		
		if( magic_chance_default )
			magicspawnchance := magic_chance_default;
			basechance := magicspawnchance;
		endif
		
		if( magic_level_default )
			magiclevel := magic_level_default; 
			baselevel  := magiclevel;
		endif

		CreateFromGroup( corpse , Cint(lootgroup) );
		
	else
		var npcdesc := ReadConfigFile( "::npcdesc" );
		var elem := FindConfigElem( npcdesc, GetObjProperty( corpse, "npctemplate" ) );
		var mprop;
		var mlevel;
		var lootgroups;
		var randomlootgroup;




		// Custom Loot, best be carefull with this, don't want bad people changing loot levels/
		// * added a check in customnpc script so only admin+ can set custom loot
		if (GetObjProperty(corpse, "CustomLoot"))
			var cloot := GetObjProperty(corpse, "CustomLoot");
			mprop  := CInt(cloot[1]);
			mlevel := CInt(cloot[2]);
			lootgroups := {cloot[3]};
		elseif( elem )
			//New code to randomize lootgroup
			/*
			randomlootgroup := GetConfigInt(elem, "lootgrouptype");
			Print( "Random loot group number is: " + randomlootgroup);
			if (randomlootgroup == 1)
				mprop  := GetConfigInt( elem, "Magicitemchance");
				mlevel := GetConfigInt( elem, "Magicitemlevel");
				lootgroups := GetConfigInt( elem, "lootgroup" );
				lootgroups := lootgroups + Random(3);
				//lootgroups := CStr(lootgroups);
				Print("Lootgroup is Random! "+lootgroups+", "+mprop+", "+mlevel);
			else
			*/
				mprop  := GetConfigInt( elem, "Magicitemchance");
				mlevel := GetConfigInt( elem, "Magicitemlevel");
				lootgroups := GetConfigStringArray( elem, "lootgroup" );
				//Print("Normal Lootgroup: "+lootgroups);
			//endif
		endif

		if( mprop )
			magicspawnchance := mprop;
			basechance := magicspawnchance;
		endif

		if( mlevel )
			magiclevel := mlevel;
			baselevel := magiclevel;
		endif

		// Almost ALWAYS just 1 lootgroup, although multiple can be used. So this function
		// runs only once.
		//New code to randomize lootgroup
		//if(randomlootgroup == 1)
		//	CreateFromGroup( corpse, lootgroups);
		//else
        	foreach str in lootgroups
            	CreateFromGroup( corpse , cint(str) );
        	endforeach
		//endif
	endif
	
endfunction


function CreateStartingEquipment( who, skillids )

    	starteqp_cfg := ReadConfigFile( "::starteqp" );

    	foreach skillid in skillids
        	var skillname := GetSkillName( CInt(skillid) );
        	CreateFromGroup( who.backpack, skillname );
    	endforeach

        CreateFromGroup(who.backpack, "all");
        
endfunction


function CreateFromGroup( who, group )

    	var elem := starteqp_cfg[ group ];
	if(!elem)
		return;
	endif

    	foreach str in GetConfigStringArray( elem, "Random" )
        	CreateFromRandomString( who, str );
     	endforeach

    	foreach str in GetConfigStringArray( elem, "Stack" )
        	CreateFromStackString( who, str );
    	endforeach

    	foreach str in GetConfigStringArray( elem, "Item" )
        	CreateFromItemString( who, str );
    	endforeach

    	foreach str in GetConfigStringArray( elem, "Unique" )
        	CreateFromUniqueString( who, str );
    	endforeach

    	foreach str in GetConfigStringArray( elem, "Group" )
        	CreateFromGroup( who, str );
    	endforeach

endfunction



function GetSkillName( skillid )

    	var cfg := ReadConfigFile( ":shilhook:skillsdef" );
    	return cfg[skillid].name;
    	
endfunction



/////////////////////////////////////////////////////////////////////////////
//
//  An 'Item' string can have one of the following forms:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Item [ObjTypeName]"                    "Item Mortar"
//  "Item [Count] [ObjTypeName]"            "Item 4 EmptyBottle"
//  "Item [ObjTypeName] [Colornum]"         "Item Robe 0xE9"
//  "Item [Count] [ObjTypeName] [Colornum]" "Item 2 Robe 0xE9"
//  "item [ObjTypeName] [chancetospawn][colornum]  "item Longsword 100 0"
//  "Item [Count] [ObjTypeName] [chancetospawn][colornum]  "item 1 Longsword 100 0"
//
//Item 1 level1map 0 1
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromItemString( who, str )

	var params := SplitWords( str );
	var count;
	var objname;
	var chance;

	// Items passed from CreateFromRandomString are passed here with 'str' being JUST their objectname (ie, katana)
	// These will hit the 'else' part of this so only ONE pass is done.
	if ( RandomDiceStr( params[1] ) )
		count := RandomDiceStr( params[1] );
        	params[1] := params[2];
        	params[2] := params[3];
        	params[3] := params[4];
    	else
        	count := 1;
    	endif

    	objname := params[1];

	if (!objname) 
		return 0; 
	endif

	// It's a chance drop
	if ( CInt(params[2]) )
		chance := CInt( params[2] );
		var killer;
		if(GetObjProperty(who, "KilledBySerial"))
			killer := SystemFindObjectBySerial(GetObjProperty(who, "KilledBySerial"),SYSFIND_SEARCH_OFFLINE_MOBILES );
		endif
		if(GetGlobalProperty("PHH") || GetObjProperty(killer, "#PPHH"))
			chance := chance*2;
			if(chance > 100)
				chance := 100;
			endif
		endif
	else

// It's a definite drop
		chance := 100;
	endif

	// count is only ever higher than 1 when items have a prefix of 'Item'. 
	// And if they are run through this function from the main loot group.
	
	// Items with a prefix of 'Random' are sent here from CreateFromRandomString.
	// Items sent from there have a 100% chance and only do 1 loop.

	while( count )

	// This below item is defined in the main body of a lootgroup. It's sent here from the
	// MAIN function and has 1 chance to spawn, that chance is %15

	// Item	1 	Level5map	15

  		if( Random(100) < chance )
			var item := CreateItemInContainer( who, objname );
			var ismagic;
			if (GetObjProperty(item, "IsGMItem"))
				AddName(item);
			endif
			if ( CInt(params[3]) )
        		item.color := CInt( params[3] );
    		endif


			// Is it a magic item? if this isn't caught then it will be a normal item
			// ie, 'a katana'. And it's possible it can happen on ANY level monster
			// with a low magicchance level. 
			// Leaving this the same, so when defining npcs remember that MagicItemChance
			// has an effect. It will basically lower the level of the loot if low.
			// It can keep looping and missing until it spawns the normal item...

			while( magiclevel > 0 )
				if( (Random(100)+1) <= magicspawnchance )
					ismagic := 1;
					break;
				else
					// Changed from 10 to 15. What this will do is help spawn magic items instead of 'normal'.
					// Test and adjust. 
					magicspawnchance := magicspawnchance + 15;
					magiclevel := magiclevel - 1;
				endif
			endwhile
			
			if( ismagic )
				var canbemagic := MagicAllowedandType( objname ); //for magic items!

			// OLD WAY
			// enchant type is based on magical level again. look into it
			// Example: level 6
			// 		between 26-100			4 = max of 300	5 = max of 351 (with below addition) 6 = max of 400
			//
			//var enchant := Random(75)+26) * magiclevel/2 + 1;
			//
			//
			// lvl 5 loot gets a bonus.
			//if (magiclevel == 5)
			//	enchant := enchant + 51;
			//endif


				case (canbemagic)
					WEAPON: 
					ApplyWeaponModNew(who, item);	
					break;
					
					ARMOR: 
					ApplyArmourModNew(who, item);	
					break;
	
				SHIELD:
					ApplyShieldModNew(who, item);	
					break;
					
				CLOTHING:
					ApplyClothingModNew(who, item);	
					break;

				MISC:
					ApplyMiscModNew(who, item);	
					break;

				PROF:
					// For PROF every loot group goes into here, alter this!
					ApplyMiscSkillMod( item ); 
					AddName( item );
					break;

					default:

				endcase
			endif
		RegisterItem(who,item);
		endif
		count := count - 1;
		magicspawnchance := basechance;
		magiclevel := baselevel;
  	endwhile
  	
endfunction


function RegisterItem(who, item)

	if (IsStackable(item, item))
		return 0;
	endif

	// Loot is from a slayed npc
	if (corpsetype)
		SetObjProperty(item, "FromLoot", {corpsetype, ReadGameClock()});
	else
		// Could be from treasure chest/sos/anything that generates loot that didnt have a corpse

		SetObjProperty(item, "FromLoot", {who.desc, ReadGameClock(), baselevel, lootgroupglob});
	endif

endfunction

//########################################################################## New functions - tedious #############################################################################################\\

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyWeaponModNew() - Chooses what to put on a weapon
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyWeaponModNew(who, item)

	/* This is the OLD way
	Min lvl 1 - 26		Max lvl 1 - 100
	Min lvl 2 - 52		Max lvl 2 - 200
	Min lvl 3 - 52		Max lvl 3 - 200
	Min lvl 4 - 78		Max lvl 4 - 300
	Min lvl 5 - 78		Max lvl 5 - 300 <----- Both these values got +51 after, bandaid solution
	Min lvl 6 - 104		Max lvl 6 - 400
	Min lvl 7 - 104		Max lvl 7 - 400
	Min lvl 8 - 130		Max lvl 8 - 500
	Min lvl 9 - 130		Max lvl 9 - 500
	Min lvl 10 - 156	Max lvl 10 - 600

	if(enchant < 50)
		ApplyHPMod( item ); // Impossible for 2+ (durable broadsword) no other chance of mods in function

	elseif(enchant < 60)
		ApplyWeapSkillMod( item ); // impossible for 4+ (arms masters broadsword) gets a chance of obtaining applyhpmod at end (arms master durable broadsword)

	elseif(enchant < 485)
		ApplyDmgMod ( item ); // Any lootgroup (+dmg!) gets chance of above mods, 25% more chance of HPmod though (arms masters broadsword of power) or (durable broadsword of power)

	else
		ApplyWeapHitscript ( who , item ); //lvl 8+ // No chance of other mods - this used to be lvl 5+.....
	endif
	AddName( item );
	*/

	// No need for anything higher
	var chance := RandomInt(10000);

	if (magiclevel == 0)
		return 0;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 30% chance 
		if (chance < 3000)
			ApplyHPMod( item ); 
		// 10% chance
		elseif (chance < 4000)
			ApplyWeapSkillMod( item ); 
		// 59.9% chance
		elseif (chance < 9990)
			ApplyDmgMod( item );
		// .1% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 2% chance 
		if (chance < 200)
			ApplyHPMod( item ); 
		// 5% chance
		elseif (chance < 700)
			ApplyWeapSkillMod( item ); 
		// 92.5% chance
		elseif (chance < 9950)
			ApplyDmgMod( item );
		// .5% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 1% chance 
		if (chance < 100)
			ApplyHPMod( item ); 
		// 2% chance
		elseif (chance < 300)
			ApplyWeapSkillMod( item ); 
		// 96% chance
		elseif (chance < 9900)
			ApplyDmgMod( item );
		// 1% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// .5% chance 
		if (chance < 50)
			ApplyHPMod( item ); 
		// 1% chance
		elseif (chance < 150)
			ApplyWeapSkillMod( item ); 
		// 93.5% chance
		elseif (chance < 9500)
			ApplyDmgMod( item );
		// 5% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// .2% chance 
		if (chance < 20)
			ApplyHPMod( item ); 
		// .5% chance
		elseif (chance < 70)
			ApplyWeapSkillMod( item ); 
		// 92.3% chance
		elseif (chance < 9000)
			ApplyDmgMod( item );
		// 7% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// .1% chance 
		if (chance < 10)
			ApplyHPMod( item ); 
		// .2% chance
		elseif (chance < 30)
			ApplyWeapSkillMod( item ); 
		// 79.7% chance
		elseif (chance < 8500)
			ApplyDmgMod( item );
		// 20% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 77%
		if (chance < 8500)
			ApplyDmgMod( item );
		// 23% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 74%
		if (chance < 8200)
			ApplyDmgMod( item );
		// 26% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 71%
		if (chance < 7900)
			ApplyDmgMod( item );
		// 29% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 68%
		if (chance < 9000)
			ApplyDmgMod( item );
		// 32% chance
		else
			ApplyWeapHitscript( who , item );
		endif
		
	else

		// 65%
		if (chance < 9000)
			ApplyDmgMod( item );
		// 35% chance
		else
			ApplyWeapHitscript( who , item );
		endif
		
	endif

	AddName( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyArmourModNew() - Chooses what to put on armour
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyArmourModNew(who, item)

/* This is the OLD way
Min lvl 1 - 26		Max lvl 1 - 100
Min lvl 2 - 52		Max lvl 2 - 200
Min lvl 3 - 52		Max lvl 3 - 200
Min lvl 4 - 78		Max lvl 4 - 300
Min lvl 5 - 78		Max lvl 5 - 300 <----- Both these values got +51 after, bandaid solution
Min lvl 6 - 104		Max lvl 6 - 400
Min lvl 7 - 104		Max lvl 7 - 400
Min lvl 8 - 130		Max lvl 8 - 500
Min lvl 9 - 130		Max lvl 9 - 500
Min lvl 10 - 156	Max lvl 10 - 600

if(enchant < 50)
ApplyHPMod( item );	// durable, woah!	
				
elseif(enchant < 100)
	ApplyArSkillMod( item ); // sets hiding or magic resistance as the skill... weird
	
elseif(enchant < 350)
	ApplyARMod( item );	// of fortification etc
	
else
	ApplyOnHitScript ( who , item );	// slayer type, spell on hit, gm armour etc. This ISN'T anywhere near as good as weapons. give more
endif
AddName( item );
*/

// No need for anything higher
var chance := RandomInt(10000);

if (magiclevel == 0)
	return 0;

// Level 1 loot
elseif (magiclevel == 1)
	
	// 20% chance 
	if (chance < 2000)
		ApplyHPMod( item );
	// 30% chance
	elseif (chance < 5000)
		ApplyArSkillMod( item ); 
	// 40% chance
	elseif (chance < 9000)
		ApplyARMod( item );
	// 10% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 2 loot
elseif (magiclevel == 2)
	
	// 10% chance 
	if (chance < 1000)
		ApplyHPMod( item );
	// 20% chance
	elseif (chance < 3000)
		ApplyArSkillMod( item ); 
	// 55% chance
	elseif (chance < 8500)
		ApplyARMod( item );
	// 15% chance
	else
		ApplyOnHitScript ( who , item );
	endif


// Level 3 loot
elseif (magiclevel == 3)
	
	// 1% chance 
	if (chance < 100)
		ApplyHPMod( item );
	// 2% chance
	elseif (chance < 300)
		ApplyArSkillMod( item ); 
	// 77% chance
	elseif (chance < 8000)
		ApplyARMod( item );
	// 20% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 4 loot
elseif (magiclevel == 4)
	
	// .7% chance 
	if (chance < 70)
		ApplyHPMod( item );
	// 1.8% chance
	elseif (chance < 250)
		ApplyArSkillMod( item ); 
	// 75.5% chance
	elseif (chance < 7800)
		ApplyARMod( item );
	// 22% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 5 loot
elseif (magiclevel == 5)
	
	// .5% chance 
	if (chance < 50)
		ApplyHPMod( item );
	// 1.5% chance
	elseif (chance < 200)
		ApplyArSkillMod( item ); 
	// 73% chance
	elseif (chance < 7000)
		ApplyARMod( item );
	// 25% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 6 loot
elseif (magiclevel == 6)
	
	// .1% chance 
	if (chance < 10)
		ApplyHPMod( item );
	// 1% chance
	elseif (chance < 110)
		ApplyArSkillMod( item ); 
	// 68.9% chance
	elseif (chance < 7000)
		ApplyARMod( item );
	// 30% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 7 loot
elseif (magiclevel == 7)
	
	// 68% chance
	if (chance < 6800)
		ApplyARMod( item );
	// 32% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 8 loot
elseif (magiclevel == 8)
	
	// 66% chance
	if (chance < 6600)
		ApplyARMod( item );
	// 34% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 9 loot
elseif (magiclevel == 9)
	
	// 64% chance
	if (chance < 6400)
		ApplyARMod( item );
	// 36% chance
	else
		ApplyOnHitScript ( who , item );
	endif

// Level 10 loot
elseif (magiclevel == 10)
	
	// 62% chance
	if (chance < 6200)
		ApplyARMod( item );
	// 38% chance
	else
		ApplyOnHitScript ( who , item );
	endif
endif

AddName( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyShieldModNew() - Chooses what to put on a shield
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyShieldModNew(who, item)

/* This is the OLD way
Min lvl 1 - 26		Max lvl 1 - 100
Min lvl 2 - 52		Max lvl 2 - 200
Min lvl 3 - 52		Max lvl 3 - 200
Min lvl 4 - 78		Max lvl 4 - 300
Min lvl 5 - 78		Max lvl 5 - 300 <----- Both these values got +51 after, bandaid solution
Min lvl 6 - 104		Max lvl 6 - 400
Min lvl 7 - 104		Max lvl 7 - 400
Min lvl 8 - 130		Max lvl 8 - 500
Min lvl 9 - 130		Max lvl 9 - 500
Min lvl 10 - 156	Max lvl 10 - 600

if(enchant < 100)
	ApplyHPMod( item ); // durable, etc

elseif(enchant < 200)
	ApplyArSkillMod( item ); // hiding or magic resist only
else
	ApplyARMod( item ); // fortification, invul etc
endif
AddName( item );
*/

// No need for anything higher
var chance := RandomInt(10000);

if (magiclevel == 0)
	return 0;

// Level 1 loot
elseif (magiclevel == 1)
	
	// 70% chance 
	if (chance < 7000)
		ApplyHPMod( item );
	// 20% chance
	elseif (chance < 9000)
		ApplyArSkillMod( item ); 
	// 10% chance
	else 
		ApplyARMod( item );
	endif

// Level 2 loot
elseif (magiclevel == 2)
	
	// 50% chance 
	if (chance < 5000)
		ApplyHPMod( item );
	// 30% chance
	elseif (chance < 8000)
		ApplyArSkillMod( item ); 
	// 20% chance
	else 
		ApplyARMod( item );
	endif


// Level 3 loot
elseif (magiclevel == 3)
	
	// 20% chance 
	if (chance < 2000)
		ApplyHPMod( item );
	// 30% chance
	elseif (chance < 5000)
		ApplyArSkillMod( item ); 
	//50% chance
	else 
		ApplyARMod( item );
	endif

// Level 4 loot
elseif (magiclevel == 4)
	
	// 5% chance 
	if (chance < 500)
		ApplyHPMod( item );
	// 30% chance
	elseif (chance < 3500)
		ApplyArSkillMod( item ); 
	// 65% chance
	else 
		ApplyARMod( item );
	endif

// Level 5 loot
elseif (magiclevel == 5)
	
	// 2% chance 
	if (chance < 200)
		ApplyHPMod( item );
	// 28% chance
	elseif (chance < 3000)
		ApplyArSkillMod( item ); 
	// 70% chance
	else 
		ApplyARMod( item );
	endif

// Level 6 loot
elseif (magiclevel == 6)
	
	// 1% chance 
	if (chance < 100)
		ApplyHPMod( item );
	// 24% chance
	elseif (chance < 2500)
		ApplyArSkillMod( item ); 
	// 75% chance
	else 
		ApplyARMod( item );
	endif

// Level 7 loot
elseif (magiclevel == 7)
	
	// .05% chance 
	if (chance < 50)
		ApplyHPMod( item );
	// 19.5% chance
	elseif (chance < 8000)
		ApplyArSkillMod( item ); 
	// 80% chance
	else 
		ApplyARMod( item );
	endif

// Level 8 loot
elseif (magiclevel == 8)
	
	// .02% chance 
	if (chance < 20)
		ApplyHPMod( item );
	// 14.98% chance
	elseif (chance < 7500)
		ApplyArSkillMod( item ); 
	// 85% chance
	else 
		ApplyARMod( item );
	endif	

// Level 9 loot
elseif (magiclevel == 9)
	
	// .01% chance 
	if (chance < 10)
		ApplyHPMod( item );
	// 9.99% chance
	elseif (chance < 9000)
		ApplyArSkillMod( item ); 
	// 90% chance
	else 
		ApplyARMod( item );
	endif

// Level 10 loot
elseif (magiclevel == 10)
	
	// .005% chance 
	if (chance < 5)
		ApplyHPMod( item );
	// 4.995% chance
	elseif (chance < 9500)
		ApplyArSkillMod( item ); 
	// 95% chance
	else 
		ApplyARMod( item );
	endif

// Level ?? loot
else 
	// 100% chance
	ApplyARMod( item );

endif

AddName( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyClothingModNew() - Chooses what to put on clothing
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyClothingModNew(who, item)

	/* This is the OLD way
	Min lvl 1 - 26		Max lvl 1 - 100
	Min lvl 2 - 52		Max lvl 2 - 200
	Min lvl 3 - 52		Max lvl 3 - 200
	Min lvl 4 - 78		Max lvl 4 - 300
	Min lvl 5 - 78		Max lvl 5 - 300 <----- Both these values got +51 after, bandaid solution
	Min lvl 6 - 104		Max lvl 6 - 400
	Min lvl 7 - 104		Max lvl 7 - 400
	Min lvl 8 - 130		Max lvl 8 - 500
	Min lvl 9 - 130		Max lvl 9 - 500
	Min lvl 10 - 156	Max lvl 10 - 600

	if(enchant < 300)
		ApplyMiscSkillMod( item );  // applies a skill and has a chance to add a stat mod
	else
		ApplyMiscArMod ( item ); // was only lvl 5+ loot. just adds armour to clothing, but has a chance of running the above function for skill/stats
	endif
	AddName( item );
	AddRandomColor( item );
	*/

	// No need for anything higher
	var chance := RandomInt(10000);

	if (magiclevel == 0)
		return 0;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 95% chance 
		if (chance < 9500)
			ApplyMiscSkillMod( item );
		// 5% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 93% chance 
		if (chance < 9300)
			ApplyMiscSkillMod( item );
		// 7% chance
		else 
			ApplyMiscArMod( item );
		endif


	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 90% chance 
		if (chance < 9000)
			ApplyMiscSkillMod( item );
		// 10% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 85% chance 
		if (chance < 8500)
			ApplyMiscSkillMod( item );
		// 15% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 80% chance 
		if (chance < 8000)
			ApplyMiscSkillMod( item );
		// 20% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 70% chance 
		if (chance < 7000)
			ApplyMiscSkillMod( item );
		// 30% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 60% chance 
		if (chance < 6000)
			ApplyMiscSkillMod( item );
		// 40% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 50% chance 
		if (chance < 5000)
			ApplyMiscSkillMod( item );
		// 50% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 40% chance 
		if (chance < 4000)
			ApplyMiscSkillMod( item );
		// 60% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 30% chance 
		if (chance < 3000)
			ApplyMiscSkillMod( item );
		// 70% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level ?? loot
	else 
		// 20% chance 
		if (chance < 2000)
			ApplyMiscSkillMod( item );
		// 80% chance
		else 
			ApplyMiscArMod( item );
		endif

	endif

	AddName( item );
	AddRandomColor( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyMiscModNew() - Chooses what to put on misc? Misc appears to be jewels! Judging from apply enchant 
//			
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyMiscModNew(who, item)

/* This is the OLD way
Min lvl 1 - 26		Max lvl 1 - 100
Min lvl 2 - 52		Max lvl 2 - 200
Min lvl 3 - 52		Max lvl 3 - 200
Min lvl 4 - 78		Max lvl 4 - 300
Min lvl 5 - 78		Max lvl 5 - 300 <----- Both these values got +51 after, bandaid solution
Min lvl 6 - 104		Max lvl 6 - 400
Min lvl 7 - 104		Max lvl 7 - 400
Min lvl 8 - 130		Max lvl 8 - 500
Min lvl 9 - 130		Max lvl 9 - 500
Min lvl 10 - 156	Max lvl 10 - 600

if(enchant < 100 )

	ApplyMiscSkillMod( item ); // adds a random skill and chance at skill mod

elseif(enchant < 200 )

	ApplyEnchant( who , item );

elseif(enchant < 400 )

	ApplyMiscSkillMod( item ); // adds a random skill and chance at skill mod

elseif(enchant < 575 )

	ApplyEnchant( who , item );
else
	ApplyMiscArMod( item ); // only run on lvl 10+ adds an armour, ie fortifications, and has a chance of applymiscskillmod to run
endif

						
AddName( item );
*/

// No need for anything higher
var chance := RandomInt(10000);

if (magiclevel == 0)
	return 0;

// Level 1 loot
elseif (magiclevel == 1)
	
	// 92% chance 
	if (chance < 9200)
		ApplyMiscSkillMod( item );
	// 7% chance
	elseif (chance < 9900)
		ApplyEnchant( who , item );
	// 1% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 2 loot
elseif (magiclevel == 2)
	
	// 88% chance 
	if (chance < 8800)
		ApplyMiscSkillMod( item );
	// 10% chance
	elseif (chance < 9800)
		ApplyEnchant( who , item );
	// 2% chance
	else 
		ApplyMiscArMod( item );
	endif


// Level 3 loot
elseif (magiclevel == 3)
	
	// 82% chance 
	if (chance < 8200)
		ApplyMiscSkillMod( item );
	// 15% chance
	elseif (chance < 9700)
		ApplyEnchant( who , item );
	// 3% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 4 loot
elseif (magiclevel == 4)
	
	// 71% chance 
	if (chance < 7100)
		ApplyMiscSkillMod( item );
	// 25% chance
	elseif (chance < 9600)
		ApplyEnchant( who , item );
	// 4% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 5 loot
elseif (magiclevel == 5)
	
	// 65% chance 
	if (chance < 6500)
		ApplyMiscSkillMod( item );
	// 30% chance
	elseif (chance < 9500)
		ApplyEnchant( who , item );
	// 5% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 6 loot
elseif (magiclevel == 6)
	
	// 59% chance 
	if (chance < 5900)
		ApplyMiscSkillMod( item );
	// 35% chance
	elseif (chance < 9400)
		ApplyEnchant( who , item );
	// 6% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 7 loot
elseif (magiclevel == 7)
	
	// 53% chance 
	if (chance < 5300)
		ApplyMiscSkillMod( item );
	// 40% chance
	elseif (chance < 9300)
		ApplyEnchant( who , item );
	// 7% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 8 loot
elseif (magiclevel == 8)
	
	// 47% chance 
	if (chance < 4700)
		ApplyMiscSkillMod( item );
	// 45% chance
	elseif (chance < 9200)
		ApplyEnchant( who , item );
	// 8% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 9 loot
elseif (magiclevel == 9)
	
	// 41% chance 
	if (chance < 4100)
		ApplyMiscSkillMod( item );
	// 50% chance
	elseif (chance < 9100)
		ApplyEnchant( who , item );
	// 9% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level 10 loot
elseif (magiclevel == 10)
	
	// 35% chance 
	if (chance < 3500)
		ApplyMiscSkillMod( item );
	// 55% chance
	elseif (chance < 9000)
		ApplyEnchant( who , item );
	// 10% chance
	else 
		ApplyMiscArMod( item );
	endif

// Level ?? loot
else 

	// 29% chance 
	if (chance < 2900)
		ApplyMiscSkillMod( item );
	// 60% chance
	elseif (chance < 8900)
		ApplyEnchant( who , item );
	// 11% chance
	else 
		ApplyMiscArMod( item );
	endif

endif

AddName( item );

endfunction
/////////////////////////////////////////////////////////////////////////////
//
//  A 'Random' string follows the following form:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Random [Count] [GroupName]"            "Random 4 Reagents"
//  "Random [Count] [GroupName] [chancetospawn]"   "Random 4 Reagents 50"
//
//  'Count' entries from 'GroupName' are chosen randomly.  The same entry
//  will not be chosen twice.
//
//
// Toadstool - Fixed a bug where all items wouldn't spawn.
// 
// This function is all based on what's defined in nlootgroup.cfg
// It works well now, does not need changing. 
//
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromRandomString( who, desc )

	// Desc looks like this atm etc { 1d5,Jewelry,65 }

    var params := SplitWords( desc );
	var count;
	var group;
	var chance;
	var elem;
	var items;
	var stacks;

	// Do a dice roll and see how many items spawn, ie 1d6 means throwing 1 6 sided die, 2d6 means
	// throwing 2 6 sided die, so a possibiblty of 12
	var roll := RandomDiceStr( params[1] );

	if ( roll )
		count := roll;
        	params[1] := params[2]; // setting name of group as 1
        	params[2] := params[3]; // setting chance to spawn as 2
    	else
	// this should never be hit
        	count := 1;
    	endif

    	group := params[1];

    	if ( CInt(params[2]) )
        	chance := CInt( params[2] );
			var killer;
			if(GetObjProperty(who, "KilledBySerial"))
				killer := SystemFindObjectBySerial(GetObjProperty(who, "KilledBySerial"),SYSFIND_SEARCH_OFFLINE_MOBILES );
			endif
			if(GetGlobalProperty("PHH") || GetObjProperty(killer, "#PPHH"))
				chance := chance*2;
				if(chance > 100)
					chance := 100;
				endif
			endif
    	else
        	chance := 100; // if no roll then spawn an item, only 1 as count is 1 if it hits here
    	endif

	// We are now entering the group of items. We were looking at the item in the main, ie Lootgroup 74
	// Now were are in the particular items group, ie Loogroup Pagans or Lootgroup Jewels

    	elem := FindConfigElem( starteqp_cfg , group);
    	if( elem == error )
        	return;
    	endif

	// Store all 'items' and 'stacks' in an array, so it'll look like '{ring, bracelet}' or '{katana, broadsword}'

    	items := GetConfigStringArray( elem , "Item" );
		stacks := GetConfigStringArray( elem , "Stack" );

	// There are items that need to be created from this group
        if( len(items) )
        
		// the number to spawn is greater than the number of items in the group, set it even.
        	if( count > len( items ) )
        		count := len( items );
        	endif

		// loop the dice roll number of times
		while( count )
			// if random under magicchance, make an item
	    		if( Random(100) < chance )

			// Toadstool Fix - This will now keep throwing a random to get
			// an item that hasn't bee picked yet.

        			var n := RandomDiceStr( "1d"+len(items) );
          		  	if( !items[n] )
					repeat 
						n := RandomDiceStr( "1d"+len(items) );
					until (n != 0);
				endif
			
             	   		CreateFromItemString( who , items[n] );
             	   		items[n] := 0;

            		endif
			count := count - 1;
        	endwhile
        	
        elseif( len(stacks) )

		if( count > len( stacks ) )
        		count := len( stacks );
        	endif

        	while( count )
	    		if( Random(100) < chance )
        			var n := RandomDiceStr( "1d" + len(stacks) );
          		  	if( stacks[n] )
             	   			CreateFromStackString( who , stacks[n] );
             	   			stacks[n] := 0;
            			else
            				continue;
            			endif		
            		endif
			count := count - 1;
        	endwhile	
        endif	
        
endfunction


/////////////////////////////////////////////////////////////////////////////
//
//  A 'Unique' string has the following form:
//
//  FORM                                    EXAMPLE
//  --------------------------------------------------------------------
//  "Unique [ObjTypeName]"                  "Unique Spellbook"
//
//  Only one of a unique item will be created as starting equipment.
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromUniqueString( who, str )

    	var objtype := GetObjtypeByName( str );
    	if (!objtype)
        	return;
    	endif
        
    	if (objtype in unique_items_created)
        	return;
    	endif

    	unique_items_created[ len(unique_items_created) + 1 ] := objtype;
    	var uitem := CreateItemInContainer( who, objtype );

		RegisterItem(who, uitem);
    	
endfunction

/////////////////////////////////////////////////////////////////////////////
//
//  A 'Stack' string is just like "Item" but stacks the item instead of making
//  count number of individual items.
//
// Toadstool - Like CreateItemFromString().
//
// If this function is called by the main CreateLoot function, ie if
// the item is defined as a Stack in the main monster lootgroup. Then whether
// it spawns is based on chance.  
//
// However if this function is called from CreateFromRandomString() then an
// item WILL be created.
//
/////////////////////////////////////////////////////////////////////////////

function CreateFromStackString( who, str )

    var params := SplitWords( str );
	var count;
	var objname;
	var killer;
	var chance;

    	if ( RandomDiceStr( params[1] ) )
        	count := RandomDiceStr( params[1] );
        	params[1] := params[2];
        	params[2] := params[3];
        	params[3] := params[4];
    	else
		// this means it was called form a sub function
        	count := 1;
    	endif

    	objname := params[1];
	if (!objname) return; endif
    	if ( CInt(params[2]) )
        	chance := CInt( params[2] );
			if(GetObjProperty(who, "KilledBySerial"))
				killer := SystemFindObjectBySerial(GetObjProperty(who, "KilledBySerial"),SYSFIND_SEARCH_OFFLINE_MOBILES );
			endif
			if(GetGlobalProperty("PHH") || GetObjProperty(killer, "#PPHH"))
				chance := chance*2;
				if(chance > 100)
					chance := 100;
				endif
			endif
    	else
        	chance := 100;
    	endif

  	if( Random(100) < chance )
  			var item;
			if(GetObjProperty(who, "KilledBySerial"))
				killer := SystemFindObjectBySerial(GetObjProperty(who, "KilledBySerial"),SYSFIND_SEARCH_OFFLINE_MOBILES );
			endif
			if(!GetGlobalProperty("PHH") || !GetObjProperty(killer, "#PPHH"))
        		item := CreateItemInContainer( who , objname , count );
			else
				item := CreateItemInContainer( who , objname , count*2 );
			endif
        	if (!item)
				return;
        	endif
 
    		if ( CInt(params[3]) )
        		item.color := CInt( params[3] );
    		endif        	
  	endif

endfunction


////
//Checks to see if that item is allowed to be magic and returns the type
////
function MagicAllowedandType( objname )

	if( !magiccfg )
		magiccfg := ReadConfigFile( ":combat:enchantableitems" );
	endif

   	var elem1 := FindConfigElem( magiccfg , WEAPON );
   	var data1 := GetConfigInt( elem1 , objname );
   	if( data1 )
      		return WEAPON;
   	endif

   	var elem2 := FindConfigElem( magiccfg , ARMOR );
   	var data2 := GetConfigInt( elem2 , objname );
   	if( data2 )
      		return ARMOR;
   	endif

   	var elem3 := FindConfigElem( magiccfg , SHIELD );
   	var data3 := GetConfigInt( elem3 , objname );
   	if( data3 )
      		return SHIELD;
   	endif

	var elem4 := FindConfigElem( magiccfg , CLOTHING );
	var data4 := GetConfigInt( elem4 , objname );
	if( data4 )
		return CLOTHING;
	endif

	var elem5 := FindConfigElem( magiccfg , MISC );
	var data5 := GetConfigInt( elem5 , objname );
	if( data5 )
		return MISC;
	endif

	var elem6 := FindConfigElem( magiccfg , PROF );
	var data6 := GetConfigInt( elem6 , objname );
	if( data6 )
		return PROF;
	endif
	
   	return 0;

endfunction

///////////////////////////////////////////////////////////
//Toadstool
//
// Altered function to a % chance based on loot level
//
// Picks and assigns a damagemod and a suffix for the item
///////////////////////////////////////////////////////////
function ApplyDmgMod( item )

var power := RandomInt(100000)+1;
var n;

// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;
	elseif (magiclevel == 1)
		
		// 98% chance of ruin
		if (power < 98000)
			n := 1;

		// 1.9% chance of might
		else
			n := 2;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of ruin
		if (power < 82000)
			n := 1;

		// 17.5% chance of might
		elseif (power < 99500)
			n := 2;

		// .49% chance of force
		else
			n := 3;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of ruin
		if (power < 30000)
			n := 1;

		// 80% chance of might
		elseif (power < 90000)
			n := 2;

		// 9.499% chance of force
		else
			n := 3;
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of ruin
		if (power < 20000)
			n := 1;

		// 64.497% chance of might
		elseif (power < 64997)
			n := 2;

		// 25% chance of force
		elseif (power < 94997)
			n := 3;
		
		// 10% chance of power
		else
			n := 4;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of ruin
		if (power < 15000)
			n := 1;

		// 34.999% chance of might
		elseif (power < 44999)
			n := 2;

		// 45% chance of force
		elseif (power < 90999)
			n := 3;
		
		// 18% chance of power
		else
			n := 4;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of ruin
		if (power < 3000)
			n := 1;

		// 0% chance of might
		elseif (power < 18000)
			n := 2;

		// 24% chance of force
		elseif (power < 48000)
			n := 3;
		
		// 55% chance of power
		elseif (power < 83000)
			n := 4;

		// 20% chance of vanq
		elseif (power < 97000)
			n := 5;
		// 1% chance of dev
		else
			n := 6;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of ruin
		if (power < 0)
			n := 1;

		// 0% chance of might
		elseif (power < 0)
			n := 2;

		// 24% chance of force
		elseif (power < 38000)
			n := 3;
		
		// 55% chance of power
		elseif (power < 78000)
			n := 4;

		// 20% chance of vanq
		elseif (power < 96000)
			n := 5;
		// 1% chance of dev
		else
			n := 6;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of ruin
		if (power < 0)
			n := 1;

		// 0% chance of might
		elseif (power < 0)
			n := 2;

		// 7% chance of force
		elseif (power < 18000)
			n := 3;
		
		// 60% chance of power
		elseif (power < 73000)
			n := 4;

		// 30% chance of vanq
		elseif (power < 90000)
			n := 5;
		// 3% chance of dev
		else
			n := 6;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of ruin
		if (power < 0)
			n := 1;

		// 0% chance of might
		elseif (power < 0)
			n := 2;

		// 0% chance of force
		elseif (power < 15000)
			n := 3;
		
		// 55% chance of power
		elseif (power < 68000)
			n := 4;

		// 40% chance of vanq
		elseif (power < 86000)
			n := 5;
		// 5% chance of dev
		else
			n := 6;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of ruin
		if (power < 0)
			n := 1;

		// 0% chance of might
		elseif (power < 0)
			n := 2;

		// 0% chance of force
		elseif (power < 25000)
			n := 3;
		
		// 40% chance of power
		elseif (power < 65000)
			n := 4;

		// 50% chance of vanq
		elseif (power < 85000)
			n := 5;
		// 10% chance of dev
		else
			n := 6;
		endif

	// Higher than 10
	else
		
		// 0% chance of ruin
		if (power < 0)
			n := 1;

		// 0% chance of might
		elseif (power < 0)
			n := 2;

		// 0% chance of force
		elseif (power < 15000)
			n := 3;
		
		// 25% chance of power
		elseif (power < 50000)
			n := 4;

		// 60% chance of vanq
		elseif (power < 80000)
			n := 5;
		// 15% chance of dev
		else
			n := 6;
		endif
	endif
		
	item.dmg_mod := (n*5);


	if(n == 5)
		var getspeedmult := GetObjProperty(item, "Enchanted");
		if(getspeedmult == "Swift" || getspeedmult == "Stygian")
			SetObjProperty(item, "isUncommon", 1);
		endif
	elseif(n == 6)
		var getspeedmult := GetObjProperty(item, "Enchanted");
		if(getspeedmult == "Swift" || getspeedmult == "Stygian")
			SetObjProperty(item, "isRare", 1);
		else
			SetObjProperty(item, "isUncommon", 1);
		endif
	endif
	
	// Toadstool: Every level has a 10% extra chance of hititng this
   	if( (Random(100)+1) <= (10 * magiclevel) )
       	if ((Random(100)+1) <= 75) // 75% chance of adding a hp mod
       		ApplyHPMod( item );
		else
			ApplyWeapSkillMod( item ); // 25% chance of adding a skill mod
		endif
   	endif

endfunction


//////////////////////////////////////////////////////////////
// Toadstool
//
// Altered function to work on a % level
//
// Picks and assigns a new HP and prefix for the item
//////////////////////////////////////////////////////////////
function ApplyHPMod( item )

 
   	/*
  	var numb := (Random(50)+1) * magiclevel * 2;
   	case( Cint(magiclevel / 3) )
		0:	break;
		1:	if( numb < 75 )
				numb := 75;
			endif
			break;
			
		2:	if( numb < 150 )
				numb := 150;
			endif
			break;
	endcase

   	if( numb < 75 )
		hp := 10;
   	elseif( numb < 125 )
		hp := 20;
   	elseif( numb < 200 )
		hp := 30;
   	elseif( numb < 300 )
		hp := 40;
   	elseif( numb < 450 )
		hp := 50;
	else
		hp := 60;
   	endif
*/

var power := RandomInt(100000)+1;
var n;

// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance of durable
		if (power < 98000)
			n := 5;

		// 1.9% chance of substantial
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of durable
		if (power < 82000)
			n := 5;

		// 17.5% chance of substantial
		elseif (power < 99500)
			n := 10;

		// .49% chance of massive
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of durable
		if (power < 30000)
			n := 5;

		// 80% chance of substantial
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of massive
		else
			n := 15;
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of durable
		if (power < 20000)
			n := 5;

		// 64.497% chance of substantial
		elseif (power < 64997)
			n := 10;

		// 25% chance of massive
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of fortified
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of durable
		if (power < 15000)
			n := 5;

		// 34% chance of substantial
		elseif (power < 44999)
			n := 10;

		// 45% chance of massive
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of fortified
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of durable
		if (power < 3000)
			n := 5;

		// 0% chance of substantial
		elseif (power < 18000)
			n := 10;

		// 17% chance of massive
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortified
		elseif (power < 77000)
			n := 20;

		// 25% chance of tempered
		elseif (power < 97000)
			n := 25;
		// 8% chance of indestructible
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of durable
		if (power < 3000)
			n := 5;

		// 0% chance of substantial
		elseif (power < 18000)
			n := 10;

		// 17% chance of massive
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortified
		elseif (power < 77000)
			n := 20;

		// 25% chance of tempered
		elseif (power < 97000)
			n := 25;
		// 8% chance of indestructible
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of durable
		if (power < 0)
			n := 5;

		// 0% chance of substantial
		elseif (power < 0)
			n := 10;

		// 4% chance of massive
		elseif (power < 38000)
			n := 15;
		
		// 40% chance of fortified
		elseif (power < 68000)
			n := 20;

		// 45% chance of tempered
		elseif (power < 95000)
			n := 25;
		// 11% chance of indestructible
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of durable
		if (power < 0)
			n := 5;

		// 0% chance of substantial
		elseif (power < 0)
			n := 10;

		// 0% chance of massive
		elseif (power < 28000)
			n := 15;
		
		// 26% chance of fortified
		elseif (power < 63000)
			n := 20;

		// 60% chance of tempered
		elseif (power < 90000)
			n := 25;
		// 14% chance of indestructible
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of durable
		if (power < 0)
			n := 5;

		// 0% chance of substantial
		elseif (power < 0)
			n := 10;

		// 0% chance of massive
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of fortified
		elseif (power < 60000)
			n := 20;

		// 70% chance of tempered
		elseif (power < 86000)
			n := 25;
		// 17% chance of indestructible
		else
			n := 30;
		endif

	// Higher than 10
	else
		
		// 0% chance of durable
		if (power < 0)
			n := 5;

		// 0% chance of substantial
		elseif (power < 0)
			n := 10;

		// 0% chance of massive
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of fortified
		elseif (power < 50000)
			n := 20;

		// 75% chance of tempered
		elseif (power < 70000)
			n := 25;
		// 20% chance of indestructible
		else
			n := 30;
		endif
	endif

   	item.maxhp_mod := n;
   	item.hp := item.maxhp;

endfunction



///////////////////////////////////////////////////////////////////////
// Toadstool
// 
// Altered to use a % system
//
// Picks a AR mod and suffix for the item
////////////////////////////////////////////////////////////////////////
function ApplyARMod( item )

	/*
   	var numb := (Random(50)+1) * magiclevel * 2;
   	
	case( Cint(magiclevel / 3) )
		0:	break;
		1:	if( numb < 150 )
				numb := 150;
			endif
			break;
			
		2:	if( numb < 300 )
				numb := 300;
			endif
			break;
	endcase
	
   	if( numb < 50 )
       		item.ar_mod := 5;
   	elseif( numb < 100 )
       		item.ar_mod := 10;
   	elseif( numb < 350 )
       		item.ar_mod := 15;
   	elseif( numb < 550 )
       		item.ar_mod := 20;
   	elseif( numb < 650 )
       		item.ar_mod := 25;
	else
		item.ar_mod := 30;
   	endif
*/
var power := RandomInt(100000)+1;
var n;

// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance of defence
		if (power < 98000)
			n := 5;

		// 1.9% chance of guarding
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of defence
		if (power < 82000)
			n := 5;

		// 17.5% chance of guarding
		elseif (power < 99500)
			n := 10;

		// .49% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of defence
		if (power < 30000)
			n := 5;

		// 80% chance of guarding
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of defence
		if (power < 20000)
			n := 5;

		// 64.497% chance of guarding
		elseif (power < 64997)
			n := 10;

		// 25% chance of hardening
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of defence
		if (power < 15000)
			n := 5;

		// 34% chance of guarding
		elseif (power < 44999)
			n := 10;

		// 45% chance of hardening
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of defence
		if (power < 2000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 4% chance of hardening
		elseif (power < 38000)
			n := 15;
		
		// 40% chance of fortification
		elseif (power < 68000)
			n := 20;

		// 45% chance of invulnerability
		elseif (power < 95000)
			n := 25;
		// 11% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 28000)
			n := 15;
		
		// 26% chance of fortification
		elseif (power < 63000)
			n := 20;

		// 60% chance of invulnerability
		elseif (power < 90000)
			n := 25;
		// 14% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of fortification
		elseif (power < 60000)
			n := 20;

		// 70% chance of invulnerability
		elseif (power < 96000)
			n := 25;
		// 17% chance of invincibility
		else
			n := 30;
		endif

	// Higher than 10
	else
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of fortification
		elseif (power < 50000)
			n := 20;

		// 75% chance of invulnerability
		elseif (power < 80000)
			n := 25;
		// 20% chance of invincibility
		else
			n := 30;
		endif
	endif

	item.ar_mod := n;

	//Check for another enchantment
	// Toadstool: Again well done, every level has a 10% chance of hitting this, with 10 AWLAYS hitting
   	if( (Random(100)+1) <= (10 * magiclevel) )
       	if ((Random(100)+1) <= 75) // 75% of adding a hp mod
       			ApplyHPMod( item );
		else
			ApplyArSkillMod( item ); // 25% chance of adding a ar skill mod
		endif
   	endif


endfunction


function ApplyMiscArMod( item )

	var amount;
	/*
	var numb := (Random(50)+1) * magiclevel * 2;
	case( Cint(magiclevel / 3) )
		0:	break;
		1:	if( numb < 200 )
				numb := 200;
			endif
			break;
			
		2:	if( numb < 350 )
				numb := 350;
			endif
			break;
	endcase

   	if( numb < 50 )
       		amount := 1;
   	elseif( numb < 150 )
       		amount := 2;
   	elseif( numb < 350 )
       		amount := 3;
   	elseif( numb < 500 )
       		amount := 4;
   	elseif( numb < 600 )
       		amount := 5;
	else
		amount := 6;
   	endif
   	*/

var power := RandomInt(100000)+1;
var n;

// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance iron
		if (power < 98000)
			n := 5;

		// 1.9% chance steel
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of iron
		if (power < 82000)
			n := 5;

		// 17.5% chance of steel
		elseif (power < 99500)
			n := 10;

		// .49% chance of met steel
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of iron
		if (power < 30000)
			n := 5;

		// 80% chance of steel
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of met steel
		else
			n := 15;		
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of iron
		if (power < 20000)
			n := 5;

		// 64.497% chance of steel
		elseif (power < 64997)
			n := 10;

		// 25% chance of met steel
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of obsidian
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of iron
		if (power < 15000)
			n := 5;

		// 34% chance of steel
		elseif (power < 44999)
			n := 10;

		// 45% chance of met steel
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of obsidian
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of iron
		if (power < 3000)
			n := 5;

		// 0% chance of steel
		elseif (power < 18000)
			n := 10;

		// 17% chance of met steel
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of obsidian
		elseif (power < 77000)
			n := 20;

		// 25% chance of onynx
		elseif (power < 97000)
			n := 25;
		// 8% chance of adamantium
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of iron
		if (power < 3000)
			n := 5;

		// 0% chance of steel
		elseif (power < 18000)
			n := 10;

		// 17% chance of met steel
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of obsidian
		elseif (power < 77000)
			n := 20;

		// 25% chance of onynx
		elseif (power < 97000)
			n := 25;
		// 8% chance of adamantium
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of iron
		if (power < 0)
			n := 5;

		// 0% chance of steel
		elseif (power < 0)
			n := 10;

		// 4% chance of met steel
		elseif (power < 38000)
			n := 15;
		
		// 40% chance of obsidian
		elseif (power < 68000)
			n := 20;

		// 45% chance of onynx
		elseif (power < 95000)
			n := 25;
		// 11% chance of adamantium
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of iron
		if (power < 0)
			n := 5;

		// 0% chance of steel
		elseif (power < 0)
			n := 10;

		// 0% chance of met steel
		elseif (power < 28000)
			n := 15;
		
		// 26% chance of obsidian
		elseif (power < 63000)
			n := 20;

		// 60% chance of onynx
		elseif (power < 90000)
			n := 25;
		// 14% chance of adamantium
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of iron
		if (power < 0)
			n := 5;

		// 0% chance of steel
		elseif (power < 0)
			n := 10;

		// 0% chance of met steel
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of obsidian
		elseif (power < 60000)
			n := 20;

		// 70% chance of onynx
		elseif (power < 96000)
			n := 25;
		// 17% chance of adamantium
		else
			n := 30;
		endif

	// Higher than 10
	else
		
		// 0% chance of iron
		if (power < 0)
			n := 5;

		// 0% chance of steel
		elseif (power < 0)
			n := 10;

		// 0% chance of met steel
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of obsidian
		elseif (power < 50000)
			n := 20;

		// 75% chance of onynx
		elseif (power < 80000)
			n := 25;
		// 20% chance of adamantium
		else
			n := 30;
		endif
	endif
	
	// So i'm lazy!
	n := n/5;

   	SetObjProperty( item , "ArBonus" , n );
   	
   	//Check for another enchantment
	// Toad: This is actually well done. Every level has an extra 8% of this being hit
   	if( (Random(100)+1) <= (8 * magiclevel) )
		ApplyMiscSkillMod( item );
   	endif
   	
endfunction

//START WEAPON HIT SCRIPTS#####################################################################################################################
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Toadstool
//
// Applyweaphitscript() - Assigns either a spell strike scripts, slayer scripts, 
// effect script (pzn, stam drain, piercing), gm wep, brock
//
// Originally only level 8+ loot could even trigger this.
//
// Now all loot can hit it, lvl 4 has 5% chance. SO it's pretty high.
// But we want this to allow low cast rates, crappy slayer types, etc.
//
// Notes: It appears that effect/gm parts don't get hit enough, test
//
///////////////////////////////////////////////////////////////////////////////////////////////////

function ApplyWeapHitscript( who , item )

	CountAvailHitscripts();
/*
	var scripttype := (Random(94)+1) + (magiclevel * 2);
	if (scripttype <= 30)
		ApplySpellHitscript( item ); // Spell on hit
	elseif (scripttype <= 50)
		ApplySlayerHitscript( item ); // slayer weapons
	elseif (scripttype <= 90)
		ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing

	elseif (scripttype <= 110) 
		DestroyItem( item );
		CreateFromRandomString( who , "GMWeapon" ); // gm weps
	else
		ApplyGreaterHitscript( item ); // only lvl 8+ loot could hit this, brock, void, ele fury
	endif

	Note: Remember, only level 8+ loot could even get here. So ApplySpellHitscript would barely get hit...
*/

	var power := RandomInt(100000);
	// 1% == 1000

 	if (magiclevel <= 0)
			ApplySpellHitscript( item ); // Spell on hit
	elseif (magiclevel == 1)
		
		// 70% chance of spell
		if (power < 70000)
			ApplySpellHitscript( item ); // Spell on hit

		// 30% chance slayer
		else
			ApplySlayerHitscript( item ); // slayer weapons

		endif

	elseif (magiclevel == 2)
		
		// 60% chance of spell
		if (power < 60000)
			ApplySpellHitscript( item ); // Spell on hit

		// 40% chance slayer
		else
			ApplySlayerHitscript( item ); // slayer weapons

		endif

	elseif (magiclevel == 3)
		
		// 48% chance of spell
		if (power < 48000)
			ApplySpellHitscript( item ); // Spell on hit

		// 48% chance slayer
		elseif (power < 96000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 3% chance effect
		elseif (power < 99900)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 1% chance br void elefury
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 4)
		
		// 46% chance of spell
		if (power < 46000)
			ApplySpellHitscript( item ); // Spell on hit

		// 47% chance slayer
		elseif (power < 92000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 7% chance effect
		elseif (power < 99900)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 1% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 5)
		
		// 45% chance of spell
		if (power < 44000)
			ApplySpellHitscript( item ); // Spell on hit

		// 45% chance slayer
		elseif (power < 88000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 10% chance effect
		elseif (power < 99500)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 2% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 6)
		
		// 42% chance of spell
		if (power < 42000)
			ApplySpellHitscript( item ); // Spell on hit

		// 42% chance slayer
		elseif (power < 84000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 14% chance effect
		elseif (power < 99500)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 2% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 7)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellHitscript( item ); // Spell on hit

		// 40% chance slayer
		elseif (power < 80000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 17% chance effect
		elseif (power < 99200)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 3% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 8)
		
		// 37% chance of spell
		if (power < 37000) 
			ApplySpellHitscript( item ); // Spell on hit
		
		// 37% chance slayer
		elseif (power < 74000) 
			ApplySlayerHitscript( item ); // slayer weapons
		
		// 22% chance effect
		elseif (power < 99200)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 4% chance gm
		else
			ApplyGreaterHitscript( item ); //brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 9)
		
		// 35% chance of spell
		if (power < 35000)
			ApplySpellHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 70000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 25% chance effect
		elseif (power < 99000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 5% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	elseif (magiclevel == 10)
		
		// 34% chance of spell
		if (power < 34000)
			ApplySpellHitscript( item ); // Spell on hit

		// 34% chance slayer
		elseif (power < 68000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 26% chance effect
		elseif (power < 98000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 6% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif

	// Higher than 10
	else
		
		// 32% chance of spell
		if (power < 30000)
			ApplySpellHitscript( item ); // Spell on hit

		// 32% chance slayer
		elseif (power < 60000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 29% chance effect
		elseif (power < 97000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 7% chance gm
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	
		
	endif

endfunction


function ApplySpellHitscript( item )

	set_critical( 1 );
	var multiplier := 10;
	
	//Set the spell at random?
	var i := RandomDiceStr( "1d" + availspells.Size() );
	var n := availspells[i];
	var spell := hitscriptcfg[n].Spellid;
	SetObjProperty( item , "HitWithSpell" , spell );
	
	//Set the spell power
	var ascircle := 1;
	var rand := RandomInt(100000)+1;

	//1000 == 1%
	if (magiclevel == 0)
		ascircle := 1;
	
	elseif (magiclevel == 1)
		
		//95% 1st circle
		if (rand < 98000)
		
			ascircle := 1;
	
		//4% 2nd circle
		else

			ascircle := 2;
		endif

	elseif (magiclevel == 2)
		
		//90% 1st circle
		if (rand < 82000)
		
			ascircle := 1;
	
		//8% 2nd circle
		elseif (rand < 99500)

			ascircle := 2;

		//2% 3rd circle
		else
			ascircle := 3;
		endif

	elseif (magiclevel == 3)
		
		//60% 1st circle
		if (rand < 30000)
		
			ascircle := 1;
	
		//30% 2nd circle
		elseif (rand < 90000)

			ascircle := 2;

		//8% 3rd circle
		else
			ascircle := 3;
		endif

	elseif (magiclevel == 4)
		
		//40% 1st circle
		if (rand < 20000)
		
			ascircle := 1;
	
		//40% 2nd circle
		elseif (rand < 64997)

			ascircle := 2;

		//15% 3rd circle
		elseif (rand < 94997)
			ascircle := 3;

		//5% 4th circle
		else
			ascircle := 4;
		endif
		
	elseif (magiclevel == 5)
		
		//20% 1st circle
		if (rand < 15000)
		
			ascircle := 1;
	
		//20% 2nd circle
		elseif (rand < 44999)

			ascircle := 2;

		//30% 3rd circle
		elseif (rand < 90999)
			ascircle := 3;

		//15% 4th circle
		else
			ascircle := 4;
		endif

	elseif (magiclevel == 6)
		
		//0% 1st circle
		if (rand < 1000)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 16000)

			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//30% 4th circle
		elseif (rand < 74000)
			ascircle := 4;

		//30% 5th circle
		elseif (rand < 94000)
			ascircle := 5;

		//15% 6th circle
		else
			ascircle := 6;
		endif
	
	elseif (magiclevel == 7)
		
		//0% 1st circle
		if (rand < 1000)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 16000)

			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//30% 4th circle
		elseif (rand < 74000)
			ascircle := 4;

		//30% 5th circle
		elseif (rand < 94000)
			ascircle := 5;

		//15% 6th circle
		else
			ascircle := 6;
		endif

	elseif (magiclevel == 8)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//5% 3rd circle
		elseif (rand < 61000)
			ascircle := 3;

		//15% 4th circle
		elseif (rand < 70000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 85000)
			ascircle := 5;

		//30% 6th circle
		elseif (rand < 95000)
			ascircle := 6;

		//30% 6th circle
		else
			ascircle := 7;
		endif

	elseif (magiclevel == 9)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 26000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 61000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 70000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 85000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 7;

		//15% 7th circle
		else
			ascircle := 8;
		endif

	elseif (magiclevel == 10)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 26000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 61000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 70000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 85000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 7;

		//15% 7th circle
		elseif (rand < 97000)
			ascircle := 8;

		else 
			ascircle := 9;
		endif

	elseif (magiclevel > 10)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 20000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 55000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 65000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 80000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 92000)
			ascircle := 7;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 8;

		//15% 7th circle
		elseif (rand < 97000)
			ascircle := 9;

		else 
			ascircle := 10;
		endif

	endif
	
	if ((ascircle > 6) || (n==10))
		SetObjProperty(item, "isRare", 1);
	elseif(n==14 || n==7 || n==9 || n==17)
		SetObjProperty(item, "isUncommon", 1);
	endif

	SetObjProperty( item , "EffectCircle" , ascircle );

	//Set the spell chance - Leave this for the time being.  
	var chanceofeffect := RandomDiceStr("1d11") * magiclevel;
	
	// cap max effect to 95
	if (chanceofeffect <= 0)
		chanceofeffect := 5;
	elseif (chanceofeffect > 80)
		chanceofeffect := 80;
	endif

	SetObjProperty( item , "ChanceOfEffect" , chanceofeffect );
	
	SetHitscript( item , n , multiplier );
	set_critical( 0 );

endfunction


////////////////////////////////////////////////////////
//
// Toadstool - Left this function as is.
//
// Previously low loot would NEVER get here anyway.
// So now we should see a lot of the lower slayers...
//
////////////////////////////////////////////////////////
function ApplySlayerHitscript( item )

	set_critical( 1 );
	
	var multiplier := 15;
	var newRoll := magiclevel + 2;
	if(newRoll > 10)
		newRoll := 10;
	endif

	//Set the creature type
	var n;
	var power := (Random(100)+1) * newRoll/2;
	if (power < 50) // Plant
		n := 21;  
	elseif (power < 105)  // Animal
		n := 22;
	elseif (power < 155)  // Slime
		n := 19;
	elseif (power < 180)  // Beholder
		n := 23;
	elseif (power < 205)  // Bewitched
		n := 27;
	elseif (power < 230)  // Gargoyle
		n := 28;
	elseif (power < 255)  // Giant
		n := 30;
	elseif (power < 280)  // Troll
		n := 29;
	elseif (power < 305)  // Ophidian
		n := 26;
	elseif (power < 330)  // Elemental
		n := 31;
	elseif (power < 355)  // Terathan
		n := 25;
	elseif (power < 400)  // Orc
		n := 24;
	elseif (power < 420) // Daemon
		n := 33;
	elseif (power < 445)  // Rat
		n := 20;
	elseif (power < 470) // Dragon
		n := 34;
	elseif (power < 490) // Undead
		n := 32;
	else				 // Bounty
		n := 35;
	endif

	//n := 35; // Bounty (removed cause we don't use the assassin slayertype.)

	if(n==34 || n==32 || n==10 || n==35)
		SetObjProperty(item, "isRare", 1);
	elseif(n==14 || n==7 || n==9 || n==17 || n==20 || n==33 || n==24)
		SetObjProperty(item, "isUncommon", 1);
	endif
	
	var type := hitscriptcfg[n].Type;
	SetObjProperty( item , "SlayType" , type );

	SetHitscript( item , n , multiplier );
	
	set_critical( 0 );

endfunction


function ApplyEffectHitscript( item )

	set_critical( 1 );
	
	var multiplier := 10;
	
	//Find the good effect
	var n, loop := 0;
	repeat
		var i := RandomDiceStr("1d"+availeffects.Size());
		n := availeffects[i];
		loop := loop+1;
	until(n != 36 || loop >= 100);
	if (loop >= 100)
		n := 37;
	endif

	SetHitscript( item , n , multiplier );
	
	set_critical( 0 );

endfunction


function ApplyGreaterHitscript ( item )

	set_critical( 1 );
	
	var multiplier := 2;
	
	//Find the good effect

	var i := RandomDiceStr("1d"+availgreaters.Size());
	var n := availgreaters[i];
	
	SetHitscript( item , n , multiplier );
	
	set_critical( 0 );

endfunction

//## END WEAPON HIT SCRIPTS ######################################################################################################################

///////////////////////////////////////////////////////
// Toadstool
//
// This is a copy/paste of armours chances. Still yet to
// remove the comments.  Change chances accordingly...
//
// Directly below this function are others that are copied and
// pasted.  tweak them as needed
///////////////////////////////////////////////////////
function ApplyWeapSkillMod( item )

	var chance := Random(1000)+1;
	if(chance <= 20)
		ApplyStatMod( item );
		SetObjProperty(item, "isRare", 1);
		return;
	endif

	var amount;

	var power := RandomInt(100000)+1;
	var n;

	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance of defence
		if (power < 98000)
			n := 5;

		// 1.9% chance of guarding
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of defence
		if (power < 82000)
			n := 5;

		// 17.5% chance of guarding
		elseif (power < 99500)
			n := 10;

		// .49% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of defence
		if (power < 30000)
			n := 5;

		// 80% chance of guarding
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of defence
		if (power < 20000)
			n := 5;

		// 64.497% chance of guarding wtf
		elseif (power < 64997)
			n := 10;

		// 25% chance of hardening
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of defence
		if (power < 15000)
			n := 5;

		// 34% chance of guarding
		elseif (power < 44999)
			n := 10;

		// 45% chance of hardening
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of defence
		if (power < 2000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 4% chance of hardening
		elseif (power < 38000)
			n := 15;
		
		// 40% chance of fortification
		elseif (power < 68000)
			n := 20;

		// 45% chance of invulnerability
		elseif (power < 90000)
			n := 25;
		// 11% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 28000)
			n := 15;
		
		// 26% chance of fortification
		elseif (power < 63000)
			n := 20;

		// 60% chance of invulnerability
		elseif (power < 85000)
			n := 25;
		// 14% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of fortification
		elseif (power < 60000)
			n := 20;

		// 70% chance of invulnerability
		elseif (power < 96000)
			n := 25;
		// 17% chance of invincibility
		else
			n := 30;
		endif

	// Higher than 10
	else
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of fortification
		elseif (power < 50000)
			n := 20;

		// 75% chance of invulnerability
		elseif (power < 80000)
			n := 25;
		// 20% chance of invincibility
		else
			n := 30;
		endif
	endif

	amount := n;

	var skillid;

	if(n == 25)
		SetObjProperty(item, "isUncommon", 1);
	elseif(n == 30)
		SetObjProperty(item, "isRare", 1);
	endif

	case(Random(2)+1)	
		1:	skillid := GetSkillIdByAttributeId(item.attribute);
			break;
		default: skillid := SKILLID_TACTICS;
	endcase

	// bows can only have archery, no tactics
	if (lower(item.attribute) == "archery")
		skillid := GetSkillIdByAttributeId(item.attribute);
	endif

	SetObjProperty( item , "skilladv" , skillid );
	SetObjProperty( item , "skilladvamount" , amount );
	SetObjProperty( item , "WeapAdv" , 1 );

	//Check for another enchantment - 5% extra chance of being hit per level
	if ((Random(100)+1) <= (5 * magiclevel))
		ApplyHpMod( item );
	endif

endfunction


function ApplyArSkillMod( item )

	var chance := Random(1000)+1;
	if(chance <= 20)
		ApplyStatMod( item );
		return;
	endif

	var skillid;
	var amount;
/*
	var numb := (Random(50)+1) * magiclevel * 2;
	
	case( Cint(magiclevel / 3) )
		0:	break;
		1:	if( numb < 200 )
				numb := 200;
			endif
			break;
			
		2:	if( numb < 300 )
				numb := 300;
			endif
			break;
	endcase
	
   	if( numb < 200 )
		amount := 1;
   	elseif( numb < 300 )
		amount := 2;
   	elseif( numb < 400 )
		amount := 3;
   	elseif( numb < 500 )
		amount := 4;
   	elseif( numb < 600 )
		amount := 5;
	else
		amount := 6;
   	endif
	*/

var power := RandomInt(100000)+1;
var n;

	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance of defence
		if (power < 98000)
			n := 5;

		// 1.9% chance of guarding
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of defence
		if (power < 82000)
			n := 5;

		// 17.5% chance of guarding
		elseif (power < 99500)
			n := 10;

		// .49% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of defence
		if (power < 30000)
			n := 5;

		// 80% chance of guarding
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of defence
		if (power < 20000)
			n := 5;

		// 64.497% chance of guarding
		elseif (power < 64997)
			n := 10;

		// 25% chance of hardening
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of defence
		if (power < 15000)
			n := 5;

		// 34% chance of guarding
		elseif (power < 44999)
			n := 10;

		// 45% chance of hardening
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 4% chance of hardening
		elseif (power < 38000)
			n := 15;
		
		// 40% chance of fortification
		elseif (power < 68000)
			n := 20;

		// 45% chance of invulnerability
		elseif (power < 90000)
			n := 25;
		// 11% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 28000)
			n := 15;
		
		// 26% chance of fortification
		elseif (power < 63000)
			n := 20;

		// 60% chance of invulnerability
		elseif (power < 85000)
			n := 25;
		// 14% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of fortification
		elseif (power < 60000)
			n := 20;

		// 70% chance of invulnerability
		elseif (power < 96000)
			n := 25;
		// 17% chance of invincibility
		else
			n := 30;
		endif

	// Higher than 10
	else
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of fortification
		elseif (power < 50000)
			n := 20;

		// 75% chance of invulnerability
		elseif (power < 80000)
			n := 25;
		// 20% chance of invincibility
		else
			n := 30;
		endif
	endif

	if(n == 30)
		SetObjProperty(item, "isRare", 1);
	endif


	amount := n/5;

	case( Random(2)+1 )
		1:	skillid := SKILLID_MAGICRESISTANCE;
			break;
			
		default: skillid := SKILLID_HIDING;
	endcase	

	SetObjProperty( item , "skilladv" , skillid );
	SetObjProperty( item , "skilladvamount" , amount );

	//Check for another enchantment
	if ((Random(100)+1) <= (5 * magiclevel))
		ApplyHpMod( item );
	endif
	
endfunction


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// ApplyMiscSkillMod() - Adds a random skill on an item, gives it a chance to be +stat too
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function ApplyMiscSkillMod( item )

	// So far stat mod is PURE chance, .5% chance to be a stat mod
	// Not based on lootlevel or anything
	var chance := Random(1000)+1;
	if(chance <= 20)
		ApplyStatMod( item );
		return;
	endif

	var skillid;
	var amount;

	/*
	var numb := (Random(50)+1) * magiclevel * 2;
	
		NUMB
	lvl 1 min : 2	lvl 1 max: 100
	lvl 2 min : 4 	lvl 2 max: 200
	lvl 3 min : 6 	lvl 3 max: 300
	lvl 4 min : 8	lvl 4 max: 400
	etc etc


	// This part just makes sure higher levels can't be below that number - making low drops impossible
	case( Cint(magiclevel / 3) )
		0:	break;
		1:	if( numb < 200 )
				numb := 200;
			endif
			break;
			
		2:	if( numb < 300 )
				numb := 300;
			endif
			break;
	endcase
	
	// Starts off at +2!!??
   	if( numb < 50 )
		amount := 2;
   	elseif( numb < 300 )
		amount := 2;
   	elseif( numb < 450 )
		amount := 3;
   	elseif( numb < 700 )
		amount := 4;
   	elseif( numb < 799 )
		amount := 5;
	else
		amount := 6;
   	endif
	*/
	var power := RandomInt(100000)+1;
	var n;

	if (magiclevel <= 0)
			n := 5;
	elseif (magiclevel == 1)
		
		// 98% chance of defence
		if (power < 98000)
			n := 5;

		// 1.9% chance of guarding
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		
		// 82% chance of defence
		if (power < 82000)
			n := 5;

		// 17.5% chance of guarding
		elseif (power < 99500)
			n := 10;

		// .49% chance of hardening
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		
		// 10% chance of defence
		if (power < 30000)
			n := 5;

		// 80% chance of guarding
		elseif (power < 90000)
			n := 10;

		// 9.499% chance of hardening
		else
			n := 15;
				
		endif

	elseif (magiclevel == 4)
		
		// .5% chance of defence
		if (power < 20000)
			n := 5;

		// 64.497% chance of guarding
		elseif (power < 64997)
			n := 10;

		// 25% chance of hardening
		elseif (power < 94997)
			n := 15;
		
		// 10% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		
		// 0% chance of defence
		if (power < 15000)
			n := 5;

		// 34% chance of guarding
		elseif (power < 44999)
			n := 10;

		// 45% chance of hardening
		elseif (power < 90999)
			n := 15;
		
		// 18% chance of fortification
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		
		// 0% chance of defence
		if (power < 3000)
			n := 5;

		// 0% chance of guarding
		elseif (power < 18000)
			n := 10;

		// 17% chance of hardening
		elseif (power < 48000)
			n := 15;
		
		// 50% chance of fortification
		elseif (power < 77000)
			n := 20;

		// 25% chance of invulnerability
		elseif (power < 97000)
			n := 25;
		// 8% chance of invincibility
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 4% chance of hardening
		elseif (power < 36000)
			n := 15;
		
		// 40% chance of fortification
		elseif (power < 66000)
			n := 20;

		// 45% chance of invulnerability
		elseif (power < 90000)
			n := 25;
		// 11% chance of invincibility
		else
			n := 30;

		endif

	elseif (magiclevel == 9)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 26000)
			n := 15;
		
		// 26% chance of fortification
		elseif (power < 61000)
			n := 20;

		// 60% chance of invulnerability
		elseif (power < 85000)
			n := 25;
		// 14% chance of invincibility
		else
			n := 30;

		endif

	elseif (magiclevel == 10)
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 20000)
			n := 15;
		
		// 13% chance of fortification
		elseif (power < 60000)
			n := 20;

		// 70% chance of invulnerability
		elseif (power < 96000)
			n := 25;

		// 14% chance of invincibility
		else
			n := 30;

		endif
	// Higher than 10
	else
		
		// 0% chance of defence
		if (power < 0)
			n := 5;

		// 0% chance of guarding
		elseif (power < 0)
			n := 10;

		// 0% chance of hardening
		elseif (power < 15000)
			n := 15;
		
		// 5% chance of fortification
		elseif (power < 50000)
			n := 20;

		// 75% chance of invulnerability
		elseif (power < 80000)
			n := 25;

		// 14% chance of invincibility
		else
			n := 30;

		endif

	endif

	if(n == 25)
		SetObjProperty(item, "isUncommon", 1);
	elseif(n == 30)
		SetObjProperty(item, "isRare", 1);
	endif

	amount := n/5;

	var multiplier := Cint( GetObjProperty( item , "SpecMult" ) );
	if( multiplier )
		amount := amount * multiplier;
	endif
		
	var skills := GetObjProperty( item , "AvailSkills" );
	if ( skills )
		skillid := skills[RandomDiceStr("1d" + len(skills))];
	else		
		skillid := Random(SKILLID__HIGHEST + 1);
	endif

	SetObjProperty( item , "skilladv" , skillid );
	SetObjProperty( item , "skilladvamount" , amount );

//if(n == 50)
//	if(skillid == SKILLID_ALCHEMY or skillid == SKILLID_MAGERY or skillid == SKILLID_ITEMID or skillid == SKILLID_EVALINT or skillid == SKILLID_INSCRIPTION or skillid == SKILLID_MAGICRESISTANCE or skillid == SKILLID_MEDITATION or skillid == SKILLID_SPIRITSPEAK)
//		EraseObjProperty( item , "ArBonus");
//	endif
//endif

endfunction


/////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// ApplyOnHitScript() - assigns the armours hit script.  
//
// Just copy pasted from weap
//
/////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyOnHitScript ( who , item )

	CountAvailOnHitscripts();

	var power := RandomInt(100000)+1;

	// 1% == 1000

 	if (magiclevel <= 0)
			ApplySpellOnHitscript( item ); // Spell on hit
	elseif (magiclevel == 1)
		
		// 80% chance of spell
		if (power < 80000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 20% chance slayer
		else
			ApplyResistantOnHitscript( item ); // 

		endif

	elseif (magiclevel == 2)
		
		// 60% chance of spell
		if (power < 60000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 40% chance slayer
		else
			ApplyResistantOnHitscript( item ); // 

		endif

	elseif (magiclevel == 3)
		
		// 50% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 45% chance slayer
		elseif (power < 95000)
			ApplyResistantOnHitscript( item ); // 

		// 4% chance effect
		else 
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 4)
		
		// 45% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 45% chance slayer
		elseif (power < 90000)
			ApplyResistantOnHitscript( item ); // 

		// 8% chance effect
		else 
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 5)
		
		// 45% chance of spell
		if (power < 45000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 40% chance slayer
		elseif (power < 85000)
			ApplyResistantOnHitscript( item ); // 

		// 12% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 6)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 75000)
			ApplyResistantOnHitscript( item ); // 

		// 20% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 7)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 65000)
			ApplyResistantOnHitscript( item ); // 

		// 30% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif		

	elseif (magiclevel == 8)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 33% chance slayer
		elseif (power < 63000)
			ApplyResistantOnHitscript( item ); // 

		// 31% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 9)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 31% chance slayer
		elseif (power < 61000)
			ApplyResistantOnHitscript( item ); // 

		// 32% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	elseif (magiclevel == 10)
		
			// 29% chance of spell
		if (power < 29000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 30% chance slayer
		elseif (power < 59000)
			ApplyResistantOnHitscript( item ); // 

		// 33% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif

	// Higher than 10
	else
		
		// 28% chance of spell
		if (power < 28000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 28% chance slayer
		elseif (power < 56000)
			ApplyResistantOnHitscript( item ); // 

		// 35% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	
	endif
endfunction



function ApplySpellOnHitscript( item )

	set_critical( 1 );
	
	var multiplier := 10;
	
	//Set the spell
	var i := RandomDiceStr( "1d"+availspellonhit.Size());
	var n := availspellonhit[i];
	var spell := onhitcfg[n].Spellid;
	SetObjProperty( item , "HitWithSpell" , spell );
	//Set the spell power
	var ascircle;

	var rand := RandomInt(100000)+1;

	//1000 == 1%

	if (magiclevel == 0)
		ascircle := 1;
	
	elseif (magiclevel == 1)
		
		//95% 1st circle
		if (rand < 98000)
		
			ascircle := 1;
	
		//4% 2nd circle
		else

			ascircle := 2;
		endif

	elseif (magiclevel == 2)
		
		//90% 1st circle
		if (rand < 82000)
		
			ascircle := 1;
	
		//8% 2nd circle
		elseif (rand < 99500)

			ascircle := 2;

		//2% 3rd circle
		else
			ascircle := 3;
		endif

	elseif (magiclevel == 3)
		
		//60% 1st circle
		if (rand < 30000)
		
			ascircle := 1;
	
		//30% 2nd circle
		elseif (rand < 90000)

			ascircle := 2;

		//8% 3rd circle
		else
			ascircle := 3;
		endif

	elseif (magiclevel == 4)
		
		//40% 1st circle
		if (rand < 20000)
		
			ascircle := 1;
	
		//40% 2nd circle
		elseif (rand < 64997)

			ascircle := 2;

		//15% 3rd circle
		elseif (rand < 94997)
			ascircle := 3;

		//5% 4th circle
		else
			ascircle := 4;
		endif
		
	elseif (magiclevel == 5)
		
		//20% 1st circle
		if (rand < 15000)
		
			ascircle := 1;
	
		//20% 2nd circle
		elseif (rand < 44999)

			ascircle := 2;

		//30% 3rd circle
		elseif (rand < 90999)
			ascircle := 3;

		//15% 4th circle
		else
			ascircle := 4;
		endif

	elseif (magiclevel == 6)
		
		//0% 1st circle
		if (rand < 1000)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 16000)

			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//30% 4th circle
		elseif (rand < 75000)
			ascircle := 4;

		//30% 5th circle
		elseif (rand < 95000)
			ascircle := 5;

		//15% 6th circle
		else
			ascircle := 6;
		endif
	
	elseif (magiclevel == 7)
		
		//0% 1st circle
		if (rand < 1000)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 16000)

			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//30% 4th circle
		elseif (rand < 75000)
			ascircle := 4;

		//30% 5th circle
		elseif (rand < 95000)
			ascircle := 5;

		//15% 6th circle
		else
			ascircle := 6;
		endif

	elseif (magiclevel == 8)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//5% 3rd circle
		elseif (rand < 61000)
			ascircle := 3;

		//15% 4th circle
		elseif (rand < 70000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 85000)
			ascircle := 5;

		//30% 6th circle
		elseif (rand < 95000)
			ascircle := 6;

		//30% 6th circle
		else
			ascircle := 7;
		endif

	elseif (magiclevel == 9)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 26000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 61000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 70000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 85000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 7;

		//15% 7th circle
		else
			ascircle := 8;
		endif

	elseif (magiclevel == 10)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 26000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 61000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 70000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 85000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 7;

		//15% 7th circle
		elseif (rand < 97000)
			ascircle := 8;

		else 
			ascircle := 9;
		endif

	elseif (magiclevel > 10)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% 3rd circle
		elseif (rand < 20000)
			ascircle := 3;

		//10% 4th circle
		elseif (rand < 55000)
			ascircle := 4;

		//40% 5th circle
		elseif (rand < 65000)
			ascircle := 5;

		//35% 6th circle
		elseif (rand < 80000)
			ascircle := 6;

		//15% 7th circle
		elseif (rand < 92000)
			ascircle := 7;

		//15% 7th circle
		elseif (rand < 95000)
			ascircle := 8;

		//15% 7th circle
		elseif (rand < 97000)
			ascircle := 9;

		else 
			ascircle := 10;
		endif

	endif

	var ascirclemod := onhitcfg[n].AsCircleMod;
	if (ascirclemod)
		ascircle := ascircle + ascirclemod;
	endif

	if (ascircle <= 0)
		ascircle := 1;
	elseif(ascircle > 5)
		SetObjProperty(item, "isRare", 1);
	endif

	SetObjProperty( item , "EffectCircle" , ascircle );

	//Set the spell chance
	var chanceofeffect := RandomDiceStr("1d11") * magiclevel;
	
	if (chanceofeffect <= 0)
		chanceofeffect := 5;
	elseif (chanceofeffect > 80)
		chanceofeffect := 80;
	endif

	SetObjProperty( item , "ChanceOfEffect" , chanceofeffect );

	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction


function ApplyResistantOnHitscript( item )

	set_critical( 1 );
	
	var multiplier := 15;
	
	var newRoll := magiclevel + 1;
	if(newRoll > 10)
		newRoll := 10;
	endif

	//Set the creature type
	var n;
	var power := (Random(100)+1) * (newRoll);
	if (power < 60)
		n := 19;
	elseif (power < 120)
		n := 20;
	elseif (power < 180)
		n := 21;
	elseif (power < 240)
		n := 22;
	elseif (power < 300)
		n := 23;
	elseif (power < 360)
		n := 24;
	elseif (power < 420)
		n := 25;
	elseif (power < 480)
		n := 26;
	elseif (power < 540)
		n := 27;
	elseif (power < 600)
		n := 28;
	elseif (power < 660)
		n := 29;
	elseif (power < 720)
		n := 30;
	elseif (power < 780)
		n := 31;
	elseif (power < 820)
		n := 32;
	elseif (power < 860)
		n := 33;
	elseif (power < 900)
		n := 34;
	else
		n := 35;
	endif

	if(n > 15)
		SetObjProperty(item, "isRare", 1);
	endif

	var type := onhitcfg[n].Type;
	SetObjProperty( item , "ProtectedType" , type );

	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction


function ApplyEffectOnHitscript( item )

	set_critical( 1 );
	
	var multiplier := 10;
	
	//Find the good effect
	var i := RandomDiceStr( "1d"+availeffectonhit.Size());
	var n := availeffectonhit[i];
	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction


function ApplyGreaterOnHitscript( item )
	
	set_critical( 1 );
	
	var multiplier := 2;
	
	//Find the good script
	var i := RandomDiceStr( "1d"+availgreateronhit.Size());
	var n := availgreateronhit[i];

	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// Str, Int and Dex Mods
//
// Currently it's exactly the same as elemental mods, copy paste
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function ApplyStatMod ( item )

/*
	var numb := (Random(100)+1) * (magiclevel - 3);
	var amount;
   	if( numb < 200 )    // Any loot below 5 will be +5. Level 5 loot has a 99.5% of +5
		amount := 2;
   	elseif( numb < 450 ) 
		amount := 4;
   	elseif( numb < 550 ) 
		amount := 6;
   	elseif( numb < 700 ) 
		amount := 8;
   	elseif( numb < 750 )
		amount := 10;
	else
		amount := 12;
   	endif
*/

	var power := RandomInt(100000)+1;
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;
	// Level 1: orc cleric, lizardmanshaman, shade, zulu, spectre, ettin, ogre 
	elseif (magiclevel == 1)
		
		// 98.9% chance of level 1
		if (power < 80000)
			n := 1;

		// 1.09% chance of level 2
		else
			n := 2;
		endif

	// Level 2: winged avenger, beholder, frost/flame/stone garg, evil warlock, adept mage
	elseif (magiclevel == 2)
	
		// 90% chance of level 1
		if (power < 70000)
			n := 1;

		// 9% chance of level 2
		elseif (power < 90000)
			n := 2;

		// 1% chance of level 3
		else
			n := 3;
		endif

	// Level 3: Corrupt Terethen, lesser shadow, nightmare, necromancer
	elseif (magiclevel == 3)
	
		// 60% chance of level 1
		if (power < 30000)
			n := 1;

		// 35% chance of level 2
		elseif (power < 80000)
			n := 2;

		// 4.75% chance of level 3
		else
			n := 3;
		endif

	// Level 4: abonible snowman, ebony horror, orc necromancer, ice ele lord, evis carc
	elseif (magiclevel == 4)

		// 35% chance of level 1
		if (power < 20000)
			n := 1;

		// 54.5% chance of level 2
		elseif (power < 70000)
			n := 2;

		// 10% chance of level 3
		elseif (power < 80000)
			n := 3;
		
		// .5% chance of level 4
		else
			n := 4;
		endif

	// Level 5: black wisp, golden drake, dark rider, bezerker, twister animation
	elseif (magiclevel == 5)

		// 8.99% chance of level 1
		if (power < 15000)
			n := 1;

		// 60% chance of level 2
		elseif (power < 44999)
			n := 2;

		// 29% chance of level 3
		elseif (power < 90000)
			n := 3;
		
		// 2% chance of level 4
		else
			n := 4;
		endif
	
	//Level 6: Undead flayer, sand dragon, dragon lord, dracoliche, fallenangel
	elseif (magiclevel == 6)

		// 3% chance of level 1
		if (power < 4000)
			n := 1;

		// 34.5% chance of level 2
		elseif (power < 39000)
			n := 2;

		// 55% chance of level 3
		elseif (power < 79000)
			n := 3;
		
		// 5% chance of level 4
		elseif (power < 95000)
			n := 4;

		// 2% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// .5% chance of level 6
		else
			n := 6;
		endif

	// Level 7: Behemoth, umbral flicker
	elseif (magiclevel == 7)

		// 3% chance of level 1
		if (power < 4000)
			n := 1;

		// 34.5% chance of level 2
		elseif (power < 34000)
			n := 2;

		// 55% chance of level 3
		elseif (power < 74000)
			n := 3;
		
		// 5% chance of level 4
		elseif (power < 94000)
			n := 4;

		// 2% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// .5% chance of level 6
		else
			n := 6;
		endif

	// Level 8: balron, Balron general, silver dragqueen, flaming bally
	elseif (magiclevel == 8)

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 14% chance of level 2
		elseif (power < 0)
			n := 2;

		// 40% chance of level 3
		elseif (power < 35000)
			n := 3;
		
		// 40% chance of level 4
		elseif (power < 65000)
			n := 4;

		// 5% chance of level 5
		elseif (power < 95000)
			n := 5;			
		
		// 1% chance of level 6
		else
			n := 6;
		endif

	// Level 9: nemesis, archangel (and soon sunshine)
	elseif (magiclevel == 9)
	
		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 5% chance of level 2
		elseif (power < 0)
			n := 2;

		// 30% chance of level 3
		elseif (power < 30000)
			n := 3;
		
		// 50% chance of level 4
		elseif (power < 60000)
			n := 4;

		// 10% chance of level 5
		elseif (power < 90000)
			n := 5;			
		
		// 5% chance of level 6
		else
			n := 6;
		endif
			

	// Anything higher than level 9, cap.
	elseif (magiclevel > 9) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 20% chance of level 3
		elseif (power < 25000)
			n := 3;
		
		// 50% chance of level 4
		elseif (power < 50000)
			n := 4;

		// 20% chance of level 5
		elseif (power < 80000)
			n := 5;			
		
		// 10% chance of level 6
		else
			n := 6;
		endif

	endif

	// So I'm lazy again!! :P
	n := n * 2;
		
	var id := Random(5)+1;
	if( id < 3 )
		SetObjProperty( item , "str" , n );
		SetObjProperty(item, "isRare", 1);
	elseif( id < 4 )
		SetObjProperty( item , "int" , n );
		SetObjProperty(item, "isRare", 1);
	else
		SetObjProperty( item , "dex" , n );
		SetObjProperty(item, "isRare", 1);
	endif

	//Check for another enchantment,
	// Add a HP mod,  each level gets an extra 2% chance
	if ((Random(100)+1) <= 2 * magiclevel)
		ApplyHpMod( item );
	endif

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// ApplyEnchant() - Function to create protection Items.
//		    3 Main Groups lowest to highest - Charge Items, Elemental Prots, Immunity Prots (brock/moon/pzn)
//			
//		    The group chance is based purely on magiclevel, the quality of the drop is also based purely on item level.
//
//		    Note: This function doesn't determine if an enchant can drop. It's what happens if one DOES drop.
//		    Look earlier to see what determines if an enchant drops.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			

function ApplyEnchant( who , item )

	var rand := RandomInt(100);
	var choice := 0;

	case (magiclevel)
		1: if (rand < 80) choice := 1; elseif (rand < 95) choice := 2; else choice := 3; endif // 80%-C 15%-E 05%-I
 		2: if (rand < 70) choice := 1; elseif (rand < 93) choice := 2; else choice := 3; endif // 70%-C 23%-E 07%-I
		3: if (rand < 60) choice := 1; elseif (rand < 90) choice := 2; else choice := 3; endif // 60%-C 30%-E 10%-I
		4: if (rand < 45) choice := 1; elseif (rand < 87) choice := 2; else choice := 3; endif // 45%-C 42%-E 13%-I
		5: if (rand < 35) choice := 1; elseif (rand < 85) choice := 2; else choice := 3; endif // 35%-C 50%-E 15%-I
		6: if (rand < 25) choice := 1; elseif (rand < 80) choice := 2; else choice := 3; endif // 25%-C 55%-E 20%-I
		7: if (rand < 20) choice := 1; elseif (rand < 75) choice := 2; else choice := 3; endif // 20%-C 55%-E 25%-I
		8: if (rand < 15) choice := 1; elseif (rand < 65) choice := 2; else choice := 3; endif // 15%-C 50%-E 35%-I
		9: if (rand < 10) choice := 1; elseif (rand < 60) choice := 2; else choice := 3; endif // 10%-C 50%-E 40%-I
		10: if (rand < 5) choice := 1; elseif (rand < 55) choice := 2; else choice := 3; endif // 05%-C 50%-E 45%-I
	endcase

	case (choice)
		1: ApplyProtection( item ); 
		2: ApplyElementalImmunity( item );
		3: ApplyImmunity( item );
	endcase

	// Still havn't touched this part yet - Leave for now.
	// Check for another enchantment
	// Old way is more chance at higher levels - 45% hitting here levl 9 - 40% at level 8, 5% level 1 10% level 2 etc
	if ((Random(100)+1) <= (5 * magiclevel))

		// 75% of it being skill mod, 25% of armour mod
		var secchance := Random(100)+1;
		if (secchance < 75)
       		ApplyMiscSkillMod( item );
		else
			ApplyMiscArMod( item );
		endif
	endif

endfunction


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// DetermineImmunityLevel() - This is the core function of determining the level for
//			      immunities/elemental/charges prots. It's based purely on the magiclevel
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
   	
function DetermineImmunityLevel(item, isimmun := 0, modifier := 0)

	var n;
	var power := RandomInt(100000)+1;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;
	// Level 1: orc cleric, lizardmanshaman, shade, zulu, spectre, ettin, ogre 
	elseif (magiclevel == 1)
		
		// 98.9% chance of level 1
		if (power < 98000)
			n := 1;

		// 1.09% chance of level 2
		else
			n := 2;
		endif

	// Level 2: winged avenger, beholder, frost/flame/stone garg, evil warlock, adept mage
	elseif (magiclevel == 2)
	
		// 90% chance of level 1
		if (power < 82000)
			n := 1;

		// 9% chance of level 2
		elseif (power < 99500)
			n := 2;

		// 1% chance of level 3
		else
			n := 3;
		endif

	// Level 3: Corrupt Terethen, lesser shadow, nightmare, necromancer
	elseif (magiclevel == 3)
	
		// 60% chance of level 1
		if (power < 30000)
			n := 1;

		// 35% chance of level 2
		elseif (power < 90000)
			n := 2;

		// 4.75% chance of level 3
		else
			n := 3;
		endif

	// Level 4: abonible snowman, ebony horror, orc necromancer, ice ele lord, evis carc
	elseif (magiclevel == 4)

		// 35% chance of level 1
		if (power < 20000)
			n := 1;

		// 54.5% chance of level 2
		elseif (power < 64997)
			n := 2;

		// 10% chance of level 3
		elseif (power < 94997)
			n := 3;
		
		// .5% chance of level 4
		else
			n := 4;
		endif

	// Level 5: black wisp, golden drake, dark rider, bezerker, twister animation
	elseif (magiclevel == 5)

		// 8.99% chance of level 1
		if (power < 15000)
			n := 1;

		// 60% chance of level 2
		elseif (power < 44999)
			n := 2;

		// 29% chance of level 3
		elseif (power < 90000)
			n := 3;
		
		// 2% chance of level 4
		else
			n := 4;
		endif
	
	//Level 6: Undead flayer, sand dragon, dragon lord, dracoliche, fallenangel
	elseif (magiclevel == 6)

		// 3% chance of level 1
		if (power < 4000)
			n := 1;

		// 34.5% chance of level 2
		elseif (power < 39000)
			n := 2;

		// 55% chance of level 3
		elseif (power < 79000)
			n := 3;
		
		// 5% chance of level 4
		elseif (power < 95000)
			n := 4;

		// 2% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// .5% chance of level 6
		else
			n := 6;
		endif

	// Level 7: Behemoth, umbral flicker
	elseif (magiclevel == 7)

		// 3% chance of level 1
		if (power < 4000)
			n := 1;

		// 34.5% chance of level 2
		elseif (power < 34000)
			n := 2;

		// 55% chance of level 3
		elseif (power < 74000)
			n := 3;
		
		// 5% chance of level 4
		elseif (power < 94000)
			n := 4;

		// 2% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// .5% chance of level 6
		else
			n := 6;
		endif

	// Level 8: balron, Balron general, silver dragqueen, flaming bally
	elseif (magiclevel == 8)

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 14% chance of level 2
		elseif (power < 0)
			n := 2;

		// 40% chance of level 3
		elseif (power < 65000)
			n := 3;
		
		// 40% chance of level 4
		elseif (power < 85000)
			n := 4;

		// 5% chance of level 5
		elseif (power < 95000)
			n := 5;			
		
		// 1% chance of level 6
		else
			n := 6;
		endif

	// Level 9: nemesis, archangel (and soon sunshine)
	elseif (magiclevel == 9)
	
		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 5% chance of level 2
		elseif (power < 0)
			n := 2;

		// 30% chance of level 3
		elseif (power < 60000)
			n := 3;
		
		// 50% chance of level 4
		elseif (power < 80000)
			n := 4;

		// 10% chance of level 5
		elseif (power < 90000)
			n := 5;			
		
		// 5% chance of level 6
		else
			n := 6;
		endif
			

	// Anything higher than level 9, cap.
	elseif (magiclevel > 9) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 20% chance of level 3
		elseif (power < 30000)
			n := 3;
		
		// 50% chance of level 4
		elseif (power < 60000)
			n := 4;

		// 20% chance of level 5
		elseif (power < 80000)
			n := 5;			
		
		// 10% chance of level 6
		else
			n := 6;
		endif

	endif
	
return n;

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// ApplyProtection() - This is the core function of determining the level for
//			      immunities/elemental prots. It's based purely on the magiclevel
//
/////////////////////////////////////////////////////////////////////////////////////////////

function ApplyProtection( item )

	var num := DetermineImmunityLevel(item);
	var ptype;
	var charges;
	

	case (num)
		1: charges := 15;
		2: charges := 20;
		3: charges := 25;
		4: charges := 30;
		5: charges := 35;
		6: charges := 40;
	endcase
   	

	case( RandomDiceStr( "1d3" ) )
		1:	ptype := "PoisonImmunity";
			break;
			 
		2:	ptype := "MagicImmunity";
			break;
			 
		3:	ptype := "SpellReflect";
			break;
			 
		default: ptype := "SpellReflect";
	endcase

	SetObjProperty( item , ptype , charges );
	
endfunction


/////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// ApplyImmunity( item )
//
// NOTE: This is for poisonprot/blackrock/moonstone ONLY
//
/////////////////////////////////////////////////////////////////////////////////

function ApplyImmunity( item )

	var itype;
	
	// Get level based on magiclevel
	var n := DetermineImmunityLevel(item, 1);
	
	if(n == 4)
		SetObjProperty(item, "isUncommonProt", 1);
	elseif(n > 4)
		SetObjProperty(item, "isRareProt", 1);
	endif

	case(Random(3)+1)
		1:	itype := "PermPoisonImmunity";
			break;
			
		2:	itype := "PermMagicImmunity";
			break;
			
		default: itype := "Permmr";
	endcase

	SetObjProperty( item , itype , n );
	
endfunction


/////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// ApplyElemental( item )
//
// NOTE: This is for elemental protections only
//
/////////////////////////////////////////////////////////////////////////////////

function ApplyElementalImmunity( item )

	var element;
	var power := DetermineImmunityLevel(item);
	var ran := Random(9)+1;
	case(ran)
		1:	element := "FireProtection";
			break;
		
		2:	element := "WaterProtection";
			break;
		
		3:	element := "AirProtection";
			break;
		
		4:	element := "EarthProtection";
			break;
		
		5:	element := "NecroProtection";
			break;
		
		6:	element := "FreeAction";
			power := 1;
			SetObjProperty(item, "isUncommonProt", 1);
			break;
			
		7:	element := "HealingIncrease";
			if(power == 4)
				SetObjProperty(item, "isUncommonProt", 1);
			elseif(power > 4)
				SetObjProperty(item, "isRareProt", 1);
			endif
			break;
			
		8:	element := "HolyProtection";
			break;
		
		default: element := "PhysicalProtection";
			if(power == 4)
				SetObjProperty(item, "isUncommonProt", 1);
			elseif(power > 4)
				SetObjProperty(item, "isRareProt", 1);
			endif
	endcase
/*  Old system
	// It's a ele protection - they give 25% protection each level
	if (ran < 6 || ran == 8)
		power := power * 25;
		if (power > 149)
			power := 150; // can't go over 150 for these base items!
		endif
	endif
	 */

	// New system to calculate protection levels.
	if (ran < 6 || ran == 8)
		case(power)
		// Level 1 protection (Bane)
		1:	power := 15;
			break;
		
		// Level 2 protection (Warding)
		2:	power := 25;
			break;

		// Level 3 protection (Protection)
		3:	power := 35;
			SetObjProperty(item, "isUncommonProt", 1);
			break;
		
		// Level 4 protection (Immunity)
		4:	power := 45;
			SetObjProperty(item, "isUncommonProt", 1);
			break;
		
		// Level 5 protection (Attunement)
		5:	power := 55;
			SetObjProperty(item, "isRareProt", 1);
			break;
		
		// Level 6 protection (Absorbsion)
		6:	power := 65;
			SetObjProperty(item, "isRareProt", 1);
			break;
		endcase
	endif

	SetObjProperty( item , element , power );
	
endfunction
	

////
//Adds "a magic " to the beginning of the item's desc.
////
function AddName( item )

   	var currdesc := item.desc;
   	if( currdesc[1] == "a" )
      		var space := find( item.desc, " ", 1 );
      		var newname := currdesc[space + 1 , len(item.desc)];

		// gm items glowing
		if( GetObjProperty( item , "IsGMItem" ))
			SetName( item , "a magic " + newname );
		else
			SetName( item , "a magic " + newname );
		endif

   	else
		if( GetObjProperty( item , "IsGMItem" ))
			SetName( item , "magic " + currdesc );
		else
			SetName( item , "magic " + currdesc );
		endif
   	endif
	
	SetObjProperty( item , "UnIDedMagicalItem" , 1 );
	
	if( GetObjProperty( item , "IsGMItem" ))
		return;
	endif

	if( (item.objtype < 0x1eff or item.objtype > 0x1f04) and (Random(100)+1) <= 5 )
		SetObjProperty(item , "Cursed" , 1 );
	endif

endfunction


function AddRandomColor( item )

	SetObjProperty( item , "Undyeable" , 1 );
	if( !GetObjProperty( item , "EnchantColor" ) )
		var color := RandomDiceStr( "1d"+MAX_COLOR );
		while( color > 999 and color < 1152 )
			color := RandomDiceStr( "1d"+MAX_COLOR );
		endwhile
	
		SetObjProperty( item , "EnchantColor" , color );
	endif

endfunction


function CountAvailHitscripts()

	hitscriptcfg := ReadConfigFile( ":combat:hitscriptdesc" );
	var all_keys := GetConfigIntKeys(hitscriptcfg);
	foreach key in all_keys
		var elem := FindConfigElem( hitscriptcfg, key );
		var type := GetConfigString( elem, "HitscriptType");
		
		if (type == "Effect")
			availeffects.append(key);
		elseif (type == "Spell")
			availspells.append(key);
		elseif (type =="Greater")
			availgreaters.append(key);
		endif
	endforeach

endfunction


function CountAvailOnHitscripts()

	onhitcfg 		:= ReadConfigFile( ":combat:onhitscriptdesc" );
	var all_keys := GetConfigIntKeys(onhitcfg);
	foreach key in all_keys
		var elem := FindConfigElem( onhitcfg, key );
		var type := GetConfigString( elem, "OnHitscriptType");
		if (type == "Effect")
			availeffectonhit.append(key);
		elseif (type == "Spell")
			availspellonhit.append(key);
		elseif (type =="Greater")
			availgreateronhit.append(key);
		endif
	endforeach
	
endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// Below 2 function not altered at all.  See how they go.
//
/////////////////////////////////////////////////////////////////////////////////////////////
function SetHitscript( item , n , multiplier )

	set_critical( 1 );
	
	//Apply the hitscript
	hitscriptcfg	:= ReadConfigFile( ":combat:hitscriptdesc" );
	item.hitscript	:= hitscriptcfg[n].Hitscript;
	SetObjProperty( item , "HitscriptNumber" , n );
	
	var CProp := hitscriptcfg[n].CProp;
	if( CProp )
		var CPropVal := hitscriptcfg[n].CPropVal;
		if( !CPropVal )
			var power := (Random(10)+1) * magiclevel;
			CPropVal := Cint(power / 6);
			var mult := onhitcfg[n].Multiplier;
			if( mult )
				CPropVal := Cint(CPropVal * mult);
			endif
			
			if( CProp == "Poisonlvl" )
				CPropVal := CPropVal / 2;
			endif
			
			if( CPropVal < 1 )
				CPropVal := 1;
			endif

			if( CProp == "Poisonlvl" )
				if(CPropVal >= 6)
				CPropVal := 6;
				SetObjProperty(item, "isRare", 1);
				endif
			endif
		endif
		SetObjProperty( item , CProp , CPropVal );
	endif
	
	//Check for another enchantment
	if (hitscriptcfg[n].Place == 2)
		if ( (Random(100)+1) <= (multiplier * magiclevel))
			if ((Random(100)+1) <= 75)
       			ApplyHPMod( item );
			else
				ApplyWeapSkillMod( item );
			endif
		endif

	else
		if ( (Random(100)+1) <= (multiplier * magiclevel))
			var secchance := (Random(100)+1);
			if (secchance < 30)
       			ApplyHPMod( item );
			elseif (secchance < 60)
				ApplyWeapSkillMod( item );
			else
				ApplyDmgMod( item );
			endif
		endif
	endif
	
	set_critical( 0 );
	
endfunction


function SetOnHitscript( item , n , multiplier )

	set_critical( 1 );
	
	//Apply the hitscript
	onhitcfg := ReadConfigFile( ":combat:onhitscriptdesc" );
	SetObjProperty( item , "OnHitScript" , onhitcfg[n].OnHitscript);
	SetObjProperty( item , "OnHitscriptNumber" , n );
	
	var CProp := onhitcfg[n].CProp;
	if( CProp )
		var CPropVal := onhitcfg[n].CPropVal;
		if( !CPropVal )
			var power := (Random(10)+1) * magiclevel;
			CPropVal := Cint(power / 6);
			var mult := onhitcfg[n].Multiplier;
			if( mult )
				CPropVal := Cint(CPropVal * mult);
			endif
			
			if( CProp == "Poisonlvl" )
				CPropVal := CPropVal - 3;
			endif
			
			if( CPropVal < 1 )
				CPropVal := 1;
			endif
			
			if( CProp == "Poisonlvl" )
				if(CPropVal >= 6)
				CPropVal := 6;
				SetObjProperty(item, "isRare", 1);
				endif
			endif
		endif
		SetObjProperty( item , CProp , CPropVal );
	endif
	
	//Check for another enchantment
	if (onhitcfg[n].Place == 2)
		if ( (Random(100)+1) <= (multiplier * magiclevel))
			if ((Random(100)+1) <= 75)
       			ApplyHPMod( item );
			else
				ApplyArSkillMod( item );
			endif
		endif

	else
		if ( (Random(100)+1) <= (multiplier * magiclevel))
			var secchance := Random(100)+1;
			if (secchance < 60)
       			ApplyHPMod( item );
			elseif (secchance < 90)
				ApplyArSkillMod( item );
			else
				ApplyARMod( item );
			endif
		endif
	endif
	
	set_critical( 0 );

endfunction                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            