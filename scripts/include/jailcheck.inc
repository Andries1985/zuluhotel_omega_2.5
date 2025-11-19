use uo;

function Jailcheck(who)
	if(who.realm == "britannia")
		if( who.x >= 5272 )
			if( who.x <= 5310 )
				if( who.y >= 1160 )
					if( who.y <= 1190 )
						return 1;
					endif
				endif
			endif
		endif
	endif

	return 0;
endfunction  