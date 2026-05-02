// omegacache_utils.inc — Shared Omega Cache utilities
//
// Lightweight include for DataFile name convention, open/create/close,
// and empty-check. Used by: omegacache.inc, sign.src, resourcemanager.inc,
// loadout scripts. No heavy dependencies.

use uo;
use cfgfile;
use datafile;

const OMEGACACHE_OBJTYPE := 0xDF0A;
const OMEGACACHE_DEED_OBJTYPE := 0xDF0B;

// Returns the maximum number of Omega Cache Containers allowed for a house type.
// Centralised lookup — used by both AssignDefaultContainers and GetMaxProps.
function GetMaxOmegaCacheForHouse(house_objtype)
	case(house_objtype)
		// Small houses (1 secure) — 1 cache
		0x6064: return 1;
		0x6066: return 1;
		0x6068: return 1;
		0x606A: return 1;
		0x606C: return 1;
		0x606E: return 1;
		0x6070: return 1;
		0x6072: return 1;
		// Medium houses (3-4 secures) — 1 cache
		0x6074: return 1;
		0x608D: return 1;
		0x60A0: return 1;
		0x60A2: return 1;
		// Large houses (5-8 secures) — 2 caches
		0x6076: return 2;
		0x6078: return 2;
		0x608C: return 2;
		0x6098: return 2;
		0x609C: return 2;
		0x609A: return 2;
		0x609E: return 2;
		0x6096: return 2;
		// Mansion (15 secures) — 3 caches
		0x607A: return 3;
		// Castle (20 secures) — 3 caches
		0x607C: return 3;
		// Fortress (25 secures) — 3 caches
		0x607E: return 3;
		// Keep — 3 caches
		0x6BB8: return 3;
		// Custom multi-houses — scale with secures
		0xA3E0: return 1;   // 3 secures
		0xA3E1: return 2;   // 6 secures
		0xA3E2: return 2;   // 12 secures
		0xA3E3: return 2;   // 11 secures
		0xA3E4: return 2;   // 14 secures
		0xA3E5: return 2;   // 6 secures
		0xA3E6: return 2;   // 7 secures
		0xA3E7: return 3;   // 22 secures
		0xA3E8: return 2;   // 6 secures
		0xA3E9: return 2;   // 10 secures
		0xA3EA: return 2;   // 8 secures
		0xA3EB: return 2;   // 11 secures
		0xA3EC: return 3;   // 20 secures
		0xA3ED: return 2;   // 15 secures
		0xA3EE: return 2;   // 12 secures
		0xA3EF: return 2;   // 8 secures
		0xA3F0: return 2;   // 13 secures
		default: return 1;  // unknown house type — safe default
	endcase
endfunction

// Opens (or creates) the Omega Cache DataFile for a house.
// Returns the DataFile handle, or 0 on failure.
function OpenOmegaCacheStore(house_serial)
	if(!house_serial)
		return 0;
	endif

	var dfname := ":omegacache:house_" + house_serial;
	var df := OpenDataFile(dfname);
	if(!df)
		df := CreateDataFile(dfname, DF_KEYTYPE_STRING);
	endif
	return df;
endfunction

// Unloads the Omega Cache DataFile for a house from memory.
function CloseOmegaCacheStore(house_serial)
	UnloadDataFile(":omegacache:house_" + house_serial);
endfunction

// Returns 1 if the house's Omega Cache has no stored items, 0 otherwise.
// Returns 1 if the DataFile doesn't exist (no items = empty).
function IsOmegaCacheEmpty(house_serial)
	var df := OpenOmegaCacheStore(house_serial);
	if(!df)
		return 1;
	endif
	var keys := df.Keys();
	return (keys.size() == 0);
endfunction
