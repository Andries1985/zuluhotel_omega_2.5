include "ai/main/npcinfo";
include "include/client";
include "include/areas";
include "include/attributes";
include "include/npcboosts";


function main_AI_loop()

	var ev;
	var wanders := 60;
	var next_loot := readgameclock() + 20;

	var LOOTSTUFF := imalooter(me.npctemplate);

	if ( getobjproperty(me,"summoned") ) LOOTSTUFF := 0; endif


	EnableMainEvents();
   

	   look_around();


	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	while (1)		
        if(!IsInSafeArea(me))
            var hiders := ListMobilesNearLocationEx( me.x , me.y , me.z , HALT_THRESHOLD , LISTEX_FLAG_HIDDEN, me.realm );
            if( Len(hiders) )
                foreach mobile in hiders
                    var hiding := GetEffectiveSkill( mobile , SKILLID_HIDING );
                    if( !mobile.npctemplate or mobile.script == "tamed" || GetObjProperty(mobile, "NPCAttack"))
                        if( CheckSkill( me , SKILLID_DETECTINGHIDDEN , CInt(hiding) , CInt(hiding*20) ) )
                            mobile.hidden := 0;
							PrintTextAbovePrivate( mobile , "You have been discovered!" , mobile );
                            Fight( mobile );
                            break;
                        endif
                    endif
                endforeach
            endif
		endif

		if (LOOTSTUFF)
			if (ReadGameClock() > next_loot)
				grabloot();
				next_loot := ReadGameClock() + 10;
			endif
		endif

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
			wanders := wanders + 1;
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
                        if(!IsInSafeArea(me))
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
						drop_anchor(me);
						WalkTowardLocation(mydestx, mydesty);
				endcase
			until (!(ev := os::wait_for_event(2)));
		endif

	endwhile

endfunction

function look_around()

	foreach critter in ListMobilesInLineOfSight( me, HALT_THRESHOLD )
		if (!critter.npctemplate or critter.script == "tamed")
			Fight( critter );
		endif
	endforeach

	var hiders := ListMobilesNearLocationEx( me.x , me.y , me.z , HALT_THRESHOLD , LISTEX_FLAG_HIDDEN );
	if( len(hiders) )
		foreach mobile in hiders
			var hiding := GetEffectiveSkill( mobile , SKILLID_HIDING );
			if( !mobile.npctemplate or mobile.script == "tamed")
				if( CheckSkill( me , SKILLID_DETECTINGHIDDEN , CInt(hiding) , Cint(hiding*20) ) )
					mobile.hidden := 0;
					PrintTextAbovePrivate( mobile , "You have been discovered!" , mobile );
					Fight( mobile );
					break;
				endif
			endif
		endforeach
	endif

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
	var ev Array;
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
