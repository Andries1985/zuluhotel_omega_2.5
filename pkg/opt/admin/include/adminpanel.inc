use uo;
use datafile;
use basic;

include "include/time";

const ADMINPANEL_PKG := "admin";

const ADMINPANEL_INITIATOR_STAFF  := "Staff";
const ADMINPANEL_INITIATOR_PLAYER := "Player";
const ADMINPANEL_INITIATOR_SYSTEM := "System";

// Reserved element key for account-wide (not character-specific) records,
// e.g. notes_history. Character elements are always keyed by a numeric
// serial, so this can never collide with one.
const ADMINPANEL_ACCOUNT_ELEMENT_KEY := "_account_";

// Hard cap on stored entries per history array (name_history/death_history/
// poison_history/notes_history), enforced on every write so a long-lived,
// frequently-renamed/killed/noted account or character can't grow any of
// them without bound.
const ADMINPANEL_MAX_HISTORY := 100;

function AdminPanelFilespec(acctname)
	return ":"+ADMINPANEL_PKG+":"+CStr(acctname);
endfunction

function OpenAdminPanelDataFile(acctname)
	var filespec := AdminPanelFilespec(acctname);
	var df := OpenDataFile(filespec);
	if(!df)
		CreateDataFile(filespec, flags := DF_KEYTYPE_STRING);
		df := OpenDataFile(filespec);
	endif
	return df;
endfunction

// Drops the oldest entries until history.size() <= max_count.
function TrimHistoryToLimit(history, max_count)
	while(history.size() > max_count)
		history.erase(1);
	endwhile
	return history;
endfunction

// The name of the currently-running .src (e.g. "setname", "SetProp"),
// matching the same GetProcess(GetPid())/split-on-"/" idiom staff.inc's
// LogCommand() already uses elsewhere in this codebase.
function CurrentScriptName()
	var script_path := CStr(GetProcess(GetPid()).name);
	var parts := SplitWords(script_path, "/");
	if(!parts.size())
		return script_path;
	endif
	return parts[parts.size()];
endfunction

// Formats a raw epoch/game-clock second count the same way NowString() does
// (see include/time.inc), so account timestamps (e.g. LastLogin) display
// consistently with everything else in this panel. "" if secs is falsy.
function FormatEpochTimestamp(secs)
	secs := CInt(secs);
	if(!secs)
		return "";
	endif
	return (GetDateString(secs) + " " + GetTimeString(secs));
endfunction

// Best-effort account name for whoever (or whatever) holds `serial` right
// now - works for offline mobiles too. "" if unresolvable or not a PC.
function ResolveAccountNameFromSerial(serial)
	serial := CInt(serial);
	if(!serial)
		return "";
	endif
	var obj := SystemFindObjectBySerial(serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	if(!obj)
		return "";
	endif
	// NPCs (and anything else not attached to an account) return an error
	// struct here rather than "" - don't let that get CStr()'d into history.
	var acctname := obj.acctname;
	if(acctname == error)
		return "";
	endif
	return CStr(acctname);
endfunction


///////////////////////////////////////////////////////////////////////////
// Names
///////////////////////////////////////////////////////////////////////////

// A single history entry: struct{"name","timestamp","script","initiator"}.
function MakeNameHistoryEntry(name, initiator_type)
	var entry := struct;
	entry.+name := CStr(name);
	entry.+timestamp := NowString();
	entry.+script := CurrentScriptName();
	entry.+initiator := CStr(initiator_type);
	return entry;
endfunction

// Bootstrap entry for a name we only know about because it was the OLD name
// on the first tracked rename - we don't know what actually set it (could be
// character creation, or a rename from before this system existed), so it's
// left blank rather than misattributed to the current rename's script/initiator.
function MakeUnknownNameHistoryEntry(name)
	var entry := struct;
	entry.+name := CStr(name);
	entry.+timestamp := "";
	entry.+script := "";
	entry.+initiator := "";
	return entry;
endfunction

// Entries recorded before timestamp/script/initiator existed are bare
// strings, or structs missing the newer fields - normalize so callers only
// ever deal with the full struct shape.
function NormalizeNameHistoryEntry(entry)
	var normalized := struct;

	if(Lower(TypeOf(entry)) == "string")
		normalized.+name := CStr(entry);
		normalized.+timestamp := "";
		normalized.+script := "";
		normalized.+initiator := "";
		return normalized;
	endif

	normalized.+name := entry.name;

	var ts := entry.timestamp;
	if(!ts)
		ts := "";
	endif
	normalized.+timestamp := ts;

	var scr := entry.script;
	if(!scr)
		scr := "";
	endif
	normalized.+script := scr;

	var init := entry.initiator;
	if(!init)
		init := "";
	endif
	normalized.+initiator := init;

	return normalized;
endfunction

// Call this BEFORE the caller applies the rename (SetName()/.name:=), so
// mobile.name here still holds the value being replaced. initiator_type
// should be ADMINPANEL_INITIATOR_STAFF or ADMINPANEL_INITIATOR_PLAYER.
function RecordCharacterNameChange(mobile, new_name, initiator_type)
	if(!mobile or !mobile.isa(POLCLASS_MOBILE) or !mobile.acct)
		return;
	endif

	var old_name := CStr(mobile.name);
	new_name := CStr(new_name);
	if(!new_name or new_name == old_name)
		return;
	endif

	var acctname := CStr(mobile.acctname);
	var df := OpenAdminPanelDataFile(acctname);
	if(!df)
		return;
	endif

	var char_key := CStr(mobile.serial);
	var elem := df.FindElement(char_key);
	if(!elem)
		elem := df.CreateElement(char_key);
	endif
	if(!elem)
		UnloadDataFile(AdminPanelFilespec(acctname));
		return;
	endif

	var history := elem.GetProp("name_history");
	if(!history)
		history := {};
	endif

	if(!history.size())
		history.append(MakeUnknownNameHistoryEntry(old_name));
	endif
	history.append(MakeNameHistoryEntry(new_name, initiator_type));
	history := TrimHistoryToLimit(history, ADMINPANEL_MAX_HISTORY);

	elem.SetProp("name_history", history);

	UnloadDataFile(AdminPanelFilespec(acctname));
endfunction

// Stored name-history array (chronological, oldest first; each entry a
// struct{"name","timestamp","script","initiator"}) for a character serial on
// an account. {} if nothing has been recorded for it yet.
function GetStoredNameHistory(acctname, serial)
	var df := OpenDataFile(AdminPanelFilespec(acctname));
	if(!df)
		return {};
	endif

	var elem := df.FindElement(CStr(serial));
	var history := {};
	if(elem)
		history := elem.GetProp("name_history");
		if(!history)
			history := {};
		endif
	endif

	UnloadDataFile(AdminPanelFilespec(acctname));
	return history;
endfunction

// struct{"lines","total","remaining"} - "Current: X (timestamp)" /
// "Previous: Y (timestamp, initiator, via script)" lines (most recent
// previous name first) for the given live mobile, capped to the most recent
// `limit` names total (Current counts as one). "remaining" is how many
// stored entries didn't fit under that cap.
function BuildNameHistorySummary(mobile, limit := 10)
	var lines := {};
	var current := CStr(mobile.name);
	var history := GetStoredNameHistory(CStr(mobile.acctname), CStr(mobile.serial));

	var current_line := "Current: "+current;
	if(history.size())
		var last_entry := NormalizeNameHistoryEntry(history[history.size()]);
		if(last_entry.name == current && last_entry.timestamp)
			current_line := current_line+"  ("+last_entry.timestamp+")";
		endif
	endif
	lines.append(current_line);

	var i;
	for(i := history.size(); i >= 1; i := i - 1)
		if(lines.size() >= limit)
			break;
		endif

		var entry := NormalizeNameHistoryEntry(history[i]);
		if(entry.name == current)
			continue;
		endif

		var line := "Previous: "+entry.name;
		var detail := "";
		if(entry.timestamp)
			detail := entry.timestamp;
		endif
		if(entry.initiator)
			if(detail)
				detail := detail+", ";
			endif
			detail := detail+entry.initiator;
		endif
		if(entry.script)
			if(detail)
				detail := detail+", ";
			endif
			detail := detail+"via "+entry.script;
		endif
		if(detail)
			line := line+"  ("+detail+")";
		endif
		lines.append(line);
	endfor

	var summary := struct;
	summary.+lines := lines;
	summary.+total := history.size();
	var remaining := history.size() - lines.size();
	if(remaining < 0)
		remaining := 0;
	endif
	summary.+remaining := remaining;

	return summary;
endfunction


///////////////////////////////////////////////////////////////////////////
// Deaths
///////////////////////////////////////////////////////////////////////////

// A single death entry: struct{"killed_by","killed_by_serial","killed_by_account","x","y","z","timestamp"}.
function MakeDeathHistoryEntry(killed_by, killed_by_serial, x, y, z)
	var entry := struct;
	entry.+killed_by := CStr(killed_by);
	entry.+killed_by_serial := CInt(killed_by_serial);
	entry.+killed_by_account := ResolveAccountNameFromSerial(killed_by_serial);
	entry.+x := CInt(x);
	entry.+y := CInt(y);
	entry.+z := CInt(z);
	entry.+timestamp := NowString();
	return entry;
endfunction

// Entries recorded before killed_by_account existed are missing that field -
// normalize so callers only ever deal with the full struct shape.
function NormalizeDeathHistoryEntry(entry)
	var acct := entry.killed_by_account;
	// Older records could carry an unresolved GetObjProperty()/obj.acctname
	// error struct (stringified via CStr) instead of "" - e.g. killed by an
	// NPC, which isn't attached to an account. Treat those as blank too.
	if(!acct or Find(CStr(acct), "errortext", 1))
		acct := "";
	endif
	entry.killed_by_account := acct;
	return entry;
endfunction

// Call this once a character's death is confirmed and its KilledBy/
// KilledBySerial/deathx/deathy/deathz cprops are set (see scripts/misc/chrdeath.src)
// but before anything erases them.
function RecordCharacterDeath(mobile, killed_by, killed_by_serial, x, y, z)
	if(!mobile or !mobile.isa(POLCLASS_MOBILE) or !mobile.acct)
		return;
	endif

	var acctname := CStr(mobile.acctname);
	var df := OpenAdminPanelDataFile(acctname);
	if(!df)
		return;
	endif

	var char_key := CStr(mobile.serial);
	var elem := df.FindElement(char_key);
	if(!elem)
		elem := df.CreateElement(char_key);
	endif
	if(!elem)
		UnloadDataFile(AdminPanelFilespec(acctname));
		return;
	endif

	var history := elem.GetProp("death_history");
	if(!history)
		history := {};
	endif

	history.append(MakeDeathHistoryEntry(killed_by, killed_by_serial, x, y, z));
	history := TrimHistoryToLimit(history, ADMINPANEL_MAX_HISTORY);

	elem.SetProp("death_history", history);

	UnloadDataFile(AdminPanelFilespec(acctname));
endfunction

// Stored death-history array (chronological, oldest first; each entry a
// struct{"killed_by","killed_by_serial","killed_by_account","x","y","z","timestamp"})
// for a character serial on an account. {} if nothing has been recorded yet.
function GetStoredDeathHistory(acctname, serial)
	var df := OpenDataFile(AdminPanelFilespec(acctname));
	if(!df)
		return {};
	endif

	var elem := df.FindElement(CStr(serial));
	var history := {};
	if(elem)
		history := elem.GetProp("death_history");
		if(!history)
			history := {};
		endif
	endif

	UnloadDataFile(AdminPanelFilespec(acctname));
	return history;
endfunction

// struct{"lines","total","remaining"} - "Killed by X, account, at (x,y,z)
// (timestamp)" lines (most recent first) for the given live mobile, capped
// to the most recent `limit` deaths. "remaining" is how many didn't fit.
function BuildDeathHistorySummary(mobile, limit := 5)
	var lines := {};
	var history := GetStoredDeathHistory(CStr(mobile.acctname), CStr(mobile.serial));

	var i;
	for(i := history.size(); i >= 1; i := i - 1)
		if(lines.size() >= limit)
			break;
		endif

		var entry := NormalizeDeathHistoryEntry(history[i]);
		var killed_by := entry.killed_by;
		// Some older records stored an unresolved GetObjProperty() error struct
		// (stringified via CStr) instead of a name, back when chrdeath.src didn't
		// guard against a missing KilledBy cprop - display those as Unknown too.
		if(!killed_by or Find(killed_by, "errortext", 1))
			killed_by := "Unknown";
		endif

		var line := "Killed by "+killed_by;
		if(entry.killed_by_serial and CInt(entry.killed_by_serial) == CInt(mobile.serial))
			line := line+" (Suicide)";
		endif
		if(entry.killed_by_account)
			line := line+", "+entry.killed_by_account;
		endif
		line := line+", at ("+entry.x+","+entry.y+","+entry.z+")";
		if(entry.timestamp)
			line := line+"  ("+entry.timestamp+")";
		endif
		lines.append(line);
	endfor

	var summary := struct;
	summary.+lines := lines;
	summary.+total := history.size();
	var remaining := history.size() - lines.size();
	if(remaining < 0)
		remaining := 0;
	endif
	summary.+remaining := remaining;

	return summary;
endfunction


///////////////////////////////////////////////////////////////////////////
// Poisonings
///////////////////////////////////////////////////////////////////////////

// A single poison entry: struct{"poisoned_by","level","timestamp"}.
function MakePoisonHistoryEntry(poisoned_by, level)
	var entry := struct;
	entry.+poisoned_by := CStr(poisoned_by);
	entry.+level := CInt(level);
	entry.+timestamp := NowString();
	return entry;
endfunction

// Call this wherever a character actually gets poisoned (see SetPoison() in
// scripts/include/dotempmods.inc) - records every poisoning, not just fatal ones.
function RecordCharacterPoisoning(mobile, poisoned_by, level)
	if(!mobile or !mobile.isa(POLCLASS_MOBILE) or !mobile.acct)
		return;
	endif

	var acctname := CStr(mobile.acctname);
	var df := OpenAdminPanelDataFile(acctname);
	if(!df)
		return;
	endif

	var char_key := CStr(mobile.serial);
	var elem := df.FindElement(char_key);
	if(!elem)
		elem := df.CreateElement(char_key);
	endif
	if(!elem)
		UnloadDataFile(AdminPanelFilespec(acctname));
		return;
	endif

	var history := elem.GetProp("poison_history");
	if(!history)
		history := {};
	endif

	history.append(MakePoisonHistoryEntry(poisoned_by, level));
	history := TrimHistoryToLimit(history, ADMINPANEL_MAX_HISTORY);

	elem.SetProp("poison_history", history);

	UnloadDataFile(AdminPanelFilespec(acctname));
endfunction

// Stored poison-history array (chronological, oldest first; each entry a
// struct{"poisoned_by","level","timestamp"}) for a character serial on an
// account. {} if nothing has been recorded yet.
function GetStoredPoisonHistory(acctname, serial)
	var df := OpenDataFile(AdminPanelFilespec(acctname));
	if(!df)
		return {};
	endif

	var elem := df.FindElement(CStr(serial));
	var history := {};
	if(elem)
		history := elem.GetProp("poison_history");
		if(!history)
			history := {};
		endif
	endif

	UnloadDataFile(AdminPanelFilespec(acctname));
	return history;
endfunction

// struct{"lines","total","remaining"} - "Poisoned by X (timestamp)" lines
// (most recent first), capped to the most recent `limit` poisonings.
function BuildPoisonHistorySummary(mobile, limit := 5)
	var lines := {};
	var history := GetStoredPoisonHistory(CStr(mobile.acctname), CStr(mobile.serial));

	var i;
	for(i := history.size(); i >= 1; i := i - 1)
		if(lines.size() >= limit)
			break;
		endif

		var entry := history[i];
		var poisoned_by := entry.poisoned_by;
		if(!poisoned_by)
			poisoned_by := "Unknown";
		endif

		var line := "Poisoned by "+poisoned_by;
		if(entry.timestamp)
			line := line+"  ("+entry.timestamp+")";
		endif
		lines.append(line);
	endfor

	var summary := struct;
	summary.+lines := lines;
	summary.+total := history.size();
	var remaining := history.size() - lines.size();
	if(remaining < 0)
		remaining := 0;
	endif
	summary.+remaining := remaining;

	return summary;
endfunction


///////////////////////////////////////////////////////////////////////////
// Account notes
///////////////////////////////////////////////////////////////////////////
// Account-wide, not character-specific - stored on the reserved
// ADMINPANEL_ACCOUNT_ELEMENT_KEY element instead of a character serial.
// This is purely an audit trail alongside the existing acc.GetProp("Notes")/
// acc.setprop("Notes", ...) mechanism, which is left completely untouched -
// staff still read/write the single "latest note" the same way they always
// have; this just additionally remembers up to the last 100 of them.

function MakeNoteHistoryEntry(note_text, initiator_type)
	var entry := struct;
	entry.+note := CStr(note_text);
	entry.+timestamp := NowString();
	entry.+script := CurrentScriptName();
	entry.+initiator := CStr(initiator_type);
	return entry;
endfunction

// Call this AFTER acc.setprop("Notes", note_text) succeeds, passing the same
// account and resulting note text. initiator_type should be
// ADMINPANEL_INITIATOR_STAFF, ADMINPANEL_INITIATOR_PLAYER, or
// ADMINPANEL_INITIATOR_SYSTEM (e.g. an automated jail note).
function RecordAccountNote(account, note_text, initiator_type)
	if(!account or account.errortext or !note_text)
		return;
	endif

	var acctname := CStr(account.name);
	var df := OpenAdminPanelDataFile(acctname);
	if(!df)
		return;
	endif

	var elem := df.FindElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	if(!elem)
		elem := df.CreateElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	endif
	if(!elem)
		UnloadDataFile(AdminPanelFilespec(acctname));
		return;
	endif

	var history := elem.GetProp("notes_history");
	if(!history)
		history := {};
	endif

	history.append(MakeNoteHistoryEntry(note_text, initiator_type));
	history := TrimHistoryToLimit(history, ADMINPANEL_MAX_HISTORY);

	elem.SetProp("notes_history", history);

	UnloadDataFile(AdminPanelFilespec(acctname));
endfunction

// Stored note-history array (chronological, oldest first; each entry a
// struct{"note","timestamp","script","initiator"}) for an account. {} if
// nothing has been recorded yet.
function GetStoredNoteHistory(acctname)
	var df := OpenDataFile(AdminPanelFilespec(acctname));
	if(!df)
		return {};
	endif

	var elem := df.FindElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	var history := {};
	if(elem)
		history := elem.GetProp("notes_history");
		if(!history)
			history := {};
		endif
	endif

	UnloadDataFile(AdminPanelFilespec(acctname));
	return history;
endfunction

// struct{"note","timestamp","initiator"} - the current acc.GetProp("Notes")
// text, paired with who/when recorded it, read off the last matching
// notes_history entry. If the stored note doesn't match the last history
// entry (e.g. it was set before this tracking existed, or by a path this
// panel doesn't hook), timestamp/initiator come back blank rather than
// misattributed.
function GetLatestNoteInfo(account)
	var info := struct;
	var raw_note := account.GetProp("Notes");
	if(raw_note == error)
		info.+note := "";
	else
		info.+note := CStr(raw_note);
	endif
	info.+timestamp := "";
	info.+initiator := "";

	var history := GetStoredNoteHistory(CStr(account.name));
	if(!history.size())
		return info;
	endif

	var last_entry := history[history.size()];
	if(CStr(last_entry.note) != info.note)
		return info;
	endif

	var ts := last_entry.timestamp;
	if(ts)
		info.timestamp := ts;
	endif

	var init := last_entry.initiator;
	if(init)
		info.initiator := init;
	endif

	return info;
endfunction


///////////////////////////////////////////////////////////////////////////
// Login IPs
///////////////////////////////////////////////////////////////////////////
// Account-wide, not character-specific - stored on the reserved
// ADMINPANEL_ACCOUNT_ELEMENT_KEY element like notes_history. Unlike the other
// histories this one de-duplicates: one entry per unique IP the account has
// ever logged in from, with a hit count and first/last-seen timestamps,
// rather than one entry per login.

// A single IP entry: struct{"ip","first_seen","last_seen","count"}.
function MakeIPHistoryEntry(ip)
	var entry := struct;
	entry.+ip := CStr(ip);
	entry.+first_seen := NowString();
	entry.+last_seen := NowString();
	entry.+count := 1;
	return entry;
endfunction

// Call this on login (see program logon() in scripts/playermanager.src)
// with the account and the IP the client connected from.
function RecordAccountLoginIP(account, ip)
	if(!account or account.errortext)
		return;
	endif
	ip := CStr(ip);
	if(!ip)
		return;
	endif

	var acctname := CStr(account.name);
	var df := OpenAdminPanelDataFile(acctname);
	if(!df)
		return;
	endif

	var elem := df.FindElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	if(!elem)
		elem := df.CreateElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	endif
	if(!elem)
		UnloadDataFile(AdminPanelFilespec(acctname));
		return;
	endif

	var history := elem.GetProp("ip_history");
	if(!history)
		history := {};
	endif

	var i;
	var found := 0;
	for(i := 1; i <= history.size(); i := i + 1)
		if(history[i].ip == ip)
			history[i].last_seen := NowString();
			history[i].count := CInt(history[i].count) + 1;
			found := 1;
			break;
		endif
	endfor

	if(!found)
		history.append(MakeIPHistoryEntry(ip));
		history := TrimHistoryToLimit(history, ADMINPANEL_MAX_HISTORY);
	endif

	elem.SetProp("ip_history", history);

	UnloadDataFile(AdminPanelFilespec(acctname));
endfunction

// Stored IP-history array (one entry per unique IP, ordered by when it was
// first seen; each entry a struct{"ip","first_seen","last_seen","count"})
// for an account. {} if nothing has been recorded yet.
function GetStoredIPHistory(acctname)
	var df := OpenDataFile(AdminPanelFilespec(acctname));
	if(!df)
		return {};
	endif

	var elem := df.FindElement(ADMINPANEL_ACCOUNT_ELEMENT_KEY);
	var history := {};
	if(elem)
		history := elem.GetProp("ip_history");
		if(!history)
			history := {};
		endif
	endif

	UnloadDataFile(AdminPanelFilespec(acctname));
	return history;
endfunction

// "X.X.X.X  (seen Nx, first: ..., last: ...)" for a single ip_history entry
// (struct{"ip","first_seen","last_seen","count"}) - shared by
// BuildIPHistorySummary and any UI that needs to render entries individually
// (e.g. one button + line per IP) instead of a flat "lines" array.
function FormatIPHistoryLine(entry)
	var line := CStr(entry.ip)+"  (seen "+CStr(entry.count)+"x";
	if(entry.first_seen)
		line := line+", first: "+entry.first_seen;
	endif
	if(entry.last_seen)
		line := line+", last: "+entry.last_seen;
	endif
	line := line+")";
	return line;
endfunction

// struct{"lines","total","remaining"} - one formatted line per unique IP
// (most recently first-seen IP first), capped to the most recent `limit`
// unique IPs.
function BuildIPHistorySummary(acctname, limit := 10)
	var lines := {};
	var history := GetStoredIPHistory(acctname);

	var i;
	for(i := history.size(); i >= 1; i := i - 1)
		if(lines.size() >= limit)
			break;
		endif

		lines.append(FormatIPHistoryLine(history[i]));
	endfor

	var summary := struct;
	summary.+lines := lines;
	summary.+total := history.size();
	var remaining := history.size() - lines.size();
	if(remaining < 0)
		remaining := 0;
	endif
	summary.+remaining := remaining;

	return summary;
endfunction

// Every account name whose live "IP" property or recorded ip_history
// contains `ip` - i.e. every account that has ever logged in from that
// address, not just whoever currently holds it. Sorted alphabetically.
function FindAccountsByIP(ip)
	ip := CStr(ip);
	var matches := {};
	if(!ip)
		return matches;
	endif

	foreach acctname in (ListAccounts())
		var name := CStr(acctname);
		if(!name)
			continue;
		endif

		var is_match := 0;

		var account := FindAccount(name);
		if(account && !account.errortext)
			var raw_ip := account.GetProp("IP");
			if(raw_ip != error && CStr(raw_ip) == ip)
				is_match := 1;
			endif
		endif

		if(!is_match)
			var history := GetStoredIPHistory(name);
			var i;
			for(i := 1; i <= history.size(); i := i + 1)
				if(history[i].ip == ip)
					is_match := 1;
					break;
				endif
			endfor
		endif

		if(is_match)
			matches.append(name);
		endif
	endforeach

	matches.sort();
	return matches;
endfunction
