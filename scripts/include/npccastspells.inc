use uo;
use os;
use npc;

include "include/client";
include "include/dotempmods";
include "include/spelldata";
include "include/random";
include "include/fields";
include "include/objtype";
include "include/attributes";
include "include/classes";

function SpecialSorcerersBane(caster, cast_on)
	var circle := GetCircle( SPELLID_SORCERERS_BANE );
	var element1 := FIRE;
	var element2 := WATER;
	var fromhit;

		PlayMovingEffect(caster, 
                                 cast_on, 
                                 FX_SPELL_SORCERERS_BANE, 
                                 SPEED_SPELL_SORCERERS_BANE, 
                                 LOOP_SPELL_SORCERERS_BANE, 
                                 explode := 0);
	
	
	PlaySoundEffect( cast_on , SFX_SPELL_SORCERERS_BANE );
	
	var immunity := IsProtected( caster , cast_on , circle );
	
	if( immunity == IMMUNED )
		return;
	endif

	if( immunity == CURSED )
		circle := circle * 2;
	endif
	
	var dmg := CalcSpellDamage( caster ,cast_on , circle );
	ApplyElementalDamage( caster , cast_on , circle , Cint( dmg / 2 ) , element1 );
	
	var stolen := Resisted( caster , cast_on , circle , ModifyWithMagicEfficiency( caster , GetMana(cast_on) ) );
	stolen := Cint( stolen / ClasseBonus( cast_on , CLASSEID_MAGE ) );

	if(caster.isa(POLCLASS_NPC))
		stolen := stolen/2;
	endif

	var wProt := GetProtLevel(cast_on, element2);
	if(wProt)
		wProt := CDbl(wProt);
		wProt := wProt / 100;
		wProt := 1.00 - wProt;
		stolen := stolen*wProt;
		stolen := CInt(stolen);
		if(stolen < 0)
			stolen := 0;
		endif
	endif

	if( stolen > GetMana(cast_on) )
		stolen := GetMana(cast_on);
	endif
	SetMana(cast_on, (GetMana(cast_on)-stolen));

	var stolenMana := GetMana(caster) + stolen;
	if( stolenMana > GetMaxMana(caster) )
		stolenMana := GetMaxMana(caster);
	endif
	SetMana(caster, stolenMana);
		
	PlaySoundEffect( caster , SFX_SPELL_SUMMON_ELEMENTAL );
	var x := cast_on.x;
	var y := cast_on.y;
	var z := cast_on.z;

	if(!cast_on)
		return;
	endif

	PlayStationaryEffect( cint(x), cint(y), cint(z+110), 0x350b, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x), cint(y), cint(z+110), 0x34ed, 0, 0, 0, caster.realm );
	var i;
	
	var waterfall_array := {};
	var waterfall_base := {};

	for(i := 10; i >= 0; i := i - 1)
		PlayStationaryEffect( cint(x), cint(y), cint(z+i*10), 0x3517, 0, 0, 0, caster.realm );
		PlayStationaryEffect( cint(x), cint(y), cint(z+i*10), 0x34f9, 0, 0, 0, caster.realm );
		sleepms( 10 );
	endfor
	
	
	PlayStationaryEffect( cint(x), cint(y+1), cint(z), 0x17a1, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x+1), cint(y), cint(z), 0x179d, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x), cint(y-1), cint(z), 0x179f, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x-1), cint(y), cint(z), 0x17a3, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x+1), cint(y+1), cint(z), 0x17a7, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x-1), cint(y+1), cint(z), 0x17a8, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x+1), cint(y-1), cint(z), 0x17a5, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x-1), cint(y-1), cint(z), 0x17a6, 0, 0, 0, caster.realm );
	PlayStationaryEffect( cint(x), cint(y), cint(z), 0x1797, 0, 0, 0, caster.realm );

	PlaySoundEffect(caster, 0x11);
	
	ApplyElementalDamage( caster , cast_on , circle , Cint( dmg / 2 ) , element2 );
endfunction

function SpecialGustOfAir(caster, cast_on)
	var circle := GetCircle( SPELLID_GUST_OF_AIR );
	var element := AIR;
		PlayStationaryEffect( cast_on.x,
				      cast_on.y,
				      cast_on.z,
				      FX_SPELL_GUST_OF_AIR,
				      SPEED_SPELL_GUST_OF_AIR,
				      LOOP_SPELL_GUST_OF_AIR );
	
	PlaySoundEffect( cast_on , SFX_SPELL_GUST_OF_AIR1 );
	PlaySoundEffect( cast_on , SFX_SPELL_GUST_OF_AIR2 );
	
	var immunity := IsProtected( caster , cast_on , circle );
	if( immunity == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif

	var magery  	:= Cint(GetEffectiveSkill( caster , SKILLID_MAGERY ) / 10 );
	if( immunity == CURSED )
		circle := circle * 2;
		magery := magery * 2;
	endif
	
	var dmg := CalcSpellDamage( caster ,cast_on , circle );
	ApplyElementalDamage( caster , cast_on , circle , dmg , element );
endfunction

function SpecialWyvernStrike(caster, cast_on)
	var circle := GetCircle( SPELLID_WYVERN_STRIKE );
	var plane := NECRO;
		PlayMovingEffect(caster, 
                                 cast_on, 
                                 FX_SPELL_WYVERN_STRIKE, 
                                 SPEED_SPELL_WYVERN_STRIKE, 
                                 LOOP_SPELL_WYVERN_STRIKE, 
                                 explode := 0);
	
	
	PlaySoundEffect( cast_on , SFX_SPELL_WYVERN_STRIKE );

	var protected := IsProtected( caster , cast_on , circle );
	if( protected == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif
		
	if( protected == CURSED )
		circle := circle * 2;
	endif
	
	var dmg	:= CalcSpellDamage( caster , cast_on , circle );
	var magery	:= GetEffectiveSkill ( caster , SKILLID_MAGERY );
	var plvl	:= ModifyWithMagicEfficiency( caster , Cint( magery /40 ) + 1 );
	var maxx	:= ModifyWithMagicEfficiency( caster , 4 );

	if( protected == CURSED )
		plvl := plvl * 2;
	endif
	
	plvl := Resisted( caster , cast_on , circle , plvl );
	
	if( plvl > maxx )
		plvl := maxx;
	endif
		
	EraseObjProperty( caster , "#Casting" );
	ApplyPlanarDamage( caster , cast_on , circle , dmg , plane );
	var resultprot := GetResultingElementalProt( cast_on, 0x0010 );
	if( plvl && resultprot < 100 )
		SetPoison( cast_on , plvl , "You were poisonned by " + caster.name + "'s Wyvern's Strike spell!", caster );
	endif
endfunction

function SpecialKill(caster, cast_on)
	var circle := GetCircle( SPELLID_KILL );
	var plane := NECRO;
		var graph := PlayObjectCenteredEffect(cast_on, 
                                         FX_SPELL_KILL,
                                         SPEED_SPELL_KILL,
                                         LOOP_SPELL_KILL);
	
	
	PlaySoundEffect( cast_on , SFX_SPELL_KILL) ;

	var protected := IsProtected( caster , cast_on , circle );
	if( protected == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif
	

	var multiplier;
	if (!cast_on.npctemplate)
		multiplier := 1;
	else
		multiplier := 2;
	endif
	
	multiplier := ModifyWithMagicEfficiency( caster , multiplier );

	if( protected == CURSED )
		circle := circle * 2;
		multiplier := multiplier * 2;
	endif
	
	
	var alternative := CalcSpellDamage( caster , cast_on , circle );
	var protection  := GetResultingPlaneProtection( cast_on , plane );
	var casterskill := GetEffectiveSkill( caster , SKILLID_EVALINT );
	var magery	  := GetEffectiveSkill( caster , SKILLID_MAGERY );
	var victimlevel := GetHP(cast_on);
	var instantkill := Cint( magery * multiplier / 3 );
	
	if( protection )
		instantkill := instantkill - Cint( instantkill * protection * 0.25 );
		if( instantkill < 1 )
			instantkill := 1;
		endif
	endif
	
	if( victimlevel <= instantkill )
		ApplyTheDamage( cast_on , caster , GetHP(cast_on)+3 , DMGID_MAGIC + plane );
	
	elseif( victimlevel <= Cint( instantkill * 1.5 ) )
		var victimskill  := GetEffectiveSkill( cast_on , SKILLID_MAGICRESISTANCE );
		var resistchance := Cint( (victimskill / multiplier) -  (casterskill / 3) ); ;
		if( RandomDiceStr("1d100") <= resistchance )
			ApplyPlanarDamage( caster , cast_on , circle , alternative , plane );
		else
			ApplyTheDamage( cast_on , caster , GetHP(cast_on) + 3 , DMGID_MAGIC + plane );
		endif
	
	else
		ApplyPlanarDamage( caster , cast_on , circle , alternative , plane );
	endif
endfunction

function SpecialCallLightning(caster, cast_on)
	var circle := GetCircle( SPELLID_CALL_LIGHTNING );
	var element := AIR;
		PlayLightningBoltEffect( cast_on );
		PlaySoundEffect( cast_on , SFX_SPELL_CALL_LIGHTNING );
		PlayLightningBoltEffect( cast_on );
		PlaySoundEffect( cast_on , SFX_SPELL_CALL_LIGHTNING );
	
	
	var immunity := IsProtected( caster , cast_on , circle );
	
	if( immunity == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif

	if( immunity == CURSED )
		circle := circle * 2;
	endif
	
	var dmg := CalcSpellDamage( caster ,cast_on , circle );
	dmg := dmg+10;
	ApplyElementalDamage( caster , cast_on , circle , dmg , element );
endfunction

function SpecialIcestrike(caster, cast_on)
	var circle := GetCircle( SPELLID_ICE_STRIKE );
	var element := WATER;
		PlayObjectCenteredEffectEx( cast_on, FX_SPELL_ICE_STRIKE, SPEED_SPELL_ICE_STRIKE, 40 );
	

	PlaySoundEffect( cast_on , SFX_SPELL_ICE_STRIKE1 );
	PlaySoundEffect( cast_on , SFX_SPELL_ICE_STRIKE2 );
	
		
	var immunity := IsProtected( caster , cast_on , circle );
	if( immunity == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif

	if( immunity == CURSED )
		circle := circle * 2;
	endif
	
	var dmg := CalcSpellDamage( caster ,cast_on , circle );
	ApplyElementalDamage( caster , cast_on , circle , dmg , element );
endfunction

function SpecialShiftingEarth(caster, cast_on)
	var circle := GetCircle( SPELLID_SHIFTING_EARTH );
	var element := EARTH;
		PlayObjectCenteredEffect(cast_on, 
                                         FX_SPELL_SHIFTING_EARTH,
                                         SPEED_SPELL_SHIFTING_EARTH,
                                         LOOP_SPELL_SHIFTING_EARTH);
	

	PlaySoundEffect( cast_on , SFX_SPELL_SHIFTING_EARTH );

	var immunity := IsProtected( caster , cast_on , circle );

	if( immunity == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif

	var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
	if( immunity == CURSED )
		SendSysmessage( cast_on , "One of your equipped items increases the effect of the spell!" );
		magery := magery * 2;
		circle := circle * 2;
	endif

	var dmg := CalcSpellDamage( caster , cast_on , circle );
	ApplyElementalDamage( caster , cast_on , circle , dmg , element );

        if( !CanMod( cast_on , "dex" ) )
        	EraseObjProperty( caster , "#Casting" );
                return;
        endif
        
	var prot := GetResultingElementalProt( cast_on , element );
	if( prot )
		magery := Cint( magery - ( magery * prot / 10 ) );
	endif
	
	if( magery < 1 )
		magery := 1;
	endif
	
	EraseObjProperty( caster , "#Casting" );
        var mod_amount := Resisted( caster , cast_on , circle , GetModAmount( caster ) );
	var duration := GetModDuration( caster );
        DoTempMod( cast_on , "dex" , 0 - mod_amount , duration );
endfunction

function SpecialChampFire(caster, cast_on, element)
		var circle := 8;

		var elecolor;
		case(element)
			"EARTH": elecolor := 1134;
			"WATER": elecolor := 1555;
			"FIRE": elecolor := 1172;
			"AIR": elecolor := 1655;
			"NECRO": elecolor := 1157;
			"HOLY": elecolor := 1153;
		endcase

		var eleName;
		case(element)
			"EARTH": eleName := "Earth";
			"WATER": eleName := "Water";
			"FIRE": eleName := "Fire";
			"AIR": eleName := "Air";
			"NECRO": eleName := "Necro";
			"HOLY": eleName := "Holy";
		endcase

		PlayMovingEffectEx(caster, 
                                 cast_on, 
                                 FX_SPELL_FIREBALL, 
                                 SPEED_SPELL_FIREBALL, 
                                 LOOP_SPELL_FIREBALL, 
                                 elecolor);
	
	PlaySoundEffect( caster, SFX_SPELL_FIREBALL );
	
	var immunity := IsProtected( caster , cast_on , circle );
	
	if( immunity == IMMUNED )
		return;
	endif

	if( immunity == CURSED )
		circle := circle * 2;
	endif
	
	var dmg := CalcSpellDamage( caster ,cast_on , circle );
	ApplyElementalDamage( caster , cast_on , circle , dmg , element );
	
	var fields := {};
	var counter := 0;
	var duration := 60;
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x , cast_on.y , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+1 , cast_on.y , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+2 , cast_on.y , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-1 , cast_on.y , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-2 , cast_on.y , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );

	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x , cast_on.y+1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+1 , cast_on.y+1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+2 , cast_on.y+1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-1 , cast_on.y+1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-2 , cast_on.y+1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x , cast_on.y-1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+1 , cast_on.y-1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+2 , cast_on.y-1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-1 , cast_on.y-1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-2 , cast_on.y-1 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );

	
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x , cast_on.y+2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+1 , cast_on.y+2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-1 , cast_on.y+2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x , cast_on.y-2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x+1 , cast_on.y-2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
	counter := counter  + 1;
	fields[counter] := CreateItemAtLocation( cast_on.x-1 , cast_on.y-2 , cast_on.z , 0x398c , 1, me.realm );
	fields[counter].color := elecolor;
	fields[counter].name := "Field of "+eleName;
	SetObjProperty(fields[counter], "specialELE", element);
	SetObjProperty( fields[counter] , "ByTrueMage" , 1 );

	MoveObjectToLocation( cast_on, cast_on.x, cast_on.y, cast_on.z, cast_on.realm, MOVEOBJECT_FORCELOCATION);
	var buffParms := {fields, duration};
	Start_script( ":summoning:npcremovefields", buffParms );
endfunction

function SpecialHolyBolt(caster, cast_on)
	var circle := 8;
	var element := HOLY;
	var graph := PlayLightningBoltEffect( cast_on );
	PlaySoundEffect( cast_on , SFX_SPELL_HOLY_BOLT );
			
	var immunity := IsProtected( caster , cast_on , circle );
	if( immunity == IMMUNED )
		EraseObjProperty( caster , "#Casting" );
		return;
	endif

	if( immunity == CURSED )
		circle := circle * 2;
	endif
	
	var dmg := (CalcSpellDamage( caster ,cast_on , circle ));
	if ((!caster.npctemplate and cast_on.npctemplate and GetObjproperty(cast_on,"Type")== "Undead") or (caster.npctemplate and !cast_on.npctemplate))
		
		ApplyElementalDamage( caster , cast_on , circle , dmg , element );
	else
		HealDamage( cast_on , dmg);
	endif
	EraseObjProperty( caster , "#Casting" );
endfunction