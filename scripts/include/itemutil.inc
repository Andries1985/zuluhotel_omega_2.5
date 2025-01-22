use uo;

include "include/objtype";
include "include/attributes";
include "include/constants/settings";
include "include/constants/propids";

function ConsumeObjType( who , objtype , amount )

	var item := FindItemInContainer( who.backpack , objtype );
	if( !item )
		return 0;
	endif
	
	if( !ReserveItem( item ) )
		return 0;
	endif
	
	if( item.amount < amount )
		return 0;
	endif
	
	if( !SubtractAmount( item , amount ) )
		return 0;
	endif
	
	return 1;
	
endfunction


function CreateItemAt( where , objtype , duration := -1 , amount := 1 , color := 0 , movable := 0 )

	var parms := {};
	parms[1] := where;
	parms[2] := objtype;
	parms[3] := duration;
	parms[4] := amount;
	parms[5] := color;
	parms[6] := movable;
	
	Start_Script( ":summoning:itemappear" , parms );
	
endfunction


function CreateMagicCircleAround( who , duration := 0 , color := 0 , sfx := 0 )

	var parms := {};
	parms[1] := who;
	parms[2] := duration;
	parms[3] := color;
	parms[4] := sfx;
	
	Start_Script( ":summoning:magiccircleappear" , parms );
	
endfunction


function CreateWaterfall( who , duration := 0 , color := 0 , sfx := 0 )

	var parms := {};
	parms[1] := who;
	parms[2] := duration;
	parms[3] := color;
	parms[4] := sfx;

	Start_Script( ":summoning:waterfallappear" , parms );

endfunction

	
function DupeItem( item, newitem )

	newitem.graphic := item.graphic;
	newitem.movable	:= item.movable;
		
	if( item.name )		newitem.name 		:= item.name; 		endif
	if( item.maxhp_mod ) 	newitem.maxhp_mod 	:= item.maxhp_mod; 	endif
	if( item.ar_mod ) 	newitem.ar_mod 		:= item.ar_mod; 	endif
	if( item.dmg_mod ) 	newitem.dmg_mod 	:= item.dmg_mod; 	endif
	if( item.hitscript ) 	newitem.hitscript 	:= item.hitscript; 	endif
	if( item.quality ) 	newitem.quality 	:= item.quality; 	endif
	if( item.color ) 	newitem.color 		:= item.color; 		endif
	if( item.newbie ) 	newitem.newbie 		:= item.newbie; 	endif
	if( item.facing )	newitem.facing		:= item.facing;		endif
	if( item.hp ) 		newitem.hp 		:= item.hp; 		endif
	if( item.buyprice )	newitem.buyprice 	:= item.buyprice; 	endif
	if( item.decayat )	newitem.decayat		:= item.decayat;	endif

	foreach prop in GetObjPropertyNames(item)
		SetObjProperty( newitem, prop, GetObjProperty( item , prop ) );
	endforeach

endfunction


function DestroyTheItem( who , item )

	var cursed := GetObjProperty( item , "Cursed" );
	if( cursed )
		SetObjProperty( item , "Cursed" , 3 );
	endif
	
	if( who.backpack )
		MoveItemToContainer( item , who.backpack );
	endif
	
	DestroyItem( item );
	
endfunction


function FindItemInContainer( container , objtype )

	var the_item;	
	foreach thing in EnumerateItemsInContainer( container );
		if( thing.objtype == objtype )
			the_item := thing;
			break;
		endif
	endforeach
	
	return the_item;

endfunction


function FindRootItemInContainer( container , objtype )

	var the_item;
	
	foreach thing in EnumerateItemsInContainer( container );
		if( (thing.container == container) and (thing.objtype == objtype) )
			the_item := thing;
			break;
		endif
	endforeach
	
	return the_item;

endfunction


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

function GetPlatemailArmorGraphics()

	return { 0x1408, 0x1409, 0x140a, 0x140b, 0x140c,
		 0x140d, 0x140e, 0x140f, 0x1410, 0x1411,
		 0x1412, 0x1413, 0x1414, 0x1415, 0x1416,
		 0x1417, 0x1418, 0x1419, 0x141a, 0x1c04, 0x1c05 };

endfunction

function GetChainmailArmorGraphics()

	return { 0x13bb, 0x13bc, 0x13bd, 0x13be, 0x13bf,
		 0x13c0, 0x13c1, 0x13c2, 0x13c3, 0x13c4 };

endfunction

function GetRingmailArmorGraphics()

	return { 0x13e5, 0x13e6, 0x13e7, 0x13e8, 0x13e9,
		 0x13ea, 0x13eb, 0x13ec, 0x13ed, 0x13ee,
		 0x13ef, 0x13f0, 0x13f1, 0x13f2 };

endfunction

function GetBoneArmorGraphics()

	return { 0x144e, 0x144f, 0x1450, 0x1451, 0x1452,
		 0x1453, 0x1454, 0x1455, 0x1456, 0x1457 };

endfunction

function GetStuddedLeatherArmorGraphics()

	return { 0x13d4, 0x13d5, 0x13d6, 0x13d7, 0x13d8,
		 0x13d9, 0x13da, 0x13db, 0x13dc, 0x13dd,
		 0x13de, 0x13df, 0x13e0, 0x13e1, 0x13e2 };

endfunction

function GetLeatherArmorGraphics()

	return { 0x13c5, 0x13c6, 0x13c7, 0x13c8, 0x13c9,
		 0x13ca, 0x13cb, 0x13cc, 0x13cd, 0x13ce,
		 0x13cf, 0x13d0, 0x13d1, 0x13d2, 0x13d3,
		 0x1db9, 0x1dba, 0x1c06, 0x1c07, 0x1c0a,
		 0x1c0b, 0x1c00, 0x1c01, 0x1c08, 0x1c09,
		 0x141b, 0x141c, 0x1545, 0x1546, 0x1547,
		 0x1548, 0x1549, 0x154a, 0x154b, 0x154c };

	// 0x141b to 0x154c is masks
endfunction

function GetShieldGraphics()

	return { 0x1b72, 0x1b73, 0x1b74, 0x1b75, 0x1b76,
		 0x1b77, 0x1b78, 0x1b79, 0x1b7a, 0x1b7b, 0x982a };

endfunction

function GetShieldGraphicsThief()

	return { 0x1b72, 0x1b74, 0x1b75, 0x1b76,
		 0x1b77, 0x1b78, 0x1b79, 0x1b7a, 0x1b7b };

endfunction

function GetGraphicsBladesinger()

	return { 0x1f02, 0x9B10 };
	
	// Cloak of Lettenhove, Grarks Shepherds Crook
endfunction

function GetPossiblePropertyNames()

	return {	"AirProtection"		,
			"ArBonus"		,
			"AvailSkills"		,
			"BaseName"		,
			"BetweenUses"		,
			"bookcontent"		,
			"ByTrueMage"		,
			"CannotBeUncursed"	,
			"CastingOpts"		,
			"CastSpell"		,
			"ChanceOfEffect"	,
			"charges"		,
			"ChoppedBy"		,
			"Circle1"		,
			"Circle2"		,
			"Circle3"		,
			"Circle4"		,
			"CircleColor"		,
			"CircleDurationMs"	,
			"CircleSfx"		,
			"ColorMod"		,
			"ConfirmationMsg"	,
			"Consecrated"		,
			"CostPerCharge"		,
			"CreatureType"		,
			"Cursed"		,
			"DefaultDex"		,
			"DefaultLocation"	,
			"Delay"			,
			"DelayBetweenMs"	,
			"DestroyAfterUse"	,
			"dex"			,
			"DontAffectCaster"	,
			"EarthProtection"	,
			"EffectCircle"		,
			"EffectType"		,
			"ElementalSpell"	,
			"EnchantColor"		,
			"Enchanted"		,
			"EnchantedWith"		,
			"equipspell"		,
			"Expire"		,
			"ExpireNum"		,
			"FireProtection"	,
			"FixedDamages"		,
			"FreeAction"		,
			"Frequency"		,
			"GateDestX"		,
			"GateDestY"		,
			"GateDestZ"		,
			"GraphicMod"		,
			"Greater"		,
			"HealingIncrease"	,
			"HitscriptNumber"	,
			"HitWithSpell"		,
			"HolyProtection"	,
			"HpRegen"		,
			"HrEnd"			,
			"HrStart"		,
			"IDed"			,
			"int"			,
			"IsExceptional"		,
			"IsGMItem"		,
			"ItemAppear"		,
			"ItemAppearAmount"	,
			"ItemAppearColor"	,
			"ItemAppearDurationMs"	,
			"ItemAppearMovable"	,
			"keep"			,
			"KilledBy"		,
			"Lesser"		,
			"LootedBy"		,
			"LumberjackingBonus"	,
			"MagicImmunity"		,
			"MagicPenalty"		,
			"ManaCost"		,
			"ManaLevel"		,
			"ManaRegen"		,
			"Max"			,
			"maxcharges"		,
			"MaxStatMod"		,
			"MiningBonus"		,
			"name"			,
			"NameMod"		,
			"NecroProtection"	,
			"NoCheckLos"		,
			"noloot"		,
			"NormalRange"		,
			"OfBlackrock"		,
			"OnHitScript"		,
			"OnHitscriptNumber"	,
			"ownername"		,
			"ownerserial"		,
			"PermMagicCircle"	,
			"PermMagicImmunity"	,
			"Permmr"		,
			"PermPoisonImmunity"	,
			"PhysicalProtection"	,
			"PlanarSpell"		,
			"PlayAnimation"		,
			"Poisoned"		,
			"PoisonImmunity"	,
			"Poisonlvl"		,
			"PolymorphColor"	,
			"PolymorphDuration"	,
			"PolymorphInto"		,
			"PolymorphMod"		,
			"Powerlevel"		,
			"PowerWords"		,
			"ProtectedType"		,
			"Range"			,
			"ReagentsNeeded"	,
			"Rechargeable"		,
			"RequireNoCharges"	,
			"runedefs"		,
			"SlayType"		,
			"ShearingBonus"		,
			"skilladv"		,
			"skilladvamount"	,
			"SkillLevelMod"		,
			"SkillMultipliersMod"	,
			"SkillPoisoned"		,
			"SpecMult"		,
			"SpellCircle"		,
			"SpellDifficulty"	,
			"SpellEffectType"	,
			"SpellFx"		,
			"SpellFxLoop"		,
			"SpellFxSpeed"		,
			"SpellFxType"		,
			"SpellHealAmount"	,
			"SpellId"		,
			"SpellModAmount"	,
			"SpellModDuration"	,
			"SpellModId"		,
			"SpellPoisonLevel"	,
			"SpellRadius"		,
			"SpellReflect"		,
			"SpellSfx"		,
			"SpellTargetOption"	,
			"StamRegen"		,
			"StaticColor"		,
			"StaticName"		,
			"StaticCursedName"	,
			"StatLevelMod"		,
			"StayEquipped"		,
			"str"			,
			"SummonDelayBeforeMs"	,
			"SummonDelayBetweenMs"	,
			"SummonSfx"		,
			"SummonNpcTemplate"	,
			"TargetingType"		,
			"Template"		,
			"TriggerCircle"		,
			"Type"			,
			"WasCursed"		,
			"WaterProtection"	,
			"WordOfPower"		,
			"x"			,
			"y"			,
			"z"			};
			
endfunction

function IsCarpet( item )

	if( item.objtype >= UOBJ_CARPET_START and item.objtype <= UOBJ_CARPET_END )
		return 1;
	else
		return 0;
	endif

endfunction

function IsCloth( item )

	if( item.objtype >= UOBJ_CLOTH_START and item.objtype <= UOBJ_CLOTH_END )
		return 1;
	else
		return 0;
	endif

endfunction

function IsConsommable( item )

	var objtype := item.objtype;
	
	if( objtype == 0x099b or objtype== 0x099f or objtype == 0x09c7 or objtype == 0x09c8 )
		return 1;
	elseif( objtype >= 0x1f7d and objtype <= 0x1f9e )
		return 1;
	elseif( objtype >= 0x09ee and objtype <= 0x09f0 )
		return 1;
	elseif( objtype >= 0x0c7f and objtype <= 0x0c82 )
		return 1;
	elseif( objtype >= 0x0c77 and objtype <= 0x0c7c )
		return 1;
	elseif( objtype >= 0x171d and objtype <= 0x172d )
		return 1;
	elseif( objtype >= 0x0c64 and objtype <= 0x0c67 )
		return 1;
	elseif( objtype >= 0x0c70 and objtype <= 0x0c75 )
		return 1;
	elseif( objtype >= 0x097b and objtype <= 0x097e )
		return 1;
	elseif( objtype >= 0xc900 and objtype <= 0xc903 )
		return 1;
	elseif( objtype >= 0xc910 and objtype <= 0xc912 )
		return 1;
	elseif( objtype >= 0xc920 and objtype <= 0xc923 )
		return 1;
	elseif( objtype >= 0xc930 and objtype <= 0xc936 )
		return 1;
	elseif( objtype >= 0xc940 and objtype <= 0xc947 )
		return 1;
	elseif( objtype == 0x0978 or objtype == 0x0979 )
		return 1;
	elseif( objtype == 0x09b7 or objtype == 0x09b8 )
		return 1;
	elseif( objtype ==0x09d0 or objtype == 0x0d1a or objtype == 0x0c5c )
		return 1;
	elseif( objtype == 0x0c6d or objtype == 0x0d3a or objtype == 0x09e9 )
		return 1;
	elseif( objtype == 0x09f2 or objtype == 0x09b6 or objtype == 0x09ea )
		return 1;
	elseif( objtype == 0x103b or objtype == 0x098c or objtype == 0x160c )
		return 1;
	elseif( objtype == 0x1e1f or objtype == 0x09c1 or objtype == 0x1608 )
		return 1;
	elseif( objtype ==0x160a or objtype == 0x1e1d or objtype == 0x1e1e )
		return 1;
	elseif( objtype == 0xc950 or objtype == 0x7059 )
		return 1;
	elseif( objtype >= 0xdc01 and objtype <= 0xdc03 )
		return 1;
	elseif( objtype >= 0xdc0b and objtype <= 0xdc16 )
		return 1;
	else
		return 0;
	endif
	
endfunction

function IsEquipped( who, what )

	var hand1	:= GetEquipmentByLayer( who , 0x01 );
	var hand2	:= GetEquipmentByLayer( who , 0x02 );
	
	if( hand1 == what or hand2 == what )
		return 1;
	elseif( !Accessible( who, what ) )
		return 0;
	else
		return EquipItem( who , what );
	endif

endfunction

function IsFish( theobj )

	if( (theobj >= UOBJ_FISH_START) and (theobj <= UOBJ_FISH_END) )
		return 1;
	elseif( (theobj >= UOBJ_SFISH_START) and (theobj <= UOBJ_SFISH_END) )
		return 1;
	else
		return 0;
	endif
		
endfunction

function IsIDable( item )

	if( item.graphic >= 0x182a and item.graphic <= 0x1848 )
		return 0;
	elseif( item.graphic >= 0x0efb and item.graphic <= 0x0f0e )
		return 0;
	elseif( item.graphic >= 0x100e and item.graphic <= 0x1013 )
		return 0;
	elseif( IsStackable( item, item ) )
		return 0;
	else
		return 1;
	endif

endfunction

function IsInContainer( container , item )

   	foreach thing in EnumerateItemsInContainer( container, ENUMERATE_ROOT_ONLY )
		if( thing.serial == item.serial )
			return 1;
		endif
   	endforeach

   	return 0;

endfunction

function IsIngot( item )

	if(( item.objtype >= UOBJ_INGOTS_START  and item.objtype <= UOBJ_INGOTS_END  ) or
	   ( item.objtype >= UOBJ_INGOTS2_START and item.objtype <= UOBJ_INGOTS2_END ) or
	   ( item.objtype  == UOBJ_GOLD_INGOT ))
		return 1;
	else
		return 0;
	endif

endfunction

function IsLog( item )

	if(( item.objtype >= UOBJ_LOGS_START  and item.objtype <= UOBJ_LOGS_END ) or ( item.objtype == UOBJ_LOGS_GODDESS ) or ( item.objtype  == UOBJ_LOGS ))
		return 1;
	else
		return 0;
	endif

endfunction

function IsMinable( theobj , othertype )

	if( (othertype >= 0x053b) and (othertype <= 0x0553) and (othertype != 0x0550 ) )
	
		return 1;
		
	elseif( ((theobj >= 0xe2)  and (theobj <= 0xe7))  or
	    ((theobj >= 0xf3)  and (theobj <= 0xf7))  or
	    ((theobj >= 0x104) and (theobj <= 0x107)) or
	    ((theobj >= 0x110) and (theobj <= 0x113)) or
	    ((theobj >= 0x122) and (theobj <= 0x125)) or
	    ((theobj >= 0x1d3) and (theobj <= 0x1da)) or
	    ((theobj >= 0x21f) and (theobj <= 0x230)) or
	    ((theobj >= 0x232) and (theobj <= 0x23B)) or
	    ((theobj >= 0x6cd) and (theobj <= 0x6d1)) or
	    ((theobj >= 0x6da) and (theobj <= 0x6dd)) or
	    ((theobj >= 0x6eb) and (theobj <= 0x6fe)) or
	    ((theobj >= 0x70d) and (theobj <= 0x714)) or
	    ((theobj >= 0x71d) and (theobj <= 0x720)) or
	    ((theobj >= 0x72b) and (theobj <= 0x732)) or
	    ((theobj >= 0x73b) and (theobj <= 0x73e)) or
	    ((theobj >= 0x749) and (theobj <= 0x750)) or
	    ((theobj >= 0x759) and (theobj <= 0x75c)) or
	    ((theobj >= 0x245) and (theobj <= 0x249)) )
	
		return 1;
	else
		return 0;
	endif
	
endfunction

function IsOre( item )

	if( item.objtype >= UOBJ_ORE_START  and item.objtype <= UOBJ_ORE_END )
		return 1;
	elseif( item.objtype >= UOBJ_ORE2_START and item.objtype <= UOBJ_ORE2_END ) 
		return 1;
	elseif( item.objtype >= UOBJ_ORE3_START and item.objtype <= UOBJ_ORE3_END ) 
		return 1;
	else
		return 0;
	endif

endfunction

function IsHide( objtype )

	if( objtype == UOBJ_HIDE1 or objtype == UOBJ_HIDE2 or objtype == 0x702f or objtype == 0xdc13 or objtype == 0xdc0a or objtype == 0x7030 or objtype == 0xff17 or objtype == 0xff18 )

		return 1;
	elseif( objtype >= UOBJ_HIDES_START and objtype <= UOBJ_HIDES_END )
		return 1;
	else
		return 0;
	endif

endfunction

function IsReagent( item )

	if( item.objtype >= UOBJ_REAGENT_START and item.objtype <= UOBJ_REAGENT_END )
		return 1;
	else
		return 0;
	endif
	
endfunction

function IsSand( theobj )

	if( ((theobj >= 0x0016 ) and (theobj <= 0x004b )) or
	    ((theobj >= 0x011e ) and (theobj <= 0x012d )) or
	    ((theobj >= 0x0192 ) and (theobj <= 0x01ab )) or
	    ((theobj >= 0x01b9 ) and (theobj <= 0x01d1 )) or
	    ((theobj >= 0x0282 ) and (theobj <= 0x0291 )) or
	    ((theobj >= 0x03b7 ) and (theobj <= 0x03ca )) or
	    ((theobj >= 0x064b ) and (theobj <= 0x0672 )) )
		return 1;
	else
		return 0;
	endif

endfunction

function IsSwamp( theobjtype )

	if( (theobjtype >= 0x053b) and (theobjtype <= 0x0553) and (theobjtype != 0x0550 ) )
		return 0;
	endif
	if( ((theobjtype >= 0x240 ) and (theobjtype <= 0x244 )) or
	    (theobjtype == 0x250) or
	    ((theobjtype >= 0x3d65) and (theobjtype <= 0x3ef0)) )
		return 1;
	else
		return 0;
	endif
	
endfunction

function IsTree( theobj )

	if((theobj == 0x0c9e) or (theobj == 0x0ca8) or (theobj == 0x0caa) or (theobj == 0x0cab))
		return 1;
	elseif((theobj >= 0x0c99) and (theobj <= 0x0cea))
		return 1;
	elseif((theobj >= 0x0cf3) and (theobj <= 0x0d03))
		return 1;
	elseif((theobj >= 0x0d41) and (theobj <= 0x0dab))
		return 1;
	elseif((theobj >= 0x0d37) and (theobj <= 0x0d38))
		return 1;
	else
		return 0;
	endif
		
endfunction

function IsWater( theobj )

	if( theobj.item )
		if( (theobj.objtype >=0x346E) and (theobj.objtype <= 0x3485) )
			return 1;
		elseif( (theobj.objtype >= 0x3494) and (theobj.objtype <= 0x34AB) )
			return 1;
		elseif( (theobj.objtype >= 0x34B8) and (theobj.objtype <= 0x34CA) )
			return 1;
		elseif( (theobj.objtype >= 0x1796) and (theobj.objtype <= 0x17B2) )
			return 1;
		endif
	endif
	
	var info := GetMapInfo( cint(theobj.x) , cint(theobj.y), theobj.realm );
	var tile := info.landtile;

	if( tile >= 0x04c and tile <= 0x06f )
		return 1;
	elseif( tile >= 0x0a8 and tile <= 0x0ab )
		return 1;
	elseif( tile == 0x0136 or tile == 0x0137 )
		return 1;
	elseif( tile >= 0x3ff0 and tile <= 0x3ff3 )
    		return 1;
    	endif
    	
	return 0;

endfunction

function PlayMagicCircleEffect( who ) 

	var parms := {};
	parms[1] := who;
	parms[2] := 1500;
	parms[3] := 0;
	parms[4] := 0;
	
	Start_Script( ":summoning:magiccircleappear" , parms );
	
endfunction

function PlayWaterfallEffect( who )

	var parms := {};
	parms[1] := who;
	parms[2] := 5000;
	parms[3] := 0;
	parms[4] := 0x218;
	
	Start_Script( ":summoning:waterfallappear" , parms );
	
endfunction

function ResetAllHitscriptPropsExcep( item , excep := "" )

	var cprops := {	"ChanceOfEffect",
				"EffectCircle"	,
				"HitWithSpell"	,
				"Poisonlvl"	,
				"Powerlevel"	,
				"ProtectedType"	,
				"SlayType"	};
				
	foreach cprop in cprops
		if( cprop != excep )
			EraseObjProperty( item , cprop );
		endif
	endforeach
	
endfunction

function GetCurseLevel( byref item )

	return Cint(GetObjProperty( item, PROPID_ITEM_CURSED ));
	
endfunction

function SetCurseLevel( byref item, byref level )

	level := Cint(level);
	if( level != SETTING_CURSE_LEVEL_NOT_CURSED )
		return SetObjProperty( item, PROPID_ITEM_CURSED, level );
	else
		return EraseObjProperty( item, PROPID_ITEM_CURSED );
	endif
	
endfunction

function IsOnPlayer( container , item )

   	foreach thing in EnumerateItemsInContainer( container )
		if( thing.serial == item.serial )
			return 1;
		endif
   	endforeach

   	return 0;

endfunction

function ReserveContainers( who, container )

	while(container != who.backpack)	
		if(!ReserveItem( container ))
   			return 0;
		endif
		container := container.container;
	endwhile
	return 1;

endfunction

function IsMetalEquipment(item)

	var metalstuff := {0x0f45, 0x0f47, 0x0f49, 0x0f4b, 0x0f4e, 0x0f51,
	0x0f5c, 0x0f5e, 0x0f60, 0x0f63, 0x13af, 0x13b5, 0x13b9, 0x13bb, 0x13bf,
	0x13c3, 0x13ec, 0x13ee, 0x13f0, 0x13f2, 0x13fa, 0x13fe, 0x1401, 0x1402,
	0x1404, 0x1406, 0x1408, 0x140a, 0x140c, 0x140e, 0x1410, 0x1411, 0x1412,
	0x1413, 0x1414, 0x1415, 0x1438, 0x143a, 0x143c, 0x143e, 0x1440, 0x1442,
	0x144E, 0x144F, 0x1450, 0x1451, 0x1452, 0x151A, 0x156A, 0x1b72,	0x1b73,
	0x1b74, 0x1b76, 0x1b7b, 0x1c04, 0x1f0b, 0x27AB, 0x982a};

	if (item.graphic in metalstuff)
		return 1;
	else
		return 0;
	endif

endfunction

function IsWoodEquipment(item)

	var woodstuff := {0x0DF0, 0x0E81, 0x0E8A, 0x13b3, 0x13F9, 0x1B78, 0x1B79,
	0x1B7A, 0x0f4f, 0x13fd, 0x1403, 0x6050, 0xb201, 0x0f4f, 0x13B2, 0x13fd,
	0x1403, 0x6050, 0xb201};

	if (item.graphic in woodstuff)
		return 1;
	else
		return 0;
	endif

endfunction

function IsLeatherEquipment(item)

	var leatherstuff := {0x13C5, 0x13C6, 0x13C7, 0x13CB, 0x13CC, 0x13D4, 0x13D5,
	0x13D6, 0x13DA, 0x13DB, 0x1C00, 0x1C02, 0x1C06, 0x1C08, 0x1C0A, 0x1C0C, 0x1DB9};

	if (item.graphic in leatherstuff)
		return 1;
	else
		return 0;
	endif

endfunction

function IsSign (item)
	case (item.objtype)
		0xb95:
		0xb96:
		0xbd0:
		0xbd2:
		0xc43:
		0xc44:
		0x1f28:
		0x1f29:
		0x17060:
		0x1708a:
			return 1;
	endcase
	if (item.objtype >= 0xba3 and item.objtype <= 0xc0e)
		return 1;
	elseif (item.objtype >= 0x1297 and item.objtype <= 0x129e)
		return 1;
	endif
	return 0;
endfunction

function IsJewel(item)

	case (item.graphic)
		0x1085:
		0x1086:
		0x1087:
		0x1088:
		0x1089:
		0x108A:
		0x5015:
			return 1;
	endcase

	return 0;
endfunction

function IsCBoost(item)
	if ((item.objtype >= 0x8B01) && (item.objtype <= 0x8B04))
		return 1;
	endif
	return 0;
endfunction