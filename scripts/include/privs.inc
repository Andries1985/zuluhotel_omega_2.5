use uo;

function GetDefaultPrivs(who)

var privs := {};
var cmdlevel := who.cmdlevel;

if (who.acct.defaultcmdlevel > cmdlevel)
	cmdlevel := who.acct.defaultcmdlevel;
endif

if (cmdlevel > 0) // count +
	privs.append("seeghosts");
	privs.append("seehidden");
	privs.append("seeinvisitems"); 
	privs.append("invul");
	privs.append("hearghosts"); 
	privs.append("freemove");
endif

if (cmdlevel > 1) // seer +
	privs.append("renameany");
	privs.append("ignoredoors");
	privs.append("clotheany");
endif

if (cmdlevel > 2) // gm +
	privs.append("attackhidden");
	privs.append("hiddenattack");
endif

if (cmdlevel > 3) // admin +
	privs.append("dblclickany");
	privs.append("losany");
	privs.append("moveanydist");
endif

if (cmdlevel > 4) // test
	privs.append("all");
	privs.append("moveany");
	privs.append("plogany");
	privs.append("firewhilemoving");
endif

return privs;

endfunction

function AlterPrivsBase(who, old, new)

var eligibleprivs := GetDefaultPrivs(who);
var currprivs := who.privileges();

// promotion, keep any bonus privs he may have
if (old < new)

	foreach priv in eligibleprivs
		GrantPrivilege(who, priv);
			// councy, enable them all
		if (new == 1)
			who.enable(priv);
		endif
	endforeach

// demotion, set all privs to command level dafult
else

	foreach priv in (currprivs.keys())
		who.disable(priv);
		RevokePrivilege(who, priv);
	endforeach

	foreach priv in eligibleprivs
		GrantPrivilege(who, priv);
	endforeach

endif

endfunction