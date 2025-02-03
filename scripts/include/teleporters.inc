////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////
//  TELEPORTERS.INC - Places teleporters into the world.
//
//  This file is to be included into start.src and the function
//  CreateTeleporters() called from there.
//
//  Author: Ignatius 1999/05/27
//
//  TODO: walk thru all these and check em out.
//
//  Thanks to Raxxla for some tweaks/fixes!
//
//  ...modifiziert von Shinigami@gmx.net
//
//    (Die Fehlermeldung wurde sinnvoller gestaltet...)
//
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////

use uo;
use os;
//use cfgfile;

//
// System teleporters
//
// Teleporter locations were mainly taken from the UOX project.
// Credit is given where credit is due!
//
// Format:
// { fromX, fromY, fromZ, toX, toY, ToZ, fromR, toR },
//
// if startZ is -255 it should affect *all* Z coordinates on that spot,
// but for now the walkonscript gets the lowest walkable location.
//
var teleporters := {
    {6058,  272,  	-8,    	6095,   279,    44,     "britannia",    "britannia"},      // ctf1a
    {6058,  273,  	-8,    	6095,   279,    44,     "britannia",    "britannia"},      // ctf1b
    {6088,  274,  	58,    	6063,   278,    -22,    "britannia",    "britannia"},      // ctf2a
    {6089,  274, 	58,    	6063,   278,    -22,    "britannia",    "britannia"},      // ctf2b
    {1595, 	2489, 	20,		1601, 	2489, 	7,      "britannia",    "britannia"},      // spirit
	
// Dungeons
    {5242,  1007,   5,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
    {5243,  1007,   4,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
    {5244,  1007,   4,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
	{1175,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {1176,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {1177,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {5129,  909,    -28,    5142,   798,   	17,     "britannia",    "britannia"},  		// Destard_1_2_x[]
    {5130,  909,    -28,    5143,   798,   	17,     "britannia",    "britannia"},  		// Destard_1_2_x[]
    {5131,  909,    -28,    5144,   798,   	17,     "britannia",    "britannia"},  		// Destard_1_2_x[]
    {5132,  909,    -28,    5145,   798,   	17,     "britannia",    "britannia"},  		// Destard_1_2_x[]
    {5142,  797,    21, 	5129,   908,   	-23,    "britannia",    "britannia"}, 		// Destard_2_1_x[]
    {5143,  797,    21,   	5130,   908,   	-23,    "britannia",    "britannia"}, 		// Destard_2_1_x[]
    {5144,  797,    20,   	5131,   908,   	-23,    "britannia",    "britannia"}, 		// Destard_2_1_x[]
    {5145,  797,    20, 	5132,   908,   	-23,    "britannia",    "britannia"}, 		// Destard_2_1_x[]
    {5153,  808,    -25,    5134,   984,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
    {5153,  809,    -25,    5134,   985,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
    {5153,  810,    -25,    5134,   986,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
	{5153,  811,    -25,    5134,   987,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
	{5133,  984,    22,   	5152,   808,   	-19,    "britannia",    "britannia"}, 		// Destard_3_2_x[]
    {5133,  985,    22,   	5152,   809,   	-19,    "britannia",    "britannia"}, 		// Destard_3_2_x[]
    {5133,  986,    22,   	5152,   810,   	-19,    "britannia",    "britannia"}, 		// Destard_3_2_x[]
    {5133,  987,    22,   	5152,   811,   	-19,    "britannia",    "britannia"}, 		// Destard_3_2_x[]
    {1628,  3320,   0,      5899,   1410,   42,     "britannia",    "britannia"},       // Ranger_ent_1_x[]
    {1629,  3320,   0,      5900,   1410,   42,     "britannia",    "britannia"},       // Ranger_ent_1_x[]
    {5899,  1411,   45,     1628,   3321,   0,      "britannia",    "britannia"},       // Ranger_1_out_x[]
    {5900,  1411,   45,     1629,   3321,   0,      "britannia",    "britannia"},       // Ranger_1_out_x[]
	{5919,  1410,   -31,    5962,   1407,  	45,     "britannia",    "britannia"},       // Ranger_1_2_x[]
	{5919,  1411,   -31,    5962,   1407,  	45,     "britannia",    "britannia"},       // Ranger_1_2_x[]
	{5919,  1412,   -31,    5962,   1408,  	45,     "britannia",    "britannia"},       // Ranger_1_2_x[]
	{5919,  1413,   -32,    5962,   1409,  	45,     "britannia",    "britannia"},       // Ranger_1_2_x[]
	{5961,  1407,   45,    	5918,  	1411,   -30,    "britannia",    "britannia"},       // Ranger_2_1_x[]
	{5961,  1408,   45,    	5918,  	1412,   -31,    "britannia",    "britannia"},       // Ranger_2_1_x[]
	{5961,  1409,   45,    	5918,  	1413,   -33,    "britannia",    "britannia"},       // Ranger_2_1_x[]
    {5166,  245,    15,     1361,   889,   	1,      "britannia",    "britannia"},       // Wind_ent_out_x[]
    {1361,	883,    0,      5166,	244,    15, "britannia", "britannia"},          // Wind_out_x[]
    {5191,  152,    0,      1367,   890,   	0, "britannia", "britannia"},   		// Wind_exit[]
    {5200,  71,     17,     5211,   22,    	15, "britannia", "britannia"},  		// Wind_w_e_x[]
    {5217,  18,     15,     5204,   74,    	17, "britannia", "britannia"},  		// Wind_e_w_x[]
	{4110,  431,    5, 		5187,   638,    1, "britannia", "britannia"},     	    // Deceit_out_x[]
    {4111,  431,    5, 		5188,   638,    1, "britannia", "britannia"},     	    // Deceit_out_x[]
    {4112,  431,    5, 		5189,   638,    1, "britannia", "britannia"},     	    // Deceit_out_x[]
	{5186,  639,    0, 	 	4110,   432,    5, "britannia", "britannia"},     	    // Deceit_ent_out_x[]
	{5187,  639,    0,   	4111,   432,   	5, "britannia", "britannia"},   		// Deceit_ent_out_x[]
    {5188,  639,    0,   	4112,   432,   	5, "britannia", "britannia"},   		// Deceit_ent_out_x[]
    {5189,  639,    0,   	4113,   432,   	5, "britannia", "britannia"},   		// Deceit_ent_out_x[]
    {5216,  586,    -13,    5305,   533,   	6, "britannia", "britannia"},   		// Deceit_1_2_x[]
    {5217,  586,    -13,    5305,   533,   	6, "britannia", "britannia"},   		// Deceit_1_2_x[]
    {5218,  586,    -13,    5305,   533,   	6, "britannia", "britannia"},   		// Deceit_1_2_x[]
	{5304,  532,    7,      5216,   585,    -8, "britannia", "britannia"},   		// Deceit_2_1_x[]
	{5305,  532,    7,      5217,   585,    -8, "britannia", "britannia"},     	    // Deceit_2_1_x[]
	{5306,  532,    7,      5218,   585,    -8, "britannia", "britannia"},     	    // Deceit_2_1_x[]
    {5346,  578,    5,   	5137,   650,    6, "britannia", "britannia"},    		// Deceit_2_3_x[]
    {5137,  649,    5,      5346,   579,   	6, "britannia", "britannia"},   		// Deceit_3_2_x[]
    {5218,  762,    -33,   	5306,   652,    2, "britannia", "britannia"},     	    // Deceit_3_4_x[]
    {5219,  762,    -28,   	5305,   652,    2, "britannia", "britannia"},     	    // Deceit_3_4_x[]
    {5306,  650,    12,   	5218,   761,    -28, "britannia", "britannia"},   	    // Deceit_4_3_x[]
    {5305,  650,    12,   	5219,   761,    -28, "britannia", "britannia"},   	    // Deceit_4_3_x[]
    {512, 	1561, 	1, 		5394, 	126, 	0, "britannia", "britannia"},       	// Shame_out_x[]
    {513, 	1561, 	1, 		5395, 	126, 	0, "britannia", "britannia"},        	// Shame_out_x[]
    {514, 	1561, 	1, 		5396, 	126, 	0, "britannia", "britannia"},       	// Shame_out_x[]
	{5394,  127,    3,  	512,    1562,   0, "britannia", "britannia"},     	    // Shame_ent_out_x[]
    {5395,  127,    3,  	513,    1562,   0, "britannia", "britannia"},     	    // Shame_ent_out_x[]
    {5396,  127,    3,  	514,    1562,   0, "britannia", "britannia"},     	    // Shame_ent_out_x[]
    {5490,  19,     -25,   	5515,   11,     5, "britannia", "britannia"},   		// Shame_1_2_x[]
    {5514,  10,     5,  	5489,   19,     -25, "britannia", "britannia"},   	    // Shame_2_1_x[]
    {5604,  102,    5,   	5514,   148,    21, "britannia", "britannia"},    	    // Shame_2_3_x[]
    {5514,  147,    25,   	5604,   103,    5, "britannia", "britannia"},     	    // Shame_3_2_y[]
    {5538,  170,    5,	   	5513,   175,    5, "britannia", "britannia"},     	    // Shame_3_4_x[]
    {5513,  176,    5,   	5537,   170,    1, "britannia", "britannia"},     	    // Shame_4_3_x[]
    {5507,  162,    5,  	5875,   18,     5, "britannia", "britannia"},    		// Shame_4_5_x[]
	{5507,  187,    0,   	5404,   43,     30, "britannia", "britannia"},     	    // Shame_4_1_x[]
	{5875,  19,     -5,   	5507,   161,    5, "britannia", "britannia"},     	    // Shame_5_4_x[]
	{5731,  90,     5,   	5741,   52,     0, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5741,  53,     0,   	5731,   91,     2, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5738,  48,     5,   	5718,   62,     0, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5711,  55,     0,   	5738,   47,     2, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5736,  48,     5,   	5702,   61,     0, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5695,  55,     0,   	5735,   47,     2, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5736,  50,     5,   	5702,   86,     0, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5695,  79,     0,   	5735,   50,     2, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5738,  50,     5,   	5726,   78,     0, "britannia", "britannia"},     	    // Shame_5_5_isl[]
	{5719,  71,     0,   	5737,   50,     5, "britannia", "britannia"},     	    // Shame_5_5_isl[]
    {5588,  630,    30,     1298,   1081,   0, "britannia", "britannia"},     	    // Despise_ent_out_x[]
    {5588,  631,    30,     1298,   1081,   0, "britannia", "britannia"},     	    // Despise_ent_out_x[]
    {5588,  632,    30,     1298,   1081,   0, "britannia", "britannia"},     	    // Despise_ent_out_x[]
    {1297,  1080,   0,      5587,   631,    30, "britannia", "britannia"},    	    // Despise_out_x[]
    {1297,  1081,   0,      5587,   631,    30, "britannia", "britannia"},    	    // Despise_out_x[]
    {1297,  1082,   0,      5587,   631,    30, "britannia", "britannia"},    	    // Despise_out_x[]
    {5573,  628,    42,   	5501,   570,    57, "britannia", "britannia"},    	    // Despise_ent_1_x[]
    {5573,  629,    42,   	5501,   570,    57, "britannia", "britannia"},    	    // Despise_ent_1_x[]
    {5573,  630,    42,   	5501,   570,    57, "britannia", "britannia"},    	    // Despise_ent_1_x[]
    {5504,  569,    46,     5576,   629,    30, "britannia", "britannia"},    	    // Despise_1_ent_x[]
    {5504,  570,    46,     5576,   629,    30, "britannia", "britannia"},    	    // Despise_1_ent_x[]
    {5504,  571,    46,     5576,   629,    30, "britannia", "britannia"},    	    // Despise_1_ent_x[]
    {5521,  672,    27,     5575,   633,    27, "britannia", "britannia"},    	    // Despise_2_ent_x[]
    {5521,  673,    27,     5575,   633,    27, "britannia", "britannia"},    	    // Despise_2_ent_x[]
    {5521,  674,    27,     5575,   633,    27, "britannia", "britannia"},    	    // Despise_2_ent_x[]
    {5573,  632,    22,   	5519,   673,    20, "britannia", "britannia"},    	    // Despise_ent_2_x[]
    {5573,  633,    22,   	5519,   673,    20, "britannia", "britannia"},    	    // Despise_ent_2_x[]
    {5573,  634,    22,   	5519,   673,    20, "britannia", "britannia"},    	    // Despise_ent_2_x[]
    {5386,  755,    -8,     5407,   859,    45, "britannia", "britannia"},   		// Despise_2_3_x[]
    {5386,  756,    -8,     5407,   859,    45, "britannia", "britannia"},      	// Despise_2_3_x[]
    {5386,  757,    -8,     5407,   859,    45, "britannia", "britannia"},    	    // Despise_2_3_x[]
    {5409,  858,    52,     5389,   756,    3, "britannia", "britannia"},     	    // Despise_3_2_x[]
    {5409,  859,    52,     5389,   756,    3, "britannia", "britannia"},     	    // Despise_3_2_x[]
    {5409,  860,    52,     5389,   756,    3, "britannia", "britannia"},     	    // Despise_3_2_x[]
    {5455,  1863,   0,      2498,   919,    1, "britannia", "britannia"},     	    // Covetous_ent_out_x[]
    {5456,  1863,   0,      2498,   919,    1, "britannia", "britannia"},     	    // Covetous_ent_out_x[]
    {5457,  1863,   1,      2498,   919,    1, "britannia", "britannia"},     	    // Covetous_ent_out_x[]
    {2498,  917,    0,      5457,   1861,   0, "britannia", "britannia"},     	    // Covetous_out_x[]
    {2499,  917,    0,      5457,   1861,   0, "britannia", "britannia"},     	    // Covetous_out_x[]
    {2500,  917,    0,      5457,   1861,   0, "britannia", "britannia"},     	    // Covetous_out_x[]
    {5392,  1959,   0,      2421,   884,    1, "britannia", "britannia"},     	    // Covetous_1_2_x[]
    {5393,  1959,   0,      2421,   884,    1, "britannia", "britannia"},     	    // Covetous_1_2_x[]
	{5394,  1959,   1,      2421,   884,    1, "britannia", "britannia"},     	    // Covetous_1_2_x[]
    {2420,  883,    0,      5392,   1958,   0, "britannia", "britannia"},     	    // Covetous_2_1_x[]
    {2421,  883,    0,      5393,   1958,   0, "britannia", "britannia"},     	    // Covetous_2_1_x[]
    {2422,  883,    0,      5394,   1958,   0, "britannia", "britannia"},     	    // Covetous_2_1_x[]
    {2385,  836,    0,      5614,   1997,   0, "britannia", "britannia"},     	    // Covetous_2_3_x[]
    {2385,  837,    0,      5614,   1997,   0, "britannia", "britannia"},     	    // Covetous_2_3_x[]
    {2385,  838,    0,      5614,   1997,   0, "britannia", "britannia"},     	    // Covetous_2_3_x[]
    {5615,  1996,   0,      2386,   837,    1, "britannia", "britannia"},     	    // Covetous_3_2_x[]
    {5615,  1997,   0,      2386,   837,    1, "britannia", "britannia"},     	    // Covetous_3_2_x[]
    {5615,  1998,   0,      2386,   837,    1, "britannia", "britannia"},     	    // Covetous_3_2_x[]
	{5388,  2027,   0,      2455,   860,    0, "britannia", "britannia"},     	    // Covetous_3_4_x[]
    {5389,  2027,   0,      2456,   860,    0, "britannia", "britannia"},     	    // Covetous_3_4_x[]
    {5390,  2027,   1,      2457,   860,    0, "britannia", "britannia"},     	    // Covetous_3_4_x[]
    {2455,  859,    0,      5388,   2026,   0, "britannia", "britannia"},     	    // Covetous_4_3_x[]
    {2456,  859,    0,      5389,   2026,   0, "britannia", "britannia"},     	    // Covetous_4_3_x[]
    {2457,  859,    0,      5390,   2026,   0, "britannia", "britannia"},     	    // Covetous_4_3_x[]
    {2544,  847, 	0, 		5579, 	1924, 	0, "britannia", "britannia"},    		// Covetous_4_5_x[]
    {2545,  847,    0, 		5579, 	1924, 	0, "britannia", "britannia"},   		// Covetous_4_5_x[]
    {2546,  847,    0, 		5579, 	1924, 	0, "britannia", "britannia"},   		// Covetous_4_5_x[]
    {5578,  1926,   0,      2545,   851,    1, "britannia", "britannia"},     	    // Covetous_5_4_x[]
    {5579,  1926,   0,      2545,   851,    1, "britannia", "britannia"},     	    // Covetous_5_4_x[]
    {5580,  1926,   0,      2545,   851,    1, "britannia", "britannia"},     	    // Covetous_5_4_x[]
	{5556,  1827,   -13, 	5551,   1806,   7, "britannia", "britannia"},			// Covetous_5_6a_x[]
	{5557,  1827,   -13, 	5552,   1806,   7, "britannia", "britannia"},			// Covetous_5_6a_x[]
	{5551,  1805,   12,   	5556,   1826,   -8, "britannia", "britannia"},     	    // Covetous_6a_5_x[]
    {5552,  1805,   12,   	5556,   1826,   -8, "britannia", "britannia"},     	    // Covetous_6a_5_x[]
	{5553,  1805,   12,  	5557,   1826,   -8, "britannia", "britannia"},     	    // Covetous_6a_5_x[]
	{5595,  1840,   -13, 	5467,   1804,   7, "britannia", "britannia"},			// Covetous_5_6b_x[]
	{5595,  1841,   -13, 	5467,   1805,   7, "britannia", "britannia"},			// Covetous_5_6b_x[]
    {5466,  1804,    12, 	5594,   1840,   -8, "britannia", "britannia"},   		// Covetous_6b_5_x[]
    {5466,  1805,    12, 	5594,   1841,   -8, "britannia", "britannia"},  		// Covetous_6b_5_x[]
	{5466,  1806,    12, 	5594,   1841,   -8, "britannia", "britannia"},  		// Covetous_6b_5_x[]
	{5823,  630,    0,		2041,   216,    14, "britannia", "britannia"},     	    // Wrong_ent_x[]
    {5824,  630,    0,		2042,   216,    14, "britannia", "britannia"},     	    // Wrong_ent_x[]
    {5825,  630,    0,		2043,   216,    14, "britannia", "britannia"},     	    // Wrong_ent_x[]
	{2041,  215,    14,     5823,   629,    0, "britannia", "britannia"},     	    // Wrong_out_x[]
    {2042,  215,    14,     5824,   629,    0, "britannia", "britannia"},     	    // Wrong_out_x[]
    {2043,  215,    14,     5825,   629,    0, "britannia", "britannia"},     	    // Wrong_out_x[]
    {5867,  529,    20,   	5690,   568,    25, "britannia", "britannia"},    	    // Wrong_1_2_x[]
    {5690,  569,    25,   	5867,   530,    20, "britannia", "britannia"},     	    // Wrong_2_1_x
	{5673,  1439,   0,  	5685,   1432,   22, "britannia", "britannia"},    	    // Fire_dung_lvl_1_1[]
	{5686,  1432,   22,  	5674,   1439,   0, "britannia", "britannia"},    		// Fire_dung_lvl_1_1[]
    {5786,  1336,   -8,     5701,   1317,   6, "britannia", "britannia"},     	    // Fire_dung_lvl_1_2[]
    {5787,  1336,   -8,     5702,   1317,   6, "britannia", "britannia"},     	    // Fire_dung_lvl_1_2[]
    {5788,  1336,   -8,     5703,   1317,   6, "britannia", "britannia"},     	    // Fire_dung_lvl_1_2[]
    {5701,  1320,   16,     5786,   1337,   -5, "britannia", "britannia"},    	    // Fire_dung_lvl_2_1[]
    {5702,  1320,   16,     5787,   1337,   -5, "britannia", "britannia"},    	    // Fire_dung_lvl_2_1[]
    {5703,  1320,   16,     5788,   1337,   -5, "britannia", "britannia"},    	    // Fire_dung_lvl_2_1[]
	{5619,  1431,  	0,   	6677,  	110,  	0, "britannia", "britannia"},   	 	// Fire_Dung_mine_2_3_x[]
	{5620,  1431,  	0,   	6678,  	110,  	0, "britannia", "britannia"},   	 	// Fire_Dung_mine_2_3_x[]
	{5621,  1431,  	0,   	6678,  	110,  	0, "britannia", "britannia"},   	 	// Fire_Dung_mine_2_3_x[]
	{6677,  111,  	0,   	5619,  	1432,  	0, "britannia", "britannia"},   	 	// Fire_Dung_mine_3_2_x[]
	{6678,  111,  	0,   	5620,  	1432,  	0, "britannia", "britannia"},   	 	// Fire_Dung_mine_3_2_x[]
    {5827,  1503,   6,      2923,   3406,   8, "britannia", "britannia"},     	    // Fire_Dung_to_Serp_hold[]
    {5828,  1503,   5,    	2923,   3406,   8, "britannia", "britannia"},     	    // Fire_Dung_to_Serp_hold[]
    {5829,  1503,   6,      2923,   3406,   8, "britannia", "britannia"},     	    // Fire_Dung_to_Serp_hold[]
	{5830,  1503,   6,      2923,   3406,   8, "britannia", "britannia"},     	    // Fire_Dung_to_Serp_hold[]
    {2922,  3405,   7,      5827,   1502,   2, "britannia", "britannia"},     	    // Serp_Hold_to_Fire_Dung[]
    {2923,  3405,   6,      5828,   1502,   1, "britannia", "britannia"},     	    // Serp_Hold_to_Fire_Dung[]
    {2924,  3405,   7,      5829,   1502,   2, "britannia", "britannia"},     	    // Serp_Hold_to_Fire_Dung[]
	{1997,  80,     7,      5880,   241,    0, "britannia", "britannia"},     	    // N_Wrong_to_Ice_dngn[]
    {1997,  81,     7,      5880,   242,    0, "britannia", "britannia"},       	// N_Wrong_to_Ice_dngn[]
    {1997,  82,     6,      5880,   243,    -1, "britannia", "britannia"},     	    // N_Wrong_to_Ice_dngn[]
	{5881,  241,    0,   	1998,   81,     5, "britannia", "britannia"},    		// Ice_dngn_to_N_Wrong[]
    {5881,  242,    0,   	1998,   81,     5, "britannia", "britannia"},    		// Ice_dngn_to_N_Wrong[]
    {5881,  243,    -1,   	1998,   81,     5, "britannia", "britannia"},    		// Ice_dngn_to_N_Wrong[]
    {5832,  323,    34,     5849,   237,    -24, "britannia", "britannia"},   	    // Orc_lvl_to_ice_dngn[]
    {5833,  323,    33,     5850,   237,    -24, "britannia", "britannia"},   	    // Orc_lvl_to_ice_dngn[]
    {5834,  323,    33,     5851,   237,    -24, "britannia", "britannia"},   	    // Orc_lvl_to_ice_dngn[]
    {5835,  323,    33,     5852,   237,    -25, "britannia", "britannia"},   	    // Orc_lvl_to_ice_dngn[]
    {5849,  238,    -25,    5832,   325,    25, "britannia", "britannia"},    	    // Ice_dngn_to_orc_lvl[]
    {5850,  238,    -25,    5833,   325,    25, "britannia", "britannia"},    	    // Ice_dngn_to_orc_lvl[]
    {5851,  238,    -25,    5834,   325,    25, "britannia", "britannia"},    	    // Ice_dngn_to_orc_lvl[]
    {5852,  238,    -25,    5835,   325,    25, "britannia", "britannia"},    	    // Ice_dngn_to_orc_lvl[]
    {5705,  145,    -45,    5705,   305,    10, "britannia", "britannia"},    	    // Ice_dngn_to_lvl2[]
    {5705,  146,    -45,    5705,   306,    10, "britannia", "britannia"},    	    // Ice_dngn_to_lvl2[]
    {5705,  147,    -45,    5705,   307,    10, "britannia", "britannia"},    	    // Ice_dngn_to_lvl2[]
    {5706,  305,    12,     5706,   145,    -44, "britannia", "britannia"},   	    // lvl2_to_ice_dngn[]
    {5706,  306,    12,     5706,   146,    -44, "britannia", "britannia"},   	    // lvl2_to_ice_dngn[]
    {5706,  307,    12,     5706,   147,    -45, "britannia", "britannia"},   	    // lvl2_to_ice_dngn[]
    {5706,  304,    12,     5706,   148,    -45, "britannia", "britannia"},   	    // lvl2_to_ice_dngn[]
	{4721,  3813,   0,   	5904,   28,     56, "britannia", "britannia"},    	    // Hythloth_out_x[]
    {4722,  3813,   0,   	5905,   28,     56, "britannia", "britannia"},    	    // Hythloth_out_x[]
    {4723,  3813,   0,   	5906,   28,     56, "britannia", "britannia"},    	    // Hythloth_out_x[]
	{5904,  29,     61,   	4721,   3815,   1, "britannia", "britannia"},     	    // Hythloth_ent_out_x[]
    {5905,  29,     61,   	4722,   3815,   1, "britannia", "britannia"},     	    // Hythloth_ent_out_x[]
    {5906,  29,     61,   	4723,   3815,   1, "britannia", "britannia"},     	    // Hythloth_ent_out_x[]
	{5973,  169,    0, 	    5905,   99,     0, "britannia", "britannia"},     	    // Hythloth_2_1_x[]	
    {5905,  97,     0, 	    5976,   169,    1, "britannia", "britannia"},     	    // Hythloth_1_2_x[]
	{5921,  168,    14,   	6083,   144,    -20, "britannia", "britannia"},   	    // Hythloth_2_3_x[]
    {5921,  169,    14,   	6083,   145,    -20, "britannia", "britannia"},   	    // Hythloth_2_3_x[]
    {5921,  170,    14,   	6083,   146,    -20, "britannia", "britannia"},   	    // Hythloth_2_3_x[]
	{6082,  144,    -15,  	5920,   168,    20, "britannia", "britannia"},    	    // Hythloth_3_2_x[]
    {6082,  145,    -15,  	5920,   169,    20, "britannia", "britannia"},    	    // Hythloth_3_2_x[]
    {6082,  146,    -15,  	5920,   170,    20, "britannia", "britannia"},    	    // Hythloth_3_2_x[]
	{6040,  192,    12,   	6059,   89,     24, "britannia", "britannia"},    	    // Hythloth_3_4_x[]
    {6040,  193,    12,   	6059,   89,     24, "britannia", "britannia"},    	    // Hythloth_3_4_x[]
    {6040,  194,    12,   	6059,   89,     24, "britannia", "britannia"},    	    // Hythloth_3_4_x[]
    {6058,  88,     29, 	6039,   193,    17, "britannia", "britannia"},    	    // Hythloth_4_3_x[]
    {6058,  89,     29, 	6039,   193,    17, "britannia", "britannia"},    	    // Hythloth_4_3_x[]
    {6058,  90,     29, 	6039,   193,    17, "britannia", "britannia"},    	    // Hythloth_4_3_x[]
	{6107,  179,    0, 		6117,   179,    0, "britannia", "britannia"},    		// Hythloth_3_3_x[]
	{6115,  179,    0, 		6105,   179,    0, "britannia", "britannia"},    		// Hythloth_3_3_x[]
	{2628,  45,     20,   	5656,   2016,   0, "britannia", "britannia"},		   	// Solen_ent_out_x[]
	{5659,  2019,   0,    	2626,   44,   	21, "britannia", "britannia"},      	// Solen_ent_out_x[]
	{5661,  1955,   0,    	5709,   1955, 	0, "britannia", "britannia"},     	    // Solen_1_2_x[]
	{5709,  1953,   0,    	5658,   1958, 	0, "britannia", "britannia"},     	    // Solen_2_1_x[]
	{5698,  1929,   0,    	5661,   1897, 	0, "britannia", "britannia"},     	    // Solen_2_3_x[]
	{5662,  1899,   0,    	5699,   1928, 	0, "britannia", "britannia"},     	    // Solen_3_2_x[]
	{5728,  1892,   0,    	5748,   1899, 	0, "britannia", "britannia"},     	    // Solen_3_4_x[]
	{5751,  1896,   0,    	5725,   1895, 	0, "britannia", "britannia"},     	    // Solen_4_3_x[]
	{5785,  1927,   0,    	5879,   1894, 	0, "britannia", "britannia"},     	    // Solen_C_5b_x[]
	{5877,  1895,   0,    	5784,   1927, 	0, "britannia", "britannia"},     	    // Solen_5b_C_x[]
	{5810,  1909,   0,    	5853,   1846, 	0, "britannia", "britannia"},     	    // Solen_C_5c_x[]
	{5853,  1852,   0,    	5810,   1903, 	0, "britannia", "britannia"},     	    // Solen_5c_C_x[]
	{5774,  1870,   0,    	5754,   1794, 	2, "britannia", "britannia"},     	    // Solen_C_5d_x[]
	{5757,  1798,   7,    	5772,   1865, 	0, "britannia", "britannia"},     	    // Solen_5d_C_x[]
	{5875,  1868,   2,    	5911,   1895, 	0, "britannia", "britannia"},     	    // Solen_5c_6_x[]
	{5910,  1894,   0,    	5874,   1870, 	2, "britannia", "britannia"},     	    // Solen_6_5c_x[]
	{5815,  2013,   0,    	5920,   2025, 	0, "britannia", "britannia"},     	    // Solen_5b_6_x[]
	{5916,  2022,   0,    	5818,   2016, 	0, "britannia", "britannia"},     	    // Solen_6_5b_x[]
	{5658,  1792,   0,    	5915,   1795, 	0, "britannia", "britannia"},     	    // Solen_5d_6a_x[]
	{5919,  1798,   0,    	5655,   1789, 	0, "britannia", "britannia"},     	    // Solen_6a_5d_x[]
	{5901,  1878,   0,    	5733,   1858, 	0, "britannia", "britannia"},     	    // Solen_6a_7_x[]
	{5732,  1861,   0,    	5900,   1876, 	0, "britannia", "britannia"},     	    // Solen_7_6a_x[]
	{1140,  2595,   0,    	5360,   1707, 	-65, "britannia", "britannia"},     	// Terathan_ent_out_x[]
	{5362,  1706, 	-65,   	1138,   2595,   0, "britannia", "britannia"},     	    // Terathan_ent_out_x[]
	{588,   1637, 	0,    	6489,   849,    40, "britannia", "britannia"},     	    // Hiddentree_ent_out_x[]
	{6488,  848, 	40,    	588,    1638,   0, "britannia", "britannia"},     	    // Hiddentree_ent_out_x[]
	{6488,  849, 	40,    	588,    1638,   0, "britannia", "britannia"},     	    // Hiddentree_ent_out_x[]
	{6488,  850, 	40,    	588,    1638,   0, "britannia", "britannia"},     	    // Hiddentree_ent_out_x[]
	{6511,  953, 	-40,   	6511,   948,    30, "britannia", "britannia"},     	    // Hiddentree_1_1_x[]
	{6511,  949,    26,   	6511,   954,    -40, "britannia", "britannia"},     	// Hiddentree_1_1_x[]
	{6529,  954, 	40,   	7047,   333,    10, "britannia", "britannia"},     	    // Hiddentree_1_1_x[]
	{7046,  332,    10,   	6527,   954,    40, "britannia", "britannia"},     	    // Hiddentree_1_1_x[]
	{6163,  7,    	4,   	1715,   1064,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  8,    	4,   	1715,   1065,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  9,    	4,   	1715,   1066,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  10,    	4,   	1715,   1067,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  11,    	4,   	1715,   1068,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  12,    	4,   	1715,   1068,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{6163,  13,    	9,   	1715,   1069,   0, "britannia", "britannia"},     	    // Raided_town_ent_out_x[]
	{1714,  1064,  	1,   	6162,   7,   	0, "britannia", "britannia"},    		// Raided_town_out_ent_x[]
	{1714,  1065,  	1,   	6162,   8,   	0, "britannia", "britannia"},    		// Raided_town_out_ent_x[]
	{1714,  1066,  	1,   	6162,   9,   	0, "britannia", "britannia"},    		// Raided_town_out_ent_x[]
	{1714,  1067,  	1,   	6162,   10,  	0, "britannia", "britannia"},   	 	// Raided_town_out_ent_x[]
	{1714,  1068,  	1,   	6162,   11,  	0, "britannia", "britannia"},   	 	// Raided_town_out_ent_x[]
	{1714,  1069,  	1,   	6162,   12,  	0, "britannia", "britannia"},   	 	// Raided_town_out_ent_x[]
	{1013,  1434,  	0,   	5139,   2017,  	0, "britannia", "britannia"},   	 	// Orc_mine_ent_out_x[]
	{1013,  1433,  	0,   	5139,   2016,  	0, "britannia", "britannia"},   	 	// Orc_mine_ent_out_x[]
	{5140,  2015,  	2,   	1014,   1433,  	0, "britannia", "britannia"},   	 	// Orc_mine_out_ent_x[]
	{5140,  2016,  	2,   	1014,   1433,  	0, "britannia", "britannia"},   	 	// Orc_mine_out_ent_x[]
	{5140,  2017,  	2,   	1014,   1434,  	0, "britannia", "britannia"},   	 	// Orc_mine_out_ent_x[]
	{5140,  2018,  	2,   	1014,   1434,  	0, "britannia", "britannia"},   	 	// Orc_mine_out_ent_x[]
	{5132,  1946,  	0,   	5136,   1914,  	0, "britannia", "britannia"},   	 	// Orc_mine_1_2_x[]
	{5133,  1946,  	0,   	5137,   1914,  	0, "britannia", "britannia"},   	 	// Orc_mine_1_2_x[]
	{5134,  1946,  	0,   	5138,   1914,  	0, "britannia", "britannia"},   	 	// Orc_mine_1_2_x[]
	{5135,  1946,  	0,   	5139,   1914,  	0, "britannia", "britannia"},   	 	// Orc_mine_1_2_x[]
	{5136,  1915,  	0,   	5132,   1947,  	0, "britannia", "britannia"},   	 	// Orc_mine_2_1_x[]
	{5137,  1915,  	0,   	5133,   1947,  	0, "britannia", "britannia"},   	 	// Orc_mine_2_1_x[]
	{5138,  1915,  	0,   	5134,   1947,  	0, "britannia", "britannia"},   	 	// Orc_mine_2_1_x[]
	{5139,  1915,  	0,   	5135,   1947,  	0, "britannia", "britannia"},   	 	// Orc_mine_2_1_x[]
	{5166,  1874,  	0,   	5363,  	1289,  	3, "britannia", "britannia"},   	 	// Orc_mine_2_3_x[]
	{5167,  1874,  	0,   	5364,  	1289,  	3, "britannia", "britannia"},   	 	// Orc_mine_2_3_x[]
	{5168,  1874,  	0,   	5364,  	1289,  	3, "britannia", "britannia"},   	 	// Orc_mine_2_3_x[]
	{5363,  1288,  	3,   	5166,  	1873,  	0, "britannia", "britannia"},   	 	// Orc_mine_3_2_x[]
	{5364,  1288,  	3,   	5167,  	1873,  	0, "britannia", "britannia"},   	 	// Orc_mine_3_2_x[]
	{5571,  1299, 	0,  	5331,   1380,   2, "britannia", "britannia"},     	    // Omega_1_2_x[]
	{5572,  1299, 	0,    	5332,   1380,   2, "britannia", "britannia"},     	    // Omega_1_2_x[]
	{5331,  1381, 	7,    	5571,   1300,   0, "britannia", "britannia"},     	    // Omega_2_1_x[]
	{5332,  1381, 	7,    	5572,   1300,   0, "britannia", "britannia"},     	    // Omega_2_1_x[]
	{1437,  1226,  	0,   	6309,  	890,  	-1, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{1437,  1227,  	0,   	6309,  	891,  	-2, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{1437,  1228,  	0,   	6309,  	892,  	-2, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{1437,  1229,  	0,   	6309,  	893,  	-1, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{6310,  890,  	3,   	1438,  	1226,  	0, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{6310,  891,  	2,   	1438,  	1227,  	0, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{6310,  892,  	2,   	1438,  	1228,  	0, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{6310,  893,  	3,   	1438,  	1229,  	0, "britannia", "britannia"},   	 	// Cave_out_ent_x[]
	{3511,  2464,  	0,   	5946,  	1463,  	20, "britannia", "britannia"},   	 	// Ocllo_mine_ent_1_x[]
	{3512,  2464,  	0,   	5946,  	1463,  	20, "britannia", "britannia"},   	 	// Ocllo_mine_ent_1_x[]
	{5946,  1464,  	22,   	3511,  	2465,  	1, "britannia", "britannia"},   	 	// Ocllo_mine_1_ent_x[]
	
// towns
    {1133,  2230,   20,     1163,   2207,   60, "britannia", "britannia"},     	    // hedge_tele_h_t[]
    {1163,  2208,   60,     1133,   2231,   20, "britannia", "britannia"},    	    // hedge_tele_t_h[]
	{1142,  3621,   5,      1414,   3828,   6, "britannia", "britannia"},     	    // jhelom_ns_m_x[]
    {1406,  3996,   5,      1414,   3828,   6, "britannia", "britannia"},     	    // jhelom_ns_m_x[]
    {1409,  3824,   5,      1124,   3623,   6, "britannia", "britannia"},     	    // jhelom_m_n_x[]
    {1419,  3832,   5,      1466,   4015,   6, "britannia", "britannia"},     	    // jhelom_m_s_x[]
    {1491,  1640,   24,     6032,   1498,   22, "britannia", "britannia"},    	    // Britain_sewer_x[]
    {1491,  1642,   24,     6032,   1498,   22, "britannia", "britannia"},    	    // Britain_sewer_x[]
    {6096,  1445,   10,     6100,   1445,   25, "britannia", "britannia"},    	    // Britain_sewer_x[]
    {6097,  1445,   25,     6095,   1445,   7, "britannia", "britannia"},    		// Britain_sewer_x[]
    {6032,  1499,   31,     1490,   1640,   24, "britannia", "britannia"},    	    // Britain_sewer_x[] to britain
    {6032,  1501,   31,     1490,   1640,   24, "britannia", "britannia"},    	    // Britain_sewer_x[] to britain
    {1592,  2489,   17,     1602,   2488,   5, "britannia", "britannia"},     	    // shrine_spirit_exit[]
    {1600,  2489,   10,     1595,   2490,   19, "britannia", "britannia"},    	    // shrine_spirit_ent1[]
    {1600,  2490,   10,     1595,   2490,   19, "britannia", "britannia"},    	    // shrine_spirit_ent2[]
    {4300,  968,    5,   	4442,   1122,   5, "britannia", "britannia"},    		// moonglow_b_x[]
    {4436,  1107,   5,   	4300,   992,    5, "britannia", "britannia"},    		// moonglow_3_x[]
    {4443,  1137,   5,   	4487,   1475,   5, "britannia", "britannia"},    		// moonglow_2_x[]
    {4449,  1107,   5,   	4539,   890,    28, "britannia", "britannia"},   		// moonglow_4_y[]
    {4449,  1115,   5,   	4671,   1135,   10, "britannia", "britannia"},   		// moonglow_1_x[]
    {4496,  1475,   15,  	4442,   1122,   5, "britannia", "britannia"},    		// moonglow_b_x[]
    {4540,  898,    32,  	4442,   1122,   5, "britannia", "britannia"},    		// moonglow_b_x[]
    {4663,  1134,   13,  	4442,   1122,   5, "britannia", "britannia"},    		// moonglow_b_x[]
    {2673, 	2278, 	-29, 	2673, 	2280, 	-40, "britannia", "britannia"}, 		// buc_den_down
    {2674, 	2278, 	-29, 	2674, 	2280, 	-40, "britannia", "britannia"}, 		// buc_den
    {2675, 	2278, 	-29, 	2675, 	2280, 	-40, "britannia", "britannia"}, 		// buc_den
    {2676, 	2278, 	-29, 	2676, 	2280, 	-40, "britannia", "britannia"}, 		// buc_den
    {2673, 	2279, 	-35, 	2673, 	2277, 	-20, "britannia", "britannia"}, 		// buc_den_up
    {2674, 	2279, 	-35, 	2674, 	2277, 	-20, "britannia", "britannia"}, 		// buc_den
    {2675, 	2279, 	-35, 	2675, 	2277, 	-20, "britannia", "britannia"}, 		// buc_den
    {2676, 	2279, 	-35, 	2676, 	2277, 	-20, "britannia", "britannia"}, 		// buc_den
    {2623, 	2274, 	-40, 	2623, 	2274, 	18, "britannia", "britannia"},  		// buc_den_exit_1
    {2623, 	2273, 	-40, 	2623, 	2273, 	18, "britannia", "britannia"},  		// buc_den    
    {2676, 	2241, 	-18, 	2673, 	2241, 	2, "britannia", "britannia"},   		// buc_den_exit_2
    {2676, 	2242, 	-18, 	2673, 	2242, 	2, "britannia", "britannia"},   		// buc_den_   
    {2649, 	2195, 	-14, 	2651, 	2195, 	5, "britannia", "britannia"},   		// buc_den_exit_3
    {2665, 	2074, 	-18, 	2669, 	2099, 	3, "britannia", "britannia"},   		// buc_den_exit_4
    {2666, 	2074, 	-18, 	2669, 	2099, 	3, "britannia", "britannia"},   		// buc_den
    {2773, 	2097, 	-20, 	2773, 	2097, 	5, "britannia", "britannia"},   		// buc_den_exit_5
	{3784, 	1100, 	-10, 	6486, 	123, 	-35, "britannia", "britannia"},  		// nujelm_mine_down
	{3785, 	1100, 	-10, 	6486, 	123, 	-35, "britannia", "britannia"},  		// nujelm_mine_down
	{6486, 	122, 	-35, 	3784, 	1099, 	-3, "britannia", "britannia"},   		// nujelm_mine_up
	{6469, 	97, 	-50, 	6476, 	75, 	-34, "britannia", "britannia"},  		// nujelm_mine
	{6476, 	76, 	-35, 	6470, 	97, 	-50, "britannia", "britannia"},   	    // nujelm_mine
	{6477, 	94, 	-49, 	6504, 	88, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6503, 	88, 	0, 		6476, 	94, 	-49, "britannia", "britannia"},   	    // nujelm_mine
	{6518, 	90, 	0, 		6525, 	88, 	-10, "britannia", "britannia"},  		// nujelm_mine
	{6524, 	88, 	-10,	6517, 	90, 	0, "britannia", "britannia"},		   	// nujelm_mine
	{6568, 	77, 	-10, 	6577, 	77, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6576, 	77, 	0, 		6567, 	77, 	-10, "britannia", "britannia"},   	    // nujelm_mine
	{6554, 	104, 	0, 		6550, 	117, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6550, 	116, 	0, 		6554, 	103, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6559, 	128, 	0, 		6553, 	144, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6553, 	143, 	0, 		6559, 	127, 	0, "britannia", "britannia"},   		// nujelm_mine
	{6577, 	160, 	10,		6576, 	177, 	30, "britannia", "britannia"},   		// nujelm_mine
	{6578, 	160, 	8,		6577, 	177, 	30, "britannia", "britannia"},   		// nujelm_mine
	{6576, 	176, 	30, 	6577, 	159, 	13, "britannia", "britannia"},   		// nujelm_mine
	{6577, 	176, 	30, 	6578, 	159, 	12, "britannia", "britannia"},   		// nujelm_mine
	{6589, 	178, 	25, 	6515, 	182, 	0, "britannia", "britannia"},   		// nujelm_mine_dung
	{6514, 	182, 	0, 		6589, 	179, 	25, "britannia", "britannia"},   		// nujelm_dung_mine
	{6541, 	116, 	-15,	6501, 	141, 	-20, "britannia", "britannia"},   	    // nujelm_boss_dung
	{6541, 	117, 	-15,	6501, 	141, 	-20, "britannia", "britannia"},   	    // nujelm_boss_dung
	{6540, 	138, 	-15, 	6501, 	131, 	-20, "britannia", "britannia"},  		// nujelm_dung_boss
	{6540, 	139, 	-15, 	6501, 	131, 	-20, "britannia", "britannia"},  		// nujelm_dung_boss
	{709, 	2238, 	-2,		682, 	2234, 	-2, "britannia", "britannia"},   		// skara_brae_boats
	{683, 	2234, 	-2, 	710, 	2238, 	-2, "britannia", "britannia"},   		// skara_brae_boats
	{683, 	2242, 	-2, 	710, 	2238, 	-2, "britannia", "britannia"},   		// skara_brae_boats	
	
// mines & caves
	{780, 	1646, 	0, 		5205, 	1993, 	0, "britannia", "britannia"},   		// mine_near_destard_0_1
	{781, 	1646, 	0, 		5206, 	1993, 	0, "britannia", "britannia"},   		// mine_near_destard_0_1
	{5205, 	1994, 	0, 		780, 	1647, 	0, "britannia", "britannia"},   		// mine_near_destard_1_0
	{5206, 	1994, 	0, 		781, 	1647, 	0, "britannia", "britannia"},   		// mine_near_destard_1_0
	{1988, 	2062, 	-40, 	5272, 	2042, 	3, "britannia", "britannia"},   		// mine_on_island_0_1
	{1989, 	2062, 	-40, 	5273, 	2042, 	3, "britannia", "britannia"},   		// mine_on_island_0_1
	{5272, 	2043, 	3, 		1988, 	2063, 	-40, "britannia", "britannia"},   	    // mine_on_island_1_0
	{5273, 	2043, 	3, 		1989, 	2063, 	-40, "britannia", "britannia"},   	    // mine_on_island_1_0
	{4229, 	611, 	0, 		5269, 	1934, 	0, "britannia", "britannia"},   		// mine_on_ice_island_0_1
	{4230, 	611, 	0, 		5270, 	1934, 	0, "britannia", "britannia"},   		// mine_on_ice_island_0_1
	{5269, 	1935, 	0, 		4229, 	612, 	0, "britannia", "britannia"},   		// mine_on_ice_island_1_0
	{5270, 	1935, 	0, 		4230, 	612, 	0, "britannia", "britannia"},   		// mine_on_ice_island_1_0
	{1104, 	1788, 	0, 		5166, 	1829, 	0, "britannia", "britannia"},   		// mine_near_britain_0_1
	{1104, 	1789, 	0, 		5166, 	1830, 	0, "britannia", "britannia"},   		// mine_near_britain_0_1
	{5167, 	1829, 	0, 		1105, 	1788, 	0, "britannia", "britannia"},   		// mine_near_britain_1_0
	{5167, 	1830, 	0, 		1105, 	1789, 	0, "britannia", "britannia"},   		// mine_near_britain_1_0	
	{2206, 	1247, 	0, 		5221, 	1862, 	0, "britannia", "britannia"},   		// mine_near_cove_0_1
	{2207, 	1247, 	0, 		5221, 	1862, 	0, "britannia", "britannia"},   		// mine_near_cove_0_1
	{2208, 	1247, 	0, 		5222, 	1862, 	0, "britannia", "britannia"},   		// mine_near_cove_0_1
	{2209, 	1247, 	0, 		5222, 	1862, 	0, "britannia", "britannia"},   		// mine_near_cove_0_1
	{5221, 	1863, 	0, 		2207, 	1248, 	0, "britannia", "britannia"},   		// mine_near_cove_1_0
	{5222, 	1863, 	0, 		2208, 	1248, 	0, "britannia", "britannia"},   		// mine_near_cove_1_0	
	{4268, 	3765, 	1, 		5237, 	2038, 	0, "britannia", "britannia"},   		// mine_on_avatar_0_1
	{5237, 	2039, 	0, 		4268, 	3766, 	0, "britannia", "britannia"},   		// mine_on_avatar_1_0
	{5238, 	2039, 	0, 		4268, 	3766, 	0, "britannia", "britannia"},   		// mine_on_avatar_1_0	
 	{1653, 	2963, 	0, 		1676, 	2987, 	0, "britannia", "britannia"},   		// tele_near_trinsic_0_1
 	{1677, 	2987, 	0, 		1654, 	2964, 	0, "britannia", "britannia"},   		// tele_near_trinsic_1_0
	{6022, 	1691, 	20, 	1715, 	2996, 	0, "britannia", "britannia"},   		// cave_near_trinsic_0_1
	{6022, 	1692, 	20, 	1715, 	2997, 	0, "britannia", "britannia"},   		// cave_near_trinsic_0_1
	{1714, 	2996, 	0, 		6021, 	1691, 	15, "britannia", "britannia"},   		// cave_near_trinsic_1_0
	{1714, 	2997, 	0, 		6021, 	1692, 	15, "britannia", "britannia"},   		// cave_near_trinsic_1_0
	{2664, 	250, 	0, 		5305, 	1858, 	0, "britannia", "britannia"},   		// mine_near_solen_0_1
	{2665, 	250, 	0, 		5306, 	1858, 	0, "britannia", "britannia"},   		// mine_near_solen_0_1
	{5305, 	1859, 	0, 		2664, 	251, 	0, "britannia", "britannia"},   		// mine_near_solen_1_0
	{5306, 	1859, 	0, 		2665, 	251, 	0, "britannia", "britannia"},   		// mine_near_solen_1_0
	{2398, 	198, 	0, 		5185, 	1941, 	0, "britannia", "britannia"},   		// mine_near_minoc_0_1
	{2399, 	198, 	0, 		5186, 	1941, 	0, "britannia", "britannia"},   		// mine_near_minoc_0_1
	{5185, 	1942, 	0, 		2398, 	199, 	0, "britannia", "britannia"},   		// mine_near_minoc_1_0
	{5186, 	1942, 	0, 		2399, 	199, 	0, "britannia", "britannia"}   		    // mine_near_minoc_1_0
};	

function CreateTeleporters()

    var fromX;
    var fromY;
    var fromZ;
    var fromR;

    var toX;
    var toY;
    var toZ;
    var toR;

    var nbrOfTeleporters := 0;

    foreach tele in teleporters

        fromX := cint(tele[1]);
        fromY := cint(tele[2]);
        fromZ := cint(tele[3]);
        fromR := tele[7];

        // check -255.
        if ( fromZ == -255 )
            fromZ := GetWorldHeight( fromx, fromy );
        endif

        toX := cint(tele[4]);
        toY := cint(tele[5]);
        toZ := cint(tele[6]);
        toR := tele[8];

        // create the teleporter.
        var teleporter;

        var alrplaced := 0;
        foreach alrtele in ListItemsAtLocation( fromX, fromY, fromZ, fromR );
            if(alrtele.name == "systemteleporter")
                alrplaced := 1;
            endif
        endforeach

        if(!alrplaced)
            teleporter := CreateItemAtLocation( fromX, fromY, fromZ, "systemteleporter", 1, fromR );
            if(!teleporter)
                var count := 0;
                while(!teleporter || count >= 100)
                    if(count % 2 == 0)
                        fromZ := GetWorldHeight( fromX, fromY, fromR );
                    else
                        fromZ := GetStandingHeight( fromX, fromY, (fromZ-5), fromR);
                    endif
                    teleporter := CreateItemAtLocation( fromX, fromY, fromZ, "systemteleporter", 1, fromR );
                    count := count + 1;
                    sleepms(100);
                endwhile
            endif

            // enter the destination CProps.
            if ( teleporter )
                SetObjProperty( teleporter, "DestX", toX );
                SetObjProperty( teleporter, "DestY", toY );
                SetObjProperty( teleporter, "DestZ", toZ );
                SetObjProperty( teleporter, "DestRealm", toR );

                nbrOfTeleporters := nbrOfTeleporters + 1;
                teleporter.decayat := 0;
                var count := 0;
                while(teleporter.decayat != 0 || count >= 100)
                    teleporter.decayat := 0;
                    count := count + 1;
                    sleepms(10);
                endwhile
            else
            syslog("Error: " + teleporter.errortext + " at: " + fromX + " " + fromY + " " + fromZ + " " + fromR );
            endif    
        endif

    endforeach
    
  	//syslog( nbrOfTeleporters +  " teleporters created." );
    return nbrOfTeleporters;
endfunction

function GoTeleporters(who, startat := 0)
    var fromX;
    var fromY;
    var fromZ;
    var fromR;

	var i := 0;
    foreach tele in teleporters
		if(i >= startat)
			if(!who.getprop("CheckingTeleporters"))
				return i;
			endif

			fromX := cint(tele[1]);
			fromY := cint(tele[2]);
			fromZ := cint(tele[3]);
            fromR := tele[7];

			// check -255.
			if ( fromZ == -255 )
				fromZ := GetWorldHeight( cint(fromx), cint(fromy), fromR );
			endif

			MoveObjectToLocation(who, fromX, fromY, fromZ, fromR, MOVEOBJECT_FORCELOCATION);
			sleep(3);
		endif
		i := i+1;
    endforeach
endfunction