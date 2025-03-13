use util;
use guilds;

include ":gumps:include/gumps";
include "guildconstants";

/////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// FindSelectedHouse() - Find the selected house to make the guild house.
//
// 50 is for the house that the player is standing in.  50+1 for 1st selection, 50+2
// for second selected etc.
//
// returns a house ref or 0
/////////////////////////////////////////////////////////////////////////////////////
function FindSelectedHouse(who, data);

	// Player selected 'standing in house' option
	if (50 in data.keys)
		if (GetObjProperty(who.multi, "ownerserial") != who.serial)
			return 2;
		else
			return who.multi;
		endif
	endif

	var i := 51;

	// Loop from 51 to 56, 
	repeat
		if (i in data.keys)
			return SystemFindObjectBySerial(GetObjProperty(who, "Houses")[i-50]);
		endif
		i := i + 1;
	until (i >= 55);

	return 0;

endfunction

///////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// DisbandGuild() - Deletes a guild, deals with sorting everything
//
// TODO: * All requests/recruits/pending alliances/enemies
//	 controlled on the sending (the one being deleted) guild ref, not players
//	
//
//	Dreams 
//
//	Removed requests/pending alliances/wars
///////////////////////////////////////////////////////////////////////////////////
function DisbandGuild(guild)

	// remove colour from guild house
	var guildhouse := guild.getprop("GuildHouse");
	//SetGuildHouseColour(guildhouse, 0);

	// remove guild from the house
	EraseObjProperty(guildhouse, "GuildHouse");

	// remove all players from guild. Remove players titles
	foreach person in (guild.members)
		SendSysMessage(person, "You are no longer in a guild.", 3, 67);
		RemoveMember(person);
	endforeach

	// remove applicants requests
		guild.eraseprop("JoinGuildRequest");
		guild.eraseprop("BanReq");

	// remove ally/enemy guilds
	foreach other in ListGuilds()
		ClearAllRequests(guild, other);
		guild.removeallyguild(other);
		guild.removeenemyguild(other);
		// This most likely isn't needed, just in case.
		other.removeallyguild(guild);
		other.removeenemyguild(guild);
	endforeach

	foreach person in EnumerateOnlineCharacters()
		SendSysMessage(person, guild.getprop("GuildName")+" has been disbanded!", 3, 43);
	endforeach

	DestroyGuild(guild);

endfunction

///////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// RemoveMember() - Removes specified member from guild
//
///////////////////////////////////////////////////////////////////////////////////
function RemoveMember(who)

	who.guild.removemember(who);
	who.title_guild := "";

	// unequip uniforms
	foreach item in ListEquippedItems( who )
		if (GetObjProperty(item, GUILD_UNIFORM) || GetObjProperty(item, "Entwined") )	
			MoveItemToContainer( item, who.backpack);
		endif

		if (GetObjProperty(item, "Entwined"))
			UnEntwine(who, item);
		endif

	endforeach

endfunction


///////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// RemoveGuildMaster() - Removes the guildmaster.  If he's online, asks him to choose
// a successor
//
///////////////////////////////////////////////////////////////////////////////////
function ChangeGuildMaster(who)

	var guild := who.guild;

	if (!who.connected)
		guild.setprop("GuildMaster", guild.members[1].serial);

		foreach member in (guild.members)
			if (member.serial != who.serial)
				SendSysMessage(member, member.name+" is now the Guild Master!", 3, 53);
			else
				SendSysMessage(member, "You are now the Guild Master!", 3, 53);
			endif
		endforeach
		return 1;
	else
		var gump := GFCreateGump(20, 20, 400, 350);
		GFResizePic(gump, 20, 20, 9260, 400, 350);
		GFResizePic(gump, 30, 30, 9350, 380, 290);
		GFResizePic(gump, 30, 321, 9350, 380, 39);

		GFResizePic(gump, 118, 35, 9200, 200, 25);
		GFTextLine(gump, 135, 37, 53, "Choose the new Guild Leader");

		GFTextLine(gump, 140, 330, 0, "Hand over guild leadership?");
		GFAddBUtton(gump, 50, 330, 2128, 2129, 1, 1000);
		GFAddBUtton(gump, 335, 330, 2119, 2120, 1, 0);

		GFTextline(gump, 100, 80, 0, "Choose a member at random");
		GFRadiobutton(gump, 60, 80, 210, 211, 1, 999); 

		var y := 120;
		var maxpage := 7;
		var count := 0;
		var page := 2;
		GFPage(gump, 1);
		GFTextLine(gump, 200, 295, 53, "Page 1");
		foreach person in (guild.members)
			if (count >= maxpage)
				GFAddButton(gump, 260, 296, 5601, 5605, 0, page);
				GFPage(gump, page);
				page := page + 1;
				GFTextLine(gump, 200, 295, 53, "Page "+(page-1));
				GFAddButton(gump, 163, 296, 5603, 5607, 0, page-2);
				y := 120;
			endif

			if (person != who)
				GFTextline(gump, 100, y, 0, person.name);
				GFRadiobutton(gump, 60, y, 210, 211, 0, _person_iter); 
				y := y + 25;
				count := count + 1;
			endif
		endforeach

		var data := GFSendGump(who, gump);

		if (data[0] == 1000)

			var newmaster;
			// selected a member
			if (data.keys[2] != 999)
				newmaster := guild.members[data.keys[2]];

			// selected random member
			elseif (data.keys[2] > 1)
				repeat
				newmaster := guild.members[RandomInt(guild.members.size())+1];
				until (newmaster.serial != who.serial);			
			endif

			guild.setprop("GuildMaster", newmaster.serial);
			SendSysMessage(who, newmaster.name+" is now the Guild Master.", 3, 43);

			foreach member in (guild.members)
				if (member.serial != who.serial && member.serial != newmaster.serial )
					SendSysMessage(member, newmaster.name+" is now the Guild Master!", 3, 53);
				endif
				if (member.serial == newmaster.serial)	
					SendSysMessage(member, "You are now the Guild Master!", 3, 53);
				endif
			endforeach
			return 1;
		endif
	endif

	return 0; 

endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// NameUniform() - Names the uniform on creation
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
function NameUniform(item)

	var name := SplitWords(item.desc);

	var newname := "";
	//not equipped
	if (!item.layer)
		foreach part in name
			if (_part_iter == 1 && lower(part) == "a")
				newname := "a Guild";
			elseif (_part_iter == 1 && lower(part) != "a")
				newname :=  "Guild "+part;
			else
				newname := newname+" "+part;
			endif
		endforeach
	SetName(item, newname);
	endif

endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// EquipGuildUniform() - Set's the uniform colour on equip
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
function EquipGuildUniform(who, item)

	if(GetObjProperty(item, "EquippedAlready"))
		return 1;
	endif

	var guild := who.guild;
	var colour := guild.getprop(GUILD_COLOUR);

	if (!guild)
		SendSysMessage(who, "You need a guild to wear a uniform!");
		return 0;
	elseif (!colour)
		SendSysMessage(who, "Your guild needs a guild colour before you can wear that!");
		return 0;
	endif

	PlaySoundEffectPrivate(who, 0x457, who);
	PlayObjectCenteredEffectEx( who, 14186, 10, 10,colour, 0, 7);
	item.color := colour;

	return 1;

endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// UnEquipGuildUniform() - Set's the uniform colour on unequip
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
function UnEquipGuildUniform(who, item)

	if (GetObjProperty(item, "Entwined"))
		item.color := GetObjProperty(item, "OriginalColour");
		if (who.dead)
			UnEntwine(who, item);
		endif
		return 1;
	endif

	if (!GetObjProperty(item, GUILD_UNIFORM))
		return 1;
	endif

	item.color := 0;

	return 1;

endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// UnEntwine() - does just that
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
function UnEntwine(who, uniform)

	var name := SplitWords(uniform.desc);
	var newname, count := 0;

	foreach thing in name
		if(count == 0)
			newname := thing;
			count := 1;
		elseif ( thing != "[Entwined]" )
			newname := newname+" "+thing;
		endif
	endforeach

	SetName(uniform, newname);
	uniform.color := GetObjProperty(uniform, "OriginalColour");

	var uni := SystemFindObjectBySerial(GetObjProperty(uniform, "Entwined"));

	if (!uni)
		Broadcast("This is odd, your uniform is gone!, page staff!",3, 33);
		syslog(who.name+"/"+who.serial+" This is odd, your uniform is gone!, page staff!");
		return 0;
	endif

	EraseObjProperty(uniform, "Entwined");
	EraseObjProperty(uniform, "OriginalColour");
	uniform.usescript := "";

	uni.movable := 1;
	uni.invisible := 0;
	uni.graphic := GetObjProperty(uni, "Graphic");
	MoveItemToContainer( uni, who.backpack );
	endfunction

	function UpdateUniformColour(guild)

	// unequip uniforms
	foreach who in (guild.members)
		foreach item in ListEquippedItems( who )
			if (GetObjProperty(item, GUILD_UNIFORM) || GetObjProperty(item, "Entwined") )	
				item.color := guild.getprop(GUILD_COLOUR);
			endif
		endforeach
	endforeach

endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dreams
//
// Guild Relation Stuff
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

function RequestStatus(who, thatguild, relation)

	var relationlist := thatguild.getprop(relation);
	var glist := who.guild.getprop(relation);

	if(!relationlist)
		relationlist := {};
	endif
	
	if(glist)
		foreach guild in glist
			if(guild == thatguild.getprop("GuildName"))
				SendSysMessage(who, "You already got a pending request from that guild.");
				return;
			endif
		endforeach
	endif
	relationlist.append(who.guild.getprop("GuildName"));
	thatguild.setprop(relation, relationlist);

endfunction

function CancelRequest(whoguild, thatguild, relation)

	var relationlist := thatguild.getprop(relation);
	var newlist := {};

	foreach guilds in relationlist
		if(guilds != whoguild.getprop("GuildName"))
			newlist.append(guilds);
		endif
	endforeach

	thatguild.setprop(relation, newlist);

endfunction

function ClearAllRequests(who, thatguild)

	CancelRequest(who.guild, thatguild, "RequestWar");
	CancelRequest(who.guild, thatguild, "RequestAlly");
	CancelRequest(who.guild, thatguild, "RequestPeace");
	CancelRequest(who.guild, thatguild, "RequestBreakup");

	CancelRequest(thatguild, who.guild, "RequestWar");
	CancelRequest(thatguild, who.guild, "RequestAlly");
	CancelRequest(thatguild, who.guild, "RequestPeace");
	CancelRequest(thatguild, who.guild, "RequestBreakup");

endfunction

function CheckStatus(whoguild, thatguild, relation)

	var CheckArray := thatguild.getprop(relation);

	if(CheckArray)
		foreach guilds in CheckArray
	    	if(guilds == whoguild.getprop("GuildName"))
				return 1;
			endif
		endforeach
	endif

	return 0;

endfunction

function WarAllyGuilds(who, ally := 0)

	var newlist := {};

	foreach member in ListGuilds()
		if(ally == 1)
			if(member.IsAllyGuild(who.guild))
				newlist.append(member);
			endif
		else
			if(member.IsEnemyGuild(who.guild))
				newlist.append(member);
			endif
		endif
	endforeach

	return newlist;

endfunction

function RelationStatusGuilds(whoguild, relation)

	var relationlist := whoguild.getprop(relation);
	var newlist := {};

	if(!relationlist)
		return 0;
	endif

	foreach member in relationlist
			newlist.append(member);
	endforeach

	return newlist;

endfunction


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dreams
//
// Join Requests / Acceptance of Requests
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
function ClearJoinGuildRequest(who)

	foreach guild in ListGuilds()
		if(guild.getprop("GuildName") == who.getprop("JoinGuildRequest"))
			var relationlist := guild.getprop("JoinGuildRequest");
			if(relationlist.size() <= 1)
				guild.eraseprop("JoinGuildRequest");
				who.eraseprop("JoinGuildRequest");
				return 0;
			endif

			var newlist := {};
			foreach member in relationlist
				if(member != who.serial)
					newlist.append(member);
				endif
			endforeach
			guild.setprop("JoinGuildRequest", newlist);
			who.eraseprop("JoinGuildRequest");
		endif
	endforeach

endfunction

function RequestJoinGuild(who, guild)

	var relationlist := guild.getprop("JoinGuildRequest");

	who.setprop("JoinGuildRequest", guild.getprop("GuildName"));
	
	if(!relationlist)
		relationlist := {};
	endif

	relationlist.append(who.serial);
	guild.setprop("JoinGuildRequest", relationlist);

endfunction

function BanReq(who, guild)

	var banlist := guild.getprop("BanReq");
	
	if(!banlist)
		banlist := {};
	endif

	banlist.append(who.serial);
	guild.setprop("BanReq", banlist);

endfunction