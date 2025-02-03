

include "include/random";

function process_combat_event(ev)

	spellattack(ev.source);

	case (ev.type)
		SYSEVENT_DAMAGED:
			if ( (GetHP(me) < (GetStrength(me)/2)) and !(GetObjProperty( me , "Type" ) == "Undead" ) )
				CastSpell(me,me,"gheal");
				if( me.poisoned )
					CastSpell(me,me,"cure");
				endif
			endif
                        if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
	endcase

endfunction

function spellattack(opponent)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(2)==1)
		sleep(1);
		cast_offensive_spell(me,opponent);
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

	foreach myfriend in listmobilesnearlocation(me.x, me.y, me.z, HALT_THRESHOLD, me.realm)
		if (myfriend.npctemplate and myfriend.script != "tamed" && !GetObjProperty(myfriend, "NPCAttack") )
			healerstuff(myfriend);
		endif
	endforeach

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);

endfunction       

function prepare_for_fight(opponent)

	if( !GetObjProperty( me , "mr" ) )
		CastSpell( me , me , "reflect" );
	endif

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	num_casts := 2;

	sleep(1);
	look_around();

endfunction


function HealerStuff(mobile)

if (!mobile.npctemplate or mobile.npctemplate["guard"] or mobile.script == "tamed")
        return;
endif
   
	if( (mobile.poisoned) and (CheckLineOfSight(me,mobile)) )
		TurnToward(mobile);
		CastSpell(me,mobile,"cure");
	elseif( (GetHP(mobile) < (GetMaxHP(mobile)/2)) and (CheckLineOfSight(me,mobile))  )
		TurnToward(mobile);
		CastSpell(me,mobile,"gheal");
	endif
    
endfunction
