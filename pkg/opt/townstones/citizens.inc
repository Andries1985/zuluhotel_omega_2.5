use uo;
use os;

// Town citizens - Patch 1.1.5. A townstone's "citizen" CProp lists one entry per character,
// {character serial, account name}, so any number of characters on an account can join a town,
// each one counts toward its population and each one votes. Before 1.1.5 it listed account names
// (one citizen per account); LoadCitizenList() converts such a list the first time it is read.

const CITIZEN_MAX_CHAR_SLOTS := 7;

function CitizenSerial(entry)
	if(TypeOf(entry) == "Array")
		return CInt(entry[1]);
	endif
	return 0;
endfunction

function CitizenAccount(entry)
	if(TypeOf(entry) == "Array")
		return CStr(entry[2]);
	endif
	return CStr(entry);
endfunction

function NewCitizenEntry(character)
	return array{CInt(character.serial), CStr(character.acctname)};
endfunction

function IsCitizenChar(citizen_list, serial)
	foreach entry in citizen_list
		if(CitizenSerial(entry) == CInt(serial))
			return 1;
		endif
	endforeach
	return 0;
endfunction

// Removes the character's entry; returns 1 when there was one.
function RemoveCitizenChar(byref citizen_list, serial)
	var i;
	for(i := 1; i <= citizen_list.size(); i := i + 1)
		if(CitizenSerial(citizen_list[i]) == CInt(serial))
			citizen_list.erase(i);
			return 1;
		endif
	endfor
	return 0;
endfunction

// A stone's citizen list, converted to per-character entries if it still holds account names.
// Each character on such an account who wears this town (CProp "town") gets an entry, an account
// with none left is dropped, and the population is recounted from the new list.
function LoadCitizenList(stone)
	var citizen_list := GetObjProperty(stone, "citizen");
	if(TypeOf(citizen_list) != "Array")
		return array{};
	endif

	var old_format := 0;
	foreach entry in citizen_list
		if(TypeOf(entry) != "Array")
			old_format := 1;
			break;
		endif
	endforeach
	if(!old_format)
		return citizen_list;
	endif

	var town_id := CInt(GetObjProperty(stone, "town"));
	var converted := array{};
	foreach entry in citizen_list
		if(TypeOf(entry) == "Array")
			if(!IsCitizenChar(converted, CitizenSerial(entry)))
				converted.append(entry);
			endif
			continue;
		endif

		var account := FindAccount(CStr(entry));
		if(!account)
			continue;
		endif
		var slot;
		for(slot := 1; slot <= CITIZEN_MAX_CHAR_SLOTS; slot := slot + 1)
			var character := account.GetCharacter(slot);
			if(character and CInt(GetObjProperty(character, "town")) == town_id and !IsCitizenChar(converted, character.serial))
				converted.append(NewCitizenEntry(character));
			endif
		endfor
	endforeach

	SetObjProperty(stone, "citizen", converted);
	SetObjProperty(stone, "populacao", converted.size());
	return converted;
endfunction

// Whether a vote rune was cast by this character. A vote from before Patch 1.1.5 holds the account
// name instead of a serial; it counts for every character on that account until the vote ends, so
// nobody votes twice in an election that spans the deploy.
function IsVoteRuneOf(runa, character)
	var voter := GetObjProperty(runa, "serial");
	if(TypeOf(voter) == "String")
		return (Lower(voter) == Lower(CStr(character.acctname)));
	endif
	return (CInt(voter) == CInt(character.serial));
endfunction
