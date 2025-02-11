include "include/npcboosts";

function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_offer := readgameclock();
    var next_wander := 0;
    var evtext;
    var resetquest := 0;

    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

    while (1)

	if(!GetObjProperty(me, "lbc") || GetObjProperty(me, "lbc") < ReadGameClock())
		SetObjProperty(me, "lbc", ReadGameClock()+5);
		checkmymod(me);
	endif
    
		if (me.hidden) me.hidden := 0; endif

	if (readgameclock() > next_wander)

		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x = mydestx and me.y = mydesty) or ( steps > 10 ) )
				mydestx := 0;
			endif
		else	
			wander();
			wanders := wanders +1;
		endif

		next_wander := readgameclock()+2;
		if (wanders > 60)
			wanders :=0;
			if (!look_around())
			if (!ResetQuest)
				if (master.serial = me.serial)
					if (!slave)
						RevokePrivilege( me, "invul" );
						//MoveCharacterToLocation(me,5288,1176,0,MOVECHAR_FORCELOCATION);
						SetObjProperty(me,"guardkill",1);
						damage( me, me.maxhp + 3 );
					endif
				else
					if (!master)
						MoveObjectToLocation(me,5288 , 1176 , 0, "britannia", MOVEOBJECT_FORCELOCATION);
						SetObjProperty(me,"guardkill",1);
						damage( me, me.maxhp + 3 );
					endif
				endif
			else
				MoveObjectToLocation(me,5288 , 1176 , 0, "britannia", MOVEOBJECT_FORCELOCATION);
				SetObjProperty(me,"guardkill",1);
				damage( me, me.maxhp + 3 );
			endif
			endif
			ev := sleepmode();
	    	endif
	else
		ev := os::wait_for_event( 3 );
	endif

	if (ev)
        repeat
            case (ev.type)
                SYSEVENT_SPEECH:
	    		next_wander := readgameclock()+30;
			evtext := lower(ev.text);
			if (evtext["take"])
				GiveQuestItem(ev.source);
			elseif ( next_offer < ReadGameClock() )
				next_offer := ReadGameClock()+45;
				TurnToward(ev.source);
				if (!OfferQuest())
		    			check_speech(ev.text, ev.source);
				endif
			else
		    		check_speech(ev.text, ev.source);
			endif
		SYSEVENT_ENTEREDAREA:
		    if (!ev.source.npctemplate)
		    	YellToFriend(ev.source);
		    endif
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
                    Fight( ev.source );
		SYSEVENT_ITEM_GIVEN:
		    wanders :=0;
		    next_wander := readgameclock()+30;
		    if ( GetObjproperty(ev.item,"slave") = cint(me.serial) )
			TakeQuestItem(ev.source,ev.item);
			ResetQuest := 1;
		    else
			TakeItem(ev.source,ev.item);
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

function look_around()

	foreach critter in ListMobilesNearLocation( me.x,me.y,me.z, 10, me.realm );
          if (!critter.npctemplate)
		return 1;
          endif
	endforeach

	return 0;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_SPEECH, 2 );
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, HALT_THRESHOLD );
    EnableEvents( SYSEVENT_ITEM_GIVEN );

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA + SYSEVENT_ITEM_GIVEN );

endfunction

function OfferQuest()

	if ( questtype < 2 )
		return OfferMailBoyQuest();
	endif

	return 0;

endfunction

function OfferMailBoyQuest()

	var item := 0;
	var slavetown;
	if (me.serial = master.serial)
		item := HasQuestItem(me, slave);
		if (item)
			slavetown := getobjproperty(me,"slavetown");
			say("I have "+ item.desc + " that I need delivered to "+slave.name+" in " + slavetown);
			sleep(1);
			say("If you will [take] it, "+ slave.name + " will pay you.");
			return 1;
		endif
	else
		if (!master)
			return 0;
		else
			item := HasQuestItem(master, me);
			slavetown := WhatTown(master);
			if ( (item) and (slavetown) )
				say(master.name + " in " + slavetown + " has " + item.desc + " for me.");
				sleep(1);
				say("If you will bring it to me, I will pay you.");
				return 1;
			endif
		endif
	endif
	return 0;

endfunction

function GiveQuestItem(you)

	var item := HasQuestItem(me, slave);
	if (item)
		var slavetown := getobjproperty(me,"slavetown");
		say("Here is "+item.desc+" for "+slave.name+" in "+slavetown);
		say("Take it to "+ slave.name + " for your payment.");
		MoveItemToContainer(item,you.backpack);
	endif

endfunction

function HasQuestItem(who, forwhom)

	foreach item in enumerateitemsincontainer(who.backpack)
		if ( getobjproperty(item,"slave") = forwhom.serial )
			return item;
		endif
	endforeach
	return 0;

endfunction

function TakeQuestItem(you, it)

	TurnToward(you);
	say("Oh this must be from "+master.name+"!  Thank you for bringing it!");
	say("And here is your pay!");
	DestroyItem(it);
	var goldamount := ((Random(3)+2)*100)+Random(100);
	CreateItemInContainer(you.backpack,"goldcoin",goldamount);

endfunction
