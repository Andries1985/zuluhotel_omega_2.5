// wfhcommon.inc - small shared helpers for the Warrior for Hire package.
use uo;

// Kills a warrior its owner has given up. AbandonedByOwner tells death.src not to drop a heart
// or write a backup, and corpsedecay.src not to escrow the gear (it stays on the corpse).
function AbandonWarrior(warriorObj)

	SetObjProperty(warriorObj, "AbandonedByOwner", 1);
	SetObjProperty(warriorObj, "guardkill", 1);
	warriorObj.kill();

endfunction
