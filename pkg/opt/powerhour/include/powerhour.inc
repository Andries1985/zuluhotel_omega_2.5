/////////////////////////////////////////////////////////////////////////////
//
// powerhour.inc - shared constants and helpers for the powerhour package.
//
// Server-wide ("global") powerhour state lives in global properties so it
// survives a restart. The scheduler (powerhour.src) owns every state
// transition; .phadmin only writes requests/settings and nudges it.
//
//   PHH / PHC / PHS      - the active type flag (read by loot/crafting/skill code)
//   PH_EndTime           - systime at which the active powerhour ends
//   PH_Source            - what started it: "scheduled", "bonus" or "admin"
//   PH_Pending           - type currently in its 2-minute warning
//   PH_Request           - type an admin asked to start (consumed by the scheduler)
//   PH_SecondChance      - 1 when a bonus powerhour has been granted and is waiting to start
//   PH_StartTime         - weekly schedule anchor (absent = the original Sunday 19:00 GMT)
//   PH_SchedulerPID      - pid of the running scheduler script
//
// Personal powerhour state lives on the character: #PPHH/#PPHC/#PPHS (the
// active type, not saved so it dies with a restart), pph_use_time and
// pph_use_weekday (the weekly lock, saved) and #SettingPH (gump open).
//
/////////////////////////////////////////////////////////////////////////////

use uo;
use os;
use basic;

const PH_MINUTE := 60;
const PH_HOUR   := 3600;
const PH_DAY    := 86400;
const PH_WEEK   := 604800;

// 2018/11/25 19:00 GMT (a Sunday) - the original hard-coded weekly anchor.
const PH_DEFAULT_START_TIME := 1543172400;
// 2018/11/25 00:00 GMT - Sunday midnight, used to rebuild an anchor from weekday/hour/minute.
const PH_REFERENCE_SUNDAY := 1543104000;

const PH_DURATION        := 3600; // seconds a powerhour lasts
const PH_WARNING_SECONDS := 120;  // broadcast lead time before a powerhour starts
const PH_TICK_SECONDS    := 60;   // scheduler poll interval

const PH_TYPE_NONE  := 0;
const PH_TYPE_HUNT  := 1;
const PH_TYPE_RES   := 2;
const PH_TYPE_SKILL := 3;

const PH_GPROP_HUNT       := "PHH";
const PH_GPROP_RES        := "PHC";
const PH_GPROP_SKILL      := "PHS";
const PH_GPROP_END_TIME   := "PH_EndTime";
const PH_GPROP_SOURCE     := "PH_Source";
const PH_GPROP_PENDING    := "PH_Pending";
const PH_GPROP_REQUEST    := "PH_Request";
const PH_GPROP_SECOND     := "PH_SecondChance";
const PH_GPROP_START_TIME := "PH_StartTime";
const PH_GPROP_SCHEDULER  := "PH_SchedulerPID";

const PH_SOURCE_SCHEDULED := "scheduled";
const PH_SOURCE_BONUS     := "bonus";
const PH_SOURCE_ADMIN     := "admin";

const PH_SCHEDULER_SCRIPT := ":powerhour:powerhour";
const PH_WAKE_EVENT       := "ph_wake";

const PPH_PROP_HUNT     := "#PPHH";
const PPH_PROP_RES      := "#PPHC";
const PPH_PROP_SKILL    := "#PPHS";
const PPH_PROP_USE_TIME := "pph_use_time";
const PPH_PROP_WEEKDAY  := "pph_use_weekday";
const PPH_PROP_SETTING  := "#SettingPH";

const PH_BROADCAST_COLOR := 1176;
const PH_MSG_COLOR_GOOD  := 2601;
const PH_MSG_COLOR_BAD   := 2595;

///////////////////////////////////////////////////////////////////////////
// Types
///////////////////////////////////////////////////////////////////////////

function PH_TypeName(ph_type)
	case(CInt(ph_type))
		PH_TYPE_HUNT:  return "Hunting";
		PH_TYPE_RES:   return "Half-resources";
		PH_TYPE_SKILL: return "Double skillgain";
	endcase
	return "Unknown";
endfunction

// Global property that flags this type as the active server-wide powerhour.
function PH_GlobalProp(ph_type)
	case(CInt(ph_type))
		PH_TYPE_HUNT:  return PH_GPROP_HUNT;
		PH_TYPE_RES:   return PH_GPROP_RES;
		PH_TYPE_SKILL: return PH_GPROP_SKILL;
	endcase
	return "";
endfunction

// Obj property that flags this type as the character's active personal powerhour.
function PPH_Prop(ph_type)
	case(CInt(ph_type))
		PH_TYPE_HUNT:  return PPH_PROP_HUNT;
		PH_TYPE_RES:   return PPH_PROP_RES;
		PH_TYPE_SKILL: return PPH_PROP_SKILL;
	endcase
	return "";
endfunction

function PH_IsValidType(ph_type)
	ph_type := CInt(ph_type);
	return (ph_type >= PH_TYPE_HUNT && ph_type <= PH_TYPE_SKILL);
endfunction

///////////////////////////////////////////////////////////////////////////
// Server-wide powerhour state
///////////////////////////////////////////////////////////////////////////

function PH_ActiveType()
	if(GetGlobalProperty(PH_GPROP_HUNT))
		return PH_TYPE_HUNT;
	elseif(GetGlobalProperty(PH_GPROP_RES))
		return PH_TYPE_RES;
	elseif(GetGlobalProperty(PH_GPROP_SKILL))
		return PH_TYPE_SKILL;
	endif
	return PH_TYPE_NONE;
endfunction

function PH_EndTime()
	return CInt(GetGlobalProperty(PH_GPROP_END_TIME));
endfunction

function PH_SecondsRemaining(time_now)
	var end_time := PH_EndTime();
	if(!end_time)
		return 0;
	endif
	var left := end_time - CInt(time_now);
	if(left < 0)
		left := 0;
	endif
	return left;
endfunction

function PH_PendingType()
	return CInt(GetGlobalProperty(PH_GPROP_PENDING));
endfunction

function PH_RequestedType()
	return CInt(GetGlobalProperty(PH_GPROP_REQUEST));
endfunction

function PH_BonusPending()
	return CInt(GetGlobalProperty(PH_GPROP_SECOND));
endfunction

// Clears everything that describes the active server-wide powerhour.
function PH_ClearActive()
	EraseGlobalProperty(PH_GPROP_HUNT);
	EraseGlobalProperty(PH_GPROP_RES);
	EraseGlobalProperty(PH_GPROP_SKILL);
	EraseGlobalProperty(PH_GPROP_END_TIME);
	EraseGlobalProperty(PH_GPROP_SOURCE);
endfunction

///////////////////////////////////////////////////////////////////////////
// Weekly schedule
///////////////////////////////////////////////////////////////////////////

function PH_GetStartTime()
	var start_time := CInt(GetGlobalProperty(PH_GPROP_START_TIME));
	if(start_time <= 0)
		start_time := PH_DEFAULT_START_TIME;
	endif
	return start_time;
endfunction

// 0 = Sunday ... 6 = Saturday, in GMT. Epoch day 0 (1970-01-01) was a Thursday.
function PH_WeekdayOf(secs)
	return (CInt(CInt(secs) / PH_DAY) + 4) % 7;
endfunction

function PH_WeekdayName(weekday)
	var names := {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	weekday := CInt(weekday);
	if(weekday < 0 || weekday > 6)
		return "?";
	endif
	return names[weekday + 1];
endfunction

// Breaks the schedule anchor into weekday (0-6), hour (0-23) and minute (0-59), all GMT.
function PH_GetSchedule(byref weekday, byref hour, byref minute)
	var offset := (PH_GetStartTime() - PH_REFERENCE_SUNDAY) % PH_WEEK;
	if(offset < 0)
		offset := offset + PH_WEEK;
	endif
	weekday := offset / PH_DAY;
	hour    := (offset % PH_DAY) / PH_HOUR;
	minute  := (offset % PH_HOUR) / PH_MINUTE;
	return 1;
endfunction

// Stores a new weekly schedule. Returns the new anchor, or 0 if the values are out of range.
function PH_SetSchedule(weekday, hour, minute)
	weekday := CInt(weekday);
	hour    := CInt(hour);
	minute  := CInt(minute);
	if(weekday < 0 || weekday > 6 || hour < 0 || hour > 23 || minute < 0 || minute > 59)
		return 0;
	endif
	var anchor := PH_REFERENCE_SUNDAY + (weekday * PH_DAY) + (hour * PH_HOUR) + (minute * PH_MINUTE);
	SetGlobalProperty(PH_GPROP_START_TIME, anchor);
	return anchor;
endfunction

// e.g. "Sunday 19:00 GMT"
function PH_ScheduleString()
	var weekday, hour, minute;
	PH_GetSchedule(weekday, hour, minute);
	return PH_WeekdayName(weekday) + " " + PH_TwoDigits(hour) + ":" + PH_TwoDigits(minute) + " GMT";
endfunction

function PH_TwoDigits(number)
	number := CInt(number);
	if(number < 10)
		return "0" + number;
	endif
	return CStr(number);
endfunction

// Seconds until the next scheduled powerhour moment (1 .. PH_WEEK).
function PH_SecondsUntilScheduled(time_now)
	var elapsed := (CInt(time_now) - PH_GetStartTime()) % PH_WEEK;
	if(elapsed < 0)
		elapsed := elapsed + PH_WEEK;
	endif
	return PH_WEEK - elapsed;
endfunction

// True while we are inside the warning window just before the scheduled moment.
function PH_InWarningWindow(time_now)
	return (PH_SecondsUntilScheduled(time_now) <= PH_WARNING_SECONDS);
endfunction

// "2 days, 3 hours and 4 minutes" / "5 minutes" / "less than a minute"
function PH_FormatDuration(secs, show_seconds := 0)
	secs := CInt(secs);
	if(secs < 0)
		secs := 0;
	endif
	var days    := secs / PH_DAY;
	var hours   := (secs % PH_DAY) / PH_HOUR;
	var minutes := (secs % PH_HOUR) / PH_MINUTE;
	var seconds := secs % PH_MINUTE;

	var parts := {};
	if(days)
		parts.append(CStr(days) + PH_Plural(days, " day"));
	endif
	if(hours)
		parts.append(CStr(hours) + PH_Plural(hours, " hour"));
	endif
	if(minutes)
		parts.append(CStr(minutes) + PH_Plural(minutes, " minute"));
	endif
	if(show_seconds && seconds)
		parts.append(CStr(seconds) + PH_Plural(seconds, " second"));
	endif

	if(!parts.size())
		if(show_seconds)
			return "0 seconds";
		endif
		return "less than a minute";
	endif

	var text := "";
	var i;
	for(i := 1; i <= parts.size(); i := i + 1)
		if(i > 1)
			if(i == parts.size())
				text := text + " and ";
			else
				text := text + ", ";
			endif
		endif
		text := text + parts[i];
	endfor
	return text;
endfunction

function PH_Plural(count, word)
	if(CInt(count) == 1)
		return word;
	endif
	return word + "s";
endfunction

///////////////////////////////////////////////////////////////////////////
// Scheduler process
///////////////////////////////////////////////////////////////////////////

// The running scheduler process, or 0.
function PH_FindScheduler()
	var pid := CInt(GetGlobalProperty(PH_GPROP_SCHEDULER));
	if(!pid)
		return 0;
	endif
	var process := GetProcess(pid);
	if(!process)
		return 0;
	endif
	// Guard against pid reuse by an unrelated script after a restart.
	var name := CStr(process.name);
	if(!Find(name, "powerhour/powerhour", 1) && !Find(name, "powerhour\\powerhour", 1))
		return 0;
	endif
	return process;
endfunction

// The running scheduler, starting it if it is not running. Returns the process ref or 0.
function PH_EnsureScheduler()
	var process := PH_FindScheduler();
	if(process)
		return process;
	endif
	process := Start_Script(PH_SCHEDULER_SCRIPT);
	if(!process)
		return 0;
	endif
	return process;
endfunction

// Wakes the scheduler so it acts on a request/setting change now instead of on its next tick.
function PH_NudgeScheduler()
	var process := PH_EnsureScheduler();
	if(!process)
		return 0;
	endif
	var ev := struct;
	ev.+type := PH_WAKE_EVENT;
	process.sendevent(ev);
	return 1;
endfunction

///////////////////////////////////////////////////////////////////////////
// Personal powerhour
///////////////////////////////////////////////////////////////////////////

function PPH_ActiveType(who)
	if(GetObjProperty(who, PPH_PROP_HUNT))
		return PH_TYPE_HUNT;
	elseif(GetObjProperty(who, PPH_PROP_RES))
		return PH_TYPE_RES;
	elseif(GetObjProperty(who, PPH_PROP_SKILL))
		return PH_TYPE_SKILL;
	endif
	return PH_TYPE_NONE;
endfunction

function PPH_UseTime(who)
	return CInt(GetObjProperty(who, PPH_PROP_USE_TIME));
endfunction

function PPH_UseWeekday(who)
	return CInt(GetObjProperty(who, PPH_PROP_WEEKDAY));
endfunction

// Seconds left on the character's active personal powerhour (0 if none).
function PPH_SecondsRemaining(who, time_now)
	if(!PPH_ActiveType(who))
		return 0;
	endif
	var left := PPH_UseTime(who) + PH_DURATION - CInt(time_now);
	if(left < 0)
		left := 0;
	endif
	return left;
endfunction

// The weekly lock lifts on the first Sunday 00:00 GMT after the last use, or a full
// week after it if it was used on a Sunday.
function PPH_IsEligible(who, time_now)
	var use_time := PPH_UseTime(who);
	if(!use_time)
		return 1;
	endif
	time_now := CInt(time_now);
	var weekday_now := PH_WeekdayOf(time_now);
	var use_weekday := PPH_UseWeekday(who);
	return ((weekday_now < use_weekday) || (time_now > use_time + PH_WEEK));
endfunction

// Seconds until PPH_IsEligible() becomes true (0 if it already is).
function PPH_SecondsUntilEligible(who, time_now)
	time_now := CInt(time_now);
	if(PPH_IsEligible(who, time_now))
		return 0;
	endif
	var use_time    := PPH_UseTime(who);
	var use_weekday := PPH_UseWeekday(who);
	var reset_time;
	if(use_weekday == 0)
		// Used on a Sunday: "weekday_now < 0" can never be true, so only the full week applies.
		reset_time := use_time + PH_WEEK + 1;
	else
		reset_time := (CInt(use_time / PH_DAY) + (7 - use_weekday)) * PH_DAY;
	endif
	var left := reset_time - time_now;
	if(left < 0)
		left := 0;
	endif
	return left;
endfunction

// Wipes every personal powerhour property (active flags, weekly lock, gump flag).
function PPH_Clear(who)
	EraseObjProperty(who, PPH_PROP_USE_TIME);
	EraseObjProperty(who, PPH_PROP_WEEKDAY);
	EraseObjProperty(who, PPH_PROP_HUNT);
	EraseObjProperty(who, PPH_PROP_RES);
	EraseObjProperty(who, PPH_PROP_SKILL);
	EraseObjProperty(who, PPH_PROP_SETTING);
	return 1;
endfunction
