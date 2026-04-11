var GUILD_PRICE := 120000;
var TEXT_ENTRY_GUILDNAME := 1;
var TEXT_ENTRY_GUILDABV := 2;

CONST HEAR_ALL := 3;
CONST HEAR_OWN := 2;
CONST HEAR_ALLY := 1;
CONST HEAR_NONE := 0;

CONST TALK_ALL := 3;
CONST TALK_OWN := 2;
CONST TALK_ALLY := 1;
CONST TALK_NONE := 0;

const MAX_GUILD_NAME_SIZE:=45;
const MAX_GUILD_ABREV_SIZE:=6;
const MAX_GUILD_TITLE_SIZE:=20;

const REQUEST_DECLARE_WAR := "DW";
const REQUEST_END_WAR := "EW";
const REQUEST_FORM_ALLIANCE := "RA";

const SEND_SURRENDER := "S";

const STATUS_WAR_STARTING := "WS";
const STATUS_WAR_ENDING := "WE";
const STATUS_ALLIANCE_FORMING := "FA";
const STATUS_ALLIANCE_ENDING := "EA";

const GUILD_UNIFORM_PRICE := 150000;
const GUILD_COLOUR_TIME := 24; //hours
const GUILD_COLOUR_TIME_SET := "TimeGuildColour";
const GUILD_NAME_TIME := 7; // days
const GUILD_NAME_TIME_SET := "TimeGuildName";
const GUILD_UNIFORM := "GuildUniform";
const GUILD_COLOUR := "GuildColour";
const GUILD_NAME := "GuildName";
const GUILD_ABV := "GuildAbv";

const GUILD_MEMBER_TITLE := "GuildTitle";

// guild clothes (matched to pkg/std/tailoring/itemdesc.cfg)
// 0x1EFD=fancyshirt, 0x1EFF=fancydress, 0x1F01=plaindress, 0x1F03=Robe
// 0x1F7B=doublet, 0x1F9F=jestersuit, 0x1FA1=tunic, 0x1FFD=surcoat
// 0x1FFE=surcoat2, 0x1515=cloak, 0x1516=skirt, 0x1517=shirt, 0x152E=shortpants
// 0x1537=kilt, 0x1539=longpants, 0x153B=halfapron, 0x153D=fullapron
// 0x1541=bodysash, 0x153F=bandana, 0x1543=skullcap, 0x141b=orcmask
// 0x1545=bearmask, 0x1547=deermask, 0x1549=tribalmask, 0x154c=voodoomask2
// 0x1713=floppyhat, 0x1714=widebrimhat, 0x1715=cap, 0x1716=tallhat, 0x1717=strawhat
// 0x1718=wizardhat, 0x1719=bonnet, 0x171A=featheredhat, 0x171B=tricornehat
// 0x171C=jesterhat, 0x170B=LeatherBoots, 0x170D=sandals, 0x170F=shoes
// 0x1711=ThighBoots, 0x2B04=elvencloak
// 0x2782=malekimono, 0x2783=femalekimono, 0x27E4=kamishimo, 0x25EA=checkeredshirt
// 0x27A1=jinbaori, 0x27A0=obi, 0x279C=hakamashita, 0x279B=tattsukehakama, 0x279A=hakama
// 0x3172=elvenglasses, 0x3173=elvenrobe1, 0x3174=elvenrobe2, 0x3175=elvenmaleshirt
// 0x3176=elvenmaleshirt2, 0x3177=elvenfemaleshirt, 0x3178=elvenfemaleshirt2
// 0x3179=elvenpants, 0x317A=elvenboot
var CLOTHING := {0x1EFD, 0x1EFF, 0x1F01, 0x1F03, 0x1F7B, 0x1F9F, 0x1FA1, 0x1FFD, 0x1FFE, 0x1515, 0x1516, 0x1517, 0x152E, 0x1537, 0x1539, 0x153B, 0x153D, 0x1541, 0x153F, 0x1543, 0x141b, 0x1545, 0x1547, 0x1549, 0x154c, 0x1713, 0x1714, 0x1715, 0x1716, 0x1717, 0x1718, 0x1719, 0x171A, 0x171B, 0x171C, 0x170B, 0x170D, 0x170F, 0x1711, 0x2B04, 0x2782, 0x2783, 0x27E4, 0x25EA, 0x27A1, 0x27A0, 0x279C, 0x279B, 0x279A, 0x3172, 0x3173, 0x3174, 0x3175, 0x3176, 0x3177, 0x3178, 0x3179, 0x317A};

var COLOURS := {1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1185, 1260, 1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268, 1269, 1271, 1272, 1280,1281,1282,1283,1284,1285,1286, 1287, 1288, 1289, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1305, 1306, 1362, 1364, 1368, 1371, 1372, 1375, 1377, 1380, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1397, 1399, 1400, 1467, 1472, 1476, 1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488, 1491, 1493, 1497, 1500, 2706, 2707, 2708, 2709, 2710, 2711, 2712, 2713, 2714, 2715, 2716, 2717, 2718, 2719, 2720, 2721, 2722, 2724, 2725, 2726, 2728, 2729, 2730, 2732, 2733, 2735, 2736, 2738, 2743, 2745, 2746, 2747, 2748, 2749, 2750, 2751, 2752, 2753, 2754, 2755, 2928, 2955};