use vitals;
use cfgfile;

include "include/client";
include "include/attributes";
include "include/areas";
include "verseconstants";
include "include/spelldata";

var cfgfile := ReadConfigFile(":versebook:verses");

//////////////////////////////////////////////////////////////////////////////////////
//
// ReturnVerseInfo()
//
/////////////////////////////////////////////////////////////////////////////////////

function ReturnVerseInfo(verse)

	var ele := FindConfigElem( cfgfile, verse);
	var Stamina := GetConfigInt(ele,"Stamina");
	var diff := GetConfigInt(ele,"Difficulty");

	return {Stamina, diff};

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// PerformSong() - Have 'who' cast a song, being a number (1 - 20)
//
/////////////////////////////////////////////////////////////////////////////////////

function PerformSong(who, song)

	var ele := FindConfigElem( cfgfile, song);
	var songname := GetConfigString(ele,"Script");

	Start_Script(":versebook:"+songname, {who, song});

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// CanPlay() - PreReqs for playing the song
//
// Returns: An array with difficult/Staminareq/skills used/ele 
//
/////////////////////////////////////////////////////////////////////////////////////

function CanPlay(who, song)

	var allowed_songs := {
		//Verse Book
		2, //Lesser Healing
		3, //Shadows
		6, //Dragon Skin
		7, //Life Balance
		9  //Beastal Bond
	};

	var allowed := song in allowed_songs;
	if(IsInAntiMagicArea( who ) && !who.cmdlevel && !allowed )
		SendSysMessage( who, "This song cannot be performed here." );
		return 0;
	endif

	if (!IsBard(who))
		SendSysMessage(who, "Only classed bards may perform the archaic verses.");
		return 0;
	endif

	// Find instrument in pack
	var instrument := FindInstrument(who);
	if (!instrument)
		SendSysMessage(who, "You need a musical instrument to perform archaic verses.");
		return 0;
	endif

	// the song ele
	var ele := FindConfigElem( cfgfile, song);

	// Check we have enough Stamina
	var Stamina := GetConfigInt(ele,"Stamina");

	if (GetStamina(who) < Stamina)
		SendSysMessage(who, "You don't have enough Stamina to perform that verse.");
		return 0;
	endif

	// The difficulty to play the song, based against song skill AND music
	var difficulty := GetConfigInt(ele,"Difficulty");

	// array of skills the song is based on
	var uses_skills := GetConfigStringArray( ele, "UseSkill");

	// musicianship is used in each song.
	var musicianship := GetEffectiveSkill(who, SKILLID_MUSICIANSHIP);
	if (musicianship < difficulty)
		SendSysMessage(who, "You must improve your musicianship before you can play such a complicated verse.");
		return 0;
	endif
		
	// Players skill must be within this many points to attempt the verse
	var skill_mod := 15;
	foreach skill in uses_skills
		// He is within 'skill_mod' points of difficulty, let him try
		if ( (GetEffectiveSkill(who, CInt(skill)) + skill_mod)  < difficulty  )
			SendSysMessage(who, "Your bardic skills arn't advanced enough to play that verse.");
			return 0;
		endif
	endforeach

	// This isn't a check at all, it's saving processing power by not having the script have to access verses.cfg again
	// in main script
	var element := GetConfigStringArray(ele,"Element");

	var name := GetConfigString(ele, "Name");

	return {Stamina, difficulty, uses_skills, element, instrument, musicianship, name};

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// FindInstrument() - Finds and returns an instrunment in players pack
//
/////////////////////////////////////////////////////////////////////////////////////

function FindInstrument(who)

	foreach item in EnumerateItemsInContainer( who.backpack)

		case (item.graphic)
	/* DRUM */	3740:	
	/* TAMB */	3741:
	/* TAMB */	3742:
	/* HARP */	3761:
	/* HARP */	3762:
	/* LUTE */	3763:
	/* LUTE */	3764:	return item;
		endcase

	endforeach

	return 0;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// PlayMusic() - Plays success or fal sound
//
/////////////////////////////////////////////////////////////////////////////////////

function PlayMusicEffect(who, instrument, type)

	var song;

		case (instrument.graphic)

	/* DRUM */	3740:	if (type == PLAY_SUCCESS)
							song := SFX_MUSIC_DRUM_GOOD;
						else
							song := SFX_MUSIC_DRUM_FAIL;
						endif
	/* TAMB */	3741:
	/* TAMB */	3742:	if (type == PLAY_SUCCESS)
							song := SFX_MUSIC_TAMBORINE_GOOD;
						else
							song := SFX_MUSIC_TAMBORINE_FAIL;
						endif

	/* LARGE HARP */3761:	if (type == PLAY_SUCCESS)
								song := SFX_MUSIC_HARP_LARGE_GOOD;
							else
								song := SFX_MUSIC_HARP_LARGE_FAIL;
							endif
	/* HARP */	3762:	if (type == PLAY_SUCCESS)
							song := SFX_MUSIC_HARP_LAP_GOOD;
						else
							song := SFX_MUSIC_HARP_LAP_FAIL;
						endif
	/* LUTE */	3763:
	/* LUTE */	3764:	if (type == PLAY_SUCCESS)
							song := SFX_MUSIC_LUTE_GOOD;
						else
							song := SFX_MUSIC_LUTE_FAIL;
						endif
		endcase

	PlaySoundEffect( who, song );

	return 1;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// PlayBardicMusic() - Just contains a check against music skill and plays
// 			success or failure tune
//
/////////////////////////////////////////////////////////////////////////////////////

function PlayBardicMusic(who, diff, instrument)

	if (!CheckSkill(who, SKILLID_MUSICIANSHIP, diff, 100))
		PrintTextAbove(who, "*"+who.name+" fumbles with the instrument*");
		SendSysMessage(who, "You have difficulty playing the complicated chords of the verse.");
		PlayMusicEffect(who, instrument, PLAY_FAIL);
		return 0;
	endif

	return PlayMusicEffect(who, instrument, PLAY_SUCCESS);

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// InterruptedPlaying() - Stop playing if these things occur
//
/////////////////////////////////////////////////////////////////////////////////////

function InterruptedPlaying(who, x, y, Stamina, hp)

	// bard moved - if x and y are 0 wel allow moving....
	if (x && y)
		if ((who.x != x) || (who.y != y))
			SendSysMessage(who, "Your movement breaks your concentration and you stop playing.");
			EraseObjProperty(who, "#Casting");
			return 1;
		endif
	endif

	if (GetStamina(who) < Stamina)
		SendSysMessage(who, "You exhaust your Stamina and stop playing.");
		EraseObjProperty(who, "#Casting");
		return 1;
	endif
	// bard hp lost
	if (GetHP(who) < hp)
		SendSysMessage(who, "The inflicted pain breaks your concentration and you stop playing.");
		EraseObjProperty(who, "#Casting");
		return 1;
	endif
	// dead bard
	if (who.dead)
		SendSysMessage(who, "Death silences your music.");
		EraseObjProperty(who, "#Casting");
		return 1;
	endif

	return 0;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// SucceedPlayingVerse() - Final check to see whether we succeed or fail
//
/////////////////////////////////////////////////////////////////////////////////////

function SucceedPlayingVerse(who, skills, diff, instrument, name)

	foreach skill in skills
		if (!CheckSkill(who, CInt(skill), diff, 0))
			PrintTextAbove(who, "*"+who.name+" fails the arachic verse of "+name+"*");
			PlayMusicEffect(who, instrument, PLAY_FAIL);

			// check for trying to add verse to book
			if (GetObjProperty(who, "#TryAddVerse"))
				SendSysMessage(who, "Your verse book shudders violently, as if rejecting the failed verse.", 3, 39);
				PlaySoundEffect(who, 0x458);
				EraseObjProperty(who, "#TryAddVerse");
			endif
	
			return 0;
		endif
	endforeach

	// add the verse to book
	if (GetObjProperty(who, "#TryAddVerse"))
		AddVerseToBook(who);
	endif

	PrintTextAbove(who, "*"+who.name+" plays the archaic verse of "+name+"*", 3, 50);
	PlayMusicEffect(who, instrument, PLAY_SUCCESS);
	SetObjProperty(who, "#Casting", 1);
	return 1;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// AddVerseToBook() - add the verse into the book and do stuff
//
//////////////////////////////////////////////////////////////////////////////////////

function AddVerseToBook(who)

	var versearray := GetObjProperty(who, "#TryAddVerse");

	var book := SystemFindObjectBySerial(versearray[1]);
	var verse_id := versearray[2];
	var verses := GetObjProperty(book, "Verses");

	if (!verses)
		verses := {};
	endif

	SendSysMessage(who, "Your verse book gently vibrates, absorbing the archaic verse within itself!", 3, 54);

	// verses array isn't as large as current verse, increase size.
	var i := 1;
	versearray := {};

	if (verse_id > verses.size() )

		repeat	
			if (verse_id == i)
				versearray.append(1);
			else
				versearray.append(CInt(verses[i]));
			endif
			i := i + 1;
		until ( i > verse_id);
		SetObjProperty(book, "Verses", versearray);

	else // its smaller or same size, add it
		verses[verse_id] := 1;
		SetObjProperty(book, "Verses", verses);
	endif

	EraseObjProperty(who, "#TryAddVerse");
	PlayObjectCenteredEffectEx( who, 14170, 8, 16 ,1288 );
	PlaySoundEffect(who, 0x46f);

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// GetBardicEfficiency() - returns something like 1.1 (10%) or .9 (-%10)
//
//////////////////////////////////////////////////////////////////////////////////////

function GetBardicEfficiency(who)

	var bonus := 0.0;

	foreach item in ListEquippedItems(who)
		if (GetObjProperty(item, "BardicEfficiency"));
			bonus += GetObjProperty(item, "BardicEfficiency");
		endif
	endforeach

	bonus := 1 + (CDbl(bonus) * 0.05);

	return bonus;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// GetVerseRange() - You know
//
//////////////////////////////////////////////////////////////////////////////////////

function GetVerseRange(who, musicianship)

	var range := CInt(musicianship/30);
	var bard :=  IsBard(who);

	if (bard)
		range += ClasseBonusByLevel( bard ) * range - 4;
	endif


	range := range * GetBardicEfficiency(who);

	return CInt(range);

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// CalculateBardicDamage() - Modifies the base damage with bard skill/class
//
//////////////////////////////////////////////////////////////////////////////////////

function CalculateBardicDamage(who, verse_skills, diff, damage)

	var modified_skill;
	var total_skills := 0;

	foreach skill in verse_skills
		total_skills += GetEffectiveSkill(who, CInt(skill));	
	endforeach
	Print("Base Damage " + damage);
	modified_skill := (total_skills/verse_skills.size()) - diff;
	damage *=  1.0 + modified_skill/100.00;

	Print("Modified by skill damage " + damage);

	// Give damage bonus for bard
	var bard := IsBard(who);
	if(DEBUG_MODE)
		SendSysMessage(who,"Pre class Damage: "+damage);
	endif
	if (bard)
		damage *= 1.0 + (CDbl(bard * 1.5)/10.0);
	endif
	if(DEBUG_MODE)
		SendSysMessage(who,"Post Damage: "+damage);
	endif

	damage *= GetBardicEfficiency(who);

	Print("Modified by BE damage " + damage);


	return CInt(damage);

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// CalculateBardicHeal() - Much like the damage calculation
//
//////////////////////////////////////////////////////////////////////////////////////

function CalculateBardicHeal(who, verse_skills, diff,amount)

	var modified_skill;
	var total_skills := 0;
	foreach skill in verse_skills
		total_skills += GetEffectiveSkill(who, CInt(skill));	
	endforeach

	modified_skill := (total_skills/verse_skills.size()) - diff;
	amount *= 1.0 + modified_skill/100.00;

	// Give amount bonus for bard
	var bard := IsBard(who);
	if(DEBUG_MODE)
		SendSysMessage(who,"Pre amount: "+amount);
	endif
	if (bard)
		amount *= 1.0 + (CDbl(bard)/10.0);
	endif

	// healing items heal for more!
	var healmod := CDbl(GetObjProperty(who, "HealingIncrease"));
	if(DEBUG_MODE)
		SendSysMessage(who,"Pre amount: "+amount);
	endif
	if (healmod)
		//5% bonus per level
		amount *= 1.0 + (healmod * 0.05);
	endif
	if(DEBUG_MODE)
		SendSysMessage(who,"Post amount: "+amount);
	endif

	amount *= GetBardicEfficiency(who);

	return CInt(amount);

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// ApplyBardicHeal() - Applies the heal
//
//////////////////////////////////////////////////////////////////////////////////////

function ApplyBardicHeal(who, person, amount)

	// healing items heal for more!
	var healmod := CDbl(GetObjProperty(person, "HealingIncrease"));
	if(DEBUG_MODE)
		SendSysMessage(who,"initial amount to heal: "+amount);
	endif
	if (healmod)
		//3% bonus per level on person
		amount *= 1.0 + (healmod * 0.03);
	endif
	if(DEBUG_MODE)
		SendSysMessage(who, "Heal with prot bonus: "+amount);
	endif
	HealDamage( person, CInt(amount) );

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// ApplyBardicDamage() - Applies the damage to the person.
//
//////////////////////////////////////////////////////////////////////////////////////

function ApplyBardicDamage(who, person, verse_elements, damage)

	// convert to dbl for accurate damages
	damage := CDbl(damage);

	// Increase damage against npcs, like the hitscript.inc
	if (person.isa(POLCLASS_NPC))
		damage *= 1.7;
	endif

	// Take off 10% of damage to use as non ele damage
	var non_ele_dmg := damage * 0.1;

	// remove the above 10% for ele calculations
	damage *= 0.9;

	var dmg_div := CDbl(damage / CDbl(verse_elements.size()) );

	// reset damage to 0 as we have done previous stuff
	damage := 0.0;

	foreach element in verse_elements

		// physical protection, check for class bonus and phys prot items
		if (element == 0x0100)
			damage := AdjustForPhysicalDamage(person, damage);
		else
			var resultprot := GetResultingElementalProt( person, CInt(element) );
			if( resultprot < 100 )
				damage += CDbl(dmg_div - CDbl(dmg_div * resultprot / 100));	
			else
				var amt := CDbl(resultprot)/100.0;
				damage -= CDbl(dmg_div * CDbl(amt-1));
			endif
		endif
	endforeach


	// Add the 10% of no resist/protection
	damage += non_ele_dmg;

	// classed bards get protection
	var bard_protection := IsBard(person);

	//Print("before bard prot: "+damage);
	if (bard_protection)
		damage *= 1.0 - ( (CDbl(bard_protection) * 5.00) / 100.0);
	endif
	//Print("after bard prot: "+damage);

	// .xx damage, convert to 1
	if (damage > 0 && damage < 1)
		damage := 1;
	endif


	// Done last, this function doesn't check for ele prots. 0x0800 is no resist (ie magic resistance skill)
	ApplyTheDamage( person, who, damage, 0x0800 );

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// AdjustForPhysicalDamage() - Magic doesn't check for warrior/mage bonus, physical protection
// bonus etc, when it comes to physical magic damage (using physical element).
// 
//////////////////////////////////////////////////////////////////////////////////////

function AdjustForPhysicalDamage(person, damage)

	if (IsWarrior(person))
		damage /= ClasseBonusByLevel( IsWarrior(person) );
	elseif (IsMage(person))
		damage *= ClasseSmallBonusByLevel( IsMage(person) );
	endif

	var phys_prot := GetObjProperty(person, "PhysicalProtection");

	if (phys_prot)
		damage *= 1 - phys_prot * 0.05;
	endif

	return damage;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// ConsumeBardStamina() - Consumes Stamina
//
// option 1: full Stamina, from an effect
// option 0: partial Stamina, when nothing happens
//////////////////////////////////////////////////////////////////////////////////////

function ConsumeBardStamina(who, Stamina, option)

	// Half Stamina req for no effect
	if (option == 0)
		Stamina := Stamina/2;

	// custom Stamina 
	elseif (option != 1)
		Stamina := option;

	// normal spell Stamina
	else
		Stamina := CInt(Stamina / GetBardicEfficiency(who));
	endif

	if (GetStamina(who) < Stamina)
		SetStamina(who, 0);
		return 0;
	else
		SetStamina(who, GetStamina(who)-Stamina);
		return 1;
	endif

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// SmartSongAoE() - Removes person according to faction, team and noPK
//////////////////////////////////////////////////////////////////////////////////////

function SmartSongAoE(who, person)

	if ((person.isa(POLCLASS_NPC)) && (person.master == who))
		return 0;
	elseif ((!person.isa(POLCLASS_NPC)) && who.party && (person.party == who.party))
		return 0;
	endif
	
	if (!person.isa(POLCLASS_NPC) && !who.isa(POLCLASS_NPC) && !who.cmdlevel)
		
		var who_noPK := GetObjProperty(who, "NOPKAREA");
		var person_noPK := GetObjProperty(person, "NOPKAREA");
		
		var who_CW := GetObjProperty(who, "cwarring");
		var same_CWcolor := 0;

		var same_faction := GetObjProperty(who, "IsRPer") == GetObjProperty(person, "IsRPer" );
		if (who_CW)
			if (GetGlobalProperty("cwtype") != "k")
				same_CWcolor := GetObjProperty(person,"cwcolor") == GetObjProperty(who,"cwcolor");
			endif
		endif
		if((who_noPK) || (person_noPK) || (!same_faction) || (same_CWcolor))
			return 0;
		endif
	endif
	
	return 1;

endfunction

//////////////////////////////////////////////////////////////////////////////////////
//
// SmartSongBoost() - Removes person according to faction and team
//////////////////////////////////////////////////////////////////////////////////////

function SmartSongBoost(who, person)

	if ((person.isa(POLCLASS_NPC)) && (person.master == who))
		return 1;
	elseif ((!person.isa(POLCLASS_NPC)) && (!who.isa(POLCLASS_NPC)) && (person.party == who.party))
		var same_CWcolor := 1;
		var same_faction := GetObjProperty(who, "IsRPer") == GetObjProperty(person, "IsRPer" );	
		if (GetObjProperty(who, "cwarring"))
			same_CWcolor := 0;
			if (GetGlobalProperty("cwtype") != "k")
				same_CWcolor := GetObjProperty(person,"cwcolor") == GetObjProperty(who,"cwcolor");
			endif
		endif
		if(same_faction && same_CWcolor)
			return 1;
		endif
	elseif (who.cmdlevel) 
		return 1;
	endif
	return 0;

endfunction