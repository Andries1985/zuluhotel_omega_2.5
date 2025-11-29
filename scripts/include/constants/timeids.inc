/////////////////////////////////////////////////////////////////////
// TimeIDs.inc
//
// Contain most used values for time conversion.
/////////////////////////////////////////////////////////////////////


const TIMEID_DAY_SUNDAY                 := 0;
const TIMEID_DAY_MONDAY                 := 1;
const TIMEID_DAY_TUESDAY                := 2;
const TIMEID_DAY_WEDNESDAY              := 3;
const TIMEID_DAY_THURSDAY               := 4;
const TIMEID_DAY_FRIDAY                 := 5;
const TIMEID_DAY_SATURDAY               := 6; // Day ids

const TIMEID_DAYS_PER_WEEK              := 7; // Days per week.

const TIMEID_MONTH_JANUARY              := 1;
const TIMEID_MONTH_FEBUARY              := 2;
const TIMEID_MONTH_MARCH                := 3;
const TIMEID_MONTH_APRIL                := 4;
const TIMEID_MONTH_MAY                  := 5;
const TIMEID_MONTH_JUNE                 := 6;
const TIMEID_MONTH_JULY                 := 7;
const TIMEID_MONTH_AUGUST               := 8;
const TIMEID_MONTH_SEMPTEMBER           := 9;
const TIMEID_MONTH_OCTOBER              := 10;
const TIMEID_MONTH_NOVEMBER             := 11;
const TIMEID_MONTH_DECEMBER             := 12; // Month ids

const TIMEID_MONTHS_PER_YEAR			:= 12; // Months per year

const TIMEID_MONTH_NAME_JANUARY			:= "January";
const TIMEID_MONTH_NAME_FEBUARY			:= "Febuary";
const TIMEID_MONTH_NAME_MARCH			:= "March";
const TIMEID_MONTH_NAME_APRIL			:= "April";
const TIMEID_MONTH_NAME_MAY             := "May";
const TIMEID_MONTH_NAME_JUNE			:= "June";
const TIMEID_MONTH_NAME_JULY			:= "July";
const TIMEID_MONTH_NAME_AUGUST			:= "August";
const TIMEID_MONTH_NAME_SEMPTEMBER		:= "September";
const TIMEID_MONTH_NAME_OCTOBER			:= "October";
const TIMEID_MONTH_NAME_NOVEMBER		:= "November";
const TIMEID_MONTH_NAME_DECEMBER		:= "December"; // Month names

const TIMEID_SECONDS_PER_MINUTE			:= 60;              // Seconds per min.
const TIMEID_SECONDS_PER_HOUR			:= 3600;            // Seconds per hour.
const TIMEID_SECONDS_PER_DAY			:= 86400;           // Seconds per day.
const TIMEID_SECONDS_PER_WEEK			:= 604800;          // Seconds per week.
const TIMEID_SECONDS_PER_YEAR_NORMAL		:= 31536000;    // Seconds per normal year.
const TIMEID_SECONDS_PER_YEAR_LEAP		:= 31622400;        // Seconds per leap year.
const TIMEID_SECONDS_PER_FOUR_YEAR_CYCLE    := 126230400;   // Seconds per 4 year cycles.
const TIMEID_SECONDS_TO_YEAR_2000		:= 946684800;       // Seconds from 01/01/1970 to 01/01/2000.
const TIMEID_SECONDS_TO_YEAR_2001		:= 978307200;       // Seconds from 01/01/1970 to 01/01/2001.

const TIMEID_DAYS_MOD_FOR_NORMAL_YEAR   := 1;           // Day id increase per full normal year.
const TIMEID_DAYS_MOD_FOR_LEAP_YEAR		:= 2;               // Day id increase per full leap year.

const TIMEID_JANUARY_1ST_2001			:= TIMEID_DAY_MONDAY;	// Was what day on 01/01/2001.