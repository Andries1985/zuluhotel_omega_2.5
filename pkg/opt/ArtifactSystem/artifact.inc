use os;
use uo;
use storage;
include "include/objtype";
include "include/client";
include "include/random";


const ARTIFACT_BOX := "artifactbox";
const ARTIFACT_FONT_COLOR := 43;
const ARTIFACT_EXPIRE_PROP := "#ArtifactExpireAt";

function getItemArtifactBox()
	
	var artifact_list := EnumerateItemsInContainer(getArtifactBox());
	if (!artifact_list || !artifact_list.Size())
		return 0;
	endif
	var rand := 1 + RandomInt(artifact_list.Size());

	return artifact_list[rand];
endfunction

function toArtifactBox(item)
	// Once recycled, relic expiry should reset on next pull from the artifact box.
	EraseObjProperty(item, ARTIFACT_EXPIRE_PROP);
	EraseObjProperty(item, "RelicUID");
	MoveItemToContainer(item, getArtifactBox());
endfunction

function getArtifactBox()
	var worldbank := OpenWorldBank();
	var artifact_box := FindRootItemInStorageArea( worldbank, ARTIFACT_BOX );
	if (!artifact_box)
		artifact_box := CreateRootItemInStorageArea( worldbank, ARTIFACT_BOX, UOBJ_BANKBOX);
	endif
	return artifact_box;
endfunction

function openArtifactBox(you)
	SendOpenSpecialContainer(you, getArtifactBox());
endfunction

function OpenWorldBank()
	var bank := FindStorageArea( "World Bank" );
	if (!bank)
		bank := CreateStorageArea( "World Bank" );
	endif
	if (!bank)
		syslog( "Unable to open or create world bank!" );
	endif
	return bank;
endfunction

function SweepExpiredArtifacts(scope)
	var now := ReadGameClock();
	var scanned := 0;
	var swept := 0;
	var errors := 0;

	var areas := StorageAreas();
	if (!areas)
		var msg := "[ArtifactSweep " + scope + "] no storage areas returned.";
		syslog(msg);
		print(msg);
		return 0;
	endif

	foreach area in areas
		if (!area)
			continue;
		endif

		foreach storage_item in EnumerateItemsInContainer(area)
			if (!storage_item)
				continue;
			endif
			if (storage_item.isa(POLCLASS_CONTAINER))
				SweepExpiredArtifactsInContainer(storage_item, now, scanned, swept, errors);
			endif
		endforeach
	endforeach

	var summary := "[ArtifactSweep " + scope + "] scanned=" + scanned + " swept=" + swept + " errors=" + errors;
	syslog(summary);
	print(summary);
	return swept;
endfunction

function SweepExpiredArtifactsInContainer(container, now, byref scanned, byref swept, byref errors)
	if (!container)
		return;
	endif

	foreach item in EnumerateItemsInContainer(container)
		if (!item)
			continue;
		endif

		scanned := scanned + 1;
		var should_recurse := 1;

		if (GetObjProperty(item, "Artifact"))
			var expire_at := CInt(GetObjProperty(item, ARTIFACT_EXPIRE_PROP));
			if (expire_at && expire_at <= now)
				DestroyItem(item);
				swept := swept + 1;
				should_recurse := 0;
			endif
		endif

		if (should_recurse && item.isa(POLCLASS_CONTAINER))
			SweepExpiredArtifactsInContainer(item, now, scanned, swept, errors);
		endif
	endforeach
endfunction