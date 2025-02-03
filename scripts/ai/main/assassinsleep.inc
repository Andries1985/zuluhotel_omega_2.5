function sleepmode()

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 10, me.realm );
          if (!critter.npctemplate)
		return 0;
          endif
	endforeach

	if (GetObjProperty(me,"killme"))
		ApplyRawDamage(me,GetMaxHP(me)+1);
	endif

	Eraseobjproperty(me,"#flees");

	DisableMainEvents();
	EnableEvents(SYSEVENT_LEFTAREA,4);
	DisableEvents(SYSEVENT_ENTEREDAREA);

	EnableEvents(SYSEVENT_ENGAGED);
	EnableEvents(SYSEVENT_DAMAGED);

	me.hidden := 1;

	var ev;

	while (1)
		ev := os::wait_for_event( 120 );

		repeat

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
			SYSEVENT_LEFTAREA:
				if ( (!ev.source.dead) and (GetHP(me) > 10) )
				    if ( (!ev.source.npctemplate) or (ev.source.script == "tamed") || GetObjProperty(ev.source, "NPCAttack") )
					if (me.hidden)
						me.hidden := 0;
						say("*" + me.name + " springs from hiding!*");
					endif
					EnableMainEvents();
					ev.type := SYSEVENT_ENTEREDAREA;
					return ev;
				    endif
				endif
		endcase

		until (! (ev := os::wait_for_event(120)) );

	endwhile      

endfunction       
