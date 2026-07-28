use uo;
use os;
use basic;
use cfgfile;

include "include/client";
include "include/skillpoints";


function play(character, succeed_sound, fail_sound, theskill)

	var thepoints := get_default_points(SKILLID_MUSICIANSHIP);

	if (CheckSkill(character, SKILLID_MUSICIANSHIP, theskill, thepoints))
		PlaySoundEffect( character, succeed_sound );
		return 1;
	else
		PlaySoundEffect( character, fail_sound);
		return 0;
	endif
endfunction

function music(character, instrument, diff)

	var inst_id := instrument.graphic;
	var ret := 0;
 
	if (inst_id == UOBJ_HARP)
		ret := play(character,SFX_HARP_SUCC,SFX_HARP_FAIL, diff);
	elseif (inst_id == UOBJ_LUTE)
		ret := play(character,SFX_LUTE_SUCC,SFX_LUTE_FAIL, diff);
	elseif (inst_id == UOBJ_DRUM)
		ret := play(character,SFX_DRUM_SUCC,SFX_DRUM_FAIL, diff);
	elseif (inst_id == UOBJ_TAMB)
		ret := play(character,SFX_TAMB_SUCC,SFX_TAMB_FAIL, diff);
	else
		PrintTextAbovePrivate( character, "That's not something you can play", character );
	endif

	return ret;
endfunction


function findinstrument(character)
	var instruments := {UOBJ_HARP, UOBJ_LUTE, UOBJ_DRUM, UOBJ_TAMB};

	foreach item in enumerateitemsincontainer(character.backpack)
		if (item.graphic in instruments)
				return item;
		endif
	endforeach
endfunction

function can_sing( me, circle)

	var cfg := readconfigfile(":combat:itemdesc");
	var item;

	foreach item in ListEquippedItems(me)
		if ((cfg[item.objtype].blockcircle) && (cfg[item.objtype].blockcircle <= circle)) 
			SendSysmessage(me, "Your equipment prevents you from using this instrument!");
			return 0;
		endif
	endforeach

	return 1;

endfunction

function ValidSongBoost(player, cast_near)
	if ((cast_near.isa(POLCLASS_NPC)) && (cast_near.master == player))
		return 1;
	elseif (!cast_near.isa(POLCLASS_NPC))
		return 1;
	endif
	return 0;
endfunction

function ValidSongTarget(player, cast_near)
	if ((cast_near.isa(POLCLASS_NPC)) && (cast_near.master == player))
		return 0;
	elseif ((!cast_near.isa(POLCLASS_NPC)) && player.party && (cast_near.party == player.party))
		return 0;
	endif
	return 1;
endfunction
