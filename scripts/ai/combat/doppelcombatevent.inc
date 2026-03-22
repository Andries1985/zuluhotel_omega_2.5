include "include/attributes";


function process_combat_event(ev)

	spellattack(ev.source);
        case (ev.type)
                SYSEVENT_DAMAGED: 
			check_for_change();
                       	if (will_cast)
                       		if ( (GetHP(me) < (GetStrength(me)/2)) and !(GetObjProperty( me , "Type" ) == "Undead" ) )
                                	CastSpell(me,me,"gheal");
					if( me.poisoned )
						CastSpell(me,me,"cure");
                       			endif
                       		endif
                       	endif
                        if ((GetHP(me) * 100/GetMaxHP(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
        endcase

endfunction

function spellattack(opponent)


	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(2)==1)
		sleep(1);
		cast_offensive_spell(me,opponent);
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	elseif ( (will_breathe) and (Random(3) == 1) )
		breathe_meteor(me);
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 10);
	else
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
        endif

	check_for_change();

endfunction

function in_combat_event_loop(opponent)

	spellattack(opponent);

endfunction       

function post_combat()
  
	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	summons := 1;

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(opponent)

	if (GetObjProperty(me, "mr") != "1")
		setobjproperty(me, "mr", "1");
	endif

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function breathe_meteor( caster )
    var parms := array;
    

    var targets := ListMobilesInLineofSight(me, 4);
    PerformAction(me,0x0c);

    foreach mobile in targets
	   var dmg := Random((cint(70* GetHP(me))/GetMaxHP(me)))+5;
	   PlayMovingEffect(caster, mobile ,MOVING_EFFECT_FIREBALL, 10,1,1);
	   applydamage(mobile, dmg);          
	
    endforeach


endfunction


function check_for_change()

	if (GetHP(me)< (GetStrength(me) /5))
		if (num_changes >= len(graphics))
			return 0;
		else
			num_changes := num_changes + 1;
			SetHP(me, GetStrength(me)); 
			var parm := splitwords(graphics[num_changes]);
			var form := cint(parm[1]);
			//print ("graphic is " + form);
			var clr := cint(parm[2]);
			will_cast := cint(parm[4]);
			will_breathe := cint(parm[3]);
			me.graphic := form;
			me.color := clr;
			printtextabove(me, "The creature changes shape!");
		endif
	endif

endfunction
