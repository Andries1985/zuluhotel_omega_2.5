include "ai/main/npcinfo";
include "include/areas";
include "include/npcboosts";


function main_AI_loop()

    var ev;
    var wanders := 60;

    EnableMainEvents();

    if(!IsInSafeArea(me))
        look_around();
    endif

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

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

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif

	if (ev)
	  repeat
            case (ev.type)
                SYSEVENT_ENTEREDAREA:
                    if ( (!ev.source.npctemplate) or (ev.source.script == "tamed") || GetObjProperty(ev.source, "NPCAttack"))
								var resProt := GetObjProperty(ev.source, "ResProtection");
								if(resProt)
									if(resProt < readgameclock())
										Fight( ev.source );
									endif
								else
										Fight( ev.source );
								endif
                    endif
		EVID_ALL_ATTACK_CMD:
		        Fight( ev.source );
        SYSEVENT_ENGAGED:
        SYSEVENT_DAMAGED:
		        if (ev.source)
		    	    get_help( ev.source );
		    	    fight(ev.source);
            endif
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor();
			WalkTowardLocation(mydestx, mydesty);
            endcase
	  until (!(ev := os::wait_for_event(2)));
	endif

   endwhile      

endfunction       

function look_around()

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
          if (!critter.npctemplate or critter.script == "tamed" || GetObjProperty(critter, "NPCAttack"))
                Fight( critter );
          endif
	endforeach

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction

function get_help( opponent )

	var inev;
	var ev := {};
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 4, me.realm )
		if ( (critter.npctemplate) and (critter.script != "tamed") and (!critter.warmode) and
		     (critter.serial != me.serial) )
			SendEvent(critter, ev);
		endif
		inev := wait_for_event(0);
		if (inev.type == EVID_ALL_ATTACK_CMD)
			return;
		endif
	endforeach

endfunction

