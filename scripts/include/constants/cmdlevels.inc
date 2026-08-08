/////////////////////////////////////////////////////////////////////
// CmdLevel.inc
//
// Contain the command level names
//
// Values must match this shard's actual ladder, defined by the CmdLevel
// blocks in config/cmds.cfg and confirmed live against the promote/demote
// tool at scripts/textcmd/test/makestaff.src (which labels level 5
// "Developer" -- cmds.cfg itself names that same directory "Test"). There
// is no separate "Shard Mom" tier and no level 6 on this shard -- only 6
// levels total, numbered 0-5.
/////////////////////////////////////////////////////////////////////


const CMDLEVEL_PLAYER		:= 0;
const CMDLEVEL_COUNSELOR	:= 1;
const CMDLEVEL_SEER		:= 2;
const CMDLEVEL_GAME_MASTER	:= 3;
const CMDLEVEL_ADMINISTRATOR	:= 4;
const CMDLEVEL_DEVELOPER	:= 5; // aka "Test" (config/cmds.cfg directory name)
