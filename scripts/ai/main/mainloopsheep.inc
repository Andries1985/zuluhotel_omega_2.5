const GFX_UNSHORN_SHEEP := 0xcf;
const GFX_SHORN_SHEEP := 0xdf;
include "include/attributes";
include "include/npcboosts";

function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_wander := 0;
    var next_regrow := readgameclock()+cint(Random(1800));
    var nextbark := readgameclock() + 30;

    CheckSheepAmount();
    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

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
			ev := sleepmode();
	    	endif
	else
		ev := os::wait_for_event( 5 );
	endif

	if (ev)
        repeat
	    case (ev.type)
                SYSEVENT_DAMAGED:
                SYSEVENT_ENGAGED:
		    RunLikeHell( ev.source );
		EVID_ALL_FOLLOW_CMD:
			mydestx := ev.x;
			mydesty := ev.y;
			steps := 0;
			drop_anchor();
			WalkTowardLocation(mydestx, mydesty);
            endcase
        until (! (ev := os::wait_for_event(1)) );
	endif

	if (readgameclock() > next_regrow)
		next_regrow := readgameclock()+cint(1800+Random(300));
		if ( me.graphic == GFX_SHORN_SHEEP )
			me.graphic := GFX_UNSHORN_SHEEP;
		endif
		CheckSheepAmount();
	endif

	if ( readgameclock() > nextbark )
		nextbark := readgameclock() + ( (Random(5)+1) * 60 );
		case (Random(2))
		0: PlaySoundEffect(me,0xD7);
		1: PlaySoundEffect(me,0xD8);
		endcase
	endif

   endwhile      

endfunction        

function RunLikeHell(opponent)

	var ev, sleepdelay;
	while ( (opponent) and not 
	    (opponent.dead or opponent.hidden or opponent.concealed) and 
	    (distance(me,opponent) < 10) ) 

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
	    Runawayfrom( opponent );
		Sleepms(sleepdelay);
        repeat
	    case (ev.type)
                SYSEVENT_DAMAGED:
		    if ( Random(3) == 1 )
                    	Fight( ev.source );
		    	return;
                    endif
            endcase
        until (! (ev := os::wait_for_event(1)) );

	endwhile

endfunction

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED);

endfunction

function CheckSheepAmount()

	var DeathChance := -4;
	Foreach npc in ListMobilesNearLocation(me.x, me.y, me.z, 4, me.realm)
		if (npc.npctemplate == "sheep")
			deathchance := deathchance + 5;
		endif
	endforeach

	if ( Random(500) < DeathChance )			
		ApplyRawdamage( me, GetMAXHP(me) + 3 );
	endif

endfunction
