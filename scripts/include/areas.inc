use uo;
use os;
use basic;
use cfgfile;

//include "include/multiutil";
include ":areas:include/areapolicy";
include "include/constants/cfgfiles";
include "include/constants/propids";

const AREAID_BRITAIN		:= "Britain";
const AREAID_BRITISH_CASTLE	:= "British Castle";
const AREAID_BLACKTHORN_CASTLE	:= "Blackthorn Castle";
const AREAID_BUCCANEERS_DEN	:= "Buccaneer's Den";
const AREAID_COVE		:= "Cove";
const AREAID_DELUCIA		:= "Delucia";
const AREAID_JHELOM		:= "Jhelom";
const AREAID_MAGINCIA		:= "Magincia";
const AREAID_MINOC		:= "Minoc";
const AREAID_MOONGLOW		:= "Moonglow";
const AREAID_NUJELM		:= "Nujel'm";
const AREAID_OCLLO		:= "Ocllo";
const AREAID_PAPUA		:= "Papua";
const AREAID_SERPENTS_HOLD	:= "Serpent's Hold";
const AREAID_SKARA_BRAE		:= "Skara Brae";
const AREAID_TRINSIC		:= "Trinsic";
const AREAID_YEW		:= "Yew";

function GetAreaPolicyMaskForWho(byref who)
	return CInt(ResolvePolicyAtLocation(who.x, who.y, who.realm));
endfunction

function GetAreaPolicyMaskAtLocation(byref x, byref y, byref realm)
	return CInt(ResolvePolicyAtLocation(x, y, realm));
endfunction



function CheckPoz( byref x, byref y )

	// Arena
	if( x >= 1385 )
		if( x <= 1414 )
			if( y >= 3729 )
				if( y <= 3758 )
					return 0;
				endif
			endif
		endif
		
		// Wind
		if( x >= 5120 )
			if( x <= 5374 )
				if( y >= 0 )
					if( y <= 254 )
						return 0;
					endif
				endif
			endif
			
			// Jail
			if( x >= 5272 )
				if( x <= 5310 )
					if( y >= 1160 )
						if( y <= 1190 )
							return 0;
						endif
					endif
				endif
				
				// Newbie Dungeon
				if( x >= 5487 )
					if( x <= 5488 )
						if( y >= 344 )
							if( y <= 345 )
								return 0;
							endif
						endif
					endif
					
					// Race Gates
					if( x >= 5536 )
						if( x <= 5537 )
							if( y >= 1154 )
								if( y <= 1155 )
									return 0;
								endif
							endif
						endif
					endif
				endif
			endif
		endif
	endif

	return 1;
	
endfunction


function IsForbiddenAreaFor( who, x, y )

	if( who.cmdlevel )
		return 0;
	endif
	
	var old_prio := set_priority(100);
	var match := ResolveAreaMatchAtLocation(x, y, who.realm);
	if(!match)
		set_priority(old_prio);
		return 0;
	endif

	var banned := GetObjProperty(who, PROPID_MOBILE_AREAS_BANNED);
	if(!banned)
		banned := {};
	endif

	var allowed := GetObjProperty(who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN);
	if(!allowed)
		allowed := {};
	endif

	if(banned[match.area_index])
		set_priority(old_prio);
		return 1;
	endif

	if(!allowed[match.area_index] && HasPolicy(match.mask, AREA_POLICY_FORBIDDEN))
		set_priority(old_prio);
		return 1;
	endif

	set_priority(old_prio);
	return 0;

endfunction


function IsForbiddenAreaToGateFrom( byref who )

	return IsForbiddenAreaToRecallFrom( who );

endfunction


function IsForbiddenAreaToGateTo( byref who, byref x, byref y )

	return IsForbiddenAreaToRecallTo( who, x, y );

endfunction


function IsForbiddenAreaToRecallFrom( byref who )
	return HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_NO_RECALL_FROM);
	/*
	if( who.cmdlevel )
		return 0;
	endif
	var old_prio := set_priority(100);	
	if( !CheckPoz( who.x, who.y ) )
		set_priority(old_prio);
		return 1;
	endif
	
	set_priority(old_prio);
	return 0;*/
	
endfunction


function IsForbiddenAreaToRecallTo( byref who, byref x, byref y )
	
	if( who.cmdlevel )
		return 0;
	endif

	var old_prio := set_priority(100);

	if( !CheckPoz( x, y ) )
		set_priority(old_prio);
		return 1;
	endif
/*
	var multi := GetMultiAtLocation( x, y );
	if( multi )
		if( !IsWelcomeInHouse( who, multi ) )
			set_priority(old_prio);
			return 1;
		endif
	endif
*/
	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		banned := {};
	endif

	var match := ResolveAreaMatchAtLocation(x, y, who.realm);
	if(!match)
		set_priority(old_prio);
		return 0;
	endif
	
	var allowed_recall := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_RECALLING );
	if( !allowed_recall )
		allowed_recall := {};
	endif	
	
	var allowed_forbidden := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN );
	if( !allowed_forbidden )
		allowed_forbidden := {};
	endif

	if( banned[match.area_index] )
		set_priority(old_prio);
		return 1;
	endif

	if( !allowed_recall[match.area_index] && HasPolicy(match.mask, AREA_POLICY_NO_RECALL_TO) )
		set_priority(old_prio);
		return 1;
	endif

	if( !allowed_forbidden[match.area_index] && HasPolicy(match.mask, AREA_POLICY_FORBIDDEN) )
		set_priority(old_prio);
		return 1;
	endif
	
	set_priority(old_prio);
	return 0;

endfunction


function IsInACity( byref what )
			// 		min_x	max_x	min_y	max_y	name
	var cities := {		{	1361,	1589,	1536,	1765,	AREAID_BRITAIN		},	// Britain
				{	1295,	1360,	1558,	1765,	AREAID_BRITISH_CASTLE	},	// British Castle
				{	1500,	1545,	1400,	1490,	AREAID_BLACKTHORN_CASTLE},	// Blackthorn Castle
				{	2592,	2783,	2080,	2271,	AREAID_BUCCANEERS_DEN	},	// Buccanner's Den
				{	2207,	2292,	1114,	1241,	AREAID_COVE		},	// Cove
				{	5151,	5312,	3930,	4059,	AREAID_DELUCIA		},	// Delucia
				{	1020,	1531,	3340,	4055,	AREAID_JHELOM		},	// Jhelom
				{	3644,	3839,	2040,	2283,	AREAID_MAGINCIA		},	// Magincia
				{	2385,	2635,	376,	626,	AREAID_MINOC		},	// Minoc
				{	4368,	4495,	1020,	1235,	AREAID_MOONGLOW		},	// Moonglow
				{	3484,	3819,	1064,	1399,	AREAID_NUJELM		},	// Nujel'm
				{	3592,	3763,	2440,	2699,	AREAID_OCLLO		},	// Ocllo
				{	5652,	5840,	3116,	3309,	AREAID_PAPUA		},	// Papua
				{	2865,	3069,	3334,	3582,	AREAID_SERPENTS_HOLD	},	// Serpent's Hold
				{	532,	695,	2108,	2271,	AREAID_SKARA_BRAE	},	// Skara Brae
				{	1822,	2145,	2654,	2899,	AREAID_TRINSIC		},	// Rennance
				{	494,	600,	927,	1033,	AREAID_YEW		}};	// Yew
		
	foreach city in cities
		if( IsInBox( what, city[1], city[2], city[3], city[4] ) )
			return city[5];
		endif
	endforeach
	
	return 0;
	
endfunction


function IsInAntiLootingArea( byref who )

	if( who.cmdlevel )
		return 0;
	endif

	var old_prio := set_priority(100);
	var match := ResolveAreaMatchAtLocation(who.x, who.y, who.realm);
	if(!match)
		set_priority(old_prio);
		return 0;
	endif

	var banned := GetObjProperty( who, PROPID_MOBILE_AREAS_BANNED );
	if( !banned )
		banned := {};
	endif

	var allowed := GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_LOOTING );
	if( !allowed )
		allowed := {};
	endif
	
	if( banned[match.area_index] )
		set_priority(old_prio);
		return 1;
	endif

	if( !allowed[match.area_index] && (HasPolicy(match.mask, AREA_POLICY_NO_LOOTING) || HasPolicy(match.mask, AREA_POLICY_NO_DAMAGE_ZONE)) )
		set_priority(old_prio);
		return 1;
	endif

	set_priority(old_prio);

	return 0;

endfunction


function IsInAntiMagicArea( byref who )

	if( who.cmdlevel )
		return 0;
	endif

	if(IsInBox(who, 5508, 5527, 1655, 1674))
		return 0;
	endif

	var old_prio := set_priority(100);
	var match := ResolveAreaMatchAtLocation(who.x, who.y, who.realm);
	if(!match)
		set_priority(old_prio);
		return 0;
	endif

	var allowed	:= GetObjProperty( who, PROPID_MOBILE_AREAS_ALLOWED_ANTIMAGIC );
	if( !allowed )
		allowed := {};
	endif

	if( !allowed[match.area_index] && (HasPolicy(match.mask, AREA_POLICY_ANTI_MAGIC) || HasPolicy(match.mask, AREA_POLICY_NO_DAMAGE_ZONE)) )
		set_priority(old_prio);
		return 1;
	endif
	
	set_priority(old_prio);

	return 0;

endfunction


function IsInAProtectedCity( byref what )
			// 		min_x	max_x	min_y	max_y	name
	var p_cities := {	{	1361,	1589,	1536,	1765,	AREAID_BRITAIN		},	// Britain
				{	1295,	1360,	1558,	1765,	AREAID_BRITISH_CASTLE	},	// British Castle
				{	1500,	1545,	1400,	1490,	AREAID_BLACKTHORN_CASTLE},	// Blackthorn Castle
				{	2207,	2292,	1114,	1241,	AREAID_COVE		},	// Cove
				{	1292,	1508,	3667,	3883,	AREAID_JHELOM		},	// Jhelom
				{	3647,	3837,	2041,	2280,	AREAID_MAGINCIA		},	// Magincia
				{	2385,	2635,	376,	626,	AREAID_MINOC		},	// Minoc
				{	4371,	4493,	1023,	1232,	AREAID_MOONGLOW		},	// Moonglow
				{	3487,	3817,	1067,	1397,	AREAID_NUJELM		},	// Nujel'm
				{	3591,	3761,	2441,	2696,	AREAID_OCLLO		},	// Ocllo
				{	534,	692,	2111,	2269,	AREAID_SKARA_BRAE	},	// Skara Brae
				{	288,	480,	1408,	1504,	AREAID_TRINSIC		},	// Rennance
				{	494,	600,	927,	1033,	AREAID_YEW		}};	// Yew
		
	foreach protected_city in p_cities
		if( IsInBox( what, protected_city[1], protected_city[2], protected_city[3], protected_city[4] ) )
			return protected_city[5];
		endif
	endforeach
	
	return 0;
	
endfunction


function IsInArea( byref what, byref area )

	case( area )				//		      x1    x2    y1    y2
		AREAID_BRITAIN:			return IsInBox( what, 1361, 1589, 1536, 1765 );
		AREAID_BRITISH_CASTLE:		return IsInBox( what, 1295, 1360, 1558, 1765 );
		AREAID_BLACKTHORN_CASTLE:	return IsInBox( what, 1500, 1545, 1400, 1490 );
		AREAID_BUCCANEERS_DEN:		return IsInBox( what, 2592, 2783, 2080, 2271 );
		AREAID_COVE:			return IsInBox( what, 2207, 2292, 1114, 1241 );
		AREAID_DELUCIA:			return IsInBox( what, 5151, 5312, 3930, 4059 );
		AREAID_JHELOM:			return IsInBox( what, 1292, 1508, 3667, 3883 );
		AREAID_MAGINCIA:		return IsInBox( what, 3647, 3837, 2041, 2280 );
		AREAID_MINOC:			return IsInBox( what, 2385, 2635, 376 , 626  );
		AREAID_MOONGLOW:		return IsInBox( what, 4371, 4493, 1023, 1232 );
		AREAID_NUJELM:			return IsInBox( what, 3487, 3817, 1067, 1397 );
		AREAID_OCLLO:			return IsInBox( what, 3591, 3761, 2441, 2696 );
		AREAID_PAPUA:			return IsInBox( what, 5652, 5840, 3116, 3309 );
		AREAID_SERPENTS_HOLD:		return IsInBox( what, 2865, 3069, 3334, 3582 );
		AREAID_SKARA_BRAE:		return IsInBox( what, 534 , 692 , 2111, 2269 );
		AREAID_TRINSIC:			return IsInBox( what, 1791, 2160, 2639, 2905 );
		AREAID_YEW:			return IsInBox( what, 494 , 600 , 927 , 1033 );
	endcase
	
	return 0;

endfunction


function IsInBox( byref object, byref x1, byref x2, byref y1, byref y2 )

	if( object.x >= x1 )
		if( object.x <= x2 )
			if( object.y >= y1 )
				if( object.y <= y2 )
					return 1;
				endif
			endif
		endif
	endif
	
	return 0;

endfunction


function IsInForbiddenArea( who )

	if( who.cmdlevel )
		return 0;
	endif
	
	var old_prio := set_priority(100);
	var match := ResolveAreaMatchAtLocation(who.x, who.y, who.realm);
	if(!match)
		set_priority(old_prio);
		return 0;
	endif

	var banned := GetObjProperty(who, PROPID_MOBILE_AREAS_BANNED);
	if(!banned)
		banned := {};
	endif

	var allowed := GetObjProperty(who, PROPID_MOBILE_AREAS_ALLOWED_FORBIDDEN);
	if(!allowed)
		allowed := {};
	endif

	if(banned[match.area_index])
		set_priority(old_prio);
		return 1;
	endif

	if(!allowed[match.area_index] && HasPolicy(match.mask, AREA_POLICY_FORBIDDEN))
		set_priority(old_prio);
		return 1;
	endif

	set_priority(old_prio);
	return 0;

endfunction


function IsInGuardedArea( byref who )

	var old_prio := set_priority(100);
	var guarded := HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_GUARDED);
	set_priority(old_prio);
	return guarded;

endfunction

function IsForbiddenAreaToMark( byref who )

	if( who.cmdlevel > 2 )
		return 0;
	endif

	var old_prio := set_priority(100);
	var disabled_marking := HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_NO_MARKING);
	set_priority(old_prio);
	return disabled_marking;

endfunction

function IsInSafeArea(who)
	return HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_SAFE_AREA);
endfunction

function IsInNOPKArea(who)
	return HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_NO_PK);
endfunction

function IsInRPArea(who)
	return HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_RP_AREA);
endfunction

function IsInNoDamageZone(who)
	return HasPolicy(GetAreaPolicyMaskForWho(who), AREA_POLICY_NO_DAMAGE_ZONE);
endfunction