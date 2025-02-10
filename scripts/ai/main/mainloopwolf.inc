include "include/areas";
include "include/npcboosts";

function main_AI_loop()

    var ev;
    var wanders := 60;
    var hatelist := "animal cat barker tamed chicken sheep bird";

    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif
    
	if (mydestx != 0)
		WalkTowardLocation(mydestx, mydesty);
		if ( (me.x == mydestx and me.y == mydesty) or ( steps > 10 ) )
			mydestx := 0;
		endif
	else	
		wander();
		wanders := wanders +1;
	endif

	if (wanders > 60)
		wanders :=0;
		ev := sleepmode();
	else
		ev := os::wait_for_event( 2 );
	endif

	if (ev)
        repeat
	    case (ev.type)
		SYSEVENT_ENTEREDAREA:
		SYSEVENT_LEFTAREA:
			if ( ( me.npctemplate == "dog" ) and (!ev.source.npctemplate) )
				case (Random(2))
				0: PlaySoundEffect(me,0x86);
				1: PlaySoundEffect(me,0x87);
				endcase
			elseif ( (ev.source.script) and ( hatelist[ev.source.script] ) )
				  get_help( ev.source );
	                    Fight( ev.source );
			elseif ( ev.source.npctemplate == me.npctemplate)
				WalkToward(ev.source);
				ev := wait_for_event(1);
				WalkToward(ev.source);
			endif
                SYSEVENT_DAMAGED:
                SYSEVENT_ENGAGED:
                    if(!IsInSafeArea(me))
		                if (ev.source)
		    	            get_help( ev.source );
                    	    Fight( ev.source );
		                endif
                    endif
		EVID_ALL_ATTACK_CMD:
                Fight( ev.source );
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor(me);
			WalkTowardLocation(mydestx, mydesty);
            endcase
        until (! (ev := os::wait_for_event(1)) );
	endif

   endwhile      

endfunction        

function look_around()

	var hatelist := "animal cat barker tamed chicken sheep bird";
	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if ( (critter.script) and ( hatelist[critter.script] ) )
			Fight( critter );
			return;
		endif
	endforeach

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA + SYSEVENT_LEFTAREA, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA + SYSEVENT_LEFTAREA);

endfunction

function get_help( opponent )

	var ev Array;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, HALT_THRESHOLD, me.realm )
		if ( (critter.npctemplate == me.npctemplate) and (critter.script != "tamed") && !GetObjProperty(ev.source, "NPCAttack") and (!critter.warmode) )
			SendEvent(critter, ev);
		endif
	endforeach

endfunction
