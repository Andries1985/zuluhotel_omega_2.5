use uo;


//include "include/all";
include "include/shard";
include "include/constants/timeids";


function CalculateDate( byref secs, byref year, byref month, byref day )

	if( ON_SUMMER_HOUR )
		secs := secs + TIMEID_SECONDS_PER_HOUR;
	endif

	secs := secs + (ShardTimeZone() * TIMEID_SECONDS_PER_HOUR);

	if( secs >= TIMEID_SECONDS_TO_YEAR_2001 )
		year := 2001;
		secs := secs - TIMEID_SECONDS_TO_YEAR_2001;

	elseif( secs >= TIMEID_SECONDS_TO_YEAR_2000 )
		year := 2000;
		secs := secs - TIMEID_SECONDS_TO_YEAR_2000;
	else
		year := 1970;
	endif
	
	if( secs >= TIMEID_SECONDS_PER_FOUR_YEAR_CYCLE )
		year := year + (Cint( secs / TIMEID_SECONDS_PER_FOUR_YEAR_CYCLE ) * 4);
		secs := secs % TIMEID_SECONDS_PER_FOUR_YEAR_CYCLE;
	endif
	
	while( secs >= TIMEID_SECONDS_PER_YEAR_NORMAL )
		if( year % 4 )
			year := year + 1;
			secs := secs - TIMEID_SECONDS_PER_YEAR_NORMAL;
		elseif( secs >= TIMEID_SECONDS_PER_YEAR_LEAP )
			year := year + 1;
			secs := secs - TIMEID_SECONDS_PER_YEAR_LEAP;
		else
			month	:= TIMEID_MONTH_DECEMBER;
			day	:= 31;
			secs	:= secs - TIMEID_SECONDS_PER_YEAR_NORMAL;
			return 1;
		endif
	endwhile
	
	
	var months := { 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if( year % 4 )
		months[2] := 28;
	else
		months[2] := 29;
	endif
	
	month := 1;
	while( secs >= (months[month] * TIMEID_SECONDS_PER_DAY) )
		secs	:= secs - (months[month] * TIMEID_SECONDS_PER_DAY);
		month	:= month + 1;
		
	endwhile
	
	day	:= 1 + Cint( secs / TIMEID_SECONDS_PER_DAY );
	secs	:= secs % TIMEID_SECONDS_PER_DAY;
	
	return 1;
	
endfunction


function CalculateTime( byref secs, byref hour, byref minutes )

	hour	:= Cint( secs / TIMEID_SECONDS_PER_HOUR );
	secs	:= secs % TIMEID_SECONDS_PER_HOUR;
	
	minutes	:= Cint( secs / TIMEID_SECONDS_PER_MINUTE );
	secs	:= secs % TIMEID_SECONDS_PER_MINUTE;
	
	return 1;
	
endfunction


function GetCurrentTimeData()

	var time_data := struct;
	time_data.+year;
	time_data.+month;
	time_data.+day;
	time_data.+hour;
	time_data.+minutes;
	time_data.+seconds := polcore().systime;
	
	CalculateDate( time_data.seconds, time_data.year, time_data.month, time_data.day );
	CalculateTime( time_data.seconds, time_data.hour, time_data.minutes );
	
	return time_data;

endfunction


function GetDateString( byref secs )

	var year;
	var month;
	var day;

	CalculateDate( secs, year, month, day );
	
	if( month >= 10 )
		year := year + "-" + month;
	else
		year := year + "-0" + month;
	endif

	if( day >= 10 )
		year := year + "-" + day;
	else
		year := year + "-0" + day;
	endif

	return year;

endfunction


function GetMacroString()

	if( MACRO_HOUR_START == MACRO_HOUR_END )
		return "Macroing is not allowed at this time, you will be jailed when caught";
	endif
	
	var secs := polcore().systime;
	var year;
	var month;
	var day;
	var hour;
	var minutes;
	
	CalculateDate( secs, year, month, day );
	CalculateTime( secs, hour, minutes );
	if( MACRO_HOUR_START < MACRO_HOUR_END )
		if( hour >= MACRO_HOUR_START )
			if( hour < MACRO_HOUR_END )
				var hrs_left := MACRO_HOUR_END - hour - 1;
				if( hrs_left <= 0 )
					return ("Macroing is allowed for another " + (60 - minutes) + " mins" );
				else
					return ("Macroing is allowed for another " + hrs_left + " hrs " + (60 - minutes) + " mins" );
				endif
			endif
		endif
	else
		var finish := MACRO_HOUR_END + 24;
		hour	:= hour + 24;
		if( hour >= MACRO_HOUR_START )
			if( hour < finish )
				var hrs_left := finish - hour - 1;
				if( hrs_left <= 0 )
					return ("Macroing is allowed for another " + (60 - minutes) + " mins" );
				else
					return ("Macroing is allowed for another " + hrs_left + " hrs " + (60 - minutes) + " mins" );
				endif
			endif
		endif
	endif
	
	return ("Macroing is not allowed at this time, you will be jailed when caught");

endfunction


function GetTimeString( byref secs, since_1970 := 0 )

	var hour;
	var minutes;
	
	if( since_1970 )
		var year;
		var month;
		var day;
		CalculateDate( secs, year, month, day );
	endif
	
	CalculateTime( secs, hour, minutes );
	if( minutes >= 10 )
		hour := hour + ":" + minutes;
	else
		hour := hour + ":0" + minutes;
	endif

	if( secs >= 10 )
		hour := hour + ":" + secs;
	else
		hour := hour + ":0" + secs;
	endif
	
	return hour;
	
endfunction


function NowString()

	var secs := polcore().systime;
	return (GetDateString(secs) + " " + GetTimeString(secs));

endfunction



