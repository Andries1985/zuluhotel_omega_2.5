//////////////////////////////////////////////////////			//
//Help system include							//
//functions here							//
//Icanpage(thechar) //returns if the char can use the page system now	//
//staffcheck() //set the number of online gms and counselors		//
//pagecheck(who, type) //check if who have paged, return  page num or 0 //
//pagewarning(type, message) //send to all staff types the message 	//
//////////////////////////////////////////////////////////////////////////
use uo;


//globals variables
var counpages := {};
var gmpages := {};

var OnlineCounselors;
var OnlineStaff;

counpages := GetGlobalProperty("counpages");
gmpages := GetGlobalProperty("gmpages");

function GetStaff()
Var Staffs:=array;
	foreach player in EnumerateOnlineCharacters()
		if( player.cmdlevel >= 1)
			Staffs.append( player );
		endif
	endforeach
return  Staffs;
endfunction

function pagecheck(serial, type)

var cserial := cint(serial);
var retval := 0;
var i;
//type if 1 for gm pages and 2 for counpages

case(cint(type))
1:	var maxi := gmpages.size(); 
	
	for(i := 0; i < maxi; i := i + 1)
		if (cserial == cint(gmpages[i]))
			retval := i;
			break;
		endif	
	endfor


2:	var maxi := counpages.size();
	for(i := 0; i < maxi; i := i + 1)
		if (cserial == Cint(counpages[i]))
			retval := i;
			break;
		endif	
	endfor

endcase

return retval;

endfunction


function SendPageToStaff(thepage, who, Staffs)

	foreach char in Staffs
			SendSysmessage( char, "Page from " + who.name + " ["+ readgameclock() +"]: " + thepage);
	endforeach

endfunction
