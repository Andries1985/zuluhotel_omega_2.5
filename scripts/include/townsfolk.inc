/////////////////////////////////////////////////////////////////////////////
// SayRandomFromArray - says a random string from the given array
/////////////////////////////////////////////////////////////////////////////
function SayRandomFromArray(arr)
        var idx := Random(10);
        say(arr[idx]);
endfunction
// Shared sayings for disengaged event (dynamic array assignment)
var DISENGAGED_SAYINGS;
DISENGAGED_SAYINGS[0] := "Is it over?";
DISENGAGED_SAYINGS[1] := "Thank the heavens!";
DISENGAGED_SAYINGS[2] := "I survived... somehow.";
DISENGAGED_SAYINGS[3] := "That was too close!";
DISENGAGED_SAYINGS[4] := "I hope they're gone for good.";
DISENGAGED_SAYINGS[5] := "Back to safety at last.";
DISENGAGED_SAYINGS[6] := "I need a drink after that!";
DISENGAGED_SAYINGS[7] := "I thought I was done for.";
DISENGAGED_SAYINGS[8] := "Maybe I should lay low for a while.";
DISENGAGED_SAYINGS[9] := "Thank you, whoever helped me!";
// Shared sayings for random responses (dynamic array assignment)
var ENGAGED_SAYINGS;
ENGAGED_SAYINGS[0] := "Help! Help! I'm being attacked!";
ENGAGED_SAYINGS[1] := "Somebody stop this madness!";
ENGAGED_SAYINGS[2] := "Guards! Guards!";
ENGAGED_SAYINGS[3] := "Why are you doing this?";
ENGAGED_SAYINGS[4] := "Please, I mean no harm!";
ENGAGED_SAYINGS[5] := "No! Stay away from me!";
ENGAGED_SAYINGS[6] := "Mercy! Mercy!";
ENGAGED_SAYINGS[7] := "What did I do to deserve this?";
ENGAGED_SAYINGS[8] := "Leave me alone!";
ENGAGED_SAYINGS[9] := "I just want to live!";

var DAMAGED_SAYINGS;
DAMAGED_SAYINGS[0] := "Ouch! Heeelp!";
DAMAGED_SAYINGS[1] := "That hurts! Stop it!";
DAMAGED_SAYINGS[2] := "Aaaagh! Why?!";
DAMAGED_SAYINGS[3] := "By the virtues, that stings!";
DAMAGED_SAYINGS[4] := "No more, please!";
DAMAGED_SAYINGS[5] := "I'm bleeding!";
DAMAGED_SAYINGS[6] := "You fiend!";
DAMAGED_SAYINGS[7] := "Is this the end for me?";
DAMAGED_SAYINGS[8] := "I need a healer!";
DAMAGED_SAYINGS[9] := "Someone, save me!";
/////////////////////////////////////////////////////////////////////////////
//
// townsfolk.inc - Shared logic for city-bound civilian NPCs.
//
// Provides:
//   FleeOpenDistance(opponent) - run away and open doors
//   RunFromOpponent(opponent, halt_threshold) - full flee loop with town-bound enforcement
//   singsongs / PlayMidi / PlaySFX - music functions for minstrel/performer
//
// Requires (in the including script):
//   use npc; use os; use uo; use basic; use math;
//   include "include/eventid";
//   include "include/client";   (for SFX_MUSIC_* constants, music scripts only)
//   var me := Self();
//
// This include pulls in anchors.inc transitively, so scripts that include
// townsfolk.inc do NOT need a separate include "include/anchors".
//
/////////////////////////////////////////////////////////////////////////////

include "include/anchors";
include "include/eventid";

/////////////////////////////////////////////////////////////////////////////
//
//  FleeOpenDistance - open the distance between self and an opponent,
//  also tries nearby NPC doors.
//  Returns: 1 if distance is already 10, 0 if movement was attempted.
//
/////////////////////////////////////////////////////////////////////////////
function FleeOpenDistance( opponent, halt_threshold )
        var me := Self();
        if (Distance( me, opponent ) >= halt_threshold)
                return 1;
        endif

        TryOpenNearbyNpcDoors();
        StepCivilianThroughDoorIfNeeded();
        RunAwayFrom( opponent );
        return 0;
endfunction

/////////////////////////////////////////////////////////////////////////////
//
//  RunFromOpponent - shared flee loop used by civilian NPCs.
//  Enforces town bounds during the flee; jails/kills if chased out of town.
//  Responds to "hold" keyword, DISENGAGED, and damage events.
//
/////////////////////////////////////////////////////////////////////////////
function RunFromOpponent( opponent, HALT_THRESHOLD )
        TurnAwayFrom( opponent );
        DisableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_LEFTAREA );

        var waittime;
  outer:
	while (opponent and !opponent.dead)
		if (!EnforceTownBounds(1))
			break outer;
		endif

		waittime := FleeOpenDistance( opponent, HALT_THRESHOLD ) * 15;
		if (!waittime)
			waittime := 1;
		endif

		var ev := wait_for_event( waittime );
		case (ev.type)
			SYSEVENT_SPEECH:
				if (ev.text["hold"])
					say( "Very well then." );
					break outer;
				endif

			SYSEVENT_DISENGAGED:
				if (ev.source.serial == opponent.serial)
					SayRandomFromArray(DISENGAGED_SAYINGS);
					break outer;
				endif

			SYSEVENT_ENGAGED:
			SYSEVENT_DAMAGED:
				if (ev.source)
					if (ev.source.serial != opponent.serial)
						SayRandomFromArray(ENGAGED_SAYINGS);
					else
						if (ev.type == "damage")
							SayRandomFromArray(DAMAGED_SAYINGS);
						endif
					endif
				endif

			SYSEVENT_OPPONENT_MOVED:

			SYSEVENT_ENTEREDAREA:
			SYSEVENT_LEFTAREA:
				// ignore
		endcase
	endwhile

        EnableEvents( SYSEVENT_ENTEREDAREA + SYSEVENT_LEFTAREA, HALT_THRESHOLD );
        SetWarMode( 0 );
endfunction

/////////////////////////////////////////////////////////////////////////////
// Music functions shared by minstrel and performer.
/////////////////////////////////////////////////////////////////////////////

function singsongs(singer, character)
        var counting := 1;
        var songcfg  := ReadConfigFile("songs");
        var maxsongs := FindConfigElem( songcfg, "songs");
        var numsongs := GetConfigInt( maxsongs, "number");
        var songnumb := Random(numsongs) + 1;
        var songelem := FindConfigElem( songcfg, songnumb);
        var midinum  := GetConfigString( songelem, "midi");
        var number   := GetConfigInt( songelem, "count");

        if ( midinum )
                PlayMidi( singer, character, midinum );
        else
                PlaySFX( character );
        endif

        while ( counting <= number )
                var linename := "line" + cint( counting );
                var song := GetConfigString( songelem, linename );
                say(song);
                sleep(4);
                counting := counting + 1;
        endwhile
endfunction

function PlayMidi(singer, character, song_id)
        var packet_id;
        var xdist := Abs(singer.x - character.x);
        var ydist := Abs(singer.y - character.y);

        foreach character in EnumerateOnlineCharacters()
                if ( xdist <= 15 and ydist <= 15 )
                        packet_id := "6D00" + song_id;
                        SendPacket( character, packet_id );
                endif
        endforeach
endfunction

function PlaySFX( character )
        var randy := Random(4);

        case( randy )
                0: playsoundeffect( character, SFX_MUSIC_LUTE_GOOD );
                1: playsoundeffect( character, SFX_MUSIC_HARP_LAP_GOOD );
                2: playsoundeffect( character, SFX_MUSIC_DRUM_GOOD );
                3: playsoundeffect( character, SFX_MUSIC_TAMBORINE_GOOD );
        endcase
endfunction
