use uo;
use os;
use util;
use polsys;
use cfgfile;
use unicode;
use attributes;

include "include/client";
include "include/dotempmods";
include "include/classes";
include "include/damages";
include "include/bitwise";
include "include/attributes";
include ":lootlottery:storedamage";
include "include/skillpoints";

var combat_settingscfg := ReadConfigFile(":combat:settings");

function RecalcDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var noPKatk := GetObjProperty(attacker, "NOPKAREA");
	var noPKdef := GetObjProperty(defender, "NOPKAREA");

	var _any_npc := 0;
	if(attacker.isa(POLCLASS_NPC) || defender.isa(POLCLASS_NPC)) 
		_any_npc := 1;
	endif

	if((!attacker.cmdlevel && !_any_npc && defender != attacker) && (noPKatk || noPKdef))	
		attacker.setwarmode(0);
		defender.setwarmode(0);
		return;
	endif

	var noSAatk := GetObjProperty(attacker, "InSafeArea");
	var noSAdef := GetObjProperty(defender, "InSafeArea");

	if((!attacker.cmdlevel && !_any_npc && defender != attacker) && (noSAatk || noSAdef))	
		if(!Getobjproperty(attacker, "pvping"))
			attacker.setwarmode(0);
		endif
		if(!Getobjproperty(defender, "pvping"))
			defender.setwarmode(0);
		endif
		return;
	endif

	if( !GetObjProperty( weapon, "Astral" ) )
		return RecalcPhysicalDmg( attacker , defender , weapon , armor , basedamage , piercing );
	else
		return RecalcAstralDmg( attacker , defender , weapon , armor , basedamage , piercing );
	endif

endfunction


function RecalcPhysicalDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var rawdamage;
	var absorbed;
	var ar := defender.ar;

   	if( !ar )
    	ar := 0;
    endif

	if(DEBUG_MODE)
		SendSysMessage(attacker, "MY opponents AR is: "+ar, FONT_NORMAL, 2601 );
	endif

	if(GetObjProperty( attacker , CLASSEID_THIEF ) || GetObjProperty( attacker , CLASSEID_MAGE) || GetObjProperty( attacker , CLASSEID_BARD))
		if(weapon.graphic in ClassWeapons(GetClass(attacker)))
			basedamage := CalcPhysicalDamage( attacker, defender, weapon, basedamage );
		else
			if(DEBUG_MODE)
				PrintTextAbovePrivate( attacker, "[WARNING] Not using a class weapon so I won't do any damage.", attacker );
			endif
			basedamage := 0;
			return basedamage;
		endif
	else
		basedamage := CalcPhysicalDamage( attacker, defender, weapon, basedamage );
	endif

	//Start Elemental Damage calculations
	var ElementalDamage := GetObjProperty(weapon, "ElementalDamage");
	if (ElementalDamage)
		var totalelemental := 0;
		var physdamage := basedamage; // save initial basedamage value
		var has_physical := 0;
		ElementalDamage := SplitWords(ElementalDamage, " ");
		foreach element in ElementalDamage
			var split_element := SplitWords(element, ":");
			var element_ID;
			case(split_element[1])
				"FIRE"		: element_ID := 0x0001;
				"AIR"		: element_ID := 0x0002;
				"EARTH"		: element_ID := 0x0004;
				"WATER"		: element_ID := 0x0008;
				"NECRO"		: element_ID := 0x0010;
				"HOLY"		: element_ID := 0x0020;
				"POISON"	: element_ID := 0x0040;
				"ACID"		: element_ID := 0x0080;
				"PHYSICAL"	: element_ID := 0x0100;
				"MAGIC"		: element_ID := 0x0200;
				"ASTRAL"	: element_ID := 0x0400;
				"NO_RESIST"	: element_ID := 0x0800;
			endcase
			var dmg_amount := physdamage * Cint(split_element[2])/100.0;
			totalelemental += dmg_amount; // add up to check if we'll still do physical later
			if (element_ID == 0x0100)
				basedamage := dmg_amount; // modify physical to pass onto the rest of hitscript
				has_physical := 1;
				continue;
			endif
			ApplyElementalDamageNoResist( defender, attacker, dmg_amount, element_ID );
		endforeach
		if (!has_physical) // if no physical damage was specified, it'll process the remainder as physical
			basedamage := Max(basedamage - totalelemental, 0);
		endif
	endif

	if( !piercing )
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Basedamage before absorption is: "+basedamage, FONT_NORMAL, 2569 );
		endif
		rawdamage := GetShieldAbsorption( attacker , defender , weapon , basedamage );
		absorbed  := ArAbsorptionCalc(ar, basedamage);
		CheckEquipmentDamage(defender, armor);
	else
		rawdamage := basedamage;
		absorbed  := 0;
	endif

	rawdamage -= absorbed;


	if(DEBUG_MODE)
		SendSysMessage(attacker, "He absorbed: "+absorbed+ " damage.", FONT_NORMAL, 2567 );
		SendSysMessage( attacker, "Damage after absorption is: "+rawdamage, FONT_NORMAL, 2573 );
	endif
	rawdamage := ModByProt( defender , rawdamage );
	if( rawdamage < 0 )
		rawdamage := 0;
	endif

	if( weapon.attribute == ATTRIBUTEID_MACEFIGHTING )
		MaceHit( attacker , defender , armor );
	endif

	if(DEBUG_MODE)
		SendSysMessage(attacker, "Damage dealt: "+rawdamage, FONT_NORMAL, 2595 );
	endif

	return CInt( rawdamage );

endfunction


function RecalcAstralDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var rawdamage;
	weapon := weapon;
	var absorbed := 0;

	var ar := CInt( GetObjProperty( armor , "Astral" ) ) * 25 * CInt( armor.ar );

	basedamage := basedamage
		* ((GetEffectiveSkill( attacker , SKILLID_SPIRITSPEAK ) + 50.0 + (GetIntelligence(attacker) * 0.2)) * 0.01)
		/ ((GetEffectiveSkill( attacker , SKILLID_TACTICS ) + 50.0 + (GetStrength(attacker) * 0.2)) * 0.01);

	AwardRawSkillPoints( attacker, SKILLID_EVALINT, 30 );
	AwardRawSkillPoints( attacker, SKILLID_SPIRITSPEAK, 30 );

	if( defender.isa(POLCLASS_NPC) )
		basedamage *= ClasseBonus( attacker , CLASSEID_MAGE );
	else
		var level := GetObjProperty( attacker , CLASSEID_MAGE );
		if( level )
			level := level - 2;
			if( level >= 1 )
				basedamage *= ClasseBonusByLevel( level );
			endif
		endif
	endif

	if( GetObjProperty( attacker , CLASSEID_WARRIOR ) )
		if( !GetObjProperty( attacker , CLASSEID_MAGE ) )
			basedamage /= ClasseBonus( attacker , CLASSEID_WARRIOR );
		endif
	endif

	if( GetObjProperty( defender , CLASSEID_WARRIOR ) )
		if( !GetObjProperty( defender , CLASSEID_MAGE ) )
			basedamage *= (ClasseBonus( defender , CLASSEID_WARRIOR ) * 5/6);
		endif
	endif

	basedamage *= ( 1 + GetEffectiveSkill( attacker , SKILLID_EVALINT ) * 0.002 );
	rawdamage  := basedamage;

	if( !piercing )

		var meditation := GetEffectiveSkill( defender , SKILLID_MEDITATION );
		meditation := meditation * ClasseBonus( defender , CLASSEID_MAGE );

		if( (Random(100) + 1) <= CInt( meditation * 0.5 ) )

			var resist := GetEffectiveSkill( defender , SKILLID_MAGICRESISTANCE );
			var defint := GetIntelligence(defender);

			AwardRawSkillPoints( defender, SKILLID_MEDITATION, 30 );
			AwardRawSkillPoints( defender, SKILLID_MAGICRESISTANCE, 30 );
			PerformAction(  defender, ANIM_CAST_AREA );
			SendSysmessage( defender, "You successfully block a part of the astral attack." );

			if( GetObjProperty( defender, CLASSEID_MAGE ) )
				rawdamage -= (resist + defint) * 0.25 * ClasseBonus( defender , CLASSEID_MAGE );
			elseif( GetObjProperty( defender, CLASSEID_WARRIOR ) )
				rawdamage -= (resist + defint) * 0.25 / ClasseBonus( defender , CLASSEID_WARRIOR );
			endif

			if( rawdamage < 0 )
				rawdamage := 0;
			endif

		endif

		if( ar )
			absorbed := CInt( ar * (Random(51) + 50) / 100 );
		endif

	endif

	rawdamage -= absorbed;
	rawdamage := rawdamage * 0.5;
	if( rawdamage < 0 )
		rawdamage := 0;
	endif

	return CInt(rawdamage);

endfunction


function CalcPhysicalDamage( attacker, defender, weapon, basedamage )

    var total_skills 			:= GetClasseSkillTotal(attacker, GetClass(attacker));
	var avg_skill				:= total_skills / 8;
	var level 					:= GetClasseLevel(attacker);
	var defenderlevel			:= GetClasseLevel(defender);

	//================================= Pre Class Calculations ===================================\\
	// Global base damage modifier.
	/* basedamage *= 1.0; */

	// Crafted weapon quality multiplier.
	if( weapon.quality > 1)
		basedamage += (weapon.quality - 1) * 15;
	endif
	// Damaged weapons deal reduced damage
	var hp_percent := CDbl(weapon.hp) / CDbl(weapon.maxhp);
	if (hp_percent < 0.5)
		basedamage *= hp_percent;
	endif

	if(DEBUG_MODE)
		SendSysMessage( attacker, "Basedamage 0: "+basedamage, FONT_NORMAL, 2569 );
	endif
	//======= Class Calculations ====================================== Class Calculations =======\\
	// Archery.
	if( weapon.attribute == ATTRIBUTEID_ARCHERY )
		// Stats increases damage for players, Dex already helps fire rate so STR matters more
		if(!attacker.isA(POLCLASS_NPC))
			basedamage *= 1 + GetStrength(attacker) * 0.0015;
			basedamage *= 1 + GetDexterity(attacker) * 0.0005;
		endif
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Stat Bonus: "+basedamage, FONT_NORMAL, 2569 );
		endif
		//Skill Bonuses
		if( GetObjProperty( attacker, CLASSEID_RANGER ) || GetObjProperty( attacker, CLASSEID_BARD ))
			basedamage *= 1 + GetAttribute(attacker, ATTRIBUTEID_ARCHERY) * 0.002;
		elseif( GetObjProperty( attacker, CLASSEID_THIEF ))
			basedamage *= 1 + avg_skill * 0.002;
		elseif(GetObjProperty( attacker, CLASSEID_WARRIOR ) || GetObjProperty( attacker, CLASSEID_POWERPLAYER ))
			basedamage *= 1 + GetAttribute(attacker, ATTRIBUTEID_TACTICS) * 0.002;
		endif
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Skill Bonus: "+basedamage, FONT_NORMAL, 2569 );
		endif
		// Ranger ranged class bonus from lvl 3.
		if( GetObjProperty( attacker, CLASSEID_RANGER ) )
			level := max(0, level-2);
			basedamage *= ClasseSmallBonusByLevel( level );
			if(DEBUG_MODE)
				SendSysMessage( attacker, "Ranger Bonus: "+basedamage, FONT_NORMAL, 2569 );
			endif
		endif
	// Melee
	else
		// Strength increases damage for players, npcs too often have 2k+STR!
		if(!attacker.isA(POLCLASS_NPC))
			basedamage *= 1 + GetStrength(attacker) *0.002;
		endif
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Stat Bonus: "+basedamage, FONT_NORMAL, 2569 );
		endif
		// Skill Bonuses
		if( GetObjProperty(attacker, CLASSEID_WARRIOR) || GetObjProperty(attacker, CLASSEID_POWERPLAYER))
			basedamage *=  1 + GetAttribute( attacker, ATTRIBUTEID_ANATOMY ) * 0.002 ;
		elseif( GetObjProperty( attacker, CLASSEID_THIEF ) && (weapon.attribute == ATTRIBUTEID_FENCING))
			basedamage *= 1 + avg_skill * 0.003;
		endif
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Skill Bonus: "+basedamage, FONT_NORMAL, 2569 );
		endif
		// Attacks vs Players.
		if( !defender.isA(POLCLASS_NPC) )
			if( GetObjProperty( attacker, CLASSEID_WARRIOR ))
				level := max(0, level-3);
				basedamage *= ClasseSmallBonusByLevel( level );
			elseif( level )
				level := max(0, level-4);
				basedamage *= ClasseSmallBonusByLevel( level );
			endif
			
		// Attacks vs NPC's.
		else
			if( GetObjProperty( attacker, CLASSEID_WARRIOR ) || GetObjProperty( attacker, CLASSEID_POWERPLAYER ))
				level := max(0, level-4);
				basedamage *= ClasseSmallBonusByLevel( level );
			elseif( level )
				level := max(0, level-4);
				basedamage *= ClasseSmallBonusByLevel( level );
			endif
		endif
		if(DEBUG_MODE)
				SendSysMessage( attacker, "War/PP Bonus: "+basedamage, FONT_NORMAL, 2569 );
			endif
		// Warrior class bonus reduction
		if( GetObjProperty( defender, CLASSEID_WARRIOR ) )
			// Damage reduction from NPC's.
			if( attacker.isA(POLCLASS_NPC) )
				basedamage := basedamage / ClasseBonusByLevel( defenderlevel );
				
			// Damage reduction from Players.
			else
				defenderlevel := max(0, defenderlevel-1);
				basedamage := basedamage / ClasseBonusByLevel( defenderlevel );
			endif
			if(DEBUG_MODE)
				SendSysMessage( attacker, "Warrior reduction: "+basedamage, FONT_NORMAL, 2555 );
			endif
		endif
		// Mage damage reduction.
		if( GetObjProperty( attacker, CLASSEID_MAGE ) )
			basedamage := basedamage / ClasseSmallBonusByLevel( level );
			if(DEBUG_MODE)
				SendSysMessage( attacker, "Mage reduction: "+basedamage, FONT_NORMAL, 2555 );
			endif
		endif

		// Mage increased damage received.
		if( GetObjProperty( defender, CLASSEID_MAGE ) )
			basedamage *= 1.5;
			// Warrior deals more damage towards mage.
			if( GetObjProperty( attacker, CLASSEID_WARRIOR ))
				basedamage *= 1.1;
			endif
			if(DEBUG_MODE)
				SendSysMessage( attacker, "Mage increase: "+basedamage, FONT_NORMAL, 2555 );
			endif
		endif
	endif
	//================================= Global changes ===================================\\
	// PvP slowdown.
	if(DEBUG_MODE)
		if( !defender.isA(POLCLASS_NPC) && GetGlobalProperty("ReducePhysical"))
			basedamage *= GetGlobalProperty("ReducePhysical");
		endif
	else
		if(!attacker.isA(POLCLASS_NPC))
			if( !defender.isA(POLCLASS_NPC))
				basedamage *= 0.7;
			endif
		endif
	endif
	
	// Ranged distance calculations for players only.
	if(!attacker.isA(POLCLASS_NPC))
		basedamage := DistanceCheck(attacker, defender, weapon, basedamage);
		if(DEBUG_MODE)
			SendSysMessage( attacker, "Distance: "+basedamage, FONT_NORMAL, 2569 );
		endif
	endif
	
	return basedamage;

endfunction


function MaceHit( attacker, defender, armor)

	var loss := Random(2) + 1;
	if(!attacker.isA(POLCLASS_NPC))
		if(loss > GetStamina(defender) )
			SetStamina(defender, 0);
		else
			SetStamina(defender, GetStamina(defender)-loss);
		endif
	else
		if(Random(100) < 5)
			if(loss > GetStamina(defender) )
				SetStamina(defender, 0);
			else
				SetStamina(defender, GetStamina(defender)-loss);
			endif
		endif
	endif
	//extra armor damage!
	CheckEquipmentDamage(defender, armor);

endfunction


function ModByProt( defender, basedamage )

	var physicalprot := GetObjProperty( defender , "PhysicalProtection" );
	if( !physicalprot )
		return basedamage;
	else
		basedamage *= ( 1 - physicalprot * 0.05 );
		return basedamage;
	endif

endfunction


function GetShieldAbsorption( attacker, defender, weapon, basedamage )

	var defender_level 	:= GetClasseLevel(defender);
   	var shield_defender := GetEquipmentByLayer( defender, LAYER_HAND2 );

    if( !shield_defender.IsA( POLCLASS_ARMOR ) )
    	shield_defender := GetEquipmentByLayer( defender, LAYER_HAND1 );
    endif
		
	if( shield_defender.IsA( POLCLASS_WEAPON ) )
			var weapcfg := ReadConfigFile( ":*:itemdesc" );

			if( weapcfg[shield_defender.objtype].CanWeaponParry != 0 )
				//Weapon Parry Stuff here
				var parry_chance := GetAttribute( defender, ATTRIBUTEID_PARRY );
				var parry_roll := ( RandomInt( 750 ) + 1 );
				//Award skill points even on failure
				AwardSkillPoints( defender, SKILLID_PARRY, 20 );
				parry_chance := CInt( parry_chance * ClasseBonusByLevel( defender_level ) );
				if( parry_roll <= parry_chance )
					var absorbed := 0;
					if( weapon.attribute == ATTRIBUTEID_ARCHERY )
						absorbed := CInt( weapcfg[shield_defender.objtype].WeaponParryChance / 2 );
						SendSysMessage( defender, "You successfully deflected the arrow." );
						SendSysMessage( attacker, "Your opponent deflected the arrow with their shield." );
						PerformAction( defender, ANIM_ATTACK_XBOW );
					else
						absorbed := ( weapcfg[shield_defender.objtype].WeaponParryChance / 2 );
						SendSysMessage( defender, "You successfully parry the attack." );
						SendSysMessage( Attacker, "Your opponent parried the attack." );
						PerformAction( defender, ANIM_ATTACK_XBOW );
					endif
					//damage shield chance on parry
					CheckEquipmentDamage(defender, shield_defender);

					basedamage -= absorbed;
					
					if( basedamage < 0 )
						basedamage := 0;
					endif

					if(DEBUG_MODE)
						PrintTextAbovePrivate( defender , "I parried this much damage: "+absorbed, defender );
						SendSysMessage( attacker, "Basedamage after shieldparry: "+basedamage, FONT_NORMAL, 2569 );
					endif

				endif
			endif
    elseif( shield_defender.IsA( POLCLASS_ARMOR ) )
		//Shield Parry Stuff Here
		var parry_chance := GetAttribute( defender, ATTRIBUTEID_PARRY );
		var parry_roll := ( RandomInt( 750 ) + 1 );
		//Award skill points even on failure
		AwardSkillPoints( defender, SKILLID_PARRY, 20 );
		parry_chance := CInt( parry_chance * ClasseBonusByLevel( defender_level ) );
		if( parry_roll <= parry_chance )
			var absorbed := 0;
			if( weapon.attribute == ATTRIBUTEID_ARCHERY )
				absorbed := CInt( shield_defender.ar / 2 );
				SendSysMessage( defender, "You successfully deflected the arrow." );
				SendSysMessage( attacker, "Your opponent deflected the arrow with their shield." );
				PerformAction( defender, ANIM_ATTACK_XBOW );
			else
				absorbed := ( shield_defender.ar / 2 );
				SendSysMessage( defender, "You successfully parry the attack." );
				SendSysMessage( Attacker, "Your opponent parried the attack." );
				PerformAction( defender, ANIM_ATTACK_XBOW );
			endif
			//damage shield chance on parry
			CheckEquipmentDamage(defender, shield_defender);

			basedamage -= absorbed;
			
			if( basedamage < 0 )
				basedamage := 0;
			endif

			if(DEBUG_MODE)
				PrintTextAbovePrivate( defender , "I parried this much damage: "+absorbed, defender );
				SendSysMessage( attacker, "Basedamage after shieldparry: "+basedamage, FONT_NORMAL, 2569 );
			endif

      	endif
    endif
	
	return basedamage;

endfunction


function DealDamage( attacker, defender, weapon, armor, basedamage, rawdamage )

	var noPKatk := GetObjProperty(attacker, "NOPKAREA");
	var noPKdef := GetObjProperty(defender, "NOPKAREA");

	var _any_npc := 0;
	if(attacker.isa(POLCLASS_NPC) || defender.isa(POLCLASS_NPC)) 
		_any_npc := 1;
	endif

	if((!attacker.cmdlevel && !_any_npc && defender != attacker) && (noPKatk || noPKdef))	
		attacker.setwarmode(0);
		defender.setwarmode(0);
		return;
	endif

	if (Getobjproperty(defender, "LottoDrops"))
		StoreDamage(attacker, defender, rawdamage);
	endif

	if( !GetObjProperty( weapon, "Astral" ) )
		var poisoned := GetObjProperty( weapon   , "SkillPoisoned" );
		var reactive := GetObjProperty( defender , "ReactiveArmor" );
		var onhit    := GetObjProperty( armor    , "OnHitScript" );
		var psnboost := GetClasseLevel(attacker);
		var plvl := poisoned;

		// Check if range exceeds maximum range to poison someone.
		var dist := Distance( attacker, defender );
		if(dist <= 14)
			//Check if the item was poisoned by someone
			if(GetObjProperty( weapon , "SkillPoisoned" ) /* && GetObjProperty( attacker , CLASSEID_THIEF ) */)
				var charges := CInt( GetObjProperty( weapon, "PoisonCharges" ));

				if( !charges )
					EraseObjProperty( weapon, "PoisonCharges" );
					EraseObjProperty( weapon , "SkillPoisoned" );
					return 0;
				endif
				
				if(GetObjProperty( weapon, "Cursed"))
					defender := attacker;
				endif

				if(!GetObjProperty( defender , "poison"))
					if( charges >= 1 )
						SetObjProperty( weapon, "PoisonCharges", charges - 1 );
					else
						EraseObjProperty( weapon, "PoisonCharges" );
						EraseObjProperty( weapon , "SkillPoisoned" );
					endif
				endif
				IncRevision(weapon);
				if( GetObjProperty( attacker , CLASSEID_THIEF ) )
					if( defender.isa(POLCLASS_NPC) )
						SetObjProperty( defender , "poisonnpc" , 1 );
					endif
				endif
					
				var maxx := plvl + psnboost;
				if(maxx >= 9)
					maxx := 9;
				endif
				
				if( !GetObjProperty( attacker , CLASSEID_THIEF ) ) 
					SetPoison( defender, plvl, "You have been poisoned by " + attacker.name + ".", attacker );
				else
					SetPoison( defender, maxx, "You have been poisoned by " + attacker.name + ".", attacker );
				endif
			endif
		else
			if(DEBUG_MODE)
				PrintTextAbovePrivate( attacker , "I've exceeded the range to poison someone!" , attacker );
			endif
		endif

		//Check if reactive armor spell is on
		if( reactive )
			EraseObjProperty( defender , "ReactiveArmor" );
			start_script( ":combat:reactivearmoronhit" , { attacker, defender, basedamage, rawdamage, reactive} );
		endif

		//Check if the armor have an on hit script and launch it
		if( onhit )
			start_script( onhit , {attacker, defender, weapon, armor, basedamage, rawdamage} );
		else
			ApplyTheDamage( defender , attacker , rawdamage );
		endif
	else
		ApplyTheAstralDamage( defender , attacker , rawdamage );
	endif

endfunction


function DestroyEquipment( who, item )

	// Wrestling, core error 
	if (item.objtype == 0x1F020)
		return 0;
	endif

	var cursed := GetObjProperty( item , "Cursed" );

	if( cursed )
		SetObjProperty( item , "Cursed" , 3 );
	endif

	if( who.backpack )
		MoveItemToContainer( item , who.backpack );
	endif

	if( !who.isa(POLCLASS_NPC) )
		SendSysMessage(who, "Your "+item.desc+" has been destroyed!", FONT_NORMAL, 2595);
	else
		if( item.attribute )
			PrintTextAbove( who , who.name + " seems exhausted and can barely hit anyone anymore!" );
		elseif(item.ar)
			PrintTextAbove( who , who.name + "'s defenses have been penetrated, making it easier to hit vital parts!" );
		endif
	endif

	DestroyItem( item );

endfunction


function DistanceCheck(attacker, defender, weapon, basedamage)

	if( weapon.attribute == ATTRIBUTEID_ARCHERY )
		// Range limit exceeded = 1/4th damage
		var dist := Distance( attacker, defender );
		if( (dist < 1) || (dist > 14 ) )
			basedamage *= 0.25;
		elseif(dist == 1)
			basedamage *= 0.5;
		endif	
	endif

	return basedamage;

endfunction


function ArAbsorptionCalc( ar, basedamage, divider := 5, multiplier := 0.05, exponent := 0.5 )

    var percent := Pow(( ar / divider ), exponent) * multiplier;

    if( percent >= 1.0 )
    	percent := 0.99;
    endif
	
    var absorbed := basedamage * percent;

    return absorbed;

endfunction

function CheckEquipmentDamage(who, item);
	if(!item)
		return 0;
	endif

	var equip_elem;
	if (item.isa(POLCLASS_WEAPON))
		equip_elem := combat_settingscfg["Weapons"];
	elseif (item.layer <3)
		equip_elem := combat_settingscfg["Parry"];
	else
		equip_elem := combat_settingscfg["Armor"];
	endif

	if (RandomInt(100) < equip_elem.WearChance )
		item.hp -= 1;
		IncRevision(item);
		if ( item.hp <= 1 )
			DestroyEquipment( who, item );
		endif
	endif

endfunction

