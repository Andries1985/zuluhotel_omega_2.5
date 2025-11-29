//Edited August 25, 2024

use uo;
use os;
use guilds;
use vitals;

const DMGID_FIRE	:= 0x0001;
const DMGID_AIR		:= 0x0002;
const DMGID_EARTH	:= 0x0004;
const DMGID_WATER	:= 0x0008;
const DMGID_NECRO	:= 0x0010;
const DMGID_HOLY	:= 0x0020;
const DMGID_POISON	:= 0x0040;
const DMGID_ACID	:= 0x0080;
const DMGID_PHYSICAL	:= 0x0100;
const DMGID_MAGIC	:= 0x0200;
const DMGID_ASTRAL	:= 0x0400;
const DMGID_NO_RESIST	:= 0x0800;

//Removed Includes
//Moved Stored Damage function to Damages include
//include "include/storedamage";

include "include/attributes";
//include "include/constants";
include ":karmafame:karmafame";
//include "include/mobileutil";
include "include/astralfights";
//include ":shilhook:shilcombat";
//include "include/areas";
//include "include/classes";
//include "include/client";


const MAX_SERIAL_IN_HITLIST := 5;

function ApplyTheDamage( who, by_who, dmg, attack_type := DMGID_PHYSICAL )

	dmg := Cint(dmg);
	if( !who.IsA(POLCLASS_MOBILE) )
		return 0;
	elseif( dmg <= 0 )
		return 0;
	elseif( who.dead )
		return 0;
	elseif( IsImmunedFromThisDamageType( who, dmg, attack_type ) )
		return 0;
	endif

	if (Getobjproperty(who, "LottoDrops"))
		StoreDamage(by_who, who, dmg);
	endif
	
	var factor := 1.0;
	if( by_who.IsA(POLCLASS_NPC) )
		var master := FindMaster(by_who);
		if( master )
			by_who := master;
			factor := factor * VALUE_MULTIPLIER_FOR_TAMED;
		endif
	endif
	
	if( dmg < GetHP(who) )
		factor := factor*VALUE_MULTIPLIER_FOR_ALLIES;
	else
		if( GetObjProperty(who,PROPID_MOBILE_RISE) )
			if( TryToInflictFinalDeath( who, attack_type ) )
				SetObjProperty( who, PROPID_MOBILE_KILLED_BY, by_who.name );
				SetObjProperty( who, "KilledBySerial", by_who.serial );
			else
				factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
			endif
		else
			SetObjProperty( who, PROPID_MOBILE_KILLED_BY, by_who.name );
			SetObjProperty( who, "KilledBySerial", by_who.serial );
		endif
	endif
	
	if( !by_who )
		if(DEBUG_MODE)
			PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
			PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
		endif
		return ApplyRawDamage( who, dmg );
	elseif( !by_who.IsA(POLCLASS_MOBILE) )
		if(DEBUG_MODE)
			PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
			PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
		endif
		return ApplyRawDamage( who, dmg );
		if(DEBUG_MODE)
			PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
			PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
		endif
	elseif( by_who == who )
		return ApplyRawDamage( who, dmg );
	endif
	
	SetScriptController(by_who);
	
	if( !by_who.IsA(POLCLASS_NPC) )
		if( !by_who.cmdlevel )
			// The defender is a npc
			if( who.IsA(POLCLASS_NPC) )
				GenerateHitlist( who, by_who.serial, factor );
			else
				var guild1 := GetObjProperty( by_who, "guild_id" );
				if( guild1 )
					var guild2 := GetObjProperty( who, "guild_id" );
					if( guild2 )
						if( guild1 == guild2 )
							if(DEBUG_MODE)
								PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
								PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
							endif
							return ApplyRawDamage( who, dmg );
						else
							guild1 := FindGuild( guild1 );
							guild2 := FindGuild( guild2 );
							if( guild1.IsEnemyGuild(guild2) )
								if(DEBUG_MODE)
									PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
									PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
								endif
								return ApplyRawDamage( who, dmg );
							elseif( guild1.IsAllyGuild(guild2) )
								if(DEBUG_MODE)
									PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
									PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
								endif
								return ApplyRawDamage( who, dmg );
							endif
						endif
					endif
				endif
				
				var defending;
				foreach entry in (by_who.reportables)
					if( who.serial == entry.serial )
						// The attacker is defending himself
						defending := 1;
					endif
				endforeach
				
				if( defending )	
					if( GetKarma(who) < GetKarma(by_who) )
						// Add to hitlist only if the target have less karma
						GenerateHitlist( who, by_who.serial, factor );
					endif
				else
					// The attacker is the agressor
					GenerateHitlist( who, by_who.serial, factor );
				endif
			endif
		endif
	endif
	
	if(DEBUG_MODE)
		PrintTextAbovePrivate( by_who , "I've dealt this much damage: " +dmg, by_who );
		PrintTextAbovePrivate( who , "I received this much damage: " +dmg, who );
	endif

	return ApplyRawDamage( who, dmg );

endfunction

function ApplyTheAstralDamage( who, by_who, dmg, attack_type := DMGID_ASTRAL )

	dmg := Cint(dmg);
	if( dmg <= 0 )
		return 0;
	elseif( who.dead )
		return 0;
	elseif( IsImmunedFromThisDamageType( who, dmg, attack_type ) )
		return 0;
	endif
	
	var factor := 1.0;	
	var master := FindMaster(by_who);
	if( master )
		by_who := master;
		factor := factor * VALUE_MULTIPLIER_FOR_TAMED;
	endif
	
	if( dmg <= who.mana )
		factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
		SetMana(who, GetMana(who)-dmg); 
	else
		dmg := dmg - GetMana(who);
		who.mana := 0;
		
		if( dmg <= GetStamina(who) )
			factor := factor * VALUE_MULTIPLIER_FOR_ALLIES;
			SetStamina(who, GetStamina(who)-dmg);
		else
			if( GetObjProperty( who, PROPID_MOBILE_RISE ) )
				TryToInflictFinalDeath( who, attack_type );
			endif
		
			who.stamina := 0;
			SetAstralIncapacity( who );
		endif
	endif
	
	if( !by_who )
		return 1;
	elseif( by_who.IsA(POLCLASS_NPC) )
		return 1;
	elseif( by_who.cmdlevel )
		return 1;
	else
		if( IsSummoned(who) || IsAnimated(who) || IsCharmed(who) )
			master := FindMaster(who);
			if( master )
				GenerateHitlist( master, by_who.serial, factor );
			else
				GenerateHitlist( who, by_who.serial, factor );
			endif
		else
			GenerateHitlist( who, by_who.serial, factor );
		endif

		return 1;
	endif

endfunction

function GenerateHitlist( who, serial, factor )

	var hitlist := GetObjProperty( who, PROPID_MOBILE_HITLIST );
	if( !hitlist )
		hitlist := array;
	endif
	
	if( len(hitlist) > MAX_SERIAL_IN_HITLIST )
		ReplaceOldestInHitlist( hitlist, serial, factor );
	else
		AddSerialToHitlist( hitlist, serial, factor );
	endif
	
	SetObjProperty( who, PROPID_MOBILE_HITLIST, hitlist );
	
endfunction

function ReplaceOldestInHitlist( byref hitlist, serial, factor )

	var min_timer	:= ReadGameClock();
	var min_pos	:= 0;
	var current;
	
	for( current := 1; current <= len(hitlist); current := current + 1 )
		if( hitlist[current][1] == serial )
			hitlist[current][3] := ReadGameClock();
			if( hitlist[current][2] < factor )
				hitlist[current][2] := factor;
			endif
			return hitlist;
			
		elseif( hitlist[current][3] < min_timer )
			min_timer := hitlist[current][3];
			min_pos   := current;
		endif
	endfor
	
	if( min_pos )
		hitlist[min_pos][1]	:= serial;
		hitlist[min_pos][2]	:= factor;
		hitlist[min_pos][3]	:= ReadGameClock();
	endif
	
	return 1;
	
endfunction

function AddSerialToHitlist( byref hitlist, serial, factor )

	foreach arr in hitlist
		if( arr[1] == serial )
			arr[3] := ReadGameClock();
			if( arr[2] < factor )
				arr[2] := factor;
			endif
			return 1;
		endif
	endforeach
	
	var add	:= { serial, factor, ReadGameClock() };
	return hitlist.append(add);
	
endfunction

function IsImmunedFromThisDamageType( who, byref dmg, attack_type )

	var immunities := GetObjProperty( who, PROPID_MOBILE_ATTACK_TYPE_IMMUNITIES );
	if( !immunities )
		return 0;
	endif
	
	if( attack_type & DMGID_NO_RESIST )
		return 0;
	endif

	var attack_amount	:= 0;
	var is_immuned_amount	:= 0;      
	foreach bit in { DMGID_FIRE, DMGID_AIR, DMGID_EARTH, DMGID_WATER, DMGID_NECRO, DMGID_HOLY, DMGID_POISON, DMGID_ACID, DMGID_PHYSICAL, DMGID_MAGIC, DMGID_ASTRAL }
		if( attack_type & bit )
			attack_amount := attack_amount + 1;
			if( immunities & bit )
				is_immuned_amount := is_immuned_amount + 1;
			endif
		endif
	endforeach
	
	if( !attack_amount || attack_amount == is_immuned_amount )
		return 1;
	else
		dmg := Cint( dmg * Cint(attack_amount-is_immuned_amount) / attack_amount );
		return 0;
	endif
	
endfunction

function TryToInflictFinalDeath( who, attack_type )
	
	var final_death := GetObjProperty(who, PROPID_MOBILE_FINAL_DEATH);
	if( !final_death )
		return 0;
	endif
	
	final_death := Cint(final_death);
	foreach bit in { DMGID_FIRE, DMGID_AIR, DMGID_EARTH, DMGID_WATER, DMGID_NECRO, DMGID_HOLY, DMGID_POISON, DMGID_ACID, DMGID_PHYSICAL, DMGID_MAGIC, DMGID_ASTRAL }
		if( final_death & bit )
			if( !(attack_type & bit) )
				return 0;
			endif
		endif
	endforeach
	
	return 1;
	
endfunction

function FindMaster( byref mobile )

	var serial := Cint( GetObjProperty( mobile, PROPID_MOBILE_MASTER_SERIAL ) );
	if( !serial )
		return 0;
	endif

	return FindMobile( serial );
	
endfunction

function FindMobile( byref serial )

	if( !serial )
		return 0;
	endif
	
	var mobile := SystemFindObjectBySerial( serial );
	
	if( !mobile )
		mobile := SystemFindObjectBySerial( serial, SYSFIND_SEARCH_OFFLINE_MOBILES );
	endif
	
	return mobile;
	
endfunction

function IsAnimated( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_ANIMATED );
	
endfunction

function IsCharmed( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_CHARMED );
	
endfunction

function IsSummoned( byref mobile )

	return GetObjProperty( mobile, PROPID_MOBILE_SUMMONED );

endfunction

function StoreDamage(attacker, defender, damage)

	var attacker_list := GetObjProperty(defender, "AttackerList");

	if (!attacker_list)
		attacker_list := dictionary;
	endif

	var attacker_serial := CInt(attacker.serial);
	
	if (!attacker_list.Exists(attacker_serial))
		attacker_list[attacker_serial] := damage;
	else
		attacker_list[attacker_serial] += damage;
	endif
	SetObjProperty(defender, "AttackerList", attacker_list);

	if (!GetObjProperty(defender, "MyMaxHP"))
		SetObjProperty(defender, "MyMaxHP", GetMaxHP(defender));
	endif

endfunction