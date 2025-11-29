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

function CheckForMaxClientsOnline(who)
    var settings := ACCT_GetSettingsCfgElem("Settings");
    var MaxIpsOnline := GetConfigInt(settings, "MaxIpsOnline");
    var MaxDiscordsOnline := GetConfigInt(settings, "MaxDiscordsOnline");
    var MaxCharsOnline := GetConfigInt(settings, "MaxCharsOnline");
    var MaxCharsPerIP := GetConfigInt(settings, "MaxCharsPerIP");

    if (!MaxIpsOnline || !MaxDiscordsOnline || !MaxCharsOnline || !MaxCharsPerIP)
        return 1; // Allow login if no limits are set
    endif

    var ini_ip := who.ip;
    var ini_discordID := who.acct.getprop("DiscordID");
    var ini_character := who.name;
    var ini_extraclient := (who.acct.getprop("ExtraClient") == 1) ? 1 : 0;

    var playerData := dictionary;

    // Step 1: Populate playerData dictionary
    foreach player in EnumerateOnlineCharacters()
        if (player.cmdlevel == 0)
            var player_ip := player.ip;
            var player_discordID := player.acct.getprop("DiscordID");
            var player_character := player.name;
            var player_extraclient := (player.acct.getprop("ExtraClient") == 1) ? 1 : 0;

            if (!playerData.exists(player_ip))
                playerData.insert(player_ip, dictionary {});
            endif

            var ipData := playerData[player_ip];

            if (!ipData.exists(player_discordID))
                ipData.insert(player_discordID, dictionary {
                    "Characters" -> array,
                    "ExtraClient" -> player_extraclient
                });
            endif

            var discordData := ipData[player_discordID];

            if (!discordData["Characters"].contains(player_character))
                discordData["Characters"].append(player_character);
            endif

            discordData["ExtraClient"] := player_extraclient;
            ipData.insert(player_discordID, discordData);
            playerData.insert(player_ip, ipData);
		endif
        
		SleepMS(10);
    
	endforeach

    // Step 2: Enforcement Checks
    foreach ip in playerData
		Print(playerData);
        var ipData := playerData[ip]; // Retrieve the dictionary for this IP
        var totalCharsPerIP := 0;

        foreach discordID in ipData
            var discordData := ipData[discordID];
            var characterList := discordData["Characters"];
            var extraClient := discordData["ExtraClient"];

            // Check 1: Prevent multiple IPs on a single Discord ID
            var ipCount := 0;
            foreach checkIP in playerData
                var checkData := playerData[checkIP];
                if (checkData.exists(discordID))
                    ipCount += 1;
                endif
            endforeach

            if (ipCount > 1)
                Print("Disconnecting: Discord ID " + discordID + " is logging in from multiple IPs.");
                return 0;
            endif

            // Check 2: Prevent multiple Discord IDs per IP (unless ExtraClient is enabled)
            var discordCount := ipData.size(); // Count Discord IDs under this IP
            if (discordCount > 1 && extraClient == 0)
                Print("Disconnecting: Multiple Discord IDs from the same IP without ExtraClient.");
                return 0;
            endif

            // Check 3: Prevent more than 2 characters per Discord ID (unless ExtraClient is enabled)
            if (characterList.size() > 2 && extraClient == 0)
                Print("Disconnecting: Too many characters under Discord ID " + discordID);
                return 0;
            endif

            // Check 4: ExtraClient Rules
            if (extraClient == 1)
                if (discordCount > 2)
                    Print("Disconnecting: ExtraClient limit exceeded - More than 2 Discord IDs from a single IP.");
                    return 0;
                endif
				if (characterList.size() > 2)
                    Print("Disconnecting: ExtraClient limit exceeded - More than 2 characters per Discord ID.");
                    return 0;
				endif

            totalCharsPerIP += characterList.size();
			endif
        endforeach

        // Check 5: Total character limit per IP
        if (totalCharsPerIP > 4)
            Print("Disconnecting: Too many total characters from a single IP.");
            return 0;
		endif
    endforeach

    return 1;
endfunction

function ACCT_FindValidPlayer(player)

	var playerserial := 0;
	var acctlist := ListAccounts();
	var i := 1;

	// Stuff for verifying that the player exists
	foreach acctnam in acctlist
		var acctref := FindAccount(acctnam);
		for ( i := 1; i<=5; i:=i+1 )
			var playername := CStr(acctref.GetCharacter(i).name);
			if(!playername)
				continue;
			endif
			if ( Lower(playername) == Lower(player) )
				playerserial := acctref.GetCharacter(i).serial;
				break;
			endif
		endfor
	endforeach

  return playerserial;

endfunction