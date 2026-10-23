use uo;
use polsys;

// Client buff bar icons (packet 0xDF). Kept on their own so spell code that only needs the
// icons (spelldata.inc, potions) doesn't have to pull in dotempmods.inc.

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
      cliloc2 := 1062495;	// blank: 0x1069B1 needs two arguments and SendBuffPacket sends one
	"meditation":
	  icon := 1013;
	  cliloc1 := 1044106;
      cliloc2 := 1079410;
	"poisonimmunity":
		icon := 1020;		// Gift of Renewal icon; nothing else here uses it
		cliloc1 := 1061161;	// Poison Resistance
		cliloc2 := 1063505;	// Poison Immunity: ~1_VAL~
	"dragonskin":
		icon := 1021;		// Attune Weapon icon; nothing else here uses it
		cliloc1 := 1061158;	// Physical Resistance
		cliloc2 := 1063504;	// Physical Protection: ~1_VAL~
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
		cliloc2 := 1062495;	// blank: 0x1069B1 needs two arguments and SendBuffPacket sends one
	"meditation":
	  icon := 1013;
	  cliloc1 := 1044106;
      cliloc2 := 1079410;
	"poisonimmunity":
		icon := 1020;		// Gift of Renewal icon; nothing else here uses it
		cliloc1 := 1061161;	// Poison Resistance
		cliloc2 := 1063505;	// Poison Immunity: ~1_VAL~
	"dragonskin":
		icon := 1021;		// Attune Weapon icon; nothing else here uses it
		cliloc1 := 1061158;	// Physical Resistance
		cliloc2 := 1063504;	// Physical Protection: ~1_VAL~
   default:
      return error{"errortext" := "Spell not found"};     
   endcase
   SendBuffPacket(who, 0, 0, 0, icon, cliloc1, cliloc2);
endfunction
