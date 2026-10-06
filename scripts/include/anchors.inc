use npc;
use uo;
use cfgfile;

include "include/attributes";
include "include/constants/locations";
include "include/random";

function drop_anchor(me)

        // assumes that var me = npc::self

        var cfg := readconfigfile("npcdesc");

        var dstart := cfg[me.npctemplate].dstart;
        var psub := cfg[me.npctemplate].psub;

        if (!psub)
                psub := 10;
        endif

        if (dstart)
                SetAnchor(me.x, me.y, dstart, psub);
        endif
endfunction

function StepCivilianThroughDoorIfNeeded()
        var me := Self();

        if (!IsDoorPushCivilianTemplate(me))
                return;
        endif

        var move_dir := FacingToMoveDir(me.facing);
        if (!move_dir)
                return;
        endif

        var stepthrudoor;
        for (stepthrudoor := 1; stepthrudoor <= 2; stepthrudoor := stepthrudoor + 1)
                if (!CanMove(move_dir))
                        break;
                endif

                if (!Move(move_dir))
                        break;
                endif
        endfor
endfunction

function TryOpenNearbyNpcDoors()
        var me := Self();
        start_script("::/misc/npcdoor", me);
endfunction

function IsDoorPushCivilianTemplate(me)
        if (!me or !me.npctemplate)
                return 0;
        endif

        if (me.npctemplate == "townperson" or me.npctemplate == "person" or me.npctemplate == "noble")
                return 1;
        endif

        return 0;
endfunction

function FacingToMoveDir(facing)
        case (CInt(facing))
                0:
                        return "N";
                1:
                        return "NE";
                2:
                        return "E";
                3:
                        return "SE";
                4:
                        return "S";
                5:
                        return "SW";
                6:
                        return "W";
                7:
                        return "NW";
                default:
                        return "";
        endcase
endfunction
