
include "include/classes";

function get_direction( character , x , y , z:= 0 )

	var ns_desc := "";
	var ew_desc := "";
	var thedesc;

	if (y < character.y)
		  ns_desc := "north";
	elseif  (y > character.y)
		  ns_desc := "south";
	endif
 
	if (x < character.x)
		  ew_desc := "west";
	elseif  (x > character.x)
		  ew_desc := "east";
	endif
	 
	if (ns_desc == "" and ew_desc == "")
		thedesc := "nearby";
	else 
		thedesc := ns_desc + ew_desc ;
	endif

	return thedesc;

endfunction


function CreateNorthSouthField( caster , fieldtype , x , y , z := 0 , length := 5 )

	var counter := 0, count := 0;
	var is_mage := GetObjProperty( caster , CLASSEID_MAGE );
	var is_cleric := GetObjProperty( caster , CLASSEID_CLERIC );
	var i;
	var fires := { };
	
	for ( i := x - 2; i <= x - 2 + length; i := i + 1)
	count := count+1;
		if( !len(ListMobilesNearLocation( i , y , z , 0 )) )
			counter := counter  + 1;
			fires[counter] := CreateItemAtLocation( i , y , z , fieldtype , 1 );
			SetObjProperty( fires[counter] , "Creator" , caster.serial );
			SetObjProperty( fires[counter] , "f" , 1 );
			if( is_mage )
				SetObjProperty( fires[counter] , "ByTrueMage" , 1 );
			endif
		endif
		if(count > 100)
			return fires;
		endif
	endfor
	
	return fires;
	
endfunction


function CreateEastWestField( caster , fieldtype , x , y , z := 0, length := 5 )
		
	var counter := 0, count := 0;
	var is_mage := GetObjProperty( caster , CLASSEID_MAGE );
	var is_cleric := GetObjProperty( caster , CLASSEID_CLERIC );
	var i;
	var fires := { };
	
	for ( i := y - 2; i <= y - 2 + length; i := i + 1)
	count := count+1;
		if( !len(ListMobilesNearLocation( x , i , z , 0 )) )
			counter := counter  + 1;
			fires[counter] := CreateItemAtLocation( x , i , z , fieldtype , 1 );
			SetObjProperty( fires[counter] , "Creator" , caster.serial );
			SetObjProperty( fires[counter] , "f" , 1 );
			if( is_mage )
				SetObjProperty( fires[counter] , "ByTrueMage" , 1 );
			endif
		endif
		if(count > 100)
			return fires;
		endif
	endfor
	
	return fires;
	
endfunction	