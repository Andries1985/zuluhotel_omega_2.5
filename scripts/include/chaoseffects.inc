use uo;
use os;
use cfgfile;

include "include/client";
include "include/random";


function CheckForChaosEffect(who, basechance, minchance, avoidance)

// This function checks to see if a chaos effect will happen.
// Basically, it's just a random number generator.  The following
// variables must be passed through:
//     who = victim
//     basechance = the unmodified chance for a chaos effect
//     minchance = the minimum modified chance for a chaos effect
//     avoidance = the # of magery pts needed to reduce chance by 1

	var i;

	i :=Random(100) + 1;
	avoidance := GetSkill(who, SKILLID_MAGERY) / avoidance;
	basechance := basechance - avoidance;

	if (basechance < minchance)
		basechance := minchance;
	endif

	if (i <= basechance)
		return 1;
	else
		return 0;
	endif

endfunction


function GenerateRandomChaosEffects(who, what, alterchance, mutatechance, gatechance, divertchance, effects)

// This will generate one or more chaos effects.  You need to
// specify the following information:
//     who = the subject (usually the caster)
//     what = the item or target (gate, rune, corpse, NPC, etc...)
//     alterchance = the chance, relative to the others, of effect
//     mutatechance = as above
//     gatechance = as above
//     divertchance = as above
//     effects = the total number of effects
// For example: GenerateRandomEffect(caster, target, 5, 5, 0, 1, 1)
// would center the effects on the caster, with a 5/11 chance of 
// a alteration or mutation, no chance for a gate, and 1/11 chance
// of being diverted.  If a creature is gated, it will go to the
// target.

	var i, x;

	While (effects > 0)
		x := alterchance + mutatechance + gatechance + divertchance;
		i := Random(x) + 1;
		x := x - alterchance;
		if (x < i)
			ChaosAlteration(who);
		else
			x := x - mutatechance;
			if (x < i)
				ChaosMutation(who);
			else
				x := x - gatechance;
				if (x < i)
					GatedDaemon(what.x, what.y, what.z, what.realm);
				else
					DivertPlayer(who);
				endif
			endif
		endif
		effects := effects - 1;
	endwhile

endfunction


function ChaosAlteration(who)

// This effect will cause minor alterations to the poor victim.  These
// alterations are minor, but some are permanent.

	var i, x, success :=0;
	i := Random(8) + 1;

	case i
		1:
			x := ( GetStrength(who) - 1);
			SetBaseStrength(who, x);
			SendSysMessage(who, "You feel weakened...");
			return;
		2:
			x := ( GetDexterity(who) - 1);
			SetBaseDextery(who, x);
			SendSysMessage(who, "You feel disoriented...");
			return;
		3:
			x := (GetIntelligence(who) - 1);
			SetBaseIntelligence(who, x);
			SendSysMessage(who, "You feel confused...");
			return;
		4:
			x := Cint( GetObjProperty(who, "poison_level") );
			x := x + 2;
			SetObjProperty(who, "poison_level", cint(x) );
			SendSysMessage(who, "You feel nauseous...");
			return;
		5:
			x := (Random(who.hp) / 3) + 1;
			ApplyRawDamage(who, x);
			SendSysMessage(who, "You are wracked with pain!");
			return;
		6:
			x := (Random(GetMana(who) / 3) + 1;
			SetMana(who, GetMana(who) - x);
			SendSysMessage(who, "Your head starts throbbing!");
			return;
		7:
			While (success = 0)
				x := (Random(SKILLID__HIGHEST) + 1);
				if (GetSkill(who, x) > 0)
					success := GetRawSkill(who, x) - 200;
					if (success < 1)
						success := 1;
					endif
					SetRawSkill(who, x, success);
				endif
			endWhile
			SendSysMessage(who, "You feel disoriented...");
			return;
		8:
			SendSysMessage(who, "Uh-oh...");
			detach();
			x := who.graphic;
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			who.graphic := 0xd0;
			i := RandomDiceStr(5d60);
			sleep(i);
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			who.graphic := x;
			return;
	endcase

endfunction		


function ChaosMutation(who)

// These effects are more severe than the alterations, and all are
// permanent.   Many are downright nasty.  Use at your discretion!

	var i, x, y, success :=0;
	i := Random(10) + 1;

	case i
		1:
			x := ( GetStrength(who) - 5);
			SetBaseStrength(who, x);
			SendSysMessage(who, "You feel weakened...");
			return;
		2:
			x := ( GetDexterity(who) - 5);
			SetBaseDextery(who, x);
			SendSysMessage(who, "You feel disoriented...");
			return;
		3:
			x := (GetIntelligence(who) - 5);
			SetBaseIntelligence(who, x);
			SendSysMessage(who, "You feel confused...");
			return;
		4:
			x := ListEquippedItems(who);
			success := Random( Len(x) ) + 1;
			DestroyItem(x[success]);
			SendSysMessage(who, "Your " + x[success].name + " has disappeared!");
			PlaySoundEffect(who, SFX_SPELL_EXPLOSION);
			return;
		5:
			ApplyRawDamage(who, who.hp);
			return;
		6:
			SetMana(who, 0);
			SendSysMessage(who, "Your head starts throbbing!");
			return;
		7:
			y := RandomDiceStr(2d4);
			While (y > 0)
				While (success = 0)
					x := (Random(SKILLID__HIGHEST) + 1);
					if (GetSkill(who, x) > 0)
						success := GetRawSkill(who, x) - 400;
						if (success < 1)
							success := 1;
						endif
						SetRawSkill(who, x, success);
					endif
				endWhile
				y := y - 1;
			endwhile

			SendSysMessage(who, "You feel disoriented...");
			return;
		8:
			SendSysMessage(who, "Uh-oh...");
			x := who.graphic;
			PlaySoundEffect(who, SFX_SPELL_POLYMORPH);
			return;

		9:
			foreach success in ListEquippedItems(who);
				x := who.x + Random(11) - 5;
				y := who.y + Random(11) - 5;
				MoveItemToLocation(success, x, y, GetWorldHeight(x, y),0 );
			endforeach
			SendSysMessage(who, "You feel colder...");
			return;

		default:
			if (who.gender = 1)
				who.gender := 0;
				who.graphic := 0x190;
				SendSysMessage(who, "You seem to have shrunk...");
			else
				who.gender := 1;
				who.graphic := 0x191;
				SendSysMessage(who, "You seem to have grown...");
			endif
			return;
	endcase

endfunction		



function GatedDaemon(xpos, ypos, zpos, rpos)

// This effect will create a very unhappy daemon, who will likely
// take his frustrations out upon the poor caster.  Suggest uses:
// random effect from a Gate spell, miscast summon, etc. etc.

	CreateNPCFromTemplate("daemon", xpos, ypos, zpos, 0, rpos);

endfunction


function DivertPlayer(who)

// This effect will change the player's location after casting a
// transportation type spell (i.e., gating, recalling).  This will
// read the "diversion" key from the "chaoseffects.cfg" file, and
// choose a random location.

	var i, desc, newx, newy, newz;
	var cfg, elem;

	cfg := ReadConfigFile("chaoseffects");
	elem := FindConfigElem(cfg, "diversion");

	desc := GetConfigStringArray(elem, "desc");
	newx := GetConfigStringArray(elem, "x");
	newy := GetConfigStringArray(elem, "y");
	newz := GetConfigStringArray(elem, "z");

	i := Len(desc);
	i := Random(i) + 1;

	print("Diverting " + who.name + " to " + desc[i]);

	SendSysMessage(who, "Something is amiss...");

	MoveObjectToLocation(who, Cint(newx[i]), Cint(newy[i]), Cint(newz[i]), "britannia", MOVEOBJECT_FORCELOCATION );
	return;

endfunction