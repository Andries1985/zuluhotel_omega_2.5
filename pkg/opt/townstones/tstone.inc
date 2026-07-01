use os;
use uo;

include "include/math";
include "include/client";

const CITIZEN_ELECTION_COOLDOWN := (60*24*3600); // 2 months in seconds
const ELECTION_DURATION := (2*60); // 2 minutes in seconds (testing)

function countvotes(item)

	var result := { };  //formato { n� votos}
	var i, voto;

	//conta os votos
	foreach runa in EnumerateItemsInContainer( item )
		if (runa.objtype == 0xa321)
			voto:= cint(GetObjProperty(runa, "opcao"));
			if (!result[voto])
				result[voto] := 1;
			else
				result[voto] := result[voto] +1;
			endif
		endif
	endforeach

	//verifica quem venceu

	var nvotos := 0;
	var  vencedor;
	var empate :=0;

	for(i:=1;i<=result.size(); i:=i+1)
		if ( nvotos < result[i] )
			nvotos := cint(result[i]) ;
			vencedor := i;
			empate := 0;
		elseif (  nvotos == cint(result[i]) )
			empate := 1;
		endif
	endfor

	if(!empate)
		return vencedor;
	else
		return 0;
	endif

endfunction

function GetTopTiedOptionNames(item)
	var counts := {};
	var maxvotes := 0;
	var i;

	foreach runa in EnumerateItemsInContainer(item)
		if(runa.objtype == 0xa321)
			var voto := CInt(GetObjProperty(runa, "opcao"));
			if(!counts[voto])
				counts[voto] := 1;
			else
				counts[voto] := counts[voto] + 1;
			endif

			if(counts[voto] > maxvotes)
				maxvotes := counts[voto];
			endif
		endif
	endforeach

	if(maxvotes <= 0)
		return "No options";
	endif

	var result_text := "";
	for(i := 1; i <= counts.size(); i := i + 1)
		if(counts[i] == maxvotes)
			var option_name := CStr(candidatos[(2 * i)]);
			if(!option_name)
				option_name := "Option " + CStr(i);
			endif

			if(result_text)
				result_text := result_text + ", " + option_name;
			else
				result_text := option_name;
			endif
		endif
	endfor

	if(!result_text)
		return "No options";
	endif

	return result_text;
endfunction


function SetVotepercentage(item)
	var retval := 0.00;
	retval := retval + cdbl(votos);
	retval := retval / cdbl(populacao);
	retval := retval * 100;
	retval := cint(retval);
	SetObjProperty(item, "votepercent", retval);
	return retval;
endfunction


function high(divided, divisor)
	var retval := divided/divisor;

	if(mod(divided, divisor))
		retval:= retval +1;
	endif
	return retval;
endfunction


function GetTownName(town)
	var namelist := GetGlobalProperty("townlist");
	var name := namelist[town];
	return name;
endfunction

function SyncTownstoneState(item)
	if(!item)
		return;
	endif

	var region_name := "";
	var town_id := CInt(GetObjProperty(item, "town"));
	if(town_id)
		region_name := GetTownName(town_id);
	endif

	if(!region_name)
		return;
	endif

	TouchTownstoneRegionEntry(item, region_name);
endfunction

function BroadcastSystemLine(message)
	Broadcast(CStr(message), FONT_RUNE, 25);
endfunction




function candidato(who, item)

	var i;
	if(who.serial == mayorserial)
		SendSysMessage(who, "You can't be candidate again");
		return 0;
	endif

	set_critical(1);
	for(i:=1;i<=candidatos.size();i:=i+2)
		if(candidatos[i] == who.serial)
			SendSysMessage(who, "You are already a candidate");
			set_critical(0);
			return 0;
		endif
	endfor

	candidatos.append(who.serial);
	candidatos.append(who.name);
	set_critical(0);
	SendSysMessage(who, "Now you are a candidate, there are  " + (candidatos.size()/2) + " others.");
	SetObjProperty(item, "candidatos", candidatos);
	SyncTownstoneState(item);

endfunction



function voteto(who, item, choose)
	//verifica se fulando jah votou
	foreach runa in EnumerateItemsInContainer( item )
		if (runa.objtype == 0xa321)
			if (GetObjProperty(runa, "serial") == who.acctname)
				SendSysMessage(who, "You have already voted.");
				return 0;
			endif
		endif
	endforeach

	//Cria 1 runa do voto.

	var voto := CreateItemInContainer( item, 0xa321, 1);
	SetObjProperty(voto, "serial", who.acctname);
	SetObjProperty(voto, "opcao", choose);
	votos := votos+1;
	SetObjProperty(item, "votos",  votos);

	SendSysMessage(who, "You just voted to " + candidatos[(2*choose)] );
	var current_votepercent := SetVotepercentage(item);
	SyncTownstoneState(item);

	if(pleibicito and current_votepercent >= 100)
		eleicao(item);
		var poll_result := GetObjProperty(item, "Pleibicito_Result");
		if(poll_result and poll_result.size() >= 2)
			SendSysMessage(who, "Poll ended: " + CStr(poll_result[2]));
		else
			SendSysMessage(who, "Poll ended.");
		endif
		return;
	endif

	if(timer < ReadGameClock())
		eleicao(item);
	endif

endfunction


function eleicao(item);
	var choose := countvotes(item);

	var type;
	if (eleicoes)
		type :=1;
	elseif(pleibicito)
		type :=2;
	else
		type :=0;
	endif

	case(type)
		1: if(votos < 3)
			BroadcastSystemLine("Election Result");
			BroadcastSystemLine("Town: " + city + " | Winner: No winner (not enough votes)");
			SetObjProperty(item, "eleicao", 0);
			SetObjProperty(item, "candidatos", { });
			SetObjProperty(item, "votos", 0);
			SetObjProperty(item, "votepercent", 0);
			SetObjProperty(item, "timer", 0);
		elseif(choose)
			BroadcastSystemLine("Election Result");
			BroadcastSystemLine("Town: " + city + " | Winner: " + candidatos[choose*2]);
			SetObjProperty(item, "mayor",  candidatos[choose*2]);
			SetObjProperty(item, "mayorserial",  candidatos[choose*2-1]);
			SetObjProperty(item, "eleicao", 0);
			SetObjProperty(item, "nextcitizenelection", ReadGameClock() + CITIZEN_ELECTION_COOLDOWN);
			SetObjProperty(item, "candidatos", { });
			SetObjProperty(item, "votos", 0);
			SetObjProperty(item, "votepercent", 0);
			SetObjProperty(item, "timer", 0);
		else
			BroadcastSystemLine("Election Result");
			BroadcastSystemLine("Town: " + city + " | Winner: No winner (tie)");
			SetObjProperty(item, "eleicao", 0);
			SetObjProperty(item, "candidatos", { });
			SetObjProperty(item, "votos", 0);
			SetObjProperty(item, "votepercent", 0);
			SetObjProperty(item, "timer", 0);
			if(GetObjProperty(item, "mayorserial"))
				SetObjProperty(item, "nextcitizenelection", ReadGameClock() + CITIZEN_ELECTION_COOLDOWN);
			else
				SetObjProperty(item, "nextcitizenelection", 0);
			endif
		endif

		2: if(choose)
			BroadcastSystemLine("Poll Result");
			BroadcastSystemLine("Town: " + city + " | Question: " + CStr(pleibicito));
			BroadcastSystemLine("Result: " + candidatos[choose*2]);
			SetObjProperty(item, "Pleibicito_Result", {pleibicito, candidatos[choose*2]} );
			SetObjProperty(item, "pleibicito", 0);
			SetObjProperty(item, "candidatos", { });
			SetObjProperty(item, "votos", 0);
			SetObjProperty(item, "votepercent", 0);
			SetObjProperty(item, "timer", 0);
		else
			if(populacao > 0 and votos >= populacao)
				var tied_options := GetTopTiedOptionNames(item);
				BroadcastSystemLine("Poll Result");
				BroadcastSystemLine("Town: " + city + " | Question: " + CStr(pleibicito));
				BroadcastSystemLine("Result: Tie: " + tied_options);
				SetObjProperty(item, "Pleibicito_Result", {pleibicito, "Tie: " + tied_options});
				SetObjProperty(item, "pleibicito", 0);
				SetObjProperty(item, "candidatos", { });
				SetObjProperty(item, "votos", 0);
				SetObjProperty(item, "votepercent", 0);
				SetObjProperty(item, "timer", 0);
			else
				timer := ReadGameclock() + (3600*6);
				SetObjProperty(item, "timer", timer);
			endif
		endif

	endcase

	SyncTownstoneState(item);

endfunction



function ConvocarEleicoes(who, item)

	if(eleicoes or pleibicito)
		SendSysMessage(who, "Elections running.");
		return;
	endif

	if(who.serial != mayorserial)
		if(populacao < 3)
			SendSysMessage(who, "You need at least 3 town members before you can call an election.");
			return;
		endif

		if(mayorserial)
			var next_election_time := cint(GetObjProperty(item, "nextcitizenelection"));
			if(next_election_time and ReadGameClock() < next_election_time)
				SendSysMessage(who, "Citizens can only call elections every 2 months while a mayor exists.");
				return;
			endif
		endif
	endif

	foreach runa in EnumerateItemsInContainer( item )
		destroyitem(runa);
	endforeach

	SetObjProperty(item, "candidatos", { });
	SetObjProperty(item, "eleicao", 1);
	SetObjProperty(item, "votos", 0);
	SetObjProperty(item, "timer", ReadGameClock() + ELECTION_DURATION );
	SetObjProperty(item, "votepercent", 0);
	SyncTownstoneState(item);

			BroadcastSystemLine("Elections for " + city + " are open!");

endfunction

function ConvocarPleibicito(who, item)

	if(eleicoes or pleibicito)
		SendSysMessage(who, "Elections running.");
		return;
	endif

	foreach runa in EnumerateItemsInContainer( item )
		destroyitem(runa);
	endforeach

	var pergunta, opcoes := { }, temp;

	pergunta := SendTextEntryGump( who, "What's the question? " , 50);
	if(!pergunta)
		SendSysMessage(who, "Cancelled");
		return;
	endif

	SendSysMessage(who, "Type the options, press ENTER to finish");

	repeat

	temp := SendTextEntryGump( who, "Enter Option " +(opcoes.size()/2+1) , 50);
	if(temp)
		opcoes.append(0);
		opcoes.append(temp);
	endif

	until(!temp);

	SetObjProperty(item, "candidatos", opcoes);
	SetObjProperty(item, "pleibicito", pergunta);
	SetObjProperty(item, "votos", 0);
	SetObjProperty(item, "timer", ReadGameClock() + (7*24*3600) );
	SetObjProperty(item, "votepercent", 0);
	SetObjProperty(item, "Pleibicito_Result", {});
	SyncTownstoneState(item);

	BroadcastSystemLine("New poll for " + city );

endfunction

