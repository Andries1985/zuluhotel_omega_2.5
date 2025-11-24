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

    ////////////////////////////////
    ////////////////////////////////
    // Britannia
    // Britannia Teleporters
    ///////////////////////////////
    ///////////////////////////////
/*
    {6058,  272,  	-8,    	6095,   279,    44,     "britannia",    "britannia"},       // ctf1a
    {6058,  273,  	-8,    	6095,   279,    44,     "britannia",    "britannia"},       // ctf1b
    {6088,  274,  	58,    	6063,   278,    -22,    "britannia",    "britannia"},       // ctf2a
    {6089,  274, 	58,    	6063,   278,    -22,    "britannia",    "britannia"},       // ctf2b
    {1595, 	2489, 	20,		1601, 	2489, 	7,      "britannia",    "britannia"},       // spirit
	
// Dungeons
    {5242,  1007,   5,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
    {5243,  1007,   4,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
    {5244,  1007,   4,      1176,   2637,  	1,      "britannia",    "britannia"},       // Destard_ent_out_x[]
	{1175,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {1176,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {1177,  2636,   0,      5243,   1006,   0,      "britannia",    "britannia"},       // Destard_out_x[]
    {5129,  909,    -28,    5142,   798,   	17,     "britannia",    "britannia"},       // Destard_1_2_x[]
    {5130,  909,    -28,    5143,   798,   	17,     "britannia",    "britannia"},       // Destard_1_2_x[]
    {5131,  909,    -28,    5144,   798,   	17,     "britannia",    "britannia"},       // Destard_1_2_x[]
    {5132,  909,    -28,    5145,   798,   	17,     "britannia",    "britannia"},       // Destard_1_2_x[]
    {5142,  797,    21, 	5129,   908,   	-23,    "britannia",    "britannia"},       // Destard_2_1_x[]
    {5143,  797,    21,   	5130,   908,   	-23,    "britannia",    "britannia"},       // Destard_2_1_x[]
    {5144,  797,    20,   	5131,   908,   	-23,    "britannia",    "britannia"},       // Destard_2_1_x[]
    {5145,  797,    20, 	5132,   908,   	-23,    "britannia",    "britannia"},       // Destard_2_1_x[]
    {5153,  808,    -25,    5134,   984,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
    {5153,  809,    -25,    5134,   985,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
    {5153,  810,    -25,    5134,   986,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
	{5153,  811,    -25,    5134,   987,   	17,     "britannia",    "britannia"},       // Destard_2_3_x[]
	{5133,  984,    22,   	5152,   808,   	-19,    "britannia",    "britannia"},       // Destard_3_2_x[]
    {5133,  985,    22,   	5152,   809,   	-19,    "britannia",    "britannia"},       // Destard_3_2_x[]
    {5133,  986,    22,   	5152,   810,   	-19,    "britannia",    "britannia"},       // Destard_3_2_x[]
    {5133,  987,    22,   	5152,   811,   	-19,    "britannia",    "britannia"},       // Destard_3_2_x[]
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
    {1361,	883,    0,      5166,	244,    15,     "britannia",	"britannia"},       // Wind_out_x[]
    {5191,  152,    0,      1367,   890,   	0,      "britannia",	"britannia"},       // Wind_exit[]
    {5200,  71,     17,     5211,   22,    	15,     "britannia",	"britannia"},       // Wind_w_e_x[]
    {5217,  18,     15,     5204,   74,    	17,     "britannia",	"britannia"},       // Wind_e_w_x[]
	{4110,  431,    5, 		5187,   638,    1,      "britannia",	"britannia"},       // Deceit_out_x[]
    {4111,  431,    5, 		5188,   638,    1,		"britannia",	"britannia"},       // Deceit_out_x[]
    {4112,  431,    5, 		5189,   638,    1,		"britannia",    "britannia"},       // Deceit_out_x[]
	{5186,  639,    0, 	 	4110,   432,    5,		"britannia",    "britannia"},       // Deceit_ent_out_x[]
	{5187,  639,    0,   	4111,   432,   	5,		"britannia",	"britannia"},       // Deceit_ent_out_x[]
    {5188,  639,    0,   	4112,   432,   	5,		"britannia",	"britannia"},       // Deceit_ent_out_x[]
    {5189,  639,    0,   	4113,   432,   	5,		"britannia",	"britannia"},       // Deceit_ent_out_x[]
    {5216,  586,    -13,    5305,   533,   	6,		"britannia",	"britannia"},       // Deceit_1_2_x[]
    {5217,  586,    -13,    5305,   533,   	6,		"britannia",	"britannia"},       // Deceit_1_2_x[]
    {5218,  586,    -13,    5305,   533,   	6,		"britannia",	"britannia"},       // Deceit_1_2_x[]
	{5304,  532,    7,      5216,   585,    -8,		"britannia",	"britannia"},       // Deceit_2_1_x[]
	{5305,  532,    7,      5217,   585,    -8,		"britannia",	"britannia"},       // Deceit_2_1_x[]
	{5306,  532,    7,      5218,   585,    -8,		"britannia",	"britannia"},       // Deceit_2_1_x[]
    {5346,  578,    5,   	5137,   650,    6,		"britannia",	"britannia"},       // Deceit_2_3_x[]
    {5137,  649,    5,      5346,   579,   	6,		"britannia",	"britannia"},       // Deceit_3_2_x[]
    {5218,  762,    -33,   	5306,   652,    2,		"britannia",	"britannia"},       // Deceit_3_4_x[]
    {5219,  762,    -28,   	5305,   652,    2,		"britannia",	"britannia"},       // Deceit_3_4_x[]
    {5306,  650,    12,   	5218,   761,    -28,	"britannia",	"britannia"},       // Deceit_4_3_x[]
    {5305,  650,    12,   	5219,   761,    -28,	"britannia",	"britannia"},       // Deceit_4_3_x[]
    {512, 	1561, 	1, 		5394, 	126, 	0,	    "britannia",	"britannia"},       // Shame_out_x[]
    {513, 	1561, 	1, 		5395, 	126, 	0,  	"britannia",	"britannia"},       // Shame_out_x[]
    {514, 	1561, 	1, 		5396, 	126, 	0,	    "britannia",	"britannia"},       // Shame_out_x[]
	{5394,  127,    3,  	512,    1562,   0,	    "britannia",	"britannia"},       // Shame_ent_out_x[]
    {5395,  127,    3,  	513,    1562,   0,	    "britannia",	"britannia"},       // Shame_ent_out_x[]
    {5396,  127,    3,  	514,    1562,   0,	    "britannia",	"britannia"},       // Shame_ent_out_x[]
    {5490,  19,     -25,   	5515,   11,     5,	    "britannia",	"britannia"},       // Shame_1_2_x[]
    {5514,  10,     5,  	5489,   19,     -25,	"britannia",	"britannia"},       // Shame_2_1_x[]
    {5604,  102,    5,   	5514,   148,    21,	    "britannia",	"britannia"},       // Shame_2_3_x[]
    {5514,  147,    25,   	5604,   103,    5,	    "britannia",	"britannia"},       // Shame_3_2_y[]
    {5538,  170,    5,	   	5513,   175,    5,  	"britannia",	"britannia"},       // Shame_3_4_x[]
    {5513,  176,    5,   	5537,   170,    1,	    "britannia",	"britannia"},       // Shame_4_3_x[]
    {5507,  162,    5,  	5875,   18,     5,  	"britannia",	"britannia"},       // Shame_4_5_x[]
	{5507,  187,    0,   	5404,   43,     30, 	"britannia",	"britannia"},       // Shame_4_1_x[]
	{5875,  19,     -5,   	5507,   161,    5,	    "britannia",	"britannia"},       // Shame_5_4_x[]
	{5731,  90,     5,   	5741,   52,     0,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5741,  53,     0,   	5731,   91,     2,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5738,  48,     5,   	5718,   62,     0,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5711,  55,     0,   	5738,   47,     2,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5736,  48,     5,   	5702,   61,     0,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5695,  55,     0,   	5735,   47,     2,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5736,  50,     5,   	5702,   86,     0,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5695,  79,     0,   	5735,   50,     2,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5738,  50,     5,   	5726,   78,     0,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
	{5719,  71,     0,   	5737,   50,     5,	    "britannia",	"britannia"},       // Shame_5_5_isl[]
    {5588,  630,    30,     1298,   1081,   0,	    "britannia",	"britannia"},       // Despise_ent_out_x[]
    {5588,  631,    30,     1298,   1081,   0,	    "britannia",	"britannia"},       // Despise_ent_out_x[]
    {5588,  632,    30,     1298,   1081,   0,	    "britannia",	"britannia"},       // Despise_ent_out_x[]
    {1297,  1080,   0,      5587,   631,    30, 	"britannia",	"britannia"},       // Despise_out_x[]
    {1297,  1081,   0,      5587,   631,    30, 	"britannia",	"britannia"},       // Despise_out_x[]
    {1297,  1082,   0,      5587,   631,    30, 	"britannia",	"britannia"},       // Despise_out_x[]
    {5573,  628,    42,   	5501,   570,    57,	    "britannia",	"britannia"},       // Despise_ent_1_x[]
    {5573,  629,    42,   	5501,   570,    57,	    "britannia",	"britannia"},       // Despise_ent_1_x[]
    {5573,  630,    42,   	5501,   570,    57,	    "britannia",	"britannia"},       // Despise_ent_1_x[]
    {5504,  569,    46,     5576,   629,    30,	    "britannia",	"britannia"},       // Despise_1_ent_x[]
    {5504,  570,    46,     5576,   629,    30,	    "britannia",	"britannia"},       // Despise_1_ent_x[]
    {5504,  571,    46,     5576,   629,    30,	    "britannia",	"britannia"},       // Despise_1_ent_x[]
    {5521,  672,    27,     5575,   633,    27,	    "britannia",	"britannia"},       // Despise_2_ent_x[]
    {5521,  673,    27,     5575,   633,    27,	    "britannia",	"britannia"},       // Despise_2_ent_x[]
    {5521,  674,    27,     5575,   633,    27,	    "britannia",	"britannia"},       // Despise_2_ent_x[]
    {5573,  632,    22,   	5519,   673,    20,	    "britannia",	"britannia"},       // Despise_ent_2_x[]
    {5573,  633,    22,   	5519,   673,    20, 	"britannia",	"britannia"},       // Despise_ent_2_x[]
    {5573,  634,    22,   	5519,   673,    20, 	"britannia",	"britannia"},       // Despise_ent_2_x[]
    {5386,  755,    -8,     5407,   859,    45, 	"britannia",	"britannia"},       // Despise_2_3_x[]
    {5386,  756,    -8,     5407,   859,    45, 	"britannia",	"britannia"},       // Despise_2_3_x[]
    {5386,  757,    -8,     5407,   859,    45, 	"britannia",	"britannia"},       // Despise_2_3_x[]
    {5409,  858,    52,     5389,   756,    3,	    "britannia",	"britannia"},       // Despise_3_2_x[]
    {5409,  859,    52,     5389,   756,    3,	    "britannia",	"britannia"},       // Despise_3_2_x[]
    {5409,  860,    52,     5389,   756,    3,	    "britannia",	"britannia"},       // Despise_3_2_x[]
    {5455,  1863,   0,      2498,   919,    1,	    "britannia",	"britannia"},       // Covetous_ent_out_x[]
    {5456,  1863,   0,      2498,   919,    1,	    "britannia",	"britannia"},       // Covetous_ent_out_x[]
    {5457,  1863,   1,      2498,   919,    1,	    "britannia",	"britannia"},       // Covetous_ent_out_x[]
    {2498,  917,    0,      5457,   1861,   0,	    "britannia",	"britannia"},       // Covetous_out_x[]
    {2499,  917,    0,      5457,   1861,   0,	    "britannia",	"britannia"},       // Covetous_out_x[]
    {2500,  917,    0,      5457,   1861,   0,	    "britannia",	"britannia"},       // Covetous_out_x[]
    {5392,  1959,   0,      2421,   884,    1,	    "britannia",	"britannia"},       // Covetous_1_2_x[]
    {5393,  1959,   0,      2421,   884,    1,	    "britannia",	"britannia"},       // Covetous_1_2_x[]
	{5394,  1959,   1,      2421,   884,    1,	    "britannia",	"britannia"},       // Covetous_1_2_x[]
    {2420,  883,    0,      5392,   1958,   0,	    "britannia",	"britannia"},       // Covetous_2_1_x[]
    {2421,  883,    0,      5393,   1958,   0,	    "britannia",	"britannia"},       // Covetous_2_1_x[]
    {2422,  883,    0,      5394,   1958,   0,	    "britannia",	"britannia"},       // Covetous_2_1_x[]
    {2385,  836,    0,      5614,   1997,   0,	    "britannia",	"britannia"},       // Covetous_2_3_x[]
    {2385,  837,    0,      5614,   1997,   0,	    "britannia",	"britannia"},       // Covetous_2_3_x[]
    {2385,  838,    0,      5614,   1997,   0,	    "britannia",	"britannia"},       // Covetous_2_3_x[]
    {5615,  1996,   0,      2386,   837,    1,	    "britannia",	"britannia"},       // Covetous_3_2_x[]
    {5615,  1997,   0,      2386,   837,    1,	    "britannia",	"britannia"},       // Covetous_3_2_x[]
    {5615,  1998,   0,      2386,   837,    1,	    "britannia",	"britannia"},       // Covetous_3_2_x[]
	{5388,  2027,   0,      2455,   860,    0,	    "britannia",	"britannia"},       // Covetous_3_4_x[]
    {5389,  2027,   0,      2456,   860,    0,  	"britannia",	"britannia"},       // Covetous_3_4_x[]
    {5390,  2027,   1,      2457,   860,    0,  	"britannia",	"britannia"},       // Covetous_3_4_x[]
    {2455,  859,    0,      5388,   2026,   0,  	"britannia",	"britannia"},       // Covetous_4_3_x[]
    {2456,  859,    0,      5389,   2026,   0,  	"britannia",	"britannia"},       // Covetous_4_3_x[]
    {2457,  859,    0,      5390,   2026,   0,  	"britannia",	"britannia"},       // Covetous_4_3_x[]
    {2544,  847, 	0, 		5579, 	1924, 	0,  	"britannia",	"britannia"},       // Covetous_4_5_x[]
    {2545,  847,    0, 		5579, 	1924, 	0,      "britannia",	"britannia"},       // Covetous_4_5_x[]
    {2546,  847,    0, 		5579, 	1924, 	0,	    "britannia",	"britannia"},       // Covetous_4_5_x[]
    {5578,  1926,   0,      2545,   851,    1,  	"britannia",	"britannia"},       // Covetous_5_4_x[]
    {5579,  1926,   0,      2545,   851,    1,  	"britannia",	"britannia"},       // Covetous_5_4_x[]
    {5580,  1926,   0,      2545,   851,    1,  	"britannia",	"britannia"},       // Covetous_5_4_x[]
	{5556,  1827,   -13, 	5551,   1806,   7,	    "britannia",	"britannia"},       // Covetous_5_6a_x[]
	{5557,  1827,   -13, 	5552,   1806,   7,	    "britannia",	"britannia"},       // Covetous_5_6a_x[]
	{5551,  1805,   12,   	5556,   1826,   -8,	    "britannia",	"britannia"},       // Covetous_6a_5_x[]
    {5552,  1805,   12,   	5556,   1826,   -8,	    "britannia",	"britannia"},       // Covetous_6a_5_x[]
	{5553,  1805,   12,  	5557,   1826,   -8,	    "britannia",	"britannia"},       // Covetous_6a_5_x[]
	{5595,  1840,   -13, 	5467,   1804,   7,	    "britannia",	"britannia"},       // Covetous_5_6b_x[]
	{5595,  1841,   -13, 	5467,   1805,   7,	    "britannia",	"britannia"},       // Covetous_5_6b_x[]
    {5466,  1804,    12, 	5594,   1840,   -8,	    "britannia",	"britannia"},       // Covetous_6b_5_x[]
    {5466,  1805,    12, 	5594,   1841,   -8,	    "britannia",	"britannia"},       // Covetous_6b_5_x[]
	{5466,  1806,    12, 	5594,   1841,   -8,	    "britannia",	"britannia"},       // Covetous_6b_5_x[]
	{5823,  630,    0,		2041,   216,    14,	    "britannia",	"britannia"},       // Wrong_ent_x[]
    {5824,  630,    0,		2042,   216,    14,	    "britannia",	"britannia"},       // Wrong_ent_x[]
    {5825,  630,    0,		2043,   216,    14,	    "britannia",	"britannia"},       // Wrong_ent_x[]
	{2041,  215,    14,     5823,   629,    0,	    "britannia",	"britannia"},       // Wrong_out_x[]
    {2042,  215,    14,     5824,   629,    0,	    "britannia",	"britannia"},       // Wrong_out_x[]
    {2043,  215,    14,     5825,   629,    0,	    "britannia",	"britannia"},       // Wrong_out_x[]
    {5867,  529,    20,   	5690,   568,    25,	    "britannia",	"britannia"},       // Wrong_1_2_x[]
    {5690,  569,    25,   	5867,   530,    20,	    "britannia",	"britannia"},       // Wrong_2_1_x
	{5673,  1439,   0,  	5685,   1432,   22,	    "britannia",	"britannia"},       // Fire_dung_lvl_1_1[]
	{5686,  1432,   22,  	5674,   1439,   0,	    "britannia",	"britannia"},       // Fire_dung_lvl_1_1[]
    {5786,  1336,   -8,     5701,   1317,   6,	    "britannia",	"britannia"},       // Fire_dung_lvl_1_2[]
    {5787,  1336,   -8,     5702,   1317,   6,	    "britannia",	"britannia"},       // Fire_dung_lvl_1_2[]
    {5788,  1336,   -8,     5703,   1317,   6,	    "britannia",	"britannia"},       // Fire_dung_lvl_1_2[]
    {5701,  1320,   16,     5786,   1337,   -5,	    "britannia",	"britannia"},       // Fire_dung_lvl_2_1[]
    {5702,  1320,   16,     5787,   1337,   -5,	    "britannia",	"britannia"},       // Fire_dung_lvl_2_1[]
    {5703,  1320,   16,     5788,   1337,   -5,	    "britannia",	"britannia"},       // Fire_dung_lvl_2_1[]
	{5619,  1431,  	0,   	6677,  	110,  	0,	    "britannia",	"britannia"},       // Fire_Dung_mine_2_3_x[]
	{5620,  1431,  	0,   	6678,  	110,  	0,	    "britannia",	"britannia"},       // Fire_Dung_mine_2_3_x[]
	{5621,  1431,  	0,   	6678,  	110,  	0,	    "britannia",	"britannia"},       // Fire_Dung_mine_2_3_x[]
	{6677,  111,  	0,   	5619,  	1432,  	0,	    "britannia",	"britannia"},       // Fire_Dung_mine_3_2_x[]
	{6678,  111,  	0,   	5620,  	1432,  	0,	    "britannia",	"britannia"},       // Fire_Dung_mine_3_2_x[]
    {5827,  1503,   6,      2923,   3406,   8,	    "britannia",	"britannia"},       // Fire_Dung_to_Serp_hold[]
    {5828,  1503,   5,    	2923,   3406,   8,	    "britannia",	"britannia"},       // Fire_Dung_to_Serp_hold[]
    {5829,  1503,   6,      2923,   3406,   8,	    "britannia",	"britannia"},       // Fire_Dung_to_Serp_hold[]
	{5830,  1503,   6,      2923,   3406,   8,	    "britannia",	"britannia"},       // Fire_Dung_to_Serp_hold[]
    {2922,  3405,   7,      5827,   1502,   2,	    "britannia",	"britannia"},       // Serp_Hold_to_Fire_Dung[]
    {2923,  3405,   6,      5828,   1502,   1,	    "britannia",	"britannia"},       // Serp_Hold_to_Fire_Dung[]
    {2924,  3405,   7,      5829,   1502,   2,	    "britannia",	"britannia"},       // Serp_Hold_to_Fire_Dung[]
	{1997,  80,     7,      5880,   241,    0,	    "britannia",	"britannia"},       // N_Wrong_to_Ice_dngn[]
    {1997,  81,     7,      5880,   242,    0,	    "britannia",	"britannia"},       // N_Wrong_to_Ice_dngn[]
    {1997,  82,     6,      5880,   243,    -1, 	"britannia",	"britannia"},       // N_Wrong_to_Ice_dngn[]
	{5881,  241,    0,   	1998,   81,     5,	    "britannia",	"britannia"},       // Ice_dngn_to_N_Wrong[]
    {5881,  242,    0,   	1998,   81,     5,	    "britannia",	"britannia"},       // Ice_dngn_to_N_Wrong[]
    {5881,  243,    -1,   	1998,   81,     5,	    "britannia",	"britannia"},       // Ice_dngn_to_N_Wrong[]
    {5832,  323,    34,     5849,   237,    -24,	"britannia",	"britannia"},       // Orc_lvl_to_ice_dngn[]
    {5833,  323,    33,     5850,   237,    -24,	"britannia",	"britannia"},       // Orc_lvl_to_ice_dngn[]
    {5834,  323,    33,     5851,   237,    -24,	"britannia",	"britannia"},       // Orc_lvl_to_ice_dngn[]
    {5835,  323,    33,     5852,   237,    -25,	"britannia",	"britannia"},       // Orc_lvl_to_ice_dngn[]
    {5849,  238,    -25,    5832,   325,    25,     "britannia",	"britannia"},       // Ice_dngn_to_orc_lvl[]
    {5850,  238,    -25,    5833,   325,    25,     "britannia",	"britannia"},       // Ice_dngn_to_orc_lvl[]
    {5851,  238,    -25,    5834,   325,    25,     "britannia",	"britannia"},       // Ice_dngn_to_orc_lvl[]
    {5852,  238,    -25,    5835,   325,    25,     "britannia",	"britannia"},       // Ice_dngn_to_orc_lvl[]
    {5705,  145,    -45,    5705,   305,    10,     "britannia",	"britannia"},       // Ice_dngn_to_lvl2[]
    {5705,  146,    -45,    5705,   306,    10,     "britannia",	"britannia"},       // Ice_dngn_to_lvl2[]
    {5705,  147,    -45,    5705,   307,    10,     "britannia",	"britannia"},       // Ice_dngn_to_lvl2[]
    {5706,  305,    12,     5706,   145,    -44,	"britannia",	"britannia"},       // lvl2_to_ice_dngn[]
    {5706,  306,    12,     5706,   146,    -44,	"britannia",	"britannia"},       // lvl2_to_ice_dngn[]
    {5706,  307,    12,     5706,   147,    -45,	"britannia",	"britannia"},       // lvl2_to_ice_dngn[]
    {5706,  304,    12,     5706,   148,    -45,	"britannia",	"britannia"},       // lvl2_to_ice_dngn[]
	{4721,  3813,   0,   	5904,   28,     56,     "britannia",	"britannia"},       // Hythloth_out_x[]
    {4722,  3813,   0,   	5905,   28,     56,     "britannia",	"britannia"},       // Hythloth_out_x[]
    {4723,  3813,   0,   	5906,   28,     56,     "britannia",	"britannia"},       // Hythloth_out_x[]
	{5904,  29,     61,   	4721,   3815,   1,      "britannia",	"britannia"},       // Hythloth_ent_out_x[]
    {5905,  29,     61,   	4722,   3815,   1,      "britannia",	"britannia"},       // Hythloth_ent_out_x[]
    {5906,  29,     61,   	4723,   3815,   1,      "britannia",	"britannia"},       // Hythloth_ent_out_x[]
	{5973,  169,    0, 	    5905,   99,     0,      "britannia",	"britannia"},       // Hythloth_2_1_x[]	
    {5905,  97,     0, 	    5976,   169,    1,      "britannia",	"britannia"},       // Hythloth_1_2_x[]
	{5921,  168,    14,   	6083,   144,    -20,	"britannia",	"britannia"},       // Hythloth_2_3_x[]
    {5921,  169,    14,   	6083,   145,    -20,	"britannia",	"britannia"},       // Hythloth_2_3_x[]
    {5921,  170,    14,   	6083,   146,    -20,	"britannia",	"britannia"},       // Hythloth_2_3_x[]
	{6082,  144,    -15,  	5920,   168,    20,     "britannia",	"britannia"},       // Hythloth_3_2_x[]
    {6082,  145,    -15,  	5920,   169,    20,     "britannia",	"britannia"},       // Hythloth_3_2_x[]
    {6082,  146,    -15,  	5920,   170,    20,     "britannia",	"britannia"},       // Hythloth_3_2_x[]
	{6040,  192,    12,   	6059,   89,     24,     "britannia",	"britannia"},       // Hythloth_3_4_x[]
    {6040,  193,    12,   	6059,   89,     24,     "britannia",	"britannia"},       // Hythloth_3_4_x[]
    {6040,  194,    12,   	6059,   89,     24,     "britannia",	"britannia"},       // Hythloth_3_4_x[]
    {6058,  88,     29, 	6039,   193,    17,     "britannia",	"britannia"},       // Hythloth_4_3_x[]
    {6058,  89,     29, 	6039,   193,    17,     "britannia",	"britannia"},       // Hythloth_4_3_x[]
    {6058,  90,     29, 	6039,   193,    17,     "britannia",	"britannia"},       // Hythloth_4_3_x[]
	{6107,  179,    0, 		6117,   179,    0,      "britannia",	"britannia"},       // Hythloth_3_3_x[]
	{6115,  179,    0, 		6105,   179,    0,      "britannia",	"britannia"},       // Hythloth_3_3_x[]
	{2628,  45,     20,   	5656,   2016,   0,      "britannia",	"britannia"},       // Solen_ent_out_x[]
	{5659,  2019,   0,    	2626,   44,   	21,     "britannia",	"britannia"},       // Solen_ent_out_x[]
	{5661,  1955,   0,    	5709,   1955, 	0,      "britannia",	"britannia"},       // Solen_1_2_x[]
	{5709,  1953,   0,    	5658,   1958, 	0,	    "britannia",	"britannia"},       // Solen_2_1_x[]
	{5698,  1929,   0,    	5661,   1897, 	0,	    "britannia",	"britannia"},       // Solen_2_3_x[]
	{5662,  1899,   0,    	5699,   1928, 	0,	    "britannia",	"britannia"},       // Solen_3_2_x[]
	{5728,  1892,   0,    	5748,   1899, 	0,	    "britannia",	"britannia"},       // Solen_3_4_x[]
	{5751,  1896,   0,    	5725,   1895, 	0,	    "britannia",	"britannia"},       // Solen_4_3_x[]
	{5785,  1927,   0,    	5879,   1894, 	0,  	"britannia",	"britannia"},       // Solen_C_5b_x[]
	{5877,  1895,   0,    	5784,   1927, 	0,	    "britannia",	"britannia"},       // Solen_5b_C_x[]
	{5810,  1909,   0,    	5853,   1846, 	0,	    "britannia",	"britannia"},       // Solen_C_5c_x[]
	{5853,  1852,   0,    	5810,   1903, 	0,	    "britannia",	"britannia"},       // Solen_5c_C_x[]
	{5774,  1870,   0,    	5754,   1794, 	2,  	"britannia",	"britannia"},       // Solen_C_5d_x[]
	{5757,  1798,   7,    	5772,   1865, 	0,  	"britannia",	"britannia"},       // Solen_5d_C_x[]
	{5875,  1868,   2,    	5911,   1895, 	0,	    "britannia",	"britannia"},       // Solen_5c_6_x[]
	{5910,  1894,   0,    	5874,   1870, 	2,	    "britannia",	"britannia"},       // Solen_6_5c_x[]
	{5815,  2013,   0,    	5920,   2025, 	0,	    "britannia",	"britannia"},       // Solen_5b_6_x[]
	{5916,  2022,   0,    	5818,   2016, 	0,  	"britannia",	"britannia"},       // Solen_6_5b_x[]
	{5658,  1792,   0,    	5915,   1795, 	0,	    "britannia",	"britannia"},       // Solen_5d_6a_x[]
	{5919,  1798,   0,    	5655,   1789, 	0,	    "britannia",	"britannia"},       // Solen_6a_5d_x[]
	{5901,  1878,   0,    	5733,   1858, 	0,	    "britannia",	"britannia"},       // Solen_6a_7_x[]
	{5732,  1861,   0,    	5900,   1876, 	0,	    "britannia",	"britannia"},       // Solen_7_6a_x[]
	{1140,  2595,   0,    	5360,   1707, 	-65,    "britannia",	"britannia"},       // Terathan_ent_out_x[]
	{5362,  1706, 	-65,   	1138,   2595,   0,  	"britannia",	"britannia"},       // Terathan_ent_out_x[]
	{588,   1637, 	0,    	6489,   849,    40, 	"britannia",	"britannia"},       // Hiddentree_ent_out_x[]
	{6488,  848, 	40,    	588,    1638,   0,	    "britannia",	"britannia"},       // Hiddentree_ent_out_x[]
	{6488,  849, 	40,    	588,    1638,   0,	    "britannia",	"britannia"},       // Hiddentree_ent_out_x[]
	{6488,  850, 	40,    	588,    1638,   0,	    "britannia",	"britannia"},       // Hiddentree_ent_out_x[]
	{6511,  953, 	-40,   	6511,   948,    30,	    "britannia",	"britannia"},       // Hiddentree_1_1_x[]
	{6511,  949,    26,   	6511,   954,    -40,	"britannia",	"britannia"},       // Hiddentree_1_1_x[]
	{6529,  954, 	40,   	7047,   333,    10, 	"britannia",	"britannia"},       // Hiddentree_1_1_x[]
	{7046,  332,    10,   	6527,   954,    40, 	"britannia",	"britannia"},       // Hiddentree_1_1_x[]
	{6163,  7,    	4,   	1715,   1064,   0,	    "britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  8,    	4,   	1715,   1065,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  9,    	4,   	1715,   1066,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  10,    	4,   	1715,   1067,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  11,    	4,   	1715,   1068,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  12,    	4,   	1715,   1068,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{6163,  13,    	9,   	1715,   1069,   0,  	"britannia",	"britannia"},       // Raided_town_ent_out_x[]
	{1714,  1064,  	1,   	6162,   7,   	0,      "britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1714,  1065,  	1,   	6162,   8,   	0,  	"britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1714,  1066,  	1,   	6162,   9,   	0,  	"britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1714,  1067,  	1,   	6162,   10,  	0,      "britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1714,  1068,  	1,   	6162,   11,  	0,  	"britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1714,  1069,  	1,   	6162,   12,  	0,  	"britannia",	"britannia"},       // Raided_town_out_ent_x[]
	{1013,  1434,  	0,   	5139,   2017,  	0,  	"britannia",	"britannia"},       // Orc_mine_ent_out_x[]
	{1013,  1433,  	0,   	5139,   2016,  	0,  	"britannia",	"britannia"},       // Orc_mine_ent_out_x[]
	{5140,  2015,  	2,   	1014,   1433,  	0,  	"britannia",	"britannia"},       // Orc_mine_out_ent_x[]
	{5140,  2016,  	2,   	1014,   1433,  	0,  	"britannia",	"britannia"},       // Orc_mine_out_ent_x[]
	{5140,  2017,  	2,   	1014,   1434,  	0,  	"britannia",	"britannia"},       // Orc_mine_out_ent_x[]
	{5140,  2018,  	2,   	1014,   1434,  	0,  	"britannia",	"britannia"},       // Orc_mine_out_ent_x[]
	{5132,  1946,  	0,   	5136,   1914,  	0,  	"britannia",	"britannia"},       // Orc_mine_1_2_x[]
	{5133,  1946,  	0,   	5137,   1914,  	0,  	"britannia",	"britannia"},       // Orc_mine_1_2_x[]
	{5134,  1946,  	0,   	5138,   1914,  	0,  	"britannia",	"britannia"},       // Orc_mine_1_2_x[]
	{5135,  1946,  	0,   	5139,   1914,  	0,  	"britannia",	"britannia"},       // Orc_mine_1_2_x[]
	{5136,  1915,  	0,   	5132,   1947,  	0,  	"britannia",	"britannia"},       // Orc_mine_2_1_x[]
	{5137,  1915,  	0,   	5133,   1947,  	0,  	"britannia",	"britannia"},       // Orc_mine_2_1_x[]
	{5138,  1915,  	0,   	5134,   1947,  	0,  	"britannia",	"britannia"},       // Orc_mine_2_1_x[]
	{5139,  1915,  	0,   	5135,   1947,  	0,  	"britannia",	"britannia"},       // Orc_mine_2_1_x[]
	{5166,  1874,  	0,   	5363,  	1289,  	3,  	"britannia",	"britannia"},       // Orc_mine_2_3_x[]
	{5167,  1874,  	0,   	5364,  	1289,  	3,  	"britannia",	"britannia"},       // Orc_mine_2_3_x[]
	{5168,  1874,  	0,   	5364,  	1289,  	3,  	"britannia",	"britannia"},       // Orc_mine_2_3_x[]
	{5363,  1288,  	3,   	5166,  	1873,  	0,  	"britannia",	"britannia"},       // Orc_mine_3_2_x[]
	{5364,  1288,  	3,   	5167,  	1873,  	0,  	"britannia",	"britannia"},       // Orc_mine_3_2_x[]
	{5571,  1299, 	0,  	5331,   1380,   2,  	"britannia",	"britannia"},       // Omega_1_2_x[]
	{5572,  1299, 	0,    	5332,   1380,   2,      "britannia",	"britannia"},       // Omega_1_2_x[]
	{5331,  1381, 	7,    	5571,   1300,   0,  	"britannia",	"britannia"},       // Omega_2_1_x[]
	{5332,  1381, 	7,    	5572,   1300,   0,  	"britannia",	"britannia"},       // Omega_2_1_x[]
	{1437,  1226,  	0,   	6309,  	890,  	-1, 	"britannia",	"britannia"},       // Cave_out_ent_x[]
	{1437,  1227,  	0,   	6309,  	891,  	-2, 	"britannia",	"britannia"},       // Cave_out_ent_x[]
	{1437,  1228,  	0,   	6309,  	892,  	-2, 	"britannia",	"britannia"},       // Cave_out_ent_x[]
	{1437,  1229,  	0,   	6309,  	893,  	-1, 	"britannia",	"britannia"},       // Cave_out_ent_x[]
	{6310,  890,  	3,   	1438,  	1226,  	0,      "britannia",	"britannia"},       // Cave_out_ent_x[]
	{6310,  891,  	2,   	1438,  	1227,  	0,	    "britannia",	"britannia"},       // Cave_out_ent_x[]
	{6310,  892,  	2,   	1438,  	1228,  	0,	    "britannia",	"britannia"},       // Cave_out_ent_x[]
	{6310,  893,  	3,   	1438,  	1229,  	0,	    "britannia",	"britannia"},       // Cave_out_ent_x[]
	{3511,  2464,  	0,   	5946,  	1463,  	20, 	"britannia",	"britannia"},       // Ocllo_mine_ent_1_x[]
	{3512,  2464,  	0,   	5946,  	1463,  	20, 	"britannia",	"britannia"},       // Ocllo_mine_ent_1_x[]
	{5946,  1464,  	22,   	3511,  	2465,  	1,	    "britannia",	"britannia"},       // Ocllo_mine_1_ent_x[]
	
// towns
    {1133,  2230,   20,     1163,   2207,   60,     "britannia",	"britannia"},       // hedge_tele_h_t[]
    {1163,  2208,   60,     1133,   2231,   20,	    "britannia",	"britannia"},       // hedge_tele_t_h[]
	{1142,  3621,   5,      1414,   3828,   6,	    "britannia",	"britannia"},       // jhelom_ns_m_x[]
    {1406,  3996,   5,      1414,   3828,   6,	    "britannia",	"britannia"},       // jhelom_ns_m_x[]
    {1409,  3824,   5,      1124,   3623,   6,	    "britannia",	"britannia"},       // jhelom_m_n_x[]
    {1419,  3832,   5,      1466,   4015,   6,  	"britannia",	"britannia"},       // jhelom_m_s_x[]
    {1491,  1640,   24,     6032,   1498,   22, 	"britannia",	"britannia"},       // Britain_sewer_x[]
    {1491,  1642,   24,     6032,   1498,   22, 	"britannia",	"britannia"},       // Britain_sewer_x[]
    {6096,  1445,   10,     6100,   1445,   25, 	"britannia",	"britannia"},       // Britain_sewer_x[]
    {6097,  1445,   25,     6095,   1445,   7,	    "britannia",	"britannia"},       // Britain_sewer_x[]
    {6032,  1499,   31,     1490,   1640,   24, 	"britannia",	"britannia"},       // Britain_sewer_x[] to britain
    {6032,  1501,   31,     1490,   1640,   24,	    "britannia",	"britannia"},       // Britain_sewer_x[] to britain
    {1592,  2489,   17,     1602,   2488,   5,	    "britannia",	"britannia"},       // shrine_spirit_exit[]
    {1600,  2489,   10,     1595,   2490,   19, 	"britannia",	"britannia"},       // shrine_spirit_ent1[]
    {1600,  2490,   10,     1595,   2490,   19, 	"britannia",	"britannia"},       // shrine_spirit_ent2[]
    {4300,  968,    5,   	4442,   1122,   5,	    "britannia",	"britannia"},       // moonglow_b_x[]
    {4436,  1107,   5,   	4300,   992,    5,  	"britannia",	"britannia"},       // moonglow_3_x[]
    {4443,  1137,   5,   	4487,   1475,   5,  	"britannia",	"britannia"},       // moonglow_2_x[]
    {4449,  1107,   5,   	4539,   890,    28,	    "britannia",	"britannia"},       // moonglow_4_y[]
    {4449,  1115,   5,   	4671,   1135,   10, 	"britannia",	"britannia"},       // moonglow_1_x[]
    {4496,  1475,   15,  	4442,   1122,   5,	    "britannia",	"britannia"},       // moonglow_b_x[]
    {4540,  898,    32,  	4442,   1122,   5,  	"britannia",	"britannia"},       // moonglow_b_x[]
    {4663,  1134,   13,  	4442,   1122,   5,  	"britannia",	"britannia"},       // moonglow_b_x[]
    {2673, 	2278, 	-29, 	2673, 	2280, 	-40,	"britannia",	"britannia"},       // buc_den_down
    {2674, 	2278, 	-29, 	2674, 	2280, 	-40,	"britannia",	"britannia"},       // buc_den
    {2675, 	2278, 	-29, 	2675, 	2280, 	-40,	"britannia",	"britannia"},       // buc_den
    {2676, 	2278, 	-29, 	2676, 	2280, 	-40,	"britannia",	"britannia"},       // buc_den
    {2673, 	2279, 	-35, 	2673, 	2277, 	-20,	"britannia",	"britannia"},       // buc_den_up
    {2674, 	2279, 	-35, 	2674, 	2277, 	-20,	"britannia",	"britannia"},       // buc_den
    {2675, 	2279, 	-35, 	2675, 	2277, 	-20,	"britannia",	"britannia"},       // buc_den
    {2676, 	2279, 	-35, 	2676, 	2277, 	-20,	"britannia",	"britannia"},       // buc_den
    {2623, 	2274, 	-40, 	2623, 	2274, 	18,	    "britannia",	"britannia"},       // buc_den_exit_1
    {2623, 	2273, 	-40, 	2623, 	2273, 	18, 	"britannia",	"britannia"},       // buc_den    
    {2676, 	2241, 	-18, 	2673, 	2241, 	2,	    "britannia",	"britannia"},       // buc_den_exit_2
    {2676, 	2242, 	-18, 	2673, 	2242, 	2,  	"britannia",	"britannia"},       // buc_den_   
    {2649, 	2195, 	-14, 	2651, 	2195, 	5,  	"britannia",	"britannia"},       // buc_den_exit_3
    {2665, 	2074, 	-18, 	2669, 	2099, 	3,  	"britannia",	"britannia"},       // buc_den_exit_4
    {2666, 	2074, 	-18, 	2669, 	2099, 	3,  	"britannia",	"britannia"},       // buc_den
    {2773, 	2097, 	-20, 	2773, 	2097, 	5,	    "britannia",	"britannia"},       // buc_den_exit_5
	{3784, 	1100, 	-10, 	6486, 	123, 	-35,	"britannia",	"britannia"},       // nujelm_mine_down
	{3785, 	1100, 	-10, 	6486, 	123, 	-35,	"britannia",	"britannia"},       // nujelm_mine_down
	{6486, 	122, 	-35, 	3784, 	1099, 	-3,	    "britannia",	"britannia"},       // nujelm_mine_up
	{6469, 	97, 	-50, 	6476, 	75, 	-34,	"britannia",	"britannia"},       // nujelm_mine
	{6476, 	76, 	-35, 	6470, 	97, 	-50,	"britannia",	"britannia"},       // nujelm_mine
	{6477, 	94, 	-49, 	6504, 	88, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6503, 	88, 	0, 		6476, 	94, 	-49,	"britannia",	"britannia"},       // nujelm_mine
	{6518, 	90, 	0, 		6525, 	88, 	-10,	"britannia",	"britannia"},       // nujelm_mine
	{6524, 	88, 	-10,	6517, 	90, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6568, 	77, 	-10, 	6577, 	77, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6576, 	77, 	0, 		6567, 	77, 	-10,	"britannia",	"britannia"},       // nujelm_mine
	{6554, 	104, 	0, 		6550, 	117, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6550, 	116, 	0, 		6554, 	103, 	0,      "britannia",	"britannia"},       // nujelm_mine
	{6559, 	128, 	0, 		6553, 	144, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6553, 	143, 	0, 		6559, 	127, 	0,	    "britannia",	"britannia"},       // nujelm_mine
	{6577, 	160, 	10,		6576, 	177, 	30, 	"britannia",	"britannia"},       // nujelm_mine
	{6578, 	160, 	8,		6577, 	177, 	30, 	"britannia",	"britannia"},       // nujelm_mine
	{6576, 	176, 	30, 	6577, 	159, 	13,	    "britannia",	"britannia"},       // nujelm_mine
	{6577, 	176, 	30, 	6578, 	159, 	12, 	"britannia",	"britannia"},       // nujelm_mine
	{6589, 	178, 	25, 	6515, 	182, 	0,	    "britannia",	"britannia"},       // nujelm_mine_dung
	{6514, 	182, 	0, 		6589, 	179, 	25, 	"britannia",	"britannia"},       // nujelm_dung_mine
	{6541, 	116, 	-15,	6501, 	141, 	-20,	"britannia",	"britannia"},       // nujelm_boss_dung
	{6541, 	117, 	-15,	6501, 	141, 	-20,	"britannia",	"britannia"},       // nujelm_boss_dung
	{6540, 	138, 	-15, 	6501, 	131, 	-20,	"britannia",	"britannia"},       // nujelm_dung_boss
	{6540, 	139, 	-15, 	6501, 	131, 	-20,	"britannia",	"britannia"},       // nujelm_dung_boss
	{709, 	2238, 	-2,		682, 	2234, 	-2,     "britannia",	"britannia"},       // skara_brae_boats
	{683, 	2234, 	-2, 	710, 	2238, 	-2, 	"britannia",	"britannia"},       // skara_brae_boats
	{683, 	2242, 	-2, 	710, 	2238, 	-2, 	"britannia",	"britannia"},       // skara_brae_boats	
	
// mines & caves
	{780, 	1646, 	0, 		5205, 	1993, 	0,	    "britannia",	"britannia"},       // mine_near_destard_0_1
	{781, 	1646, 	0, 		5206, 	1993, 	0,  	"britannia",	"britannia"},       // mine_near_destard_0_1
	{5205, 	1994, 	0, 		780, 	1647, 	0,	    "britannia",	"britannia"},       // mine_near_destard_1_0
	{5206, 	1994, 	0, 		781, 	1647, 	0,	    "britannia",	"britannia"},       // mine_near_destard_1_0
	{1988, 	2062, 	-40, 	5272, 	2042, 	3,	    "britannia",	"britannia"},       // mine_on_island_0_1
	{1989, 	2062, 	-40, 	5273, 	2042, 	3,	    "britannia",	"britannia"},       // mine_on_island_0_1
	{5272, 	2043, 	3, 		1988, 	2063, 	-40,   	"britannia",	"britannia"},       // mine_on_island_1_0
	{5273, 	2043, 	3, 		1989, 	2063, 	-40,	"britannia",	"britannia"},       // mine_on_island_1_0
	{4229, 	611, 	0, 		5269, 	1934, 	0,	    "britannia",	"britannia"},       // mine_on_ice_island_0_1
	{4230, 	611, 	0, 		5270, 	1934, 	0,	    "britannia",	"britannia"},       // mine_on_ice_island_0_1
	{5269, 	1935, 	0, 		4229, 	612, 	0,  	"britannia",	"britannia"},       // mine_on_ice_island_1_0
	{5270, 	1935, 	0, 		4230, 	612, 	0,      "britannia",	"britannia"},       // mine_on_ice_island_1_0
	{1104, 	1788, 	0, 		5166, 	1829, 	0,	    "britannia",	"britannia"},       // mine_near_britain_0_1
	{1104, 	1789, 	0, 		5166, 	1830, 	0,	    "britannia",	"britannia"},       // mine_near_britain_0_1
	{5167, 	1829, 	0, 		1105, 	1788, 	0,  	"britannia",	"britannia"},       // mine_near_britain_1_0
	{5167, 	1830, 	0, 		1105, 	1789, 	0,  	"britannia",	"britannia"},       // mine_near_britain_1_0	
	{2206, 	1247, 	0, 		5221, 	1862, 	0,	    "britannia",	"britannia"},       // mine_near_cove_0_1
	{2207, 	1247, 	0, 		5221, 	1862, 	0,	    "britannia",	"britannia"},       // mine_near_cove_0_1
	{2208, 	1247, 	0, 		5222, 	1862, 	0,	    "britannia",	"britannia"},       // mine_near_cove_0_1
	{2209, 	1247, 	0, 		5222, 	1862, 	0,	    "britannia",	"britannia"},       // mine_near_cove_0_1
	{5221, 	1863, 	0, 		2207, 	1248, 	0,	    "britannia",	"britannia"},       // mine_near_cove_1_0
	{5222, 	1863, 	0, 		2208, 	1248, 	0,	    "britannia",	"britannia"},       // mine_near_cove_1_0	
	{4268, 	3765, 	1, 		5237, 	2038, 	0,	    "britannia",	"britannia"},       // mine_on_avatar_0_1
	{5237, 	2039, 	0, 		4268, 	3766, 	0,	    "britannia",	"britannia"},       // mine_on_avatar_1_0
	{5238, 	2039, 	0, 		4268, 	3766, 	0,  	"britannia",	"britannia"},       // mine_on_avatar_1_0	
 	{1653, 	2963, 	0, 		1676, 	2987, 	0,	    "britannia",	"britannia"},       // tele_near_trinsic_0_1
 	{1677, 	2987, 	0, 		1654, 	2964, 	0,	    "britannia",	"britannia"},       // tele_near_trinsic_1_0
	{6022, 	1691, 	20, 	1715, 	2996, 	0,	    "britannia",	"britannia"},       // cave_near_trinsic_0_1
	{6022, 	1692, 	20, 	1715, 	2997, 	0,	    "britannia",	"britannia"},       // cave_near_trinsic_0_1
	{1714, 	2996, 	0, 		6021, 	1691, 	15, 	"britannia",	"britannia"},       // cave_near_trinsic_1_0
	{1714, 	2997, 	0, 		6021, 	1692, 	15, 	"britannia",	"britannia"},       // cave_near_trinsic_1_0
	{2664, 	250, 	0, 		5305, 	1858, 	0,	    "britannia",	"britannia"},       // mine_near_solen_0_1
	{2665, 	250, 	0, 		5306, 	1858, 	0,	    "britannia",	"britannia"},       // mine_near_solen_0_1
	{5305, 	1859, 	0, 		2664, 	251, 	0,	    "britannia",	"britannia"},       // mine_near_solen_1_0
	{5306, 	1859, 	0, 		2665, 	251, 	0,	    "britannia",	"britannia"},       // mine_near_solen_1_0
	{2398, 	198, 	0, 		5185, 	1941, 	0,	    "britannia",	"britannia"},       // mine_near_minoc_0_1
	{2399, 	198, 	0, 		5186, 	1941, 	0,	    "britannia",	"britannia"},       // mine_near_minoc_0_1
	{5185, 	1942, 	0, 		2398, 	199, 	0,	    "britannia",	"britannia"},       // mine_near_minoc_1_0
	{5186, 	1942, 	0, 		2399, 	199, 	0,  	"britannia",	"britannia"},       // mine_near_minoc_1_0
*/
    ////////////////////////////////
    ////////////////////////////////
    // Sosaria
    // City Teleporters
    ///////////////////////////////
    ///////////////////////////////

    //Khaz Darak
    {2546,  1766,   -13,    5552,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance - Fixed for New Map
    {2547,  1766,  -13,     5552,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2548,  1766,  -13,     5553,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2549,  1766,  -13,     5554,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2550,  1766,  -13,     5555,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2551,  1766,  -13,     5556,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2552,  1766,  -13,     5557,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2553,  1766,  -13,     5558,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {2554,  1766,  -13,     5558,   1169,   22,     "britannia_alt",    "britannia_alt"},   //Khaz Darak Mountain Entrance
    {5552,  1170,  22,      2546,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance - Fixed for New Map
    {5553,  1170,  22,      2547,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5554,  1170,  22,      2548,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5555,  1170,  22,      2549,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5556,  1170,  22,      2550,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5557,  1170,  22,      2551,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5558,  1170,  22,      2552,   1767,   -8,     "britannia_alt",    "britannia_alt"},   //Khaz Darak South Entrance
    {5476,  1175,   0,      5550,   1194,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining - Fixed for New Map
    {5477,  1175,   0,      5551,   1194,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5478,  1175,   0,      5552,   1194,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5479,  1175,   0,      5553,   1194,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5480,  1175,   0,      5554,   1194,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5550,  1193,   0,      5476,   1174,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5551,  1193,   0,      5477,   1174,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5552,  1193,   0,      5478,   1174,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5553,  1193,   0,      5479,   1174,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining
    {5554,  1193,   0,      5480,   1174,   0,      "britannia_alt",    "britannia_alt"},   //Khaz Darak Mining

    //Shandalaar
    {1396,  1866,   10,     6535,  141,     11,     "britannia_alt",    "britannia_alt"},   //To Shandalaar Bank Basement - Fixed for New Map
    {1397,  1866,   10,     6536,  141,     11,     "britannia_alt",    "britannia_alt"},   //To Shandalaar Bank Basement
    {6535,  142,    11,     1396,  1867,    12,     "britannia_alt",    "britannia_alt"},   //From Shandalaar Bank Basement - Fixed for New Map
    {6536,  142,    11,     1397,  1867,    12,     "britannia_alt",    "britannia_alt"},   //From Shandalaar Bank Basement
    {1307,  1810,   27,     1310,  1811,    32,     "britannia_alt",    "britannia_alt"},   //Shandalaar Wall Fix - Fixed for New Map
    {1309,  1811,   34,     1306,  1810,    27,     "britannia_alt",    "britannia_alt"},   //Shandalaar Wall Fix
    {1363,  1884,  0,       5217,  422,     1,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining - Fixed for New Map
    {1364,  1884,  0,       5218,  422,     1,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {1365,  1884,  0,       5219,  422,     1,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {1366,  1884,  0,       5220,  422,     1,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {1367,  1884,  0,       5221,  422,     1,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {5217,  423,  0,        1363,  1885,    0,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {5218,  423,  0,        1364,  1885,    0,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {5219,  423,  0,        1365,  1885,    0,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {5220,  423,  0,        1366,  1885,    0,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {5221,  423,  0,        1367,  1885,    0,      "britannia_alt",    "britannia_alt"},   //Shandalaar Mining
    {1796,  1870,   30,     1835,   1858,   30,     "britannia_alt",    "britannia_alt"},   //Shandalaar Water Passage Tower - Fixed for New Map
    {1834,  1858,   30,     1795,   1870,   30,     "britannia_alt",    "britannia_alt"},   //Shandalaar Water Passage Tower

    //Nexus
    {5939, 1989, 0, 5278, 2127, 0, "britannia_alt", "britannia_alt" }, //Nexus Mine -> Nexus - Fixed for New Map
    {5939, 1990, 0, 5278, 2128, 0, "britannia_alt", "britannia_alt" }, //Nexus Mine -> Nexus
    {5939, 1991, 0, 5278, 2129, 0, "britannia_alt", "britannia_alt" }, //Nexus Mine -> Nexus
    {5939, 1992, 0, 5278, 2130, 0, "britannia_alt", "britannia_alt" }, //Nexus Mine -> Nexus
    {5939, 1993, 0, 5278, 2131, 0, "britannia_alt", "britannia_alt" }, //Nexus Mine -> Nexus
    {5277, 2127, 0, 5938, 1989, 0, "britannia_alt", "britannia_alt" }, //Nexus -> Nexus Mine - Fixed for New Map
    {5277, 2128, 0, 5938, 1990, 0, "britannia_alt", "britannia_alt" }, //Nexus -> Nexus Mine
    {5277, 2129, 0, 5938, 1991, 0, "britannia_alt", "britannia_alt" }, //Nexus -> Nexus Mine
    {5277, 2130, 0, 5938, 1992, 0, "britannia_alt", "britannia_alt" }, //Nexus -> Nexus Mine
    {5277, 2131, 0, 5938, 1993, 0, "britannia_alt", "britannia_alt" }, //Nexus -> Nexus Mine

    //Nexus Outpost
    {5857,  390,    1,      5918,  549,     5,      "britannia_alt",    "britannia_alt"},   //Nexus Outpost to Outpost Mine - Fixed for New Map
    {5857,  391,    2,      5918,  550,     3,      "britannia_alt",    "britannia_alt"},   //Nexus Outpost to Outpost Mine
    {5919,  549,    4,      5858,  390,     1,      "britannia_alt",    "britannia_alt"},   //Nexus Outpost Mine to Outpost
    {5919,  550,    4,      5858,  391,     2,      "britannia_alt",    "britannia_alt"},   //Nexus Outpost Mine to Outpost

    //Dracham
    {572,  1973,  0,  5872,  759,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining - Fixed for New Map
    {572,  1974,  0,  5872,  760,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {572,  1975,  0,  5872,  761,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {572,  1976,  0,  5872,  762,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {5873,  759,  0,  573,  1973,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {5873,  760,  0,  573,  1974,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {5873,  761,  0,  573,  1975,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining
    {5873,  762,  0,  573,  1976,  0,     "britannia_alt",    "britannia_alt"},   //Dracham Mining

    //Faymoor
    {3552,  1453,  20,  5843,  1857,  0,     "britannia_alt",    "britannia_alt"}, //Faymoor Mining - Fixed for New Map
    {3553,  1453,  20,  5844,  1857,  0,     "britannia_alt",    "britannia_alt"}, //Faymoor Mining
    {5843,  1858,  0,  3552,  1454,  20,     "britannia_alt",    "britannia_alt"}, //Faymoor Mining
    {5844,  1858,  0,  3553,  1454,  20,     "britannia_alt",    "britannia_alt"}, //Faymoor Mining

    //Winterwyn
    {1068,  629,  20,  5184,  1160,  0,     "britannia_alt",    "britannia_alt"},   //Winterwyn Mining - Need added back to map
    {1069,  629,  20,  5183,  1160,  0,     "britannia_alt",    "britannia_alt"},   //Winterwyn Mining
    {5183,  1161,  0,  1068,  630,  20,     "britannia_alt",    "britannia_alt"},   //Winterwyn Mining
    {5184,  1161,  0,  1069,  630,  20,     "britannia_alt",    "britannia_alt"},   //Winterwyn Mining

    //Sunken City
    { 1908, 3665, 60, 1979, 3651, 5, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Mainland - Fixed for New Map
    { 1976, 3651, 5, 1907, 3665, 60, "britannia_alt", "britannia_alt" }, //Mainland -> Sunken City Hub
    { 1908, 3641, 30, 1965, 3793, 10, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1965, 3794, 10, 1909, 3640, 30, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1896, 3653, 5, 1832, 3755, 15, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1832, 3758, 10, 1895, 3654, 5, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1884, 3665, 30, 1850, 3638, 10, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1850, 3638, 10, 1885, 3665, 30, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1896, 3677, 5, 1984, 3706, 10, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1985, 3704, 10, 1898, 3677, 5, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1908, 3689, 30, 1970, 3762, 5, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1968, 3762, 10, 1908, 3690, 30, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1920, 3677, 5, 1897, 3778, 7, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1897, 3776, 10, 1920, 3675, 5, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1932, 3665, 30, 1923, 3735, 10, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1923, 3733, 10, 1933, 3665, 30, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub
    { 1920, 3653, 5, 1871, 3589, 65, "britannia_alt", "britannia_alt" }, //Sunken City Hub -> Sunken City Island
    { 1870, 3591, 65, 1918, 3653, 5, "britannia_alt", "britannia_alt" }, //Sunken City Island -> Sunken City Hub

    ////////////////////////////////
    ////////////////////////////////
    // Sosaria
    // Points of Interest Teleporters
    ///////////////////////////////
    ///////////////////////////////

    //Great Maze
    { 2785, 1956, 8, 2784, 1985, 7, "britannia_alt", "britannia_alt" }, //Great Maze  - Fixed for New Map

    //Waygate of Khaz Darak
    { 3005, 1920, 85, 3127, 1923, 87, "britannia_alt", "britannia_alt" }, //Mainland to Faymoor Island  - Fixed for New Map
    { 3005, 1919, 85, 3127, 1922, 87, "britannia_alt", "britannia_alt" }, //Mainland to Faymoor Island
    { 3126, 1923, 85, 3004, 1920, 85, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mainland
    { 3126, 1922, 85, 3004, 1919, 85, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mainland
    { 3005, 1907, 85, 3127, 1910, 87, "britannia_alt", "britannia_alt" }, //Mainland to Faymoor Island
    { 3005, 1908, 85, 3127, 1911, 87, "britannia_alt", "britannia_alt" }, //Mainland to Faymoor Island
    { 3126, 1910, 85, 3004, 1907, 85, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mainland
    { 3126, 1911, 85, 3004, 1908, 85, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mainland

    //Stonemoor
    { 786, 1473, -20, 5129, 798, 0, "britannia_alt", "britannia_alt" }, //Mainland to Stonemoor Mine  - Fixed for New Map
    { 786, 1474, -20, 5129, 799, 0, "britannia_alt", "britannia_alt" }, //Mainland to Stonemoor Mine
    { 786, 1475, -20, 5129, 800, 0, "britannia_alt", "britannia_alt" }, //Mainland to Stonemoor Mine
    { 786, 1476, -20, 5129, 801, 0, "britannia_alt", "britannia_alt" }, //Mainland to Stonemoor Mine
    { 5128, 798, 0, 785, 1473, -20, "britannia_alt", "britannia_alt" }, //Stonemoor Mine to Mainland
    { 5128, 799, 0, 785, 1473, -20, "britannia_alt", "britannia_alt" }, //Stonemoor Mine to Mainland
    { 5128, 800, 0, 785, 1474, -20, "britannia_alt", "britannia_alt" }, //Stonemoor Mine to Mainland
    { 5128, 801, 0, 785, 1475, -20, "britannia_alt", "britannia_alt" }, //Stonemoor Mine to Mainland
    { 5128, 802, 0, 785, 1476, -20, "britannia_alt", "britannia_alt" }, //Stonemoor Mine to Mainland
    
    ////////////////////////////////
    ////////////////////////////////
    // Sosaria
    // Dungeon Teleporters
    ///////////////////////////////
    ///////////////////////////////

    //Shandalaar
    {1311,  1996,   -27,    5424,  486,     10,     "britannia_alt",    "britannia_alt"},   //To Shandalaar Blood Sewers West City Entrance  - Fixed for New Map
    {1328,  2129,   0,      5401,  704,     -24,    "britannia_alt",    "britannia_alt"},   //To Shandalaar Blood Sewers Graveyard Entrance
    {1328,  2128,   0,      5401,  703,     -24,    "britannia_alt",    "britannia_alt"},   //To Shandalaar Blood Sewers Graveyard Entrance
    {1489,  1800,   5,      5430,  407,     10,     "britannia_alt",    "britannia_alt"},   //To Shandalaar Blood Sewers North City Entrance
    {5429,  407,    15,     1489,  1801,    12,     "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to North City Entrance
    {5423,  486,    15,     1311,  1997,    -20,    "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to West City Entrance
    {5423,  486,    15,     1311,  1997,    -20,    "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to West City Entrance
    {5402,  702,    -25,    1329,  2128,    0,      "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to Graveyard Entrance
    {5402,  703,    -25,    1329,  2129,    0,      "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to Graveyard Entrance
    {5402,  704,    -25,    1329,  2129,    0,      "britannia_alt",    "britannia_alt"},   //From Shandalaar Blood Sewers to Graveyard Entrance
    {1248,  1794,   -38,    5266,  1579,    2,      "britannia_alt",    "britannia_alt"},   //Shandalaar To Shandalaar Cathedral Dungeon  - Fixed for New Map
    {1248,  1795,   -38,    5266,  1580,    2,      "britannia_alt",    "britannia_alt"},   //Shandalaar To Shandalaar Cathedral Dungeon
    {1248,  1796,   -38,    5266,  1581,    2,      "britannia_alt",    "britannia_alt"},   //Shandalaar To Shandalaar Cathedral Dungeon
    {1248,  1797,   -38,    5266,  1582,    2,      "britannia_alt",    "britannia_alt"},   //Shandalaar To Shandalaar Cathedral Dungeon
    {5267,  1579,   7,      1249,  1794,    -33,    "britannia_alt",    "britannia_alt"},   //Shandalaar Cathedral Dungeon to Shandalaar
    {5267,  1580,   7,      1249,  1795,    -33,    "britannia_alt",    "britannia_alt"},   //Shandalaar Cathedral Dungeon to Shandalaar
    {5267,  1581,   7,      1249,  1796,    -33,    "britannia_alt",    "britannia_alt"},   //Shandalaar Cathedral Dungeon to Shandalaar
    {5267,  1582,   7,      1249,  1797,    -33,    "britannia_alt",    "britannia_alt" },  //Shandalaar Cathedral Dungeon to Shandalaar

    //Nexus
    {5742, 3307, 0, 5373,  2120, 0, "britannia_alt", "britannia_alt"}, //Underworld to Nexus - Fixed for New Map
    {5743, 3307, 0, 5374,  2120, 0, "britannia_alt", "britannia_alt"}, //Underworld to Nexus
    {5744, 3307, 0, 5375,  2120, 0, "britannia_alt", "britannia_alt"}, //Underworld to Nexus
    {5745, 3307, 0, 5376,  2120, 0, "britannia_alt", "britannia_alt"}, //Underworld to Nexus
    {5373, 2121, 0, 5742,  3308, 0, "britannia_alt", "britannia_alt"}, //Nexus to Underworld - Fixed for New Map
    {5374, 2121, 0, 5743,  3308, 0, "britannia_alt", "britannia_alt"}, //Nexus to Underworld
    {5375, 2121, 0, 5744,  3308, 0, "britannia_alt", "britannia_alt"}, //Nexus to Underworld
    {5376, 2121, 0, 5745,  3308, 0, "britannia_alt", "britannia_alt"}, //Nexus to Underworld

    //Khaz Darak
    {6724,  3280,  18,      5604,   1118,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance - Fixed for New Map
    {6724,  3281,  18,      5604,   1119,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {6724,  3282,  18,      5604,   1120,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {6724,  3283,  18,      5604,   1121,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {6724,  3284,  18,      5604,   1122,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {6724,  3285,  18,      5604,   1123,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {6724,  3286,  18,      5604,   1124,   -3,     "britannia_alt",    "britannia_alt"},   //Khaz Darak to Underworld Dungeon Entrance
    {5605,  1118,  -8,      6725,   3280,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1119,  -8,      6725,   3281,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1120,  -8,      6725,   3282,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1121,  -8,      6725,   3283,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1122,  -8,      6725,   3284,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1123,  -8,      6725,   3285,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    {5605,  1124,  -8,      6725,   3286,   13,     "britannia_alt",    "britannia_alt"},   //Underworld to Khaz Darak East Entrance
    { 5539, 1050, 0, 5553, 1483, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak -> Kaz Darak Ruins - Fixed for New Map
    { 5540, 1050, 0, 5554, 1483, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak -> Kaz Darak Ruins
    { 5541, 1050, 0, 5555, 1483, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak -> Kaz Darak Ruins
    { 5542, 1050, 0, 5556, 1483, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak -> Kaz Darak Ruins
    { 5543, 1050, 0, 5557, 1483, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak -> Kaz Darak Ruins
    { 5553, 1484, 0, 5539, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak
    { 5554, 1484, 0, 5540, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak
    { 5555, 1484, 0, 5541, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak
    { 5556, 1484, 0, 5542, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak
    { 5557, 1484, 0, 5543, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak
    { 5558, 1484, 0, 5544, 1051, 0, "britannia_alt", "britannia_alt" }, //Kaz Darak Ruins -> Kaz Darak

    //Winterwyrn Dungeon (Needs a Name)
    {820,  573,  -35,  5484,  892,  75,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance - Fixed for New Map
    {820,  574,  -35,  5484,  893,  75,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance
    {820,  575,  -35,  5484,  894,  75,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance
    {5485,  892,  77,  821,  573,  -30,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance
    {5485,  893,  77,  821,  574,  -30,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance
    {5485,  894,  77,  821,  575,  -30,     "britannia_alt",    "britannia_alt"},  //Winterwyn Dungeon Entrance

    //Mausoleum of Aeloria Veyn
    { 3609, 1988, 5, 5737, 1891, 10, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mausoleum of Aeloria Veyn - Fixed for New Map
    { 3609, 1989, 5, 5737, 1892, 10, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mausoleum of Aeloria Veyn
    { 3609, 1990, 5, 5737, 1893, 10, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mausoleum of Aeloria Veyn
    { 3609, 1991, 5, 5737, 1894, 10, "britannia_alt", "britannia_alt" }, //Faymoor Island to Mausoleum of Aeloria Veyn
    { 5738, 1891, 10, 3610, 1988, 5, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn to Faymoor Island
    { 5738, 1892, 10, 3610, 1989, 5, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn to Faymoor Island
    { 5738, 1893, 10, 3610, 1990, 5, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn to Faymoor Island
    { 5738, 1894, 10, 3610, 1991, 5, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn to Faymoor Island
    { 5770, 1787, 12, 5736, 1787, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl1 -> Lvl2 - Fixed for New Map
    { 5771, 1787, 12, 5737, 1787, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl1 -> Lvl2
    { 5772, 1787, 12, 5737, 1787, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl1 -> Lvl2
    { 5773, 1787, 12, 5738, 1787, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl1 -> Lvl2
    { 5736, 1788, 0, 5770, 1788, 7, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl2 -> Lvl1
    { 5737, 1788, 0, 5771, 1788, 7, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl2 -> Lvl1
    { 5738, 1788, 0, 5772, 1788, 7, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl2 -> Lvl1
    { 5686, 1884, 10, 5762, 1702, 7, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Lvl2 -> Boss - Fixed for New Map
    { 5762, 1701, 10, 5686, 1880, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Boss -> Lvl2
    { 5740, 1723, 10, 5686, 1880, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Boss -> Lvl2
    { 5762, 1745, 10, 5686, 1880, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Boss -> Lvl2
    { 5784, 1723, 10, 5686, 1880, 0, "britannia_alt", "britannia_alt" }, //Mausoleum of Aeloria Veyn Boss -> Lvl2

    //Great Pyramid
    { 2398, 2384, -23, 5741, 267, 60, "britannia_alt", "britannia_alt" }, //Great Pyramid Entrance - Fixed for New Map
    { 2398, 2385, -23, 5741, 268, 60, "britannia_alt", "britannia_alt" }, //Great Pyramid Entrance
    { 5742, 267, 60, 2399, 2384, -18, "britannia_alt", "britannia_alt" }, //Great Pyramid Exit
    { 5742, 268, 60, 2399, 2384, -18, "britannia_alt", "britannia_alt" }, //Great Pyramid Exit
    { 5742, 269, 60, 2399, 2385, -18, "britannia_alt", "britannia_alt" }, //Great Pyramid Exit
    { 5742, 270, 60, 2399, 2385, -18, "britannia_alt", "britannia_alt" }, //Great Pyramid Exit

    //Tomb of Hashut
    { 5648, 271, 40, 2447, 2904, 0, "britannia_alt", "britannia_alt" }, //Tomb of Hashut to City of Rust - Fixed for New Map
    { 5649, 271, 40, 2447, 2904, 0, "britannia_alt", "britannia_alt" }, //Tomb of Hashut to City of Rust
    { 5650, 271, 40, 2448, 2904, 0, "britannia_alt", "britannia_alt" }, //Tomb of Hashut to City of Rust
    { 5651, 271, 40, 2449, 2904, 0, "britannia_alt", "britannia_alt" }, //Tomb of Hashut to City of Rust
    { 2447, 2903, 0, 5648, 270, 39, "britannia_alt", "britannia_alt" }, //City of Rust to Tomb of Hashut
    { 2448, 2903, 0, 5649, 270, 39, "britannia_alt", "britannia_alt" }, //City of Rust to Tomb of Hashut
    { 2449, 2903, 2, 5650, 270, 39, "britannia_alt", "britannia_alt" }, //City of Rust to Tomb of Hashut

    //Spider Area
    { 1553, 2720, -68, 7138, 3417, 0, "britannia_alt", "britannia_alt" }, //Spider Area Entrance -> Underworld - Fixed for New Map
    { 7139, 3416, 0, 1554, 2720, -63, "britannia_alt", "britannia_alt" }, //Underworld -> Spider Area Entrance
    { 7139, 3417, 0, 1554, 2720, -63, "britannia_alt", "britannia_alt" }, //Underworld -> Spider Area Entrance
    { 7139, 3418, 0, 1554, 2720, -63, "britannia_alt", "britannia_alt" }, //Underworld -> Spider Area Entrance

    //Crystal Dungeon
    { 7096, 3262, 0, 5616, 101, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Crystal Dungeon Entrance - Fixed for New Map
    { 7096, 3263, 0, 5616, 102, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Crystal Dungeon Entrance
    { 7096, 3264, 0, 5616, 103, 6, "britannia_alt", "britannia_alt" }, //Underworld -> Crystal Dungeon Entrance
    { 5615, 101, 0, 7095, 3262, 0, "britannia_alt", "britannia_alt" }, //Crystal Dungeon Exit -> Underworld
    { 5615, 102, 1, 7095, 3263, 0, "britannia_alt", "britannia_alt" }, //Crystal Dungeon Exit -> Underworld
    { 5615, 103, 7, 7095, 3264, 0, "britannia_alt", "britannia_alt" }, //Crystal Dungeon Exit -> Underworld
    { 5864, 75, 0, 5841, 55, 1, "britannia_alt", "britannia_alt" }, //Boss -> Crystal Dungeon Lvl1 - Fixed for New Map
    { 5842, 55, 1, 5865, 75, 0, "britannia_alt", "britannia_alt" }, //Crystal Dungeon Lvl1 -> Boss

    //Black City of the Damned
    { 5342, 961, 15, 1085, 2910, 27, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Exit - Fixed for New Map
    { 5343, 961, 15, 1086, 2910, 27, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Exit
    { 5344, 961, 15, 1087, 2910, 27, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Exit
    { 5345, 961, 15, 1088, 2910, 27, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Exit
    { 1085, 2911, 22, 5342, 962, 15, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Entrance
    { 1086, 2911, 22, 5343, 962, 15, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Entrance
    { 1087, 2911, 22, 5344, 962, 15, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Entrance
    { 1088, 2911, 22, 5345, 962, 15, "britannia_alt", "britannia_alt" }, //Black City of the Damned Dungeon Entrance
    { 5269, 993, 0, 5688, 3957, 0, "britannia_alt", "britannia_alt" }, //Black City of the Damned -> Underworld - Fixed for New Map
    { 5269, 994, 0, 5688, 3958, 0, "britannia_alt", "britannia_alt" }, //Black City of the Damned -> Underworld
    { 5269, 995, 0, 5688, 3959, 0, "britannia_alt", "britannia_alt" }, //Black City of the Damned -> Underworld
    { 5269, 996, 0, 5688, 3960, 0, "britannia_alt", "britannia_alt" }, //Black City of the Damned -> Underworld
    { 5687, 3957, 0, 5268, 993, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Black City of the Damned
    { 5687, 3958, 0, 5268, 994, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Black City of the Damned
    { 5687, 3959, 0, 5268, 995, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Black City of the Damned
    { 5687, 3960, 0, 5268, 996, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Black City of the Damned

    //Ancient Pyramid of Nehekharas
    { 3429, 3523, -20, 6214, 413, 0, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Entrance - Fixed for New Map
    { 3430, 3523, -20, 6215, 413, 0, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Entrance
    { 3431, 3523, -20, 6216, 413, 0, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Entrance
    { 6214, 414, 0, 3429, 3524, -18, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Exit
    { 6215, 414, 0, 3430, 3524, -18, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Exit
    { 6216, 414, 0, 3431, 3524, -18, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Exit
    { 6214, 295, 0, 6356, 772, 32, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl1 -> Lvl2 - Fixed for New Map
    { 6215, 295, 0, 6356, 772, 32, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl1 -> Lvl2
    { 6216, 295, 0, 6357, 772, 32, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl1 -> Lvl2
    { 6356, 773, 35, 6214, 296, 0, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl2 -> Lvl1
    { 6357, 773, 35, 6215, 296, 0, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl2 -> Lvl1
    { 6544, 931, 12, 6495, 815, 55, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Lvl2 -> Boss - Fixed for New Map
    { 6494, 815, 55, 6543, 931, 12, "britannia_alt", "britannia_alt" }, //Ancient Pyramid of Nehekharas Boss -> Lvl2

    //Poison Dungeon
    { 2025, 3454, -13, 5944, 863, 17, "britannia_alt", "britannia_alt" }, //Poison Dungeon Entrance - Fixed for New Map
    { 2026, 3454, -13, 5945, 863, 17, "britannia_alt", "britannia_alt" }, //Poison Dungeon Entrance
    { 2027, 3454, -13, 5946, 863, 17, "britannia_alt", "britannia_alt" }, //Poison Dungeon Entrance
    { 5944, 864, 22, 2025, 3455, -8, "britannia_alt", "britannia_alt" }, //Poison Dungeon Exit
    { 5945, 864, 22, 2026, 3455, -8, "britannia_alt", "britannia_alt" }, //Poison Dungeon Exit
    { 5946, 864, 22, 2027, 3455, -8, "britannia_alt", "britannia_alt" }, //Poison Dungeon Exit

    //Fire Dungeon
    { 2847, 1875, 5, 5450, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance - Fixed for New Map
    { 2848, 1875, 5, 5451, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance
    { 2849, 1875, 5, 5452, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance
    { 2850, 1875, 5, 5453, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance
    { 2851, 1875, 5, 5454, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance
    { 2852, 1875, 5, 5455, 2281, 0, "britannia_alt", "britannia_alt" }, //Fire Dungeon Entrance
    { 5450, 2282, 0, 2847, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5451, 2282, 0, 2848, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5452, 2282, 0, 2849, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5453, 2282, 0, 2850, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5454, 2282, 0, 2851, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5455, 2282, 0, 2852, 1876, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon Exit
    { 5382, 2443, 25, 5353, 2536, 15, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl1 -> lvl2 - Fixed for New Map
    { 5354, 2536, 15, 5383, 2443, 22, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl2 -> lvl1
    { 5286, 2221, 5, 5212, 2303, 10, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl2 -> lvl3 - Fixed for New Map
    { 5287, 2221, 5, 5213, 2303, 10, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl2 -> lvl3
    { 5212, 2304, 10, 5286, 2222, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl3 -> lvl2
    { 5213, 2304, 10, 5287, 2222, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon lvl3 -> lvl2
    { 6022, 3433, 5, 5440, 2496, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Fire Dungeon - Fixed for New Map
    { 6023, 3433, 5, 5441, 2496, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Fire Dungeon
    { 6024, 3433, 5, 5442, 2496, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Fire Dungeon
    { 5440, 2497, 0, 6022, 3434, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon -> Underworld
    { 5441, 2497, 0, 6023, 3434, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon -> Underworld
    { 5442, 2497, 0, 6024, 3434, 5, "britannia_alt", "britannia_alt" }, //Fire Dungeon -> Underworld

    //Caverns of Despair
    { 1978, 2408, 0, 5971, 1672, 32, "britannia_alt", "britannia_alt" }, //Caverns of Despair Entrance - Fixed for New Map
    { 1979, 2408, 0, 5972, 1672, 32, "britannia_alt", "britannia_alt" }, //Caverns of Despair Entrance
    { 1980, 2408, 0, 5973, 1672, 32, "britannia_alt", "britannia_alt" }, //Caverns of Despair Entrance
    { 5971, 1673, 37, 1978, 2409, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair Exit
    { 5972, 1673, 37, 1979, 2409, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair Exit
    { 5973, 1673, 37, 1980, 2409, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair Exit
    { 6004, 1652, 0, 5268, 30, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Unnamed Dungeon - Fixed for New Map
    { 6004, 1653, 0, 5268, 31, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Unnamed Dungeon
    { 6004, 1654, 0, 5268, 32, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Unnamed Dungeon
    { 6004, 1655, 0, 5268, 33, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Unnamed Dungeon
    { 6004, 1656, 0, 5268, 34, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Unnamed Dungeon
    { 5267, 30, 0, 6003, 1652, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon -> Caverns of Despair
    { 5267, 31, 0, 6003, 1653, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon -> Caverns of Despair
    { 5267, 32, 0, 6003, 1654, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon -> Caverns of Despair
    { 5267, 33, 0, 6003, 1655, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon -> Caverns of Despair
    { 5267, 34, 0, 6003, 1656, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon -> Caverns of Despair

    //Coven of the Eternal Night
    { 5890, 1465, 0, 6819, 1480, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Coven of the Eternal Night - Fixed for New Map
    { 5891, 1465, 0, 6820, 1480, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Coven of the Eternal Night
    { 5892, 1465, 0, 6821, 1480, 0, "britannia_alt", "britannia_alt" }, //Caverns of Despair -> Coven of the Eternal Night
    { 6819, 1481, 0, 5890, 1466, 0, "britannia_alt", "britannia_alt" }, //Coven of the Eternal Night -> Caverns of Despair
    { 6820, 1481, 0, 5891, 1466, 0, "britannia_alt", "britannia_alt" }, //Coven of the Eternal Night -> Caverns of Despair
    { 6821, 1481, 0, 5892, 1466, 0, "britannia_alt", "britannia_alt" }, //Coven of the Eternal Night -> Caverns of Despair

    //Lair of Golaroth
    { 6276, 3549, 5, 5270, 1431, 2, "britannia_alt", "britannia_alt" }, //Underworld -> Lair of Golaroth - Fixed for New Map
    { 6277, 3549, 5, 5271, 1431, 2, "britannia_alt", "britannia_alt" }, //Underworld -> Lair of Golaroth
    { 6278, 3549, 5, 5272, 1431, 2, "britannia_alt", "britannia_alt" }, //Underworld -> Lair of Golaroth
    { 6279, 3549, 5, 5273, 1431, 2, "britannia_alt", "britannia_alt" }, //Underworld -> Lair of Golaroth
    { 5270, 1432, 2, 6276, 3550, 5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth -> Underworld
    { 5271, 1432, 2, 6277, 3550, 5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth -> Underworld
    { 5272, 1432, 2, 6278, 3550, 5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth -> Underworld
    { 5273, 1432, 2, 6279, 3550, 5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth -> Underworld
    { 5200, 1212, -8, 5365, 1406, 13, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lvl2 - Fixed for New Map
    { 5201, 1212, -9, 5366, 1406, 10, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lvl2
    { 5202, 1212, -7, 5367, 1406, 11, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lvl2
    { 5203, 1212, -7, 5368, 1406, 12, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lvl2
    { 5204, 1212, -6, 5369, 1406, 14, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lvl2
    { 5365, 1407, 16, 5200, 1213, -5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Lvl1
    { 5366, 1407, 14, 5201, 1213, -4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Lvl1
    { 5367, 1407, 14, 5202, 1213, -5, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Lvl1
    { 5368, 1407, 16, 5203, 1213, -4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Lvl1
    { 5369, 1407, 16, 5204, 1213, -4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Lvl1
    { 5319, 1166, -20, 5689, 562, 9, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Boss - Fixed for New Map
    { 5319, 1167, -20, 5689, 563, 8, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Boss
    { 5319, 1168, -20, 5689, 564, 8, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Boss
    { 5319, 1169, -20, 5689, 565, 9, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Boss
    { 5319, 1170, -20, 5689, 566, 9, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl2 -> Boss
    { 5690, 562, 9, 5320, 1166, -18, "britannia_alt", "britannia_alt" }, //Boss -> Lair of Golaroth Lvl2
    { 5690, 563, 8, 5320, 1167, -20, "britannia_alt", "britannia_alt" }, //Boss -> Lair of Golaroth Lvl2
    { 5690, 564, 8, 5320, 1168, -15, "britannia_alt", "britannia_alt" }, //Boss -> Lair of Golaroth Lvl2
    { 5690, 565, 10, 5320, 1169, -15, "britannia_alt", "britannia_alt" }, //Boss -> Lair of Golaroth Lvl2
    { 5690, 566, 9, 5320, 1170, -10, "britannia_alt", "britannia_alt" }, //Boss -> Lair of Golaroth Lvl2
    { 5174, 1500, 11, 5995, 519, -14, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lair of Golaroth Cave - Fixed for New Map
    { 5174, 1501, 11, 5995, 520, -14, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lair of Golaroth Cave
    { 5174, 1502, 11, 5995, 521, -14, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Lvl1 -> Lair of Golaroth Cave
    { 5996, 519, -13, 5175, 1500, 11, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave -> Lair of Golaroth Lvl1
    { 5996, 520, -13, 5175, 1501, 11, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave -> Lair of Golaroth Lvl1
    { 5996, 521, -13, 5175, 1502, 11, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave -> Lair of Golaroth Lvl1
    { 577, 1276, 42, 6021, 601, 4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Entrance - Fixed for New Map
    { 578, 1276, 42, 6022, 601, 4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Entrance
    { 579, 1276, 42, 6023, 601, 4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Entrance
    { 580, 1276, 42, 6024, 601, 4, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Entrance
    { 6021, 602, 6, 577, 1277, 47, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Exit
    { 6022, 602, 6, 578, 1277, 47, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Exit
    { 6023, 602, 6, 579, 1277, 47, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Exit
    { 6024, 602, 6, 580, 1277, 47, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Exit
    { 6025, 602, 6, 580, 1277, 47, "britannia_alt", "britannia_alt" }, //Lair of Golaroth Cave Exit

    //Cult of the Serpent
    { 378, 881, -5, 5408, 1643, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Snow Mine - Fixed for New Map
    { 378, 882, -5, 5408, 1644, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Snow Mine
    { 378, 883, -5, 5408, 1645, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Snow Mine
    { 378, 884, -5, 5408, 1646, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Snow Mine
    { 5409, 1643, 0, 379, 881, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Mainland
    { 5409, 1644, 0, 379, 882, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Mainland
    { 5409, 1645, 0, 379, 883, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Mainland
    { 5409, 1646, 0, 379, 884, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Mainland
    { 5396, 1611, 0, 5355, 1604, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Cult of the Serpent - Fixed for New Map
    { 5396, 1612, 0, 5355, 1605, 0, "britannia_alt", "britannia_alt" }, //Snow Mine -> Cult of the Serpent
    { 5356, 1604, 0, 5397, 1611, 0, "britannia_alt", "britannia_alt" }, //Cult of the Serpent -> Snow Mine
    { 5356, 1605, 0, 5397, 1612, 0, "britannia_alt", "britannia_alt" }, //Cult of the Serpent -> Snow Mine

    //Temple of Itzcali
    { 5462, 1592, 25, 2053, 3430, -30, "britannia_alt", "britannia_alt" }, //The Dark Brotherhood Guild Hall -> Temple of Itzcali - Fixed for New Map
    { 5463, 1592, 25, 2054, 3430, -30, "britannia_alt", "britannia_alt" }, //The Dark Brotherhood Guild Hall -> Temple of Itzcali
    { 2053, 3429, -30, 5462, 1591, 25, "britannia_alt", "britannia_alt" }, //Temple of Itzcali -> The Dark Brotherhood Guild Hall
    { 2054, 3429, -30, 5463, 1591, 25, "britannia_alt", "britannia_alt" }, //Temple of Itzcali -> The Dark Brotherhood Guild Hall

    //Ogre Cave
    { 411, 1345, 2, 6163, 463, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Ogre Cave - Fixed for New Map
    { 412, 1345, 2, 6164, 463, 0, "britannia_alt", "britannia_alt" }, //Mainland -> Ogre Cave
    { 6163, 464, 0, 411, 1346, 2, "britannia_alt", "britannia_alt" }, //Ogre Cave -> Mainland
    { 6164, 464, 0, 412, 1346, 2, "britannia_alt", "britannia_alt" }, //Ogre Cave -> Mainland

    //Underworld <> Tartarus
    { 6012, 3250, 0, 7109, 3954, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Tartarus - Fixed for New Map
    { 6013, 3250, 0, 7110, 3954, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Tartarus
    { 6014, 3250, 0, 7111, 3954, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Tartarus
    { 6015, 3250, 0, 7112, 3954, 0, "britannia_alt", "britannia_alt" }, //Underworld -> Tartarus
    { 7109, 3955, 0, 6012, 3251, 0, "britannia_alt", "britannia_alt" }, //Tartarus -> Underworld
    { 7110, 3955, 0, 6013, 3251, 0, "britannia_alt", "britannia_alt" }, //Tartarus -> Underworld
    { 7111, 3955, 0, 6014, 3251, 0, "britannia_alt", "britannia_alt" }, //Tartarus -> Underworld
    { 7112, 3955, 0, 6015, 3251, 0, "britannia_alt", "britannia_alt" }, //Tartarus -> Underworld

    //Rikktors Lair
    { 5256, 620, 25, 5276, 620, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5275, 620, 25, 5255, 620, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5284, 610, 25, 5284, 584, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5284, 585, 25, 5284, 611, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5284, 507, 25, 5285, 491, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5285, 492, 25, 5284, 508, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5153, 610, 25, 5144, 716, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5145, 716, 25, 5152, 610, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5226, 620, 25, 5204, 628, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5227, 650, 25, 5205, 628, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5204, 629, 25, 5226, 651, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5205, 629, 25, 5227, 651, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair
    { 5246, 737, 25, 6099, 3786, 22, "britannia_alt", "britannia_alt" }, //Rikktors Lair to Tartarus
    { 6099, 3785, 25, 5246, 736, 22, "britannia_alt", "britannia_alt" }, //Tartarus to Rikktors Lair Entrance

    ////////////////////////////////
    ////////////////////////////////
    // Nagash Added Teleporters
    // Sosaria Unsorted
    ///////////////////////////////
    ///////////////////////////////

    //Not in Photopea
    //Rebellion Island Ship  (Needs more connecting Teleporters in level 2, all the different halls)
    {391,   2790,   -6,     6328,   416,    85,     "britannia_alt",    "britannia_alt"},   //Rebellion Island Dungeon To Level 1
    {6328,  415,    96,     391,    2789,   -1,     "britannia_alt",    "britannia_alt"},   //Rebellion Island Dungeon To Entrance
    {6383,  535,    0,      6650,   80,     11,     "britannia_alt",    "britannia_alt"},   //To Rebellion Island To Level 2
    {6384,  535,    0,      6651,   80,     11,     "britannia_alt",    "britannia_alt"},   //To Rebellion Island To Level 2
    {6650,  79,     11,     6383,   534,    0,      "britannia_alt",    "britannia_alt"},   //To Rebellion Island From Level 2 to Level 1
    {6651,  79,     11,     6384,   534,    0,      "britannia_alt",    "britannia_alt"},   //To Rebellion Island From Level 2 to Level 1
    {6652,  79,     11,     6385,   534,    0,      "britannia_alt",    "britannia_alt"},   //To Rebellion Island From Level 2 to Level 1

    //Nexus Outpost (Needs a teleporter still to a dungeon, its under the top area buildings)
    { 5863, 355, 1, 5939, 2750, 0, "britannia_alt", "britannia_alt" }, //Nexus Outpost -> Underworld
    { 5864, 355, 1, 5940, 2750, -1, "britannia_alt", "britannia_alt" }, //Nexus Outpost -> Underworld
    { 5865, 355, 0, 5941, 2750, -2, "britannia_alt", "britannia_alt" }, //Nexus Outpost -> Underworld
    { 5866, 355, 0, 5942, 2750, 2, "britannia_alt", "britannia_alt" }, //Nexus Outpost -> Underworld
    { 5939, 2749, 0, 5863, 354, 3, "britannia_alt", "britannia_alt" }, //Underworld -> Nexus Outpost 
    { 5940, 2749, 0, 5864, 354, 2, "britannia_alt", "britannia_alt" }, //Underworld -> Nexus Outpost 
    { 5941, 2749, -1, 5865, 354, 1, "britannia_alt", "britannia_alt" }, //Underworld -> Nexus Outpost 
    { 5942, 2749, 5, 5866, 354, 1, "britannia_alt", "britannia_alt" }, //Underworld -> Nexus Outpost

    //Top Unnamed big dungeon... has like 3 entrances still not connected
    { 5540, 76, 0, 5330, 414, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon to Society of Mages
    { 5541, 76, 0, 5331, 414, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon to Society of Mages
    { 5542, 76, 0, 5332, 414, 0, "britannia_alt", "britannia_alt" }, //Unnamed Dungeon to Society of Mages
    { 5330, 415, 0, 5540, 77, 0, "britannia_alt", "britannia_alt" }, //Society of Mages to Unnamed Dungeon
    { 5331, 415, 0, 5541, 77, 0, "britannia_alt", "britannia_alt" }, //Society of Mages to Unnamed Dungeon
    { 5332, 415, 0, 5542, 77, 0, "britannia_alt", "britannia_alt" }, //Society of Mages to Unnamed Dungeon


    ////////////////////////////////
    ////////////////////////////////
    // Malas and Tokuno
    // Sorted But Incomplete
    ///////////////////////////////
    ///////////////////////////////

    //Yomotsu Mines
    { 3, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 4, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 5, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 6, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 7, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 8, 128, -1, 259, 785, 64, "malas", "tokuno" }, //Yomotsu Mines To Tokuno
    { 257, 783, 63, 5, 129, -1, "tokuno", "malas" }, //Tokuno To Yomotsu Mines
    { 258, 783, 63, 5, 129, -1, "tokuno", "malas" }, //Tokuno To Yomotsu Mines
    { 259, 783, 63, 5, 129, -1, "tokuno", "malas" }, //Tokuno To Yomotsu Mines
    { 260, 783, 63, 5, 129, -1, "tokuno", "malas" }, //Tokuno To Yomotsu Mines

    //Fan Dancer Dojo
    { 64, 336, 11, 983, 195, 24, "malas", "tokuno" }, //Fan Dancer Dojo to Tokuno
    { 64, 337, 11, 983, 195, 24, "malas", "tokuno" }, //Fan Dancer Dojo to Tokuno
    { 64, 338, 11, 983, 195, 24, "malas", "tokuno" }, //Fan Dancer Dojo to Tokuno
    { 64, 339, 11, 983, 195, 24, "malas", "tokuno" }, //Fan Dancer Dojo to Tokuno
    { 987, 196, 15, 67, 337, -1, "tokuno", "malas" }, //Tokuno to Fan Dancer Dojo
    { 988, 194, 15, 67, 337, -1, "tokuno", "malas" }, //Tokuno to Fan Dancer Dojo
    { 988, 195, 15, 67, 337, -1, "tokuno", "malas" }, //Tokuno to Fan Dancer Dojo
    { 988, 197, 18, 67, 337, -1, "tokuno", "malas" }, //Tokuno to Fan Dancer Dojo
    { 66, 351, -7, 63, 524, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 1 to 2
    { 66, 352, -7, 63, 524, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 1 to 2
    { 66, 353, -7, 63, 524, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 1 to 2
    { 66, 354, -7, 63, 524, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 1 to 2
    { 61, 523, 6, 63, 352, -3, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 1
    { 61, 524, 6, 63, 352, -3, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 1
    { 61, 525, 6, 63, 352, -3, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 1
    { 61, 526, 6, 63, 352, -3, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 1
    { 103, 555, -6, 76, 691, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 3
    { 103, 556, -6, 76, 691, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 3
    { 103, 557, -6, 76, 691, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 3
    { 103, 558, -6, 76, 691, -1, "malas", "malas" }, //Fan Dancer Dojo Lvl 2 to 3
    { 73, 688, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 689, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 690, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 691, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 692, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 693, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2
    { 73, 694, 6, 100, 556, -2, "malas", "malas" },  //Fan Dancer Dojo Lvl 3 to 2

    //Bedlam Dungeon (Incomplete... need a way in, in OSI its a quest in Umbra)
    { 84, 1673, -2, 156, 1613, 0, "malas", "malas" }, //1 to 2
    { 156, 1609, 17, 87, 1673, 0, "malas", "malas" }, //2 to 1
    { 157, 1609, 17, 87, 1673, 0, "malas", "malas" }, //2 to 1

    //Labyrinth Dungeon (Incomplete... need a way in on the island in Malas, and need a teleport from main area to island built)
    { 328, 1972, 5, 1731, 978, -80, "malas", "malas" }, //Labyrinth Dungeon
    { 328, 1973, 5, 1731, 978, -80, "malas", "malas" }, //Labyrinth Dungeon
    { 328, 1974, 5, 1731, 978, -80, "malas", "malas" }, //Labyrinth Dungeon
    { 328, 1975, 5, 1731, 978, -80, "malas", "malas" }, //Labyrinth Dungeon
    
    //Ninja Event Area (Need to fix Entrance and Exit to Zento)
    { 330, 706, -1, 384, 733, -1, "malas", "malas" },
    { 331, 769, -1, 384, 733, -1, "malas", "malas" },
    { 355, 745, -1, 384, 733, -1, "malas", "malas" },
    { 389, 704, -1, 384, 733, -1, "malas", "malas" },
    { 426, 780, -1, 384, 733, -1, "malas", "malas" },
    { 391, 777, -1, 770, 1209, 25, "malas", "tokuno" }, //Back to Zento
    { 375, 802, 1, 770, 1209, 25, "malas", "tokuno" }, //Back to Zento
    { 384, 810, -1, 403, 1167, 0, "malas", "malas" },  //Ninja Event Area to Dungeon

    //Ninja Cave of Ninja Event Area (Need to fix how to get to area 3)
    { 403, 1169, 0, 385, 811, -1, "malas", "malas" }, //Ninja Cave 1 to Ninja Event Area
    { 404, 1169, 0, 385, 808, -1, "malas", "malas" }, //Ninja Cave 1 to Ninja Event Area
    { 405, 1169, 0, 385, 808, -1, "malas", "malas" }, //Ninja Cave 1 to Ninja Event Area
    { 412, 1086, 0, 391, 803, -1, "malas", "malas" }, //Ninja Cave 3 to Ninja Event Area
    { 412, 1123, 0, 417, 806, -1, "malas", "malas" }, //Ninja Cave 1 to Ninja Cave 2
    { 411, 1117, 0, 422, 806, -1, "malas", "malas" }, //Ninja Cave 2 to Ninja Cave 1
    { 421, 1092, 0, 391, 803, -1, "malas", "malas" }, //Ninja Cave 2 to Ninja Event Area
    
    //Doom Dungeon (Needs teleporter to secret side area for healer and exit from gauntlet)
    { 2315, 1267, -109, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2315, 1268, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2315, 1269, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2316, 1267, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2316, 1269, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2317, 1266, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2317, 1267, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2317, 1268, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 2317, 1269, -110, 381, 132, 33, "malas", "malas" }, //Doom Entrance
    { 496, 49, 6, 2368, 1267, -85, "malas", "malas" },  //Doom Before Ship to Doom Entrance
    { 407, 254, 2, 428, 318, 2, "malas", "malas" }, //Doom Ship 1 to Ship 2
    { 408, 254, 2, 428, 318, 2, "malas", "malas" }, //Doom Ship 1 to Ship 2
    { 409, 254, 2, 428, 318, 2, "malas", "malas" }, //Doom Ship 1 to Ship 2
    { 427, 321, 2, 422, 327, -1, "malas", "malas" }, //Doom Ship 2 to Beach
    { 428, 321, 2, 422, 327, -1, "malas", "malas" }, //Doom Ship 2 to Beach
    { 429, 321, 2, 422, 327, -1, "malas", "malas" }, //Doom Ship 2 to Beach
    
    ////////////////////////////////
    ////////////////////////////////
    // Ilshenar
    // Sorted
    ///////////////////////////////
    ///////////////////////////////

    //Shrine Room
    {2263,  1562,   -28,    1527,   1340,   -3,     "ilshenar",     "ilshenar"},    //Spirituality 1
    {2264,  1562,   -28,    1527,   1340,   -3,     "ilshenar",     "ilshenar"},    //Spirituality 2
    {2249,  1558,   -28,    1216,   468,    -13,    "ilshenar",     "ilshenar"},    //Compassion 1
    {2249,  1559,   -28,    1216,   468,    -13,    "ilshenar",     "ilshenar"},    //Compassion 2
    {2249,  1548,   -28,    747,    730,    -29,    "ilshenar",     "ilshenar"},    //Honor 1
    {2250,  1548,   -28,    747,    730,    -29,    "ilshenar",     "ilshenar"},    //Honor 2
    {2256,  1542,   -28,    723,    1355,   -61,    "ilshenar",     "ilshenar"},    //Honesty 1
    {2257,  1542,   -28,    723,    1355,   -61,    "ilshenar",     "ilshenar"},    //Honesty 2
    {2263,  1540,   -28,    286,    1018,   0,      "ilshenar",     "ilshenar"},    //Humility 1
    {2264,  1540,   -28,    286,    1018,   0,      "ilshenar",     "ilshenar"},    //Humility 2
    {2271,  1542,   -28,    987,    1001,   -36,    "ilshenar",     "ilshenar"},    //Justice 1
    {2271,  1543,   -28,    987,    1001,   -36,    "ilshenar",     "ilshenar"},    //Justice 2
    {2278,  1548,   -28,    528,    211,    -42,    "ilshenar",     "ilshenar"},    //Valor 1
    {2278,  1549,   -28,    528,    211,    -42,    "ilshenar",     "ilshenar"},    //Valor 2
    {2278,  1558,   -28,    1171,   1287,   -30,    "ilshenar",     "ilshenar"},    //Sacrifice 1
    {2278,  1559,   -28,    1171,   1287,   -30,    "ilshenar",     "ilshenar"},    //Sacrifice 2

    // Ankh Dungeon
    { 10, 1519, -28, 10, 873, -28, "ilshenar", "ilshenar" },    //Ankh Dungeon to Kirin Passage
    { 11, 1519, -27, 11, 873, -28, "ilshenar", "ilshenar" },    //Ankh Dungeon to Kirin Passage
    { 12, 1519, -27, 12, 873, -27, "ilshenar", "ilshenar" },    //Ankh Dungeon to Kirin Passage
    { 10, 872, -28, 10, 1518, -27, "ilshenar", "ilshenar" },    //Kirin Passage to Ankh Dungeon
    { 11, 872, -28, 11, 1518, -28, "ilshenar", "ilshenar" },    //Kirin Passage to Ankh Dungeon
    { 12, 872, -27, 12, 1518, -28, "ilshenar", "ilshenar" },    //Kirin Passage to Ankh Dungeon
    { 78, 1365, -41, 531, 1532, -9, "ilshenar", "ilshenar" },  //Ankh Dungeon to Serpentine Passage
    { 79, 1365, -41, 532, 1532, -7, "ilshenar", "ilshenar" },  //Ankh Dungeon to Serpentine Passage
    { 80, 1365, -41, 533, 1532, -7, "ilshenar", "ilshenar" },  //Ankh Dungeon to Serpentine Passage
    { 81, 1365, -41, 534, 1532, -7, "ilshenar", "ilshenar" },  //Ankh Dungeon to Serpentine Passage
    { 531, 1533, -4, 78, 1366, -36, "ilshenar", "ilshenar" },  //Serpentine Passage to Ankh Dungeon
    { 532, 1533, -4, 79, 1366, -36, "ilshenar", "ilshenar" },  //Serpentine Passage to Ankh Dungeon
    { 533, 1533, -4, 80, 1366, -36, "ilshenar", "ilshenar" },  //Serpentine Passage to Ankh Dungeon
    { 534, 1533, -4, 81, 1366, -36, "ilshenar", "ilshenar" },  //Serpentine Passage to Ankh Dungeon
    { 154, 1472, -8, 575, 1155, -121, "ilshenar", "ilshenar" }, //Ankh Dungeon to The Abandoned Tower
    { 155, 1472, -8, 576, 1155, -120, "ilshenar", "ilshenar" }, //Ankh Dungeon to The Abandoned Tower
    { 156, 1472, -8, 577, 1155, -120, "ilshenar", "ilshenar" }, //Ankh Dungeon to The Abandoned Tower
    { 575, 1156, -121, 154, 1473, -8, "ilshenar", "ilshenar" }, //The Abandoned Tower to Ankh Dungeon
    { 576, 1156, -121, 155, 1473, -8, "ilshenar", "ilshenar" }, //The Abandoned Tower to Ankh Dungeon
    { 577, 1156, -121, 156, 1473, -8, "ilshenar", "ilshenar" }, //The Abandoned Tower to Ankh Dungeon
    { 3, 1267, -8, 666, 928, -83, "ilshenar", "ilshenar" },     //Ankh Dungeon to The Ruined Tower
    { 3, 1268, -8, 666, 929, -83, "ilshenar", "ilshenar" },     //Ankh Dungeon to The Ruined Tower
    { 3, 1269, -8, 666, 930, -83, "ilshenar", "ilshenar" },     //Ankh Dungeon to The Ruined Tower
    { 667, 927, -85, 4, 1267, -11, "ilshenar", "ilshenar" },    //The Ruined Tower to Ankh Dungeon
    { 667, 928, -85, 4, 1267, -11, "ilshenar", "ilshenar" },    //The Ruined Tower to Ankh Dungeon
    { 667, 929, -85, 4, 1268, -11, "ilshenar", "ilshenar" },    //The Ruined Tower to Ankh Dungeon
    { 667, 930, -85, 4, 1269, -11, "ilshenar", "ilshenar" },    //The Ruined Tower to Ankh Dungeon

    // Kirin Passage
    { 348, 1427, 15, 18, 1197, -6, "ilshenar", "ilshenar" },    //To Kirin Passage
    { 349, 1427, 15, 19, 1197, -6, "ilshenar", "ilshenar" },    //To Kirin Passage
    { 350, 1427, 15, 20, 1197, -6, "ilshenar", "ilshenar" },    //To Kirin Passage
    { 351, 1427, 15, 20, 1197, -6, "ilshenar", "ilshenar" },    //To Kirin Passage
    { 18, 1198, -5, 348, 1428, 15, "ilshenar", "ilshenar" },    //To Kirin Passage Entrance
    { 19, 1198, -5, 349, 1428, 15, "ilshenar", "ilshenar" },    //To Kirin Passage Entrance
    { 20, 1198, -5, 350, 1428, 15, "ilshenar", "ilshenar" },    //To Kirin Passage Entrance
    
    //Serpentine Passage
    { 393, 1587, -13, 810, 875, -40, "ilshenar", "ilshenar" },   //To Serpentine Entrance
    { 394, 1587, -13, 811, 875, -39, "ilshenar", "ilshenar" },   //To Serpentine Entrance
    { 395, 1587, -13, 812, 875, -39, "ilshenar", "ilshenar" },   //To Serpentine Entrance
    { 396, 1587, -13, 813, 875, -40, "ilshenar", "ilshenar" },   //To Serpentine Entrance
    { 810, 874, -39, 393, 1586, -13, "ilshenar", "ilshenar" },   //To Serpentine Passage
    { 811, 874, -39, 394, 1586, -13, "ilshenar", "ilshenar" },   //To Serpentine Passage
    { 812, 874, -39, 395, 1586, -13, "ilshenar", "ilshenar" },   //To Serpentine Passage

    //Blood Dungeon
    { 2113, 829, -13, 1745, 1235, -29, "ilshenar", "ilshenar" },    //To Blood Dungeon Entrance
    { 2114, 829, -13, 1746, 1235, -29, "ilshenar", "ilshenar" },    //To Blood Dungeon Entrance
    { 2115, 829, -13, 1747, 1235, -29, "ilshenar", "ilshenar" },    //To Blood Dungeon Entrance
    { 2116, 829, -13, 1748, 1235, -29, "ilshenar", "ilshenar" },    //To Blood Dungeon Entrance
    { 1745, 1236, -30, 2113, 830, -11, "ilshenar", "ilshenar" },    //To Blood Dungeon
    { 1746, 1236, -30, 2114, 830, -11, "ilshenar", "ilshenar" },    //To Blood Dungeon
    { 1747, 1236, -30, 2115, 830, -11, "ilshenar", "ilshenar" },    //To Blood Dungeon
    { 1748, 1236, -30, 2116, 830, -11, "ilshenar", "ilshenar" },    //To Blood Dungeon

    //Sorcerer`s Dungeon  (Needs a teleporter to Level 4)
    { 426, 113, -28, 546, 456, -40, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon Entrance
    { 427, 113, -28, 547, 456, -40, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon Entrance
    { 428, 113, -28, 548, 456, -40, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon Entrance
    { 429, 113, -28, 549, 456, -40, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon Entrance
    { 546, 455, -40, 426, 112, -28, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon
    { 547, 455, -40, 427, 112, -28, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon
    { 548, 455, -40, 428, 112, -28, "ilshenar", "ilshenar" },   //To Sorcerers Dungeon

    { 371, 29, -36, 241, 25, -18, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 1 to 2
    { 371, 30, -36, 241, 26, -18, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 1 to 2
    { 371, 31, -36, 241, 27, -18, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 1 to 2
    { 242, 25, -16, 372, 29, -31, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 2 to 1
    { 242, 26, -16, 372, 30, -31, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 2 to 1
    { 242, 27, -16, 372, 31, -31, "ilshenar", "ilshenar" },     //Sorcerers Dungeon Level 2 to 1
    
    { 284, 67, -30, 131, 128, -21, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 South
    { 285, 67, -30, 132, 128, -21, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 South
    { 286, 67, -30, 133, 128, -21, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 South
    { 287, 67, -30, 134, 128, -21, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 South
    { 131, 129, -16, 284, 68, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 South to 2
    { 132, 129, -16, 285, 68, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 South to 2
    { 133, 129, -16, 286, 68, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 South to 2
    { 134, 129, -16, 287, 68, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 South to 2

    { 358, 40, -36, 156, 88, -18, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 North
    { 358, 41, -36, 156, 89, -18, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 North
    { 358, 42, -36, 156, 90, -18, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to 3 North
    { 155, 88, -16, 357, 40, -31, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 North to 2
    { 155, 89, -16, 357, 41, -31, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 North to 2
    { 155, 90, -16, 357, 42, -31, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 3 North to 2

    //Terort Skitas
    { 284, 73, -16, 265, 131, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to Terort Skitas Atrium
    { 285, 73, -16, 266, 131, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to Terort Skitas Atrium
    { 286, 73, -16, 267, 131, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to Terort Skitas Atrium
    { 287, 73, -16, 268, 131, -28, "ilshenar", "ilshenar" },    //Sorcerers Dungeon Level 2 to Terort Skitas Atrium
    { 265, 130, -31, 284, 72, -21, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Sorcerers Dungeon Level 2
    { 266, 130, -31, 285, 72, -21, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Sorcerers Dungeon Level 2
    { 267, 130, -31, 286, 72, -21, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Sorcerers Dungeon Level 2
    { 268, 130, -31, 287, 72, -21, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Sorcerers Dungeon Level 2
    { 272, 141, -16, 555, 427, -1, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Terort Skitas
    { 273, 141, -16, 556, 427, -1, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Terort Skitas
    { 274, 141, -16, 557, 427, -1, "ilshenar", "ilshenar" },    //Terort Skitas Atrium to Terort Skitas
    { 555, 426, -6, 272, 140, -21, "ilshenar", "ilshenar" },    //Terort Skitas to Terort Skitas Atrium
    { 556, 426, -6, 273, 140, -21, "ilshenar", "ilshenar" },    //Terort Skitas to Terort Skitas Atrium
    { 557, 426, -6, 274, 140, -21, "ilshenar", "ilshenar" },    //Terort Skitas to Terort Skitas Atrium

    //Ratman Villa
    { 636, 813, -62, 164, 743, -28, "ilshenar", "ilshenar" },   //Ratman Villa to Rathman Cellar
    { 164, 746, -16, 636, 815, -52, "ilshenar", "ilshenar" },   //Ratman Cellar to Ratman Villa

    //Lizardman Village
    { 313, 1329, -40, 327, 1592, -13, "ilshenar", "ilshenar" }, //Lizardman Village to Lizardman Passage
    { 314, 1329, -38, 328, 1592, -13, "ilshenar", "ilshenar" }, //Lizardman Village to Lizardman Passage
    { 315, 1329, -36, 329, 1592, -13, "ilshenar", "ilshenar" }, //Lizardman Village to Lizardman Passage
    { 327, 1593, -13, 313, 1330, -39, "ilshenar", "ilshenar" }, //Lizardman Passage to Lizardman Village
    { 328, 1593, -13, 314, 1330, -37, "ilshenar", "ilshenar" }, //Lizardman Passage to Lizardman Village
    { 329, 1593, -13, 315, 1330, -35, "ilshenar", "ilshenar" }, //Lizardman Passage to Lizardman Village
    { 330, 1593, -13, 315, 1330, -35, "ilshenar", "ilshenar" }, //Lizardman Passage to Lizardman Village
    { 265, 1587, -28, 225, 1335, -20, "ilshenar", "ilshenar" }, //Lizardman Passage to Wyrms Lair
    { 266, 1587, -28, 226, 1335, -20, "ilshenar", "ilshenar" }, //Lizardman Passage to Wyrms Lair
    { 225, 1334, -20, 265, 1586, -28, "ilshenar", "ilshenar" }, //Lizardman Passage to Wyrms Lair
    { 226, 1334, -20, 266, 1586, -28, "ilshenar", "ilshenar" }, //Lizardman Passage to Wyrms Lair
    { 227, 1334, -20, 266, 1586, -28, "ilshenar", "ilshenar" }, //Lizardman Passage to Wyrms Lair

    //Wisp Dungeon
    { 650, 1297, -58, 626, 1526, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Entrance To Wisp Dungeon
    { 651, 1297, -58, 627, 1526, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Entrance To Wisp Dungeon
    { 652, 1297, -58, 628, 1526, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Entrance To Wisp Dungeon
    { 626, 1527, -28, 650, 1298, -58, "ilshenar", "ilshenar" }, //Wisp Dungeon to Wisp Dungeon Entrance
    { 627, 1527, -28, 651, 1298, -59, "ilshenar", "ilshenar" }, //Wisp Dungeon to Wisp Dungeon Entrance
    { 628, 1527, -28, 652, 1298, -58, "ilshenar", "ilshenar" }, //Wisp Dungeon to Wisp Dungeon Entrance
    { 629, 1527, -28, 652, 1298, -58, "ilshenar", "ilshenar" }, //Wisp Dungeon to Wisp Dungeon Entrance
    { 694, 1490, -53, 713, 1490, -6, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to Wisp Dungeon Level 1 to 2 Passage
    { 694, 1491, -53, 713, 1491, -6, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to Wisp Dungeon Level 1 to 2 Passage
    { 694, 1492, -53, 713, 1492, -6, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to Wisp Dungeon Level 1 to 2 Passage
    { 694, 1493, -53, 713, 1493, -6, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to Wisp Dungeon Level 1 to 2 Passage
    { 712, 1490, -3, 686, 1490, -28, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 712, 1491, -3, 686, 1491, -28, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 712, 1492, -3, 686, 1492, -28, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 712, 1493, -3, 686, 1493, -28, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 722, 1505, -53, 837, 1550, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 1 to 2 Passage to 2
    { 722, 1506, -53, 837, 1551, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 1 to 2 Passage to 2
    { 722, 1507, -53, 837, 1552, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 1 to 2 Passage to 2
    { 722, 1508, -53, 837, 1553, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 1 to 2 Passage to 2
    { 838, 1550, -6, 723, 1505, -51, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 838, 1551, -6, 723, 1506, -51, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 838, 1552, -6, 723, 1507, -51, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 838, 1553, -6, 723, 1508, -51, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 1 to 2 Passage to 1
    { 892, 1449, -51, 870, 1433, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to Wisp Dungeon Level 2 to 3 Passage
    { 892, 1450, -51, 870, 1434, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to Wisp Dungeon Level 2 to 3 Passage
    { 892, 1451, -51, 870, 1435, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to Wisp Dungeon Level 2 to 3 Passage
    { 892, 1452, -51, 870, 1436, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to Wisp Dungeon Level 2 to 3 Passage
    { 892, 1453, -51, 870, 1437, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to Wisp Dungeon Level 2 to 3 Passage
    { 871, 1433, -6, 893, 1449, -46, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to 3 Passage to 2
    { 871, 1434, -6, 893, 1450, -46, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to 3 Passage to 2
    { 871, 1435, -6, 893, 1451, -46, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to 3 Passage to 2
    { 871, 1436, -6, 893, 1452, -46, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to 3 Passage to 2
    { 871, 1437, -6, 893, 1453, -46, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to 3 Passage to 2
    { 840, 1433, -53, 811, 1546, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to 3 Passage to 3
    { 840, 1434, -53, 811, 1547, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to 3 Passage to 3
    { 840, 1435, -53, 811, 1548, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to 3 Passage to 3
    { 840, 1436, -53, 811, 1549, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to 3 Passage to 3
    { 840, 1437, -53, 811, 1549, -11, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 2 to 3 Passage to 3
    { 812, 1546, -6, 841, 1433, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 3 to Wisp Dungeon Level 2 to 3 Passage
    { 812, 1547, -6, 841, 1434, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 3 to Wisp Dungeon Level 2 to 3 Passage
    { 812, 1548, -6, 841, 1435, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 3 to Wisp Dungeon Level 2 to 3 Passage
    { 812, 1549, -6, 841, 1436, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 3 to Wisp Dungeon Level 2 to 3 Passage

    //Wisp Dungeon Maze
    { 791, 1547, -28, 783, 1542, -28, "ilshenar", "ilshenar" }, //Maze Rune 1
    { 791, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 1 Reset
    { 791, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 1 Reset
    { 791, 1548, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 1 Reset
    { 791, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 1 Reset
    { 791, 1550, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 1 Reset
    { 784, 1542, -28, 781, 1550, -28, "ilshenar", "ilshenar" }, //Maze Rune 2
    { 782, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 783, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 784, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 782, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 782, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 783, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 784, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 2 Reset
    { 782, 1550, -28, 789, 1556, -28, "ilshenar", "ilshenar" }, //Maze Rune 3
    { 780, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 781, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 782, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 780, 1550, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 780, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 781, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 782, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 3 Reset
    { 788, 1556, -28, 789, 1552, -28, "ilshenar", "ilshenar" }, //Maze Rune 4
    { 788, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 789, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 790, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 790, 1556, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 788, 1557, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 789, 1557, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 790, 1557, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 4 Reset
    { 788, 1552, -28, 789, 1546, -28, "ilshenar", "ilshenar" }, //Maze Rune 5
    { 788, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 789, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 790, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 790, 1552, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 788, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 789, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 790, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 5 Reset
    { 789, 1547, -28, 783, 1538, -28, "ilshenar", "ilshenar" }, //Maze Rune 6
    { 788, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 789, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 790, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 788, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 790, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 788, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 790, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 6 Reset
    { 782, 1538, -28, 781, 1554, -28, "ilshenar", "ilshenar" }, //Maze Rune 7
    { 782, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 783, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 784, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 784, 1538, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 782, 1539, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 783, 1539, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 784, 1539, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 7 Reset
    { 780, 1554, -28, 777, 1554, -28, "ilshenar", "ilshenar" }, //Maze Rune 8
    { 780, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 781, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 782, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 782, 1554, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 780, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 781, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 782, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 8 Reset
    { 777, 1553, -28, 785, 1546, -28, "ilshenar", "ilshenar" }, //Maze Rune 9
    { 778, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 9 Reset
    { 778, 1554, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 9 Reset
    { 777, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 9 Reset
    { 778, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 9 Reset
    { 786, 1546, -28, 787, 1538, -28, "ilshenar", "ilshenar" }, //Maze Rune 10
    { 784, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 785, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 786, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 784, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 784, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 785, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 786, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 10 Reset
    { 787, 1539, -28, 785, 1554, -28, "ilshenar", "ilshenar" }, //Maze Rune 11
    { 786, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 787, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 788, 1537, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 786, 1538, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 788, 1538, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 786, 1539, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 788, 1539, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 11 Reset
    { 785, 1555, -28, 787, 1542, -28, "ilshenar", "ilshenar" }, //Maze Rune 12
    { 784, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 785, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 786, 1553, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 784, 1554, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 786, 1554, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 784, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 786, 1555, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 12 Reset
    { 787, 1541, -28, 779, 1541, -28, "ilshenar", "ilshenar" }, //Maze Rune 13
    { 786, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 788, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 786, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 788, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 786, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 787, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 788, 1543, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 13 Reset
    { 780, 1541, -28, 785, 1550, -28, "ilshenar", "ilshenar" }, //Maze Rune 14
    { 778, 1540, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 779, 1540, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 780, 1540, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 778, 1541, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 778, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 779, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 780, 1542, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 14 Reset
    { 785, 1549, -28, 781, 1546, -28, "ilshenar", "ilshenar" }, //Maze Rune 15
    { 784, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 786, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 784, 1550, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 786, 1550, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 784, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 785, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 786, 1551, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 15 Reset
    { 781, 1547, -28, 776, 1548, -28, "ilshenar", "ilshenar" }, //Maze Rune 16
    { 780, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 781, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 782, 1545, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 780, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 782, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 780, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 782, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 16 Reset
    { 776, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset
    { 777, 1546, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset
    { 777, 1547, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset
    { 777, 1548, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset
    { 777, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset
    { 776, 1549, -28, 798, 1547, -28, "ilshenar", "ilshenar" }, //Maze Rune 17 Reset

    //More Wisp Dungeon
    { 752, 1549, -28, 751, 1484, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 3 to 4
    { 747, 1539, -28, 785, 1514, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon North Room 1 Teleport
    { 785, 1524, -28, 779, 1538, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon North Room 2 Teleport
    { 779, 1514, -28, 772, 1514, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon North Room 3 Teleport
    { 770, 1524, -28, 755, 1547, -28, "ilshenar", "ilshenar" }, //Maze North Room Reset
    { 747, 1555, -28, 785, 1570, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon South Room 1 Teleport
    { 784, 1580, -28, 779, 1559, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon South Room 2 Teleport
    { 777, 1579, -28, 772, 1581, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon South Room 3 Teleport
    { 769, 1569, -28, 755, 1547, -28, "ilshenar", "ilshenar" }, //Maze South Room Reset
    { 751, 1479, -28, 763, 1555, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 4 to 3
    { 751, 1473, -28, 763, 1479, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 4 to Boss Room
    { 775, 1467, -28, 658, 1498, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 4 to 1
    { 775, 1492, -28, 827, 1515, -28, "ilshenar", "ilshenar" }, //Wisp Dungeon Level 4 to 2
    { 879, 1490, 24, 952, 1425, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to Cyclops Dungeon Passage 
    { 879, 1491, 24, 952, 1426, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to Cyclops Dungeon Passage
    { 879, 1492, 24, 952, 1427, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to Cyclops Dungeon Passage
    { 879, 1493, 24, 952, 1428, -53, "ilshenar", "ilshenar" },  //Wisp Dungeon Level 2 to Cyclops Dungeon Passage
    { 951, 1424, -53, 878, 1490, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1425, -53, 878, 1490, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1426, -53, 878, 1491, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1427, -53, 878, 1492, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1428, -53, 878, 1493, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1429, -53, 878, 1493, 19, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Wisp Dungeon Level 2
    { 951, 1442, -6, 947, 1464, -51, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Cyclops Dungeon Level 1
    { 951, 1443, -6, 947, 1465, -51, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Cyclops Dungeon Level 1
    { 951, 1444, -6, 947, 1466, -51, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Cyclops Dungeon Level 1
    { 951, 1445, -6, 947, 1467, -51, "ilshenar", "ilshenar" },  //Cyclops Dungeon Passage to Cyclops Dungeon Level 1
    { 948, 1464, -56, 952, 1442, -11, "ilshenar", "ilshenar" }, //Cyclops Dungeon Level 1 to Cyclops Dungeon Passage
    { 948, 1465, -56, 952, 1443, -11, "ilshenar", "ilshenar" }, //Cyclops Dungeon Level 1 to Cyclops Dungeon Passage
    { 948, 1466, -56, 952, 1444, -11, "ilshenar", "ilshenar" }, //Cyclops Dungeon Level 1 to Cyclops Dungeon Passage
    { 948, 1467, -56, 952, 1445, -11, "ilshenar", "ilshenar" }, //Cyclops Dungeon Level 1 to Cyclops Dungeon Passage

    //Cyclops Dungeon
    { 904, 1360, -21, 1013, 1506, 0, "ilshenar", "ilshenar" },  //Cyclops Dungeon Entrance To Cyclops Dungeon Level 1
    { 904, 1361, -21, 1013, 1507, 0, "ilshenar", "ilshenar" },  //Cyclops Dungeon Entrance To Cyclops Dungeon Level 1
    { 904, 1362, -21, 1013, 1508, 0, "ilshenar", "ilshenar" },  //Cyclops Dungeon Entrance To Cyclops Dungeon Level 1
    { 904, 1363, -21, 1013, 1509, 0, "ilshenar", "ilshenar" },  //Cyclops Dungeon Entrance To Cyclops Dungeon Level 1
    { 1014, 1506, 0, 905, 1360, -21, "ilshenar", "ilshenar" },  //Cyclops Dungeon Level 1 to Cyclops Dungeon Entrance
    { 1014, 1507, 0, 905, 1361, -21, "ilshenar", "ilshenar" },  //Cyclops Dungeon Level 1 to Cyclops Dungeon Entrance
    { 1014, 1508, 0, 905, 1362, -21, "ilshenar", "ilshenar" },  //Cyclops Dungeon Level 1 to Cyclops Dungeon Entrance
    { 1014, 1509, 0, 905, 1363, -21, "ilshenar", "ilshenar" },  //Cyclops Dungeon Level 1 to Cyclops Dungeon Entrance

    //Exodus Dungeon
    { 827, 777, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 827, 778, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 827, 779, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 828, 777, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 828, 778, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 828, 779, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 829, 777, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 829, 778, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 829, 779, -80, 1975, 114, -28, "ilshenar", "ilshenar" },  //Exodus City to Exodus Dungeon
    { 1978, 114, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1978, 115, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1978, 116, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1978, 117, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1979, 114, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1979, 115, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1979, 116, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1979, 117, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1980, 114, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1980, 115, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1980, 116, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1980, 117, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1981, 114, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1981, 115, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1981, 116, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City
    { 1981, 117, -28, 835, 778, -80, "ilshenar", "ilshenar" },  //Exodus Dungeon to Exodus City

    //Central Ilshenar
    { 1237, 583, -19, 1139, 594, -80, "ilshenar", "ilshenar" }, //Central Area East Entrance
    { 1237, 584, -19, 1140, 594, -80, "ilshenar", "ilshenar" }, //Central Area East Entrance
    { 1237, 585, -19, 1141, 594, -80, "ilshenar", "ilshenar" }, //Central Area East Entrance
    { 1139, 593, -80, 1238, 583, -19, "ilshenar", "ilshenar" }, //Central Area East Exit
    { 1140, 593, -80, 1238, 584, -19, "ilshenar", "ilshenar" }, //Central Area East Exit
    { 1141, 593, -80, 1238, 585, -19, "ilshenar", "ilshenar" }, //Central Area East Exit
    { 1142, 593, -80, 1238, 585, -19, "ilshenar", "ilshenar" }, //Central Area East Exit

    //Central Ilshenar
    { 709, 668, -39, 913, 452, -79, "ilshenar", "ilshenar" },   //Central Area West Entrance
    { 710, 668, -38, 913, 453, -79, "ilshenar", "ilshenar" },   //Central Area West Entrance
    { 912, 451, -78, 709, 669, -38, "ilshenar", "ilshenar" },   //Central Area West Exit
    { 912, 452, -78, 709, 669, -38, "ilshenar", "ilshenar" },   //Central Area West Exit
    { 912, 453, -78, 710, 669, -38, "ilshenar", "ilshenar" },   //Central Area West Exit

    //Ancient Lair
    { 938, 494, -40, 83, 748, -26, "ilshenar", "ilshenar" },    //Ancient Lair Entrance
    { 939, 494, -40, 84, 748, -26, "ilshenar", "ilshenar" },    //Ancient Lair Entrance
    { 940, 494, -40, 85, 748, -26, "ilshenar", "ilshenar" },    //Ancient Lair Entrance
    { 941, 494, -40, 86, 748, -26, "ilshenar", "ilshenar" },    //Ancient Lair Entrance
    { 83, 749, -23, 938, 495, -35, "ilshenar", "ilshenar" },    //Ancient Lair
    { 84, 749, -23, 939, 495, -35, "ilshenar", "ilshenar" },    //Ancient Lair
    { 85, 749, -23, 940, 495, -35, "ilshenar", "ilshenar" },    //Ancient Lair
    { 86, 749, -23, 941, 495, -35, "ilshenar", "ilshenar" },    //Ancient Lair
    { 87, 749, -23, 941, 495, -35, "ilshenar", "ilshenar" },    //Ancient Lair

    //Ratman Mine
    { 1029, 1153, -24, 1348, 1509, -3, "ilshenar", "ilshenar" },    //Ratman Mine Entrance to Ratman Mine Level 1
    { 1029, 1154, -24, 1348, 1510, -3, "ilshenar", "ilshenar" },    //Ratman Mine Entrance to Ratman Mine Level 1
    { 1029, 1155, -24, 1348, 1511, -3, "ilshenar", "ilshenar" },    //Ratman Mine Entrance to Ratman Mine Level 1
    { 1349, 1509, -3, 1030, 1153, -24, "ilshenar", "ilshenar" },    //Ratman Mine Level 1 to Ratman Mine Entrance
    { 1349, 1510, -3, 1030, 1154, -24, "ilshenar", "ilshenar" },    //Ratman Mine Level 1 to Ratman Mine Entrance
    { 1349, 1511, -3, 1030, 1155, -24, "ilshenar", "ilshenar" },    //Ratman Mine Level 1 to Ratman Mine Entrance
    { 1268, 1508, -28, 1249, 1508, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 1 to Ratman Mine Level 2
    { 1268, 1509, -28, 1249, 1509, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 1 to Ratman Mine Level 2
    { 1268, 1510, -28, 1249, 1510, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 1 to Ratman Mine Level 2
    { 1250, 1508, -28, 1269, 1508, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 2 to Ratman Mine Level 1
    { 1250, 1509, -28, 1269, 1509, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 2 to Ratman Mine Level 1
    { 1250, 1510, -28, 1269, 1510, -28, "ilshenar", "ilshenar" },   //Ratman Mine Level 2 to Ratman Mine Level 1

    //Mushroom Cave
    { 1456, 1328, -27, 1478, 1494, -28, "ilshenar", "ilshenar" },   //Mushroom Cave Entrance
    { 1456, 1329, -27, 1478, 1495, -28, "ilshenar", "ilshenar" },   //Mushroom Cave Entrance
    { 1456, 1330, -27, 1478, 1496, -28, "ilshenar", "ilshenar" },   //Mushroom Cave Entrance
    { 1479, 1493, -28, 1457, 1328, -27, "ilshenar", "ilshenar" },   //Mushroom Cave Exit
    { 1479, 1494, -28, 1457, 1328, -27, "ilshenar", "ilshenar" },   //Mushroom Cave Exit
    { 1479, 1495, -28, 1457, 1329, -27, "ilshenar", "ilshenar" },   //Mushroom Cave Exit
    { 1479, 1496, -28, 1457, 1330, -27, "ilshenar", "ilshenar" },   //Mushroom Cave Exit

    //Spider Cave
    { 1419, 909, -10, 1784, 993, -28, "ilshenar", "ilshenar" }, //Spider Cave Entrance
    { 1420, 909, -10, 1785, 993, -28, "ilshenar", "ilshenar" }, //Spider Cave Entrance
    { 1421, 909, -10, 1786, 993, -28, "ilshenar", "ilshenar" }, //Spider Cave Entrance
    { 1784, 994, -29, 1419, 910, -10, "ilshenar", "ilshenar" }, //Spider Cave Exit
    { 1785, 994, -29, 1420, 910, -10, "ilshenar", "ilshenar" }, //Spider Cave Exit
    { 1786, 994, -29, 1421, 910, -10, "ilshenar", "ilshenar" }, //Spider Cave Exit
    { 1861, 980, -28, 1490, 877, 10, "ilshenar", "ilshenar" },  //Spider Cave to Spider Shrine
    { 1861, 981, -28, 1490, 878, 10, "ilshenar", "ilshenar" },  //Spider Cave to Spider Shrine
    { 1861, 982, -28, 1490, 879, 10, "ilshenar", "ilshenar" },  //Spider Cave to Spider Shrine
    { 1861, 983, -28, 1490, 880, 10, "ilshenar", "ilshenar" },  //Spider Cave to Spider Shrine
    { 1861, 984, -28, 1490, 880, 10, "ilshenar", "ilshenar" },  //Spider Cave to Spider Shrine
    { 1516, 879, 10, 1363, 1105, -26, "ilshenar", "ilshenar" }, //Spider Shrine to Reg Volom (Spectre Dungeon)

    //Spectre Dungeon
    { 1362, 1031, -13, 1981, 1106, -21, "ilshenar", "ilshenar" },   //Spectre Dungeon Entrance
    { 1363, 1031, -13, 1982, 1106, -21, "ilshenar", "ilshenar" },   //Spectre Dungeon Entrance
    { 1364, 1031, -13, 1983, 1106, -21, "ilshenar", "ilshenar" },   //Spectre Dungeon Entrance
    { 1980, 1107, -16, 1362, 1032, -11, "ilshenar", "ilshenar" },   //Spectre Dungeon Exit
    { 1981, 1107, -16, 1362, 1032, -11, "ilshenar", "ilshenar" },   //Spectre Dungeon Exit
    { 1982, 1107, -16, 1363, 1032, -11, "ilshenar", "ilshenar" },   //Spectre Dungeon Exit
    { 1983, 1107, -16, 1364, 1032, -11, "ilshenar", "ilshenar" },   //Spectre Dungeon Exit
    { 1984, 1107, -16, 1364, 1032, -11, "ilshenar", "ilshenar" },   //Spectre Dungeon Exit

    //Rock Dungeon
    { 1787, 569, 74, 2187, 319, -7, "ilshenar", "ilshenar" },   //Rock Dungeon Entrance to Rock Dungeon Passage
    { 1788, 569, 74, 2187, 319, -7, "ilshenar", "ilshenar" },   //Rock Dungeon Entrance to Rock Dungeon Passage
    { 2187, 320, -7, 1787, 570, 71, "ilshenar", "ilshenar" },   //Rock Dungeon Passage to Rock Dungeon Entrance
    { 2188, 320, -7, 1787, 570, 71, "ilshenar", "ilshenar" },   //Rock Dungeon Passage to Rock Dungeon Entrance
    { 2189, 320, -7, 1788, 570, 71, "ilshenar", "ilshenar" },   //Rock Dungeon Passage to Rock Dungeon Entrance
    { 2186, 294, -27, 2186, 32, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Passage to Rock Dungeon Level 1
    { 2187, 294, -27, 2187, 32, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Passage to Rock Dungeon Level 1
    { 2188, 294, -27, 2188, 32, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Passage to Rock Dungeon Level 1
    { 2189, 294, -27, 2189, 32, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Passage to Rock Dungeon Level 1
    { 2186, 33, -27, 2186, 295, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Level 1 to Rock Dungeon Passage
    { 2187, 33, -27, 2187, 295, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Level 1 to Rock Dungeon Passage
    { 2188, 33, -27, 2188, 295, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Level 1 to Rock Dungeon Passage
    { 2189, 33, -27, 2189, 295, -27, "ilshenar", "ilshenar" },  //Rock Dungeon Level 1 to Rock Dungeon Passage

    //Sparkles
    { 1576, 849, -25, 857, 902, -33, "ilshenar", "ilshenar" },   //Sparkles from Lake to Dam
    { 857, 903, -33, 1577, 850, -25, "ilshenar", "ilshenar" },   //Sparkles from Dam to Lake

    //Meer Caverns
    { 1037, 579, -73, 1790, 66, -26, "ilshenar", "ilshenar" },  //Meer Caverns Entrance
    { 1037, 580, -73, 1790, 67, -26, "ilshenar", "ilshenar" },
    { 1037, 581, -73, 1790, 68, -26, "ilshenar", "ilshenar" },
    { 1038, 578, -73, 1786, 70, -26, "ilshenar", "ilshenar" },
    { 1038, 579, -73, 1786, 70, -26, "ilshenar", "ilshenar" },
    { 1038, 581, -73, 1786, 65, -26, "ilshenar", "ilshenar" },
    { 1038, 582, -73, 1786, 65, -26, "ilshenar", "ilshenar" },
    { 1039, 578, -73, 1787, 70, -26, "ilshenar", "ilshenar" },
    { 1039, 582, -73, 1787, 65, -26, "ilshenar", "ilshenar" },
    { 1040, 578, -73, 1788, 70, -26, "ilshenar", "ilshenar" },
    { 1040, 579, -73, 1788, 70, -26, "ilshenar", "ilshenar" },
    { 1040, 581, -73, 1788, 65, -26, "ilshenar", "ilshenar" },
    { 1040, 582, -73, 1788, 65, -26, "ilshenar", "ilshenar" },
    { 1041, 579, -73, 1785, 66, -26, "ilshenar", "ilshenar" },
    { 1041, 580, -73, 1785, 67, -26, "ilshenar", "ilshenar" },
    { 1041, 581, -73, 1785, 68, -26, "ilshenar", "ilshenar" },
    { 1139, 592, -80, 1238, 583, -19, "ilshenar", "ilshenar" },
    { 1140, 592, -80, 1238, 584, -19, "ilshenar", "ilshenar" },
    { 1141, 592, -80, 1238, 585, -19, "ilshenar", "ilshenar" },
    { 1142, 592, -80, 1238, 585, -19, "ilshenar", "ilshenar" },
    { 1786, 66, -26, 1038, 583, -78, "ilshenar", "ilshenar" },  //Meer Caverns Exit
    { 1786, 67, -26, 1042, 579, -78, "ilshenar", "ilshenar" },
    { 1786, 68, -26, 1042, 580, -78, "ilshenar", "ilshenar" },
    { 1786, 69, -26, 1042, 581, -78, "ilshenar", "ilshenar" },
    { 1787, 66, -26, 1039, 583, -78, "ilshenar", "ilshenar" },
    { 1787, 69, -26, 1038, 577, -78, "ilshenar", "ilshenar" },
    { 1788, 66, -26, 1040, 583, -78, "ilshenar", "ilshenar" },
    { 1788, 69, -26, 1039, 577, -78, "ilshenar", "ilshenar" },
    { 1789, 66, -26, 1041, 583, -78, "ilshenar", "ilshenar" },
    { 1789, 67, -26, 1036, 579, -78, "ilshenar", "ilshenar" },
    { 1789, 68, -26, 1036, 578, -78, "ilshenar", "ilshenar" },
    { 1789, 69, -26, 1036, 581, -78, "ilshenar", "ilshenar" },

    ////////////////////////////////
    ////////////////////////////////
    // Termur
    // Incomplete
    ///////////////////////////////
    ///////////////////////////////

    //Tomb of Kings
    { 33, 241, 7, 996, 3842, -41, "termur", "termur" },     //Holy City to Tomb of Kings
    { 34, 241, 7, 997, 3842, -41, "termur", "termur" },
    { 35, 241, 7, 998, 3842, -41, "termur", "termur" },
    { 36, 241, 7, 999, 3842, -41, "termur", "termur" },
    { 37, 241, 7, 1000, 3842, -41, "termur", "termur" },
    { 996, 3841, -41, 33, 240, 5, "termur", "termur" },     //Tomb of Kings to Holy City
    { 997, 3841, -41, 34, 240, 5, "termur", "termur" },
    { 998, 3841, -41, 35, 240, 5, "termur", "termur" },
    { 999, 3841, -41, 36, 240, 5, "termur", "termur" },
    { 1000, 3841, -41, 37, 240, 5, "termur", "termur" },

    //Stygian Dungeon Lair
    //Encounter Room 1
    { 79, 379, 5, 52, 389, 65, "termur", "termur" },        //Tele A Bottom Level
    { 52, 389, 65, 79, 379, 5, "termur", "termur" },        //Tele A Top Level
    { 52, 343, 65, 80, 338, 85, "termur", "termur" },       //Tele B Bottom Level
    { 80, 338, 85, 52, 343, 64, "termur", "termur" },       //Tele B Top Level
    { 45, 366, 85, 81, 399, 105, "termur", "termur" },      //Tele C Bottom Level
    { 81, 399, 105, 45, 366, 85, "termur", "termur" },      //Tele C Top Level
    { 79, 353, 5, 40, 366, 105, "termur", "termur" },       //Tele D Bottom Level
    { 40, 366, 105, 79, 353, 5, "termur", "termur" },       //Tele D Top Level

    //Encounter Room 2


    //Unsorted
    { 40, 564, 105, 79, 551, 5, "termur", "termur" },
    { 40, 772, 105, 79, 759, 5, "termur", "termur" },
    
    { 45, 564, 85, 81, 597, 105, "termur", "termur" },
    { 45, 772, 85, 81, 805, 105, "termur", "termur" },
    
   
    { 52, 541, 65, 80, 536, 85, "termur", "termur" },
    { 52, 587, 65, 79, 577, 5, "termur", "termur" },
    { 52, 749, 65, 80, 744, 85, "termur", "termur" },
    { 52, 795, 65, 79, 785, 5, "termur", "termur" },
   
    
    { 79, 551, 5, 40, 564, 105, "termur", "termur" },
    { 79, 577, 5, 52, 587, 65, "termur", "termur" },
    { 79, 759, 5, 40, 772, 105, "termur", "termur" },
    { 79, 785, 5, 52, 795, 65, "termur", "termur" },
    
    { 80, 536, 85, 52, 541, 64, "termur", "termur" },
    { 80, 744, 85, 52, 749, 64, "termur", "termur" },
    
    { 81, 597, 105, 45, 564, 85, "termur", "termur" },
    { 81, 805, 105, 45, 772, 85, "termur", "termur" },
    { 317, 136, 65, 310, 159, 85, "termur", "termur" },
    { 317, 182, 65, 346, 192, 105, "termur", "termur" },
    { 344, 146, 5, 349, 138, 60, "termur", "termur" },
    { 344, 172, 5, 349, 180, 60, "termur", "termur" },
    { 345, 131, 85, 345, 128, 100, "termur", "termur" },
    { 442, 152, -38, 850, 272, -11, "termur", "termur" },
    { 442, 153, -38, 850, 273, -11, "termur", "termur" },
    { 442, 166, -38, 850, 286, -11, "termur", "termur" },
    { 442, 167, -38, 850, 287, -11, "termur", "termur" },
    { 511, 584, 11, 6992, 1367, -15, "termur", "Felucca" },
    { 512, 584, 11, 6992, 1367, -15, "termur", "Felucca" },
    { 513, 584, 11, 6993, 1367, -15, "termur", "Felucca" },
    { 514, 584, 11, 6993, 1367, -15, "termur", "Felucca" },
    { 519, 919, 39, 1125, 1075, -117, "termur", "termur" },
    { 519, 919, 39, 1125, 1076, -117, "termur", "termur" },
    { 520, 919, 39, 1125, 1075, -117, "termur", "termur" },
    { 520, 919, 39, 1126, 1076, -117, "termur", "termur" },
    { 521, 919, 39, 1126, 1075, -117, "termur", "termur" },
    { 522, 919, 36, 1126, 1075, -117, "termur", "termur" },
    { 594, 3846, -31, 644, 3857, -43, "termur", "termur" },
    { 644, 3857, -43, 594, 3846, -31, "termur", "termur" },
    { 841, 272, 30, 434, 152, 0, "termur", "termur" },
    { 841, 273, 30, 434, 153, 0, "termur", "termur" },
    { 841, 287, 30, 433, 167, 0, "termur", "termur" },
    { 843, 286, 22, 433, 166, 0, "termur", "termur" },
   
    { 1006, 1110, -42, 1012, 1071, -32, "termur", "termur" },
    { 1006, 1127, -42, 1010, 1144, -65, "termur", "termur" },
    { 1010, 1144, -65, 1006, 1127, -42, "termur", "termur" },
    { 1012, 1071, -32, 1006, 1110, -42, "termur", "termur" },
    { 1042, 1047, -33, 1238, 1151, -42, "termur", "termur" },
    { 1079, 882, 0, 1080, 977, -21, "termur", "termur" },
    { 1079, 883, 0, 1080, 977, -21, "termur", "termur" },
    { 1079, 884, 0, 1080, 977, -21, "termur", "termur" },
    { 1079, 885, 0, 1080, 977, -21, "termur", "termur" },
    { 1080, 974, -20, 1081, 883, -5, "termur", "termur" },
    { 1080, 975, -20, 1081, 883, -5, "termur", "termur" },
    { 1081, 974, -20, 1081, 883, -5, "termur", "termur" },
    { 1081, 975, -20, 1081, 883, -5, "termur", "termur" },
    { 1125, 1076, -117, 520, 920, 39, "termur", "termur" },
    { 1125, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1125, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1126, 1076, 15, 521, 920, 39, "termur", "termur" },
    { 1126, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1126, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1127, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1127, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1128, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1128, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1129, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1129, 1215, 5, 4195, 3261, 5, "termur", "britannia_alt" },
    { 1130, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1130, 1215, 5, 4195, 3261, 5, "termur", "britannia_alt" },
    { 1131, 1215, 5, 4194, 3261, 5, "termur", "britannia_alt" },
    { 1131, 1215, 5, 4195, 3261, 5, "termur", "britannia_alt" },
    { 1162, 1120, -42, 1147, 1089, -42, "termur", "termur" },
    { 1180, 885, 0, 1187, 1127, -42, "termur", "termur" },
    { 1183, 1180, -45, 1183, 1182, -47, "termur", "termur" },
    { 1183, 1182, -47, 1183, 1180, -45, "termur", "termur" },
    { 1183, 1187, -45, 1183, 1189, -44, "termur", "termur" },
    { 1183, 1189, -44, 1183, 1187, -45, "termur", "termur" },
    { 1184, 1177, -47, 1186, 1177, -45, "termur", "termur" },
    { 1186, 1177, -45, 1184, 1177, -47, "termur", "termur" },
    { 1186, 1183, -40, 1184, 1183, -43, "termur", "termur" },
    { 1187, 1126, -42, 1180, 884, 0, "termur", "termur" },
    { 1187, 1173, -47, 1187, 1175, -45, "termur", "termur" },
    { 1187, 1175, -45, 1187, 1173, -47, "termur", "termur" },
    { 1190, 1185, -40, 1190, 1183, -40, "termur", "termur" },
    { 1192, 1186, -40, 1190, 1186, -41, "termur", "termur" },
    { 1197, 1189, -58, 1195, 1189, -56, "termur", "termur" },
    { 1198, 1187, -45, 1196, 1187, -45, "termur", "termur" },
    { 1200, 1187, -66, 1200, 1190, -64, "termur", "termur" },
    { 1201, 1189, -71, 1201, 1186, -68, "termur", "termur" },
    { 1202, 1183, -45, 1202, 1185, -45, "termur", "termur" },
    { 1203, 1190, -74, 1201, 1190, -73, "termur", "termur" },
    { 1206, 1181, -47, 1204, 1181, -45, "termur", "termur" },
    { 1238, 1151, -42, 1140, 951, -42, "termur", "termur" },
    { 4194, 3260, 5, 1128, 1213, -2, "termur", "termur" },
    { 4195, 3260, 5, 1129, 1213, -2, "termur", "termur" },
    { 7149, 756, 25, 511, 585, 11, "termur", "termur" },
    { 7149, 756, 25, 511, 585, 9, "termur", "termur" },
    { 7150, 756, 25, 511, 585, 9, "termur", "termur" },
    { 7151, 756, 25, 511, 585, 9, "termur", "termur" },
    { 7152, 756, 25, 511, 585, 9, "termur", "termur" },
    { 7153, 756, 25, 511, 585, 9, "termur", "termur" }
    

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
           fromZ := GetWorldHeight( fromx, fromy, fromR );
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