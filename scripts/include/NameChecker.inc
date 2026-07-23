use uo;
use os;
use cfgfile;

include ":gumps:include/gumps";
include ":gumps:include/gumps_ex";

var blacklist := {"developer", "admin", "gm", "cn", "dev", "seer", "coun", "counselor", "game master", "gamemaster", "administrator", "dev"};
// Fallback/baseline town list, used if regions.cfg lookup below fails for any reason.
// Kept in sync conceptually with GetKnownTownNames(), which also pulls every City=1
// region from regions.cfg (including player-run townstone regions) so the block covers
// custom town names too, not just the classic built-in cities.
var bLTowns := {"vesper", "minoc", "ocllo", "occlo", "cove", "jhelom", "nujel'm", "britain", "trinsic", "moonglow", "elven town", "nujelm", "magincia", "buccaneer's den", "serpent's hold", "skara brae", "yew", "buccaneers den", "serpents hold"};
var chars := {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", " "};

var MAX_LENGTH := 19;
var MIN_LENGTH := 3;

Set_Script_Option(SCRIPTOPT_NO_RUNAWAY, 1);

// Returns every town/townstone name that could ever be appended to a character
// name via Citizenship() in townstones/tstone.src, lowercased and apostrophe-stripped.
// Pulls from three sources so it stays correct even for player-run towns:
//  1. bLTowns - static fallback (original built-in cities)
//  2. config/regions.cfg City=1 entries - the actual source townstones are created from
//  3. GetGlobalProperty("townlist") - covers a townstone whose region entry was later removed
function GetKnownTownNames()
	var names := {};

	foreach thing in bLTowns
		var lname := lower(thing);
		if (!(lname in names))
			names.append(lname);
		endif
	endforeach

	var regions_cfg := ReadConfigFile("::regions/regions");
	if (regions_cfg && !regions_cfg.errortext)
		foreach region_name in GetConfigStringKeys(regions_cfg)
			var elem := FindConfigElem(regions_cfg, region_name);
			if (elem)
				var city_flag := CInt(GetConfigString(elem, "City"));
				if (city_flag == 1)
					var rname := lower(StrReplace(region_name, "'", ""));
					if (rname && !(rname in names))
						names.append(rname);
					endif
				endif
			endif
		endforeach
	endif

	var townlist := GetGlobalProperty("townlist");
	if (townlist)
		foreach entry in townlist
			if (entry)
				var ename := lower(StrReplace(CStr(entry), "'", ""));
				if (ename && !(ename in names))
					names.append(ename);
				endif
			endif
		endforeach
	endif

	return names;
endfunction

// If name ends with " of <town>" for any known town, returns name with that
// suffix removed. Otherwise returns name unchanged. Used so name-uniqueness
// comparisons treat "a pig" and "a pig of Trinsic" as the same identity -
// closes the exploit of holding both the bare and town-suffixed name at once.
function StripTownSuffix(name)
	var lname := lower(CStr(name));
	foreach town in GetKnownTownNames()
		var suffix := " of " + town;
		var suffix_len := len(suffix);
		if (len(lname) > suffix_len)
			if (lname[len(lname)-suffix_len+1, suffix_len] == suffix)
				return name[1, len(name)-suffix_len];
			endif
		endif
	endforeach
	return name;
endfunction

// Returns 1 if any character on any account (other than exclude_serial) already
// has the same name once town suffixes are stripped and case is ignored.
function IsNameTaken(candidate_name, exclude_serial := 0)
	var target_name := lower(StripTownSuffix(candidate_name));
	var accounts := ListAccounts();

	foreach account_name in accounts
		var account := FindAccount(account_name);
		var i := 1;
		repeat
			var ch := account.getcharacter(i);
			if (ch && ch.serial != exclude_serial)
				if (lower(StripTownSuffix(ch.name)) == target_name)
					return 1;
				endif
			endif
			i := i + 1;
		until(i >= 6);
	endforeach

	return 0;
endfunction

// This checks proposed names as well as current names on each log in.
// who is not passed on proposed names, but is on log in to check
// another name doesn't exist
// Staff don't have the check, and arn't checked against
//
// force_town_check: pass 1 whenever the player is actively choosing a brand-new
// name (character creation, the rename gump) so town names are always blocked
// regardless of the player's current town membership. Login/reconnect revalidation
// of an already-assigned, legitimately town-suffixed name should leave this 0.
function CheckName(name, who := 0, force_town_check := 0)

if (who.cmdlevel > 0)
	return 1;
endif

if(GetObjProperty( who, "IsRPer" ) || GetObjProperty( who, "WasRPer" ))
	return 1;
endif

if (len(name) > MAX_LENGTH && !GetObjProperty(who, "town"))
	return {0, "too long"};
elseif (len(name) < MIN_LENGTH)
	return {0, "too short"};
endif

foreach thing in blacklist
	if (find(lower(name), thing, 0))
		return {0, "blacklist"};
	endif
endforeach

if(force_town_check || !GetObjProperty(who, "town"))
	foreach thing in GetKnownTownNames()
		if (find(lower(name), thing, 0))
			return {0, "bLTowns"};
		endif
	endforeach
endif

var valid := 1;

var i := 1;
repeat

if (lower(name[i]) in chars)
	valid := 1;
else
	valid := 0;

endif
i := i + 1;
until (i > len(name) || valid == 0);

if (valid == 0)
	return {0, "illegal characters"};
endif;

if (IsNameTaken(name, who.serial))
	return {0, "duplicate"};
endif

return 1;

endfunction