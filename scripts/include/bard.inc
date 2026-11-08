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

// Song scripts get the singer from a songbook, or {"#MOB", caster, target} when an npc casts (npccast.inc)
function IsNpcSong(parms)
	if (parms[1] == "#MOB")
		return 1;
	endif
	return 0;
endfunction

function GetSongSinger(parms)
	if (parms[1] == "#MOB")
		return parms[2];
	endif
	return parms;
endfunction

// Npcs sing without carrying an instrument; a harp only picks the sounds
function GetSongInstrument(singer, npc_cast)
	var instrument := findinstrument(singer);
	if (!instrument && npc_cast)
		instrument := struct;
		instrument.+graphic := UOBJ_HARP;
	endif
	return instrument;
endfunction

// An npc's bard level from its Musicianship, on the class level scale in classes.inc
// (AVERAGE_SKILL 75 = level 1, +AVERAGE_LEVEL_MOD 15 per level): 90 = 2, 105 = 3, 120 = 4, 135 = 5, 150 = 6
function GetNpcBardLevel(musicianship)
	if (musicianship < 75)
		return 0;
	endif
	var level := 1 + CInt((musicianship - 75) / 15);
	if (level > 6)
		level := 6;
	endif
	return level;
endfunction

function ValidSongBoost(player, cast_near)
	// Every song runs SmartBoost() first, which already narrowed an npc singer's list to its own
	// side (NpcSmartBoost in spelldata.inc), so there is nothing left to check here
	if (player.isa(POLCLASS_NPC))
		return 1;
	endif
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
