// Shilitems Include 

//Word of Power Gate Function

use os;
use uo;

include "include/attributes";

function FitRequirements( who , item )

	var required := GetObjProperty( item , "ToUseRequirements" );
	if( !required )
		return 1;
	endif
	
	foreach condition in required
		var type := condition[1];
		case( type )
			"skill":	if( GetEffectiveSkill( who , Cint(condition[2]) ) < Cint( condition[3] ) )
						return 0;
					endif
					break;
					
			"stat":		case( condition[2] )
						"str":	if( GetStrength(who) < Cint( condition[3] ) )
								return 0;
							endif
							break;
							
						"dex":	if(GetDexterity(who) < Cint( condition[3] ) )
								return 0;
							endif
							break;
							
						"int":	if( GetIntelligence(who) < Cint( condition[3] ) )
								return 0;
							endif
							break;
					endcase
					break;
					
			"spec":		if( Cint( GetObjProperty( who , condition[2] ) ) < Cint( condition[3] ) )
						return 0;
					endif
					break;
					
			"owner":	if( !( who.serial in condition[2] ) )
						return 0;
					endif
					break;
		endcase
	endforeach
	
	return 1;
	
endfunction