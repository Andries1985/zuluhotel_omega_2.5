include "include/random";
include "include/spelldata";
include "include/dismount";
include "include/dotempmods";

function process_combat_event(ev)
  ProcessDamageTriggers(ev.source);
endfunction

function in_combat_event_loop(opponent)
  ProcessDamageTriggers(opponent);
endfunction

function post_combat()
  DisableCombatEvents();
  EnableMainEvents();

  SetWarMode(0);
  SetOpponent(0);

  sleep(1);
  look_around();
endfunction

function prepare_for_fight(opponent)
  DisableMainEvents();
  EnableCombatEvents();
endfunction

function ProcessDamageTriggers(tgt)

var currTime := ReadGameClock();
var nextPoly := GetObjProperty(me, "#nextPoly");
var nextSlap := GetObjProperty(me, "#nextSlap");

if(!nextPoly || nextPoly < currTime)
  if((Random(3)+1) == 3)
	SetObjProperty(me, "#nextPoly", ReadGameClock() + 60);
    PolymorphOpponent(tgt);
    return;
  endif
endif
  
if(!nextSlap || nextSlap < currTime)
  if(Random(5)+1 == 5)
    SlapRandomOpponent();
	SetObjProperty(me, "#nextSlap", ReadGameClock() + 10);
	RestartScript(me);  //Retarget fun
    return;
  endif
endif
  
  PolymorphSelf();
endfunction

function PolymorphSelf()
  if(GetHp(me) <= GetMaxHP(me)/2 && me.graphic != 0x2d)
	  PrintTextAbove(me, "You cannot not defeat me!");
	  sleep(1);
	  PlayStationaryEffect( me.x, me.y, me.z, 0x3728, 7, 0x10, 0, me.realm);
	  sleepms(100);
	  PlayStationaryEffect( me.x, me.y, me.z, 0x3728, 7, 0x10, 0, me.realm);
	  me.graphic := 0x2d;
  endif
endfunction

function PolymorphOpponent(who)
  // TODO: Check if attacker wears savage kin paint to prevent polymorph
  // TODO: Find out how to make the polymorphee grey.
  // Polymorph attacker into attackable Ratman. Dismount is to prevent clipping.

  if(who.graphic == 0x2a || who.dead)
    return;
  endif
  
  if( CanMod( who , "poly" ) )
	  dismount(who);
	  who.graphic := 0x2a;
	  PolymorphMobile(who, 0x2a, 0, 60);
	  PrintTextAbove(me, "You will join my legion, "+ who.name +"!");
  endif
  return;
endfunction

function SlapRandomOpponent()
  var victim, slapthat;
  var victims := ListMobilesNearLocation(me.x, me.y, me.z, 20, me.realm);
  foreach v in victims
	slapthat := 1;
    if(v == me || (v.isa(POLCLASS_NPC) && v.script != "tamed") || v.cmdlevel)
		slapthat := 0;
	endif

	if(slapthat)
		victim := v;

		PlayStationaryEffect( me.x, me.y, me.z, 0x3728, 7, 0x10, 0, me.realm);
		MoveObjectToLocation(me, victim.x, victim.y, victim.z, victim.realm, MOVEOBJECT_FORCELOCATION);
		PlayStationaryEffect( me.x, me.y, me.z, 0x3728, 7, 0x10, 0, me.realm);

		PrintTextAbove(me, "*slap*");
		ApplyRawDamage(victim, 100);

		sleepMS(200);
    endif
  endforeach
endfunction
