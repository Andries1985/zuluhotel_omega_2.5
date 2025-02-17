/*
        Maintened by *Edwards
        For FantasiaShard.com
        
        Edwards@FantasiaShard.com
        
        2009-06-04
        
        Last Modifications 2010-01-24
             * Changed order of param for CreateNewAccount()
             * Added signup_ip param to CreateNewAccount()

        Last Modifications 2010-03-01
             * Removed MaxAccountIPs()
             * Removed CheckForMaxClientsOnline()
*/
use uo;
use os;

include "include/security";
include ":accounts:settings";
include "include/time";

// Max numbers of characters an account can have.
CONST ACCT_MAX_CHARACTERS	:= 7;

function CreateNewAccount( username, password, discordID, email, pin, expansion:="" )

	var account := CreateAccount( username, password, 1 );
	if( account.errortext )
		return account;
	endif

	account := FindAccount(UserName);
	account.setprop("DiscordID", discordID);
	account.setprop("Email", email);
	account.setprop("Pin", CInt(pin));
	account.setprop("Login ", NowString());
	account.setprop("Notes", "<none>");
	account.setprop("Release", 0);
	account.SetProp( "CreatedAt", NowString());

	if( expansion )
		account.Set_UO_Expansion( expansion );
	endif
	
	//account.SetProp( "SignupIP", signup_ip );
	ACCT_SetLastLogin( account );

	return account;
endfunction

function ACCT_SetLastLogin( account, time:=0 )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	if( !time )
		time := POLCore().systime;
	endif
    
	return account.SetProp( "LastLogin", time );
endfunction

function ACCT_GetLastLogin( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	return CInt( account.GetProp( "LastLogin" ));
endfunction

function ACCT_GetCharacters( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	var mobile_list := array;
	for i:=1 to ACCT_MAX_CHARACTERS
		var mobile := account.GetCharacter(i);
		if( mobile )
			mobile_list.Append( mobile );
		endif
		SleepMS(2);
	endfor

	return mobile_list;
endfunction

function ACCT_GetNumCharacters( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	var count := 0;
	for i:=1 to ACCT_MAX_CHARACTERS
		if( account.GetCharacter( i ))
			count := count+1;
		endif
		SleepMS(2);
	endfor

	return count;
endfunction

function ACCT_DeleteAllCharacters( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	var count := 0;
	for i:=1 to ACCT_MAX_CHARACTERS
		account.DeleteCharacter( i );
		SleepMS(2);
	endfor

	return count;
endfunction

function ACCT_DeleteAccount( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	foreach package in ( POLCore().packages )
		Run_Script_To_Completion( ":"+package+":acctDelete/acctDelete", account );
		SleepMS(2);
	endforeach

	ACCT_DeleteAllCharacters( account );

	return account.Delete();
endfunction

// function CheckForMaxClientsOnline( who )

//     var settings := ACCT_GetSettingsCfgElem( "Settings" );
//     var IpsOnline := GetConfigInt( settings, "MaxIpsOnline" );
//         if( !IpsOnline )
//         return 1;
//     endif

//     var discordIDsOnline := GetConfigInt( settings, "MaxDiscordsOnline" );
//         if( !discordIDsOnline )
//         return 1;
//     endif
	
//     var CharsOnline := GetConfigInt( settings, "MaxCharsOnline" );
//         if( !CharsOnline )
//         return 1;
//     endif

// 	if(who.acct.getprop("ExtraClient") == 1)
// 		//IpsOnline += 1;
//         discordIDsOnline += 1;
// 		CharsOnline += 2;
//     endif

//     var ini_ip := who.ip;
//     var ips := 0;
//     var chars := 0;
// 	var dcids := 0;

//     foreach player in EnumerateOnlineCharacters()
//         if(player.cmdlevel == 0)
// 			if( player.getprop("DiscordID") == who.acct.getprop("DiscordID"))
// 				dcids += 1;
// 			endif
// 			if( player.ip == ini_ip )
// 				ips += 2;
// 			endif
// 			chars += 1;
// 		endif
// 		//if(player.cmdlevel == 0)
// 		//        chars += 1;
// 		//    endif
// 		SleepMS(10);
//     endforeach


// 	if( ( dcids ) <= discordIDsOnline )
// 		if( chars > CharsOnline )
// 			Print("Disconnecting Char for too many Characters Online");
// 			return 0;
// 		elseif(ips >= IpsOnline )
// 		    Print("Disconnecting Char for too many IPs Online");
//         	return 0;
// 		endif
// 	elseif((dcids-1) > discordIDsOnline)
// 		Print("Disconnecting Char for too many Discord IDs Online");
//         return 0;
// 	endif
    
// //	if( chars > CharsOnline )
// //		Print("Disconnecting Char for too many Characters Online");
// //		return 0;
// //	elseif( ips > IpsOnline )
// //		Print("Disconnecting Char for too many IPs Online");
// //		return 0;
// //	elseif( dcids > discordIDsOnline )
// //		Print("Disconnecting Char for too many Discord IDs Online");
// //		return 0;
// //	endif
//     return 1;

// endfunction

function VerifyStaffOnline( who )

        foreach player in EnumerateOnlineCharacters()
                SleepMS(5);

                if( who == player )
                        continue;
                elseif( player.ip != who.ip )
                        continue;
                endif

                /*if( !who.cmdlevel && player.cmdlevel < 3 )
                        DisconnectClient( player );
                elseif( who.cmdlevel <�3 && !player.cmdlevel )
                        DisconnectClient( who );
                endif*/
        endforeach

        return 1;
endfunction

//ChatGPT Version
function CheckForMaxClientsOnline(who)

    var settings := ACCT_GetSettingsCfgElem("Settings");
    var MaxIpsOnline := GetConfigInt(settings, "MaxIpsOnline");  // Default: 1 IP
    var MaxDiscordsOnline := GetConfigInt(settings, "MaxDiscordsOnline"); // Default: 1 Discord per IP
    var MaxCharsOnline := GetConfigInt(settings, "MaxCharsOnline");  // Default: 2 chars per Discord
    var MaxCharsPerIP := GetConfigInt(settings, "MaxCharsPerIP"); // Default: 2 characters per IP

    if( !MaxIpsOnline || !MaxDiscordsOnline || !MaxCharsOnline || !MaxCharsPerIP)
        return 1; // If no limits are set, allow login
    endif

    var ini_ip := who.ip;
    var ini_discordID := who.acct.getprop("DiscordID");

    // Check if the account has ExtraClient enabled
    if( who.acct.getprop("ExtraClient") == 1 )
        MaxDiscordsOnline := 2; // Allows 2 Discord IDs per IP
        MaxCharsOnline := 2; // Allows up to 2 characters per Discord ID
        MaxCharsPerIP := 4; // Allows up to 4 characters per IP if 2 Discord IDs allowed
    endif

    // **Tracking Variables**
    var charsPerDiscord := 0;
    var charsPerIP := 0;
    var discordIDsPerIP := 0; // Tracks unique Discord IDs per this IP

    foreach player in EnumerateOnlineCharacters()
        if( player.cmdlevel == 0 ) // Only count normal players

            // **Step 1: Count unique Discord IDs per IP**
            if( player.ip == ini_ip )
                if( player.getprop("DiscordID") == who.acct.getprop("DiscordID") )
                    discordIDsPerIP += 1;
                endif
				// **Step 2: Count characters per IP**
				charsPerIP += 1;
				// **Step 3: Count characters for this specific Discord ID**
				if( player.getprop("DiscordID") == ini_discordID )
                	charsPerDiscord += 1;
            	endif
            endif
        endif
        SleepMS(10); // Prevent CPU overload
    endforeach

	Print(" DCPIP " + discordIDsPerIP);

    // **Step 1: Enforce Discord ID per IP rule first**
    if( discordIDsPerIP > MaxDiscordsOnline )
        Print("Disconnecting: Too many Discord IDs from this IP.");
        return 0;
    endif

    // **Step 2: Enforce character limit per Discord ID**
    if( charsPerDiscord > MaxCharsOnline )
        Print("Disconnecting: Too many characters for this Discord ID.");
        return 0;
    endif

    // **Step 3: Enforce character limit per IP**
    if( charsPerIP > MaxCharsPerIP  )
        Print("Disconnecting: Too many characters from this IP.");
        return 0;
    endif

    return 1;

endfunction