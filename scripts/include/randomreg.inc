  
function make_random_reg()
  
    var thereg := Random(regs.length());
    CreateItemAtLocation( me.x, me.y, me.z, regs[thereg], Random(2) + 1, me.realm );
endfunction