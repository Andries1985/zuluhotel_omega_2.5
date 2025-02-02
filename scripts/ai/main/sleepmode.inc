include "include/attributes";


include "include/client";

function sleepmode()

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 20, me.realm );
		if (!critter.npctemplate)
			return 0;
		endif
	endforeach

	if(me.script != "merchant")
	var chkski := GetEffectiveSkill( me , SKILLID_DETECTINGHIDDEN );
		if(chkski)
			var hiders := ListMobilesNearLocationEx( me.x, me.y, me.z, 5, LISTEX_FLAG_HIDDEN, me.realm );
			foreach mobile in hiders
				var hiding := GetEffectiveSkill( mobile, SKILLID_HIDING );
				if( !mobile.npctemplate or mobile.script == "tamed")
					if( CheckSkill( me, SKILLID_DETECTINGHIDDEN, CInt(hiding), CInt(hiding*20) ) )
						mobile.hidden := 0;
						PrintTextAbovePrivate( mobile , "You have been discovered!" , mobile );
						return 0;
					endif
				endif
			endforeach
		endif
	endif

	if (GetObjProperty(me, "killme"))
		ApplyRawDamage(me, GetMaxHP(me)+1);
	endif

	Eraseobjproperty(me, "#flees");

	DisableMainEvents();
	if ( (GetEffectiveSkill(me, 21) > 0 ) and (me.script != "merchant") )
		me.hidden := 1;
		EnableEvents(SYSEVENT_ENTEREDAREA, 4);
	else
		EnableEvents(SYSEVENT_ENTEREDAREA, 18);
	endif
	
	EnableEvents(SYSEVENT_ENGAGED);
	EnableEvents(SYSEVENT_DAMAGED);

	var ev;

	while (1)

		ev := os::wait_for_event( 120 );

		if (ev)

			case (ev.type)
				SYSEVENT_ENGAGED:
				SYSEVENT_DAMAGED:
				EVID_ALL_ATTACK_CMD:
					if (me.hidden)
						me.hidden := 0;
						say("*" + me.name + " springs from hiding!*");
					endif
					EnableMainEvents();
					return ev;
				SYSEVENT_ENTEREDAREA:
					if ( (!ev.source.dead) )
						if ( (GetHP(me) > (GetMaxHP(me)/2)) or (!me.hidden) )
							if (((!ev.source.npctemplate) or (ev.source.script == "tamed")) && !ev.source.concealed)
								if (me.hidden)
									me.hidden := 0;
									say("*" + me.name + " springs from hiding!*");
								endif
								EnableMainEvents();
								return ev;
							endif
						endif
					endif
			endcase

		endif

	endwhile

endfunction

function cleareventque()

	var ev;
	repeat
	until (! (ev := os::wait_for_event(0)) );

endfunction
