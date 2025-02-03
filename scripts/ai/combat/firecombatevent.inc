include "include/spelldata";
include "include/random";
include "include/attributes";

function process_combat_event(ev)

	spellattack(ev.source);
        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
        endif

endfunction

function spellattack(opponent);

        cast_meteor(me);

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);
         
endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction


function cast_meteor( caster )

	var nextbreath := GetObjProperty(me,"#lastbreath");

	if ( (nextbreath) and (nextbreath > ReadGameClock() ) )
		return;
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + cint(Random(10)+10) );

	var targets := ListMobilesNearLocation(me.x, me.y, me.z, 4, me.realm);
	PerformAction(me,0x0c);

	var dmg := Cint( GetHP(me) / 30 ) + RandomDiceStr("1d15");
		
	foreach mobile in targets
		if ( (mobile) and (!mobile.npctemplate or mobile.script == "tamed" || GetObjProperty(mobile, "NPCAttack")) and (!mobile.cmdlevel))
			if(CheckLineOfSight( mobile, caster ))
				PlayMovingEffect(caster, mobile ,MOVING_EFFECT_FIREBALL, 10,1,1);
				ApplyElementalDamageNoResist( mobile , caster , dmg , FIRE );
			endif
		endif
	endforeach

endfunction
