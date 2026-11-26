//Edited August 25, 2024

use uo;
use os;
use npc;
use cfgfile;

include "include/attributes";
include "include/random";
include ":karmafame:karmafame";
include "include/charm";

var modnpc := Self();

if( !GetStrengthMod(modnpc) )
	var strmod := GetObjProperty( modnpc , "BaseStrmod" );
	if (strmod)
		SetStrengthMod(modnpc, strmod);
		if (GetObjProperty(modnpc, "CustomHitsLevel"))
			SetHP(modnpc, GetMaxHP(modnpc));
		else
			SetHP(modnpc, GetStrength(modnpc));
		endif
	endif
endif


if( !GetIntelligenceMod(modnpc) )	
	var intmod := GetObjProperty( modnpc , "BaseIntmod" );
	if (intmod)
		SetIntelligenceMod(modnpc,  intmod);
		SetMana(modnpc, GetIntelligence(modnpc));
	endif
endif


if( !GetDexterityMod(modnpc) )
	var dexmod := GetObjProperty( modnpc , "BaseDexmod" );
	if (dexmod)
		SetDexterityMod(modnpc, dexmod);
		SetStamina(modnpc, GetDexterity(modnpc));
	endif
endif


// Regeneration from the template's BaseHpRegen / BaseManaRegen / BaseStaminaRegen CProps
// (units: x12 hundredths a minute, 100 = normal; see the npcdesc.cfg header), read live from
// npcdesc.cfg. Runs on every AI start, so a changed template or a stale rate is corrected,
// tamed pets included. HP is left alone while the NPC is poisoned: poison sets its own
// rate and CurePoison restores this one.
if( !modnpc.poisoned )
	SetVitalRegenRateOrDefault( modnpc, VITALID_LIFE, NpcTemplateRegenRate( modnpc, "BaseHpRegen" ) );
endif
SetVitalRegenRateOrDefault( modnpc, VITALID_MANA, NpcTemplateRegenRate( modnpc, "BaseManaRegen" ) );
SetVitalRegenRateOrDefault( modnpc, VITALID_STAMINA, NpcTemplateRegenRate( modnpc, "BaseStaminaRegen" ) );

// A wild NPC still marked by Control Undead / Spell Bind: before 1.1.6 Control Undead left
// the caster as the core master of the freed creature, so its attacks were reported (and
// flagged) as the caster's, and "charmed" stayed on it. The tamed AI ends a live charm itself.
if( modnpc.script != "tamed" and IsSpellCharmed( modnpc ) )
	modnpc.setmaster( 0 );
	EraseObjProperty( modnpc, PROPID_CHARM );
	EraseObjProperty( modnpc, "charmed" );
	EraseObjProperty( modnpc, "master" );
endif

// This runs on every AI start. A Warrior for Hire dresses itself once (SetMeUp) and keeps what its
// master gives it, so re-equipping its template here would hand out free items on each restart.
if( !GetObjProperty(modnpc, "WarriorForHire") )
	var equip_cfg := ReadConfigFile(":*:equip");
	if( equip_cfg )
	var npccfg := readconfigfile("::npcdesc");
		var equip_elem := FindConfigelem(equip_cfg, npccfg[modnpc.npctemplate].equip);
		if( equip_elem )
			EquipFromTemplate(modnpc, npccfg[modnpc.npctemplate].equip);
		endif
	endif
endif


// Karma and fame come from the template's alignment / Karma / Fame fields (npcdesc.cfg header).
// Kill awards read the template directly; these CProps are kept for the other checks that
// read an NPC's karma or fame. Set on every AI start so template changes take effect.
var kf_values := NpcTemplateKarmaFame( modnpc.npctemplate );
if( kf_values )
	if( CInt(GetObjProperty( modnpc , "Karma" )) != kf_values.karma )
		SetObjProperty( modnpc , "Karma" , kf_values.karma );
	endif
	if( CInt(GetObjProperty( modnpc , "Fame" )) != kf_values.fame )
		SetObjProperty( modnpc , "Fame" , kf_values.fame );
	endif
endif


if( !GetObjProperty( modnpc , "AiSerial" ) or GetObjProperty( modnpc , "AiSerial" ) != modnpc.serial )
	SetObjProperty( modnpc , "AiSerial" , modnpc.serial );
endif


if( GetProperty( "Anchor" ) )
	var anchor := GetProperty( "Anchor" );
	SetAnchor( cint(anchor.x), cint(anchor.y), cint(anchor.range), cint(anchor.psub) );
endif