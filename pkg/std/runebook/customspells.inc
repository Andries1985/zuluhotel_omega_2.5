/////////////////////////////////////////////////////
// customspells.inc -- Simplified gate and recall code
//                     for use with runebook.src
//
// Author(s): Racalac
// Revisions: 1.0, 11/13/99: Gate, Recall
//                           Mana, Reagent consumption
//                           Normal Spell Restrictions
//
// Notes: (rev1.0) - None
//
// Dependencies: (rev1.0) - client.inc
//                          objtype.inc
//                          spellrestrictions.inc
//
/////////////////////////////////////////////////////

use uo;
use os;

include "include/objtype";
include "include/spelldata";
include "include/areas";
include "include/client";
include ":housing:utility";
include ":statichousing:recallgatehelp";

const RECALL_DELAY_MS	:= 5000;
const GATE_DELAY_MS	:= 8000;


function CustomRecall( caster, x, y, z, realm, from_charge := 0 )

	var shp	:= GetHP(caster);
	var sx	:= caster.x;
	var sy	:= caster.y;
	var sz	:= caster.z;
	var srealm := caster.realm;
	var smana	:= GetMana(caster);
	var circle	:= 1;
	var spelldelay := RECALL_DELAY_MS;

	caster.hidden := 0;
	PerformAction( caster , ANIM_CAST_DIR );
        SpeakPowerWords( caster , SPELLID_RECALL );

	if( !CanUseBook( caster ) )
		return 0;
	endif
	
	var nearbyDanger;
	while( spelldelay >= 500 )
		Sleepms( 500 );
		spelldelay := spelldelay - 500;
		if( GetHP(caster) < shp )
			SendSysmessage( caster , "You were interrupted during the casting" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		if( caster.x != sx or caster.y != sy or caster.z != sz or caster.realm != srealm )
			SendSysmessage( caster , "You've lost your concentration!" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		nearbyDanger := ListHostiles(caster, 4);
		if( len(nearbyDanger) )
			foreach mob in nearbyDanger
				if(CheckLineOfSight( mob, caster ))
					SendSysmessage( caster , "You've lost your concentration!" );
					PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
					PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
					return 0;
				endif
			endforeach
		endif
	endwhile
	/*
	Sleepms( spelldelay );

	if( GetHP(caster) < shp )
		SendSysmessage( caster , "You were interrupted during the casting" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

	if( caster.x != sx or caster.y != sy or caster.z != sz )
		SendSysmessage( caster , "You've lost your concentration!" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif*/

	if(!GetObjProperty( caster, "betatest" ))
        	if(!ConsumeReagents( caster, SPELLID_RECALL ))
		   	PrintTextAbovePrivate(caster,"*No reagents*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif

        	if(!ConsumeMana( caster , SPELLID_RECALL ))
		   	PrintTextAbovePrivate(caster,"*No mana*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif

	        if(!CheckSkill( caster , SKILLID_MAGERY , GetSpellDifficulty(SPELLID_RECALL), 50 ) )
        		SetMana(caster, smana);
			PrintTextAbovePrivate(caster,"*The spell fizzle*", caster);
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
           		return 0;
        	endif
        endif

	EraseObjProperty( caster, "betatest" );
	if( !can_cast( caster , circle ) )
		return 0;
	endif

	if( IsForbiddenAreaToRecallFrom( caster ) )
		SendSysMessage( caster, "You can't recall from there." );
		return 0;
	endif

	var tox := x;
	var toy := y;
	var toz := z;
	var torealm := realm;

	if( !Checkpoz(tox,toy) )
		SendSysMessage( caster , "The destination is a magic place with protection." );
		return 0;
	endif

	if( IsForbiddenAreaToRecallTo( caster , tox , toy ) )
		SendsysMessage( caster , "The destination is a magic place with protection." );
		return 0;
	endif
	var owner;
	var lamp;
	var Near_Items := ListItemsNearLocation(tox, toy, toz, 10, torealm);
	foreach thing in Near_Items
		if (thing.objtype == 0xf156)
			lamp := thing;
			owner := GetObjProperty(lamp,"Owner");
		endif
	endforeach
	if (lamp and owner)
		if (caster.serial != owner)
			SendSysMessage( caster, "You can't recall to this house." );
			return;
		endif
	endif
	
	
	var multi := GetStandingHeight( tox, toy, toz, torealm ).multi;
	if( multi )

		if( !Iscowner( caster, multi )  )
			if( !IsFriend( caster, multi, RECALL_TO ) )
				SendSysMessage( caster, "You can't recall to this house." );
				return;
			endif
		endif
	endif

	var multi2 := GetStandingHeight(sx, sy, sz, srealm).multi;
	if(multi2)
		if( !Iscowner( caster, multi2 )  )
			if( !IsFriend( caster, multi2, RECALL_TO ) )
				SendSysMessage( caster, "You can't recall from this house." );
				return;
			endif
		endif
	endif
	// Same check as above for static houses
	var serialstatichouse := StaticHouseSerial( caster );
	if(serialstatichouse)
		var sign := SystemFindObjectBySerial(serialstatichouse);
		if(GetObjProperty( sign, "OwnerAcct") != caster.acctname)
			if( !Iscowner( caster, sign )  )
				if( !IsFriend( caster, sign, RECALL_TO) )
					SendSysMessage( caster, "You can't recall from this house." );
					return;
				endif
			endif
		endif
	endif
	serialstatichouse := StaticHouseSerialAtLocation( tox, toy, toz );
	if(serialstatichouse)
		var sign := SystemFindObjectBySerial(serialstatichouse);
		if(GetObjProperty( sign, "OwnerAcct") != caster.acctname)
			if( !Iscowner( caster, sign )  )
				if( !IsFriend( caster, sign, RECALL_TO) )
					SendSysMessage( caster, "You can't recall to this house." );
					return;
				endif
			endif
		endif
	endif
	PlaySoundEffect( caster, SFX_SPELL_RECALL1 );
	sleepms(250);

	MoveObjectToLocation( caster, tox, toy, toz, torealm );
	PlaySoundEffect( caster, SFX_SPELL_RECALL2 );

        return 1;

endfunction


function CustomGate( caster, x, y, z, realm )

	var shp	:= GetHP(caster);
	var sx	:= caster.x;
	var sy	:= caster.y;
	var sz	:= caster.z;
	var srealm := caster.realm;
	var smana	:= GetMana(caster);
	var circle	:= 7;
	var spelldelay := GATE_DELAY_MS;

	caster.hidden := 0;
	PerformAction( caster , ANIM_CAST_DIR );
        SpeakPowerWords( caster , SPELLID_GATE_TRAVEL );

	if( !CanUseBook( caster ) )
		return 0;
	endif
	
	var nearbyDanger;

	while( spelldelay >= 500 )
		Sleepms( 500 );
		spelldelay := spelldelay - 500;
		if( getHP(caster) < shp )
			SendSysmessage( caster , "You were interrupted during the casting" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		if( caster.x != sx or caster.y != sy or caster.z != sz or caster.realm != srealm )
			SendSysmessage( caster , "You've lost your concentration!" );
			PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE,LOOP_SPELL_FIZZLE );
			PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
			return 0;
		endif

		nearbyDanger := ListHostiles(caster, 4);
		if( len(nearbyDanger) )
			foreach mob in nearbyDanger
				if(CheckLineOfSight( mob, caster ))
					SendSysmessage( caster , "You've lost your concentration!" );
					PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
					PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
					return 0;
				endif
			endforeach
		endif
	endwhile

	/*Sleepms( spelldelay );

	if( GetHP(Caster) < shp )
		SendSysmessage( caster , "You were interrupted during the casting" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif

	if( caster.x != sx or caster.y != sy or caster.z != sz )
		SendSysmessage( caster , "You've lost your concentration!" );
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		return 0;
	endif*/

        if(!ConsumeReagents( caster, SPELLID_GATE_TRAVEL ))
	   	PrintTextAbovePrivate(caster,"*No reagents!*", caster);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
        	return 0;
        endif

        if(!ConsumeMana( caster , SPELLID_GATE_TRAVEL ))
	   	PrintTextAbovePrivate(caster,"*No mana!*", caster);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
        	return 0;
        endif

        if(!CheckSkill( caster , SKILLID_MAGERY, GetSpellDifficulty(SPELLID_GATE_TRAVEL), 50 ) )
	       	SetMana(caster, smana);
		PlayObjectCenteredEffect(caster, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
		PlaySoundEffect(caster, SFX_SPELL_FIZZLE);
		SendSysmessage(caster, "The spell fizzles");
        	return 0;
        endif

	if( !can_cast(caster, circle) )
		return 0;
	endif

	if( IsForbiddenAreaToGateFrom( caster ) )
		SendSysMessage( caster, "You can't gate from there." );
		return 0;
	endif

	var antigate := ListItemsNearLocation(caster.x, caster.y, caster.z, 10, caster.realm);
	foreach item in antigate
		if (item.objtype == 0xa3c9)
			SendSysMessage( caster, "You can't gate from here." );
			return;
		endif
	endforeach
	
	var multi := GetStandingHeight( caster.x, caster.y, caster.z, caster.realm ).multi;
	if( multi )
		if( !Iscowner( caster, multi )  )
			if( !IsFriend( caster, multi, GATE_FROM) )
				SendSysMessage( caster, "You can't gate from this house." );
				return;
			endif
		endif
	endif
	
	var tox := x;
	var toy := y;
	var toz := z;
	var torealm := realm;

	if( IsForbiddenAreaToGateTo( caster, tox, toy ) )
		SendSysMessage(caster, "The destination is a magic place with protection.");
		return 0;
	endif
		var owner;
	var lamp;
	var Near_Items := ListItemsNearLocation(tox, toy, toz, 10, torealm);
	foreach thing in Near_Items
		if (thing.objtype== 0xf156)
			lamp := thing;
			owner := GetObjProperty(lamp,"Owner");
		endif
	endforeach
	if (lamp and owner)
		if (caster.serial != owner)
			SendSysMessage( caster, "You can't gate to this house." );
			return;
		endif
	endif

	var antigate2 := ListItemsNearLocation(tox, toy, toz, 10, torealm);
	foreach item in antigate2
		if (item.objtype == 0xa3c9)
			SendSysMessage( caster, "You can't gate there." );
			return;
		endif
	endforeach
	
  multi := GetStandingHeight( tox, toy, toz, torealm ).multi;
	if( multi )
		if( !Iscowner( caster, multi )  )
			if( !IsFriend( caster, multi, GATE_TO) )
				SendSysMessage( caster, "You can't gate to this house." );
				return;
			endif
		endif
	endif
	// Same check as above for static houses
	var serialstatichouse := StaticHouseSerial( caster );
	if(serialstatichouse)
		var sign := SystemFindObjectBySerial(serialstatichouse);
		if(GetObjProperty( sign, "OwnerAcct") != caster.acctname)
			if( !Iscowner( caster, sign )  )
				if( !IsFriend( caster, sign, GATE_FROM) )
					SendSysMessage( caster, "You can't gate from this house." );
					return;
				endif
			endif
		endif
	endif
	serialstatichouse := StaticHouseSerialAtLocation( tox, toy, toz );
	if(serialstatichouse)
		var sign := SystemFindObjectBySerial(serialstatichouse);
		if(GetObjProperty( sign, "OwnerAcct") != caster.acctname)
			if( !Iscowner( caster, sign )  )
				if( !IsFriend( caster, sign, GATE_FROM) )
					SendSysMessage( caster, "You can't gate to this house." );
					return;
				endif
			endif
		endif
	endif

	var gate1 := CreateItemAtLocation( caster.x, caster.y, caster.z, 0x99b1, 1, caster.realm );
	gate1.facing := 1;

	var gate2 := CreateItemAtLocation( tox, toy, toz, 0x99b1, 1, torealm );
	gate2.facing := 2;

	if (!gate1 or !gate2)
		DestroyItem(gate1);
		DestroyItem(gate2);
		return 0;
	endif

	SetObjProperty( gate1, "GateDestX", gate2.x );
	SetObjProperty( gate1, "GateDestY", gate2.y );
	SetObjProperty( gate1, "GateDestZ", gate2.z );
	SetObjProperty( gate1, "GateDestRealm", gate2.realm );
	SetObjProperty( gate2, "GateDestX", gate1.x );
	SetObjProperty( gate2, "GateDestY", gate1.y );
	SetObjProperty( gate2, "GateDestZ", gate1.z );
	SetObjProperty( gate2, "GateDestRealm", gate1.realm );

	Detach();

	set_critical(0);

	PlaySoundEffect( caster, SFX_SPELL_GATE_TRAVEL );

	Sleep(30);

	set_critical(1);
	DestroyItem( gate1 );
	DestroyItem( gate2 );
	set_critical(0);

        return 1;

endfunction


function CanUseBook( me )

	var hand1 := GetEquipmentByLayer( me , LAYER_HAND1 );
	var hand2 := GetEquipmentByLayer( me , LAYER_HAND2 );

	if( hand1 or hand2 )
		var weapcfg := ReadConfigFile( ":*:itemdesc" );

		if( hand1 and weapcfg[hand1.objtype].BlocksCastingIfInHand != 0 )
			SendSysmessage( me , "Your hands must be free to use that." );
			PlayObjectCenteredEffect( me , FX_SPELL_FIZZLE , SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
			PlaySoundEffect( me , SFX_SPELL_FIZZLE );
			return 0;
		endif

		if( hand2 and weapcfg[hand2.objtype].BlocksCastingIfInHand != 0)
			SendSysmessage( me , "Your hand must be free to use that." );
			PlayObjectCenteredEffect( me , FX_SPELL_FIZZLE , SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
			PlaySoundEffect( me , SFX_SPELL_FIZZLE );
			return 0;
		endif
	endif

	return 1;

endfunction

