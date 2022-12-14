/*
        Created by *Edwards
        For FantasiaShard.com
        
        Edwards@FantasiaShard.com
        
        2008-09-28
        
        Last Modifications 2009-12-20
             * Cleaned codes for better performances
*/
use uo;
use os;

include ":security:report";

function AuthorizationCode( mobile )

        if( GetObjProperty( mobile, "#AuthCodeGiven" ));
                return 1;
        endif

	var autocode := GetGlobalProperty( "CA" );

	if( !autocode )
		SendSysMessage( mobile, "Authorization code is not given yet.", 3, 53 );
		return 0;
	else
		var code := SendTextEntryGump( mobile, "Enter the Authorization Code.", TE_CANCEL_ENABLE,TE_STYLE_NORMAL, 10 );
		if( code == autocode )
                        SendSysMessage( mobile, "Code Accepted!", 3, 70 );
                        SetObjProperty( mobile, "#AuthCodeGiven", 1 );
                        return 1;
		else
			SendSysMessage( mobile, "It is not the Authorization code.", 3, 33 );
                        NotifyStaffOnline( mobile, "has provided a wrong authorization code." );
			return 0;
		endIf
	endIf
EndFunction

function NotifyStaffOnline( object, text )

         var name := object.name;
         if( !name )
                  name := object.desc;
                  if( !name )
                           name := object;
                  endif
         endif

         foreach character in EnumerateOnlineCharacters()
                 if( GetObjProperty( character, "Notify" ))
                       if( object.cmdlevel <= character.cmdlevel || character.cmdlevel >= 4 )
                               SendSysMessage( character, "You receive a notification:", 3, 55 );
                               SendSysMessage( character, name+" >>> "+text, 3, 55 );
                       endif
                 endif
                 
                 SleepMS(5);
         endforeach
         
         if( object.serial )
                  ReportText( name+"["+object.serial+"] >>> "+text, REPORT_SYSLOG );
         else
                  ReportText( name+" >>> "+text, REPORT_SYSLOG );
         endif

         return 1;
endfunction
