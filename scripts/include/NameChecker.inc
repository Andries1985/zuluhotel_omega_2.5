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
//
// Cached (per-program + GlobalProperty, same shape as the areas package's caches):
// regions.cfg is effectively static at runtime, and this used to be recomputed
// (a full regions.cfg read + parse) on EVERY StripTownSuffix() call, which
// IsNameTaken() was calling once per character slot per account - with 5000+
// accounts that was 20000+ full regions.cfg re-parses in a single name check,
// which is what caused the ~50 second rename-gump hang.
var _known_town_names_cache := 0;

const NAMECHECKER_TOWN_CACHE_KEY := "zh.namechecker.knowntownnames";

function GetKnownTownNames()
	if (_known_town_names_cache)
		return _known_town_names_cache;
	endif

	var global_cached := GetGlobalProperty(NAMECHECKER_TOWN_CACHE_KEY);
	if (global_cached && Lower(TypeOf(global_cached)) == "array")
		_known_town_names_cache := global_cached;
		return global_cached;
	endif

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

	_known_town_names_cache := names;
	SetGlobalProperty(NAMECHECKER_TOWN_CACHE_KEY, names);
	return names;
endfunction

// Call this after creating a new townstone (a new town name entering the
// world) so the cache picks up the new name instead of serving a stale list
// until the next server restart.
function InvalidateKnownTownNamesCache()
	_known_town_names_cache := 0;
	EraseGlobalProperty(NAMECHECKER_TOWN_CACHE_KEY);
	return 1;
endfunction

// If name ends with " of <town>" for any known town, returns name with that
// suffix removed. Otherwise returns name unchanged. Used so name-uniqueness
// comparisons treat "a pig" and "a pig of Trinsic" as the same identity -
// closes the exploit of holding both the bare and town-suffixed name at once.
//
// town_names is optional - pass the result of GetKnownTownNames() when calling
// this in a loop (e.g. IsNameTaken) so it isn't recomputed on every call.
function StripTownSuffix(name, town_names := 0)
	if (!town_names)
		town_names := GetKnownTownNames();
	endif

	var lname := lower(CStr(name));
	foreach town in town_names
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

// Lowercases, trims leading/trailing spaces, and collapses any run of
// interior spaces down to one. Applied on top of StripTownSuffix() so a
// malformed stored name (e.g. a double space left over from a base name
// that already had a trailing space before " of Town" got appended) still
// compares equal to the clean version instead of silently evading the
// duplicate check.
function NormalizeNameForCompare(name)
	var result := lower(CStr(name));
	while (find(result, "  ", 0))
		result := StrReplace(result, "  ", " ");
	endwhile
	if (len(result) && result[1] == " ")
		result := result[2, len(result)-1];
	endif
	if (len(result) && result[len(result)] == " ")
		result := result[1, len(result)-1];
	endif
	return result;
endfunction

// Returns 1 if any character on any account (other than exclude_serial) already
// has the same name once town suffixes are stripped, whitespace is normalized,
// and case is ignored.
function IsNameTaken(candidate_name, exclude_serial := 0)
	var town_names := GetKnownTownNames();
	var target_name := NormalizeNameForCompare(StripTownSuffix(candidate_name, town_names));
	var accounts := ListAccounts();

	foreach account_name in accounts
		var account := FindAccount(account_name);
		var i := 1;
		repeat
			var ch := account.getcharacter(i);
			if (ch && ch.serial != exclude_serial)
				if (NormalizeNameForCompare(StripTownSuffix(ch.name, town_names)) == target_name)
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

// Leading/trailing/doubled spaces are technically "legal characters" per the
// loop below, but they let a base name like "Rahl " silently turn into a
// double space ("Rahl  of Town") once Citizenship() appends the town suffix -
// which then evades exact-match duplicate detection. Reject them outright,
// unconditionally (not gated by force_town_check), so an already-malformed
// stored name also gets caught and corrected on next login.
if (name[1] == " " || name[len(name)] == " " || find(name, "  ", 0))
	return {0, "bad spacing"};
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

// Human-readable explanation for a CheckName() failure reason code (the
// [2] element of its returned {0, reason} array), so callers can tell staff
// WHY a name was rejected instead of a single generic "invalid" message that
// makes every rejection look like a duplicate-name hit.
function NameCheckFailureReason(reason_code)
	case (reason_code)
		"too long":    return "it is longer than "+MAX_LENGTH+" characters";
		"too short":   return "it is shorter than "+MIN_LENGTH+" characters";
		"bad spacing": return "it starts/ends with a space, or contains a double space";
		"blacklist":   return "it contains a reserved/blacklisted word";
		"bLTowns":     return "it contains a town name";
		"duplicate":   return "another character already has that name";
		default:       return "it failed name validation";
	endcase
endfunction