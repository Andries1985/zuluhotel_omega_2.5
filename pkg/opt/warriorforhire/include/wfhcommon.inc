// wfhcommon.inc - small shared helpers for the Warrior for Hire package.
use uo;
use os;

const WFH_PAYOUT_PACK := 0x0e75;	// a backpack; its own name because several includes define UOBJ_BACKPACK
const WFH_SKILL_CAP_TENTHS := 1300; // 130.0 displayed - hard ceiling regardless of the master's own skill

// A warrior can be brought back from each of its first ten deaths, by its heart or by the High
// Priest's memory (WFHBackup), and either way it spends the same life. The eleventh death is final.
const WFH_MAX_DEATHS := 10;

// Anatomy, Parry, Healing, Tactics, Archery, Swordsmanship, Macefighting, Fencing, Wrestling.
// (ZH3.0 also tracks Throwing, skill 57, which this shard does not have.)
function WfhTrackedSkills()

	return {1, 5, 17, 27, 31, 40, 41, 42, 43};

endfunction

// A heart revives only the death the owner's WFHBackup was written for. death.src stamps both with
// the corpse serial (WFHDeathId) and either revival erases the backup, so a heart held back from an
// earlier death cannot be used later to roll the death count back. Hearts written before the stamp
// existed carry none and are honoured only while no stamped backup has replaced them.
function IsCurrentWfhHeart(heart, owner)

	var heartid := GetObjProperty(heart, "WFHDeathId");
	var backupid := 0;
	var backup := GetObjProperty(owner, "WFHBackup");
	if (backup)
		backupid := backup["WFHDeathId"];
	endif

	if (!heartid and !backupid)
		return 1;
	endif
	return (heartid and heartid == backupid);

endfunction

// Kills a warrior its owner has given up. AbandonedByOwner tells death.src not to drop a heart
// or write a backup; guardkill makes it destroy the corpse at once, after escrowing the gear
// (EscrowWarriorForHireGear), so the owner can recover it from the High Priest for 90 days.
function AbandonWarrior(warriorObj)

	SetObjProperty(warriorObj, "AbandonedByOwner", 1);
	SetObjProperty(warriorObj, "guardkill", 1);
	warriorObj.kill();

endfunction

// A hired Warrior for Hire's leftover gear is parked in escrow (recoverable through the High
// Priest for 90 days) instead of vanishing with the corpse. Ported from ZH3.0. The datafile work
// happens in :warriorforhire:wfhescrowsave so the caller stays light. Called by corpsedecay.src
// when a warrior's corpse decays, and by death.src for a corpse destroyed on the spot - a warrior
// released or given up by its owner, or one killed by the guards - which never reaches decay.
function EscrowWarriorForHireGear( corpse )

	if( GetObjProperty( corpse, "npctemplate" ) != "warriorforhire" )
		return;
	endif
	var ownerserial := CInt( GetObjProperty( corpse, "master" ) );
	if( !ownerserial )
		return;
	endif
	var rootitems := EnumerateItemsInContainer( corpse, ENUMERATE_ROOT_ONLY );
	if( !len( rootitems ) )
		return;
	endif

	var payout := CreateItemAtLocation( corpse.x, corpse.y, corpse.z, WFH_PAYOUT_PACK, 1, corpse.realm );
	if( !payout )
		return;
	endif
	foreach item in rootitems
		MoveItemToContainer( item, payout );
	endforeach

	Start_Script( ":warriorforhire:wfhescrowsave", { payout, ownerserial, corpse.serial } );

endfunction
