//Safe-zone testing wasnt added here. Lets hope no non-invulnerable blue npcs will be running around.

include "ai/main/npcinfo";
include "include/npcboosts";

function main_AI_loop()

	var LOOTSTUFF := imalooter(me.npctemplate);

	if ( getobjproperty(me,"summoned") ) LOOTSTUFF := 0; endif

    var ev;
    var wanders := 60;
	var next_wander := ReadGameClock()+2;
    var next_loot := readgameclock() + 20;

    EnableMainEvents();

    look_around();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif
    
	if (LOOTSTUFF)
		if (ReadGameClock() > next_loot)
			grabloot();
			next_loot := ReadGameClock() + 10;
		endif
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
                SYSEVENT_SPEECH:
		    if (!ev.source.npctemplate)
		    	next_wander := readgameclock()+20;
		    	check_speech(ev.text, ev.source);
		    endif
                SYSEVENT_DAMAGED:
                SYSEVENT_ENGAGED:
			Run( ev.source );
		SYSEVENT_ITEM_GIVEN:
		    next_wander := readgameclock()+60;
		    TakeItem(ev.source,ev.item);
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor(me);
			WalkTowardLocation(mydestx, mydesty);
            endcase
        until (! (ev := os::wait_for_event(2)) );
	endif

   endwhile      

endfunction        

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );
    EnableEvents( SYSEVENT_SPEECH , 3 );
    EnableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction
