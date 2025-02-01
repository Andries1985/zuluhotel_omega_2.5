use uo;

function createArena()
	var i;
	for(i := 0;i<20; i:=i+1)
		CreateItemAtLocation( 5508+i, 1674, 0, 0x9A94, 1, "britannia" );
		CreateItemAtLocation( 5508, 1674-i, 0, 0x9A94, 1, "britannia"  );

		if(i%2 == 0)
			CreateItemAtLocation( 5509, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5511, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5513, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5515, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5517, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5519, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5521, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5523, 1674-i, 0, 0x9A94, 1, "britannia"  );
			CreateItemAtLocation( 5525, 1674-i, 0, 0x9A94, 1, "britannia"  );
		endif
	endfor
	return;
endfunction