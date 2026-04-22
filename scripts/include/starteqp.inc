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
var creating_starter_all_group := 0;

set_script_option( SCRIPTOPT_NO_RUNAWAY, 1 );

//Touched by Dev Alryc 04-11-2026
function MakeLoot( corpse , lootgroup := 0, magic_chance_default := 0, magic_level_default := 0 )

	UnloadConfigFile( "::nlootgroup" );
	starteqp_cfg := ReadConfigFile( "::nlootgroup" );
	
	if( lootgroup )

		//This fires if it isnt a death related event.
		//In death.src it is just MakeLoot( corpse ) and lootgroup is set to 0, so it goes to the 'else' part of this function.


		//Treasure chests for Tmaps
		//Seems to be set in digtreasure.src, so commenting out for now
		// if ( (lootgroup >= 5) and (lootgroup <= 10) )
		// 	magicspawnchance := lootgroup * 9;
		// 	magiclevel := lootgroup - 3;
		// 	basechance := magicspawnchance;
		// 	baselevel  := magiclevel;
		// endif

		//Not sure where this is called as it not defined atm in nlootgroup, commenting out for now
		// if ( (lootgroup == 100))
		// 	magicspawnchance := (lootgroup - 91 ) * 10;
		// 	magiclevel := lootgroup - 91;
		// 	basechance := magicspawnchance;
		// 	baselevel  := magiclevel;
		// endif

		//Not sure what uses this, commenting out for now
		// if ( (lootgroup >= 101) and (lootgroup <= 105) )
		// 	Print("In lootgroup 101-105 for MakeLoot");
		// 	magicspawnchance := (lootgroup - 96 ) * 10;
		// 	magiclevel := (lootgroup - 96);
		// 	basechance := magicspawnchance;
		// 	baselevel  := magiclevel;
		// endif

		//No longer used, commenting out
		//spawnchests are lootgroups 109 through 119
		// if ( (lootgroup >= 109) and (lootgroup <= 119) )
		// 	magicspawnchance := lootgroup - 75;
		// 	magiclevel := Cint((lootgroup - 109) / 3); 
		// 	basechance := magicspawnchance;
		// 	baselevel  := magiclevel;
		// endif
		
		if( magic_chance_default )
			//Print("In magic_chance_default for MakeLoot");
			magicspawnchance := magic_chance_default;
			basechance := magicspawnchance;
		endif
		
		if( magic_level_default )
			//Print("In magic_level_default for MakeLoot");
			magiclevel := magic_level_default; 
			baselevel  := magiclevel;
		endif

		CreateFromGroup( corpse , Cint(lootgroup) );
		
	else
		//Print("In corpse loot for MakeLoot");
		var npcdesc := ReadConfigFile( "::npcdesc" );
		var elem := FindConfigElem( npcdesc, GetObjProperty( corpse, "npctemplate" ) );
		var mprop;
		var mlevel;
		var lootgroups;

		// Custom Loot, best be carefull with this, don't want bad people changing loot levels/
		// * added a check in customnpc script so only admin+ can set custom loot
		if (GetObjProperty(corpse, "CustomLoot"))
			var cloot := GetObjProperty(corpse, "CustomLoot");
			mprop  := CInt(cloot[1]);
			mlevel := CInt(cloot[2]);
			lootgroups := {cloot[3]};
		elseif( elem )
			mprop  := GetConfigInt( elem, "Magicitemchance");
			mlevel := GetConfigInt( elem, "Magicitemlevel");
			lootgroups := GetConfigStringArray( elem, "lootgroup" );
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
        	foreach str in lootgroups
            		CreateFromGroup( corpse , cint(str) );
        	endforeach
	endif
	
endfunction

//Touched by Dev Alryc 04-11-2026
function CreateStartingEquipment( who, skillids )

    	starteqp_cfg := ReadConfigFile( "::starteqp" );

    	foreach skillid in skillids
        	var skillname := GetSkillName( CInt(skillid) );
        	CreateFromGroup( who.backpack, skillname );
    	endforeach

		creating_starter_all_group := 1;
		CreateFromGroup(who.backpack, "all");
		creating_starter_all_group := 0;

endfunction

//Touched by Dev Alryc 04-11-2026
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

		//Commenting out as this is a recursive call to this function, and there is no 'Group' entry in the config file for any group, so it serves no purpose.
    	// foreach str in GetConfigStringArray( elem, "Group" )
        // CreateFromGroup( who, str );
    	// endforeach

endfunction

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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
			if (creating_starter_all_group && lower(objname) == "dagger")
				item.newbie := 1;
			endif
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

//Touched by Dev Alryc 04-11-2026
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyWeaponModNew() - Chooses what to put on a weapon
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
function ApplyWeaponModNew(who, item)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// ApplyHPMod - adds a HP mod to the item - Least Desirable
	// ApplyWeapSkillMod - adds a skill mod to the item - Semi Desirable
	// ApplyDmgMod - adds a damage mod to the item - Most Desirable
	// ApplyWeapHitscript - adds a hitscript mod to the item - Most Desirable
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		// 89.3% chance
		elseif (chance < 9000)
			ApplyDmgMod( item );
		// 10% chance
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
		// 84.7% chance
		elseif (chance < 8500)
			ApplyDmgMod( item );
		// 15% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 85% chance
		if (chance < 8500)
			ApplyDmgMod( item );
		// 15% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 82% chance
		if (chance < 8200)
			ApplyDmgMod( item );
		// 18% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 79% chance
		if (chance < 7900)
			ApplyDmgMod( item );
		// 21% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 76% chance
		if (chance < 7600)
			ApplyDmgMod( item );
		// 24% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 70% chance
		if (chance < 7000)
			ApplyDmgMod( item );
		// 30% chance
		else
			ApplyWeapHitscript( who , item );
		endif

	// Level ?? Loot	
	else

		// 50% chance
		if (chance < 5000)
			ApplyDmgMod( item );
		// 50% chance
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

//Touched by Dev Alryc 04-11-2026
function ApplyArmourModNew( unused who, item )

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
			ApplyOnHitScript( item );
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
			ApplyOnHitScript( item );
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
			ApplyOnHitScript( item );
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
			ApplyOnHitScript( item );
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// .5% chance 
		if (chance < 50)
			ApplyHPMod( item );
		// 1.5% chance
		elseif (chance < 200)
			ApplyArSkillMod( item ); 
		// 68% chance
		elseif (chance < 7000)
			ApplyARMod( item );
		// 30% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// .1% chance 
		if (chance < 10)
			ApplyHPMod( item );
		// 1% chance
		elseif (chance < 110)
			ApplyArSkillMod( item ); 
		// 80% chance
		elseif (chance < 8110)
			ApplyARMod( item );
		// 18.9% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 78% chance
		if (chance < 7800)
			ApplyARMod( item );
		// 22% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 76% chance
		if (chance < 7600)
			ApplyARMod( item );
		// 24% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		// 74% chance
		if (chance < 7400)
			ApplyARMod( item );
		// 26% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		// 72% chance
		if (chance < 7200)
			ApplyARMod( item );
		// 28% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		// 70% chance
		if (chance < 7000)
			ApplyARMod( item );
		// 30% chance
		else
			ApplyOnHitScript( item );
		endif

	// Level ?? Loot
	else

			// 68% chance
			if (chance < 6800)
				ApplyARMod( item );
			// 32% chance
			else
				ApplyOnHitScript( item );
			endif
			
		endif

	AddName( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyShieldModNew() - Chooses what to put on a shield
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
function ApplyShieldModNew(unused who, item)

	// No need for anything higher
	var chance := RandomInt(10000);

	if (magiclevel == 0)
		return 0;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 70% chance 
		if (chance < 7000)
			ShieldApplyHPMod( item );
		// 20% chance
		elseif (chance < 9000)
			ShieldApplyArSkillMod( item ); 
		// 10% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 60% chance 
		if (chance < 6000)
			ShieldApplyHPMod( item );
		// 20% chance
		elseif (chance < 8000)
			ShieldApplyArSkillMod( item ); 
		// 20% chance
		else 
			ShieldApplyARMod( item );
		endif


	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 50% chance 
		if (chance < 5000)
			ShieldApplyHPMod( item );
		// 25% chance
		elseif (chance < 7500)
			ShieldApplyArSkillMod( item ); 
		// 25% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 40% chance 
		if (chance < 4000)
			ShieldApplyHPMod( item );
		// 30% chance
		elseif (chance < 7000)
			ShieldApplyArSkillMod( item ); 
		// 30% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 30% chance 
		if (chance < 3000)
			ShieldApplyHPMod( item );
		// 35% chance
		elseif (chance < 6500)
			ShieldApplyArSkillMod( item ); 
		// 35% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 20% chance 
		if (chance < 2000)
			ShieldApplyHPMod( item );
		// 40% chance
		elseif (chance < 6000)
			ShieldApplyArSkillMod( item ); 
		// 40% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 10% chance 
		if (chance < 1000)
			ShieldApplyHPMod( item );
		// 45% chance
		elseif (chance < 5500)
			ShieldApplyArSkillMod( item ); 
		// 45% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 10% chance 
		if (chance < 1000)
			ShieldApplyHPMod( item );
		// 40% chance
		elseif (chance < 5000)
			ShieldApplyArSkillMod( item ); 
		// 50% chance
		else 
			ShieldApplyARMod( item );
		endif	

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 20% chance 
		if (chance < 2000)
			ShieldApplyHPMod( item );
		// 25% chance
		elseif (chance < 4500)
			ShieldApplyArSkillMod( item ); 
		// 55% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 10% chance 
		if (chance < 1000)
			ShieldApplyHPMod( item );
		// 30% chance
		elseif (chance < 4000)
			ShieldApplyArSkillMod( item ); 
		// 60% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance 
		if (chance < 0)
			ShieldApplyHPMod( item );
		// 30% chance
		elseif (chance < 3000)
			ShieldApplyArSkillMod( item ); 
		// 70% chance
		else 
			ShieldApplyARMod( item );
		endif

	// Level ?? loot
	else 
		// 100% chance
		ShieldApplyARMod( item );

	endif

	AddName( item );

endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ApplyClothingModNew() - Chooses what to put on clothing
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
function ApplyClothingModNew( unused who, item )

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
		
		// 45% chance 
		if (chance < 4500)
			ApplyMiscSkillMod( item );
		// 55% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 40% chance 
		if (chance < 4000)
			ApplyMiscSkillMod( item );
		// 60% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 35% chance 
		if (chance < 3500)
			ApplyMiscSkillMod( item );
		// 65% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level ?? loot
	else 
		// 30% chance 
		if (chance < 3000)
			ApplyMiscSkillMod( item );
		// 70% chance
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
function ApplyMiscModNew(who, item)

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
		
		// 45% chance 
		if (chance < 4500)
			ApplyMiscSkillMod( item );
		// 45% chance
		elseif (chance < 9000)
			ApplyEnchant( who , item );
		// 10% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 40% chance 
		if (chance < 4000)
			ApplyMiscSkillMod( item );
		// 40% chance
		elseif (chance < 8000)
			ApplyEnchant( who , item );
		// 20% chance
		else 
			ApplyMiscArMod( item );
		endif

	// Level ?? loot
	else 

		// 37.5% chance 
		if (chance < 3750)
			ApplyMiscSkillMod( item );
		// 37.5% chance
		elseif (chance < 7500)
			ApplyEnchant( who , item );
		// 25% chance
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Checks to see if that item is allowed to be magic and returns the type
//			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
function ApplyDmgMod( item )

	var power := RandomInt(100000);
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;

	// Level 1 loot
	elseif (magiclevel == 1)

		// 98% chance
		if (power < 98000)
			n := 1;

		// 2% chance
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 1;

		// 17.5% chance
		elseif (power < 99500)
			n := 2;

		// 0.5% chance
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 1;

		// 60% chance
		elseif (power < 90000)
			n := 2;

		// 10% chance
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 1;

		// 45% chance
		elseif (power < 64997)
			n := 2;

		// 30% chance
		elseif (power < 94997)
			n := 3;
		
		// 5% chance
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 1;

		// 30% chance
		elseif (power < 44999)
			n := 2;

		// 46% chance
		elseif (power < 90999)
			n := 3;
		
		// 9% chance
		else
			n := 4;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 1;

		// 15% chance
		elseif (power < 18000)
			n := 2;

		// 30% chance
		elseif (power < 48000)
			n := 3;
		
		// 35% chance
		elseif (power < 83000)
			n := 4;

		// 14% chance
		elseif (power < 97000)
			n := 5;
		// 3% chance
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 38% chance
		elseif (power < 38000)
			n := 3;
		
		// 40% chance
		elseif (power < 78000)
			n := 4;

		// 18% chance
		elseif (power < 96000)
			n := 5;
		// 4% chance
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 18% chance
		elseif (power < 18000)
			n := 3;
		
		// 55% chance
		elseif (power < 73000)
			n := 4;

		// 17% chance
		elseif (power < 90000)
			n := 5;

		// 10% chance
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 15% chance
		elseif (power < 15000)
			n := 3;
		
		// 50% chance
		elseif (power < 65000)
			n := 4;

		// 20% chance
		elseif (power < 85000)
			n := 5;

		// 15% chance
		else
			n := 6;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 10% chance
		elseif (power < 10000)
			n := 3;
		
		// 45% chance
		elseif (power < 55000)
			n := 4;

		// 25% chance
		elseif (power < 80000)
			n := 5;

		// 20% chance
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 40% chance
		elseif (power < 30000)
			n := 5;

		// 40% chance
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 20% chance
		elseif (power < 40000)
			n := 5;

		// 60% chance
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
	
	// Equal per-level scaling: +1/11 per level, level 11 is 100%.
   	if( (RandomInt(11)+1) <= magiclevel )
       	if ((Random(100)+1) <= 75) // 75% chance of adding a hp mod
       		ApplyHPMod( item );
		else
			ApplyWeapSkillMod( item ); // 25% chance of adding a skill mod
		endif
   	endif

endfunction

//Touched by Dev Alryc 04-11-2026
function ApplyHPMod( item )

	var power := RandomInt(100000);
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;

	// Level 1 loot
	elseif (magiclevel == 1)

		// 98% chance
		if (power < 98000)
			n := 1;

		// 2% chance
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 1;

		// 17.5% chance
		elseif (power < 99500)
			n := 2;

		// 0.5% chance
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 1;

		// 60% chance
		elseif (power < 90000)
			n := 2;

		// 10% chance
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 1;

		// 45% chance
		elseif (power < 64997)
			n := 2;

		// 30% chance
		elseif (power < 94997)
			n := 3;
		
		// 5% chance
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 1;

		// 30% chance
		elseif (power < 44999)
			n := 2;

		// 46% chance
		elseif (power < 90999)
			n := 3;
		
		// 9% chance
		else
			n := 4;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 1;

		// 15% chance
		elseif (power < 18000)
			n := 2;

		// 30% chance
		elseif (power < 48000)
			n := 3;
		
		// 35% chance
		elseif (power < 83000)
			n := 4;

		// 14% chance
		elseif (power < 97000)
			n := 5;
		// 3% chance
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 38% chance
		elseif (power < 38000)
			n := 3;
		
		// 40% chance
		elseif (power < 78000)
			n := 4;

		// 18% chance
		elseif (power < 96000)
			n := 5;
		// 4% chance
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 18% chance
		elseif (power < 18000)
			n := 3;
		
		// 55% chance
		elseif (power < 73000)
			n := 4;

		// 17% chance
		elseif (power < 90000)
			n := 5;

		// 10% chance
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 15% chance
		elseif (power < 15000)
			n := 3;
		
		// 50% chance
		elseif (power < 65000)
			n := 4;

		// 20% chance
		elseif (power < 85000)
			n := 5;

		// 15% chance
		else
			n := 6;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 10% chance
		elseif (power < 10000)
			n := 3;
		
		// 45% chance
		elseif (power < 55000)
			n := 4;

		// 25% chance
		elseif (power < 80000)
			n := 5;

		// 20% chance
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 40% chance
		elseif (power < 30000)
			n := 5;

		// 40% chance
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 20% chance
		elseif (power < 40000)
			n := 5;

		// 60% chance
		else
			n := 6;
		endif
	endif

   	item.maxhp_mod := CInt(n*5);
   	item.hp := item.maxhp;

endfunction

//Touched by Dev Alryc 04-11-2026
function ApplyARMod( item )

	var power := RandomInt(100000);
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;

	// Level 1 loot
	elseif (magiclevel == 1)

		// 98% chance
		if (power < 98000)
			n := 1;

		// 2% chance
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 1;

		// 17.5% chance
		elseif (power < 99500)
			n := 2;

		// 0.5% chance
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 1;

		// 60% chance
		elseif (power < 90000)
			n := 2;

		// 10% chance
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 1;

		// 45% chance
		elseif (power < 64997)
			n := 2;

		// 30% chance
		elseif (power < 94997)
			n := 3;
		
		// 5% chance
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 1;

		// 30% chance
		elseif (power < 44999)
			n := 2;

		// 46% chance
		elseif (power < 90999)
			n := 3;
		
		// 9% chance
		else
			n := 4;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 1;

		// 15% chance
		elseif (power < 18000)
			n := 2;

		// 30% chance
		elseif (power < 48000)
			n := 3;
		
		// 35% chance
		elseif (power < 83000)
			n := 4;

		// 14% chance
		elseif (power < 97000)
			n := 5;
		// 3% chance
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 38% chance
		elseif (power < 38000)
			n := 3;
		
		// 40% chance
		elseif (power < 78000)
			n := 4;

		// 18% chance
		elseif (power < 96000)
			n := 5;
		// 4% chance
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 18% chance
		elseif (power < 18000)
			n := 3;
		
		// 55% chance
		elseif (power < 73000)
			n := 4;

		// 17% chance
		elseif (power < 90000)
			n := 5;

		// 10% chance
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 15% chance
		elseif (power < 15000)
			n := 3;
		
		// 50% chance
		elseif (power < 65000)
			n := 4;

		// 20% chance
		elseif (power < 85000)
			n := 5;

		// 15% chance
		else
			n := 6;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 10% chance
		elseif (power < 10000)
			n := 3;
		
		// 45% chance
		elseif (power < 55000)
			n := 4;

		// 25% chance
		elseif (power < 80000)
			n := 5;

		// 20% chance
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 40% chance
		elseif (power < 30000)
			n := 5;

		// 40% chance
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 20% chance
		elseif (power < 40000)
			n := 5;

		// 60% chance
		else
			n := 6;
		endif
	endif
		
	item.ar_mod := CInt(n);

	//Check for another enchantment
	// Equal per-level scaling: +1/11 per level, level 11 is 100%.
   	if( (RandomInt(11)+1) <= magiclevel )
       	if ((Random(100)+1) <= 75) // 75% of adding a hp mod
       		ApplyHPMod( item );
		else
			ApplyArSkillMod( item ); // 25% chance of adding a ar skill mod
		endif
   	endif

endfunction

//Touched by Dev Alryc 04-11-2026
function ApplyMiscArMod( item )

	var power := RandomInt(100000);
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;

	// Level 1 loot
	elseif (magiclevel == 1)

		// 98% chance
		if (power < 98000)
			n := 1;

		// 2% chance
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 1;

		// 17.5% chance
		elseif (power < 99500)
			n := 2;

		// 0.5% chance
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 1;

		// 60% chance
		elseif (power < 90000)
			n := 2;

		// 10% chance
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 1;

		// 45% chance
		elseif (power < 64997)
			n := 2;

		// 30% chance
		elseif (power < 94997)
			n := 3;
		
		// 5% chance
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 1;

		// 30% chance
		elseif (power < 44999)
			n := 2;

		// 46% chance
		elseif (power < 90999)
			n := 3;
		
		// 9% chance
		else
			n := 4;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 1;

		// 15% chance
		elseif (power < 18000)
			n := 2;

		// 30% chance
		elseif (power < 48000)
			n := 3;
		
		// 35% chance
		elseif (power < 83000)
			n := 4;

		// 14% chance
		elseif (power < 97000)
			n := 5;
		// 3% chance
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 38% chance
		elseif (power < 38000)
			n := 3;
		
		// 40% chance
		elseif (power < 78000)
			n := 4;

		// 18% chance
		elseif (power < 96000)
			n := 5;
		// 4% chance
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 18% chance
		elseif (power < 18000)
			n := 3;
		
		// 55% chance
		elseif (power < 73000)
			n := 4;

		// 17% chance
		elseif (power < 90000)
			n := 5;

		// 10% chance
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 15% chance
		elseif (power < 15000)
			n := 3;
		
		// 50% chance
		elseif (power < 65000)
			n := 4;

		// 20% chance
		elseif (power < 85000)
			n := 5;

		// 15% chance
		else
			n := 6;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 10% chance
		elseif (power < 10000)
			n := 3;
		
		// 45% chance
		elseif (power < 55000)
			n := 4;

		// 25% chance
		elseif (power < 80000)
			n := 5;

		// 20% chance
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 40% chance
		elseif (power < 30000)
			n := 5;

		// 40% chance
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance
		if (power < 0)
			n := 1;

		// 0% chance
		elseif (power < 0)
			n := 2;

		// 0% chance
		elseif (power < 0)
			n := 3;
		
		// 20% chance
		elseif (power < 20000)
			n := 4;

		// 20% chance
		elseif (power < 40000)
			n := 5;

		// 60% chance
		else
			n := 6;
		endif
	endif

	n := CInt(n);

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

//Touched by Dev Alryc 04-11-2026
function ApplyWeapHitscript( unused who , item )

	CountAvailHitscripts();
	var power := RandomInt(100000);
	// 1% == 1000

 	if (magiclevel <= 0)
			ApplySpellHitscript( item ); // Spell on hit

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 70% chance of spell
		if (power < 70000)
			ApplySpellHitscript( item ); // Spell on hit

		// 30% chance slayer
		else
			ApplySlayerHitscript( item ); // slayer weapons
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 60% chance of spell
		if (power < 60000)
			ApplySpellHitscript( item ); // Spell on hit

		// 40% chance slayer
		else
			ApplySlayerHitscript( item ); // slayer weapons
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 48% chance of spell
		if (power < 48000)
			ApplySpellHitscript( item ); // Spell on hit

		// 48% chance slayer
		elseif (power < 96000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 3.9% chance effect
		elseif (power < 99900)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.1% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 46% chance of spell
		if (power < 46000)
			ApplySpellHitscript( item ); // Spell on hit

		// 46% chance slayer
		elseif (power < 92000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 7.9% chance effect
		elseif (power < 99900)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.1% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 44% chance of spell
		if (power < 44000)
			ApplySpellHitscript( item ); // Spell on hit

		// 44% chance slayer
		elseif (power < 88000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 11.5% chance effect
		elseif (power < 99500)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.5% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 42% chance of spell
		if (power < 42000)
			ApplySpellHitscript( item ); // Spell on hit

		// 42% chance slayer
		elseif (power < 84000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 15.5% chance effect
		elseif (power < 99500)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.5% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellHitscript( item ); // Spell on hit

		// 40% chance slayer
		elseif (power < 80000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 19.2% chance effect
		elseif (power < 99200)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.8% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 37% chance of spell
		if (power < 37000) 
			ApplySpellHitscript( item ); // Spell on hit
		
		// 37% chance slayer
		elseif (power < 74000) 
			ApplySlayerHitscript( item ); // slayer weapons
		
		// 25.2% chance effect
		elseif (power < 99200)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 0.8% chance greater
		else
			ApplyGreaterHitscript( item ); //brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 35% chance of spell
		if (power < 35000)
			ApplySpellHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 70000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 29% chance effect
		elseif (power < 99000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 1% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellHitscript( item ); // Spell on hit

		// 30% chance slayer
		elseif (power < 60000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 37% chance effect
		elseif (power < 97000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 3% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 25% chance of spell
		if (power < 25000)
			ApplySpellHitscript( item ); // Spell on hit

		// 25% chance slayer
		elseif (power < 50000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 25% chance effect
		elseif (power < 75000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 25% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif

	// Level ?? loot
	else
		
		// 25% chance of spell
		if (power < 25000)
			ApplySpellHitscript( item ); // Spell on hit

		// 25% chance slayer
		elseif (power < 50000)
			ApplySlayerHitscript( item ); // slayer weapons

		// 25% chance effect
		elseif (power < 75000)
			ApplyEffectHitscript( item ); // blinding/stam,mana, life drain/pzn/banish/piercing
			SetObjProperty(item, "isRare", 1);

		// 25% chance greater
		else
			ApplyGreaterHitscript( item ); // brock, void, ele fury
			SetObjProperty(item, "isRare", 1);
		endif	
		
	endif

endfunction

//Touched by Dev Alryc 04-11-2026
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
	var rand := RandomInt(100000);

	//1000 == 1%
	if (magiclevel == 0)
		ascircle := 1;
	
	// Level 1 loot
	elseif (magiclevel == 1)
		
		//98% 1st circle
		if (rand < 98000)
			ascircle := 1;
	
		//2% 2nd circle
		else

			ascircle := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		//82% 1st circle
		if (rand < 82000)
			ascircle := 1;
	
		//17.5% 2nd circle
		elseif (rand < 99500)
			ascircle := 2;

		//0.5% 3rd circle
		else
			ascircle := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		//30% 1st circle
		if (rand < 30000)
			ascircle := 1;
	
		//60% 2nd circle
		elseif (rand < 90000)
			ascircle := 2;

		//10% 3rd circle
		else
			ascircle := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		//20% 1st circle
		if (rand < 20000)
			ascircle := 1;
	
		//45% 2nd circle
		elseif (rand < 64997)
			ascircle := 2;

		//30% 3rd circle
		elseif (rand < 94997)
			ascircle := 3;

		//5% 4th circle
		else
			ascircle := 4;
		endif
		
	// Level 5 loot
	elseif (magiclevel == 5)
		
		//15% 1st circle
		if (rand < 15000)
			ascircle := 1;
	
		//30% 2nd circle
		elseif (rand < 44999)
			ascircle := 2;

		//46% 3rd circle
		elseif (rand < 90999)
			ascircle := 3;

		//9% 4th circle
		else
			ascircle := 4;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		//1% 1st circle
		if (rand < 1000)
			ascircle := 1;
	
		//15% 2nd circle
		elseif (rand < 16000)
			ascircle := 2;

		//30% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//28% 4th circle
		elseif (rand < 74000)
			ascircle := 4;

		//20% 5th circle
		elseif (rand < 94000)
			ascircle := 5;

		//6% 6th circle
		else
			ascircle := 6;
		endif
	
	// Level 7 loot
	elseif (magiclevel == 7)
		
		//1% 1st circle
		if (rand < 1000)
			ascircle := 1;
	
		//15% 2nd circle
		elseif (rand < 16000)
			ascircle := 2;

		//30% 3rd circle
		elseif (rand < 46000)
			ascircle := 3;

		//28% 4th circle
		elseif (rand < 74000)
			ascircle := 4;

		//20% 5th circle
		elseif (rand < 94000)
			ascircle := 5;

		//6% 6th circle
		else
			ascircle := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//61% 3rd circle
		elseif (rand < 61000)
			ascircle := 3;

		//9% 4th circle
		elseif (rand < 70000)
			ascircle := 4;

		//15% 5th circle
		elseif (rand < 85000)
			ascircle := 5;

		//10% 6th circle
		elseif (rand < 95000)
			ascircle := 6;

		//5% 7th circle
		else
			ascircle := 7;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//26% 3rd circle
		elseif (rand < 26000)
			ascircle := 3;

		//35% 4th circle
		elseif (rand < 61000)
			ascircle := 4;

		//9% 5th circle
		elseif (rand < 70000)
			ascircle := 5;

		//15% 6th circle
		elseif (rand < 85000)
			ascircle := 6;

		//10% 7th circle
		elseif (rand < 95000)
			ascircle := 7;

		//5% 8th circle
		else
			ascircle := 8;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 20000)
			ascircle := 3;

		//35% 4th circle
		elseif (rand < 55000)
			ascircle := 4;

		//10% 5th circle
		elseif (rand < 65000)
			ascircle := 5;

		//15% 6th circle
		elseif (rand < 80000)
			ascircle := 6;

		//11% 7th circle
		elseif (rand < 91000)
			ascircle := 7;

		//3% 8th circle
		elseif (rand < 94000)
			ascircle := 8;

		//3% 9th circle
		elseif (rand < 97000)
			ascircle := 9;

		//3% 10th circle
		else 
			ascircle := 10;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 0)
			ascircle := 3;

		//35% 4th circle
		elseif (rand < 0)
			ascircle := 4;

		//25% 5th circle
		elseif (rand < 25000)
			ascircle := 5;

		//30% 6th circle
		elseif (rand < 55000)
			ascircle := 6;

		//30% 7th circle
		elseif (rand < 85000)
			ascircle := 7;

		//5% 8th circle
		elseif (rand < 90000)
			ascircle := 8;

		//5% 9th circle
		elseif (rand < 95000)
			ascircle := 9;

		//5% 10th circle
		else 
			ascircle := 10;
		endif

	// Level 11+ loot
	else

		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//20% 3rd circle
		elseif (rand < 0)
			ascircle := 3;

		//35% 4th circle
		elseif (rand < 0)
			ascircle := 4;

		//25% 5th circle
		elseif (rand < 25000)
			ascircle := 5;

		//30% 6th circle
		elseif (rand < 55000)
			ascircle := 6;

		//30% 7th circle
		elseif (rand < 85000)
			ascircle := 7;

		//5% 8th circle
		elseif (rand < 90000)
			ascircle := 8;

		//5% 9th circle
		elseif (rand < 95000)
			ascircle := 9;

		//5% 10th circle
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
	//elseif (chanceofeffect > 80)
	//	chanceofeffect := 80;
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

//Touched by Dev Alryc 04-11-2026
function ApplySlayerHitscript( item )

	set_critical( 1 );
	
	var multiplier := 15;
	var newRoll := magiclevel + 1;
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
function ApplyWeapSkillMod( item )

	var chance := Random(1000)+1;
	if(chance <= 40)
		ApplyStatMod( item );
		SetObjProperty(item, "isRare", 1);
		return;
	endif

	var amount;

	var power := RandomInt(100000);
	var n;

	if (magiclevel <= 0)
			n := 5;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 98% chance
		if (power < 98000)
			n := 5;

		// 2% chance
		else
			n := 10;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 5;

		// 17.5% chance
		elseif (power < 99500)
			n := 10;

		// 0.5% chance
		else
			n := 15;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 5;

		// 60% chance
		elseif (power < 90000)
			n := 10;

		// 10% chance
		else
			n := 15;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 5;

		// 45% chance
		elseif (power < 65000)
			n := 10;

		// 30% chance
		elseif (power < 95000)
			n := 15;
		
		// 5% chance
		else
			n := 20;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 5;

		// 30% chance
		elseif (power < 45000)
			n := 10;

		// 46% chance
		elseif (power < 91000)
			n := 15;
		
		// 9% chance
		else
			n := 20;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 5;

		// 15% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 2% chance
		if (power < 2000)
			n := 5;

		// 16% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 38% chance
		elseif (power < 38000)
			n := 15;
		
		// 30% chance
		elseif (power < 68000)
			n := 20;

		// 22% chance
		elseif (power < 90000)
			n := 25;

		// 10% chance
		else
			n := 30;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 25% chance
		elseif (power < 25000)
			n := 15;
		
		// 38% chance
		elseif (power < 63000)
			n := 20;

		// 25% chance
		elseif (power < 88000)
			n := 25;

		// 12% chance
		else
			n := 30;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 15% chance
		elseif (power < 15000)
			n := 15;
		
		// 40% chance
		elseif (power < 55000)
			n := 20;

		// 30% chance
		elseif (power < 85000)
			n := 25;

		// 15% chance
		else
			n := 30;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 10% chance
		elseif (power < 10000)
			n := 15;
		
		// 35% chance
		elseif (power < 45000)
			n := 20;

		// 35% chance
		elseif (power < 80000)
			n := 25;

		// 20% chance
		else
			n := 30;
		endif

	// Level ?? loot
	else
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 0% chance
		elseif (power < 0)
			n := 15;
		
		// 30% chance
		elseif (power < 30000)
			n := 20;

		// 45% chance
		elseif (power < 75000)
			n := 25;

		// 25% chance
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

//Touched by Dev Alryc 04-11-2026
function ApplyArSkillMod( item )

	var chance := Random(1000)+1;
	if(chance <= 20)
		ApplyStatMod( item );
		return;
	endif

	var skillid;
	var amount;
	var power := RandomInt(100000);
	var n;

	if (magiclevel <= 0)
			n := 5;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 98% chance
		if (power < 98000)
			n := 5;

		// 2% chance
		else
			n := 10;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 5;

		// 17.5% chance
		elseif (power < 99500)
			n := 10;

		// 0.5% chance
		else
			n := 15;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 5;

		// 60% chance
		elseif (power < 90000)
			n := 10;

		// 10% chance
		else
			n := 15;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 5;

		// 44.997% chance
		elseif (power < 64997)
			n := 10;

		// 30% chance
		elseif (power < 94997)
			n := 15;
		
		// 5.003% chance
		else
			n := 20;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 5;

		// 29.999% chance
		elseif (power < 44999)
			n := 10;

		// 46% chance
		elseif (power < 90999)
			n := 15;
		
		// 9.001% chance
		else
			n := 20;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 5;

		// 15% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 3% chance
		if (power < 3000)
			n := 5;

		// 15% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 38% chance
		elseif (power < 38000)
			n := 15;
		
		// 30% chance
		elseif (power < 68000)
			n := 20;

		// 22% chance
		elseif (power < 90000)
			n := 25;

		// 10% chance
		else
			n := 30;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 28% chance
		elseif (power < 28000)
			n := 15;
		
		// 35% chance
		elseif (power < 63000)
			n := 20;

		// 22% chance
		elseif (power < 85000)
			n := 25;

		// 15% chance
		else
			n := 30;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 15% chance
		elseif (power < 15000)
			n := 15;
		
		// 35% chance
		elseif (power < 50000)
			n := 20;

		// 30% chance
		elseif (power < 80000)
			n := 25;

		// 20% chance
		else
			n := 30;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 10% chance
		elseif (power < 10000)
			n := 15;
		
		// 35% chance
		elseif (power < 45000)
			n := 20;

		// 30% chance
		elseif (power < 75000)
			n := 25;

		// 25% chance
		else
			n := 30;
		endif

	// Level 11+ loot
	else
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 10% chance
		elseif (power < 10000)
			n := 15;
		
		// 30% chance
		elseif (power < 40000)
			n := 20;

		// 30% chance
		elseif (power < 70000)
			n := 25;

		// 30% chance
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

//Touched by Dev Alryc 04-11-2026
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
	var power := RandomInt(100000);
	var n;

	if (magiclevel <= 0)
			n := 5;

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 98% chance
		if (power < 98000)
			n := 5;

		// 2% chance
		else
			n := 10;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 82% chance
		if (power < 82000)
			n := 5;

		// 17.5% chance
		elseif (power < 99500)
			n := 10;

		// 0.5% chance
		else
			n := 15;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 30% chance
		if (power < 30000)
			n := 5;

		// 60% chance
		elseif (power < 90000)
			n := 10;

		// 10% chance
		else
			n := 15;
				
		endif

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 20% chance
		if (power < 20000)
			n := 5;

		// 44.997% chance
		elseif (power < 64997)
			n := 10;

		// 30% chance
		elseif (power < 94997)
			n := 15;
		
		// 5.003% chance
		else
			n := 20;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 15% chance
		if (power < 15000)
			n := 5;

		// 29.999% chance
		elseif (power < 44999)
			n := 10;

		// 46% chance
		elseif (power < 90999)
			n := 15;
		
		// 9.001% chance
		else
			n := 20;
		endif

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 3% chance
		if (power < 3000)
			n := 5;

		// 15% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 3% chance
		if (power < 3000)
			n := 5;

		// 15% chance
		elseif (power < 18000)
			n := 10;

		// 30% chance
		elseif (power < 48000)
			n := 15;
		
		// 29% chance
		elseif (power < 77000)
			n := 20;

		// 20% chance
		elseif (power < 97000)
			n := 25;

		// 3% chance
		else
			n := 30;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 36% chance
		elseif (power < 36000)
			n := 15;
		
		// 30% chance
		elseif (power < 66000)
			n := 20;

		// 24% chance
		elseif (power < 90000)
			n := 25;

		// 10% chance
		else
			n := 30;

		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 26% chance
		elseif (power < 26000)
			n := 15;
		
		// 35% chance
		elseif (power < 61000)
			n := 20;

		// 24% chance
		elseif (power < 85000)
			n := 25;

		// 14.95% chance
		elseif (power < 99950)
			n := 30;

		// 0.05% chance
		else
		 	n := 50;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 20% chance
		elseif (power < 20000)
			n := 15;
		
		// 35% chance
		elseif (power < 55000)
			n := 20;

		// 25% chance
		elseif (power < 80000)
			n := 25;

		// 19.9% chance
		elseif (power < 99900)
			n := 30;

		// 0.1% chance
		else
			n := 50;

		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 15% chance
		elseif (power < 15000)
			n := 15;
		
		// 35% chance
		elseif (power < 50000)
			n := 20;

		// 25% chance
		elseif (power < 75000)
			n := 25;

		// 24% chance
		elseif (power < 99000)
			n := 30;

		// 1% chance
		else
			n := 50;

		endif

	// Level 11+ loot
	else
		
		// 0% chance
		if (power < 0)
			n := 5;

		// 0% chance
		elseif (power < 0)
			n := 10;

		// 15% chance
		elseif (power < 15000)
			n := 15;
		
		// 35% chance
		elseif (power < 50000)
			n := 20;

		// 25% chance
		elseif (power < 75000)
			n := 25;

		// 24.8% chance
		elseif (power < 99800)
			n := 30;

		// 0.2% chance
		else
			n := 50;

		endif

	endif

	if(n == 25)
		SetObjProperty(item, "isUncommon", 1);
	elseif(n == 30)
		SetObjProperty(item, "isRare", 1);
	elseif(n == 50)
		SetObjProperty(item, "isLegendary", 1);
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

	//	if(n == 50)
	//		if(skillid == SKILLID_ALCHEMY or skillid == SKILLID_MAGERY or skillid == SKILLID_ITEMID or skillid == SKILLID_EVALINT or skillid == SKILLID_INSCRIPTION or skillid == SKILLID_MAGICRESISTANCE or skillid == SKILLID_MEDITATION or skillid == SKILLID_SPIRITSPEAK)
	//			EraseObjProperty( item , "ArBonus");
	//		endif
	//	endif

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// ApplyOnHitScript() - assigns the armours hit script.  
//
// Just copy pasted from weap
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
function ApplyOnHitScript( item )

	CountAvailOnHitscripts();

	var power := RandomInt(100000);

 	if (magiclevel <= 0)
			ApplySpellOnHitscript( item ); // Spell on hit

	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 80% chance of spell
		if (power < 80000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 20% chance slayer
		else
			ApplyResistantOnHitscript( item ); // 

		endif

	// Level 2 loot
	elseif (magiclevel == 2)
		
		// 60% chance of spell
		if (power < 60000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 40% chance slayer
		else
			ApplyResistantOnHitscript( item ); // 

		endif

	// Level 3 loot
	elseif (magiclevel == 3)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 55% chance slayer
		elseif (power < 95000)
			ApplyResistantOnHitscript( item ); // 

		// 5% chance effect
		else 
			ApplyEffectOnHitscript( item ); // 
		endif	

	// Level 4 loot
	elseif (magiclevel == 4)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 50% chance slayer
		elseif (power < 90000)
			ApplyResistantOnHitscript( item ); // 

		// 10% chance effect
		else 
			ApplyEffectOnHitscript( item ); // 
		endif	

	// Level 5 loot
	elseif (magiclevel == 5)
		
		// 45% chance of spell
		if (power < 45000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 40% chance slayer
		elseif (power < 85000)
			ApplyResistantOnHitscript( item ); // 

		// 15% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	// Level 6 loot
	elseif (magiclevel == 6)
		
		// 40% chance of spell
		if (power < 40000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 75000)
			ApplyResistantOnHitscript( item ); // 

		// 25% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif	

	// Level 7 loot
	elseif (magiclevel == 7)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 35% chance slayer
		elseif (power < 65000)
			ApplyResistantOnHitscript( item ); // 

		// 35% chance effect
		else
			ApplyEffectOnHitscript( item ); // 
		endif		

	// Level 8 loot
	elseif (magiclevel == 8)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 32.5% chance slayer
		elseif (power < 62500)
			ApplyResistantOnHitscript( item ); // 

		// 32.5% chance effect
		elseif (power < 95000)
			ApplyEffectOnHitscript( item ); // 
		
		// 5% chance greater effect
		else
			ApplyGreaterOnHitscript( item ); //
		endif	

	// Level 9 loot
	elseif (magiclevel == 9)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 30% chance slayer
		elseif (power < 60000)
			ApplyResistantOnHitscript( item ); // 

		// 30% chance effect
		elseif (power < 90000)
			ApplyEffectOnHitscript( item ); // 
		
		// 10% chance greater effect
		else
			ApplyGreaterOnHitscript( item ); //
		endif	

	// Level 10 loot
	elseif (magiclevel == 10)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 10% chance slayer
		elseif (power < 40000)
			ApplyResistantOnHitscript( item ); // 

		// 30% chance effect
		elseif (power < 70000)
			ApplyEffectOnHitscript( item ); // 
		
		// 30% chance greater effect
		else
			ApplyGreaterOnHitscript( item ); //
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 20% chance slayer
		elseif (power < 50000)
			ApplyResistantOnHitscript( item ); // 

		// 25% chance effect
		elseif (power < 75000)
			ApplyEffectOnHitscript( item ); // 
		
		// 25% chance greater effect
		else
			ApplyGreaterOnHitscript( item ); //
		endif

	// Level 11+ loot
	else
		
		// 30% chance of spell
		if (power < 30000)
			ApplySpellOnHitscript( item ); // Spell on hit

		// 10% chance slayer
		elseif (power < 40000)
			ApplyResistantOnHitscript( item ); // 

		// 30% chance effect
		elseif (power < 70000)
			ApplyEffectOnHitscript( item ); // 
		
		// 30% chance greater effect
		else
			ApplyGreaterOnHitscript( item ); //
		endif
	endif
endfunction

//Touched by Dev Alryc 04-11-2026
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

	var rand := RandomInt(100000);

	//1000 == 1%

	if (magiclevel == 0)
		ascircle := 1;

	// Level 1 loot
	
	elseif (magiclevel == 1)
		
		//98% chance
		if (rand < 98000)
		
			ascircle := 1;
	
		//2% chance
		else

			ascircle := 2;
		endif

	// Level 2 loot

	elseif (magiclevel == 2)
		
		//82% chance
		if (rand < 82000)
		
			ascircle := 1;
	
		//17.5% chance
		elseif (rand < 99500)

			ascircle := 2;

		//0.5% chance
		else
			ascircle := 3;
		endif

	// Level 3 loot

	elseif (magiclevel == 3)
		
		//30% chance
		if (rand < 30000)
		
			ascircle := 1;
	
		//60% chance
		elseif (rand < 90000)

			ascircle := 2;

		//10% chance
		else
			ascircle := 3;
		endif

	// Level 4 loot

	elseif (magiclevel == 4)
		
		//20% chance
		if (rand < 20000)
		
			ascircle := 1;
	
		//44.997% chance
		elseif (rand < 64997)

			ascircle := 2;

		//30% chance
		elseif (rand < 94997)
			ascircle := 3;

		//5.003% chance
		else
			ascircle := 4;
		endif

	// Level 5 loot
		
	elseif (magiclevel == 5)
		
		//15% chance
		if (rand < 15000)
		
			ascircle := 1;
	
		//29.999% chance
		elseif (rand < 44999)

			ascircle := 2;

		//46% chance
		elseif (rand < 90999)
			ascircle := 3;

		//9.001% chance
		else
			ascircle := 4;
		endif

	// Level 6 loot

	elseif (magiclevel == 6)
		
		//1% chance
		if (rand < 1000)
		
			ascircle := 1;
	
		//15% chance
		elseif (rand < 16000)

			ascircle := 2;

		//30% chance
		elseif (rand < 46000)
			ascircle := 3;

		//29% chance
		elseif (rand < 75000)
			ascircle := 4;

		//20% chance
		elseif (rand < 95000)
			ascircle := 5;

		//5% chance
		else
			ascircle := 6;
		endif

	// Level 7 loot
	
	elseif (magiclevel == 7)
		
		//1% chance
		if (rand < 1000)
		
			ascircle := 1;
	
		//15% chance
		elseif (rand < 16000)

			ascircle := 2;

		//30% chance
		elseif (rand < 46000)
			ascircle := 3;

		//29% chance
		elseif (rand < 75000)
			ascircle := 4;

		//20% chance
		elseif (rand < 95000)
			ascircle := 5;

		//5% chance
		else
			ascircle := 6;
		endif

	// Level 8 loot

	elseif (magiclevel == 8)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//61% chance
		elseif (rand < 61000)
			ascircle := 3;

		//9% chance
		elseif (rand < 70000)
			ascircle := 4;

		//15% chance
		elseif (rand < 85000)
			ascircle := 5;

		//10% chance
		elseif (rand < 95000)
			ascircle := 6;

		//5% chance
		else
			ascircle := 7;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//26% chance
		elseif (rand < 26000)
			ascircle := 3;

		//35% chance
		elseif (rand < 61000)
			ascircle := 4;

		//9% chance
		elseif (rand < 70000)
			ascircle := 5;

		//15% chance
		elseif (rand < 85000)
			ascircle := 6;

		//10% chance
		elseif (rand < 95000)
			ascircle := 7;

		//5% chance
		else
			ascircle := 8;
		endif

	// Level 10 loot
	elseif (magiclevel == 10)
		
		//0% 1st circle
		if (rand < 0)
		
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)

			ascircle := 2;

		//0% chance
		elseif (rand < 0)
			ascircle := 3;

		//35% chance
		elseif (rand < 35000)
			ascircle := 4;

		//30% chance
		elseif (rand < 65000)
			ascircle := 5;

		//15% chance
		elseif (rand < 80000)
			ascircle := 6;

		//12% chance
		elseif (rand < 92000)
			ascircle := 7;

		//5% chance
		elseif (rand < 97000)
			ascircle := 8;

		//1% chance
		elseif (rand < 98000)
			ascircle := 9;

		//2% chance
		else 
			ascircle := 10;
		endif

	// Level 11 loot
	elseif (magiclevel == 11)
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//0% chance
		elseif (rand < 0)
			ascircle := 3;

		//0% chance
		elseif (rand < 0)
			ascircle := 4;

		//50% chance
		elseif (rand < 50000)
			ascircle := 5;

		//20% chance
		elseif (rand < 70000)
			ascircle := 6;

		//15% chance
		elseif (rand < 85000)
			ascircle := 7;

		//10% chance
		elseif (rand < 95000)
			ascircle := 8;

		//3% chance
		elseif (rand < 98000)
			ascircle := 9;

		//2% chance
		else 
			ascircle := 10;
		endif

	// Level 11+ loot
	else
		
		//0% 1st circle
		if (rand < 0)
			ascircle := 1;
	
		//0% 2nd circle
		elseif (rand < 0)
			ascircle := 2;

		//0% chance
		elseif (rand < 0)
			ascircle := 3;

		//0% chance
		elseif (rand < 0)
			ascircle := 4;

		//40% chance
		elseif (rand < 40000)
			ascircle := 5;

		//22.5% chance
		elseif (rand < 62500)
			ascircle := 6;

		//17.5% chance
		elseif (rand < 80000)
			ascircle := 7;

		//15% chance
		elseif (rand < 95000)
			ascircle := 8;

		//3% chance
		elseif (rand < 98000)
			ascircle := 9;

		//2% chance
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
	elseif(ascircle > 7)
		SetObjProperty(item, "isRare", 1);
	endif

	SetObjProperty( item , "EffectCircle" , ascircle );

	//Set the spell chance
	var chanceofeffect := RandomDiceStr("1d11") * magiclevel;
	
	if (chanceofeffect <= 0)
		chanceofeffect := 5;
	elseif (chanceofeffect > 80)
		SetObjProperty(item, "isRare", 1);
	//	chanceofeffect := 80;
	endif

	SetObjProperty( item , "ChanceOfEffect" , chanceofeffect );
	SetOnHitscript( item , n , multiplier );
	set_critical( 0 );
	
endfunction

//Touched by Dev Alryc 04-11-2026
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
		SetObjProperty(item, "isUncommon", 1);
	endif

	var type := onhitcfg[n].Type;
	SetObjProperty( item , "ProtectedType" , type );

	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction

//Touched by Dev Alryc 04-11-2026
function ApplyEffectOnHitscript( item )
	
	set_critical( 1 );
	
	var multiplier := 10;
	
	//Find the good effect
	var i := RandomDiceStr( "1d"+availeffectonhit.Size());
	var n := availeffectonhit[i];
	SetOnHitscript( item , n , multiplier );

	set_critical( 0 );
	
endfunction

//Touched by Dev Alryc 04-11-2026
function ApplyGreaterOnHitscript( item )
	//Print("Applying greater on hit script");
	set_critical( 1 );

	var multiplier := 2;
	
	//Find the good script
	var i := RandomDiceStr( "1d"+availgreateronhit.Size());
	//Print("Greater on hit script number: " + i);
	var n := availgreateronhit[i];

	SetOnHitscript( item , n , multiplier );
	SetObjProperty(item, "isRare", 1);

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

//Touched by Dev Alryc 04-11-2026
function ApplyStatMod ( item )

	var power := RandomInt(100000);
	var n;

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;
	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 80% chance
		if (power < 80000)
			n := 1;

		// 20% chance
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
	
		// 70% chance
		if (power < 70000)
			n := 1;

		// 20% chance
		elseif (power < 90000)
			n := 2;

		// 10% chance
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
	
		// 30% chance
		if (power < 30000)
			n := 1;

		// 50% chance
		elseif (power < 80000)
			n := 2;

		// 20% chance
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)

		// 20% chance
		if (power < 20000)
			n := 1;

		// 50% chance
		elseif (power < 70000)
			n := 2;

		// 10% chance of level 3
		elseif (power < 80000)
			n := 3;
		
		// 20% chance
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)

		// 15% chance
		if (power < 15000)
			n := 1;

		// 30% chance
		elseif (power < 45000)
			n := 2;

		// 45% chance
		elseif (power < 90000)
			n := 3;
		
		// 10% chance
		else
			n := 4;
		endif
	
	// Level 6 loot
	elseif (magiclevel == 6)

		// 4% chance
		if (power < 4000)
			n := 1;

		// 35% chance
		elseif (power < 39000)
			n := 2;

		// 40% chance
		elseif (power < 79000)
			n := 3;
		
		// 16% chance
		elseif (power < 95000)
			n := 4;

		// 3% chance
		elseif (power < 98000)
			n := 5;			
		
		// 2% chance
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)

		// 4% chance
		if (power < 4000)
			n := 1;

		// 30% chance
		elseif (power < 34000)
			n := 2;

		// 40% chance
		elseif (power < 74000)
			n := 3;
		
		// 20% chance
		elseif (power < 94000)
			n := 4;

		// 4% chance
		elseif (power < 98000)
			n := 5;			
		
		// 2% chance
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 35% chance
		elseif (power < 35000)
			n := 3;
		
		// 30% chance
		elseif (power < 65000)
			n := 4;

		// 30% chance
		elseif (power < 95000)
			n := 5;			
		
		// 5% chance
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
	
		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 30% chance of level 3
		elseif (power < 30000)
			n := 3;
		
		// 30% chance of level 4
		elseif (power < 60000)
			n := 4;

		// 30% chance of level 5
		elseif (power < 90000)
			n := 5;			
		
		// 10% chance of level 6
		else
			n := 6;
		endif
	
	// Level 10 loot
	elseif (magiclevel == 10) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 10% chance of level 3
		elseif (power < 10000)
			n := 3;
		
		// 37.5% chance
		elseif (power < 47500)
			n := 4;

		// 37.5% chance
		elseif (power < 85000)
			n := 5;			
		
		// 15% chance
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 0% chance of level 3
		elseif (power < 0)
			n := 3;
		
		// 40% chance
		elseif (power < 40000)
			n := 4;

		// 40% chance
		elseif (power < 80000)
			n := 5;			
		
		// 20% chance
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 0% chance of level 3
		elseif (power < 0)
			n := 3;
		
		// 30% chance
		elseif (power < 30000)
			n := 4;

		// 30% chance
		elseif (power < 60000)
			n := 5;			
		
		// 40% chance
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
// 			ApplyEnchant - Function to create protection Items.
//		    3 Main Groups lowest to highest - Charge Items, Elemental Prots, Immunity Prots (brock/moon/pzn)
//			
//		    The group chance is based purely on magiclevel, the quality of the drop is also based purely on item level.
//
//		    Note: This function doesn't determine if an enchant can drop. It's what happens if one DOES drop.
//		    Look earlier to see what determines if an enchant drops.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			

//Touched by Dev Alryc 04-11-2026
function ApplyEnchant( unused who , item )

	var rand := RandomInt(100);
	var choice := 0;

	case (magiclevel)
		1: if (rand < 85) choice := 1; elseif (rand < 96) choice := 2; else choice := 3; endif // 85%-C 11%-E 04%-I
	 	2: if (rand < 75) choice := 1; elseif (rand < 93) choice := 2; else choice := 3; endif // 75%-C 18%-E 07%-I
		3: if (rand < 65) choice := 1; elseif (rand < 90) choice := 2; else choice := 3; endif // 65%-C 25%-E 10%-I
		4: if (rand < 55) choice := 1; elseif (rand < 87) choice := 2; else choice := 3; endif // 55%-C 32%-E 13%-I
		5: if (rand < 45) choice := 1; elseif (rand < 84) choice := 2; else choice := 3; endif // 45%-C 39%-E 16%-I
		6: if (rand < 35) choice := 1; elseif (rand < 81) choice := 2; else choice := 3; endif // 35%-C 46%-E 19%-I
		7: if (rand < 25) choice := 1; elseif (rand < 78) choice := 2; else choice := 3; endif // 25%-C 53%-E 22%-I
		8: if (rand < 20) choice := 1; elseif (rand < 75) choice := 2; else choice := 3; endif // 20%-C 55%-E 25%-I
		9: if (rand < 15) choice := 1; elseif (rand < 70) choice := 2; else choice := 3; endif // 15%-C 55%-E 30%-I
		10: if (rand < 10) choice := 1; elseif (rand < 65) choice := 2; else choice := 3; endif // 10%-C 55%-E 35%-I
		11: if (rand < 5) choice := 1; elseif (rand < 60) choice := 2; else choice := 3; endif // 05%-C 55%-E 40%-I
		default: if (rand < 5) choice := 1; elseif (rand < 50) choice := 2; else choice := 3; endif // 05%-C 45%-E 50%-I
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

//Touched by Dev Alryc 04-11-2026
function DetermineImmunityLevel( unused item, unused isimmun := 0, unused modifier := 0)

	var n;
	var power := RandomInt(100000);

	// Nothing with this should get here 	
	if (magiclevel <= 0)
			n := 1;
	// Level 1 loot
	elseif (magiclevel == 1)
		
		// 98% chance of level 1
		if (power < 98000)
			n := 1;

		// 2% chance of level 2
		else
			n := 2;
		endif

	// Level 2 loot
	elseif (magiclevel == 2)
	
		// 82% chance of level 1
		if (power < 82000)
			n := 1;

		// 17.5% chance of level 2
		elseif (power < 99500)
			n := 2;

		// 0.5% chance of level 3
		else
			n := 3;
		endif

	// Level 3 loot
	elseif (magiclevel == 3)
	
		// 30% chance of level 1
		if (power < 30000)
			n := 1;

		// 60% chance of level 2
		elseif (power < 90000)
			n := 2;

		// 10% chance of level 3
		else
			n := 3;
		endif

	// Level 4 loot
	elseif (magiclevel == 4)

		// 20% chance of level 1
		if (power < 20000)
			n := 1;

		// 45% chance of level 2
		elseif (power < 65000)
			n := 2;

		// 30% chance of level 3
		elseif (power < 95000)
			n := 3;
		
		// 5% chance of level 4
		else
			n := 4;
		endif

	// Level 5 loot
	elseif (magiclevel == 5)

		// 15% chance of level 1
		if (power < 15000)
			n := 1;

		// 30% chance of level 2
		elseif (power < 45000)
			n := 2;

		// 45% chance of level 3
		elseif (power < 90000)
			n := 3;
		
		// 10% chance of level 4
		else
			n := 4;
		endif
	
	// Level 6 loot
	elseif (magiclevel == 6)

		// 4% chance of level 1
		if (power < 4000)
			n := 1;

		// 35% chance of level 2
		elseif (power < 39000)
			n := 2;

		// 40% chance of level 3
		elseif (power < 79000)
			n := 3;
		
		// 16% chance of level 4
		elseif (power < 95000)
			n := 4;

		// 3% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// 2% chance of level 6
		else
			n := 6;
		endif

	// Level 7 loot
	elseif (magiclevel == 7)

		// 4% chance of level 1
		if (power < 4000)
			n := 1;

		// 30% chance of level 2
		elseif (power < 34000)
			n := 2;

		// 40% chance of level 3
		elseif (power < 74000)
			n := 3;
		
		// 20% chance of level 4
		elseif (power < 94000)
			n := 4;

		// 4% chance of level 5
		elseif (power < 98000)
			n := 5;			
		
		// 2% chance of level 6
		else
			n := 6;
		endif

	// Level 8 loot
	elseif (magiclevel == 8)

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 65% chance of level 3
		elseif (power < 65000)
			n := 3;
		
		// 20% chance of level 4
		elseif (power < 85000)
			n := 4;

		// 10% chance of level 5
		elseif (power < 95000)
			n := 5;			
		
		// 5% chance of level 6
		else
			n := 6;
		endif

	// Level 9 loot
	elseif (magiclevel == 9)
	
		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 60% chance of level 3
		elseif (power < 60000)
			n := 3;
		
		// 20% chance of level 4
		elseif (power < 80000)
			n := 4;

		// 10% chance of level 5
		elseif (power < 90000)
			n := 5;			
		
		// 10% chance of level 6
		else
			n := 6;
		endif
			
	// Level 10 loot
	elseif (magiclevel == 10) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 15% chance of level 3
		elseif (power < 15000)
			n := 3;
		
		// 45% chance of level 4
		elseif (power < 60000)
			n := 4;

		// 25% chance of level 5
		elseif (power < 85000)
			n := 5;			
		
		// 15% chance of level 6
		else
			n := 6;
		endif

	// Level 11 loot
	elseif (magiclevel == 11) 

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 10% chance of level 3
		elseif (power < 10000)
			n := 3;
		
		// 40% chance of level 4
		elseif (power < 50000)
			n := 4;

		// 30% chance of level 5
		elseif (power < 80000)
			n := 5;			
		
		// 20% chance of level 6
		else
			n := 6;
		endif

	// Level 11+ loot
	else

		// 0% chance of level 1
		if (power < 0)
			n := 1;

		// 0% chance of level 2
		elseif (power < 0)
			n := 2;

		// 0% chance of level 3
		elseif (power < 0)
			n := 3;
		
		// 40% chance of level 4
		elseif (power < 40000)
			n := 4;

		// 35% chance of level 5
		elseif (power < 75000)
			n := 5;			
		
		// 25% chance of level 6
		else
			n := 6;
		endif

	endif
	
	return n;

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool - 21/04/2012
//
// 				ApplyProtection - This is the core function of determining the level for
//			    immunities/elemental prots. It's based purely on the magiclevel
//
/////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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
			if(power == 4)
				SetObjProperty(item, "isUncommonProt", 1);
			elseif(power > 4)
				SetObjProperty(item, "isRareProt", 1);
			endif
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

	// New system to calculate protection levels.
	if (ran < 6 || ran == 8)
		case(power)
		// Level 1 protection (Bane)
		1:	power := 25;
			break;
		
		// Level 2 protection (Warding)
		2:	power := 50;
			break;

		// Level 3 protection (Protection)
		3:	power := 65;
			SetObjProperty(item, "isUncommonProt", 1);
			break;
		
		// Level 4 protection (Immunity)
		4:	power := 75;
			SetObjProperty(item, "isUncommonProt", 1);
			break;
		
		// Level 5 protection (Attunement)
		5:	power := 85;
			SetObjProperty(item, "isRareProt", 1);
			break;
		
		// Level 6 protection (Absorbsion)
		6:	power := 95;
			SetObjProperty(item, "isRareProt", 1);
			break;
		endcase
	endif

	SetObjProperty( item , element , power );
	
endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
//
//
// Adds "a magic " to the beginning of the item's desc.
//
/////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
function CountAvailHitscripts()

	availeffects := {};
	availspells := {};
	availgreaters := {};

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
	//Print("Available on hit scripts: " + availspellonhit.Size() + " spells, " + availeffectonhit.Size() + " effects, " + availgreateronhit.Size() + " greaters.");
endfunction

//Touched by Dev Alryc 04-11-2026
function CountAvailOnHitscripts()
	availeffectonhit := {};
	availspellonhit := {};
	availgreateronhit := {};

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
	//Print("Available on hit scripts: " + availspellonhit.Size() + " spells, " + availeffectonhit.Size() + " effects, " + availgreateronhit.Size() + " greaters.");
endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// Below 2 function not altered at all.  See how they go.
//
/////////////////////////////////////////////////////////////////////////////////////////////

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
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

//Touched by Dev Alryc 04-11-2026
// Shared shield table for HP/AR/ArSkill rolls.
// Keep this in sync with ShieldApplyHPMod percentages.
function ShieldRollModTierFromHPTable()

	var power := RandomInt(100000)+1;
	var n;

	if (magiclevel <= 0)
		n := 5;

	elseif (magiclevel == 1)
		if (power < 98000)
			n := 5;
		else
			n := 10;
		endif

	elseif (magiclevel == 2)
		if (power < 82000)
			n := 5;
		elseif (power < 99500)
			n := 10;
		else
			n := 15;
		endif

	elseif (magiclevel == 3)
		if (power < 30000)
			n := 5;
		elseif (power < 90000)
			n := 10;
		else
			n := 15;
		endif

	elseif (magiclevel == 4)
		if (power < 20000)
			n := 5;
		elseif (power < 65000)
			n := 10;
		elseif (power < 95000)
			n := 15;
		else
			n := 20;
		endif

	elseif (magiclevel == 5)
		if (power < 15000)
			n := 5;
		elseif (power < 45000)
			n := 10;
		elseif (power < 91000)
			n := 15;
		else
			n := 20;
		endif

	elseif (magiclevel == 6)
		if (power < 3000)
			n := 5;
		elseif (power < 18000)
			n := 10;
		elseif (power < 48000)
			n := 15;
		elseif (power < 77000)
			n := 20;
		elseif (power < 97000)
			n := 25;
		else
			n := 30;
		endif

	elseif (magiclevel == 7)
		if (power < 3000)
			n := 5;
		elseif (power < 18000)
			n := 10;
		elseif (power < 48000)
			n := 15;
		elseif (power < 77000)
			n := 20;
		elseif (power < 97000)
			n := 25;
		else
			n := 30;
		endif

	elseif (magiclevel == 8)
		if (power < 0)
			n := 5;
		elseif (power < 0)
			n := 10;
		elseif (power < 38000)
			n := 15;
		elseif (power < 68000)
			n := 20;
		elseif (power < 95000)
			n := 25;
		else
			n := 30;
		endif

	elseif (magiclevel == 9)
		if (power < 0)
			n := 5;
		elseif (power < 0)
			n := 10;
		elseif (power < 28000)
			n := 15;
		elseif (power < 63000)
			n := 20;
		elseif (power < 85000)
			n := 25;
		else
			n := 30;
		endif

	elseif (magiclevel == 10)
		if (power < 0)
			n := 5;
		elseif (power < 0)
			n := 10;
		elseif (power < 0)
			n := 15;
		elseif (power < 40000)
			n := 20;
		elseif (power < 80000)
			n := 25;
		else
			n := 30;
		endif

	elseif (magiclevel == 11)
		if (power < 0)
			n := 5;
		elseif (power < 0)
			n := 10;
		elseif (power < 0)
			n := 15;
		elseif (power < 25000)
			n := 20;
		elseif (power < 75000)
			n := 25;
		else
			n := 30;
		endif

	else
		if (power < 0)
			n := 5;
		elseif (power < 0)
			n := 10;
		elseif (power < 0)
			n := 15;
		elseif (power < 20000)
			n := 20;
		elseif (power < 70000)
			n := 25;
		else
			n := 30;
		endif
	endif

	return n;

endfunction

//Touched by Dev Alryc 04-11-2026
function ShieldApplyHPMod( item )

	var n := ShieldRollModTierFromHPTable();

	item.maxhp_mod := n;
	item.hp := item.maxhp;

endfunction

//Touched by Dev Alryc 04-11-2026
function ShieldApplyARMod( item )

	var n := ShieldRollModTierFromHPTable();

	item.ar_mod := n;

	//Check for another enchantment
	// Equal per-level scaling: +1/11 per level, level 11 is 100%.
	if( (RandomInt(11)+1) <= magiclevel )
		if ((Random(100)+1) <= 50) // 50% of adding a hp mod
			ShieldApplyHPMod( item );
		else
			ShieldApplyArSkillMod( item ); // 50% chance of adding a ar skill mod
		endif
	endif

endfunction

//Touched by Dev Alryc 04-11-2026
function ShieldApplyStatMod( item )

	var n := CInt( ShieldRollModTierFromHPTable() / 5 );

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
	// Add a HP mod,  each level gets an extra 5% chance
	if ((Random(100)+1) <= 5 * magiclevel)
		ShieldApplyHPMod( item );
	endif

endfunction

//Touched by Dev Alryc 04-11-2026
function ShieldApplyArSkillMod( item )

	var chance := Random(100);
	if(chance <= 20)
		ShieldApplyStatMod( item );
		return;
	endif

	var skillid;
	var amount;
	var n := ShieldRollModTierFromHPTable();

	if(n == 30 || n == 25)
		SetObjProperty(item, "isRare", 1);
	endif

	amount := CInt( n / 2 );

	// Shield/armor skill mods use melee weapon skills plus parry.
	// Archery, Hiding and Magic Resistance are intentionally excluded.
	var skillpool := { SKILLID_FENCING, SKILLID_MACEFIGHTING, SKILLID_SWORDSMANSHIP, SKILLID_TACTICS, SKILLID_PARRY, SKILLID_WRESTLING };
	var skillroll := RandomDiceStr( "1d" + len(skillpool) );
	skillid := skillpool[skillroll];

	SetObjProperty( item , "ShieldAdv", 1 );
	SetObjProperty( item , "skilladv" , skillid );
	SetObjProperty( item , "skilladvamount" , amount );

	//Check for another enchantment
	if ((Random(100)+1) <= (5 * magiclevel))
		ShieldApplyHPMod( item );
	endif

endfunction