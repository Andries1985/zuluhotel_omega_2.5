const CHAMPION_GREED        := 1;
const CHAMPION_POWER        := 2;
const CHAMPION_CHAOS        := 3;
const CHAMPION_INFLUENCE    := 4;

function GetSkullTypes()
  var champions := dictionary;
  champions[CHAMPION_GREED]     := "Greed";
  champions[CHAMPION_POWER]     := "Power";
  champions[CHAMPION_CHAOS]     := "Chaos";
  champions[CHAMPION_INFLUENCE] := "Influence";
  return champions;
endfunction

function GetSkullType(champion_id)
  var champions := GetSkullTypes();
  return champions[champion_id];
endfunction
