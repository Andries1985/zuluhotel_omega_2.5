use os;
use uo;

Function CheckCity(item)

  if(item.realm == "britannia")
    if ( item.x>=1244 and item.y>=1530 and item.x <= 1704 and item.y <= 1758)
        return 1; // Britain
      elseif ( item.x>=4379 and item.y>=1045 and item.x <= 4487 and item.y <= 1178)
        return 1; // Moonglow
      elseif ( item.x>=5638 and item.y>=3083 and item.x<=5799 and item.y<=3328)
        return 1; // Papua
      elseif ( item.x>=5799 and item.y>=3121 and item.x<=5822 and item.y<=3327)
        return 1; // Papua
      elseif ( item.x>=5137 and item.y>=3948 and item.x<=5196 and item.y<=4082)
        return 1; // Delucia
      elseif ( item.x>=5196 and item.y>=3923 and item.x<=5322 and item.y<=4085)
        return 1; // Delucia
      elseif ( item.x>=1294 and item.y>=3664 and item.x <= 1506 and item.y <= 3886)
        return 1; // Jhelom
      elseif ( item.x>=494 and item.y>=926 and item.x <= 600 and item.y <= 1034)
        return 1; // Yew
      elseif ( item.x>=590 and item.y>=796 and item.x <= 670 and item.y <= 886)
        return 1; // Empath Abbey
      elseif ( item.x>=2394 and item.y>=370 and item.x <= 2624 and item.y <= 632)
        return 1; // Minoc
      elseif ( item.x>=1794 and item.y>=2638 and item.x <= 2136 and item.y <= 2902)
        return 1; // Trinsic
      elseif ( item.x>=538 and item.y>=2106 and item.x <= 686 and item.y <= 2274)
        return 1; // Skara Brae
      elseif ( item.x>=3646 and item.y>=2040 and item.x <= 3814 and item.y <= 2280)
        return 1; // Magincia
      elseif ( item.x>=3588 and item.y>=2446 and item.x <= 3762 and item.y <= 2690)
        return 1; // Occlo
      elseif ( item.x>=2612 and item.y>=2082 and item.x <= 2772 and item.y <= 2256)
        return 1; // Buccaneers Den
      elseif ( item.x>=3496 and item.y>=1062 and item.x <= 3808 and item.y <= 1400)
        return 1; // Nujelm
      elseif ( item.x>=2794 and item.y>=634 and item.x <= 3026 and item.y <= 1000)
        return 1; // Vesper
      elseif ( item.x>=2206 and item.y>=1116 and item.x <= 2290 and item.y <= 1236)
        return 1; // Cove
      elseif ( item.x>=5120 and item.y>=0 and item.x <= 5374 and item.y <= 254)
        return 1; // Wind
      endif
  elseif(item.realm == "britannia_alt")
    return 1;
  elseif(item.realm == "malas")
    return 1;
  elseif(item.realm == "ilshenar")
    return 1;
  elseif(item.realm == "tokuno")
    if(item.x >= 624 and item.y >= 1160 and item.x <= 863 and item.y <= 1447)
      return 1;
    endif
  elseif(item.realm == "termur")
    return 1;
  endif

  return 0;
endfunction
