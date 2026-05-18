use os;
use uo;
use storage;
include "include/objtype";
include "include/client";
include "include/random";


const ARTIFACT_BOX := "artifactbox";
const ARTIFACT_FONT_COLOR := 43;

function getItemArtifactBox()
	
	var artifact_list := EnumerateItemsInContainer(getArtifactBox());
	var rand := 1 + RandomInt(artifact_list.Size());

	return artifact_list[rand];
endfunction

function toArtifactBox(item)
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