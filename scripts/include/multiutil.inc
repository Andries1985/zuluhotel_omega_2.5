use uo;

include "include/constants/propids";
include "include/damages";

function FindMultiOwner( house )

	return FindMobile( Cint(GetObjProperty(house,PROPID_MULTI_OWNER)) );

endfunction


function GetMultiAtLocation( x, y, z := 0, realm )
	
	return GetStandingHeight( cint(x), cint(y), cint(z), realm).multi;

endfunction


function IsHouseCoOwner( mobile, house )

	var co_owners := GetObjProperty( house, PROPID_MULTI_CO_OWNERS );
	if( !co_owners )
		return 0;
	elseif( mobile.serial in co_owners )
		return 1;
	else
		return 0;
	endif
	
endfunction


function IsHouseFriend( mobile, house )

	var friends := GetObjProperty( house, PROPID_MULTI_FRIENDS );
	if( !friends )
		return 0;
	elseif( mobile.serial in friends )
		return 1;
	else
		return 0;
	endif
	
endfunction


function IsHouseOwner( mobile, house )

	if( GetObjProperty( house, PROPID_MULTI_OWNER ) == mobile.serial )
		return 1;
	else
		return 0;
	endif
	
endfunction


function IsWelcomeInHouse( mobile, house )

	if( mobile.cmdlevel )
		return 1;
	endif
	
	if( GetObjProperty( house, PROPID_MULTI_OWNER ) == mobile.serial )
		return 1;
	endif
	
	var co_owners := GetObjProperty( house, PROPID_MULTI_CO_OWNERS );
	if( !co_owners )
		co_owners := {};
	endif
	
	if( mobile.serial in co_owners )
		return 1;
	endif
	
	var friends := GetObjProperty( house, PROPID_MULTI_FRIENDS );
	if( !friends )
		return 0;
	elseif( mobile.serial in friends )
		return 1;
	else
		return 0;
	endif

endfunction
