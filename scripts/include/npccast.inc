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
include "include/npccastspells";

function castspell( caster, cast_on, spellname )

	case (spellname)
	"summondrag":
		SummonDrag(caster);	
	"summonskel":
		SummonSkel(caster);	
	"gluttons":
		Gluttons(caster);	
	"superspider":
		Superspider(caster);	
	"summonskelarmy":
		SummonSkelarmy(caster);
	"massvatten":
		Massvatten(caster);
	"masseld":
		Masseld(caster, cast_on);
	"massgust":
		Massgust(caster);
	"masswyvern":
		Masswyvern(caster);
	"masskill":
		Masskill(caster);
	"masscalllightning":
		Masscalllightning(caster);
	"massicestrike":
		Massicestrike(caster);
	"massshiftingearth":
		Massshiftingearth(caster);
	"massholybolt":
		MassHolyBolt(caster);
	"champfire":
		ChampFire(caster);
	"summonsshiftingearth":
		SummonsShiftingEarth(caster, cast_on);
	"summonsgustofair":
		SummonsGustOfAir(caster, cast_on);
	"summonsrisingfire":
		SummonsRisingFire(caster, cast_on);
	"summonsicestrike":
		SummonsIceStrike(caster, cast_on);
	"summonsspectretouch":
		SummonsSpectreTouch(caster, cast_on);

	default:
		var SpellID := GetSpellID(spellname);
		if (!spellID)
			return;
		endif

		If (!PrepareToCast(caster,cast_on,spellid))
			return;
		endif

		var parms array;
		parms[1] := "#MOB";
		parms[2] := caster;
		parms[3] := cast_on;

		detach();

		start_script( GetScript( spellid ), parms );
	endcase
endfunction

function Massvatten(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Fluctus Perturbo Magus Navitas");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialSorcerersBane(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Massgust(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Gust Do Ar");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialGustOfAir(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Masswyvern(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Umbrae Tenebrae Venarent");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialWyvernStrike(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Masskill(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Ulties Manum Necarent");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialKill(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Masscalllightning(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Batida Do Deus");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialCallLightning(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Massicestrike(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Geada Com Inverno");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialIcestrike(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Massshiftingearth(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Esmagamento Con Pedra");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialShiftingEarth(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function MassHolyBolt(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	say("Vas Sanctus Utor Telum");
	sleep(2);
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialHolyBolt(me, mobile);
				endif
			endif
		endif
	endforeach
endfunction

function Masseld(me, onwho)
	var counter := 0;
	var fields := {};
	var duration := 30;
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );
	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC))
				var los := CheckLineOfSight( me, mobile );
				if(los)
					PlayMovingEffect(me, onwho, FX_SPELL_FIREBALL, SPEED_SPELL_FIREBALL,  LOOP_SPELL_FIREBALL, 0);
					PlaySoundEffect( onwho, SFX_SPELL_FIREBALL );
					counter := counter  + 1;
					fields[counter] := CreateItemAtLocation( mobile.x , mobile.y , mobile.z , 0x398c , 1, me.realm );
					SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
					counter := counter  + 1;
					fields[counter] := CreateItemAtLocation( mobile.x+1 , mobile.y , mobile.z , 0x398c , 1, me.realm );
					SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
					counter := counter  + 1;
					fields[counter] := CreateItemAtLocation( mobile.x-1 , mobile.y , mobile.z , 0x398c , 1, me.realm );
					SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
					counter := counter  + 1;
					fields[counter] := CreateItemAtLocation( mobile.x , mobile.y+1 , mobile.z , 0x398c , 1, me.realm );
					SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
					counter := counter  + 1;
					fields[counter] := CreateItemAtLocation( mobile.x , mobile.y-1 , mobile.z , 0x398c , 1, me.realm );
					SetObjProperty( fields[counter] , "ByTrueMage" , 1 );
					Sleepms(10);
				endif
			endif
		endif
	endforeach

	var buffParms := {fields, duration};
	Start_script( ":summoning:npcremovefields", buffParms );
endfunction

function ChampFire(me)
	var players := ListMobilesNearLocation( me.x , me.y , me.z , 20 , me.realm );

	var ele := Random(6)+1;
	var element;
	case(ele)
		1: element := "EARTH";
		2: element := "WATER";
		3: element := "FIRE";
		4: element := "AIR";
		5: element := "NECRO";
		6: element := "HOLY";
	endcase

	foreach mobile in players
		if (mobile.cmdlevel < 1)
			if(!mobile.isa(POLCLASS_NPC) ||  mobile.master)
				var los := CheckLineOfSight( me, mobile );
				if(los)
					SpecialChampFire(me, mobile, element);
				endif
			endif
		endif
	endforeach
endfunction

function SummonDrag(me)

	var timescasted := GetObjProperty(me, "timescasted");
	if(!timescasted)
		timescasted := 0;
	elseif (timescasted >= 3)
		return; 
	endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	var utterance := Random(2);

	case (utterance)
        	0: say("Awake, my children!");
        	1: say("Come forth my minions!");
	endcase

	var i;
	for(i:=0; i < 3; i:= i+1)
		var drakes := Random(15);
		case(drakes)
			0: NpcSummonCreature( me , "goldendrake" );
			1: NpcSummonCreature( me , "cavedrake" );
			2: NpcSummonCreature( me , "airdrake" );
			3: NpcSummonCreature( me , "drake1" );  
			4: NpcSummonCreature( me , "poisondrake" );
			5: NpcSummonCreature( me , "sanddrake" );
			6: NpcSummonCreature( me , "wyvern" );
			7: NpcSummonCreature( me , "firedrake" );
			8: NpcSummonCreature( me , "undeaddrake" );
			9: NpcSummonCreature( me , "frostdrake" );
			10: NpcSummonCreature( me , "tidaldrake" );
			11: NpcSummonCreature( me , "stormdrake" );
			12: NpcSummonCreature( me , "rockdrake" );
			13: NpcSummonCreature( me , "celestialdrake" );
			Default: NpcSummonCreature( me , "heavenlydrake" );
		endcase
	endfor
endfunction


function SummonSkelarmy(me)

	var timescasted := GetObjProperty(me, "timescasted");
	if(!timescasted)
		timescasted := 0;
	elseif (timescasted >= 3)
		return; 
	endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	var utterance := Random(4);

	case (utterance)
        	0: say("Awake, my children! Come forth and take your rightful place!");
        	1: say("Come forth my minions!");
        	2: say("I call upon the forces of darkness to attend me!");
        	3: say("Mortal, you shall soon be my slave!");
	endcase

	NpcSummonCreature( me , "skeletalwarrior" );
	NpcSummonCreature( me , "skeletonarcher" );
	NpcSummonCreature( me , "skeletalassassin" );
	NpcSummonCreature( me , "flamingskeleton" );
	NpcSummonCreature( me , "bonemage" );

endfunction

function SummonSkel(me)


	var timescasted := GetObjProperty(me, "timescasted");
	if(!timescasted)
		timescasted := 0;
	elseif (timescasted >= 3)
		return; 
	endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	var utterance := Random(4);

	case (utterance)
        	0: say("Awake, my children! Come forth and take your rightful place!");
        	1: say("Come forth my minions!");
        	2: say("I call upon the forces of darkness to attend me!");
        	3: say("Mortal, you shall soon be my slave!");
	endcase

	NpcSummonCreature( me , "skeleton" );

endfunction

function Gluttons(me)

	var timescasted := GetObjProperty(me, "timescasted");
	if(!timescasted)
		timescasted := 0;
	elseif (timescasted >= 10)
		return; 
	endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	var glutts := Random(2);
	
	case (glutts)
	0: NpcSummonCreature( me , "gluttons" );

	1: NpcSummonCreature( me , "gluttons2" );  
	endcase
endfunction

function Superspider(me)

	var timescasted := GetObjProperty(me, "timescasted");
	if(!timescasted)
		timescasted := 0;
	elseif (timescasted >= 10)
		return; 
	endif

	timescasted := timescasted +1;
	SetObjProperty(me, "timescasted", timescasted);

	NpcSummonCreature( me , "superspider" );

endfunction

Function GetSpellID(spellname)

	var spellid := 0;

	case (spellname)
	"clumsy"	: spellid := 1;
	"feeblemind"	: spellid := 3;
	"heal"		: spellid := 4;
	"magicarrow"	: spellid := 5;
	"nightsight"	: spellid := 6;
	"reactivearmor"	: spellid := 7;
	"weaken"	: spellid := 8;
	"agility"	: spellid := 9;
	"cunning"	: spellid := 10;
	"cure"		: spellid := 11;
	"harm"		: spellid := 12;
	"protection"	: spellid := 15;
	"strength"	: spellid := 16;
	"bless"		: spellid := 17;
	"fireball"	: spellid := 18;
	"poison"	: spellid := 20;
	"teleport"	: spellid := 22;
	"wallofstone"	: spellid := 24;
	"archcure"	: spellid := 25;
	"archprot"	: spellid := 26;
	"curse"		: spellid := 27;
	"firefield"	: spellid := 28;
	"gheal"		: spellid := 29;
	"lightning"	: spellid := 30;
	"manadrain"	: spellid := 31;
	"blade_spirit"	: spellid := 33;
	"dispel_field"	: spellid := 34;
	"reflect"	: spellid := 36;
	"mindblast"	: spellid := 37;
	"paralyze"	: spellid := 38;
	"poisonfield"	: spellid := 39;
	"summon_creature": spellid := 40;
	"dispel"	: spellid := 41;
	"ebolt"		: spellid := 42;
	"explosion"	: spellid := 43;
	"invisibility"	: spellid := 44;
	"masscurse"	: spellid := 46;
	"parafield"	: spellid := 47;
	"reveal"	: spellid := 48;
	"chainlightning": spellid := 49;
	"energy_field"	: spellid := 50;
	"flamestrike"	: spellid := 51;
	"manavamp"	: spellid := 53;
	"massdispel"	: spellid := 54;
	"meteor_swarm"	: spellid := 55;
	"polymorph"	: spellid := 56;
	"earthquake"	: spellid := 57;
	"vortex"	: spellid := 58;
	"resurrect"	: spellid := 59;
	"summon_air"	: spellid := 60;
	"summon_daemon"	: spellid := 61;
	"summon_earth"	: spellid := 62;
	"summon_fire"	: spellid := 63;
	"summon_water"	: spellid := 64;
	"controlundead"	: spellid := 65;
	"darkness"	: spellid := 66;
	"decayingray"	: spellid := 67;
	"spectretouch"	: spellid := 68;
	"abyssalflame"	: spellid := 69;
	"animate_dead"	: spellid := 70;
	"sacrifice"	: spellid := 71;
	"wraithbreath"	: spellid := 72;
	"sorcerersbane"	: spellid := 73;
	"summon_spirit"	: spellid := 74;
	"wraithform"	: spellid := 75;
	"wyvernstrike"	: spellid := 76;
	"kill"		: spellid := 77;
	"liche"		: spellid := 78;
	"plague"	: spellid := 79;
	"spellbind"	: spellid := 80;
	"antidote"	: spellid := 81;
	"owlsight"	: spellid := 82;
	"shiftingearth"	: spellid := 83;
	"summonmammals"	: spellid := 84;
	"calllightning"	: spellid := 85;
	"earthblessing"	: spellid := 86;
	"earthportal"	: spellid := 87;
	"naturestouch"	: spellid := 88;
	"gustofair"	: spellid := 89;
	"risingfire"	: spellid := 90;
	"shapeshift"	: spellid := 91;
	"icestrike"	: spellid := 92;
	"earthspirit"	: spellid := 93;
	"flamespirit"	: spellid := 94;
	"stormspirit"	: spellid := 95;
	"waterspirit"	: spellid := 96;
	"angelicaura"	: spellid := 169;
	"holybolt"	: spellid := 170;
	"seraphimswill" : spellid := 171;
	"wrathofgod"	: spellid := 174;
	"divinefury"	: spellid := 175;
	"astralstorm"	: spellid := 176;
	"enlightenment" : spellid := 177;
	"sanctuary"	: spellid := 179; 
	"summonguardian": spellid := 180;
	"apocalypse"    : spellid := 181;
	"songoflight"   : spellid := 182;
	"songofhaste"   : spellid := 183;
	"songofdefense" : spellid := 184;
	"songofglory"   : spellid := 185;
	"songofcloaking" : spellid := 186;
	"songofremedy"  : spellid := 187;
	"songoflife"    : spellid := 188;
	"songofdismissal" : spellid := 189;
	"songofsirens"  : spellid := 190;
	"songofearth"   : spellid := 191;
	"songofair"     : spellid := 192;
	"songoffire"    : spellid := 193;
	"songofwater"   : spellid := 194;
	"songofbeckon"  : spellid := 195;
	"songoffright"  : spellid := 196;
	"songofsalvation" : spellid := 197;
	endcase

	return spellid;

endfunction

function PrepareToCast(caster, cast_on, spellid)

	if (!CheckLineOfSight( caster, cast_on ))
		return 0;
	endif
	if(spellid == 60 || spellid == 61 || spellid == 62 || spellid == 63  || spellid == 64 || spellid == 74 || spellid == 93 || spellid == 94 || spellid == 95 || spellid == 06 )
		var utterance := Random(4);
		case (utterance)
			0: say("Awake, my children! Come forth and take your rightful place!");
			1: say("Come forth my minions!");
			2: say("I call upon the forces of darkness to attend me!");
			3: say("Mortal, you shall soon be my slave!");
		endcase
		PerformAction(caster,ANIM_CAST_DIR);
		sleep(1);
	elseif (saywords)
		SpeakPowerWords( me, spellid);
		PerformAction(caster,ANIM_CAST_DIR);
		sleep(1);
	endif

	if (GetEffectiveSkill(caster, SKILLID_MAGERY))
		if (!CheckSkill(caster, SKILLID_MAGERY, GetSpellDifficulty( spellid ), 100))
			PlaySoundEffect(caster,0x005d);
			PlayObjectCenteredEffect(caster, 0x3735,10, 30);
			return 0;
		endif
	endif

	if (! ConsumeMana( caster, spellid ))
		return 0;
	endif

	return 1;

endfunction

function cast_offensive_spell(caster, cast_on)

	var thespell := spells[Random( len(spells) +1 )];

	if( thespell == "gheal" )
		CastSpell(caster, caster, thespell);
	else
		CastSpell(caster, cast_on, thespell);
	endif

endfunction

function NpcSummonCreature( caster , npctemplate , sfx := 0x0218 )

	var parms := {};

	parms[1] := caster;
	parms[2] := npctemplate;
	parms[3] := sfx;

	Start_script( ":summoning:npcsummoning" , parms );

endfunction

function SummonsShiftingEarth(caster, cast_on)
	var circle := GetCircle( SPELLID_SHIFTING_EARTH );
	var element := EARTH;
	if(IsInSafeArea(caster))
			EraseObjProperty( caster , "#Casting" );
            return;
    endif
	
	say("Esmagamento Con Pedra");
	PlayObjectCenteredEffect(cast_on, FX_SPELL_SHIFTING_EARTH, SPEED_SPELL_SHIFTING_EARTH, LOOP_SPELL_SHIFTING_EARTH);
	PlaySoundEffect( cast_on, SFX_SPELL_SHIFTING_EARTH );
	
	if(cast_on.isa(POLCLASS_NPC))
		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );
		EraseObjProperty( caster , "#Casting" );
	else
		var immunity := IsProtected( caster , cast_on , circle );
		if( immunity == IMMUNED )
			EraseObjProperty( caster , "#Casting" );
			return;
		endif

		//cast_on := Reflected( caster, cast_on, circle );

		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		if( immunity == CURSED )
			SendSysmessage( cast_on , "One of your equipped items increases the effect of the spell!" );
			magery := magery * 2;
			circle := circle * 2;
		endif
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );

		if( !CanMod( cast_on, "dex" ) )
        	EraseObjProperty( caster, "#Casting" );
        	return;
        endif
		var prot := GetResultingElementalProt( cast_on, element );
		if( prot )
			magery := Cint( magery - ( magery * prot / 10 ) );
		endif
		
		if( magery < 1 )
			magery := 1;
		endif
		
		EraseObjProperty( caster , "#Casting" );
		var mod_amount := Resisted( caster, cast_on, circle, GetModAmount( caster ) );
		var duration := GetModDuration( caster );
		DoTempMod( cast_on, "dex", 0 - mod_amount, duration );
	endif

endfunction

function SummonsGustOfAir(caster, cast_on)
	var circle := GetCircle( SPELLID_GUST_OF_AIR);
	var element := AIR;

	if(IsInSafeArea(caster))
			EraseObjProperty( caster, "#Casting" );
            return;
    endif
	
	say("Gust Do Ar");
	PlayObjectCenteredEffect(cast_on, FX_SPELL_GUST_OF_AIR, SPEED_SPELL_GUST_OF_AIR, LOOP_SPELL_GUST_OF_AIR);
	PlaySoundEffect( cast_on, SFX_SPELL_GUST_OF_AIR1 );
	PlaySoundEffect( cast_on, SFX_SPELL_GUST_OF_AIR2 );
		
	if(cast_on.isa(POLCLASS_NPC))
		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );
		EraseObjProperty( caster , "#Casting" );
	else
		var immunity := IsProtected( caster , cast_on , circle );
		if( immunity == IMMUNED )
			EraseObjProperty( caster , "#Casting" );
			return;
		endif

		//cast_on := Reflected( caster , cast_on , circle );

		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		if( immunity == CURSED )
			SendSysmessage( cast_on, "One of your equipped items increases the effect of the spell!" );
			magery := magery * 2;
			circle := circle * 2;
		endif
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );
		EraseObjProperty( caster, "#Casting" );
	endif

endfunction

function SummonsIceStrike(caster, cast_on)
	var circle := GetCircle( SPELLID_ICE_STRIKE);
	var element := WATER;

	if(IsInSafeArea(caster))
			EraseObjProperty( caster, "#Casting" );
            return;
    endif
	
	say("Geada Com Inverno");
	PlayObjectCenteredEffect(cast_on, FX_SPELL_ICE_STRIKE, SPEED_SPELL_ICE_STRIKE, LOOP_SPELL_ICE_STRIKE);
	PlaySoundEffect( cast_on, SFX_SPELL_ICE_STRIKE1 );
	PlaySoundEffect( cast_on, SFX_SPELL_ICE_STRIKE2 );
	
	if(cast_on.isa(POLCLASS_NPC))
		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );
		EraseObjProperty( caster , "#Casting" );
	else
		var immunity := IsProtected( caster , cast_on , circle );
		if( immunity == IMMUNED )
			EraseObjProperty( caster , "#Casting" );
			return;
		endif

		//cast_on := Reflected( caster , cast_on , circle );

		var magery := GeteffectiveSkill(caster, SKILLID_MAGERY);
		if( immunity == CURSED )
			SendSysmessage( cast_on, "One of your equipped items increases the effect of the spell!" );
			magery := magery * 2;
			circle := circle * 2;
		endif
		var dmg := CalcSpellDamage( caster, cast_on, circle );
		ApplyElementalDamage( caster, cast_on, circle, dmg, element );
		EraseObjProperty( caster, "#Casting" );
	endif

endfunction

function SummonsRisingFire(caster, cast_on)
	var circle := GetCircle( SPELLID_RISING_FIRE);
	var element := FIRE;

	if(IsInSafeArea(caster))
			EraseObjProperty( caster, "#Casting" );
			Print("Cant cast in safe area");
            return;
    endif

	say("Batida Do Fogo");

	if(cast_on.isa(POLCLASS_NPC))
		var circ := circle;
		var dmg := CalcSpellDamage( caster , cast_on , circ , AREA_EFFECT_SPELL );

		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);					
		send_attack( cast_on , caster , SPELLID_RISING_FIRE );
		Sleepms(250);
		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
		PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
		Sleepms(250);
		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
		PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
		ApplyElementalDamage( caster , cast_on , circ , Cint(dmg / 2) , element );

		Sleepms(25);

		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
		send_attack( cast_on , caster , SPELLID_RISING_FIRE );
		Sleepms(250);
		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
		PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
		Sleepms(250);
		PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
		PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
		ApplyElementalDamage( caster , cast_on , circ , Cint(dmg / 2) , element );
	else
		var immunity := IsProtected( caster , cast_on , circle );
		var circ := circle;
		var dmg := CalcSpellDamage( caster , cast_on , circ , AREA_EFFECT_SPELL );

		if( immunity != IMMUNED )
			if( immunity == CURSED )
				circ := circ * 2;
			endif
		
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			send_attack( cast_on , caster , SPELLID_RISING_FIRE );
			Sleepms(250);
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
			Sleepms(250);
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
			ApplyElementalDamage( caster , cast_on , circ , Cint(dmg / 2) , element );
		endif

		Sleepms(25);

		immunity := IsProtected( caster , cast_on , circle );
		circ := circle;
		if( immunity != IMMUNED )
			if( immunity == CURSED )
				circ := circ * 2;
			endif
		
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			send_attack( cast_on , caster , SPELLID_RISING_FIRE );
			Sleepms(250);
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
			Sleepms(250);
			PlayObjectCenteredEffect(cast_on, FX_SPELL_RISING_FIRE, SPEED_SPELL_RISING_FIRE, LOOP_SPELL_RISING_FIRE);
			PlaySoundEffect( cast_on , SFX_SPELL_RISING_FIRE );
			ApplyElementalDamage( caster , cast_on , circ , Cint(dmg / 2) , element );
		endif
	endif

	EraseObjProperty( caster , "#Casting" );

endfunction

function SummonsSpectreTouch(caster, cast_on)
	var circle := GetCircle( SPELLID_SPECTER_TOUCH);
	var plane := NECRO;

	if(IsInSafeArea(caster))
			EraseObjProperty( caster, "#Casting" );
			Print("Cant cast in safe area");
            return;
    endif

	say("Enervare");

	if(cast_on.isa(POLCLASS_NPC))
		var circ := circle;
		var dmg := CalcSpellDamage( caster , cast_on , circ , AREA_EFFECT_SPELL );
		Send_attack( cast_on , caster , SPELLID_SPECTER_TOUCH );
		PlaySoundEffect( cast_on , SFX_SPELL_SPECTER_TOUCH );
		PlayStationaryEffect( cint(cast_on.x), cint(cast_on.y), cint(cast_on.z) , FX_SPELL_SPECTER_TOUCH , SPEED_SPELL_SPECTER_TOUCH , LOOP_SPELL_SPECTER_TOUCH, 0, cast_on.realm );
		ApplyplanarDamage( caster , cast_on , circ , dmg , plane );
	else
		var immunity := IsProtected( caster , cast_on , circle );
		var circ := circle;
		if( immunity != IMMUNED )
			if( immunity == CURSED )
				circ := circ * 2;
			endif
		
			var dmg := CalcSpellDamage( caster , cast_on , circ , AREA_EFFECT_SPELL );
			Send_attack( cast_on , caster , SPELLID_SPECTER_TOUCH );
			PlaySoundEffect( cast_on , SFX_SPELL_SPECTER_TOUCH );
			PlayStationaryEffect( cint(cast_on.x), cint(cast_on.y), cint(cast_on.z) , FX_SPELL_SPECTER_TOUCH , SPEED_SPELL_SPECTER_TOUCH , LOOP_SPELL_SPECTER_TOUCH, 0, cast_on.realm );
			ApplyplanarDamage( caster , cast_on , circ , dmg , plane );
		endif
	endif

	EraseObjProperty( caster , "#Casting" );

endfunction