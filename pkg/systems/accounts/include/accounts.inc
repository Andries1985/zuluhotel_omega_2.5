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
CONST ACCT_MAX_CHARACTERS	:= 5;

function CreateNewAccount( username, password, signup_ip, email:="", cmdlevel:=0, expansion:="" )

	var account := CreateAccount( username, password, 1 );
	if( account.errortext )
		return account;
	endif

	if( cmdlevel )
		account.SetDefaultCmdLevel( CInt( cmdlevel ));
	endif

	if( expansion )
		account.Set_UO_Expansion( expansion );
	endif

	if( email )
		account.SetProp( "EMail", email );
	endif
	
	account.SetProp( "CreatedAt", NowString());
	account.SetProp( "SignupIP", signup_ip );
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

// How long an account that passed the login server counts against its IP's MaxIpsOnline
// while it sits at server/character select, before it is in the world.
CONST ACCT_IPLOGIN_PENDING_SECS := 120;
CONST ACCT_IPLOGIN_PENDING_PROP := "#IPLimitPendingLogins";

// Staff and ExtraClient accounts are exempt from MaxIpsOnline.
function ACCT_IsIPLimitExempt( account )

	if( !account )
		return 0;
	elseif( account.defaultcmdlevel || account.GetProp( "ExtraClient" ))
		return 1;
	endif

	// A staff character on an account whose default cmdlevel was never raised.
	for slot := 1 to 7
		var chr := account.GetCharacter( slot );
		if( chr && chr.cmdlevel )
			return 1;
		endif
	endfor

	return 0;
endfunction

// Pending logins with expired entries dropped: dictionary{ ip -> dictionary{ acct_name -> expiry } }.
function ACCT_GetPendingIPLogins()

	var pending := dictionary{};
	var stored := GetGlobalProperty( ACCT_IPLOGIN_PENDING_PROP );
	if( Lower( TypeOf( stored )) != "dictionary" )
		return pending;
	endif

	var now := ReadGameClock();
	foreach ip_pending in stored
		var kept := dictionary{};
		foreach expires in ip_pending
			if( expires > now )
				kept[_expires_iter] := expires;
			endif
		endforeach
		if( kept.Size() )
			pending[_ip_pending_iter] := kept;
		endif
	endforeach

	return pending;
endfunction

// Returns 1 when one more client on 'acct_name' from 'ip' would go over MaxIpsOnline.
// Counts distinct non-exempt accounts on that IP other than acct_name itself (logging an
// account in again takes over its own session). With include_pending, accounts that passed
// the login server in the last ACCT_IPLOGIN_PENDING_SECS but are not in the world yet count
// too, so several clients logging in together cannot all get through before any of them is
// in the world. With reserve, an allowed login is recorded as pending.
// No sleeps: this runs inside the 0x80 login packet hook, which runs to completion.
function ACCT_IPLimitReached( acct_name, ip, include_pending:=0, reserve:=0 )

	var settings := ACCT_GetSettingsCfgElem( "Settings" );
	var max_online := GetConfigInt( settings, "MaxIpsOnline" );
	if( !max_online )
		return 0;
	endif

	acct_name := Lower( CStr( acct_name ));
	if( ACCT_IsIPLimitExempt( FindAccount( acct_name )))
		return 0;
	endif

	var counted := dictionary{};
	foreach player in EnumerateOnlineCharacters()
		var name := Lower( CStr( player.acctname ));
		if( player.ip != ip || player.cmdlevel || name == acct_name )
			continue;
		elseif( player.acct.GetProp( "ExtraClient" ))
			continue;
		endif
		counted[name] := 1;
	endforeach

	var pending := dictionary{};
	if( include_pending || reserve )
		pending := ACCT_GetPendingIPLogins();
	endif

	if( include_pending && pending.Exists( ip ))
		foreach expires in ( pending[ip] )
			if( _expires_iter != acct_name )
				counted[_expires_iter] := 1;
			endif
		endforeach
	endif

	if( counted.Size() >= max_online )
		return 1;
	endif

	if( reserve )
		var ip_pending := dictionary{};
		if( pending.Exists( ip ))
			ip_pending := pending[ip];
		endif
		ip_pending[acct_name] := ReadGameClock() + ACCT_IPLOGIN_PENDING_SECS;
		pending[ip] := ip_pending;
		SetGlobalProperty( ACCT_IPLOGIN_PENDING_PROP, pending );
	endif

	return 0;
endfunction

// In-world backstop (logon/reconnect). Over-limit logins are normally refused at the
// login server by hook/onLogin.src. Counts only characters in the world, as before.
function CheckForMaxClientsOnline( who )

	if( who.cmdlevel )
		return 1;
	endif

	return !ACCT_IPLimitReached( who.acctname, who.ip );
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
