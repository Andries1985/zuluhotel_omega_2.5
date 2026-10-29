//raider fight ai
//ver 0.7c
//Now compatible with most other ai includes

include "include/npcboosts";

function Fight( opponent )

	if ((opponent.cmdlevel > 0) or (opponent.serial = me.serial))
        	setwarmode(0);
        	opponent := 0;
		return;
	endif

	// Nothing to fight: return before post_combat(). Otherwise look_around() picks the same ghost
	// again a second later, one call deeper each time, until the core kills the AI script. Leave
	// war mode as post_combat() would; the events are only switched by prepare_for_fight() below.
	if (opponent.dead or opponent.hidden or opponent.concealed)
		SetWarMode(0);
		SetOpponent(0);
		return;
	endif

	if (opponent.multi.serial)
		if (me.multi.serial != opponent.multi.serial)
			flee(opponent);
			return;
		endif
	endif

	SetOpponent( opponent );
    prepare_for_fight( opponent );
	turntoward( opponent );


	RemoveDEXUpgd();

	if (randomint(2)=1 and (!opponent.npctemplate))
		var defendpoints := GetobjProperty(opponent, "#imadefender");
		if (!defendpoints)
			defendpoints := 1;
		else
			defendpoints := defendpoints + 1;
		endif
		SetobjProperty(opponent, "#imadefender",  defendpoints);
	endif

	var loops := 0;
    var ev;
	var waittime := 0;

	DisableEvents( SYSEVENT_LEFTAREA + SYSEVENT_ENTEREDAREA);

    while ( (opponent) and not (opponent.dead or opponent.hidden or opponent.concealed)) 

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif

	case(CloseDistance( opponent ) )
		default:
		0:	
			loops := loops + 1;
			waittime := 0;
		1:
			loops := 0;
			waittime := 1;
			turntoward(opponent);
		2:
			SetWarMode( 0 );
			SetOpponent( 0 );
			sleep(1);
			DisableCombatEvents();
			EnableMainEvents();
			return;
	endcase

	in_combat_event_loop2(opponent, loops);

	ev := wait_for_event( waittime );
	case (ev.type)
        SYSEVENT_DAMAGED:
		SYSEVENT_ENGAGED:
			if (ev.source)
			  process_combat_event(ev);
			  if (RandomInt(6)=1)
				if ((!ev.source.npctemplate) or (ev.source.script = "tamed") || GetObjProperty(ev.source, "NPCAttack"))
					opponent := ev.source;
			  		SetOpponent( opponent );
					turntoward( opponent );
				endif
			  endif
			endif
        EVID_PEACEMADE:
			SetWarMode( 0 );
			SetOpponent( 0 );
			sleep(1);
			DisableCombatEvents();
			EnableMainEvents();
			return;
	endcase

    endwhile

    post_combat();
      
endfunction

function EnableCombatEvents()

    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + EVID_PEACEMADE );

endfunction

function DisableCombatEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + EVID_PEACEMADE );

endfunction

//Ultra simple flee =)
function flee(opponent)
	runawayfrom(opponent);
endfunction
