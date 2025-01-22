use uo;
use os;
use util;
use basic;
use polsys;

include "include/dismount";
include "include/client";
include "include/spelldata";
include "include/classes";
include "include/random";


function AddToPersistedMods( byref allmods , byref mmod )

	set_critical(1);
	var i;
	for( i := 1 ; i <= len(allmods) ; i := i + 1 )
		if ( allmods[i][1] == mmod[1] )
			return 0;
		endif
	endfor

	allmods.append( mmod );
	set_critical(0);
	
	return i;

endfunction


function AddToPoisonMod( byref currentmod , byref mmod )

	set_critical(1);
	if( mmod[1] > currentmod[1] )
		currentmod[1] := mmod[1];
	endif
	
	if( mmod[2] > currentmod[2] )
		currentmod[2] := mmod[2];
	endif
	set_critical(0);

endfunction


function AddToStatMods( byref allmods , byref mmod )

	set_critical(1);
	var i;
	for( i := 1 ; i <= len(allmods) ; i := i + 1 )
		if ( allmods[i][1] == mmod[1] )
			return 0;
		endif
	endfor

	allmods.append( mmod );
	set_critical(0);
	
	return i;

endfunction


function BaseRegenRate( who )

	var baseregen := GetObjProperty(who,"baseregen");
	
	if (!baseregen)
		return 100;
	endif
	
	return baseregen;

endfunction


function CanMod( who , stat , persistedprop := 0 )

	var modarray := "#mods";
	if( persistedprop )
		modarray := "mods";
	endif

	var currentmods := GetObjProperty( who , modarray );
	if (!currentmods)
		return 1;
	endif
	
	foreach mmod in currentmods
		if ( mmod[1] == stat )
			return 0;
		endif
	endforeach
		
	return 1;

endfunction


function CurePoison( who , sound := 0 , msg := 0)

	Set_Critical( 1 );
	
	var currentmod := GetObjProperty( who , "poison" );
	if( !currentmod && !who.poisoned)
		Set_Critical( 0 );
		return;
	endif

	EraseObjProperty(who, "poison_level");
	EraseObjProperty( who , "poison" );
	SetHpRegenRate(who , ( BaseRegenRate(who)) * 12);
	who.SetPoisoned( 0 );
	IncRevision(who);
	Set_Critical( 0 );
	
	if( sound )
		PlaySoundEffectPrivate( who , sound , who );
	endif
	
	if( msg )
		PrintTextAbovePrivate( who , msg , who );
	endif

endfunction


//for name, graphic and color changes!
function DoPersistedMod( who , prop , newvalue , oldvalue , duration )

	var parms := {};
	parms[1] := who;
	parms[2] := prop;
	parms[3] := newvalue;
	parms[4] := oldvalue;
	parms[5] := duration;
	
	Start_Script( ":summoning:processpersistedmod" , parms );
	Sleepms(250);

endfunction


//for nonpersistedmods
function DoTempMod( who , stat , amt , duration , liche := 0)

	var parms := {};
	parms[1] := who;
	parms[2] := stat;
	parms[3] := CInt(amt*0.8);
	parms[4] := duration;
	if(liche)
		parms[5] := liche;
	endif
	
	Start_Script( ":summoning:processtempmod" , parms );
	Sleepms(250);

endfunction


function GetModAmount( who ) 

	var magery := GetEffectiveSkill( who , SKILLID_MAGERY );
	var amount := RandomInt(5) + Cint(magery/7);
	amount := ModifyWithMagicEfficiency( who, amount );
	if( !amount )
		amount := 1;
	endif

	return Cint(amount);

endfunction


function GetModDuration( who )

	var magery := GetEffectiveSkill( who , SKILLID_MAGERY );
	var duration := magery * 5;
	duration := ModifyWithMagicEfficiency( who , duration );
	if( !duration )
		duration := 1;
	endif
	duration := (duration * 2);
	return Cint(duration);

endfunction


function GetPoisonRegenMod( plvl )

	case (plvl)
		0: return 100;
		1: return -500;
		2: return -750;
		3: return -2500;
		4: return -5000;
		5: return -5500;
		6: return -6000;
	endcase

/*	
		0: return 0;
		1: return -500;
		2: return -1000;
		3: return -2000;
		4: return -4000;
		5: return -8000;
		6: return -16000;
*/

endfunction


function PoisonLevel( who )

	var currentmod := GetObjProperty( who , "poison" );
	if( !currentmod )
		currentmod := {};
	endif
	
	if( !len(currentmod) )
		return 0;
	else
		return Cint( currentmod[1] );
	endif

endfunction


function PoisonProtected( cast_on , plvl, thiefPoison := 0 )

	if(thiefPoison)
		return 0;
	endif

	//var prot := "PoisonImmunity";
	var protserial := GetObjProperty( cast_on , "PoisonImmunity" );
	var protlvl := GetObjProperty( cast_on , "PermPoisonImmunity" );
	if( !protlvl )
		protlvl := 0;
	endif
	
	var cmi := 0 - protlvl;
	var item;
	var charges;
	var cursed;
	
	if( protserial )
		foreach thing in ListEquippedItems( cast_on )
			if ( thing.serial == protserial )
				item := thing;
				charges := GetObjProperty( item , "PoisonImmunity" );
				cursed := GetObjProperty( item , "Cursed" );
				if (!charges)
					EraseObjProperty( cast_on , "PoisonImmunity" );
					EraseObjProperty( item, "PoisonImmunity" );
					PrintTextAbovePrivate(item , item.desc + " is out of charges!", cast_on );
					CheckForAnotherProtItem( cast_on , item , "PoisonImmunity" );
					IncRevision(item);
				endif
				break;
			endif
		endforeach
	endif

	if( charges >= plvl ) 
		charges := charges - plvl;
		setobjproperty( item , "PoisonImmunity" , charges );
		IncRevision(item);
		if( !cursed )
			PrintTextAbovePrivate( cast_on , "Your " + item.desc + " protected you from the poison!" , cast_on );
			return IMMUNED;
		else
			return CURSED;
		endif
		
	elseif( cmi >= plvl )
		return CURSED;
		
	elseif( protlvl >= plvl )
		PrintTextAbovePrivate( cast_on , "Your poison immunity protect you from the poison" , cast_on );
		return IMMUNED;
	endif
		
	return 0;

	
endfunction


function RandomHairColor()

	return Random(46)+1102;

endfunction


function SetPoison( who , plvl , msg := 0, caster := 0, shuriken := 0)
	
	if( plvl < 1 ) 
		plvl := 1;
	endif

	if(caster)
		SetObjProperty(who, "PoisonedBy", caster.name);
	endif

	var parms := {};
	parms[1] := who;
	parms[2] := plvl;
	parms[3] := Cstr( msg );
	parms[4] := caster;
	parms[5] := shuriken;
	
	Start_script( ":summoning:processpoisonmod" , parms );
	Sleepms(250);

endfunction


function UndoTheMod( who , prop , amt , liche := 0 )

	var 	msg,
		sound;
		
	var 	oldvalue := amt;

	case (prop)
		"cstr":
		"str" :		SetStrengthMod(who, GetStrengthMod(who) - amt); 
					if(amt > 0)
						BuffOff(who, "strength");
					else
						BuffOff(who, "weaken");
					endif
		"cdex":
		"dex" :		SetDexterityMod(who, GetDexterityMod(who)-amt);
					if(amt > 0)
						BuffOff(who, "agility");
					else
						BuffOff(who, "clumsy");
					endif
		
		"cint":
		"int" :		SetIntelligenceMod(who, GetIntelligenceMod(who)-amt);
					if(amt > 0)
						BuffOff(who, "cunning");
					else
						BuffOff(who, "feeblemind");
					endif
		
		"car":
		"ar"  :		who.ar_mod := who.ar_mod - amt;
					msg := "Armor is now " + who.ar;
					sound := 0x01E3;
					BuffOff(who, "protection");
				
		"p"   :		who.SetParalyzed( 0 );
					BuffOff(who, "paralyze");
		"call":
		"all" :		SetStrengthMod(who, GetStrengthMod(who) - amt);
					SetDexterityMod(who, GetDexterityMod(who)-amt);
					SetIntelligenceMod(who, GetIntelligenceMod(who)-amt);
					if(amt > 0)
						BuffOff(who, "bless");
					else
						BuffOff(who, "curse");
					endif
		"cebless":
		"ebless": 
					SetStrengthMod(who, GetStrengthMod(who) - amt);
					SetIntelligenceMod(who, GetIntelligenceMod(who)-amt);
					if(amt > 0)
						BuffOff(who, "bless");
					else
						BuffOff(who, "curse");
					endif
		"cpoly":
		"poly":		
					if(!GetObjProperty( who, "IsLiche" ))
						who.ar_mod := who.ar_mod - cint( amt / 3 );
						msg := "Armor is now " + who.ar;
						SetStrengthMod(who, GetStrengthMod(who) - amt);
						SetDexterityMod(who, GetDexterityMod(who)-amt);
						SetIntelligenceMod(who, GetIntelligenceMod(who)-amt);
					else
						SetStrengthMod(who, GetStrengthMod(who) - amt);
						SetDexterityMod(who, GetDexterityMod(who) - amt);
						SetIntelligenceMod(who, GetIntelligenceMod(who)-liche);
						EraseObjProperty( who, "IsLiche" );
						if(GetObjProperty( who, "Type" ) == "Undead")
							EraseObjProperty( who, "Type" );
						endif
					endif
					SendSysMessage( who , "You resume to your true form." );
					who.graphic := who.trueobjtype;
					who.color	:= GetObjProperty(who, "old_colah");
							
					if( who.dead )
						who.graphic := who.graphic + 2;
					endif
					sound := 0x01E3;
					BuffOff(who, "polymorph");
				
        "name": 	who.name := oldvalue;
					msg := "Your name is now " + who.name;
					sound := 0x01E3;
				
		"graphic" : 	who.graphic := oldvalue;
		
		"color"   : 	who.color := oldvalue;
		
		"spiritspeak":	if (!amt)
							RevokePrivilege( who, "hearghosts" );
						endif
				
        "title_prefix": who.title_prefix := oldvalue;
        	
        "title_suffix": who.title_suffix := oldvalue;
        	
		"gender":	who.gender := oldvalue;
		
		"hair":
					DestroyItem( GetEquipmentByLayer( who , LAYER_HAIR ));
					if (oldvalue)
						var newhair := CreateItemAtLocation(5288 , 1176 , 0, oldvalue, 1);
						newhair.color := GetObjProperty(who,"haircolor");
						EquipItem(who,newhair);
					endif
		"beard":
					DestroyItem(GetEquipmentByLayer(who,LAYER_BEARD));
					if (oldvalue)
						var newhair := CreateItemAtLocation(5288 , 1176 , 0, oldvalue, 1);
						newhair.color := GetObjProperty(who,"haircolor");
						EquipItem(who,newhair);
					endif
		"poison":
					SetHpRegenRate( who,  Cint(BaseRegenRate( who )));
					who.SetPoisoned( 0 );
	endcase

	if( sound )
		PlaySoundEffectPrivate(who, sound, who);
	endif
	
	if( msg )
		PrintTextAbovePrivate(who,msg,who);
	endif
	IncRevision(who);
endfunction


function WipeMods( who )

	Set_Critical( 1 );
	
	var had_mods := 0;
	var modarray := GetObjProperty( who , "#mods" );
	if( !modarray )
		modarray := {};
	else
		had_mods := 1;
	endif
	
	foreach mmod in modarray
		if(mmod[4])
			UndoTheMod( who , mmod[1] , mmod[2] , mmod[4] );
		else
			UndoTheMod( who , mmod[1] , mmod[2] );
		endif
	endforeach
	EraseObjProperty( who , "#mods" );
	
	
	var persitedmodarray := GetObjProperty( who , "mods" );
	var newmods := {};
	if( !persitedmodarray )
		persitedmodarray := {};
	else
		had_mods := 1;
	endif
	
	foreach mmod in persitedmodarray
		if( mmod[1] != "poison" )
			UndoTheMod( who , mmod[1] , mmod[2] );
		else
			newmods.append( mmod );
		endif
	endforeach
	
	if( len(newmods) )
		SetObjProperty( who , "mods" , newmods );
	else
		EraseObjProperty( who , "mods" );
	endif
	
	Set_Critical(0);
	
	return had_mods;

endfunction

function BuffOn(who, buff, amount, duration)
   var icon, cliloc1, cliloc2;
   if(who.isa(POLCLASS_NPC))
     return 0;
   endif
  case(lower(buff))
   "reactivearmor":
      icon := 1028;
      cliloc1 := 1075812;
      cliloc2 := 1062495;
   "protection":
      icon := 1029;
      cliloc1 := 1075814;
      cliloc2 := 1062495;
   "magicreflection":
      icon := 1031;
      cliloc1 := 1075817;
      cliloc2 := 1062495;
   "polymorph":
      icon := 1035;
      cliloc1 := 1075824;
      cliloc2 := 1062495;
   "paralyze":
      icon := 1037;
      cliloc1 := 1075827;
      cliloc2 := 1075828;
   "clumsy":
      icon := 1040;
      cliloc1 := 1075831;
      cliloc2 := 1062495;
   "feeblemind":
      icon := 1041;
      cliloc1 := 1075833;
      cliloc2 := 1062495;
   "weaken":
      icon := 1042;
      cliloc1 := 1075837;
      cliloc2 := 1062495;
   "curse":
      icon := 1043;
      cliloc1 := 1075835;
      cliloc2 := 1062495;
   "agility":
      icon := 1045;
      cliloc1 := 1075841;
      cliloc2 := 1062495;
   "cunning":
      icon := 1046;
      cliloc1 := 1075843;
      cliloc2 := 1062495;
   "strength":
      icon := 1047;
      cliloc1 := 1075845;
      cliloc2 := 1062495;
   "bless":
      icon := 1048;
      cliloc1 := 1075847;
      cliloc2 := 1062495;
   "regeneration":
      icon := 1009;
      cliloc1 := 1044106;
      cliloc2 := 1075106;
   "nightsight":
      icon := 1005;
      cliloc1 := 1075643;
      cliloc2 := 1075644;
   "archprotection":
      icon := 1030;
      cliloc1 := 1075816;
      cliloc2 := 1075816;
   "incognito":
      icon := 1032;
      cliloc1 := 1075819;
      cliloc2 := 1075820;
   "invisibility":
      icon := 1036;
      cliloc1 := 1075825;
      cliloc2 := 1075826;
   "poison":
      icon := 1038;
      cliloc1 := 0x0F8627;
      cliloc2 := 0x1069B1;
	"meditation":
	  icon := 1013;
	  cliloc1 := 1044106;
      cliloc2 := 1079410;
   default:
      return error{"errortext" := "Spell not found"};
   endcase
   SendBuffPacket(who, 1, CInt(amount), CInt(duration), icon, cliloc1, cliloc2);
   return 1;
endfunction

function SendBuffPacket(who, turn, amount, duration, icon, cliloc1, cliloc2)
   const BUFF_CMD := 0xDF;
   const OFFSET_MSGLEN := 1;
   const OFFSET_SERIAL := 3;
   var str := "  " + CStr(CInt(amount));
   var outpkt2 := CreatePacket(BUFF_CMD, MSGLEN_VARIABLE);
   outpkt2.SetInt16(OFFSET_MSGLEN, outpkt2.GetSize());
   outpkt2.SetInt32(OFFSET_SERIAL, CInt(who.serial));
   outpkt2.SetInt16(7, icon); // Icon Nr.
   outpkt2.SetInt8(9, 0);
   outpkt2.SetInt8(10, turn); //2 additional data, 1 add, 0 remove
   outpkt2.SetInt32(11, 0);
   outpkt2.SetInt16(15, icon); // Icon Nr.
   outpkt2.SetInt8(17, 0);
   outpkt2.SetInt8(18, turn); //2 additional data, 1 add, 0 remove
   if(turn)
   outpkt2.SetInt32(19, 0); 
   outpkt2.SetInt16(23, duration);
   outpkt2.SetInt16(25, 0x0);
   outpkt2.SetInt8(27, 0x0);
   outpkt2.SetInt32(28, cliloc1); 
   outpkt2.SetInt32(32, cliloc2);
   outpkt2.SetInt32(36, 0x0);
   outpkt2.SetInt8(40, 0x0);
   outpkt2.SetInt8(41, 0x1);
   outpkt2.SetUnicodeStringFlipped(42,CAscZ(str),1);
   endif
   outpkt2.SendPacket(who);
endfunction

function BuffOff(who, buff)
   var icon, cliloc1, cliloc2;
   if(who.isa(POLCLASS_NPC))
	return 0;
   endif
   case(lower(buff))
	"reactivearmor":
		icon := 1028;
		cliloc1 := 1075812;
		cliloc2 := 1062495;
	"protection":
		icon := 1029;
		cliloc1 := 1075814;
		cliloc2 := 1062495;
	"magicreflection":
		icon := 1031;
		cliloc1 := 1075817;
		cliloc2 := 1062495;
	"polymorph":
		icon := 1035;
		cliloc1 := 1075824;
		cliloc2 := 1062495;
	"paralyze":
		icon := 1037;
		cliloc1 := 1075827;
		cliloc2 := 1075828;
	"clumsy":
		icon := 1040;
		cliloc1 := 1075831;
		cliloc2 := 1062495;
	"feeblemind":
		icon := 1041;
		cliloc1 := 1075833;
		cliloc2 := 1062495;
	"weaken":
		icon := 1042;
		cliloc1 := 1075837;
		cliloc2 := 1062495;
	"curse":
		icon := 1043;
		cliloc1 := 1075835;
		cliloc2 := 1062495;
	"agility":
		icon := 1045;
		cliloc1 := 1075841;
		cliloc2 := 1062495;
	"cunning":
		icon := 1046;
		cliloc1 := 1075843;
		cliloc2 := 1062495;
	"strength":
		icon := 1047;
		cliloc1 := 1075845;
		cliloc2 := 1062495;
	"bless":
		icon := 1048;
		cliloc1 := 1075847;
		cliloc2 := 1062495;
	"regeneration":
		icon := 1009;
		cliloc1 := 1044106;
		cliloc2 := 1075106;
	"nightsight":
		icon := 1005;
		cliloc1 := 1075643;
		cliloc2 := 1075644;
	"archprotection":
		icon := 1030;
		cliloc1 := 1075816;
		cliloc2 := 1075816;
	"incognito":
		icon := 1032;
		cliloc1 := 1075819;
		cliloc2 := 1075820;
	"invisibility":
		icon := 1036;
		cliloc1 := 1075825;
		cliloc2 := 1075826;
	"poison":
		icon := 1038;
		cliloc1 := 0x0F8627;
		cliloc2 := 0x1069B1;
	"meditation":
	  icon := 1013;
	  cliloc1 := 1044106;
      cliloc2 := 1079410;
   default:
      return error{"errortext" := "Spell not found"};     
   endcase
   SendBuffPacket(who, 0, 0, 0, icon, cliloc1, cliloc2);
endfunction