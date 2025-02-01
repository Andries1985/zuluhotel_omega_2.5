function process_combat_event(ev)

        if (ev.type == SYSEVENT_DAMAGED)
                        if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
        endif

endfunction

function in_combat_event_loop(opponent)

	OpenDoors(opponent);

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function OpenDoors(opponent)

	//only check once per 5 seconds:
	var nextbreath := GetObjProperty(me,"#lastbreath");
	if ( (nextbreath) and (nextbreath > ReadGameClock() ) )
		return;
	endif

	if ( distance(me,opponent) < 2 )
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 20 );
		return;
	endif

	var itemdesc := ReadConfigFile( ":doors:itemdesc" );
	var doorstoopen := 0;
	var tox, toy;
	var Doors := ListItemsNearLocation( me.x,me.y,me.z,3,me.realm );
	foreach door in Doors
			if (itemdesc[door.objtype].doortype)
				doorstoopen := 1;
				tox := door.x;
				toy := door.y;
				break;
			endif
	endforeach

	if ( doorstoopen )
		start_script("::/misc/npcdoor", me);
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 20 );
		sleepms(150);
		RunTowardLocation(tox, toy);
		sleepms(150);
		RunTowardLocation(tox, toy);
	else
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5 );
	endif

endfunction
