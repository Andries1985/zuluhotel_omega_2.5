use os;
use uo;

Function FindCity(me)

    if ( me.x>=1244 and me.y>=1530 and me.x<=1704 and me.y<=1758)
      return "Britain";

    elseif( me.x>=5272 and me.x<=5310 and me.y>=1160 and me.y<=1190)
      return "Jail";

    elseif( me.x>=1385 and me.x<=1414 and me.y>=3729 and me.y<=3758)
      return "Fighting Arena";

    elseif( me.x>=4094 and me.y>=3073 and me.x<=4868 and me.y<=3993)
      return "Isle of the Avatar";

    elseif( me.x>=3839 and me.x<=4309 and me.y>=167 and me.y<=773)
      return "Ice Isle";

    elseif ( me.x>=4228 and me.y>=774 and me.x<=4829 and me.y<=1546)
      return "Moonglow Isle";
    elseif ( me.x>=4379 and me.y>=1045 and me.x<=4487 and me.y<=1178)
      return "Moonglow";
    /*
    elseif ( me.x>=5137 and me.y>=3948 and me.x<=5196 and me.y<=4082)
    return "Delucia";
    elseif ( me.x>=5196 and me.y>=3923 and me.x<=5322 and me.y<=4085)
    return "Delucia";

    elseif ( me.x>=5638 and me.y>=3083 and me.x<=5799 and me.y<=3328)
    return "Papua";
    elseif ( me.x>=5799 and me.y>=3121 and me.x<=5822 and me.y<=3327)
    return "Papua";
    */
    elseif ( me.x>=5132 and me.y>=1754 and me.x<=5161 and me.y<=1782)
      return "Star Room";

    elseif ( me.x>=1294 and me.y>=3664 and me.x<=1506 and me.y<=3886)
      return "Jhelom";

    elseif ( me.x>=494 and me.y>=926 and me.x<=600 and me.y<=1034)
      return "Yew";

    elseif ( me.x>=590 and me.y>=796 and me.x<=670 and me.y<=886)
      return "Empath Abbey";

    elseif ( me.x>=2394 and me.y>=370 and me.x<=2624 and me.y<=632)
      return "Minoc";

    elseif ( me.x>=1794 and me.y>=2638 and me.x<=2136 and me.y<=2902)
      return "Trinsic";

    elseif ( me.x>=538 and me.y>=2106 and me.x<=686 and me.y<=2274)
      return "Skara Brae";

    elseif ( me.x>=3646 and me.y>=2040 and me.x<=3814 and me.y<=2280)
      return "Magincia";

    elseif ( me.x>=3588 and me.y>=2446 and me.x<=3762 and me.y<=2690)
      return "Ocllo";

    elseif ( me.x>=2612 and me.y>=2082 and me.x<=2772 and me.y<=2256)
      return "Buccaneers Den";

    elseif ( me.x>=3496 and me.y>=1062 and me.x<=3808 and me.y<=1400)
      return "Nujelm";

    elseif ( me.x>=2794 and me.y>=634 and me.x<=3026 and me.y<=1000)
      return "Vesper";

    elseif ( me.x>=2206 and me.y>=1116 and me.x<=2290 and me.y<=1236)
      return "Cove";

    elseif ( me.x>=6912 and me.y>=256 and me.x<=7167 and me.y<=511)
      return "Elven";

    elseif ( me.x>=5128 and me.y>=256 and me.x<=5375 and me.y<=511)
      return "Randorin";

    elseif ( me.x>=5120 and me.y>=0 and me.x<=5374 and me.y<=254)
      return "Wind"; 

    elseif ( me.x>=6457 and me.y>=15 and me.x<=6706 and me.y<=245)
      return "Gemstone Cave";

    elseif ( me.x>=6144 and me.y>=0 and me.x<=6398 and me.y<=254)
      return "Raided Town";

    elseif ( me.x>=5121 and me.y>=1810 and me.x<=5368 and me.y<=2061)
      return "Cave";

    elseif ( me.x>=5633 and me.y>=1762 and me.x<=5947 and me.y<=2047)
      return "Solen Hive";

    elseif ( me.x>=5494 and me.y>=1641 and me.x<=5541 and me.y<=1688)
      return "Arena";

    elseif ( me.x>=5914 and me.y>=910 and me.x<=6143 and me.y<=1061)
      return "Exhibit Area";

    elseif ( me.x>=6027 and me.y>=845 and me.x<=6072 and me.y<=890)
      return "Vendor Market";
    elseif ( me.x>=5921 and me.y>=845 and me.x<=5966 and me.y<=890)
      return "Vendor Market";

    elseif ( me.x>=6013 and me.y>=713 and me.x<=6039 and me.y<=739)
      return "Gate Room";

    elseif ( me.x>=6097 and me.y>=607 and me.x<=6123 and me.y<=625)
      return "Race Room";

    elseif ( me.x>=5443 and me.y>=1581 and me.x<=5475 and me.y<=1621)
      return "Wedding Chapel";

    elseif ( me.x>=5427 and me.y>=1082 and me.x<=5440 and me.y<=1094)
      return "Staff Room";

    elseif ( me.x>=6030 and me.y>=1333 and me.x<=6136 and me.y<=1412)
      return "Capture the Flag";

    elseif ( me.x>=6052 and me.y>=1280 and me.x<=6142 and me.y<=1347)
      return "Capture the Flag";

    elseif ( me.x>=5374 and me.y>=1079 and me.x<=5635 and me.y<=1283)
      return "Green Acres";
    elseif ( me.x>=5627 and me.y>=1021 and me.x<=6144 and me.y<=1283)
      return "Green Acres";
    elseif ( me.x>=5887 and me.y>=510 and me.x<=6144 and me.y<=1022)
      return "Green Acres";

    elseif ( me.x>=5378 and me.y>=256 and me.x<=5630 and me.y<=511)
      return "No-PvP Dungeon";

    elseif ( me.x>=5120 and me.y>=514 and me.x<=6128 and me.y<=1992)
      return "Dungeons";

    elseif ( me.x>=5923 and me.y>=1573 and me.x<=6024 and me.y<=1694)
      return "Dungeons";
	    
    Endif

return "Zulu";
EndFunction
