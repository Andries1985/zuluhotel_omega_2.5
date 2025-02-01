include "include/itemutil";
include "include/random";
include "include/attributes";

function process_combat_event(ev)

	//do_damage(ev.source);

        case (ev.type)
                SYSEVENT_DAMAGED: 
			if (Random(10) == 1)
			    if ( (!cint(getobjproperty(me, "guardkill"))) and
			       (!cint(GetObjProperty(me,"killme"))) )
				var it := createnpcfromtemplate(me.npctemplate, me.x , me.y -1, me.z, 0, me.realm);
        			if (!it.errortext)
                			printtextabove(me, me.name + " splits when struck!");
					SetObjProperty(it,"killme",1);
        			endif
        		    endif
			endif
                        if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
        endcase

endfunction

function in_combat_event_loop(opponent)

	//do_damage(opponent);
        
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

function do_damage(evsource)
/*
	if ( (distance(me, evsource) < 3 ) and (Random(40) == 1) )
	  PrintTextAbovePrivate(me , me.name + " spits acid at you!", evsource);
	  if (!HasShield(evsource))
		var dmg := Random(5) + 1;
		var things := ListEquippedItems( evsource );

		var thing := things[ cint(Random(len(things))+1) ];

		if ( CanEat(evsource, thing) )
		  PrintTextAbovePrivate(thing, "Acid damages "+ thing.desc, evsource);
		  if (dmg > thing.hp)
			dmg := thing.hp;
		  endif
		  thing.hp := thing.hp - dmg;
		  if ( (GetHP(thing) <= 0) or (GetHP(thing)>  GetMaxHP(thing)) )
			PrintTextAbovePrivate(evsource, thing.desc + " is destroyed!", evsource);
			DestroyTheItem( evsource , thing );
		  endif
		endif
	  endif
	endif
*/
endfunction

function CanEat(character, item)

	if ( ( item.objtype >= 0x2030 ) and (item.objtype <= 0x2060) )
		return 0;
	endif
	if ( (item.objtype == 0x0E75) or (item.objtype == 0x1F021 ) )
		return 0;
	endif

	if (!item.maxhp)
		return 0;
	endif

	return 1;

endfunction

function HasShield(you)

	var shield := GetEquipmentByLayer( you, LAYER_HAND2 );
	if ( (shield.objtype == 0x1bc3) or (shield.objtype == 0x1bc4) or (shield.objtype == 0x86ef) or (shield.objtype == 0x86df))
		PrintTextAbovePrivate(shield,"Your shield blocks the acid!",you);
		return 1;
	endif

	return 0;

endfunction
