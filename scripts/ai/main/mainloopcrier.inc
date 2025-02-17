include "include/npcboosts";

function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_wander := readgameclock();
    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	SetAnchor( cint(me.x), cint(me.y), 4, 50 );

    while (1)

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif
	
	if (readgameclock() > next_wander)
		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x == mydestx and me.y == mydesty) or ( steps > 10 ) )
				mydestx := 0;
			endif
		else	
			wander();
			wanders := wanders +1;
		endif

		next_wander := readgameclock()+5;
		if (wanders > 60)
			wanders :=0;
			walkhome();
			ev := sleepmode();
    		endif
	else
		ev := os::wait_for_event( 2 );
	endif

	if (ev)
        repeat
            case (ev.type)
                SYSEVENT_SPEECH:
		    if (!ev.source.npctemplate)
		    	if (lower(ev.text) == "news")
				wanders :=0;
		    		next_wander := readgameclock()+60;
		    		SpoutTheNews();
				elseif (lower(ev.text) == "add" and ev.source.cmdlevel >= 4)
				wanders :=0;
		    	next_wander := readgameclock()+60;
		    		var news := GetGlobalProperty("news"); 
					var newnews := SendTextEntryGump( ev.source , "What news item shall I tell people?", maximum := 280);
					if (!newnews)
        				newnews := news;
					endif
					setGlobalProperty("news", newnews);
					say("I shall take your message to the people");
				endif
			endif
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
                    Fight( ev.source );
		    ReturnHome();
		EVID_ALL_FOLLOW_CMD:
			mydestx := cint(ev.x);
			mydesty := cint(ev.y);
			steps := 0;
			SetAnchor( mydestx, mydesty, 4, 50 );
			WalkTowardLocation(mydestx, mydesty);
		SYSEVENT_ENTEREDAREA:
			if (randomint(5)>4)
				SpoutTheNews();
			endif
		SYSEVENT_LEFTAREA:
            endcase

        until (! (ev := os::wait_for_event(2)) );
	endif

   endwhile      

endfunction        

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );
    EnableEvents( SYSEVENT_SPEECH, 3 );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function SpoutTheNews()

	var news := GetGlobalProperty("news");
	if (!news)
		Say("No news is good news.");
		return;
	endif
	Say(news);
	sleep(2);

endfunction

function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveObjectToLocation(me,myhome[1],myhome[2],myhome[3], me.realm, MOVEOBJECT_FORCELOCATION);

endfunction
