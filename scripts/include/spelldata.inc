//////////////////////////////////////////////////////////////////////////////////
//										//
//	spelldata.inc : 							//
//										//
//	This file include all the constant related to existing spells, as	//
//	well as useful fuction about spells. The first line of each spell	//
//	is the spellid, then his SFX anf finally his FX				//
//										//
//////////////////////////////////////////////////////////////////////////////////
use uo;
use os;
use util;
use basic;
use cfgfile;
use polsys;

include "include/client";
include "include/random";
include "include/eventid";
include "include/classes";
include "include/damages";
include "include/areas";
include "include/skillpoints";
include "include/attributes";
include "include/difficulty";
include ":lootlottery:storedamage";

////////////////////////////////
// Spell functions' constants //
////////////////////////////////


// TryToCast return values
const IN_ANTIMAGIC_AREA := 1;
const INTERRUPTED       := 2;
const INVALID_EQUIPMENT := 3;
const NO_REGS           := 4;
const NO_MANA           := 5;
const NO_SKILL          := 6;
const SUCCESS           := 7;


// Casting options
const CASTINGOPT_PROP_NAME      := "CastingOpts";
const PERM_CASTINGOPT_PROP_NAME := "PermCastingOpts";
const CASTINGOPT_ANY_EQUIPMENT  := "ANYEQUIP";
const CASTINGOPT_NO_WORDS       := "NOWORDS";
const CASTINGOPT_NO_DELAY       := "NODELAY";
const CASTINGOPT_NO_REAGENTS    := "NOREGS";
const CASTINGOPT_NO_MANA        := "NOMANA";
const CASTINGOPT_NO_SKILL_CHECK := "NOSKILL";
const CASTINGOPT_SCROLL_SKILL_GAIN := "SCROLLGAIN";
const CASTINGOPT_USE_SCROLL 	:= "USESCROLL";

// CalcSpellDamage options
const SINGLE_TARGET_SPELL := 0;
const AREA_EFFECT_SPELL   := 1;

const SPELL_DAMAGES_MAGERY_DIVIDER    := 5;
const SPELL_DAMAGES_ON_PLAYER_DIVIDER := 3;
const SPELL_DAMAGES_CIRCLE_MULTIPLIER := 3; // number of dices to throw per circle
const SPELL_DAMAGES_DICE_TYPE         := 5; // type of dices to throw


// Immunities constants
const IMMUNED	:= 1;
const CURSED	:= 2;


// Elements contants
const FIRE	:= 0x01;
const AIR	:= 0x02;
const EARTH	:= 0x04;
const WATER	:= 0x08;
const NECRO	:= 0x10;
const HOLY	:= 0x20;


// Error messages
const ERROR_MSG_ANTI_MAGIC_AREA		:= "Spells cannot be cast here.";
const ERROR_MSG_FIZZLE				:= "The spell fizzles.";
const ERROR_MSG_INTERRUPTED			:= "You were interrupted during the casting.";
const ERROR_MSG_LOST_CONCENTRATION	:= "You've lost your concentration!";
const ERROR_MSG_NOT_ENOUGH_REAGENTS	:= "Insufficient Reagents.";
const ERROR_MSG_NOT_ENOUGH_MANA		:= "Insufficient Mana.";
const ERROR_MSG_NOT_ENOUGH_SKILL	:= "This spell is above your magical abilities.";
const ERROR_MSG_NOT_IN_SIGHT		:= "You can't see that.";
const ERROR_MSG_WRONG_EQUIPMENT		:= "Your hands must be free to cast spells.";


//////////////////////////////////////////////////////////////////////////////////////////////////
// Spell functions :										//
//	This include all functions from spellrestrictions.inc, plus some more.			//
//	Don't erase the spellrestrictions however, unless you are certain that you changed	//
//	include "include/spellrestrictions" from ALL pkgs that used it.	Some parameters of	//
//	the functions changed however.								//
//////////////////////////////////////////////////////////////////////////////////////////////////


function ApplyElementalDamage( caster, cast_on, circle, dmg, element )

	if(caster.dead) return; endif
	if(cast_on.dead) return; endif
	if(cast_on.hidden) return; endif
	var noPKatk := GetObjProperty(caster, "NOPKAREA");
	var noPKdef := GetObjProperty(cast_on, "NOPKAREA");
	if((!caster.cmdlevel && cast_on.isa(POLCLASS_MOBILE) && !cast_on.isa(POLCLASS_NPC) && cast_on != caster) && (noPKatk || noPKdef))
		return;
	endif

	var resultprot := GetResultingElementalProt( cast_on, element );
	
	if( resultprot < 100 )
		if( GetObjProperty( caster , CLASSEID_MAGE ) )
			var level := GetClasseLevel(caster);
			resultprot := resultprot - (level * 3);
			resultprot := Cint(dmg * resultprot / 100);
			dmg := dmg - resultprot;	
		else
			dmg := dmg - Cint(dmg * resultprot / 100);	
		endif

		ApplyTheDamage( cast_on , caster , Resisted( caster, cast_on, circle, dmg ) , DMGID_MAGIC + element );
	else
		// Toadstool - heals x% of the damage that;s over 100. so 120% protection heals 20% of the damage
		var amt := CDbl(resultprot)/100.0;
		HealDamage( cast_on , Cint(dmg * CDbl(amt-1)) );
	endif

endfunction


function ApplyElementalDamageNoResist( on_who, by_who, dmg, element )

	if(by_who.dead) return; endif
	if(on_who.dead) return; endif
	if(on_who.hidden) return; endif
	var noPKatk := GetObjProperty(by_who, "NOPKAREA");
	var noPKdef := GetObjProperty(on_who, "NOPKAREA");
	if((!by_who.cmdlevel && on_who.isa(POLCLASS_MOBILE) && !on_who.isa(POLCLASS_NPC) && on_who != by_who) && (noPKatk || noPKdef))
		return;
	endif

	var resultprot := GetResultingElementalProt( on_who, element );

	if( resultprot < 100 )
		dmg := dmg - Cint(dmg * resultprot / 100);	
		ApplyTheDamage( on_who , by_who ,dmg, element );
	else
		// Toadstool - heals x% of the damage that;s over 100. so 120% protection heals 20% of the damage
		var amt := CDbl(resultprot)/100.0;
		HealDamage( on_who , Cint(dmg * CDbl(amt-1)) );
	endif
	
endfunction


function ApplyPlanarDamage( caster, cast_on, circle, dmg, plane )

	if(caster.dead) return; endif
	if(cast_on.dead) return; endif
	if(cast_on.hidden) return; endif
	var noPKatk := GetObjProperty(caster, "NOPKAREA");
	var noPKdef := GetObjProperty(cast_on, "NOPKAREA");
	if((!caster.cmdlevel && cast_on.isa(POLCLASS_MOBILE) && !cast_on.isa(POLCLASS_NPC) && cast_on != caster) && (noPKatk || noPKdef))
		return;
	endif

	var resultprot := GetResultingPlaneProtection( cast_on, plane );

	if( resultprot < 100 )
		dmg := dmg - Cint(dmg * resultprot / 100);	
		ApplyTheDamage( cast_on , caster , Resisted( caster, cast_on, circle, dmg ) , DMGID_MAGIC + plane );
	else
		// Toadstool - heals x% of the damage that;s over 100. so 120% protection heals 20% of the damage
		var amt := CDbl(resultprot)/100.0;
		HealDamage( cast_on , Cint(dmg * CDbl(amt-1)) );
	endif
	
endfunction


function BurnSelf ( caster , circle )

	if(caster.dead) return; endif

	var dmg;
	var cast_on := caster;
        dmg := CalcSpellDamage( caster , cast_on , circle );
        if( GetObjProperty( caster , CLASSEID_MAGE ) )
        	dmg := Cint( dmg / ClasseBonus( caster , CLASSEID_MAGE ) );
	  elseif( GetObjProperty( caster , CLASSEID_CLERIC ) )
        	dmg := Cint( dmg / ClasseBonus( caster , CLASSEID_CLERIC ) );
        elseif( GetObjProperty( caster , CLASSEID_WARRIOR ) )
        	dmg := Cint( dmg * ClasseBonus( caster , CLASSEID_WARRIOR ) );
        endif

	ApplyRawDamage( caster , Resisted( caster , circle , cast_on , Cint(dmg / 2) ) );

endfunction


function CalcSpellDamage( caster, cast_on, circle, area_spell := SINGLE_TARGET_SPELL )

	if(caster.dead) return 0; endif
	if(cast_on.dead) return 0; endif
	if(cast_on.hidden) return 0; endif
	var noPKatk := GetObjProperty(caster, "NOPKAREA");
	var noPKdef := GetObjProperty(cast_on, "NOPKAREA");
	if((!caster.cmdlevel && cast_on.isa(POLCLASS_MOBILE) && !cast_on.isa(POLCLASS_NPC) && cast_on != caster) && (noPKatk || noPKdef))
		return 0;
	endif

	if( area_spell ) circle := circle-3; endif

	if( circle < 1 ) circle := 1; endif

	var dmg := RandomDiceRoll(
		 circle * SPELL_DAMAGES_CIRCLE_MULTIPLIER+"d"+
		 SPELL_DAMAGES_DICE_TYPE+"+"+
		 CInt( GetEffectiveSkill(caster, SKILLID_MAGERY) / SPELL_DAMAGES_MAGERY_DIVIDER )
	) ;

	if( dmg > CInt(circle * (13 + circle)) ) dmg := CInt(circle * (13 + circle)); endif

	dmg := ModifyWithMagicEfficiency( caster, dmg );

	if( !cast_on.npctemplate ) dmg := CInt(dmg / SPELL_DAMAGES_ON_PLAYER_DIVIDER); endif
	if(cast_on.npctemplate) 
		dmg := CInt(dmg*1.5);
	endif

	return dmg;

endfunction

function colorstuff(who,ncolor)

	var item,i,armor:={0x03,0x04,0x05,0x06,0x07,0x0a,0x0c,0x0d,0x0e,0x11,0x13,0x14,0x16,0x17,0x18};

	for(i:=1;i<=15;i:=i+1)
		item:=getequipmentbylayer(who,armor[i]);
		if (ncolor>=0)
			item.color:=ncolor;
			setobjproperty(item,"X6",1);
		else
			destroyitem(item);
		endif
	endfor

endfunction


function can_cast( me, circle )

	if(me.dead) return 0; endif

	var max_circle := GetMaxCircle( me );
	if( max_circle )
		if( circle > max_circle )
			PlayObjectCenteredEffect( me , FX_SPELL_FIZZLE , SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
			PlaySoundEffect( me, SFX_SPELL_FIZZLE );
			SendSysMessage( me, ERROR_MSG_NOT_ENOUGH_SKILL );
			return 0;
		endif
	endif

	return 1;

endfunction


function CanTargetArea( caster, circle := 1 )

	if (!can_cast(caster, circle))
        	return 0;
	endif

	var cx := caster.x;
	var cy := caster.y;
	var shp := GetHp(caster);

	SetObjProperty(caster, "#Casting", 1);
	Start_script( ":summoning:checkmove", caster );

	var cast_loc := TargetCoordinates( caster );
	if( !cast_loc.x )
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif

	if( (cx != caster.x) or (cy != caster.y) or (shp > GetHP(caster)) || GetObjProperty(caster, "#SpellMove") )
		SendSysMessage( caster, ERROR_MSG_LOST_CONCENTRATION );
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif

	if( !CheckLosAt( caster, cast_loc.x, cast_loc.y, cast_loc.z ) )
        SendSysMessage( caster, ERROR_MSG_NOT_IN_SIGHT );
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif
	
	/*
	if( IsInNOPKArea(cast_loc) )
        SendSysMessage( caster, "Can't cast area of effect spells in NO-PK zone." );
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif
	*/
	if(GetObjProperty(caster, "#SpellMove"))
		EraseObjProperty(caster, "#SpellMove");
	endif
	EraseObjProperty(caster, "#Casting");
	return cast_loc;

endfunction


function CanTargetSpell( caster, circle := 1 , TargOpt := TGTOPT_CHECK_LOS )

	if (!can_cast(caster, circle))
        	return 0;
	endif


	var cx := caster.x;
	var cy := caster.y;
	var shp := GetHP(caster);

	SetObjProperty(caster, "#Casting", 1);
	Start_script( ":summoning:checkmove", caster );

	var cast_on := Target( caster, TargOpt );
	if(!cast_on  or !cast_on.isa(POLCLASS_MOBILE))
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif

	if ( (cx != caster.x) or (cy != caster.y) or (shp > GetHP(caster)) || GetObjProperty(caster, "#SpellMove") )
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		SendSysMessage( caster, ERROR_MSG_LOST_CONCENTRATION );
		return 0;
	endif
	
	if(GetObjProperty(caster, "#SpellMove"))
		EraseObjProperty(caster, "#SpellMove");
	endif
	EraseObjProperty(caster, "#Casting");
	return cast_on;

endfunction


function CanTargetThing( caster, circle := 1, TargOpt := TGTOPT_CHECK_LOS )

	var cx := caster.x;
	var cy := caster.y;
	var shp := GetHP(caster);

	if(!can_cast(caster, circle))
        	return 0;
	endif
	
	SetObjProperty(caster, "#Casting", 1);
	Start_script( ":summoning:checkmove", caster );

	var cast_on := Target( caster, TargOpt );
	if(!cast_on)
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif

	if ( (cx != caster.x) or (cy != caster.y) or (shp > GetHP(caster)) || GetObjProperty(caster, "#SpellMove") )
		SendSysMessage( caster, ERROR_MSG_LOST_CONCENTRATION );
		if(GetObjProperty(caster, "#SpellMove"))
			EraseObjProperty(caster, "#SpellMove");
		endif
		EraseObjProperty(caster, "#Casting");
		return 0;
	endif
	
	if(GetObjProperty(caster, "#SpellMove"))
		EraseObjProperty(caster, "#SpellMove");
	endif
	EraseObjProperty(caster, "#Casting");
	return cast_on;

endfunction


function CheckForAnotherProtItem( who, item, prot )
	item := item;
	var prots := { "PoisonImmunity",
			 "MagicImmunity",
			 "SpellReflect" };

	if( !(prot in prots) )
		return 0;
	endif

	EraseObjProperty( who, prot );
	foreach thing in ListEquippedItems( who )
		if( GetObjProperty( thing, prot ) )
			SetObjProperty( who, prot, thing.serial );
			break;
		endif
	endforeach

	return 1;

endfunction


function ExtractName( name )

	var words := SplitWords( name );

	//Trunc the articles
	if( (words[1] == "A") or (words[1] == "a") or (words[1] == "An") or (words[1] == "an") )
		name := words[2];
		var i := 3;
		for( i ; i <= len(words) ; i := i + 1 )
			name := name + " " + words[i];
		endfor
	endif


	//Remove the "corpse of" from the name
	words := SplitWords( name );
	if( (words[1] == "corpse") and (words[2] == "of") )
		name := words[3];
		var i := 4;
		if( words[3] == "a" )
			name := words[4];
			i := 5;
		endif

		for( i ; i <= len(words) ; i := i + 1)
			name := name + " " + words[i];
		endfor
	endif


	//Remove the "Reanimated" or "conjured" from the name
	words := SplitWords( name );
	if( (words[1] == "Reanimated") or (words[1] == "conjured") )
		name := words[2];
		var i := 3;
		for( i ; i <= len(words) ; i := i + 1 )
			name := name + " " + words[i];
		endfor
	endif

	if( !name )
		name := "creature";
	endif

	return name;

endfunction


function FindOption( options, wanted )

	if( wanted in options )
		return 1;
	else
		return 0;
	endif

endfunction


function GetCircle( spell )

	var conf   	:= ReadConfigFile(":*:spells");
	var elem   	:= FindConfigElem(conf, spell);
	var circ 	:= GetConfigString(elem, "Circle");
	conf		:= ReadConfigFile(":*:circles");
	var circle	:= conf[circ].UseCircle;
	if (!circle)
		circle 	:= circ;
	endif

	return Cint(circle);

endfunction


function GetMagicEfficiencyPenalty( who )

	var penalty := 0;

	if(who.dead) return 0; endif

	if( who.npctemplate ) return 0; endif

	foreach item in ListEquippedItems( who )
		if(GetObjProperty( who , CLASSEID_MAGE ) && item.graphic in ByPassForMages(CLASSEID_MAGE))
		elseif( GetObjProperty( item , "MagicPenalty" ) )
			penalty := penalty + GetObjProperty( item , "MagicPenalty" );
		endif
	endforeach
	if( penalty > 100 )
		penalty := 100;
	endif

	return Cint(penalty);

endfunction


function GetProtLevel( cast_on, element )

	var prot;

	case( element )

		FIRE:	prot := GetObjProperty( cast_on , "FireProtection" );
			break;

		AIR:	prot := GetObjProperty( cast_on , "AirProtection" );
			break;

		EARTH:	prot := GetObjProperty( cast_on , "EarthProtection" );
			break;

		WATER:	prot := GetObjProperty( cast_on , "WaterProtection" );
			break;

		NECRO:	prot := GetObjProperty( cast_on , "NecroProtection" );
			break;

		HOLY:	prot := GetObjProperty( cast_on , "HolyProtection" );
			break;

		default:

	endcase

	if( !prot )
		prot := 0;
	endif

	return prot;

endfunction


function GetResultingElementalProt( cast_on, element )

	// Remove opposing complementing prots, what u get is what u get now
	return GetProtLevel( cast_on, element );
	
	var complementary;
	var opposed;
	case( element )
		FIRE:	complementary 	:= WATER;
			opposed		:= AIR;
			break;

		AIR:	complementary 	:= FIRE;
			opposed		:= EARTH;
			break;

		EARTH:	complementary 	:= AIR;
			opposed		:= WATER;
			break;

		WATER:	complementary 	:= EARTH;
			opposed		:= FIRE;
			break;
	endcase

	var eprot := GetProtLevel( cast_on, element );
	var cprot := GetProtLevel( cast_on, complementary );
	var oprot := GetProtLevel( cast_on, opposed );

	if( oprot < 0 )
		oprot := 6 - oprot;
	endif

	var resultprot := eprot + Cint(cprot / 2) -  Cint(oprot / 2);

	return resultprot;

endfunction


function GetResultingPlaneProtection( cast_on, plane )

	// remove complementary/opposing prots
	return GetProtLevel( cast_on, plane );
	
	var opposed;
	if( plane == NECRO )
		opposed := HOLY;
	elseif( plane == HOLY )
		opposed := NECRO;
	else
		return 0;
	endif

	var planeprot := GetProtLevel( cast_on, plane );
	var opposedprot := GetProtLevel( cast_on, opposed );

	if( opposedprot < 0 )
		opposedprot := 6 - opposedprot;
	endif

	var resultingprot := planeprot - opposedprot;

	return resultingprot;

endfunction


function GetScript( spellid )

	var spellcfg := ReadConfigFile( ":*:spells" );
	var script := spellcfg[spellid].Script;
	if( spellid <= MAX_NORMAL_SPELL )
		script := ":spells:" + script;
	elseif( spellid <= MAX_NECRO_SPELL )
		script := ":necro:" + script;
	elseif( spellid <= MAX_EARTH_SPELL )
		script := ":earth:" + script;
	elseif( spellid <= MAX_RITUAL_SPELL )
		script := ":artificier:" + script;
	elseif( spellid <= 181 and spellid >= 166 )
		script := ":holybook:" + script;
	elseif( spellid <=197 and spellid >= 182 )
		script := ":songbook:" + script;
	endif

	return script;

endfunction


function IsProtected( caster, cast_on, circle, nomsg := 0 )
	
	var protserial := GetObjProperty( cast_on, "MagicImmunity" );
	var protlvl	 := GetObjProperty( cast_on, "PermMagicImmunity" );
	if( !protlvl )
		protlvl := 0;
	endif


	if(GetObjProperty( cast_on , CLASSEID_WARRIOR ))
		//Print($"Target is a warrior!");
		if( protlvl == 0)
			circle := circle * ClasseBonus( cast_on, CLASSEID_WARRIOR );
		elseif( protlvl <= 6)
			circle := circle;
		else
			circle := circle * ClasseBonus( cast_on, CLASSEID_WARRIOR );
		endif
	endif

	var cmi := 0 - protlvl;
	var item;
	var charges;
	var cursed;

	if( protserial )
		foreach thing in ListEquippedItems( cast_on )
			if( thing.serial == protserial )
				item := thing;
				charges := Cint( GetObjProperty( item, "MagicImmunity" ) );
				cursed := GetObjProperty( item, "Cursed" );
				if( !charges )
					EraseObjProperty( cast_on, "MagicImmunity" );
					EraseObjProperty( item, "MagicImmunity" );
					PrintTextAbovePrivate(item, cstr(item.desc + " is out of charges!"), cast_on );
					CheckForAnotherProtItem( cast_on, item, "MagicImmunity" );
					IncRevision(item);
				endif
				break;
			endif
		endforeach
	endif

	if( charges >= circle )
		charges := charges - circle;
		SetObjProperty( item, "MagicImmunity", charges );
		IncRevision(item);
		if( !cursed )
			if(nomsg)
				return IMMUNED;
			endif
			PrintTextAbovePrivate( cast_on, cstr("Your " + item.desc + " protected you from the spell!"), cast_on );
			return IMMUNED;
		else
			return CURSED;
		endif

	elseif( cmi >= circle )
		return CURSED;

	elseif( protlvl >= circle )
		if(nomsg)
			return IMMUNED;
		endif
		PrintTextAbovePrivate( cast_on, cstr(caster.name + "'s spell is absorbed by your magical protection!") , cast_on );
		PrintTextAbovePrivate( cast_on, cstr("The spell dissipates upon contact with " + cast_on.name + "'s magical barrier!") , caster );
		return IMMUNED;
	endif

	return 0;

endfunction


function ModifyWithMagicEfficiency( who, val )

	if( GetObjProperty( who, CLASSEID_MAGE ) )
		val := CInt(val * ClasseBonus( who, CLASSEID_MAGE ));
	elseif( GetObjProperty( who, CLASSEID_CLERIC ) )
		if( !GetObjProperty( who, "CastingNecro"))
			//Not casting necro, gets bonus
		val := CInt(val * ClasseBonus( who, CLASSEID_CLERIC ));
		else
		//Casting necro, no bonus & erases CProp	
		EraseObjProperty( who, "CastingNecro");
	endif
	elseif( GetObjProperty( who, CLASSEID_WARRIOR ) )
		val := CInt(val / ClasseBonus( who, CLASSEID_WARRIOR ));
	endif

	var penalty := CInt( GetMagicEfficiencyPenalty( who ) );
	if( penalty >= 100 )
		return 0;
	endif

	return CInt( val * ( 100 - penalty ) / 100 );

endfunction


function PolymorphMobile( cast_on, new_form, new_color, duration, modd := 0, liche := 0 )

	var parms := {};

	parms[1] := cast_on;
	parms[2] := Cint( new_form );
	parms[3] := Cint( new_color );
	parms[4] := Cint( duration );
	if( modd )
		parms[5] := modd;
	endif
	if(liche)
		parms[6] := liche;
	endif

	Start_script( ":summoning:polymorphing", parms );

endfunction


function Reflect( source, targ )

	if(source.dead) return; endif
	if(targ.dead) return; endif

	PlaySoundEffect( source, SFX_SPELL_MAGIC_REFLECTION );
	PlayObjectCenteredEffect( source, FX_SPELL_MAGIC_REFLECTION , SPEED_SPELL_MAGIC_REFLECTION , LOOP_SPELL_MAGIC_REFLECTION );
	SendSysMessage( source, "Your magic reflect spell saved you!" );
	if( SPELL_EFFECT_TYPE_MOVING == 1 )
		Sleep( 1 );
		PlayMovingEffect(source,
		targ,
		SPELL_EFFECT_ID,
		SPELL_EFFECT_SPEED,
		SPELL_EFFECT_LOOP,
		explode := 0);
	endif

endfunction


function Reflected( caster, cast_on, circle )

	var source	:= caster;
	var targ	:= cast_on;
	
	if(GetObjProperty( cast_on , CLASSEID_WARRIOR ))
		return targ;
	endif

	if( source == targ )
		return targ;
	endif

	var permmr   := Cint( GetObjProperty( targ, "Permmr" ) );
	var corrupmr := Cint( 0 - permmr );
	var protserial := Cint( GetObjProperty( targ, "SpellReflect") );
	var item;
	var cursed;
	var charges;
	if( protserial )
		set_critical(1);
		foreach thing in ListEquippedItems( targ )
			if( thing.serial == protserial )
				item := thing;
				cursed  := GetObjProperty( item, "Cursed" );
				charges := Cint( GetObjProperty( item, "SpellReflect" ) );
				if( !charges )
					EraseObjProperty( item, "SpellReflect" );
					EraseObjProperty( targ, "SpellReflect" );
					PrintTextAbovePrivate( item, cstr(item.desc + " is out of charges!"), targ );
					CheckForAnotherProtItem( targ, item, "SpellReflect" );
					IncRevision(item);
				endif
				break;
			endif
		endforeach
		set_critical(0);
	endif


	if( charges and charges >= circle )
		charges := Cint(charges - circle);
		SetObjProperty( item, "SpellReflect", charges );
		IncRevision(item);
		if( !cursed )
			PrintTextAbovePrivate( targ , cstr("Your " + item.desc + " reflected the spell!") , targ );
			source := cast_on;
			targ := caster;
			Reflect( source, targ );
			//ReflectBack( caster, cast_on, source, targ, circle );
		else
			PrintTextAbovePrivate( targ, cstr("Your " + item.desc + " prevent all reflections!"), targ );
		endif

	elseif( corrupmr >= circle )
		PrintTextAbovePrivate( targ, cstr("One of your item prevent all reflections!"), targ );

	elseif( Cint(GetObjProperty( targ, "mr" )) >= circle )
		EraseObjProperty(targ, "mr");
		source := cast_on;
		targ := caster;
		Reflect( source, targ );
		//ReflectBack( caster, cast_on, source, targ, circle );

	elseif( permmr >= circle )
		source := cast_on;
		targ := caster;
		Reflect( source, targ );
		//ReflectBack( caster, cast_on, source, targ, circle );
	endif


	if( targ == caster )
		SendSysmessage( caster , "The spell is reflected back to you!" );
	endif

	return targ;

endfunction


function Resisted( caster , cast_on , circle , dmg )

	if( cast_on.dead ) return 0; endif
	if( caster.dead ) return 0; endif

	var points 	:= circle * 40;
	var magery 	:= GetEffectiveSkill( caster  , SKILLID_MAGERY );
	var evalint 	:= GetEffectiveSkill( caster  , SKILLID_EVALINT );
	var resist 	:= GetEffectiveSkill( cast_on , SKILLID_MAGICRESISTANCE );
	var chance 	:= Cint( resist / 6);
	var chance2 	:= Cint( resist - ((magery / 4) + (circle * 6)) );

	if( chance2 > chance )
		chance := chance2;
	endif

	if( GetObjProperty( cast_on , CLASSEID_MAGE ) )
		chance := Cint( chance * ClasseBonus( cast_on , CLASSEID_MAGE ) );
	elseif( GetObjProperty( cast_on , CLASSEID_CLERIC ) )
		chance := Cint( chance * ClasseBonus( cast_on , CLASSEID_CLERIC ) );
	elseif( GetObjProperty( cast_on , CLASSEID_WARRIOR ) )
		var bonus := ClasseBonus( cast_on , CLASSEID_WARRIOR );
		chance := Cint( chance / bonus / 2 );
		resist := Cint( resist / bonus );
	endif

	if( GetObjProperty( caster , CLASSEID_MAGE ) )
		chance := Cint( chance / ClasseBonus( caster , CLASSEID_MAGE ) );
	elseif( GetObjProperty( caster , CLASSEID_CLERIC ) )
		chance := Cint( chance / ClasseBonus( caster , CLASSEID_CLERIC ) );
	elseif( GetObjProperty( caster , CLASSEID_WARRIOR ) )
		var bonus := ClasseBonus( caster , CLASSEID_WARRIOR );
		chance := Cint( chance * bonus * 2 );
		resist := Cint( resist * bonus );
	endif

	if( resist < 25 )
		AwardSkillPoints( cast_on , SKILLID_MAGICRESISTANCE , Cint( points / 3 ) );
	endif

	if( Random(100)+1 <= chance )
		SendSysMessage( cast_on , "You feel yourself resisting magical energy!" );
		PlaySoundEffect( cast_on , SFX_SPELL_WEAKEN );
		PlayObjectCenteredEffect( cast_on , FX_SPELL_CURSE , 5 , 5 );
		dmg := Cint( dmg / 2 );
		if( dmg < 1 )
			dmg := 1;
		endif
		AwardSkillPoints( cast_on , SKILLID_MAGICRESISTANCE , points );
	endif

	dmg := Cint( dmg * (1 + (evalint - resist) / 200) );

	if( GetObjProperty( cast_on , CLASSEID_MAGE ) )
		dmg := Cint( dmg / ClasseBonus( cast_on , CLASSEID_MAGE ) );
	elseif( GetObjProperty( cast_on , CLASSEID_CLERIC ) )
		dmg := Cint( dmg / ClasseBonus( cast_on , CLASSEID_CLERIC ) );
	elseif( GetObjProperty( cast_on , CLASSEID_WARRIOR ) )
		dmg := Cint( dmg * ClasseBonus( cast_on , CLASSEID_WARRIOR ) );
	endif

	if( dmg < 0 )
		dmg := 1;
	endif

	return dmg;

endfunction


function ResistedDefinedSkillGain( caster , cast_on , dmg , diff , points := 0 )

	if( cast_on.dead ) return 0; endif
	if(caster.dead) return 0; endif

	var evalint;
	if( caster )
		evalint := GetEffectiveSkill( caster  , SKILLID_EVALINT );
	endif

	if( !evalint )
		evalint := 0;
	endif

	var resist	:= GetEffectiveSkill( cast_on , SKILLID_MAGICRESISTANCE );

	if( CheckSkill( cast_on, SKILLID_MAGICRESISTANCE, diff, points ) )
		dmg := Cint( dmg / 2 );
		if( dmg < 1 )
			dmg := 1;
		endif
	endif

	dmg := Cint( dmg * (1 + (evalint - resist) / 200) );

	if( GetObjProperty( cast_on , CLASSEID_MAGE ) )
		dmg := Cint( dmg / ClasseBonus( cast_on , CLASSEID_MAGE ) );
	elseif( GetObjProperty( cast_on , CLASSEID_CLERIC ) )
		dmg := Cint( dmg / ClasseBonus( cast_on , CLASSEID_CLERIC ) );
	elseif( GetObjProperty( cast_on , CLASSEID_WARRIOR ) )
		dmg := Cint( dmg * ClasseBonus( cast_on , CLASSEID_WARRIOR ) );
	endif

	if( dmg < 0 )
		dmg := 1;
	endif

	return dmg;

endfunction


function Send_Attack( ttarget , source , spellid := 0 )

	var ev := array;
	ev.+type;
	ev.+source;

	ev.type := SYSEVENT_ENGAGED;
	ev.source := source;
	SendEvent( ttarget , ev );

 	if (spellid)
         	ev.+spell;
         	ev.type := 0x0800; //spell attack
         	ev.spell := spellid;
         	SendEvent(ttarget, ev);
 	endif

endfunction


function SummonCreature( caster , npctemplate , cast_loc , sfx := 0x0218 , delay := 0 )

	var parms := {};

	parms[1] := caster;
	parms[2] := npctemplate;
	parms[3] := cast_loc;
	parms[4] := sfx;
	parms[5] := delay;

	Start_script( ":summoning:summoning" , parms );

endfunction


function TruncPkgName( name )

	if( name[1] == ":" )
		var i;
		for( i := 2 ; i <= len(name) ; i := i + 1 )
			if( name[i] == ":" )
				break;
			endif
		endfor

		return name[i+1,len(name)];

	else
		return name;
	endif

endfunction


function TryToCast( caster, spell, anim := ANIM_CAST_DIR )
	var allowed_spells := array{
	//Magery Spells
	2, //Create Food
	4, //Heal
	6, //Night Sight
	7, //Reactive Armor
	9, //Agility
	10, //Cunning
	11, //Cure
	14, //Magic Untrap
	15, //Protection
	16, //Strength
	17, //Bless
	21, //Telekinesis
	22, //Teleport
	23, //Unlock
	25, //Arch Cure
	26, //Arch Protection
	29, //Greater heal
	32, //Recall
	35, //Incognito
	36, //Spell Reflection
	44, //Invisibility
	45, //Mark
	48, //Reveal
	52, //Gate
	56, //Polymorph
	59, //Resurrection
	//Necro Book Spells
	78, //Liche
	//Earth Book Spells
	81, //Antidote
	82, //Owl Sight
	86, //Earth Blessing
	87, //Earth Portal
	88, //Nature's Touch
	91, //Shapeshift
	//Holy Book Spells
	166, //Grand Feast
	168, //Light of Day
	169, //Angelic Aura
	171, //Seraphim's Will
	172, //Angelic Gate
	173, //Remove Curse
	177, //Enlightenment
	178, //Revive
	179 //Sanctuary
	};

	var allowed := spell in allowed_spells;
	if(!allowed)
		if( !caster.cmdlevel )
			if( IsInAntiMagicArea( caster ) )
				SendSysMessage( caster, ERROR_MSG_ANTI_MAGIC_AREA );
				return IN_ANTIMAGIC_AREA;
			endif
		endif
	endif

	if (caster.hidden)
		caster.hidden := 0;
	endif
	
	if (caster.dead)
		return 0;
	endif

	//Variables declaration and initialisation
	var conf	 := ReadConfigFile( ":*:spells" );
	var elem	 := FindConfigElem( conf, spell );
	var circ	 := GetConfigInt( elem, "Circle" );
	conf	 	 := ReadConfigFile( ":*:circles" );
	var spelldelay := Cint( conf[circ].Delay );
	var value	 := Cint( conf[circ].PointValue );
	var manacost	 := Cint( conf[circ].Mana );
	var difficulty := Cint( conf[circ].Difficulty );
	var circle	:= conf[circ].UseCircle;

	//Casting Options management
	var options := GetObjProperty( caster, CASTINGOPT_PROP_NAME );
	if( !options )
		options := GetObjProperty( caster, PERM_CASTINGOPT_PROP_NAME );
		if( !options )
			options := "";
		endif
	else
		EraseObjProperty( caster, CASTINGOPT_PROP_NAME );
	endif
	
	sendDiff( caster, difficulty );
	//Try to cast the spell
	PerformAction( caster, anim );

	//Speak the power words
	if ( !FindOption( options, CASTINGOPT_NO_WORDS ) )	
		SpeakPowerWords( caster, spell );
	endif

	//Check for the equipment in hands
	if( !FindOption( options, CASTINGOPT_ANY_EQUIPMENT ) )

		var hand1	:= GetEquipmentByLayer( caster, LAYER_HAND1 );
		var hand2	:= GetEquipmentByLayer( caster, LAYER_HAND2 );

		if( hand1 or hand2 )
			var weapcfg := ReadConfigFile( ":*:itemdesc" );

			if( hand1 and weapcfg[hand1.objtype].BlocksCastingIfInHand != 0 )
				SendSysmessage( caster, ERROR_MSG_WRONG_EQUIPMENT );
				PlayObjectCenteredEffect( caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
				PlaySoundEffect( caster, SFX_SPELL_FIZZLE );
				return INVALID_EQUIPMENT;
			endif

			if( hand2 and weapcfg[hand2.objtype].BlocksCastingIfInHand != 0)
				SendSysmessage( caster, ERROR_MSG_WRONG_EQUIPMENT );
				PlayObjectCenteredEffect( caster, FX_SPELL_FIZZLE , SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
				PlaySoundEffect( caster, SFX_SPELL_FIZZLE );
				return INVALID_EQUIPMENT;
			endif
		endif
	endif

	//Delay and interruption management
	if( !FindOption( options, CASTINGOPT_NO_DELAY ) )
		var shp := GetHP(caster);
		var sx := caster.x;
		var sy := caster.y;
		var sz := caster.z;

		//Warriors cast slower
		if( GetObjProperty( caster, CLASSEID_WARRIOR ) )
			spelldelay := Cint( spelldelay * ClasseBonus( caster, CLASSEID_WARRIOR ) );
		endif
		if(spell == 32)
			spelldelay := 5000;
		elseif(spell == 52)
			spelldelay := 8000;
		endif
		var nearbyDanger;

		while( spelldelay >= 500 )
			if (!can_cast(caster, circle))
				return 0;
			endif
			Sleepms( 500 );
			spelldelay := spelldelay - 500;
			nearbyDanger := ListHostiles(caster,4);
			if( (spell == 32 || spell == 52) && len(nearbyDanger) )
				foreach mob in nearbyDanger
					if(CheckLineOfSight( mob, caster ))
						SendSysmessage( caster, ERROR_MSG_INTERRUPTED );
						PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
						PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
						return INTERRUPTED;
					endif
				endforeach
			endif
			
			if( (spell == 32 || spell == 52) && GetHP(caster) < shp )
				SendSysmessage( caster, ERROR_MSG_INTERRUPTED );
				PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
				PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
				return INTERRUPTED;
			endif

			// Using a scroll? Not allowed to move then!
			if( FindOption( options , CASTINGOPT_USE_SCROLL ) )
				if( caster.x != sx or caster.y != sy or caster.z != sz )
					SendSysmessage( caster, ERROR_MSG_LOST_CONCENTRATION );
					PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
					PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
					return INTERRUPTED;
				endif
			endif

			if(/* circ > 8 || */ spell == 32 || spell == 52)
				if( caster.x != sx or caster.y != sy or caster.z != sz )
					SendSysmessage( caster, ERROR_MSG_LOST_CONCENTRATION );
					PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
					PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
					return INTERRUPTED;
				endif
			endif
		endwhile

		sleepms( spelldelay );

		shp := GetHP(caster);
		if( GetHp(caster) < shp )
			SendSysmessage( caster , ERROR_MSG_INTERRUPTED );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return INTERRUPTED;
		endif
/*
		if( caster.x != sx or caster.y != sy or caster.z != sz )
			SendSysmessage( caster, ERROR_MSG_LOST_CONCENTRATION );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return INTERRUPTED;
		endif
*/
	endif
	
	//Consume the mana
	if( !FindOption( options, CASTINGOPT_NO_MANA ) )
		if( !ConsumeMana( caster, spell ) )
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE);
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			SendSysMessage( caster, ERROR_MSG_NOT_ENOUGH_MANA );
			return NO_MANA;
		endif
	endif

	//Consume the reagents
	if( !FindOption( options, CASTINGOPT_NO_REAGENTS ) )
		if( !ConsumeReagents( caster, spell ) )
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			SendSysMessage( caster, ERROR_MSG_NOT_ENOUGH_REAGENTS );
			return NO_REGS;
		endif
	endif

	// toad - half skill gain for scrolls
	if( FindOption( options , CASTINGOPT_SCROLL_SKILL_GAIN ) )
		value := CInt(value);
	endif

	//Do the skill check and return mana if failure occur
	if( !FindOption( options , CASTINGOPT_NO_SKILL_CHECK ) )
      if( !CheckSkill( caster , SKILLID_MAGERY , difficulty , value ) )
			SetMana(caster, GetMana(caster)+manacost);
			if( GetMana(caster) > GetMaxMana(caster) )
				SetMana(caster, GetMaxMana(caster)); 
			endif
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect( caster, SFX_SPELL_FIZZLE );
			SendSysmessage( caster, ERROR_MSG_FIZZLE );
			return NO_SKILL;
		endif
	endif

	return SUCCESS;

endfunction

function SendBoostMessage( mobile, targ, spell, points, delay, a )
points := CInt(points*0.8);
	if(!delay)
        if( mobile.serial != targ.serial )
			SendSysMessage( mobile, "You cast "+spell+" on "+targ.name+" for "+points+" points." );
		else
			SendSysMessage( mobile, "You cast "+spell+" on yourself for "+points+" points." );
        endif
 
        if( mobile.serial != targ.serial )
			if( a == 1 )
                 SendSysMessage( targ, "You recieved an "+spell+" boost for "+points+" points." );
			else
                 SendSysMessage( targ, "You recieved a "+spell+" boost for "+points+" points." );
			endif
		endif
	else
        if( mobile.serial != targ.serial )
			SendSysMessage( mobile, "You cast "+spell+" on "+targ.name+" for "+points+" points. The effect will last for "+ delay / 60.0+" minutes." );
		else
			SendSysMessage( mobile, "You cast "+spell+" on yourself for "+points+" points. The effect will last for "+ delay / 60.0 +" minutes." );
        endif
 
        if( mobile.serial != targ.serial )
			if( a == 1 )
                 SendSysMessage( targ, "You recieved an "+spell+" boost for "+points+" points. The effect will last for "+ delay / 60.0 +" minutes." );
			else
                 SendSysMessage( targ, "You recieved a "+spell+" boost for "+points+" points. The effect will last for "+ delay / 60.0 +" minutes." );
			endif
		endif
	endif
 return 1;
endfunction

function SmartAoE( caster, victims)

	var caster_RPer := GetObjProperty(caster, "IsRPer");
	var caster_noPK := GetObjProperty(caster, "NOPKAREA");
	var caster_CW := GetObjProperty(caster, "cwarring");
	var caster_CWcolor := GetObjProperty(caster,"cwcolor");
	var same_CWcolor := 0;
	var i;
	if(DEBUG_MODE)
		SendSysMessage( caster, "caster_noPK "+caster_noPK, color :=53);
		SendSysMessage( caster, "caster_CW "+caster_CW, color :=53);
		SendSysMessage( caster, "caster_CWcolor "+caster_CWcolor, color :=53);
	endif
	for(i:=1; i<=victims.Size(); i:=i+1)
		var victim_RPer := GetObjProperty(victims[i], "IsRPer" );
		var victim_noPK := GetObjProperty(victims[i], "NOPKAREA");
		var victim_CWcolor := GetObjProperty(victims[i],"cwcolor");
		var same_faction := caster_RPer == victim_RPer;
		if (caster_CW)
			if (GetGlobalProperty("cwtype") != "k")
				same_CWcolor := victim_CWcolor == caster_CWcolor;
			endif
		endif
		if(DEBUG_MODE)
			SendSysMessage( caster, victims[i].name, color :=63);
			SendSysMessage( caster, "victim_noPK "+victim_noPK, color :=53);
			SendSysMessage( caster, "same_faction "+same_faction, color :=53);
			SendSysMessage( caster, "same_CWcolor "+same_CWcolor, color :=53);
		endif
		//Borrowed from SmartSongAoE Function
		if ((victims[i].isa(POLCLASS_NPC)) && (victims[i].master == caster))
			victims.erase(i);
		endif
		if ((!victims[i].isa(POLCLASS_NPC)) && caster.party && (victims[i].party == caster.party))
			victims.erase(i);
		endif
		//Back to Original
		if (!caster.isa(POLCLASS_NPC) && !caster.cmdlevel && !victims[i].isa(POLCLASS_NPC))
			if((caster_noPK) || (victim_noPK) || (!same_faction) || (same_CWcolor))
				if (DEBUG_MODE)
					SendSysMessage( caster, "erased", color :=34);
				endif
				victims.erase(i);
			endif
		endif
	endfor

	return victims;
endfunction

function SmartBoost(caster, affected)

	var caster_RPer := GetObjProperty(caster, "IsRPer");
	var caster_CW := GetObjProperty(caster, "cwarring");
	var caster_CWcolor := GetObjProperty(caster,"cwcolor");
	var same_CWcolor := 1;
	var i;
	if(DEBUG_MODE)
		SendSysMessage( caster, "caster_RPer "+caster_RPer, color :=53);
		SendSysMessage( caster, "caster_CW "+caster_CW, color :=53);
		SendSysMessage( caster, "caster_CWcolor "+caster_CWcolor, color :=53);
	endif
	for(i:=1; i<=affected.Size(); i:=i+1)
		var victim_RPer := GetObjProperty(affected[i], "IsRPer" );
		var victim_CWcolor := GetObjProperty(affected[i],"cwcolor");
		var same_faction := caster_RPer == victim_RPer;
		if (caster_CW)
			if (GetGlobalProperty("cwtype") != "k")
				same_CWcolor := victim_CWcolor == caster_CWcolor;
			endif
		endif
		if(DEBUG_MODE)
			SendSysMessage( caster, affected[i].name, color :=63);
			SendSysMessage( caster, "victim_RPer "+victim_RPer, color :=53);
			SendSysMessage( caster, "same_faction "+same_faction, color :=53);
			SendSysMessage( caster, "same_CWcolor "+same_CWcolor, color :=53);
		endif
		if (!caster.isa(POLCLASS_NPC) && !caster.cmdlevel && !affected[i].isa(POLCLASS_NPC))
			if((!same_faction) || (!same_CWcolor))
				if (DEBUG_MODE)
					SendSysMessage( caster, "erased", color :=34);
				endif
				affected.erase(i);
			endif
		endif
	endfor

	return affected;

endfunction

