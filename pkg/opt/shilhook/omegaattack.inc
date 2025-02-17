//Edited September 6, 2024

use uo;
use os;
use util;
use cfgfile;

include "include/classes";
include ":combat:hitscriptinc";
include ":armor:armorZones";
include "include/random";

function OmegaAttack(attacker, defender)

	//var attackerweaponType := cfg[attacker.weapon.objtype];

	var weapfile := ReadConfigFile( ":combat:itemdesc" );
	var attackerweaponType := weapfile[attacker.weapon.objtype];

//	var listofanim	:=	GetConfigIntArray( attackerweaponType, "Anim" );

//	Print($"listofanim " +listofanim);

//	var Anim := listofanim[(Random(3)+1)];
//	Print($"animation " +Anim);

	if(attacker.weapon.attribute == ATTRIBUTEID_ARCHERY)
		if (!checkForAndDecrementAmmunition( attacker, attackerweaponType))	
			return;
		endif
		if (!ValidArcheryWeapon(attackerweaponType))
			return;
		endif
		PerformAction( attacker, attackerweaponType.Anim);
		PlayArcheryAnimation(attacker, defender, attackerweaponType);
		if(CheckHitChance(attacker, defender))
			if(attackerweaponType.HitSound)
				PlaySoundEffect( attacker, attackerweaponType.ProjectileSound );
				PlaySoundEffect( defender, attackerweaponType.ProjectileSound );
			endif
		else
			return;
		endif
	else
		PerformAction( attacker, attackerweaponType.Anim);
		if(CheckHitChance(attacker, defender))
			if(attackerweaponType.HitSound)
				PlaySoundEffect( attacker, attackerweaponType.HitSound );
				PlaySoundEffect( defender, attackerweaponType.HitSound );
			endif
		else
			if(attackerweaponType.MissSound)
				PlaySoundEffect( attacker, attackerweaponType.MissSound );
				PlaySoundEffect( defender, attackerweaponType.MissSound );
				return;
			endif
		endif
	endif

	var hitscript := attacker.weapon.hitscript;

	// This is currently spamming the staff. Not sure how it's triggered though.
 	if(!hitscript)
		// Broadcast to staff
		var msg := "Can't find a weapon hitscript: " + hitscript +"From attacker: "+ attacker.name + " using item: "+attackerweaponType;
		foreach staff in EnumerateOnlineCharacters()
			if( staff.cmdlevel >= 4 )
				SendSysMessage( staff, msg, 3, 2581 );
			endif
		endforeach
		return;
	endif 
	//now we're sure there's a hit so check for damage on the weapon
	CheckEquipmentDamage(attacker, attacker.weapon);

	var weapDMG := attackerweaponType.damage;
	if(!weapDMG)
		weapDMG := "1d1";
	endif

	var wbasedamage:= RandomDiceRoll( weapDMG ); 
	if(attacker.weapon.dmg_mod)
		wbasedamage += attacker.weapon.dmg_mod;
	endif

	//find random piece of armor to hit -- need this for onhit script and damage
	var def_armor:=GetHitArmor(defender);	
	
	start_script(hitscript, {attacker, defender, attacker.weapon, def_armor, wbasedamage, 0});
	
endfunction


function GetLowestClassSkillValue(who)
	var classe := GetClass(who);
	var skills := GetClasseSkills(classe);
	var lowest_skill := 0;

	foreach s in (skills)
		var sval := GetEffectiveSkill(who, s);
		if (lowest_skill == 0)
			lowest_skill := sval;
		elseif (sval < lowest_skill)
			lowest_skill := sval;
		endif
	endforeach

	return lowest_skill;

endfunction


function CheckHitChance(attacker, defender)

	var skill_factor := 214.285714285714;
	var class_factor := 0.0238;
	var hunger_factor := 0.05;
	var min_hit_chance := 0.1;
	var weapon := attacker.weapon;
	var wep_attr := attacker.weapon.attribute;
	var wep_skill := GetSkillIdByAttributeId(wep_attr);
	var hunger := CDBl(GetObjProperty(attacker, "hunger"));
	var class_level := CDbl(GetClasseLevel(attacker));
	var attacker_wep_skill;
	//award skill points regardless of hit or miss
	CombatAdvancement( attacker, weapon, defender );
	if(GetObjProperty( attacker , CLASSEID_THIEF ) || GetObjProperty( attacker , CLASSEID_MAGE) || GetObjProperty( attacker , CLASSEID_BARD))
		attacker_wep_skill := CDbl(GetLowestClassSkillValue(attacker));
	else 
		attacker_wep_skill := CDbl(GetEffectiveSkill(attacker, wep_skill));
	endif

	var class_chance := 1 + (class_level * class_factor);
	var skill_chance := attacker_wep_skill / skill_factor;
	var hunger_penalty := 1 - (hunger * hunger_factor);

	var hit_chance := skill_chance * class_chance * hunger_penalty;
	var chance := RandomFloat(1.0);

	if (hit_chance < min_hit_chance)
		hit_chance := min_hit_chance;
	endif
	
	if( chance < hit_chance )
		return 1;
	else 
		return 0;
	endif
	
endfunction


function ValidArcheryWeapon(weaponType)

	//returns true if itemdesc elem contains the miniumum required props for processing
	if(!weaponType.ProjectileAnim || !weaponType.Anim || !weaponType.hitSound || !weaponType.projectileSound ||
		!weaponType.hitscript || !weaponType.damage || !weaponType.Projectile || !weaponType.ProjectileType )
		return 0;
	endif

	return 1;

endfunction


function checkForAndDecrementAmmunition( who, weaponType )

	var mypack := EnumerateItemsInContainer( who.backpack );
	var expotions := {0xdc08, 0xdc09, 0xdc07, 0xff2f, 0xff30, 0xff31, 0xff32, 0xff33, 0xff34, 0xff35, 0xff36};
	var type;

	foreach item in mypack
		if( item.objtype == weaponType.ProjectileType )
			if(SubtractAmount( item, weaponType.Projectile))
				return 1;
			endif
		elseif ( weaponType.ProjectileType == 1 && item.objtype in expotions)
			type := item.objtype;
			if(SubtractAmount( item, weaponType.Projectile))
				SetObjProperty(who, "#LastExplosionShot", type);
				return 1;
			endif
		endif
	endforeach

	return 0;

endfunction

function PlayArcheryAnimation(attacker, defender, attackerweaponType)
	var animation := attackerweaponType.ProjectileAnim;
	var color := 0;

	// Cains crossbow special!
	if (GetObjProperty(attacker.weapon, "#Activated") && attacker.weapon.objtype == 0xda15)
		color := 1304;
		animation := 0x183e;
	endif

	PlayMovingEffectXYZEx( attacker.x, attacker.y, attacker.z+9, defender.x, defender.y, defender.z+5, defender.realm, animation, 2, 0, color);
endfunction

function CombatAdvancement( attacker, weapon, defender )
	
	AwardSkillPoints( attacker, GetSkillIdByAttributeId( weapon.attribute ), 20 );

	// Don't raise tactics with archery
	if (weapon.attribute != ATTRIBUTEID_ARCHERY)
		AwardSkillPoints( attacker, SKILLID_TACTICS, 20 );
	endif

endfunction

function GetHitArmor(who)

	var hit_zone := CS_GetRandomArmorZone();
	var armor_hit := CS_GetEquipmentInArmorZone(who, hit_zone);
	
	if ( armor_hit.Size() < 1 )
		return 0;
	endif

	var best_armor := struct;
	best_armor.name := "nothing-"+hit_zone;
	best_armor.desc := "nothing"+hit_zone;
	best_armor.ar := 0;
		
	foreach item in ( armor_hit )
		if ( item.ar > best_armor.ar )
			best_armor := item;
		endif
		SleepMS(2);
	endforeach
	if (best_armor.ar)
		return best_armor;
	else
		return 0;
	endif

endfunction