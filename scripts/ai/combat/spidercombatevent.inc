
include "include/dotempmods";
include "include/random";
include "include/attributes";

function process_combat_event(ev)

	send_web(ev.source);
	DoPoison(ev.source);

        if (ev.type == SYSEVENT_DAMAGED)
		if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
			EraseObjProperty(me,"#flees");
			flee(ev.source);
		endif
        endif

endfunction

function in_combat_event_loop(opponent)

	send_web(opponent);
	DoPoison(opponent);

endfunction       



function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	num_casts := 2;

	sleep(1);
	look_around();
   
endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function send_web(opponent)

	if (Random(20) != 1)
		return;
	endif

	if (!CheckLineOfSight( me, opponent ))
		return;
	endif

	printtextabove(me, "The spider spits a web!");

	var x := opponent.x;
	var y := opponent.y;
	var z := opponent.z;
	var r := opponent.realm;

	sleep(1);

	createitematlocation(x, y, z, UOBJ_SPIDERWEB,  1, r);
	
	var prot	:= GetObjProperty( opponent , "FreeAction" );
	if(prot == 1)
		return;
	endif
	if ( opponent.x == x and opponent.y == y)	
		if( CanMod( opponent , "p" ) )	
			SendSysMessage( opponent , "You are trapped in a spider web!" );
			DoTempMod( opponent , "p" , 1 , 15 );
		endif
	endif

endfunction
 
function DoPoison(you)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 10);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 10);

	if ( (distance(me,you)<2) and (Random(4)==1) )

		var plvl := Random(GetMaxHP(me)/40)+1;
		SetPoison(you,plvl,"You have been poisoned by " + me.name);

	endif

endfunction
