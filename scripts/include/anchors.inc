use npc;
use cfgfile;

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