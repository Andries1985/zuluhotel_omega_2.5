use math;

include "include/bitwise";
include ":gumps:yesno";

const ORDERGUARDTYPE := 1;
const CHAOSGUARDTYPE := 2;
const CHAOSSHIELDTYPE := 0x86df;
const ORDERSHIELDTYPE := 0x86ef;

function AddVirtue(who, amount :=1, nomsg:=0)

	if (!amount) amount := 1; endif

	var yourvirtue := GetObjProperty(who,"virtue");
	if (!yourvirtue) yourvirtue := 0; endif
	if (yourvirtue > 50000) return; endif

	var desc;
	case (abs(amount))
		1: desc := " a tiny amount of ";
		2: desc := " a very small amount of ";
		3: desc := " a small amount of ";
		4: desc := " some ";
		5: desc := " a good amount of ";
		6: desc := " a lot of ";
		7: desc := " great amount of ";
		default: desc := " massive amount of ";
	endcase

	yourvirtue := yourvirtue + amount;
	if (!nomsg)
		if (amount > 0)
			SendSysMessage(who,"You have gained" + desc + "Virtue");
		else
			SendSysMessage(who,"You have lost" + desc + "Virtue");
		endif
	endif
	SetObjProperty(who,"virtue",yourvirtue);
	if (GetObjProperty(who,"guardtype") = ORDERGUARDTYPE)
		if ( Random(8) < amount ) BonusHonor(who); endif
	endif

endfunction

function AwardVirtueForRes(you,them)

	var lastres := GetObjProperty(you,"#lastres");
	if (!lastres) lastres := { }; endif
	foreach person in lastres
		if (person = them.serial) return; endif
	endforeach
	lastres.append(them.serial);
	if ( lastres.size() > 3 ) lastres.erase(1); endif
	SetObjProperty(you,"#lastres",lastres);

	var amt := 2;
	var lastdeath := GetObjProperty(them,"death");
	if (!lastdeath) lastdeath := ReadGameClock() + 3600; endif
	if ( lastdeath > (ReadGameClock()+3000) )
		amt := 5;
	endif

	AddVirtue(you,amt);

endfunction

function VirtueAmount(you)

	var yourvirtue := GetObjProperty(you,"virtue");
	if (!yourvirtue) return 0; else return yourvirtue; endif

endfunction

function SetMyTitle(who,virtue)

	if ( virtue < 200 )
		who.title_prefix := "Squire ";
	elseif ( virtue < 400 )
		if (who.gender)
			who.title_prefix := "Dame ";
		else
			who.title_prefix := "Sir ";
		endif
	elseif ( virtue < 800 )
		if (who.gender)
			who.title_prefix := "The Esteemed Dame ";
		else
			who.title_prefix := "The Esteemed Sir ";
		endif
	elseif ( virtue < 1600 )
		if (who.gender)
			who.title_prefix := "The Noble Dame ";
		else
			who.title_prefix := "The Noble Sir ";
		endif
	elseif ( virtue < 3200 )
		if (who.gender)
			who.title_prefix := "The Illustrious Dame ";
		else
			who.title_prefix := "The Illustrious Sir ";
		endif
	elseif ( virtue < 6400 )
		if (who.gender)
			who.title_prefix := "Lady ";
		else
			who.title_prefix := "Lord ";
		endif
	elseif ( virtue < 12800 )
		if (who.gender)
			who.title_prefix := "The Noble Lady ";
		else
			who.title_prefix := "The Noble Lord ";
		endif
	elseif ( virtue < 25600 )
		if (who.gender)
			who.title_prefix := "The Illustrious Lady ";
		else
			who.title_prefix := "The Illustrious Lord ";
		endif
	else
		if (who.gender)
			who.title_prefix := "The Glorious Lady ";
		else
			who.title_prefix := "The Glorious Lord ";
		endif
	endif

endfunction

function CanJoin(guard, recruit, guardtype, shieldtype)

	if (recruit.npctemplate) return; endif
	if (!recruit.acctname) return; endif
	if (guard = recruit) return; endif

	var yourtype := GetObjProperty(recruit,"guardtype");
	if (yourtype)
		SendSysMessage(guard,"They're already a Virtue Guard.");
		if (yourtype = guardtype)
		  var newshield := CreateItemAtLocation(5288 , 1176 , 0, shieldtype, 1, "britannia");
		  if (!EquipItem(recruit,newshield))
		    DestroyItem(newshield);
		  else
		    var founditem, addonitem := cint(GetObjProperty( recruit, "virtueshield" ));
		    if (addonitem)
		      founditem := destroyitem(systemfindobjectbyserial(addonitem));
		      if (founditem<>1) founditem:=destroyitem(systemfindobjectbyserial(addonitem,1)); endif
		      if (founditem<>1) destroyitem(systemfindobjectbyserial(addonitem,2)); endif
		    endif
		    setobjproperty(recruit,"virtueshield",newshield.serial);
		  endif
		  return;
		else
		SendSysMessage(guard,"They're already a Virtue Guard.");
		SendSysMessage(recruit,"You are already a Virtue Guard.");
		endif
	endif

	var basestr := recruit.strength - recruit.strength_mod;
	if (basestr < 90)
		SendSysMessage(guard,"They don't seem strong enough, to you.");
		return;
	endif

	var leadership := getskill(recruit, SKILLID_LEADERSHIP);
	if (leadership < 90)
		SendSysMessage(guard,"They seem to lack the necessary leadership skills...");
		return;
	endif

	if ( VirtueAmount(recruit) < 3000 )
		SendSysMessage(guard,"They don't seem to be terribly Virtuous...");
		return;
	endif

	SendSysMessage(recruit,"Do you want to join the Virtue Guards?");
	if (!YesNo(recruit,"Yes!")) 
		SendSysMessage(guard,"They don't want to join.");
		return; 
	endif

	setobjproperty(recruit,"guardtype",guardtype);
	recruit.title_prefix := "Squire ";
	recruit.title_suffix := ", Virtue Guard";
	var newshield := CreateItemAtLocation(recruit.x , recruit.y , recruit.z, shieldtype, 1, "britannia");
	EquipItem(recruit,newshield);
	PlaySoundEffect(recruit, SFX_SPELL_BLESS);
	PlayObjectCenteredEffect( recruit, FX_BLESS_EFFECT, 10,10);
        setobjproperty(recruit,"virtueshield",newshield.serial);
	SendSysMessage(guard,"Tell them to MAKE US PROUD!");

endfunction

function BestowHonor(me, you, howmuch := 0)

	if (you.npctemplate) return; endif
	if (!you.acctname) return; endif
	if (me = you) return; endif

	var yourtype := GetObjProperty(you,"guardtype");
	if (!yourtype)
		SendSysMessage(me,"That's not a Virtue Guard");
		return; 
	endif
	if (yourtype != ORDERGUARDTYPE)
		SendSysMessage(me,"That's not an Order Guard");
		return;
	endif

//honor[1] is how many points I've spent, honor[2] is points received
//honor[3] is the last person you honored, so it takes 
//at least three twinks to sploit

	var myvirtue := VirtueAmount(me);
	var myhonor := GetObjProperty(me,"honor");
	if (!myhonor)
		myhonor := {};
		myhonor[1] := myvirtue;
		myhonor[2] := 0;
		myhonor[3] := 0;
	endif

	var yourvirtue := VirtueAmount(you);
	var yourhonor := GetObjProperty(you,"honor");
	if (!yourhonor)
		yourhonor := {};
		yourhonor[1] := yourvirtue;
		yourhonor[2] := 0;
		yourhonor[3] := 0;
	endif

	if (myhonor[3] != you.serial)
		var myunspent := myvirtue - myhonor[1];
		if (howmuch)
			myunspent := cint(myunspent/2);
		endif
		if ( myunspent > 0)
			yourhonor[2] := yourhonor[2] + myunspent;
			myhonor[1] := myhonor[1] + myunspent;
			myhonor[3] := you.serial;
			// SetMyTitle(you,yourhonor[2]);
			SetObjProperty(me,"honor",myhonor);
			SetObjProperty(you,"honor",yourhonor);
		endif
	endif

	if (!howmuch)
		PerformAction(me,ANIM_BOW);
		SendSysMessage(me,"You honor " + you.name);
		SendSysMessage(you,me.name + " honors you");
	else
		PerformAction(me,ANIM_SALUTE);
		SendSysMessage(me, "You respect " + you.name);
		SendSysMessage(you, me.name + " respects you");
	endif

endfunction

function BonusHonor(me, amt := 1)

//we have to make up for "spent points" leaving the system via
//capped characters, quitting character, and recriminations:

	var myhonor := GetObjProperty(me,"honor");
	if (!myhonor) return; endif
	myhonor[1] := myhonor[1] - amt;
	SetObjProperty(me,"honor",myhonor);

endfunction

function Censure( me, you )

	if (you.npctemplate) return; endif
	if (!you.acctname) return; endif
	if (me = you) return; endif

	var yourtype := GetObjProperty(you,"guardtype");
	if (!yourtype)
		SendSysMessage(me,"That's not a Virtue Guard");
		return; 
	endif
	if (yourtype != ORDERGUARDTYPE)
		SendSysMessage(me,"That's not an Order Guard");
		return;
	endif

//honor[1] is how many points I've spent, honor[2] is points received
//honor[3] is the last person you honored, so it takes 
//at least three twinks to sploit

	var myvirtue := VirtueAmount(me);
	var myhonor := GetObjProperty(me,"honor");
	if (!myhonor)
		myhonor := {};
		myhonor[1] := 0;
		myhonor[2] := 0;
		myhonor[3] := 0;
	endif

	var yourvirtue := VirtueAmount(you);
	var yourhonor := GetObjProperty(you,"honor");
	if (!yourhonor)
		yourhonor := {};
		yourhonor[1] := 0;
		yourhonor[2] := 0;
		yourhonor[3] := 0;
	endif

	if (myhonor[3] != you.serial)
		var myunspent := myvirtue - myhonor[1];
		myunspent := cint(myunspent/10);
		if ( myunspent > 0)
			yourhonor[2] := yourhonor[2] - myunspent;
			myhonor[1] := myhonor[1] - myunspent;
			myhonor[3] := you.serial;
			// SetMyTitle(you,yourhonor[2]);
			// SetMyTitle(me,myhonor[2]);
			SetObjProperty(me,"honor",myhonor);
			SetObjProperty(you,"honor",yourhonor);
		endif
	endif

	PerformAction(me,ANIM_FIDGET_2);
	sleepms(500);
	PerformAction(me,ANIM_FIDGET_1);
	sleepms(500);
	PerformAction(me,ANIM_EAT);
	SendSysMessage(me,"You censure " + you.name);
	SendSysMessage(you,me.name + " has censured you");

endfunction

function SetNonVGTitle( who )

	var virtue := VirtueAmount(who);
	if (!virtue) return; endif

	if ( GetObjProperty(who,"guardtype") )
		SetMyTitle(who,virtue);
		return;
	endif

	if ( virtue < -25600 )
		who.title_prefix := "The Sinister ";
		SetObjProperty(who,"permared",1);
		who.SetMurderer(1);
	elseif ( virtue < -12800 )
		who.title_prefix := "The Vile ";
		SetObjProperty(who,"permared",1);
		who.SetMurderer(1);
	elseif ( virtue < -6400 )
		who.title_prefix := "The Ignoble ";
		SetObjProperty(who,"permared",1);
		who.SetMurderer(1);
	elseif ( virtue < -3200 )
		who.title_prefix := "The Malicious ";
		SetObjProperty(who,"permared",0);
		who.SetMurderer(0);
		who.setcriminal( 65500 );
	elseif ( virtue < -1600 )
		who.title_prefix := "The Dastardly ";
		who.setcriminal( 65500 );
	elseif ( virtue < -800 )
		who.title_prefix := "The Dishonorable ";
	elseif ( virtue < -400 )
		who.title_prefix := "The Unsavory ";
	elseif ( virtue < -200 )
		who.title_prefix := "The Rude ";
	elseif ( virtue < 200 )
		who.title_prefix := "";
	elseif ( virtue < 400 )
		who.title_prefix := "The Honorable ";
	elseif ( virtue < 800 )
		who.title_prefix := "The Commendable ";
	elseif ( virtue < 1600 )
		who.title_prefix := "The Admirable ";
	elseif ( virtue < 3200 )
		who.title_prefix := "The Renowned  ";
	elseif ( virtue < 6400 )
		who.title_prefix := "The Famed ";
	elseif ( virtue < 12800 )
		who.title_prefix := "The Noble ";
	elseif ( virtue < 25600 )
		who.title_prefix := "The Illustrious ";
	else
		who.title_prefix := "The Glorious ";
	endif

endfunction
