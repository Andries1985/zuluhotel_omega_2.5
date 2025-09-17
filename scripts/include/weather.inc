use util;
use os;

function StartWeather()

	/*
	const OUTDOORS := "outdoors";
	const ICEISLAND := "IceIsland";
	const THUNDER := 0x29;

	local severity := 25;
	local currentweather := randomint(3);
	local waxing := 1;

	local nextweatherchange := ReadGameClock() + ( 60 * Randomint(10) );

	while (1)

		if ( ReadGameClock() > nextweatherchange )
			nextweatherchange := ReadGameClock() + ( 60 * Randomint(10) );
			if (waxing)
				severity := severity + 25;
				if ( severity > 100 )
					severity := 75;
					waxing := 0;
				endif
			else
				severity := severity -25;
				if (severity < 1)
					severity := 25;
					waxing := 1;
					if ( randomint(4) != 1 )
						currentweather := 0;
					else
						currentweather := randomint(3);
					endif					
				endif
			endif
		endif

		SetRegionWeatherLevel( OUTDOORS, currentweather, severity, 0, 1);
		SetRegionWeatherLevel( ICEISLAND, 2, 50, 0, 1);

		sleep(10);
		if ( currentweather = 1 and severity > 75 )
			if ( randomint(10) = 1 )
				foreach plr in EnumerateOnlineCharacters()
					if ( plr.x < 5088 and plr.y < 4048 )
						PlaySoundEffect(plr, THUNDER + RandomInt(2) );
					endif
				endforeach
			endif
		endif

	endwhile
	*/
	
endfunction
