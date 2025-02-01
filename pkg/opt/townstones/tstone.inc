use os;
use uo;

include "include/math";

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


function SetVotepercentage(item)
	var retval := 0.00;
	retval := retval + cdbl(votos);
	retval := retval / cdbl(populacao);
	retval := retval * 100;
	retval := cint(retval);
	SetObjProperty(item, "votepercent", retval);
	return 1;
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


function Append_data(original, newdata)
	var i;
	for(i:=1; i<=newdata.size();i:=i+1);
		original.append(newdata[i]);
	endfor
	return original;
endfunction


function votemenu(who, item, type)
	//build vote menu
	var i;
	var curpage := 2;
	var textY;
	var posicao := 1;
	var size := (candidatos.size()/2);
	var lastpage := 1 + high(size,6);
	var firstdata := data.size();

	//loop
	for(i:=1; i<=size; i:=i+1)

		//setting the header and the scrollers

		if (posicao == 1)
			layout.append(cstr("page " + curpage));
			if(curpage !=2)
				layout.append(cstr("button 435 220 250 251 0 " + (curpage-1) + " 0 "));
			endif
			if(lastpage != curpage)
				layout.append(cstr("button 435 410 252 253 0 " + (curpage+1) + " 0 "));
			endif
			curpage := curpage +1;
		endif


		//text
		textY := (275 + (cint(posicao) * 20)) -(type*20);

		layout.append(cstr("text 165 " +textY+ " 0 " + (firstdata-1+i)));


		//button
		layout.append(cstr("button 330 " + cstr(textY) + " 5230 5231 1 0 " + cstr(i)));

		//endloop increases
		if (posicao <6)
			posicao := posicao +1;
		else
			posicao := 1;
		endif

		//data
		data[(firstdata+i)] :=  cstr(candidatos[(2*i)] );

	endfor
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
			return 0;
		endif
	endfor

	candidatos.append(who.serial);
	candidatos.append(who.name);
	set_critical(0);
	SendSysMessage(who, "Now you are a candidate, there are  " + (candidatos.size()/2) + " others.");
	SetObjProperty(item, "candidatos", candidatos);
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
	SetVotepercentage(item);

	if(votos > 5 and votos >= (populacao*2/3) and timer < ReadGameClock() )
		eleicao(item);
	endif
endfunction


function EleicoesMenu(who, item)

	layout := Append_data(layout, eleicaolayout);
	data := Append_data(data, eleicaodata);

	data[13] := candidatos.size()/2;
	data[14] := votepercent + "%";

	votemenu(who, item, 0);

endfunction


function PleibicitoMenu(who, item)

	layout := Append_data(layout, pleibicitolayout);
	data := Append_data(data, pleibicitodata);
	data[11] := pleibicito;
	data[14] :=  votepercent + "%";
	votemenu(who, item, 1);

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
		1: if(choose)
			Broadcast(" " +city+ " has a new leader: " + candidatos[choose*2]);
			SetObjProperty(item, "mayor",  candidatos[choose*2]);
			SetObjProperty(item, "mayorserial",  candidatos[choose*2-1]);
			SetObjProperty(item, "eleicao", 0);
			else
			Broadcast("Elections in "+city+" are a tie, new counting delayed");
			timer := ReadGameclock() + (3600*6);
			SetObjProperty(item, "timer", timer);
			endif

		2: if(choose)
			SetObjProperty(item, "Pleibicito_Result", {pleibicito, candidatos[choose*2]} );
			SetObjProperty(item, "pleibicito", 0);
		else
			timer := ReadGameclock() + (3600*6);
			SetObjProperty(item, "timer", timer);
		endif

	endcase

endfunction



function ConvocarEleicoes(who, item)
	if(eleicoes or pleibicito)
		SendSysMessage(who, "Election running");
		return;
	endif

	foreach runa in EnumerateItemsInContainer( item )
		destroyitem(runa);
	endforeach

	SetObjProperty(item, "candidatos", { });
	SetObjProperty(item, "eleicao", 1);
	SetObjProperty(item, "votos", 0);
	SetObjProperty(item, "timer", ReadGameClock() + (12*3600) );
	SetObjProperty(item, "votepercent", 0);

	BroadCast("Elections on " + city + " are open");

endfunction

function ConvocarPleibicito(who, item)
	if(eleicoes or pleibicito)
		SendSysMessage(who, "Election runing");
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

	SendSysMessage(who, "Type now the options, press ENTER to finish");

	repeat

	temp := SendTextEntryGump( who, "Diga a opcao " +(opcoes.size()/2+1) , 50);
	if(temp)
		opcoes.append(0);
		opcoes.append(temp);
	endif

	until(!temp);

	SetObjProperty(item, "candidatos", opcoes);
	SetObjProperty(item, "pleibicito", pergunta);
	SetObjProperty(item, "votos", 0);
	SetObjProperty(item, "timer", ReadGameClock() + (12*3600) );
	SetObjProperty(item, "votepercent", 0);
	SetObjProperty(item, "Pleibicito_Result", {});

	BroadCast("New pool on " + city );

endfunction

