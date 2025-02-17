include "include/areas";
include "include/attributes";
include "include/npcboosts";


function main_AI_loop()

    var ev;
    var wanders := 60;
    var mynoise := bark();

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
		        if (!ev.source.npctemplate)
		    	    PlaySoundEffect(me, mynoise);
		        endif
                SYSEVENT_DAMAGED:
                    if(!IsInSafeArea(me))
                        Fight( ev.source );
                    endif
                SYSEVENT_ENGAGED:
                    if(!IsInSafeArea(me))
		                RunLikeHell( ev.source );
                    endif
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
		    if ( Random(3)==1 )
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

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);

endfunction

function Bark()

	var noise := 0;
	case (me.graphic)
	0x06://bird
		case (Random(3))
		0: noise := 0x1B;
		1: noise := 0x18;
		2: noise := 0x277;
		endcase
	0xc9://cat	
		noise := 0x6B;
	0xe7:
	0xe9:
	0xe8:
	0xd8://cow
		noise := 0x7A;
	0xd9://dog
		case (Random(2))
		0: noise := 0x86;
		1: noise := 0x87;
		endcase
	0xd1://goat
		case (Random(2))
		0: noise := 0x9A;
		1: noise := 0x9B;
		endcase	
	0xcb://pig
		noise := 0xC6;
	0xcc:
	0xc8:
	0xe2:
	0xe4://horses
		case (Random(4))
		0: noise := 0xA9;
		1: noise := 0xAA;
		2: noise := 0xAB;
		3: noise := 0xAC;
		endcase
	0x51://toad
		case (Random(4))
		0: noise := 0x267;
		1: noise := 0x268;
		2: noise := 0x26A;
		3: noise := 0x26D;
		endcase
	0xd3://bears
		case (Random(5))
		0: noise := 0xA4;
		1: noise := 0xA5;
		2: noise := 0xA6;
		3: noise := 0xA7;
		4: noise := 0xA8;
		endcase
	0x1d://gorilla
		case (Random(3))
		0: noise := 0x9F;
		1: noise := 0xA0;
		2: noise := 0xA2;
		endcase
	0x97://dolphin
		case (Random(3))
		0: noise := 0x8b;
		1: noise := 0x8c;
		2: noise := 0x8d;
		endcase
	endcase

	return noise;

endfunction
