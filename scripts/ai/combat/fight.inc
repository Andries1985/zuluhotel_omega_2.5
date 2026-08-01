
include "include/random";
include "include/npcboosts";
include "ai/main/npcinfo";
include "include/attributes";
include "ai/main/lootblockers";

function Fight( opponent )

	if ((opponent.cmdlevel > 0) or (opponent.serial == me.serial))
        	setwarmode(0);
        	opponent := 0;
		return;
	endif

	if (opponent.npctemplate == "humuc")
        	setwarmode(0);
        	opponent := 0;
		return;
	endif

	var oldprio := set_priority(50);

	SetOpponent( opponent );
    	prepare_for_fight( opponent );
	turntoward( opponent );

	if ( GetObjProperty( me, "keywordbattlecry") )
		Say ( GetObjProperty( me, "keywordbattlecry") );
	endif

	var loops := 0;
	var steps := 0;
    	var ev;
	var waittime := 0;
	var randomruns := 0;
	var oldx := opponent.x;
	var oldy := opponent.y;
	var oldd := distance(me,opponent);
	var cd;

	var wasmoving:=1;
    while ( (opponent) and not 
	    (opponent.dead or opponent.hidden or opponent.concealed) and 
	    (distance(me,opponent) < 20) ) 

		if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
			SetObjProperty(me, "lbc", ReadGameClock()+5);
			checkmymod(me);
		endif

	cd := CloseDistance( opponent );
	case (cd)
	0:
		//Closedistance returned 0, which means the mob needs to move
		wasmoving := 1;
		loops := loops + 1;
		steps := steps + 1;
		waittime := 0;
		if ( ( oldx >= (opponent.x-1) and (oldx <= opponent.x+1)) and 
		     ( oldy >= (opponent.y-1) and (oldy <= opponent.y+1)) and (oldd > 1) )
			//our opponent hasn't moved much but we did, so:
			if ( (steps > 4) and (oldd <= distance(me,opponent)) )
				//after 4 steps we're not closing in, so lets do something else:
				randomruns := randomruns + 1;
				if ( runrandom(opponent, randomruns) )
					PeaceMade();
					return;
				endif
				oldd := distance(me,opponent);
				steps := 0;
				if ( randomruns > 5 )
					//screw it, we can't get to him
					flee(opponent);
				endif
			else
				//if we're closing in, lets reset the distance and steps
				if ( distance(me,opponent) < oldd )
					steps := 0;
					oldd := distance(me, opponent);
				endif
			endif
		else
			//we moved, but so did our opponent, so lets chase him longer:
			oldx := opponent.x;
			oldy := opponent.y;
			steps := 0;
			if ( loops > 50 )
				//screw it, we can't catch him
				flee(opponent);
			endif
		endif
	1:
		//Closedistance returned 1, which means the mob doesn't need to move
		//unless he can't see his opponent
		if (wasmoving) TurnToward(opponent); endif
		wasmoving := 0;

		steps := 0;
		randomruns := 0;
		if ( CheckLineOfSight(me,opponent) )
			loops := 0;
			waittime := 1;
			EraseObjProperty(me,"#flees");
		else
			loops := loops +1;
			waittime := 1;
			RunTowardLocation(me.x+(Random(3)-1), me.y+(Random(3)-1));
		endif			
	2:
		//closedistance returned 2, which means the mob was peacemade:
		PeaceMade();
		return;
	3:
		//closedistance returned 3, which means the mob is running away
		loops := 0;//closedistance maintains its own runaway counter
		steps := 0;
		randomruns := 0;
		waittime := 0;
		wasmoving := 1;
	endcase

	in_combat_event_loop(opponent);

	ev := wait_for_event( waittime );
	case (ev.type)
		SYSEVENT_DAMAGED:
		SYSEVENT_ENGAGED:
			if (ev.source)
			  process_combat_event(ev);
			  if (Random(6)==1)
				if ((!ev.source.npctemplate) or ev.source.script == "tamed" || GetObjProperty(ev.source, "NPCAttack") )
					opponent := ev.source;
			  		SetOpponent( opponent );
					turntoward( opponent );
				endif
			  endif
			endif
                EVID_PEACEMADE:
			PeaceMade();
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

function flee(opponent)

	EraseObjProperty(me,"killme");

	if ( (me.script == "immobile") or (me.script == "immobilespell") or (me.frozen) or (me.paralyzed) )
		sleep(1);
		return;
	endif

	var numflees := getobjproperty(me,"#flees");

	if (numflees)
		if ( numflees > 10 )
			// MoveCharacterToLocation(me,5288,1176,0,MOVECHAR_FORCELOCATION);
			//RevokePrivilege( me, "invul" );
			//SetObjProperty(me,"guardkill",1);
			//damage( me, me.maxhp + 3 );
			EraseObjProperty( me, "#flees");
		else
			numflees := numflees + 1;
		endif
	else
		numflees := 1;
	endif

	setobjproperty(me,"#flees",numflees);

	var runs := 0;
	var ev, sleepdelay;

	while ( (distance(me, opponent) < 10) and (runs < 25) )

		if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
			SetObjProperty(me, "lbc", ReadGameClock()+5);
			checkmymod(me);
		endif

	var sleepdelay := 300 - (CInt(GetDexterity(me)) * 1.5);
	if (sleepdelay < 0) 
		sleepdelay := 0; 
	endif
	var MoveSpeed := GetObjProperty(me, "MoveSpeed");
	if(MoveSpeed)
		sleepdelay := GetSpeed(me, MoveSpeed);
	endif
		WalkAwayFrom(opponent);
		Sleepms(sleepdelay);

		in_combat_event_loop(opponent);
		runs := runs +1;
		ev := wait_for_event(0);
		if (ev.type == SYSEVENT_DAMAGED)
			process_combat_event(ev);
		endif
	endwhile

	RestartScript( me );

endfunction

function runrandom(opponent, numruns := 2)

	if ( (me.script == "immobile") or (me.script == "immobilespell") )
		sleep(1);
		return 0;
	endif

	var stepstotake := (numruns)+4;

	var tox := me.x;
	var toy := me.y;

	var highx := tox - opponent.x;
	if ( highx < 0 ) highx := 0 - highx; endif
	var highy := toy - opponent.y;
	if ( highy < 0 ) highy := 0 - highy; endif

	case ( highy > highx )
	0:	//run along the x access
		case ( Random(2) )
		0:	//to the right
			toy := (me.y - stepstotake);
			if (Random(2)) tox := opponent.x; endif //to his x
		1:	//to the left
			toy := (me.y + stepstotake);
			if (Random(2)) tox := opponent.x; endif //to his x
		endcase
	1:
		//run along the y access
		case( Random(2) )
		0:	//to the right
			tox := (me.x - stepstotake);
			if (Random(2)) toy := opponent.y; endif //to his y
		1:	//to the left
			tox := (me.x + stepstotake);
			if (Random(2)) toy := opponent.y; endif //to his y
		endcase
	endcase

	var runs := 0;
	var ev;
	while ( (distance(me, opponent) > 1) and (runs <= stepstotake) )

		if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
			SetObjProperty(me, "lbc", ReadGameClock()+5);
			checkmymod(me);
		endif
		
		if(GetObjProperty (me, "Boss") || GetObjProperty (me, "SuperBoss") || GetObjProperty (me, "Champion"))
			grabblocker();
		endif
		WalkTowardLocation( tox, toy );
		in_combat_event_loop(opponent);
		runs := runs +1;
		ev := wait_for_event(0);
		if (ev)
			case (ev.type)
			EVID_PEACEMADE: return 1;
			SYSEVENT_DAMAGED:
				process_combat_event(ev);
				if (ev.source.serial == opponent.serial)
					flee(opponent);
				endif
			endcase
		endif
		if ( (me.x == tox) and (me.y == toy) )
			return 0;
		endif
	endwhile

	return 0;

endfunction

function PeaceMade()

	SetWarMode( 0 );
	SetOpponent( 0 );
	sleep(1);
	DisableCombatEvents();
	EnableMainEvents();
	// EraseObjProperty(me,"#fighttime");

endfunction