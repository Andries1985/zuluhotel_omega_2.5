//Edited Augst 28, 2024

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////
//  CLIENT.INC - defines client releated constants.
//
//  This include file is designed to hold all the client lowlevel constants.
//  Not all constants might be usable as of now, but in the future, they
//  might be.
//
//  Author: Ignatius 1999/01/20
//
//  Recent Modifications
//		Added all the sounds from UOFiddler
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////


// DEBUG MODE
const DEBUG_MODE	:= 0;


////
// Soundeffects
// The first section is just an enum of existing sounds,
// the second builds on the first ones, but providing more
// logical names. 
///

const SFX_1 := 0x001; // d_frst01.wav
const SFX_2 := 0x002; // d_frst02.wav
const SFX_3 := 0x003; // d_frst03.wav
const SFX_4 := 0x004; // d_jung01.wav
const SFX_5 := 0x005; // d_jung02.wav
const SFX_6 := 0x006; // d_jung03.wav
const SFX_7 := 0x007; // d_swmp01.wav
const SFX_8 := 0x008; // d_swmp02.wav
const SFX_9 := 0x009; // n_frst01.wav
const SFX_A := 0x00A; // n_frst02.wav
const SFX_B := 0x00B; // n_gen01.wav
const SFX_C := 0x00C; // n_gen02.wav
const SFX_D := 0x00D; // n_jung01.wav
const SFX_E := 0x00E; // n_jung02.wav
const SFX_F := 0x00F; // n_swmp01.wav
const SFX_10 := 0x010; // n_swmp02.wav
const SFX_11 := 0x011; // watr01_l.wav
const SFX_12 := 0x012; // watr02_l.wav
const SFX_13 := 0x013; // wave01_l.wav
const SFX_14 := 0x014; // wave02_l.wav
const SFX_15 := 0x015; // wind01_l.wav
const SFX_16 := 0x016; // wind02_l.wav
const SFX_17 := 0x017; // wind03_l.wav
const SFX_18 := 0x018; // bird01.wav
const SFX_19 := 0x019; // bird02.wav
const SFX_1A := 0x01A; // bird03.wav
const SFX_1B := 0x01B; // bird04.wav
const SFX_1C := 0x01C; // bird05.wav
const SFX_1D := 0x01D; // bird06.wav
const SFX_1E := 0x01E; // bird07.wav
const SFX_1F := 0x01F; // bird08.wav
const SFX_20 := 0x020; // bird09.wav
const SFX_21 := 0x021; // bubl01_l.wav
const SFX_22 := 0x022; // bubl02_l.wav
const SFX_23 := 0x023; // drip01.wav
const SFX_24 := 0x024; // drip02.wav
const SFX_25 := 0x025; // drip03_l.wav
const SFX_26 := 0x026; // splash01.wav
const SFX_27 := 0x027; // splash02.wav
const SFX_28 := 0x028; // splash03.wav
const SFX_29 := 0x029; // thundr01.wav
const SFX_2A := 0x02A; // thundr02.wav
const SFX_2B := 0x02B; // anvil.wav
const SFX_2C := 0x02C; // bellows.wav
const SFX_2D := 0x02D; // chstcls3.wav
const SFX_2E := 0x02E; // chstopn2.wav
const SFX_2F := 0x02F; // drawercl.wav
const SFX_30 := 0x030; // drawerop.wav
const SFX_31 := 0x031; // drink01.wav
const SFX_32 := 0x032; // drink05.wav
const SFX_33 := 0x033; // dropgem1.wav
const SFX_34 := 0x034; // dropgem2.wav
const SFX_35 := 0x035; // dropgem3.wav
const SFX_36 := 0x036; // dropmny1.wav
const SFX_37 := 0x037; // dropmny2.wav
const SFX_38 := 0x038; // dropmny3.wav
const SFX_39 := 0x039; // drum01.wav
const SFX_3A := 0x03A; // drum01_b.wav
const SFX_3B := 0x03B; // eat01.wav
const SFX_3C := 0x03C; // eat03.wav
const SFX_3D := 0x03D; // eat06.wav
const SFX_3E := 0x03E; // flute01.wav
const SFX_3F := 0x03F; // glasbrk1.wav
const SFX_40 := 0x040; // glasbrk2.wav
const SFX_41 := 0x041; // glasbrk3.wav
const SFX_42 := 0x042; // glasbrk4.wav
const SFX_43 := 0x043; // hamer01.wav
const SFX_44 := 0x044; // harp01.wav
const SFX_45 := 0x045; // harp01_b.wav
const SFX_46 := 0x046; // harp02.wav
const SFX_47 := 0x047; // harp02_b.wav
const SFX_48 := 0x048; // igntorch.wav
const SFX_49 := 0x049; // leather1.wav
const SFX_4A := 0x04A; // lever01.wav
const SFX_4B := 0x04B; // lever02.wav
const SFX_4C := 0x04C; // lever05.wav
const SFX_4D := 0x04D; // lute01.wav
const SFX_4E := 0x04E; // lute01_b.wav
const SFX_4F := 0x04F; // pouring.wav
const SFX_50 := 0x050; // rustle.wav
const SFX_51 := 0x051; // swallow2.wav
const SFX_52 := 0x052; // switch01.wav
const SFX_53 := 0x053; // tamborin.wav
const SFX_54 := 0x054; // tambrn_b.wav
const SFX_55 := 0x055; // torch01.wav
const SFX_56 := 0x056; // turnpage.wav
const SFX_57 := 0x057; // unsheth1.wav
const SFX_58 := 0x058; // usecloth.wav
const SFX_59 := 0x059; // uselethr.wav
const SFX_5A := 0x05A; // uselthr1.wav
const SFX_5B := 0x05B; // aligatr1.wav
const SFX_5C := 0x05C; // aligatr2.wav
const SFX_5D := 0x05D; // aligatr3.wav
const SFX_5E := 0x05E; // aligatr4.wav
const SFX_5F := 0x05F; // aligatr5.wav
const SFX_60 := 0x060; // bear1.wav
const SFX_61 := 0x061; // bear2.wav
const SFX_62 := 0x062; // bear3.wav
const SFX_63 := 0x063; // bear4.wav
const SFX_64 := 0x064; // bear5.wav
const SFX_65 := 0x065; // bull1.wav
const SFX_66 := 0x066; // bull2.wav
const SFX_67 := 0x067; // bull3.wav
const SFX_68 := 0x068; // bull4.wav
const SFX_69 := 0x069; // bull5.wav
const SFX_6A := 0x06A; // cat1.wav
const SFX_6B := 0x06B; // cat2.wav
const SFX_6C := 0x06C; // cat3.wav
const SFX_6D := 0x06D; // cat4.wav
const SFX_6E := 0x06E; // cat5.wav
const SFX_6F := 0x06F; // chicken1.wav
const SFX_70 := 0x070; // chicken2.wav
const SFX_71 := 0x071; // chicken3.wav
const SFX_72 := 0x072; // chicken4.wav
const SFX_73 := 0x073; // chicken5.wav
const SFX_74 := 0x074; // cougar1.wav
const SFX_75 := 0x075; // cougar2.wav
const SFX_76 := 0x076; // cougar3.wav
const SFX_77 := 0x077; // cougar4.wav
const SFX_78 := 0x078; // cougar5.wav
const SFX_79 := 0x079; // cow1.wav
const SFX_7A := 0x07A; // cow2.wav
const SFX_7B := 0x07B; // cow3.wav
const SFX_7C := 0x07C; // cow4.wav
const SFX_7D := 0x07D; // cow5.wav
const SFX_7E := 0x07E; // crow1.wav
const SFX_7F := 0x07F; // crow2.wav
const SFX_80 := 0x080; // crow3.wav
const SFX_81 := 0x081; // crow4.wav
const SFX_82 := 0x082; // crow5.wav
const SFX_83 := 0x083; // deer3.wav
const SFX_84 := 0x084; // deer4.wav
const SFX_85 := 0x085; // deer5.wav
const SFX_86 := 0x086; // dog1.wav
const SFX_87 := 0x087; // dog2.wav
const SFX_88 := 0x088; // dog3.wav
const SFX_89 := 0x089; // dog4.wav
const SFX_8A := 0x08A; // dog5.wav
const SFX_8B := 0x08B; // dolphin1.wav
const SFX_8C := 0x08C; // dolphin2.wav
const SFX_8D := 0x08D; // dolphin3.wav
const SFX_8E := 0x08E; // dolphin4.wav
const SFX_8F := 0x08F; // dolphin5.wav
const SFX_90 := 0x090; // eagle1.wav
const SFX_91 := 0x091; // eagle2.wav
const SFX_92 := 0x092; // eagle3.wav
const SFX_93 := 0x093; // eagle4.wav
const SFX_94 := 0x094; // eagle5.wav
const SFX_95 := 0x095; // frstbrd1.wav
const SFX_96 := 0x096; // frstbrd2.wav
const SFX_97 := 0x097; // frstbrd3.wav
const SFX_98 := 0x098; // frstbrd4.wav
const SFX_99 := 0x099; // frstbrd5.wav
const SFX_9A := 0x09A; // goat1.wav
const SFX_9B := 0x09B; // goat2.wav
const SFX_9C := 0x09C; // goat3.wav
const SFX_9D := 0x09D; // goat4.wav
const SFX_9E := 0x09E; // goat5.wav
const SFX_9F := 0x09F; // gorilla1.wav
const SFX_A0 := 0x0A0; // gorilla2.wav
const SFX_A1 := 0x0A1; // gorilla3.wav
const SFX_A2 := 0x0A2; // gorilla4.wav
const SFX_A3 := 0x0A3; // gorilla5.wav
const SFX_A4 := 0x0A4; // grizzly1.wav
const SFX_A5 := 0x0A5; // grizzly2.wav
const SFX_A6 := 0x0A6; // grizzly3.wav
const SFX_A7 := 0x0A7; // grizzly4.wav
const SFX_A8 := 0x0A8; // grizzly5.wav
const SFX_A9 := 0x0A9; // horse1.wav
const SFX_AA := 0x0AA; // horse2.wav
const SFX_AB := 0x0AB; // horse3.wav
const SFX_AC := 0x0AC; // horse4.wav
const SFX_AD := 0x0AD; // horse5.wav
const SFX_AE := 0x0AE; // jungbrd1.wav
const SFX_AF := 0x0AF; // jungbrd2.wav
const SFX_B0 := 0x0B0; // jungbrd3.wav
const SFX_B1 := 0x0B1; // jungbrd4.wav
const SFX_B2 := 0x0B2; // jungbrd5.wav
const SFX_B3 := 0x0B3; // lion1.wav
const SFX_B4 := 0x0B4; // lion2.wav
const SFX_B5 := 0x0B5; // lion3.wav
const SFX_B6 := 0x0B6; // lion4.wav
const SFX_B7 := 0x0B7; // lion5.wav
const SFX_B8 := 0x0B8; // llama3.wav
const SFX_B9 := 0x0B9; // llama4.wav
const SFX_BA := 0x0BA; // llama5.wav
const SFX_BB := 0x0BB; // panther1.wav
const SFX_BC := 0x0BC; // panther2.wav
const SFX_BD := 0x0BD; // panther3.wav
const SFX_BE := 0x0BE; // panther4.wav
const SFX_BF := 0x0BF; // panther5.wav
const SFX_C0 := 0x0C0; // parrot1.wav
const SFX_C1 := 0x0C1; // parrot2.wav
const SFX_C2 := 0x0C2; // parrot3.wav
const SFX_C3 := 0x0C3; // parrot4.wav
const SFX_C4 := 0x0C4; // parrot5.wav
const SFX_C5 := 0x0C5; // pig1.wav
const SFX_C6 := 0x0C6; // pig2.wav
const SFX_C7 := 0x0C7; // pig3.wav
const SFX_C8 := 0x0C8; // pig4.wav
const SFX_C9 := 0x0C9; // pig5.wav
const SFX_CA := 0x0CA; // rabbit3.wav
const SFX_CB := 0x0CB; // rabbit4.wav
const SFX_CC := 0x0CC; // rabbit5.wav
const SFX_CD := 0x0CD; // rat1.wav
const SFX_CE := 0x0CE; // rat2.wav
const SFX_CF := 0x0CF; // rat3.wav
const SFX_D0 := 0x0D0; // rat4.wav
const SFX_D1 := 0x0D1; // rat5.wav
const SFX_D2 := 0x0D2; // raven1.wav
const SFX_D3 := 0x0D3; // raven2.wav
const SFX_D4 := 0x0D4; // raven3.wav
const SFX_D5 := 0x0D5; // raven4.wav
const SFX_D6 := 0x0D6; // raven5.wav
const SFX_D7 := 0x0D7; // sheep1.wav
const SFX_D8 := 0x0D8; // sheep2.wav
const SFX_D9 := 0x0D9; // sheep3.wav
const SFX_DA := 0x0DA; // sheep4.wav
const SFX_DB := 0x0DB; // sheep5.wav
const SFX_DC := 0x0DC; // snake1.wav
const SFX_DD := 0x0DD; // snake2.wav
const SFX_DE := 0x0DE; // snake3.wav
const SFX_DF := 0x0DF; // snake4.wav
const SFX_E0 := 0x0E0; // snake5.wav
const SFX_E1 := 0x0E1; // walrus1.wav
const SFX_E2 := 0x0E2; // walrus2.wav
const SFX_E3 := 0x0E3; // walrus3.wav
const SFX_E4 := 0x0E4; // walrus4.wav
const SFX_E5 := 0x0E5; // walrus5.wav
const SFX_E6 := 0x0E6; // wolf1.wav
const SFX_E7 := 0x0E7; // wolf2.wav
const SFX_E8 := 0x0E8; // wolf3.wav
const SFX_E9 := 0x0E9; // wolf4.wav
const SFX_EA := 0x0EA; // wolf5.wav
const SFX_EB := 0x0EB; // open01a.wav
const SFX_EC := 0x0EC; // open03a.wav
const SFX_ED := 0x0ED; // open04b.wav
const SFX_EE := 0x0EE; // open09.wav
const SFX_EF := 0x0EF; // portclos.wav
const SFX_F0 := 0x0F0; // portcul4.wav
const SFX_F1 := 0x0F1; // portcul5.wav
const SFX_F2 := 0x0F2; // shut03a.wav
const SFX_F3 := 0x0F3; // shut07a.wav
const SFX_F4 := 0x0F4; // shut14b.wav
const SFX_F5 := 0x0F5; // shut17.wav
const SFX_F6 := 0x0F6; // sfx01.wav
const SFX_F7 := 0x0F7; // sfx02.wav
const SFX_F8 := 0x0F8; // sfx04.wav
const SFX_F9 := 0x0F9; // sfx05.wav
const SFX_FA := 0x0FA; // sfx06.wav
const SFX_FB := 0x0FB; // sfx08.wav
const SFX_FC := 0x0FC; // sfx10.wav
const SFX_FD := 0x0FD; // sfx11.wav
const SFX_FE := 0x0FE; // sfx12_l.wav
const SFX_FF := 0x0FF; // sfx13.wav
const SFX_100 := 0x100; // sfx14_l.wav
const SFX_101 := 0x101; // sfx15.wav
const SFX_102 := 0x102; // sfx16_l.wav
const SFX_103 := 0x103; // sfx17_l.wav
const SFX_104 := 0x104; // sfx18_l.wav
const SFX_105 := 0x105; // sfx20_l.wav
const SFX_106 := 0x106; // sfx21_l.wav
const SFX_107 := 0x107; // sfx22_l.wav
const SFX_108 := 0x108; // airelem1.wav
const SFX_109 := 0x109; // airelem2.wav
const SFX_10A := 0x10A; // airelem3.wav
const SFX_10B := 0x10B; // airelem4.wav
const SFX_10C := 0x10C; // airelem5.wav
const SFX_10D := 0x10D; // erthel1.wav
const SFX_10E := 0x10E; // erthel2.wav
const SFX_10F := 0x10F; // erthel3.wav
const SFX_110 := 0x110; // erthel4.wav
const SFX_111 := 0x111; // erthel5.wav
const SFX_112 := 0x112; // firelem1.wav
const SFX_113 := 0x113; // firelem2.wav
const SFX_114 := 0x114; // firelem3.wav
const SFX_115 := 0x115; // firelem4.wav
const SFX_116 := 0x116; // firelem5.wav
const SFX_117 := 0x117; // watrel1.wav
const SFX_118 := 0x118; // watrel2.wav
const SFX_119 := 0x119; // watrel3.wav
const SFX_11A := 0x11A; // watrel4.wav
const SFX_11B := 0x11B; // watrel5.wav
const SFX_11C := 0x11C; // exp01.wav
const SFX_11D := 0x11D; // exp04.wav
const SFX_11E := 0x11E; // exp05.wav
const SFX_11F := 0x11F; // exp06.wav
const SFX_120 := 0x120; // bigfoot1.wav
const SFX_121 := 0x121; // bigfoot2.wav
const SFX_122 := 0x122; // feet01a.wav
const SFX_123 := 0x123; // feet01b.wav
const SFX_124 := 0x124; // feet03a.wav
const SFX_125 := 0x125; // feet03b.wav
const SFX_126 := 0x126; // feet05a.wav
const SFX_127 := 0x127; // feet05b.wav
const SFX_128 := 0x128; // feet08a.wav
const SFX_129 := 0x129; // feet08b.wav
const SFX_12A := 0x12A; // feet10a.wav
const SFX_12B := 0x12B; // feet10b.wav
const SFX_12C := 0x12C; // feet12a.wav
const SFX_12D := 0x12D; // feet12b.wav
const SFX_12E := 0x12E; // feet13a.wav
const SFX_12F := 0x12F; // feet13b.wav
const SFX_130 := 0x130; // feet14a.wav
const SFX_131 := 0x131; // feet14b.wav
const SFX_132 := 0x132; // feet15a.wav
const SFX_133 := 0x133; // feet15b.wav
const SFX_134 := 0x134; // feet15c.wav
const SFX_135 := 0x135; // feet15d.wav
const SFX_136 := 0x136; // hit01.wav
const SFX_137 := 0x137; // hit02.wav
const SFX_138 := 0x138; // hit03.wav
const SFX_139 := 0x139; // hit04.wav
const SFX_13A := 0x13A; // hit05.wav
const SFX_13B := 0x13B; // hit06.wav
const SFX_13C := 0x13C; // hit07.wav
const SFX_13D := 0x13D; // hit08.wav
const SFX_13E := 0x13E; // hit09.wav
const SFX_13F := 0x13F; // hit10.wav
const SFX_140 := 0x140; // hit11.wav
const SFX_141 := 0x141; // hit12.wav
const SFX_142 := 0x142; // hit13.wav
const SFX_143 := 0x143; // hit14.wav
const SFX_144 := 0x144; // hit15.wav
const SFX_145 := 0x145; // hit16.wav
const SFX_146 := 0x146; // hit17.wav
const SFX_147 := 0x147; // hit18.wav
const SFX_148 := 0x148; // hit19.wav
const SFX_149 := 0x149; // hit20.wav
const SFX_14A := 0x14A; // hit21.wav
const SFX_14B := 0x14B; // hit22.wav
const SFX_14C := 0x14C; // foomph01.wav
const SFX_14D := 0x14D; // foomph06.wav
const SFX_14E := 0x14E; // foomph07.wav
const SFX_14F := 0x14F; // foomph08.wav
const SFX_150 := 0x150; // foomph09.wav
const SFX_151 := 0x151; // f_die_01.wav
const SFX_152 := 0x152; // f_die_02.wav
const SFX_153 := 0x153; // f_die_03.wav
const SFX_154 := 0x154; // f_die_04.wav
const SFX_155 := 0x155; // moomph01.wav
const SFX_156 := 0x156; // moomph02.wav
const SFX_157 := 0x157; // moomph03.wav
const SFX_158 := 0x158; // moomph04.wav
const SFX_159 := 0x159; // moomph05.wav
const SFX_15A := 0x15A; // moomph13.wav
const SFX_15B := 0x15B; // m_die_01.wav
const SFX_15C := 0x15C; // m_die_02.wav
const SFX_15D := 0x15D; // m_die_03.wav
const SFX_15E := 0x15E; // m_die_04.wav
const SFX_15F := 0x15F; // fyrbal01.wav
const SFX_160 := 0x160; // fyrbal02.wav
const SFX_161 := 0x161; // fyrbal03.wav
const SFX_162 := 0x162; // corpsr1.wav
const SFX_163 := 0x163; // corpsr3.wav
const SFX_164 := 0x164; // corpsr4.wav
const SFX_165 := 0x165; // corpsr5.wav
const SFX_166 := 0x166; // daemon1.wav
const SFX_167 := 0x167; // daemon2.wav
const SFX_168 := 0x168; // daemon3.wav
const SFX_169 := 0x169; // daemon4.wav
const SFX_16A := 0x16A; // daemon5.wav
const SFX_16B := 0x16B; // dragon1.wav
const SFX_16C := 0x16C; // dragon2.wav
const SFX_16D := 0x16D; // dragon3.wav
const SFX_16E := 0x16E; // dragon4.wav
const SFX_16F := 0x16F; // dragon5.wav
const SFX_170 := 0x170; // ettin1.wav
const SFX_171 := 0x171; // ettin2.wav
const SFX_172 := 0x172; // ettin3.wav
const SFX_173 := 0x173; // ettin4.wav
const SFX_174 := 0x174; // ettin5.wav
const SFX_175 := 0x175; // gargyl1.wav
const SFX_176 := 0x176; // gargyl2.wav
const SFX_177 := 0x177; // gargyl3.wav
const SFX_178 := 0x178; // gargyl4.wav
const SFX_179 := 0x179; // gargyl5.wav
const SFX_17A := 0x17A; // gazer1.wav
const SFX_17B := 0x17B; // gazer2.wav
const SFX_17C := 0x17C; // gazer3.wav
const SFX_17D := 0x17D; // gazer4.wav
const SFX_17E := 0x17E; // gazer5.wav
const SFX_17F := 0x17F; // ghost1.wav
const SFX_180 := 0x180; // ghost2.wav
const SFX_181 := 0x181; // ghost3.wav
const SFX_182 := 0x182; // ghost4.wav
const SFX_183 := 0x183; // ghost5.wav
const SFX_184 := 0x184; // gispidr1.wav
const SFX_185 := 0x185; // gispidr2.wav
const SFX_186 := 0x186; // gispidr3.wav
const SFX_187 := 0x187; // gispidr4.wav
const SFX_188 := 0x188; // gispidr5.wav
const SFX_189 := 0x189; // grat1.wav
const SFX_18A := 0x18A; // grat2.wav
const SFX_18B := 0x18B; // grat3.wav
const SFX_18C := 0x18C; // grat4.wav
const SFX_18D := 0x18D; // grat5.wav
const SFX_18E := 0x18E; // gscorp1.wav
const SFX_18F := 0x18F; // gscorp2.wav
const SFX_190 := 0x190; // gscorp3.wav
const SFX_191 := 0x191; // gscorp4.wav
const SFX_192 := 0x192; // gscorp5.wav
const SFX_193 := 0x193; // harpy1.wav
const SFX_194 := 0x194; // harpy2.wav
const SFX_195 := 0x195; // harpy3.wav
const SFX_196 := 0x196; // harpy4.wav
const SFX_197 := 0x197; // harpy5.wav
const SFX_198 := 0x198; // headls1.wav
const SFX_199 := 0x199; // headls2.wav
const SFX_19A := 0x19A; // headls3.wav
const SFX_19B := 0x19B; // headls4.wav
const SFX_19C := 0x19C; // headls5.wav
const SFX_19D := 0x19D; // liche1.wav
const SFX_19E := 0x19E; // liche2.wav
const SFX_19F := 0x19F; // liche3.wav
const SFX_1A0 := 0x1A0; // liche4.wav
const SFX_1A1 := 0x1A1; // liche5.wav
const SFX_1A2 := 0x1A2; // lzrdmn1.wav
const SFX_1A3 := 0x1A3; // lzrdmn2.wav
const SFX_1A4 := 0x1A4; // lzrdmn3.wav
const SFX_1A5 := 0x1A5; // lzrdmn4.wav
const SFX_1A6 := 0x1A6; // lzrdmn5.wav
const SFX_1A7 := 0x1A7; // mngbat1.wav
const SFX_1A8 := 0x1A8; // mngbat2.wav
const SFX_1A9 := 0x1A9; // mngbat3.wav
const SFX_1AA := 0x1AA; // mngbat4.wav
const SFX_1AB := 0x1AB; // mngbat5.wav
const SFX_1AC := 0x1AC; // ogre1.wav
const SFX_1AD := 0x1AD; // ogre2.wav
const SFX_1AE := 0x1AE; // ogre3.wav
const SFX_1AF := 0x1AF; // ogre4.wav
const SFX_1B0 := 0x1B0; // ogre5.wav
const SFX_1B1 := 0x1B1; // orc1.wav
const SFX_1B2 := 0x1B2; // orc2.wav
const SFX_1B3 := 0x1B3; // orc3.wav
const SFX_1B4 := 0x1B4; // orc4.wav
const SFX_1B5 := 0x1B5; // orc5.wav
const SFX_1B6 := 0x1B6; // ratman1.wav
const SFX_1B7 := 0x1B7; // ratman2.wav
const SFX_1B8 := 0x1B8; // ratman3.wav
const SFX_1B9 := 0x1B9; // ratman4.wav
const SFX_1BA := 0x1BA; // ratman5.wav
const SFX_1BB := 0x1BB; // reaper1.wav
const SFX_1BC := 0x1BC; // reaper2.wav
const SFX_1BD := 0x1BD; // reaper3.wav
const SFX_1BE := 0x1BE; // reaper4.wav
const SFX_1BF := 0x1BF; // reaper5.wav
const SFX_1C0 := 0x1C0; // seaserp1.wav
const SFX_1C1 := 0x1C1; // seaserp3.wav
const SFX_1C2 := 0x1C2; // seaserp4.wav
const SFX_1C3 := 0x1C3; // seaserp5.wav
const SFX_1C4 := 0x1C4; // skletn1.wav
const SFX_1C5 := 0x1C5; // skletn2.wav
const SFX_1C6 := 0x1C6; // skletn3.wav
const SFX_1C7 := 0x1C7; // skletn4.wav
const SFX_1C8 := 0x1C8; // skletn5.wav
const SFX_1C9 := 0x1C9; // slime1.wav
const SFX_1CA := 0x1CA; // slime2.wav
const SFX_1CB := 0x1CB; // slime3.wav
const SFX_1CC := 0x1CC; // slime4.wav
const SFX_1CD := 0x1CD; // slime5.wav
const SFX_1CE := 0x1CE; // troll1.wav
const SFX_1CF := 0x1CF; // troll2.wav
const SFX_1D0 := 0x1D0; // troll3.wav
const SFX_1D1 := 0x1D1; // troll4.wav
const SFX_1D2 := 0x1D2; // troll5.wav
const SFX_1D3 := 0x1D3; // wisp1.wav
const SFX_1D4 := 0x1D4; // wisp2.wav
const SFX_1D5 := 0x1D5; // wisp3.wav
const SFX_1D6 := 0x1D6; // wisp4.wav
const SFX_1D7 := 0x1D7; // wisp5.wav
const SFX_1D8 := 0x1D8; // zombie1.wav
const SFX_1D9 := 0x1D9; // zombie2.wav
const SFX_1DA := 0x1DA; // zombie3.wav
const SFX_1DB := 0x1DB; // zombie4.wav
const SFX_1DC := 0x1DC; // zombie5.wav
const SFX_1DD := 0x1DD; // elec_l.wav
const SFX_1DE := 0x1DE; // fire_l.wav
const SFX_1DF := 0x1DF; // hiss_l.wav
const SFX_1E0 := 0x1E0; // clumsy.wav
const SFX_1E1 := 0x1E1; // cure.wav
const SFX_1E2 := 0x1E2; // curse.wav
const SFX_1E3 := 0x1E3; // c_food.wav
const SFX_1E4 := 0x1E4; // c_light.wav
const SFX_1E5 := 0x1E5; // feblmind.wav
const SFX_1E6 := 0x1E6; // m_arrow.wav
const SFX_1E7 := 0x1E7; // weaken.wav
const SFX_1E8 := 0x1E8; // agility.wav
const SFX_1E9 := 0x1E9; // archcure.wav
const SFX_1EA := 0x1EA; // birdseye.wav
const SFX_1EB := 0x1EB; // bless.wav
const SFX_1EC := 0x1EC; // cunning.wav
const SFX_1ED := 0x1ED; // identify.wav
const SFX_1EE := 0x1EE; // protect.wav
const SFX_1EF := 0x1EF; // strength.wav
const SFX_1F0 := 0x1F0; // trap.wav
const SFX_1F1 := 0x1F1; // untrap.wav
const SFX_1F2 := 0x1F2; // harm.wav
const SFX_1F3 := 0x1F3; // heal.wav
const SFX_1F4 := 0x1F4; // magic_fyrbal01.wav
const SFX_1F5 := 0x1F5; // m_lock.wav
const SFX_1F6 := 0x1F6; // teleknes.wav
const SFX_1F7 := 0x1F7; // walstone.wav
const SFX_1F8 := 0x1F8; // aprotect.wav
const SFX_1F9 := 0x1F9; // manadran.wav
const SFX_1FA := 0x1FA; // manavamp.wav
const SFX_1FB := 0x1FB; // mark.wav
const SFX_1FC := 0x1FC; // masscurs.wav
const SFX_1FD := 0x1FD; // recall.wav
const SFX_1FE := 0x1FE; // reveal.wav
const SFX_1FF := 0x1FF; // teleport.wav
const SFX_200 := 0x200; // unlock.wav
const SFX_201 := 0x201; // bladsprt.wav
const SFX_202 := 0x202; // dispfild.wav
const SFX_203 := 0x203; // gratheal.wav
const SFX_204 := 0x204; // invisibl.wav
const SFX_205 := 0x205; // paralyze.wav
const SFX_206 := 0x206; // poison.wav
const SFX_207 := 0x207; // thundr01.wav
const SFX_208 := 0x208; // explode.wav
const SFX_209 := 0x209; // flamstrk.wav
const SFX_20A := 0x20A; // massdisp.wav
const SFX_20B := 0x20B; // nrgybolt.wav
const SFX_20C := 0x20C; // parafild.wav
const SFX_20D := 0x20D; // posnfild.wav
const SFX_20E := 0x20E; // earthqke.wav
const SFX_20F := 0x20F; // gatetrvl.wav
const SFX_210 := 0x210; // morph.wav
const SFX_211 := 0x211; // nergyfld.wav
const SFX_212 := 0x212; // nrgywind.wav
const SFX_213 := 0x213; // dethvrtx.wav
const SFX_214 := 0x214; // mindblst.wav
const SFX_215 := 0x215; // resurect.wav
const SFX_216 := 0x216; // sumncrtr.wav
const SFX_217 := 0x217; // sumndemn.wav
const SFX_218 := 0x218; // sumnelem.wav
const SFX_219 := 0x219; // blade01.wav
const SFX_21A := 0x21A; // blade02.wav
const SFX_21B := 0x21B; // blade03.wav
const SFX_21C := 0x21C; // blade04.wav
const SFX_21D := 0x21D; // blade05.wav
const SFX_21E := 0x21E; // boulder1.wav
const SFX_21F := 0x21F; // boulder2.wav
const SFX_220 := 0x220; // crumble1.wav
const SFX_221 := 0x221; // crumble2.wav
const SFX_222 := 0x222; // crumble3.wav
const SFX_223 := 0x223; // crumble4.wav
const SFX_224 := 0x224; // dart01.wav
const SFX_225 := 0x225; // dart02.wav
const SFX_226 := 0x226; // flame01.wav
const SFX_227 := 0x227; // flame03.wav
const SFX_228 := 0x228; // flame05.wav
const SFX_229 := 0x229; // mushrm01.wav
const SFX_22A := 0x22A; // mushrm02.wav
const SFX_22B := 0x22B; // mushrm03.wav
const SFX_22C := 0x22C; // spike01.wav
const SFX_22D := 0x22D; // spike02.wav
const SFX_22E := 0x22E; // spike03.wav
const SFX_22F := 0x22F; // spike04.wav
const SFX_230 := 0x230; // vapor01.wav
const SFX_231 := 0x231; // vapor02.wav
const SFX_232 := 0x232; // vapor03.wav
const SFX_233 := 0x233; // axe01.wav
const SFX_234 := 0x234; // blunt01.wav
const SFX_235 := 0x235; // crossbow.wav
const SFX_236 := 0x236; // gilotin1.wav
const SFX_237 := 0x237; // hvyswrd1.wav
const SFX_238 := 0x238; // hvyswrd4.wav
const SFX_239 := 0x239; // swish01.wav
const SFX_23A := 0x23A; // swish02.wav
const SFX_23B := 0x23B; // swish03.wav
const SFX_23C := 0x23C; // sword1.wav
const SFX_23D := 0x23D; // sword7.wav
const SFX_23E := 0x23E; // carpntry.wav
const SFX_23F := 0x23F; // dye.wav
const SFX_240 := 0x240; // fshsplsh.wav
const SFX_241 := 0x241; // liquid.wav
const SFX_242 := 0x242; // lockpick.wav
const SFX_243 := 0x243; // mortar.wav
const SFX_244 := 0x244; // mystery1.wav
const SFX_245 := 0x245; // mystery2.wav
const SFX_246 := 0x246; // mystery3.wav
const SFX_247 := 0x247; // poison.wav
const SFX_248 := 0x248; // powder.wav
const SFX_249 := 0x249; // scissors.wav
const SFX_24A := 0x24A; // scribe.wav
const SFX_24B := 0x24B; // spirtspk.wav
const SFX_24C := 0x24C; // z_horseft2.wav
const SFX_24D := 0x24D; // z_horseft1.wav
const SFX_24E := 0x24E; // SpidrW1.wav
const SFX_24F := 0x24F; // SpidrW2.wav
const SFX_250 := 0x250; // SpidrW3.wav
const SFX_251 := 0x251; // SpidrW4.wav
const SFX_252 := 0x252; // SpidrW5.wav
const SFX_253 := 0x253; // SpidrD1.wav
const SFX_254 := 0x254; // SpidrD2.wav
const SFX_255 := 0x255; // SpidrD3.wav
const SFX_256 := 0x256; // SpidrD4.wav
const SFX_257 := 0x257; // SpidrD5.wav
const SFX_258 := 0x258; // SpidrQ1.wav
const SFX_259 := 0x259; // SpidrQ2.wav
const SFX_25A := 0x25A; // SpidrQ3.wav
const SFX_25B := 0x25B; // SpidrQ4.wav
const SFX_25C := 0x25C; // SpidrQ5.wav
const SFX_25D := 0x25D; // Cyclp1_1.wav
const SFX_25E := 0x25E; // Cyclp1_2.wav
const SFX_25F := 0x25F; // Cyclp1_3.wav
const SFX_260 := 0x260; // Cyclp1_4.wav
const SFX_261 := 0x261; // Cyclp1_5.wav
const SFX_262 := 0x262; // Cyclp2_1.wav
const SFX_263 := 0x263; // Cyclp2_2.wav
const SFX_264 := 0x264; // Cyclp2_3.wav
const SFX_265 := 0x265; // Cyclp2_4.wav
const SFX_266 := 0x266; // Cyclp2_5.wav
const SFX_267 := 0x267; // Frog1_1.wav
const SFX_268 := 0x268; // Frog1_2.wav
const SFX_269 := 0x269; // Frog1_3.wav
const SFX_26A := 0x26A; // Frog1_4.wav
const SFX_26B := 0x26B; // Frog1_5.wav
const SFX_26C := 0x26C; // Frog2_1.wav
const SFX_26D := 0x26D; // Frog2_2.wav
const SFX_26E := 0x26E; // Frog2_3.wav
const SFX_26F := 0x26F; // Frog2_4.wav
const SFX_270 := 0x270; // Frog2_5.wav
const SFX_271 := 0x271; // Bird1_1.wav
const SFX_272 := 0x272; // Bird1_2.wav
const SFX_273 := 0x273; // Bird1_3.wav
const SFX_274 := 0x274; // Bird1_4.wav
const SFX_275 := 0x275; // Bird1_5.wav
const SFX_276 := 0x276; // Bird2_1.wav
const SFX_277 := 0x277; // Bird2_2.wav
const SFX_278 := 0x278; // Bird2_3.wav
const SFX_279 := 0x279; // Bird2_4.wav
const SFX_27A := 0x27A; // Bird2_5.wav
const SFX_27B := 0x27B; // SnakeW1.wav
const SFX_27C := 0x27C; // SnakeW2.wav
const SFX_27D := 0x27D; // SnakeW3.wav
const SFX_27E := 0x27E; // SnakeW4.wav
const SFX_27F := 0x27F; // SnakeW5.wav
const SFX_280 := 0x280; // SnakeM1.wav
const SFX_281 := 0x281; // SnakeM2.wav
const SFX_282 := 0x282; // SnakeM3.wav
const SFX_283 := 0x283; // SnakeM4.wav
const SFX_284 := 0x284; // SnakeM5.wav
const SFX_285 := 0x285; // SnakeQ1.wav
const SFX_286 := 0x286; // SnakeQ2.wav
const SFX_287 := 0x287; // SnakeQ3.wav
const SFX_288 := 0x288; // SnakeQ4.wav
const SFX_289 := 0x289; // SnakeQ5.wav
const SFX_28A := 0x28A; // Lizard1.wav
const SFX_28B := 0x28B; // Lizard2.wav
const SFX_28C := 0x28C; // Lizard3.wav
const SFX_28D := 0x28D; // Lizard4.wav
const SFX_28E := 0x28E; // Lizard5.wav
const SFX_28F := 0x28F; // agility.wav
const SFX_290 := 0x290; // air01.wav
const SFX_291 := 0x291; // air02.wav
const SFX_292 := 0x292; // air03.wav
const SFX_293 := 0x293; // air04.wav
const SFX_294 := 0x294; // air05.wav
const SFX_295 := 0x295; // aligatr1.wav
const SFX_296 := 0x296; // aligatr2.wav
const SFX_297 := 0x297; // aligatr3.wav
const SFX_298 := 0x298; // aligatr4.wav
const SFX_299 := 0x299; // aligatr5.wav
const SFX_29A := 0x29A; // archcure.wav
const SFX_29B := 0x29B; // bear1.wav
const SFX_29C := 0x29C; // bird1_5.wav
const SFX_29D := 0x29D; // bird2_1.wav
const SFX_29E := 0x29E; // bird2_2.wav
const SFX_29F := 0x29F; // bird2_3.wav
const SFX_2A0 := 0x2A0; // bird2_4.wav
const SFX_2A1 := 0x2A1; // bird2_5.wav
const SFX_2A2 := 0x2A2; // bladesprt.wav
const SFX_2A3 := 0x2A3; // cat1.wav
const SFX_2A4 := 0x2A4; // cat2.wav
const SFX_2A5 := 0x2A5; // cat3.wav
const SFX_2A6 := 0x2A6; // cat4.wav
const SFX_2A7 := 0x2A7; // cat5.wav
const SFX_2A8 := 0x2A8; // centaur1.wav
const SFX_2A9 := 0x2A9; // centaur2.wav
const SFX_2AA := 0x2AA; // centaur3.wav
const SFX_2AB := 0x2AB; // centaur4.wav
const SFX_2AC := 0x2AC; // centaur5.wav
const SFX_2AD := 0x2AD; // corpsr1.wav
const SFX_2AE := 0x2AE; // corpsr2.wav
const SFX_2AF := 0x2AF; // corpsr3.wav
const SFX_2B0 := 0x2B0; // corpsr4.wav
const SFX_2B1 := 0x2B1; // corpsr5.wav
const SFX_2B2 := 0x2B2; // crossbow.wav
const SFX_2B3 := 0x2B3; // crossbow_alt.wav
const SFX_2B4 := 0x2B4; // d_jung01.wav
const SFX_2B5 := 0x2B5; // d_jung02.wav
const SFX_2B6 := 0x2B6; // d_jung03.wav
const SFX_2B7 := 0x2B7; // d_swmp01.wav
const SFX_2B8 := 0x2B8; // d_swmp02.wav
const SFX_2B9 := 0x2B9; // daemon1.wav
const SFX_2BA := 0x2BA; // daemon2.wav
const SFX_2BB := 0x2BB; // daemon3.wav
const SFX_2BC := 0x2BC; // daemon4.wav
const SFX_2BD := 0x2BD; // daemon5.wav
const SFX_2BE := 0x2BE; // deer3.wav
const SFX_2BF := 0x2BF; // deer4.wav
const SFX_2C0 := 0x2C0; // deer5.wav
const SFX_2C1 := 0x2C1; // dragon_asian_atk.wav
const SFX_2C2 := 0x2C2; // dragon_asian_die1.wav
const SFX_2C3 := 0x2C3; // dragon_asian_figt.wav
const SFX_2C4 := 0x2C4; // dragon_asian_gethit.wav
const SFX_2C5 := 0x2C5; // dragon_asian_idle.wav
const SFX_2C6 := 0x2C6; // dragon_asian_run.wav
const SFX_2C7 := 0x2C7; // dragon_asian_walk.wav
const SFX_2C8 := 0x2C8; // dragon_red_atk1.wav
const SFX_2C9 := 0x2C9; // dragon_red_atk2.wav
const SFX_2CA := 0x2CA; // dragon_red_atk3.wav
const SFX_2CB := 0x2CB; // dragon_red_atk3_a.wav
const SFX_2CC := 0x2CC; // dragon_red_atk3_b.wav
const SFX_2CD := 0x2CD; // dragon_red_die1.wav
const SFX_2CE := 0x2CE; // dragon_red_dieair.wav
const SFX_2CF := 0x2CF; // dragon_red_fgt.wav
const SFX_2D0 := 0x2D0; // dragon_red_figt.wav
const SFX_2D1 := 0x2D1; // dragon_red_fly.wav
const SFX_2D2 := 0x2D2; // dragon_red_gethit.wav
const SFX_2D3 := 0x2D3; // dragon_red_idle.wav
const SFX_2D4 := 0x2D4; // dragon_red_run.wav
const SFX_2D5 := 0x2D5; // dragon_red_standtof.wav
const SFX_2D6 := 0x2D6; // dragon_red_walk.wav
const SFX_2D7 := 0x2D7; // drink5.wav
const SFX_2D8 := 0x2D8; // drip01.wav
const SFX_2D9 := 0x2D9; // drip02.wav
const SFX_2DA := 0x2DA; // drip03.wav
const SFX_2DB := 0x2DB; // drip03_1.wav
const SFX_2DC := 0x2DC; // drip04.wav
const SFX_2DD := 0x2DD; // drip05.wav
const SFX_2DE := 0x2DE; // drip06.wav
const SFX_2DF := 0x2DF; // drip07.wav
const SFX_2E0 := 0x2E0; // drip08.wav
const SFX_2E1 := 0x2E1; // drip09.wav
const SFX_2E2 := 0x2E2; // dropgem1.wav
const SFX_2E3 := 0x2E3; // dropgem2.wav
const SFX_2E4 := 0x2E4; // dropgem3.wav
const SFX_2E5 := 0x2E5; // dropmny1.wav
const SFX_2E6 := 0x2E6; // dropmny2.wav
const SFX_2E7 := 0x2E7; // dropmny3.wav
const SFX_2E8 := 0x2E8; // drum_battle.wav
const SFX_2E9 := 0x2E9; // drum_drama.wav
const SFX_2EA := 0x2EA; // drum_peace.wav
const SFX_2EB := 0x2EB; // drum01.wav
const SFX_2EC := 0x2EC; // drum01_b.wav
const SFX_2ED := 0x2ED; // drum02.wav
const SFX_2EE := 0x2EE; // drum03.wav
const SFX_2EF := 0x2EF; // eagle1.wav
const SFX_2F0 := 0x2F0; // eagle2.wav
const SFX_2F1 := 0x2F1; // eagle3.wav
const SFX_2F2 := 0x2F2; // eagle4.wav
const SFX_2F3 := 0x2F3; // eagle5.wav
const SFX_2F4 := 0x2F4; // earthquake.wav
const SFX_2F5 := 0x2F5; // elec_1.wav
const SFX_2F6 := 0x2F6; // ethwar_atk1.wav
const SFX_2F7 := 0x2F7; // ethwar_atk2.wav
const SFX_2F8 := 0x2F8; // ethwar_die.wav
const SFX_2F9 := 0x2F9; // ethwar_figt.wav
const SFX_2FA := 0x2FA; // ethwar_gethit.wav
const SFX_2FB := 0x2FB; // ethwar_walk.wav
const SFX_2FC := 0x2FC; // ettin1.wav
const SFX_2FD := 0x2FD; // ettin2.wav
const SFX_2FE := 0x2FE; // ettin3.wav
const SFX_2FF := 0x2FF; // ettin4.wav
const SFX_300 := 0x300; // ettin5.wav
const SFX_301 := 0x301; // exodus1.wav
const SFX_302 := 0x302; // exodus2.wav
const SFX_303 := 0x303; // exodus3.wav
const SFX_304 := 0x304; // exodus4.wav
const SFX_305 := 0x305; // exodus5.wav
const SFX_306 := 0x306; // exp01.wav
const SFX_307 := 0x307; // exp02.wav
const SFX_308 := 0x308; // exp03.wav
const SFX_309 := 0x309; // exp04.wav
const SFX_30A := 0x30A; // exp05.wav
const SFX_30B := 0x30B; // f_ah.wav
const SFX_30C := 0x30C; // f_ahha.wav
const SFX_30D := 0x30D; // f_applaud.wav
const SFX_30E := 0x30E; // f_blownose.wav
const SFX_30F := 0x30F; // f_burp.wav
const SFX_310 := 0x310; // f_cheer.wav
const SFX_311 := 0x311; // f_clear_throat.wav
const SFX_312 := 0x312; // f_cough.wav
const SFX_313 := 0x313; // f_coughbs.wav
const SFX_314 := 0x314; // f_cry.wav
const SFX_315 := 0x315; // f_death_01.wav
const SFX_316 := 0x316; // f_death_02.wav
const SFX_317 := 0x317; // f_death_03.wav
const SFX_318 := 0x318; // f_death_04.wav
const SFX_319 := 0x319; // f_fart.wav
const SFX_31A := 0x31A; // f_gasp.wav
const SFX_31B := 0x31B; // f_giggle.wav
const SFX_31C := 0x31C; // f_groan.wav
const SFX_31D := 0x31D; // f_growl.wav
const SFX_31E := 0x31E; // f_hey.wav
const SFX_31F := 0x31F; // f_hiccup.wav
const SFX_320 := 0x320; // f_huh.wav
const SFX_321 := 0x321; // f_kiss.wav
const SFX_322 := 0x322; // f_laugh.wav
const SFX_323 := 0x323; // f_no.wav
const SFX_324 := 0x324; // f_oh.wav
const SFX_325 := 0x325; // f_oomph_01.wav
const SFX_326 := 0x326; // f_oomph_02.wav
const SFX_327 := 0x327; // f_oomph_03.wav
const SFX_328 := 0x328; // f_oomph_04.wav
const SFX_329 := 0x329; // f_oomph_05.wav
const SFX_32A := 0x32A; // f_oomph_06.wav
const SFX_32B := 0x32B; // f_oomph_07.wav
const SFX_32C := 0x32C; // f_oooh.wav
const SFX_32D := 0x32D; // f_oops.wav
const SFX_32E := 0x32E; // f_puke.wav
const SFX_32F := 0x32F; // f_scream.wav
const SFX_330 := 0x330; // f_shush.wav
const SFX_331 := 0x331; // f_sigh.wav
const SFX_332 := 0x332; // f_sneeze.wav
const SFX_333 := 0x333; // f_sniff.wav
const SFX_334 := 0x334; // f_snore.wav
const SFX_335 := 0x335; // f_spit.wav
const SFX_336 := 0x336; // f_whistle.wav
const SFX_337 := 0x337; // f_yawn.wav
const SFX_338 := 0x338; // f_yea.wav
const SFX_339 := 0x339; // f_yell.wav
const SFX_33A := 0x33A; // feet_grss_a.wav
const SFX_33B := 0x33B; // feet_grss_b.wav
const SFX_33C := 0x33C; // feet_grvl_a.wav
const SFX_33D := 0x33D; // feet_grvl_b.wav
const SFX_33E := 0x33E; // feet_pvmt_a.wav
const SFX_33F := 0x33F; // feet_pvmt_b.wav
const SFX_340 := 0x340; // feet_sand_a.wav
const SFX_341 := 0x341; // feet_sand_b.wav
const SFX_342 := 0x342; // feet_snow_a.wav
const SFX_343 := 0x343; // feet_snow_b.wav
const SFX_344 := 0x344; // feet_wood_a.wav
const SFX_345 := 0x345; // feet_wood_b.wav
const SFX_346 := 0x346; // fire_1.wav
const SFX_347 := 0x347; // fire01.wav
const SFX_348 := 0x348; // fire02.wav
const SFX_349 := 0x349; // fire03.wav
const SFX_34A := 0x34A; // fire04.wav
const SFX_34B := 0x34B; // fire05.wav
const SFX_34C := 0x34C; // flailcor_die1.wav
const SFX_34D := 0x34D; // flailcor_figt1.wav
const SFX_34E := 0x34E; // flailcor_figt2.wav
const SFX_34F := 0x34F; // flailcor_idle.wav
const SFX_350 := 0x350; // flailcor_run.wav
const SFX_351 := 0x351; // flailcor_walk.wav
const SFX_352 := 0x352; // flailkrak_atk1.wav
const SFX_353 := 0x353; // flailkrak_die1.wav
const SFX_354 := 0x354; // flailkrak_figt.wav
const SFX_355 := 0x355; // flailkrak_gethit.wav
const SFX_356 := 0x356; // flailkrak_walk.wav
const SFX_357 := 0x357; // flame01.wav
const SFX_358 := 0x358; // flame03.wav
const SFX_359 := 0x359; // flame03.wav
const SFX_35A := 0x35A; // flame05.wav
const SFX_35B := 0x35B; // frog1_1.wav
const SFX_35C := 0x35C; // frog1_2.wav
const SFX_35D := 0x35D; // frog1_3.wav
const SFX_35E := 0x35E; // frog1_4.wav
const SFX_35F := 0x35F; // frog1_5.wav
const SFX_360 := 0x360; // frog2_1.wav
const SFX_361 := 0x361; // frog2_2.wav
const SFX_362 := 0x362; // frog2_3.wav
const SFX_363 := 0x363; // frog2_4.wav
const SFX_364 := 0x364; // frog2_5.wav
const SFX_365 := 0x365; // fshplsh.wav
const SFX_366 := 0x366; // gallop_grss.wav
const SFX_367 := 0x367; // gallop_grvl.wav
const SFX_368 := 0x368; // gallop_pvmt.wav
const SFX_369 := 0x369; // gallop_sand.wav
const SFX_36A := 0x36A; // gallop_snow.wav
const SFX_36B := 0x36B; // gallop_wood.wav
const SFX_36C := 0x36C; // gazer1.wav
const SFX_36D := 0x36D; // gazer2.wav
const SFX_36E := 0x36E; // gazer3.wav
const SFX_36F := 0x36F; // gazer4.wav
const SFX_370 := 0x370; // gazer5.wav
const SFX_371 := 0x371; // genie1.wav
const SFX_372 := 0x372; // genie1_a.wav
const SFX_373 := 0x373; // genie1_b.wav
const SFX_374 := 0x374; // genie2.wav
const SFX_375 := 0x375; // genie3.wav
const SFX_376 := 0x376; // genie4.wav
const SFX_377 := 0x377; // genie5.wav
const SFX_378 := 0x378; // genie5_a.wav
const SFX_379 := 0x379; // genie5_b.wav
const SFX_37A := 0x37A; // ghost1.wav
const SFX_37B := 0x37B; // ghost2.wav
const SFX_37C := 0x37C; // ghost2_1.wav
const SFX_37D := 0x37D; // ghost2_2.wav
const SFX_37E := 0x37E; // ghost2_3.wav
const SFX_37F := 0x37F; // ghost2_4.wav
const SFX_380 := 0x380; // ghost3.wav
const SFX_381 := 0x381; // ghost4.wav
const SFX_382 := 0x382; // ghost5.wav
const SFX_383 := 0x383; // giant_snail1.wav
const SFX_384 := 0x384; // giant_snail2.wav
const SFX_385 := 0x385; // giant_snail3.wav
const SFX_386 := 0x386; // giant_snail4.wav
const SFX_387 := 0x387; // giant_snail5.wav
const SFX_388 := 0x388; // gilotin1.wav
const SFX_389 := 0x389; // gispidr1.wav
const SFX_38A := 0x38A; // gispidr2.wav
const SFX_38B := 0x38B; // gispidr3.wav
const SFX_38C := 0x38C; // gispidr4.wav
const SFX_38D := 0x38D; // gispidr5.wav
const SFX_38E := 0x38E; // glasbrk1.wav
const SFX_38F := 0x38F; // glasbrk2.wav
const SFX_390 := 0x390; // glasbrk3.wav
const SFX_391 := 0x391; // glasbrk4.wav
const SFX_392 := 0x392; // harp_battle.wav
const SFX_393 := 0x393; // harp_drama.wav
const SFX_394 := 0x394; // harp_peace.wav
const SFX_395 := 0x395; // harpie_atk1.wav
const SFX_396 := 0x396; // harpie_die1.wav
const SFX_397 := 0x397; // harpie_figt.wav
const SFX_398 := 0x398; // harpie_gethit.wav
const SFX_399 := 0x399; // harpie_walk.wav
const SFX_39A := 0x39A; // harpy_die.wav
const SFX_39B := 0x39B; // harpy_fidget.wav
const SFX_39C := 0x39C; // harpy_gethit.wav
const SFX_39D := 0x39D; // harpy_walk.wav
const SFX_39E := 0x39E; // headls1.wav
const SFX_39F := 0x39F; // headls2.wav
const SFX_3A0 := 0x3A0; // headls3.wav
const SFX_3A1 := 0x3A1; // headls4.wav
const SFX_3A2 := 0x3A2; // headls5.wav
const SFX_3A3 := 0x3A3; // hit1.wav
const SFX_3A4 := 0x3A4; // hit10.wav
const SFX_3A5 := 0x3A5; // hit11.wav
const SFX_3A6 := 0x3A6; // hit12.wav
const SFX_3A7 := 0x3A7; // hit13.wav
const SFX_3A8 := 0x3A8; // hit14.wav
const SFX_3A9 := 0x3A9; // hit15.wav
const SFX_3AA := 0x3AA; // hit16.wav
const SFX_3AB := 0x3AB; // hit17.wav
const SFX_3AC := 0x3AC; // hit18.wav
const SFX_3AD := 0x3AD; // hit19.wav
const SFX_3AE := 0x3AE; // hit2.wav
const SFX_3AF := 0x3AF; // hit20.wav
const SFX_3B0 := 0x3B0; // hit21.wav
const SFX_3B1 := 0x3B1; // hit22.wav
const SFX_3B2 := 0x3B2; // hit3.wav
const SFX_3B3 := 0x3B3; // hit4.wav
const SFX_3B4 := 0x3B4; // hit5.wav
const SFX_3B5 := 0x3B5; // hit6.wav
const SFX_3B6 := 0x3B6; // hit7.wav
const SFX_3B7 := 0x3B7; // hit8.wav
const SFX_3B8 := 0x3B8; // hit9.wav
const SFX_3B9 := 0x3B9; // hvyswrd1.wav
const SFX_3BA := 0x3BA; // hvyswrd2.wav
const SFX_3BB := 0x3BB; // hvyswrd3.wav
const SFX_3BC := 0x3BC; // hvyswrd4.wav
const SFX_3BD := 0x3BD; // hvyswrd5.wav
const SFX_3BE := 0x3BE; // identify.wav
const SFX_3BF := 0x3BF; // igntorch.wav
const SFX_3C0 := 0x3C0; // insect_race1.wav
const SFX_3C1 := 0x3C1; // insect_race2.wav
const SFX_3C2 := 0x3C2; // insect_race3.wav
const SFX_3C3 := 0x3C3; // insect_race4.wav
const SFX_3C4 := 0x3C4; // insect_race5.wav
const SFX_3C5 := 0x3C5; // invisibl.wav
const SFX_3C6 := 0x3C6; // kirin1.wav
const SFX_3C7 := 0x3C7; // kirin2.wav
const SFX_3C8 := 0x3C8; // kirin3.wav
const SFX_3C9 := 0x3C9; // kirin4.wav
const SFX_3CA := 0x3CA; // kirin5.wav
const SFX_3CB := 0x3CB; // lap_harp_a_1.wav
const SFX_3CC := 0x3CC; // lap_harp_a_2.wav
const SFX_3CD := 0x3CD; // lap_harp_as_1.wav
const SFX_3CE := 0x3CE; // lap_harp_as_2.wav
const SFX_3CF := 0x3CF; // lap_harp_b_1.wav
const SFX_3D0 := 0x3D0; // lap_harp_b_2.wav
const SFX_3D1 := 0x3D1; // lap_harp_c_1.wav
const SFX_3D2 := 0x3D2; // lap_harp_c_2.wav
const SFX_3D3 := 0x3D3; // lap_harp_c_3.wav
const SFX_3D4 := 0x3D4; // lap_harp_cs_1.wav
const SFX_3D5 := 0x3D5; // lap_harp_cs_2.wav
const SFX_3D6 := 0x3D6; // lap_harp_d_1.wav
const SFX_3D7 := 0x3D7; // lap_harp_d_2.wav
const SFX_3D8 := 0x3D8; // lap_harp_ds_1.wav
const SFX_3D9 := 0x3D9; // lap_harp_ds_2.wav
const SFX_3DA := 0x3DA; // lap_harp_e_1.wav
const SFX_3DB := 0x3DB; // lap_harp_e_2.wav
const SFX_3DC := 0x3DC; // lap_harp_f_1.wav
const SFX_3DD := 0x3DD; // lap_harp_f_2.wav
const SFX_3DE := 0x3DE; // lap_harp_fs_1.wav
const SFX_3DF := 0x3DF; // lap_harp_fs_2.wav
const SFX_3E0 := 0x3E0; // lap_harp_g_1.wav
const SFX_3E1 := 0x3E1; // lap_harp_g_2.wav
const SFX_3E2 := 0x3E2; // lap_harp_gs_1.wav
const SFX_3E3 := 0x3E3; // lap_harp_gs_2.wav
const SFX_3E4 := 0x3E4; // leathr1.wav
const SFX_3E5 := 0x3E5; // lever01.wav
const SFX_3E6 := 0x3E6; // lever02.wav
const SFX_3E7 := 0x3E7; // lever03.wav
const SFX_3E8 := 0x3E8; // lever04.wav
const SFX_3E9 := 0x3E9; // lever05.wav
const SFX_3EA := 0x3EA; // liche1.wav
const SFX_3EB := 0x3EB; // liche2.wav
const SFX_3EC := 0x3EC; // liche3.wav
const SFX_3ED := 0x3ED; // liche4.wav
const SFX_3EE := 0x3EE; // liche5.wav
const SFX_3EF := 0x3EF; // lion1.wav
const SFX_3F0 := 0x3F0; // lion2.wav
const SFX_3F1 := 0x3F1; // lion3.wav
const SFX_3F2 := 0x3F2; // lion4.wav
const SFX_3F3 := 0x3F3; // lion5.wav
const SFX_3F4 := 0x3F4; // llama1.wav
const SFX_3F5 := 0x3F5; // llama2.wav
const SFX_3F6 := 0x3F6; // llama3.wav
const SFX_3F7 := 0x3F7; // llama4.wav
const SFX_3F8 := 0x3F8; // llama5.wav
const SFX_3F9 := 0x3F9; // longhorns1.wav
const SFX_3FA := 0x3FA; // longhorns2.wav
const SFX_3FB := 0x3FB; // longhorns3.wav
const SFX_3FC := 0x3FC; // longhorns4.wav
const SFX_3FD := 0x3FD; // longhorns5.wav
const SFX_3FE := 0x3FE; // lute_a_1.wav
const SFX_3FF := 0x3FF; // lute_a_2.wav
const SFX_400 := 0x400; // lute_as_1.wav
const SFX_401 := 0x401; // lute_as_2.wav
const SFX_402 := 0x402; // lute_b_1.wav
const SFX_403 := 0x403; // lute_b_2.wav
const SFX_404 := 0x404; // lute_battle.wav
const SFX_405 := 0x405; // lute_c_1.wav
const SFX_406 := 0x406; // lute_c_2.wav
const SFX_407 := 0x407; // lute_c_3.wav
const SFX_408 := 0x408; // lute_cs_1.wav
const SFX_409 := 0x409; // lute_cs_2.wav
const SFX_40A := 0x40A; // lute_d_1.wav
const SFX_40B := 0x40B; // lute_d_2.wav
const SFX_40C := 0x40C; // lute_drama.wav
const SFX_40D := 0x40D; // lute_ds_1.wav
const SFX_40E := 0x40E; // lute_ds_2.wav
const SFX_40F := 0x40F; // lute_e_1.wav
const SFX_410 := 0x410; // lute_e_2.wav
const SFX_411 := 0x411; // lute_f_1.wav
const SFX_412 := 0x412; // lute_f_2.wav
const SFX_413 := 0x413; // lute_fs_1.wav
const SFX_414 := 0x414; // lute_fs_2.wav
const SFX_415 := 0x415; // lute_g_1.wav
const SFX_416 := 0x416; // lute_g_2.wav
const SFX_417 := 0x417; // lute_gs_1.wav
const SFX_418 := 0x418; // lute_gs_2.wav
const SFX_419 := 0x419; // lute_peace.wav
const SFX_41A := 0x41A; // m_ah.wav
const SFX_41B := 0x41B; // m_ahha.wav
const SFX_41C := 0x41C; // m_applaud.wav
const SFX_41D := 0x41D; // m_blownose.wav
const SFX_41E := 0x41E; // m_burp.wav
const SFX_41F := 0x41F; // m_cheer.wav
const SFX_420 := 0x420; // m_clear_throat.wav
const SFX_421 := 0x421; // m_cough.wav
const SFX_422 := 0x422; // m_coughbs.wav
const SFX_423 := 0x423; // m_cry.wav
const SFX_424 := 0x424; // m_death_01.wav
const SFX_425 := 0x425; // m_death_02.wav
const SFX_426 := 0x426; // m_death_03.wav
const SFX_427 := 0x427; // m_death_04.wav
const SFX_428 := 0x428; // m_death_05.wav
const SFX_429 := 0x429; // m_fart.wav
const SFX_42A := 0x42A; // m_gasp.wav
const SFX_42B := 0x42B; // m_giggle.wav
const SFX_42C := 0x42C; // m_groan.wav
const SFX_42D := 0x42D; // m_growl.wav
const SFX_42E := 0x42E; // m_hey.wav
const SFX_42F := 0x42F; // m_hiccup.wav
const SFX_430 := 0x430; // m_huh.wav
const SFX_431 := 0x431; // m_kiss.wav
const SFX_432 := 0x432; // m_laugh.wav
const SFX_433 := 0x433; // m_no.wav
const SFX_434 := 0x434; // m_oh.wav
const SFX_435 := 0x435; // m_oomph_01.wav
const SFX_436 := 0x436; // m_oomph_02.wav
const SFX_437 := 0x437; // m_oomph_03.wav
const SFX_438 := 0x438; // m_oomph_04.wav
const SFX_439 := 0x439; // m_oomph_05.wav
const SFX_43A := 0x43A; // m_oomph_06.wav
const SFX_43B := 0x43B; // m_oomph_07.wav
const SFX_43C := 0x43C; // m_oomph_08.wav
const SFX_43D := 0x43D; // m_oomph_09.wav
const SFX_43E := 0x43E; // m_oooh.wav
const SFX_43F := 0x43F; // m_oops.wav
const SFX_440 := 0x440; // m_puke.wav
const SFX_441 := 0x441; // m_scream.wav
const SFX_442 := 0x442; // m_shush.wav
const SFX_443 := 0x443; // m_sigh.wav
const SFX_444 := 0x444; // m_sneeze.wav
const SFX_445 := 0x445; // m_sniff.wav
const SFX_446 := 0x446; // m_snore.wav
const SFX_447 := 0x447; // m_spit.wav
const SFX_448 := 0x448; // m_whistle.wav
const SFX_449 := 0x449; // m_yawn.wav
const SFX_44A := 0x44A; // m_yea.wav
const SFX_44B := 0x44B; // m_yell.wav
const SFX_44C := 0x44C; // magic_fyrbal01.wav
const SFX_44D := 0x44D; // mimic1.wav
const SFX_44E := 0x44E; // mimic2.wav
const SFX_44F := 0x44F; // mimic3.wav
const SFX_450 := 0x450; // mimic4.wav
const SFX_451 := 0x451; // mimic5.wav
const SFX_452 := 0x452; // mushroom_men1.wav
const SFX_453 := 0x453; // mushroom_men2.wav
const SFX_454 := 0x454; // mushroom_men3.wav
const SFX_455 := 0x455; // mushroom_men4.wav
const SFX_456 := 0x456; // mushroom_men5.wav
const SFX_457 := 0x457; // nergyfld.wav
const SFX_458 := 0x458; // nrgywind.wav
const SFX_459 := 0x459; // orc1.wav
const SFX_45A := 0x45A; // orc2.wav
const SFX_45B := 0x45B; // orc2_1.wav
const SFX_45C := 0x45C; // orc2_2.wav
const SFX_45D := 0x45D; // orc2_3.wav
const SFX_45E := 0x45E; // orc2_4.wav
const SFX_45F := 0x45F; // orc2_5.wav
const SFX_460 := 0x460; // orc3.wav
const SFX_461 := 0x461; // orc4.wav
const SFX_462 := 0x462; // orc5.wav
const SFX_463 := 0x463; // panther1.wav
const SFX_464 := 0x464; // panther2.wav
const SFX_465 := 0x465; // panther3.wav
const SFX_466 := 0x466; // panther4.wav
const SFX_467 := 0x467; // panther5.wav
const SFX_468 := 0x468; // pixie_atk1.wav
const SFX_469 := 0x469; // pixie_cast.wav
const SFX_46A := 0x46A; // pixie_die.wav
const SFX_46B := 0x46B; // pixie_figt.wav
const SFX_46C := 0x46C; // pixie_gethit.wav
const SFX_46D := 0x46D; // pixie_walk.wav
const SFX_46E := 0x46E; // pixie2_attack.wav
const SFX_46F := 0x46F; // pixie2_cast.wav
const SFX_470 := 0x470; // pixie2_die.wav
const SFX_471 := 0x471; // pixie2_fidget.wav
const SFX_472 := 0x472; // pixie2_gethit.wav
const SFX_473 := 0x473; // pixie2_walk_fly1.wav
const SFX_474 := 0x474; // pixie2_walk_fly2.wav
const SFX_475 := 0x475; // poison.wav
const SFX_476 := 0x476; // portclos.wav
const SFX_477 := 0x477; // portcul4.wav
const SFX_478 := 0x478; // portcul5.wav
const SFX_479 := 0x479; // seahorse1.wav
const SFX_47A := 0x47A; // seahorse2.wav
const SFX_47B := 0x47B; // seahorse3.wav
const SFX_47C := 0x47C; // seahorse4.wav
const SFX_47D := 0x47D; // seahorse5.wav
const SFX_47E := 0x47E; // shadowlord1.wav
const SFX_47F := 0x47F; // shadowlord2.wav
const SFX_480 := 0x480; // shadowlord3.wav
const SFX_481 := 0x481; // shadowlord4.wav
const SFX_482 := 0x482; // shadowlord5.wav
const SFX_483 := 0x483; // shadows1.wav
const SFX_484 := 0x484; // shadows2.wav
const SFX_485 := 0x485; // shadows3.wav
const SFX_486 := 0x486; // shadows4.wav
const SFX_487 := 0x487; // shadows5.wav
const SFX_488 := 0x488; // sheep3.wav
const SFX_489 := 0x489; // skeletaldragon1.wav
const SFX_48A := 0x48A; // skeletaldragon2.wav
const SFX_48B := 0x48B; // skeletaldragon3.wav
const SFX_48C := 0x48C; // skeletaldragon4.wav
const SFX_48D := 0x48D; // skeletaldragon5.wav
const SFX_48E := 0x48E; // skletn1.wav
const SFX_48F := 0x48F; // skletn2.wav
const SFX_490 := 0x490; // skletn3.wav
const SFX_491 := 0x491; // skletn4.wav
const SFX_492 := 0x492; // skletn5.wav
const SFX_493 := 0x493; // spidrd1.wav
const SFX_494 := 0x494; // spidrd2.wav
const SFX_495 := 0x495; // spidrd3.wav
const SFX_496 := 0x496; // spidrd4.wav
const SFX_497 := 0x497; // spidrd5.wav
const SFX_498 := 0x498; // stand_harp_a_1.wav
const SFX_499 := 0x499; // stand_harp_a_2.wav
const SFX_49A := 0x49A; // stand_harp_as_1.wav
const SFX_49B := 0x49B; // stand_harp_as_2.wav
const SFX_49C := 0x49C; // stand_harp_b_1.wav
const SFX_49D := 0x49D; // stand_harp_b_2.wav
const SFX_49E := 0x49E; // stand_harp_c_1.wav
const SFX_49F := 0x49F; // stand_harp_c_2.wav
const SFX_4A0 := 0x4A0; // stand_harp_c_3.wav
const SFX_4A1 := 0x4A1; // stand_harp_cs_1.wav
const SFX_4A2 := 0x4A2; // stand_harp_cs_2.wav
const SFX_4A3 := 0x4A3; // stand_harp_d_1.wav
const SFX_4A4 := 0x4A4; // stand_harp_d_2.wav
const SFX_4A5 := 0x4A5; // stand_harp_ds_1.wav
const SFX_4A6 := 0x4A6; // stand_harp_ds_2.wav
const SFX_4A7 := 0x4A7; // stand_harp_e_1.wav
const SFX_4A8 := 0x4A8; // stand_harp_e_2.wav
const SFX_4A9 := 0x4A9; // stand_harp_f_1.wav
const SFX_4AA := 0x4AA; // stand_harp_f_2.wav
const SFX_4AB := 0x4AB; // stand_harp_fs_1.wav
const SFX_4AC := 0x4AC; // stand_harp_fs_2.wav
const SFX_4AD := 0x4AD; // stand_harp_g_1.wav
const SFX_4AE := 0x4AE; // stand_harp_g_2.wav
const SFX_4AF := 0x4AF; // stand_harp_gs_1.wav
const SFX_4B0 := 0x4B0; // stand_harp_gs_2.wav
const SFX_4B1 := 0x4B1; // succubus1.wav
const SFX_4B2 := 0x4B2; // succubus2.wav
const SFX_4B3 := 0x4B3; // succubus3.wav
const SFX_4B4 := 0x4B4; // succubus4.wav
const SFX_4B5 := 0x4B5; // succubus5.wav
const SFX_4B6 := 0x4B6; // tamb_battle.wav
const SFX_4B7 := 0x4B7; // tamb_drama.wav
const SFX_4B8 := 0x4B8; // tamb_peace.wav
const SFX_4B9 := 0x4B9; // torch_ext.wav
const SFX_4BA := 0x4BA; // torch_ignite.wav
const SFX_4BB := 0x4BB; // torch_loop.wav
const SFX_4BC := 0x4BC; // torch01.wav
const SFX_4BD := 0x4BD; // unicorn1.wav
const SFX_4BE := 0x4BE; // unicorn2.wav
const SFX_4BF := 0x4BF; // unicorn3.wav
const SFX_4C0 := 0x4C0; // unicorn4.wav
const SFX_4C1 := 0x4C1; // unicorn5.wav
const SFX_4C2 := 0x4C2; // walrus5.wav
const SFX_4C3 := 0x4C3; // bear1b.wav
const SFX_4C4 := 0x4C4; // bear1c.wav
const SFX_4C5 := 0x4C5; // bear2c.wav
const SFX_4C6 := 0x4C6; // bear3c.wav
const SFX_4C7 := 0x4C7; // bear4c.wav
const SFX_4C8 := 0x4C8; // bear5c.wav
const SFX_4C9 := 0x4C9; // crossbowb.wav
const SFX_4CA := 0x4CA; // crossbowc.wav
const SFX_4CB := 0x4CB; // deer4b.wav
const SFX_4CC := 0x4CC; // deer5.wav
const SFX_4CD := 0x4CD; // drink5b.wav
const SFX_4CE := 0x4CE; // drink5c.wav
const SFX_4CF := 0x4CF; // earthqke.wav
const SFX_4D0 := 0x4D0; // exp05b.wav
const SFX_4D1 := 0x4D1; // flame1b.wav
const SFX_4D2 := 0x4D2; // fshplshb.wav
const SFX_4D3 := 0x4D3; // lever01b.wav
const SFX_4D4 := 0x4D4; // lever02b.wav
const SFX_4D5 := 0x4D5; // llama3.wav
const SFX_4D6 := 0x4D6; // defense_mastery.wav
const SFX_4D7 := 0x4D7; // crane_sfxdie.wav
const SFX_4D8 := 0x4D8; // crane_sfxhit.wav
const SFX_4D9 := 0x4D9; // crane_sfxidle.wav
const SFX_4DA := 0x4DA; // crane_sfxnotice.wav
const SFX_4DB := 0x4DB; // crane_sfxwashit.wav
const SFX_4DC := 0x4DC; // kitsune_tsuki_sfxdie.wav
const SFX_4DD := 0x4DD; // kitsune_tsuki_sfxhit.wav
const SFX_4DE := 0x4DE; // kitsune_tsuki_sfxidle.wav
const SFX_4DF := 0x4DF; // kitsune_tsuki_sfxnotice.wav
const SFX_4E0 := 0x4E0; // kitsune_tsuki_sfxwashit.wav
const SFX_4E1 := 0x4E1; // oni_sfxdie.wav
const SFX_4E2 := 0x4E2; // oni_sfxhit.wav
const SFX_4E3 := 0x4E3; // oni_sfxidle.wav
const SFX_4E4 := 0x4E4; // oni_sfxnotice.wav
const SFX_4E5 := 0x4E5; // oni_sfxwashit.wav
const SFX_4E6 := 0x4E6; // runebeetle_sxfdie.wav
const SFX_4E7 := 0x4E7; // runebeetle_sxfhit.wav
const SFX_4E8 := 0x4E8; // runebeetle_sxfidle.wav
const SFX_4E9 := 0x4E9; // runebeetle_sxfnotice.wav
const SFX_4EA := 0x4EA; // runebeetle_sxfwashit.wav
const SFX_4EB := 0x4EB; // yamandon_sfxdie.wav
const SFX_4EC := 0x4EC; // yamandon_sfxdie2.wav
const SFX_4ED := 0x4ED; // yamandon_sfxhit.wav
const SFX_4EE := 0x4EE; // yamandon_sfxidle.wav
const SFX_4EF := 0x4EF; // yamandon_sfxnotice.wav
const SFX_4F0 := 0x4F0; // yamandon_sfxwashit.wav
const SFX_4F1 := 0x4F1; // deathbeetle_sfxdies.wav
const SFX_4F2 := 0x4F2; // deathbeetle_sfxhit.wav
const SFX_4F3 := 0x4F3; // deathbeetle_sfxidle.wav
const SFX_4F4 := 0x4F4; // deathbeetle_sfxnotice.wav
const SFX_4F5 := 0x4F5; // deathbeetle_sfxwashit.wav
const SFX_4F6 := 0x4F6; // gamam_sfxdie.wav
const SFX_4F7 := 0x4F7; // gamam_sfxhit.wav
const SFX_4F8 := 0x4F8; // gamam_sfxidle.wav
const SFX_4F9 := 0x4F9; // gamam_sfxidle2.wav
const SFX_4FA := 0x4FA; // gamam_sfxnotice.wav
const SFX_4FB := 0x4FB; // gamam_sfxwashit.wav
const SFX_4FC := 0x4FC; // hairiyo_sfxdie.wav
const SFX_4FD := 0x4FD; // hairiyo_sfxhit.wav
const SFX_4FE := 0x4FE; // hairiyo_sfxidle.wav
const SFX_4FF := 0x4FF; // hairiyo_sfxnotice.wav
const SFX_500 := 0x500; // hairiyo_sfxwashit.wav
const SFX_501 := 0x501; // origami_01.wav
const SFX_502 := 0x502; // origami_02.wav
const SFX_503 := 0x503; // origami_03.wav
const SFX_504 := 0x504; // shaku_bad.wav
const SFX_505 := 0x505; // shaku_good.wav
const SFX_506 := 0x506; // windchimes_01.wav
const SFX_507 := 0x507; // windchimes_02.wav
const SFX_508 := 0x508; // windchimes_03.wav
const SFX_509 := 0x509; // kappa_sfxdie.wav
const SFX_50A := 0x50A; // kappa_sfxhit.wav
const SFX_50B := 0x50B; // kappa_sfxidle.wav
const SFX_50C := 0x50C; // kappa_sfxnotice.wav
const SFX_50D := 0x50D; // kappa_sfxwashit.wav
const SFX_50E := 0x50E; // deathstrike_female.wav
const SFX_50F := 0x50F; // deathstrike_male.wav
const SFX_510 := 0x510; // focusattack.wav
const SFX_511 := 0x511; // kiattack.wav
const SFX_512 := 0x512; // mirrorimage.wav
const SFX_513 := 0x513; // shadowjump.wav
const SFX_514 := 0x514; // surpriseattack_female.wav
const SFX_515 := 0x515; // surpriseattack_male.wav
const SFX_516 := 0x516; // revlion_sfxdie.wav
const SFX_517 := 0x517; // revlion_sfxhit.wav
const SFX_518 := 0x518; // revlion_sfxidle.wav
const SFX_519 := 0x519; // revlion_sfxnotice.wav
const SFX_51A := 0x51A; // revlion_sfxwashit.wav
const SFX_51B := 0x51B; // confidence.wav
const SFX_51C := 0x51C; // evasion.wav
const SFX_51D := 0x51D; // execution.wav
const SFX_51E := 0x51E; // lightningstrike.wav
const SFX_51F := 0x51F; // momentumstrike.wav
const SFX_520 := 0x520; // whirlwind.wav
const SFX_521 := 0x521; // block.wav
const SFX_522 := 0x522; // defencemastery.wav
const SFX_523 := 0x523; // doubleshot.wav
const SFX_524 := 0x524; // doubleshot_02.wav
const SFX_525 := 0x525; // duelwield.wav
const SFX_526 := 0x526; // feint.wav
const SFX_527 := 0x527; // frenziedwhirlwind.wav
const SFX_528 := 0x528; // tengu_idle01.wav
const SFX_529 := 0x529; // tengu_idle02.wav
const SFX_52A := 0x52A; // tengu_idle03.wav
const SFX_52B := 0x52B; // triwolf_sfxdie.wav
const SFX_52C := 0x52C; // triwolf_sfxhit.wav
const SFX_52D := 0x52D; // triwolf_sfxidle.wav
const SFX_52E := 0x52E; // triwolf_sfxnotice.wav
const SFX_52F := 0x52F; // triwolf_washit.wav
const SFX_530 := 0x530; // fukiya.wav
const SFX_531 := 0x531; // shuriken_hit.wav
const SFX_532 := 0x532; // shuriken_miss.wav
const SFX_533 := 0x533; // smokebomb.wav
const SFX_534 := 0x534; // weaponhit_01.wav
const SFX_535 := 0x535; // weaponhit_02.wav
const SFX_536 := 0x536; // weaponhit_03.wav
const SFX_537 := 0x537; // weaponhit_04.wav
const SFX_538 := 0x538; // weaponhit_05.wav
const SFX_539 := 0x539; // weaponmiss_01.wav
const SFX_53A := 0x53A; // weaponmiss_02.wav
const SFX_53B := 0x53B; // weaponmiss_03.wav
const SFX_53C := 0x53C; // mmm.wav
const SFX_53D := 0x53D; // oooo.wav
const SFX_53E := 0x53E; // yikes.wav
const SFX_53F := 0x53F; // mmm.wav
const SFX_540 := 0x540; // oooo.wav
const SFX_541 := 0x541; // ouch.wav
const SFX_542 := 0x542; // owie.wav
const SFX_543 := 0x543; // yikes.wav
const SFX_544 := 0x544; // tadao.wav
const SFX_545 := 0x545; // squeal.wav
const SFX_546 := 0x546; // weight.wav
const SFX_547 := 0x547; // yelp.wav
const SFX_548 := 0x548; // squeal.wav
const SFX_549 := 0x549; // weight.wav
const SFX_54A := 0x54A; // yelp1.wav
const SFX_54B := 0x54B; // yelp2.wav
const SFX_54C := 0x54C; // moan1.wav
const SFX_54D := 0x54D; // moan2.wav
const SFX_54E := 0x54E; // moan3.wav
const SFX_54F := 0x54F; // look.wav
const SFX_550 := 0x550; // sleep.wav
const SFX_551 := 0x551; // ugly.wav
const SFX_552 := 0x552; // look.wav
const SFX_553 := 0x553; // sleep.wav
const SFX_554 := 0x554; // worry.wav
const SFX_555 := 0x555; // bonus.wav
const SFX_556 := 0x556; // phrase1.wav
const SFX_557 := 0x557; // phrase2.wav
const SFX_558 := 0x558; // phrase3.wav
const SFX_559 := 0x559; // bonus.wav
const SFX_55A := 0x55A; // phrase1.wav
const SFX_55B := 0x55B; // phrase2.wav
const SFX_55C := 0x55C; // phrase3.wav
const SFX_55D := 0x55D; // bonus.wav
const SFX_55E := 0x55E; // phrase1.wav
const SFX_55F := 0x55F; // phrase2.wav
const SFX_560 := 0x560; // bonus.wav
const SFX_561 := 0x561; // phrase1.wav
const SFX_562 := 0x562; // phrase2.wav
const SFX_563 := 0x563; // phrase1.wav
const SFX_564 := 0x564; // phrase2.wav
const SFX_565 := 0x565; // phrase3.wav
const SFX_566 := 0x566; // handsoff.wav
const SFX_567 := 0x567; // look.wav
const SFX_568 := 0x568; // icu.wav
const SFX_569 := 0x569; // look.wav
const SFX_56A := 0x56A; // howl.wav
const SFX_56B := 0x56B; // changeling_special1.wav
const SFX_56C := 0x56C; // corporealbrume_sfxwashit.wav
const SFX_56D := 0x56D; // fetidessence_sfxwashit.wav
const SFX_56E := 0x56E; // hydra_special1.wav
const SFX_56F := 0x56F; // mantraeffervescence_sfxwashit.wa
const SFX_570 := 0x570; // paroxysmus_sfxdie.wav
const SFX_571 := 0x571; // paroxysmus_sfxhit.wav
const SFX_572 := 0x572; // paroxysmus_sfxidle.wav
const SFX_573 := 0x573; // paroxysmus_sfxnotice.wav
const SFX_574 := 0x574; // paroxysmus_sfxwashit.wav
const SFX_575 := 0x575; // paroxysmus_special1.wav
const SFX_576 := 0x576; // cusidhe_sfxdie.wav
const SFX_577 := 0x577; // cusidhe_sfxhit.wav
const SFX_578 := 0x578; // cusidhe_sfxidle.wav
const SFX_579 := 0x579; // cusidhe_sfxnotice.wav
const SFX_57A := 0x57A; // cusidhe_sfxwashit.wav
const SFX_57B := 0x57B; // dryad_sfxdie.wav
const SFX_57C := 0x57C; // dryad_sfxhit.wav
const SFX_57D := 0x57D; // dryad_sfxidle.wav
const SFX_57E := 0x57E; // dryad_sfxnotice.wav
const SFX_57F := 0x57F; // dryad_sfxwashit.wav
const SFX_580 := 0x580; // interredgrizzle_sfxdie.wav
const SFX_581 := 0x581; // interredgrizzle_sfxhit.wav
const SFX_582 := 0x582; // interredgrizzle_sfxidle.wav
const SFX_583 := 0x583; // interredgrizzle_sfxnotice.wav
const SFX_584 := 0x584; // interredgrizzle_sfxwashit.wav
const SFX_585 := 0x585; // interredgrizzle_special1.wav
const SFX_586 := 0x586; // satyr_sfxdie.wav
const SFX_587 := 0x587; // satyr_sfxhit.wav
const SFX_588 := 0x588; // satyr_sfxidle.wav
const SFX_589 := 0x589; // satyr_sfxnotice.wav
const SFX_58A := 0x58A; // satyr_sfxwashit.wav
const SFX_58B := 0x58B; // satyr_special1.wav
const SFX_58C := 0x58C; // satyr_special2.wav
const SFX_58D := 0x58D; // satyr_special3.wav
const SFX_58E := 0x58E; // shimmeringeffusion_sfxdie.wav
const SFX_58F := 0x58F; // shimmeringeffusion_sfxhit1.wav
const SFX_590 := 0x590; // shimmeringeffusion_sfxhit2.wav
const SFX_591 := 0x591; // shimmeringeffusion_sfxhit3.wav
const SFX_592 := 0x592; // shimmeringeffusion_sfxhit4.wav
const SFX_593 := 0x593; // shimmeringeffusion_sfxhit5.wav
const SFX_594 := 0x594; // shimmeringeffusion_sfxidle.wav
const SFX_595 := 0x595; // shimmeringeffusion_sfxnotice.wav
const SFX_596 := 0x596; // shimmeringeffusion_sfxwashit.wav
const SFX_597 := 0x597; // tormentedminotaur_sfxdie.wav
const SFX_598 := 0x598; // tormentedminotaur_sfxhit.wav
const SFX_599 := 0x599; // tormentedminotaur_sfxidle.wav
const SFX_59A := 0x59A; // tormentedminotaur_sfxnotice.wav
const SFX_59B := 0x59B; // tormentedminotaur_sfxwashit.wav
const SFX_59C := 0x59C; // tormentedminotaur_special1.wav
const SFX_59D := 0x59D; // tormentedminotaur_special2.wav
const SFX_59E := 0x59E; // troglodyte_sfxdie.wav
const SFX_59F := 0x59F; // troglodyte_sfxhit.wav
const SFX_5A0 := 0x5A0; // troglodyte_sfxidle.wav
const SFX_5A1 := 0x5A1; // troglodyte_sfxnotice.wav
const SFX_5A2 := 0x5A2; // troglodyte_sfxwashit.wav
const SFX_5A3 := 0x5A3; // aquarium_deadfish.wav
const SFX_5A4 := 0x5A4; // aquarium_rewards.wav
const SFX_5A5 := 0x5A5; // aquarium_use.wav
const SFX_5A6 := 0x5A6; // collection_negative.wav
const SFX_5A7 := 0x5A7; // collection_positive.wav
const SFX_5A8 := 0x5A8; // collection_reward.wav
const SFX_5A9 := 0x5A9; // crafting_effect1.wav
const SFX_5AA := 0x5AA; // crafting_effect2.wav
const SFX_5AB := 0x5AB; // crafting_effect3.wav
const SFX_5AC := 0x5AC; // crafting_effect4.wav
const SFX_5AD := 0x5AD; // crafting_effect5.wav
const SFX_5AE := 0x5AE; // crafting_effect6.wav
const SFX_5AF := 0x5AF; // crafting_effect7.wav
const SFX_5B0 := 0x5B0; // crafting_effect8.wav
const SFX_5B1 := 0x5B1; // fiddle_use_poorly.wav
const SFX_5B2 := 0x5B2; // fiddle_use_well.wav
const SFX_5B3 := 0x5B3; // parrot_use.wav
const SFX_5B4 := 0x5B4; // quest_abandon.wav
const SFX_5B5 := 0x5B5; // quest_accept.wav
const SFX_5B6 := 0x5B6; // quest_complete.wav
const SFX_5B7 := 0x5B7; // quest_update.wav
const SFX_5B8 := 0x5B8; // satyrpipe_use_poorly.wav
const SFX_5B9 := 0x5B9; // satyrpipe_use_well.wav
const SFX_5BA := 0x5BA; // virtuecards_add_remove.wav
const SFX_5BB := 0x5BB; // virtuecards_rightcombo.wav
const SFX_5BC := 0x5BC; // virtuecards_wrongcombo.wav
const SFX_5BD := 0x5BD; // bladeweave.wav
const SFX_5BE := 0x5BE; // forcearrow.wav
const SFX_5BF := 0x5BF; // forceofnature.wav
const SFX_5C0 := 0x5C0; // lightningarrow.wav
const SFX_5C1 := 0x5C1; // arcanecircle.wav
const SFX_5C2 := 0x5C2; // arcaneempowerment.wav
const SFX_5C3 := 0x5C3; // arcaneempowerment_end.wav
const SFX_5C4 := 0x5C4; // attuneweapon.wav
const SFX_5C5 := 0x5C5; // dryadallure.wav
const SFX_5C6 := 0x5C6; // dryadallure_failure.wav
const SFX_5C7 := 0x5C7; // essenceofwind.wav
const SFX_5C8 := 0x5C8; // essenceofwind_cold.wav
const SFX_5C9 := 0x5C9; // etherealvoyage.wav
const SFX_5CA := 0x5CA; // giftofrenewal.wav
const SFX_5CB := 0x5CB; // immolatingweapon.wav
const SFX_5CC := 0x5CC; // naturesfury.wav
const SFX_5CD := 0x5CD; // naturesfury_swarmattack.wav
const SFX_5CE := 0x5CE; // naturesfury_swarmidle.wav
const SFX_5CF := 0x5CF; // thunderstorm.wav
const SFX_5D0 := 0x5D0; // wildfire.wav

// Fill in the rest of the T2A sounds here.
// * music *
const SFX_MUSIC_LUTE_GOOD			:=  SFX_4D;
const SFX_MUSIC_LUTE_FAIL           :=  SFX_4E;
const SFX_MUSIC_DRUM_GOOD           :=  SFX_39;
const SFX_MUSIC_DRUM_FAIL           :=  SFX_3A;
const SFX_MUSIC_HARP_LARGE_GOOD     :=  SFX_44;
const SFX_MUSIC_HARP_LARGE_FAIL     :=  SFX_45;
const SFX_MUSIC_HARP_LAP_GOOD       :=  SFX_46;
const SFX_MUSIC_HARP_LAP_FAIL       :=  SFX_47;
const SFX_MUSIC_TAMBORINE_GOOD      :=  SFX_53;
const SFX_MUSIC_TAMBORINE_FAIL      :=  SFX_54;


// * skills *
const SFX_SKILL_SPIRITSPEAK         :=  SFX_24B;
const SFX_SKILL_LUMBERJACK          :=  SFX_13F;

// * items *
const SFX_TRAP_NEEDLE               :=  SFX_225;
const SFX_TRAP_POISON               :=  SFX_206;
const SFX_TRAP_EXPLOSION            :=  SFX_208;
const SFX_ANVIL                     :=  SFX_2B;
const SFX_PICK                      :=  SFX_13B;
const SFX_HAMMER                    :=  SFX_43;
const SFX_GLASSBREAK4               :=  SFX_42;
const SFX_HARP_SUCC                 :=  SFX_46;
const SFX_LUTE_SUCC                 :=  SFX_4D;
const SFX_DRUM_SUCC                 :=  SFX_39;
const SFX_TAMB_SUCC                 :=  SFX_53;

const SFX_HARP_FAIL                 :=  SFX_47;
const SFX_LUTE_FAIL                 :=  SFX_4E;
const SFX_DRUM_FAIL                 :=  SFX_3A;
const SFX_TAMB_FAIL                 :=  SFX_54;


// * doors *
const SFX_OPEN_WOODEN_DOOR          :=  SFX_EB;
const SFX_CLOSE_WOODEN_DOOR         :=  SFX_F2;
const SFX_OPEN_METAL_DOOR           :=  SFX_ED;
const SFX_CLOSE_METAL_DOOR          :=  SFX_F4;


////
// Animations/Actions
///
// * Human animations (all humans have these) (this is not complete, will update later)
const ANIM_WALK_UNARMED         :=  0x0000; // Walk unarmed.
const ANIM_WALK_ARMED           :=  0x0001; // Walk armed.
const ANIM_RUN_UNARMED          :=  0x0002; // Run unarmed.
const ANIM_RUN_ARMED            :=  0x0003; // Run armed.
const ANIM_STAND                :=  0x0004; // Stand.
const ANIM_FIDGET_1             :=  0x0005; // Twist a little (look around?).
const ANIM_FIDGET_2             :=  0x0006; // Look down.
const ANIM_STAND_1HAND          :=  0x0007; // Stand 1 handed attack.
const ANIM_STAND_2HAND          :=  0x0008; // Stand 2 handed attack.
const ANIM_ATTACK_1HAND_WIDE    :=  0x0009; // Attack 1 handed wide (sword/any).
const ANIM_ATTACK_1HAND_JAB     :=  0x000a; // Attack 1 handed jab (fencing).
const ANIM_ATTACK_1HAND_DOWN    :=  0x000b; // Attack 1 handed down (mace).
const ANIM_ATTACK_2HAND_DOWN    :=  0x000c; // Attack 2 handed jab (mace).
const ANIM_ATTACK_2HAND_WIDE    :=  0x000d; // Attack 2 handed wide.
const ANIM_ATTACK_2HAND_JAB     :=  0x000e; // Attack 2 handed jab (spear).
const ANIM_WALK_WARMODE         :=  0x000f; // Walk in warmode.
const ANIM_CAST_DIR             :=  0x0010; // Spellcasting direction.
const ANIM_CAST_AREA            :=  0x0011; // Spellcasting area.
const ANIM_ATTACK_BOW           :=  0x0012; // Attack bow.
const ANIM_ATTACK_XBOW          :=  0x0013; // Attack xbow.
const ANIM_HIT                  :=  0x0014; // Get hit.
const ANIM_DIE_BACK             :=  0x0015; // Die backward.
const ANIM_DIE_FOR              :=  0x0016; // Die forward.
const ANIM_BOW                  :=  0x0020; // Bow.
const ANIM_SALUTE               :=  0x0021; // Salute.
const ANIM_EAT                  :=  0x0022; // Eat.

////
// Graphicaleffects (itemIDs)
///
// * general *
const FX_SPLASH         :=  0x352d; //
const FX_EXPLODE_1      :=  0x36ca; //
const FX_EXPLODE_2      :=  0x36bd; //
const FX_EXPLODE_3      :=  0x36b0; //
const FX_FIREBALL       :=  0x36d4; //
const FX_MAGIC_ARROW    :=  0x36e4; //
const FX_FIRE_BOLT      :=  0x36f4; // Firesnake. (pagan?)
const FX_EXPLODE_BALL   :=  0x36fe; // Not used.

const FX_FLAMESTRIKE    :=  0x3709; //
const FX_SMOKE          :=  0x3728;
const FX_TELE_VANISH    :=  0x372a; // Teleport vanish.
const FX_SPELL_FAIL     :=  0x3735; //
const FX_BLESS_EFFECT   :=  0x373a; //
const FX_CURSE_EFFECT   :=  0x374a; //
const FX_SPARK_EFFECT   :=  0x375a; // 
const FX_HEAL_EFFECT    :=  0x376a; //
const FX_MARK_EFFECT    :=  0x3779;
const FX_BLUEMOONSTART  :=  0x3789; // Deathvortex?
const FX_ENERGY_BOLT    :=  0x379f; //
const FX_BLADES_EMERGE  :=  0x37a0; // Not used.
const FX_GLOW           :=  0x37b9; // Not used.
const FX_GLOW_SPIKE     :=  0x37c3; // Not used.
const FX_DEATH_FUNNEL   :=  0x37cc; //
const FX_BLADES         :=  0x37eb; //
const FX_STATIC         :=  0x3818; // Not used. (pink static)
const FX_POISON_F_EW    :=  0x3914; // Poison field.
const FX_POISON_F_NS    :=  0x3920; // Poison field.
const FX_ENERGY_F_EW    :=  0x3947; // Energy field.
const FX_ENERGY_F_NS    :=  0x3956; // Energy field.
const FX_PARA_F_EW      :=  0x3967; // Paralyze field.
const FX_PARA_F_NS      :=  0x3979; // Paralyze field.
const FX_FIRE_F_EW      :=  0x398c; // Fire field.
const FX_FIRE_F_NS      :=  0x3996; // Fire field.

////
// Layers
/// 
const LAYER_NONE        :=  0x00;   // Not used?
const LAYER_HAND1       :=  0x01;   // One handed weapon.
const LAYER_HAND2       :=  0x02;   // Two handed weapon, shield or misc.
const LAYER_SHOES       :=  0x03;   // Shoes.
const LAYER_PANTS       :=  0x04;   // Pants.
const LAYER_SHIRT       :=  0x05;   // Shirt.
const LAYER_HELM        :=  0x06;   // Helm or Hat.
const LAYER_GLOVES      :=  0x07;   // Gloves.
const LAYER_RING        :=  0x08;   // Ring.
const LAYER_UNUSED9     :=  0x09;   // Unused.
const LAYER_NECK        :=  0x0a;   // Neck.
const LAYER_HAIR        :=  0x0b;   // Hair
const LAYER_WAIST       :=  0x0c;   // Waist (half apron).
const LAYER_CHEST       :=  0x0d;   // Torso (inner) (chest armor).
const LAYER_WRIST       :=  0x0e;   // Bracelet.
const LAYER_PACK2       :=  0x0f;   // Unused (backpack, but ord. bp is 0x15).
const LAYER_BEARD       :=  0x10;   // Facial hair.
const LAYER_TUNIC       :=  0x11;   // Torso (middle) (tunic, sash etc.).
const LAYER_EARS        :=  0x12;   // Earrings.
const LAYER_ARMS        :=  0x13;   // Arms.
const LAYER_CAPE        :=  0x14;   // Back (cloak).
const LAYER_PACK        :=  0x15;   // Backpack
const LAYER_ROBE        :=  0x16;   // Torso (outer) (robe)
const LAYER_SKIRT       :=  0x17;   // Legs (outer) (skirt/robe).
const LAYER_LEGS        :=  0x18;   // Legs (inner) (leg armor).
const LAYER_MOUNT       :=  0x19;   // Mount (horse, ostard etc.).
const LAYER_VENDORSTOCK :=  0x1a;   // This vendor will sell and restock.
const LAYER_VENDOREXTRA :=  0x1b;   // This vendor will resell to players but not restock.
const LAYER_VENDORBUY   :=  0x1c;   // This vendor can buy from players but does not stock.
const LAYER_BANKBOX     :=  0x1d;   // Contents of bankbox
const LAYER_TRADE       :=  0x1e;   // Can be multiple of these, do not use directly.

////
// Fonts
/// 
const FONT_MAX          :=  0x00;   // Lowest font.
const FONT_MIN          :=  0x09;   // Highest font.

const FONT_BOLD         :=  0x00;   // Bold text.
const FONT_SHADOW       :=  0x01;   // Text with shadow.
const FONT_BOLD_SHAD    :=  0x02;   // Bold text with shadow.
const FONT_NORMAL       :=  0x03;   // Normal (default).
const FONT_GOTHIC       :=  0x04;   // Gothic.
const FONT_ITALIC       :=  0x05;   // Italic
const FONT_SMALL_DARK   :=  0x06;   // Small dark letters.
const FONT_COLOR        :=  0x07;   // Colorful font.
const FONT_RUNE         :=  0x08;   // Rune font (only capitals).
const FONT_SMALL_LIGHT  :=  0x09;   // Small light letters.

////
// Creature IDs ( hardcoded graphic IDs )
/// 
const CID_ZERO			:= 0x0000;  // Not used.
const CID_OGRE			:= 0x0001;  // Ogre.
const CID_ETTIN			:= 0x0002;  // Ettin.
const CID_ZOMBIE		:= 0x0003;  // Zombie.
const CID_GARGOYLE		:= 0x0004;  // Gargoyle.
const CID_EAGLE			:= 0x0005;  // Eagle.
const CID_BIRD			:= 0x0006;  // Bird.
const CID_ORC_LORD		:= 0x0007;  // Orc Lord.
const CID_CORPSER		:= 0x0008;  // Corpser.
const CID_DAEMON		:= 0x0009;  // Daemon.
const CID_DAEMON_SWORD		:= 0x000a;  // Daemon with sword.
const CID_DRAGON_GREY		:= 0x000c;  // Dragon grey.
const CID_AIR_ELEMENTAL		:= 0x000d;  // Air Elemental.
const CID_EARTH_ELEMENTAL	:= 0x000e;  // Earth Elemental.
const CID_FIRE_ELEMENTAL	:= 0x000f;  // Fire Elemental.
const CID_WATER_ELEMENTAL	:= 0x0010;  // Water elemental.
const CID_ORC			:= 0x0011;  // Orc.
const CID_ETTIN_AXE		:= 0x0012;  // Ettin with axe.
const CID_GIANT_SNAKE		:= 0x0015;  // Giant
const CID_UNSHORN_SHEEP		:= 0x00cf;
const CID_SHORN_SHEEP		:= 0x00df;

const CID_HUMAN_MALE		:= 0x0190;
const CID_HUMAN_FEMALE		:= 0x0191;
const CID_GM			:= 0x03db;

////
// Directions
/// 
const DIR_N             :=  0x00;   // North.
const DIR_NE            :=  0x01;   // Northeast.
const DIR_E             :=  0x02;   // East.
const DIR_SE            :=  0x03;   // Southeast.
const DIR_S             :=  0x04;   // South.
const DIR_SW            :=  0x05;   // Southwest.
const DIR_W             :=  0x06;   // West.
const DIR_NW            :=  0x07;   // Northwest.

// * Colors *
// This is not suposed to be a "complete" colorchart, but a
// quick way to access normal colors.
const COLOR_DEFAULT         := 0x0000;
const COLOR_BLACK           := 0x0001;
const COLOR_INDIGO_DARK     := 0x0007;
const COLOR_INDIGO          := 0x0008;
const COLOR_INDIGO_LIGHT    := 0x0009;

const COLOR_VIOLET_DARK     := 0x000c;
const COLOR_VIOLET          := 0x000d;
const COLOR_VIOLET_LIGHT    := 0x000e;
const COLOR_MAGENTA_DARK    := 0x0011;
const COLOR_MAGENTA         := 0x0012;
const COLOR_MAGENTA_LIGHT   := 0x0013;
const COLOR_RED_DARK        := 0x0020;
const COLOR_RED             := 0x0022;
const COLOR_RED_LIGHT       := 0x0023;
const COLOR_ORANGE_DARK     := 0x002a;
const COLOR_ORANGE          := 0x002b;
const COLOR_ORANGE_LIGHT    := 0x002c;
const COLOR_YELLOW_DARK     := 0x0034;
const COLOR_YELLOW          := 0x0035;
const COLOR_YELLOW_LIGHT    := 0x0036;
const COLOR_GREEN_DARK      := 0x003e;
const COLOR_GREEN           := 0x003f;
const COLOR_GREEN_LIGHT     := 0x0040;
const COLOR_CYAN_DARK       := 0x0057;
const COLOR_CYAN            := 0x0058;
const COLOR_CYAN_LIGHT      := 0x0059;
const COLOR_BLUE_DARK       := 0x0061;
const COLOR_BLUE            := 0x0062;
const COLOR_BLUE_LIGHT      := 0x0063;
const COLOR_GRAY_DARK       := 0x0386;
const COLOR_GRAY            := 0x0387;
const COLOR_GRAY_LIGHT      := 0x0388;
const COLOR_BLACKMETAL      := 0x044e;
const COLOR_GOLDMETAL       := 0x046e;
const COLOR_BSHIMMER        := 0x0480;	// T2A Only : Waterelemental color.
const COLOR_WHITE           := 0x0481;	// T2A Only : True white.
const COLOR_STONE           := 0x0482;	// T2A Only : Rock ( stoneharpy?)
const COLOR_SPECTRAL        := 0x4631;  // Spectral
const COLOR_TRANSLUCENT     := 0x7cf8;  // Translucent

////
// Font Colours.
////
const FONTCOLOR_RED		:= 0x0026;
const FONTCOLOR_GREEN		:= 0x0044;
const FONTCOLOR_BLUE		:= 0x005a;
const FONTCOLOR_MAUVE		:= 0x001e;
const FONTCOLOR_LAVENDER	:= 0x0014;
const FONTCOLOR_BLACK		:= 0x0001;
const FONTCOLOR_BROWN		:= 0x002e;
const FONTCOLOR_YELLOW		:= 0x0035;

////
// SkillIDs
/// 
const SKILLID_ALCHEMY           := 0;
const SKILLID_ANATOMY           := 1;
const SKILLID_ANIMALLORE        := 2;
const SKILLID_ITEMID            := 3;
const SKILLID_ARMSLORE          := 4;
const SKILLID_BATTLE_DEFENSE    := 5;
const SKILLID_PARRY             := 5;
const SKILLID_BEGGING           := 6;
const SKILLID_BLACKSMITHY       := 7;
const SKILLID_BOWCRAFT          := 8;
const SKILLID_PEACEMAKING       := 9;
const SKILLID_CAMPING           := 10;
const SKILLID_CARPENTRY         := 11;
const SKILLID_CARTOGRAPHY       := 12;
const SKILLID_COOKING           := 13;
const SKILLID_DETECTINGHIDDEN   := 14;
const SKILLID_ENTICEMENT        := 15;
const SKILLID_EVALINT           := 16;
const SKILLID_HEALING           := 17;
const SKILLID_FISHING           := 18;
const SKILLID_FORENSICS         := 19;
const SKILLID_HERDING           := 20;
const SKILLID_HIDING            := 21;
const SKILLID_PROVOCATION       := 22;
const SKILLID_INSCRIPTION       := 23;
const SKILLID_LOCKPICKING       := 24;
const SKILLID_MAGERY            := 25;
const SKILLID_MAGICRESISTANCE   := 26;
const SKILLID_TACTICS           := 27;
const SKILLID_SNOOPING          := 28;
const SKILLID_MUSICIANSHIP      := 29;
const SKILLID_POISONING         := 30;
const SKILLID_ARCHERY           := 31;
const SKILLID_SPIRITSPEAK       := 32;
const SKILLID_STEALING          := 33;
const SKILLID_TAILORING         := 34;
const SKILLID_TAMING            := 35;
const SKILLID_TASTEID           := 36;
const SKILLID_TINKERING         := 37;
const SKILLID_TRACKING          := 38;
const SKILLID_VETERINARY        := 39;
const SKILLID_SWORDSMANSHIP     := 40;
const SKILLID_MACEFIGHTING      := 41;
const SKILLID_FENCING           := 42;
const SKILLID_WRESTLING         := 43;
const SKILLID_LUMBERJACKING     := 44;
const SKILLID_MINING            := 45;
const SKILLID_MEDITATION        := 46;
const SKILLID_STEALTH           := 47;
const SKILLID_REMOVETRAP        := 48;
const SKILLID__HIGHEST          := 48;


/////////////////////////////////////  SPELLS START  //////////////////////////////

///////////////////////
//        All        //
///////////////////////

const ACTION_BOW		:= 0x0020;	// Bow animation

const SFX_SPELL_FIZZLE		:= 0x05d;	// Fizzling a spell SFX
const FX_SPELL_FIZZLE		:= 0x3735; 	// Fizzle a spell FX
const MOVING_SPELL_FIZZLE	:= 0;
const SPEED_SPELL_FIZZLE	:= 7;		// Speed for fizzling effect
const LOOP_SPELL_FIZZLE		:= 7;		// Loop for fizzling effect


const CORPSE_OBJTYPE		:= 0x2006;	// Objtype of a corpse


///////////////////////
// 1st Circle Spells //
///////////////////////

//Clumsy
const SPELLID_CLUMSY  		:= 1;
const SFX_SPELL_CLUMSY 		:= 0x01e0;
const FX_SPELL_CLUMSY		:= 0x374a;
const MOVING_SPELL_CLUMSY	:= 0;
const SPEED_SPELL_CLUMSY	:= 7;
const LOOP_SPELL_CLUMSY		:= 7;

//Create Food
const SPELLID_CREATE_FOOD	:= 2;
const SFX_SPELL_CREATE_FOOD	:= 0x01e3;
				// Have no FX

//Feeblemind
const SPELLID_FEEBLEMIND	:= 3;
const SFX_SPELL_FEEBLEMIND      := 0x01e5;
const FX_SPELL_FEEBLEMIND   	:= 0x374a;
const MOVING_SPELL_FEEBLEMIND	:= 0;
const SPEED_SPELL_FEEBLEMIND	:= 7;
const LOOP_SPELL_FEEBLEMIND	:= 7;

//Heal
const SPELLID_HEAL		:= 4;
const SFX_SPELL_HEAL            := 0x01f3;
const FX_SPELL_HEAL    		:= 0x376a;
const MOVING_SPELL_HEAL		:= 0;
const SPEED_SPELL_HEAL		:= 7;
const LOOP_SPELL_HEAL		:= 0x10;

//Magic Arrow
const SPELLID_MAGIC_ARROW	:= 5;
const SFX_SPELL_MAGIC_ARROW     := 0x01e6;
const FX_SPELL_MAGIC_ARROW    	:= 0x36e4;
const MOVING_SPELL_MAGIC_ARROW 	:= 1;
const SPEED_SPELL_MAGIC_ARROW  	:= 5;
const LOOP_SPELL_MAGIC_ARROW	:= 1;

//Night Sight
const SPELLID_NIGHTSIGHT	:= 6;
const SFX_SPELL_NIGHT_SIGHT     := 0x01e4;
const FX_SPELL_NIGHT_SIGHT   	:= 0x373a;
const MOVING_SPELL_NIGHT_SIGHT 	:= 0;
const SPEED_SPELL_NIGHT_SIGHT  	:= 10;
const LOOP_SPELL_NIGHT_SIGHT	:= 10;

//Reactive Armor
const SPELLID_REACTIVE_ARMOR	:= 7;
const SFX_SPELL_REACTIVE_ARMOR  := 0x01f3;
const FX_SPELL_REACTIVE_ARMOR	:= 0x373a;
const MOVING_SPELL_REACTIVE_ARMOR 	:= 0;
const SPEED_SPELL_REACTIVE_ARMOR  	:= 5;
const LOOP_SPELL_REACTIVE_ARMOR		:= 5;

//Weaken
const SPELLID_WEAKEN		:= 8;
const SFX_SPELL_WEAKEN          := 0x01e7;
const FX_SPELL_WEAKEN	   	:= 0x374a;
const MOVING_SPELL_WEAKEN	:= 0;
const SPEED_SPELL_WEAKEN	:= 7;
const LOOP_SPELL_WEAKEN		:= 7;


///////////////////////
// 2nd Circle Spells //
///////////////////////

//Agility
const SPELLID_AGILITY		:= 9;
const SFX_SPELL_AGILITY         := 0x01e8;
const FX_SPELL_AGILITY 		:= 0x373a;
const MOVING_SPELL_AGILITY	:= 0;
const SPEED_SPELL_AGILITY	:= 7;
const LOOP_SPELL_AGILITY	:= 7;

//Cunning
const SPELLID_CUNNING		:= 10;
const SFX_SPELL_CUNNING         := 0x01ec;
const FX_SPELL_CUNNING   	:= 0x373a;
const MOVING_SPELL_CUNNING	:= 0;
const SPEED_SPELL_CUNNING	:= 7;
const LOOP_SPELL_CUNNING	:= 7;

//Cure
const SPELLID_CURE		:= 11;
const SFX_SPELL_CURE            := 0x01e1;
const FX_SPELL_CURE   		:= 0x373a;
const MOVING_SPELL_CURE		:= 7;
const SPEED_SPELL_CURE		:= 7;
const LOOP_SPELL_CURE		:= 0xa;

//Harm
const SPELLID_HARM		:= 12;
const SFX_SPELL_HARM            := 0x01f2;
const FX_SPELL_HARM		:= 0x37c3;
const MOVING_SPELL_HARM		:= 0;
const SPEED_SPELL_HARM		:= 0x0a;
const LOOP_SPELL_HARM		:= 0x1e;

//Magic Trap
const SPELLID_MAGIC_TRAP	:= 13;
const SFX_SPELL_MAGIC_TRAP1 	:= 0x01f0;
const SFX_SPELL_MAGIC_TRAP2 	:= 0x0236;
				// Unknow FX

//Magic Untrap
const SPELLID_MAGIC_UNTRAP	:= 14;
const SFX_SPELL_MAGIC_UNTRAP1	:= 0x01f1;
const SFX_SPELL_MAGIC_UNTRAP2	:= 0x011f;
				// Unknow FX

//Protection
const SPELLID_PROTECTION	:= 15;
const SFX_SPELL_PROTECTION      := 0x01ee;
const FX_SPELL_PROTECTION	:= 0x373a;
const MOVING_SPELL_PROTECTION	:= 0;
const SPEED_SPELL_PROTECTION	:= 7;
const LOOP_SPELL_PROTECTION	:= 7;

//Strength
const SPELLID_STRENGTH		:= 16;
const SFX_SPELL_STRENGTH        := 0x01ef;
const FX_SPELL_STRENGTH		:= 0x373a;
const MOVING_SPELL_STRENGTH	:= 0;
const SPEED_SPELL_STRENGTH	:= 7;
const LOOP_SPELL_STRENGTH	:= 7;


///////////////////////
// 3rd Circle Spells //
///////////////////////

//Bless
const SPELLID_BLESS		:= 17;
const SFX_SPELL_BLESS           := 0x01eb;
const FX_SPELL_BLESS   		:= 0x373a;
const MOVING_SPELL_BLESS	:= 0;
const SPEED_SPELL_BLESS		:= 10;
const LOOP_SPELL_BLESS		:= 10;

//Fireball
const SPELLID_FIREBALL		:= 18;
const SFX_SPELL_FIREBALL        := 0x0160;
const FX_SPELL_FIREBALL		:= 0x36d4;
const MOVING_SPELL_FIREBALL	:= 1;
const SPEED_SPELL_FIREBALL	:= 5;
const LOOP_SPELL_FIREBALL	:= 1;

//Magic Lock
const SPELLID_MAGIC_LOCK	:= 19;
const SFX_SPELL_MAGIC_LOCK1	:= 0x01f5;
const SFX_SPELL_MAGIC_LOCK2	:= 0x011f;
				// Unknow FX

//Poison
const SPELLID_POISON		:= 20;
const SFX_SPELL_POISON          := 0x0247;
const FX_SPELL_POISON	   	:= 0x374a;
const MOVING_SPELL_POISON	:= 0;
const SPEED_SPELL_POISON	:= 0x0a;
const LOOP_SPELL_POISON		:= 0x1e;

//Telekinesis
const SPELLID_TELEKINESIS	:= 21;
const SFX_SPELL_TELEKINESIS     := 0x01f6;
				// No FX

//Teleport
const SPELLID_TELEPORT		:= 22;
const SFX_SPELL_TELEPORT        := 0x01ff;
const FX_SPELL_TELEPORT		:= 0x3728;
const MOVING_SPELL_TELEPORT	:= 0;
const SPEED_SPELL_TELEPORT	:= 0xa;
const LOOP_SPELL_TELEPORT	:= 0xa;

//Unlock
const SPELLID_UNLOCK		:= 23;
const SFX_SPELL_UNLOCK1  	:= 0x0200;
const SFX_SPELL_UNLOCK2  	:= 0x011f;
				// Unknow FX

//Wall of Stone
const SPELLID_WALL_OF_STONE	:= 24;
const SFX_SPELL_WALL_OF_STONE   := 0x01f7;
				// No FX (Create the Wall)


///////////////////////
// 4th Circle Spells //
///////////////////////

//Arch Cure
const SPELLID_ARCH_CURE		:= 25;
const SFX_SPELL_ARCH_CURE       := 0x01e9;
const FX_SPELL_ARCH_CURE	:= 0x373a;
const MOVING_SPELL_ARCH_CURE	:= 0;
const SPEED_SPELL_ARCH_CURE	:= 10;
const LOOP_SPELL_ARCH_CURE	:= 10;

//Arch Protection
const SPELLID_ARCH_PROTECTION	:= 26;
const SFX_SPELL_ARCH_PROTECTION := 0x01f8;
const FX_SPELL_ARCH_PROTECTION	:= 0x373a;
const MOVING_SPELL_ARCH_PROTECTION	:= 0;
const SPEED_SPELL_ARCH_PROTECTION	:= 7;
const LOOP_SPELL_ARCH_PROTECTION	:= 7;

//Curse
const SPELLID_CURSE		:= 27;
const SFX_SPELL_CURSE           := 0x01e2;
const FX_SPELL_CURSE	   	:= 0x374a;
const MOVING_SPELL_CURSE	:= 0;
const SPEED_SPELL_CURSE		:= 10;
const LOOP_SPELL_CURSE		:= 10;

//Fire Field
const SPELLID_FIRE_FIELD	:= 28;
const SFX_SPELL_FIRE_FIELD      := 0x01de;
const FX_SPELL_FIRE_FIELD_EW    := 0x398c;
const FX_SPELL_FIRE_FIELD_NS    := 0x3996;

//Greater Heal
const SPELLID_GREATER_HEAL	:= 29;
const SFX_SPELL_GREATER_HEAL    := 0x0203;
const FX_SPELL_GREATER_HEAL   	:= 0x375a;
const MOVING_SPELL_GREATER_HEAL	:= 0;
const SPEED_SPELL_GREATER_HEAL	:= 7;
const LOOP_SPELL_GREATER_HEAL	:= 0x10;

//Lightning
const SPELLID_LIGHTNING		:= 30;
const SFX_SPELL_LIGHTNING       := 0x02a;
const MOVING_SPELL_LIGHTNING	:= 2;
	// For FX use :		PlayLightningBoltEffect(cast_on); from UO.em

//Mana Drain
const SPELLID_MANA_DRAIN	:= 31;
const SFX_SPELL_MANA_DRAIN      := 0x01f9;
const FX_SPELL_MANA_DRAIN	:= 0x376a;
const MOVING_SPELL_MANA_DRAIN	:= 0;
const SPEED_SPELL_MANA_DRAIN	:= 7;
const LOOP_SPELL_MANA_DRAIN	:= 0x10;

//Recall
const SPELLID_RECALL		:= 32;
const SFX_SPELL_RECALL1  	:= 0x0210;
const SFX_SPELL_RECALL2		:= 0x01fd;
const SFX_SPELL_RECALL		:= 0x01fd;
				// No FX

///////////////////////
// 5th Circle Spells //
///////////////////////

//Blade Spirit
const SPELLID_BLADE_SPIRIT	:= 33;
const SFX_SPELL_BLADE_SPIRIT 	:= 0x0213;
const FX_SPELL_BLADE_SPIRIT	:= 0x3728;
const MOVING_SPELL_BLADE_SPIRIT := 0;
const SPEED_SPELL_BLADE_SPIRIT  := 0xa;
const LOOP_SPELL_BLADE_SPIRIT	:= 0xa;

//Dispel Field
const SPELLID_DISPEL_FIELD	:= 34;
const SFX_SPELL_DISPEL_FIELD    := 0x0202;
const FX_SPELL_DISPEL_FIELD	:= 0x3728;
const MOVING_SPELL_DISPEL_FIELD := 0;
const SPEED_SPELL_DISPEL_FIELD  := 0xa;
const LOOP_SPELL_DISPEL_FIELD	:= 0xa;

//Incognito
const SPELLID_INCOGNITO		:= 35;
const SFX_SPELL_INCOGNITO       := 0x01e3;
				// No FX

//Spell Reflection
const SPELLID_MAGIC_REFLECTION	:= 36;
const SFX_SPELL_MAGIC_REFLECTION:= 0x01e8;
const FX_SPELL_MAGIC_REFLECTION := 0x374a;
const MOVING_SPELL_MAGIC_REFLECTION 	:= 0;
const SPEED_SPELL_MAGIC_REFLECTION  	:= 10;
const LOOP_SPELL_MAGIC_REFLECTION	:= 10;

//Mind Blast
const SPELLID_MIND_BLAST	:= 37;
const SFX_SPELL_MIND_BLAST      := 0x0214;
const FX_SPELL_MIND_BLAST 	:= 0x374a;
const MOVING_SPELL_MIND_BLAST 	:= 0;
const SPEED_SPELL_MIND_BLAST  	:= 0x0a;
const LOOP_SPELL_MIND_BLAST	:= 0x1e;

//Paralyze
const SPELLID_PARALYZE		:= 38;
const SFX_SPELL_PARALYZE        := 0x0205;
const FX_SPELL_PARALYZE 	:= 0x374a;
const MOVING_SPELL_PARALYZE 	:= 0;
const SPEED_SPELL_PARALYZE  	:= 10;
const LOOP_SPELL_PARALYZE	:= 0xa;

//Poison Field
const SPELLID_POISON_FIELD	:= 39;
const SFX_SPELL_POISON_FIELD	:= 0x0247;
const FX_SPELL_POISON_FIELD_EW 	:= 0x3914;
const FX_SPELL_POISON_FIELD_NS 	:= 0x3920;

//Summon Creature
const SPELLID_SUMMON_CREATURE	:= 40;
const SFX_SPELL_SUMMON_CREATURE := 0x0216;
const FX_SPELL_SUMMON_CREATURE	:= 0x3728;
const MOVING_SPELL_SUMMON_CREATURE	:= 0;
const SPEED_SPELL_SUMMON_CREATURE	:= 0xa;
const LOOP_SPELL_SUMMON_CREATURE	:= 0xa;


///////////////////////
// 6th Circle Spells //
///////////////////////

//Dispel
const SPELLID_DISPEL		:= 41;
const SFX_SPELL_DISPEL          := 0x0202;
const FX_SPELL_DISPEL		:= 0x3728;
const MOVING_SPELL_DISPEL	:= 0;
const SPEED_SPELL_DISPEL	:= 0xa;
const LOOP_SPELL_DISPEL		:= 0xa;

//Energy Bolt
const SPELLID_ENERGY_BOLT	:= 42;
const SFX_SPELL_ENERGY_BOLT     := 0x020b;
const FX_SPELL_ENERGY_BOLT    	:= 0x379f;
const MOVING_SPELL_ENERGY_BOLT	:= 1;
const SPEED_SPELL_ENERGY_BOLT	:= 0xa;
const LOOP_SPELL_ENERGY_BOLT	:= 0x1e;

//Explosion
const SPELLID_EXPLOSION		:= 43;
const SFX_SPELL_EXPLOSION       := 0x0208;
const FX_SPELL_EXPLOSION      	:= 0x36b0;
const MOVING_SPELL_EXPLOSION	:= 0;
const SPEED_SPELL_EXPLOSION	:= 7;
const LOOP_SPELL_EXPLOSION	:= 0x10;

//Invisibility
const SPELLID_INVISIBILITY	:= 44;
const SFX_SPELL_INVISIBILITY    := 0x0204;
const FX_SPELL_INVISIBILITY	:= 0;
const MOVING_SPELL_INVISIBILITY	:= 0;
const SPEED_SPELL_INVISIBILITY	:= 0;
const LOOP_SPELL_INVISIBILITY	:= 0;

//Mark
const SPELLID_MARK		:= 45;
const SFX_SPELL_MARK            := 0x01fb;
const FX_SPELL_MARK    		:= 0x3779;
const MOVING_SPELL_MARK		:= 0;
const SPEED_SPELL_MARK		:= 10;
const LOOP_SPELL_MARK		:= 10;

//Mass Curse
const SPELLID_MASS_CURSE	:= 46;
const SFX_SPELL_MASS_CURSE      := 0x01fc;
const FX_SPELL_MASS_CURSE 	:= 0x374a;
const MOVING_SPELL_MASS_CURSE	:= 0;
const SPEED_SPELL_MASS_CURSE	:= 10;
const LOOP_SPELL_MASS_CURSE	:= 10;

//Paralyze Field
const SPELLID_PARALYZE_FIELD	:= 47;
const SFX_SPELL_PARALYZE_FIELD  := 0x020c;
const FX_SPELL_PARALYZE_FIELD_EW:= 0x3967;
const FX_SPELL_PARALYZE_FIELD_NS:= 0x3979;

//Reveal
const SPELLID_REVEAL		:= 48;
const SFX_SPELL_REVEAL          := 0x01fe;
				// No FX

///////////////////////
// 7th Circle Spells //
///////////////////////

//Chain Lightning
const SPELLID_CHAIN_LIGHTNING	:= 49;
const SFX_SPELL_CHAIN_LIGHTNING := 0x02a;
const MOVING_SPELL_CHAIN_LIGHTNING	:= 2;
	//For FX use :		PlayLightningBoltEffect(cast_on); from UO.em

//Energy Field
const SPELLID_ENERGY_FIELD	:= 50;
const SFX_SPELL_ENERGY_FIELD    := 0x0211;

//Flame Strike
const SPELLID_FLAME_STRIKE	:= 51;
const SFX_SPELL_FLAME_STRIKE    := 0x0209;
const FX_SPELL_FLAME_STRIKE    	:= 0x3709;
const MOVING_SPELL_FLAME_STRIKE	:= 0;
const SPEED_SPELL_FLAME_STRIKE	:= 0x0a;
const LOOP_SPELL_FLAME_STRIKE	:= 0x1e;

//Gate Travel
const SPELLID_GATE_TRAVEL	:= 52;
const SFX_SPELL_GATE_TRAVEL     := 0x020f;
				// No FX

//Mana Vampire
const SPELLID_MANA_VAMPIRE	:= 53;
const SFX_SPELL_MANA_VAMPIRE    := 0x01fa;
const FX_SPELL_MANA_VAMPIRE	:= 0x37b9;
const MOVING_SPELL_MANA_VAMPIRE	:= 0;
const SPEED_SPELL_MANA_VAMPIRE	:= 7;
const LOOP_SPELL_MANA_VAMPIRE	:= 0x10;

//Mass Dispel
const SPELLID_MASS_DISPEL	:= 54;
const SFX_SPELL_MASS_DISPEL     := 0x020a;
const FX_SPELL_MASS_DISPEL	:= 0x3728;
const MOVING_SPELL_MASS_DISPEL	:= 0;
const SPEED_SPELL_MASS_DISPEL	:= 0xa;
const LOOP_SPELL_MASS_DISPEL	:= 0xa;

//Meteor Swarm
const SPELLID_METEOR_SWARM	:= 55;
const SFX_SPELL_METEOR_SWARM    := 0x0161;
const FX_SPELL_METEOR_SWARM	:= 0x36d4;
const MOVING_SPELL_METEOR_SWARM	:= 1;
const SPEED_SPELL_METEOR_SWARM	:= 5;
const LOOP_SPELL_METEOR_SWARM	:= 1;

//Polymorph
const SPELLID_POLYMORPH		:= 56;
const SFX_SPELL_POLYMORPH       := 0x020a;
const FX_SPELL_POLYMORPH	:= 0x01fa;
const MOVING_SPELL_POLYMORPH	:= 0;
const SPEED_SPELL_POLYMORPH	:= 0xa;
const LOOP_SPELL_POLYMORPH	:= 0xa;


///////////////////////
// 8th Circle Spells //
///////////////////////

//Earthquake
const SPELLID_EARTHQUAKE	:= 57;
const SFX_SPELL_EARTHQUAKE	:= 0x020e;
const SFX_SPELL_EARTHQUAKE1     := 0x0220;
const SFX_SPELL_EARTHQUAKE2	:= 0x0223;
const FX_SPELL_EARTHQUAKE	:= 0x3728;
const MOVING_SPELL_EARTHQUAKE	:= 0;
const SPEED_SPELL_EARTHQUAKE	:= 7;
const LOOP_SPELL_EARTHQUAKE	:= 7;

//Energy Vortex
const SPELLID_ENERGY_VORTEX	:= 58;
const SFX_SPELL_ENERGY_VORTEX   := 0x0213;
const FX_SPELL_ENERGY_VORTEX	:= 0x3728;
const MOVING_SPELL_ENERGY_VORTEX	:= 0;
const SPEED_SPELL_ENERGY_VORTEX	:= 0xa;
const LOOP_SPELL_ENERGY_VORTEX	:= 0xa;

//Resurrection
const SPELLID_RESURRECTION	:= 59;
const SFX_SPELL_RESURRECTION    := 0x0215;
const FX_SPELL_RESURRECTION	:= 0x3728;
const MOVING_SPELL_RESURRECTION	:= 0;
const SPEED_SPELL_RESURRECTION	:= 0xa;
const LOOP_SPELL_RESURRECTION	:= 0xa;

//Summon Air Elemental
const SPELLID_SUMMON_AIR_ELEMENTAL	:= 60;
const SFX_SPELL_SUMMON_AIR_ELEMENTAL   	:= 0x0218;
const SFX_SPELL_SUMMON_ELEMENTAL	:= 0x0218;
					// No FX

//Summon Daemon
const SPELLID_SUMMON_DAEMON	:= 61;
const SFX_SPELL_SUMMON_DAEMON   := 0x0217;
				// No FX

//Summon Earth Elemental
const SPELLID_SUMMON_EARTH_ELEMENTAL	:= 62;
const SFX_SPELL_SUMMON_EARTH_ELEMENTAL 	:= 0x0218;
					// No FX

//Summon Fire Elemental
const SPELLID_SUMMON_FIRE_ELEMENTAL	:= 63;
const SFX_SPELL_SUMMON_FIRE_ELEMENTAL  	:= 0x0218;
					// No FX

//Summon Water Elemental
const SPELLID_SUMMON_WATER_ELEMENTAL	:= 64;
const SFX_SPELL_SUMMON_WATER_ELEMENTAL 	:= 0x0218;
					// No FX


/////////////////////////////////
// Lesser Chants of Necromancy //
/////////////////////////////////

//Control Undead
const SPELLID_CONTROL_UNDEAD	:= 65;
const SFX_SPELL_CONTROL_UNDEAD	:= 0x216;
const FX_SPELL_CONTROL_UNDEAD	:= 0x37b9;
const MOVING_SPELL_CONTROL_UNDEAD := 0;
const SPEED_SPELL_CONTROL_UNDEAD := 7;
const LOOP_SPELL_CONTROL_UNDEAD	:= 0x10;

//Darkness
const SPELLID_DARKNESS		:= 66;
const SFX_SPELL_DARKNESS	:= 0x01e4;
const FX_SPELL_DARKNESS		:= 0x0373a;
const MOVING_SPELL_DARKNESS	:= 0;
const SPEED_SPELL_DARKNESS	:= 0xA;
const LOOP_SPELL_DARKNESS	:= 0xA;

//Decaying Ray
const SPELLID_DECAYING_RAY	:= 67;
const SFX_SPELL_DECAYING_RAY	:= 0x00fe;
const FX_SPELL_DECAYING_RAY	:= 0x374a;
const MOVING_SPELL_DECAYING_RAY	:= 0;
const SPEED_SPELL_DECAYING_RAY	:= 7;
const LOOP_SPELL_DECAYING_RAY	:= 0x10;

//Specter's Touch
const SPELLID_SPECTER_TOUCH	:= 68;
const SFX_SPELL_SPECTER_TOUCH	:= 0x01f2;
const FX_SPELL_SPECTER_TOUCH	:= 0x37c3;
const MOVING_SPELL_SPECTER_TOUCH := 0;
const SPEED_SPELL_SPECTER_TOUCH	:= 0xa;
const LOOP_SPELL_SPECTER_TOUCH	:= 0x1e;

//Abyssal Flame
const SPELLID_ABYSSAL_FLAME	:= 69;
const SFX_SPELL_ABYSSAL_FLAME	:= 0x0160;
const FX_SPELL_ABYSSAL_FLAME	:= 0x36d4;
const MOVING_SPELL_ABYSSAL_FLAME := 1;
const SPEED_SPELL_ABYSSAL_FLAME	:= 5;
const LOOP_SPELL_ABYSSAL_FLAME	:= 1;

//Animate Dead
const SPELLID_ANIMATE_DEAD	:= 70;
const SFX_SPELL_ANIMATE_DEAD	:= 0x022b;
const FX_SPELL_ANIMATE_DEAD	:= 0x37b9;
const MOVING_SPELL_ANIMATE_DEAD	:= 0;
const SPEED_SPELL_ANIMATE_DEAD	:= 7;
const LOOP_SPELL_ANIMATE_DEAD	:= 0xa;
const ANIMATE_DEAD_CORPSE_COLOR	:= 0x482;

//Sacrifice
const SPELLID_SACRIFICE		:= 71;
const SFX_SPELL_SACRIFICE	:= 0x0208;
const FX_SPELL_SACRIFICE	:= 0x36b0;
const MOVING_SPELL_SACRIFICE	:= 0;
const SPEED_SPELL_SACRIFICE	:= 7;
const LOOP_SPELL_SACRIFICE	:= 0x10;
	//Plus a :		PlayLightningBoltEffect(cast_on);

//Wraith's Breath
const SPELLID_WRAITH_BREATH	:= 72;
const SFX_SPELL_WRAITH_BREATH	:= 0x1fa;
const FX_SPELL_WRAITH_BREATH	:= 0x374a;
const MOVING_SPELL_WRAITH_BREATH := 0;
const SPEED_SPELL_WRAITH_BREATH := 7;
const LOOP_SPELL_WRAITH_BREATH	:= 0x10;



//////////////////////////////////
// Greater Chants of Necromancy //
//////////////////////////////////


//Raise Dead
const SPELLID_SORCERERS_BANE	:= 73;
const SFX_SPELL_SORCERERS_BANE	:= 0x0209;
const FX_SPELL_SORCERERS_BANE	:= 0x3709;
const MOVING_SPELL_SORCERERS_BANE	:= 0;
const SPEED_SPELL_SORCERERS_BANE	:= 0x0a;
const LOOP_SPELL_SORCERERS_BANE	:= 0x1e;

//Summon Spirit
const SPELLID_SUMMON_SPIRIT	:= 74;
const SFX_SPELL_SUMMON_SPIRIT	:= 0x022b;

//Wraithform
const SPELLID_WRAITHFORM	:= 75;
const WRAITHFORM_DRAIN_SOUND	:= 0x1fa;
const WRAITHFORM_MORPH_SOUND	:= 0x210;
const WRAITHFORM_WRAITH_GRAPHIC	:= 0x1a;
const WRAITHFORM_WRAITH_COLOR	:= 0x482;
const WRAITHFORM_DRAIN_EFFECT	:= 0x374a;
const WRAITHFORM_MORPH_EFFECT	:= 0x3709;
const MOVING_SPELL_WRAITHFORM	:= 0;
const SPEED_SPELL_WRAITHFORM	:= 1;
const LOOP_SPELL_WRAITHFORM	:= 0x10;

//Wyvern Strike
const SPELLID_WYVERN_STRIKE	:= 76;
const SFX_SPELL_WYVERN_STRIKE	:= 0x1e2;
const FX_SPELL_WYVERN_STRIKE	:= 0x3709;
const MOVING_SPELL_WYVERN_STRIKE := 1;
const SPEED_SPELL_WYVERN_STRIKE	:= 5;
const LOOP_SPELL_WYVERN_STRIKE	:= 1;

//Kill
const SPELLID_KILL		:= 77;
const SFX_SPELL_KILL		:= 0x202;
const FX_SPELL_KILL		:= 0x375a;
const MOVING_SPELL_KILL 	:= 0;
const SPEED_SPELL_KILL		:= 1;
const LOOP_SPELL_KILL		:= 0x10;

//Liche
const SPELLID_LICHE		:= 78;
const LICHE_KILL_SOUND 		:= 0x202;
const LICHE_MORPH_SOUND 	:= 0x20a;
const FX_SPELL_LICHE	 	:= 0x375a;
const MOVING_SPELL_LICHE 	:= 0;
const SPEED_SPELL_LICHE		:= 1;
const LOOP_SPELL_LICHE		:= 0x10;

//Plague
const SPELLID_PLAGUE		:= 79;
const SFX_SPELL_PLAGUE		:= 0x1e2;
const FX_SPELL_PLAGUE		:= 0x374a;
const MOVING_SPELL_PLAGUE 	:= 0;
const SPEED_SPELL_PLAGUE	:= 7;
const LOOP_SPELL_PLAGUE		:= 0x10;

//Spellbind
const SPELLID_SPELLBIND		:= 80;
const SFX_SPELL_SPELLBIND	:= 0x20d;
const FX_SPELL_SPELLBIND	:= 0x37b9;
const MOVING_SPELL_SPELLBIND	:= 0;
const SPEED_SPELL_SPELLBIND	:= 7;
const LOOP_SPELL_SPELLBIND	:= 0x10;


///////////////////////////////////////
//           Holy Spells             //
///////////////////////////////////////

//Turn Undead
const SPELLID_TURN_UNDEAD		:= 167;
const SFX_SPELL_TURN_UNDEAD		:= 0x0216;
const FX_SPELL_TURN_UNDEAD  		:= 0x376a;
const MOVING_SPELL_TURN_UNDEAD 	:= 0;
const SPEED_SPELL_TURN_UNDEAD 	:= 10;
const LOOP_SPELL_TURN_UNDEAD		:= 10;

//Karma Blast
const SPELLID_KARMA_BLAST		:= 174;
const SFX_SPELL_KARMA_BLAST   	:= 0x0228;
const FX_SPELL_KARMA_BLAST	 	:= 0x375a;
const MOVING_SPELL_KARMA_BLAST 	:= 0;
const SPEED_SPELL_KARMA_BLAST  	:= 7;
const LOOP_SPELL_KARMA_BLAST		:= 0x10;

//Touch of God
const SPELLID_TOUCH_OF_GOD	 := 169;
const SFX_SPELL_TOUCH_OF_GOD	 := 0x0203;
const FX_SPELL_TOUCH_OF_GOD  	 := 0x375a;
const MOVING_SPELL_TOUCH_OF_GOD := 0;
const SPEED_SPELL_TOUCH_OF_GOD  := 7;
const LOOP_SPELL_TOUCH_OF_GOD   := 0x10;

//Holy Bolt
const SPELLID_HOLY_BOLT		:= 170;
const SFX_SPELL_HOLY_BOLT	:= 0x0207;
const FX_SPELL_HOLY_BOLT	:= 0x376a;
const MOVING_SPELL_HOLY_BOLT	:= 0;
const SPEED_SPELL_HOLY_BOLT	:= 10;
const LOOP_SPELL_HOLY_BOLT	:= 10;

//Holy Bless
const SPELLID_HOLY_BLESS	  := 171;
const SFX_SPELL_HOLY_BLESS	  := 0x01eb;
const FX_SPELL_HOLY_BLESS     := 0x373a;
const MOVING_SPELL_HOLY_BLESS := 0;
const SPEED_SPELL_HOLY_BLESS  := 10;
const LOOP_SPELL_HOLY_BLESS	  := 10;

//Angelic Gate
const SPELLID_ANGELIC_GATE	:= 172;
const SFX_SPELL_ANGELIC_GATE	:= 0x020f;

//Great Feast
const SPELLID_ANGELIC_FEAST	:= 166;
const SFX_SPELL_ANGELIC_FEAST	:= 0x01e3;

//Light of Day
const SPELLID_LIGHT_OF_DAY		:= 168;
const SFX_SPELL_LIGHT_OF_DAY		:= 0x01e4;
const FX_SPELL_LIGHT_OF_DAY   	:= 0x373a;
const MOVING_SPELL_LIGHT_OF_DAY 	:= 0;
const SPEED_SPELL_LIGHT_OF_DAY  	:= 10;
const LOOP_SPELL_LIGHT_OF_DAY		:= 10;

//Divine Fury
const SPELLID_DIVINE_FURY		:= 175;
const SFX_SPELL_DIVINE_FURY		:= 0x0209;
const FX_SPELL_DIVINE_FURY		:= 0x37be;
const MOVING_SPELL_DIVINE_FURY	:= 0;
const SPEED_SPELL_DIVINE_FURY		:= 10;
const LOOP_SPELL_DIVINE_FURY		:= 10;
	
//Enlightenment
const SPELLID_ENLIGHTENMENT		:= 177;
const SFX_SPELL_ENLIGHTENMENT		:= 0xF8;
const FX_SPELL_ENLIGHTENMENT		:= 0x375a;
const MOVING_SPELL_ENLIGHTENMENT	:= 0;
const SPEED_SPELL_ENLIGHTENMENT	:= 10;
const LOOP_SPELL_ENLIGHTENMENT	:= 10;	

//Revive
const SPELLID_REVIVE			:= 178;
const SFX_SPELL_REVIVE    		:= 0x0215;
const FX_SPELL_REVIVE			:= 0;
const MOVING_SPELL_REVIVE		:= 0;
const SPEED_SPELL_REVIVE		:= 0;
const LOOP_SPELL_REVIVE			:= 0;	

//Summon Guardian
const SPELLID_SUMMON_GUARDIAN	:= 180;

//Astral Storm
const SPELLID_ASTRAL_STORM		:= 176;
const SFX_SPELL_ASTRAL_STORM		:= 0x17;
const FX_SPELL_ASTRAL_STORM		:= 0x36fe;
const MOVING_SPELL_ASTRAL_STORM	:= 0;
const SPEED_SPELL_ASTRAL_STORM	:= 10;
const LOOP_SPELL_ASTRAL_STORM		:= 10;

//Remove Curse
const SPELLID_REMOVE_CURSE		:= 173;
const SFX_SPELL_REMOVE_CURSE		:= 0x0207;
const FX_SPELL_REMOVE_CURSE 		:= 0x376a;
const MOVING_SPELL_REMOVE_CURSE	:= 0;
const SPEED_SPELL_REMOVE_CURSE	:= 10;
const LOOP_SPELL_REMOVE_CURSE		:= 10;

//Sanctuary
const SPELLID_SANCTUARY		:= 179;
const SFX_SPELL_SANCTUARY	:= 0x01e1;
const FX_SPELL_SANCTUARY   	:= 0x373a;
const MOVING_SPELL_SANCTUARY 	:= 0;
const SPEED_SPELL_SANCTUARY  	:= 10;
const LOOP_SPELL_SANCTUARY	:= 10;
	
//Apocalypse
const SPELLID_APOCALYPSE	:= 181;
const SFX_SPELL_APOCALYPSE := 0x02a;
const MOVING_SPELL_APOCALYPSE	:= 2;

///////////////////////////////////////
//			Songs              //
///////////////////////////////////////

//Song of Light
const SPELLID_SONG_OF_LIGHT := 182;

//Song of Haste
const SPELLID_SONG_OF_HASTE := 183;

//Song of Defense
const SPELLID_SONG_OF_DEFENSE := 184;

//Song of Glory
const SPELLID_SONG_OF_GLORY := 185;

//Song of Cloaking
const SPELLID_SONG_OF_CLOAKING := 186;

//Song of Remedy
const SPELLID_SONG_OF_REMEDY := 187;

//Song of Life
const SPELLID_SONG_OF_LIFE := 188;

//Song of Dismissal
const SPELLID_SONG_OF_DISMISSAL := 189;

//Song of Sirens
const SPELLID_SONG_OF_SIRENS := 190;

//Song of Earth
const SPELLID_SONG_OF_EARTH := 191;

//Song of Air
const SPELLID_SONG_OF_AIR := 192;

//Song of Fire
const SPELLID_SONG_OF_FIRE := 193;

//Song of Water
const SPELLID_SONG_OF_WATER := 194;

//Song of Beckon
const SPELLID_SONG_OF_BECKON := 195;

//Song of Fright
const SPELLID_SONG_OF_FRIGHT := 196;

//Song of Salvation
const SPELLID_SONG_OF_SALVATION := 197;

///////////////////////////////////////
// Lesser 1st circle of Earth Spells //
///////////////////////////////////////

//Antidote
const SPELLID_ANTIDOTE		:= 81;
const SFX_SPELL_ANTIDOTE	:= 0x01e1;
const FX_SPELL_ANTIDOTE   	:= 0x373a;
const MOVING_SPELL_ANTIDOTE 	:= 0;
const SPEED_SPELL_ANTIDOTE  	:= 10;
const LOOP_SPELL_ANTIDOTE	:= 10;

//Owl Sight
const SPELLID_OWL_SIGHT		:= 82;
const SFX_SPELL_OWL_SIGHT	:= 0x01e4;
const FX_SPELL_OWL_SIGHT   	:= 0x373a;
const MOVING_SPELL_OWL_SIGHT 	:= 0;
const SPEED_SPELL_OWL_SIGHT  	:= 10;
const LOOP_SPELL_OWL_SIGHT	:= 10;

//Shifting Earth
const SPELLID_SHIFTING_EARTH	  := 83;
const SFX_SPELL_SHIFTING_EARTH	  := 0x020e;
const FX_SPELL_SHIFTING_EARTH     := 0x3789;
const MOVING_SPELL_SHIFTING_EARTH := 0;
const SPEED_SPELL_SHIFTING_EARTH  := 0xA;
const LOOP_SPELL_SHIFTING_EARTH	  := 0xA;

//Summon Mammals
const SPELLID_SUMMON_MAMMALS	:= 84;


////////////////////////////////////////
// Greater 1st circle of Earth Spells //
////////////////////////////////////////


//Call Lightning
const SPELLID_CALL_LIGHTNING	  := 85;
const SFX_SPELL_CALL_LIGHTNING	  := 0x0207;
const MOVING_SPELL_CALL_LIGHTNING := 3;

//Earth Blessing
const SPELLID_EARTH_BLESSING	  := 86;
const SFX_SPELL_EARTH_BLESSING	  := 0x01eb;
const FX_SPELL_EARTH_BLESSING     := 0x373a;
const MOVING_SPELL_EARTH_BLESSING := 0;
const SPEED_SPELL_EARTH_BLESSING  := 10;
const LOOP_SPELL_EARTH_BLESSING	  := 10;

//Earth Portal
const SPELLID_EARTH_PORTAL	:= 87;
const SFX_SPELL_EARTH_PORTAL	:= 0x020f;

//Nature's Touch
const SPELLID_NATURES_TOUCH	 := 88;
const SFX_SPELL_NATURES_TOUCH	 := 0x0203;
const FX_SPELL_NATURES_TOUCH   	 := 0x375a;
const MOVING_SPELL_NATURES_TOUCH := 0;
const SPEED_SPELL_NATURES_TOUCH  := 7;
const LOOP_SPELL_NATURES_TOUCH	 := 0x10;


///////////////////////////////////////
// Lesser 2nd circle of Earth Spells //
///////////////////////////////////////

//Gust of Air
const SPELLID_GUST_OF_AIR	:= 89;
const SFX_SPELL_GUST_OF_AIR1	:= 0x0108;
const SFX_SPELL_GUST_OF_AIR2	:= 0x0109;
const FX_SPELL_GUST_OF_AIR   	:= 0x37cc;
const MOVING_SPELL_GUST_OF_AIR	:= 4;
const SPEED_SPELL_GUST_OF_AIR	:= 30;
const LOOP_SPELL_GUST_OF_AIR	:= 30;

//Rising Fire
const SPELLID_RISING_FIRE	:= 90;
const SFX_SPELL_RISING_FIRE	:= 0x0209;
const FX_SPELL_RISING_FIRE   	:= 0x3709;
const MOVING_SPELL_RISING_FIRE	:= 0;
const SPEED_SPELL_RISING_FIRE	:= 0x0a;
const LOOP_SPELL_RISING_FIRE	:= 0x1e;

//Shapeshift
const SPELLID_SHAPESHIFT	:= 91;
const SFX_SPELL_SHAPESHIFT	:= 0x020a;
const FX_SPELL_SHAPESHIFT1   	:= 0x1;
const FX_SPELL_SHAPESHIFT2   	:= 0x373a;
const MOVING_SPELL_SHAPESHIFT	:= 0;
const SPEED_SPELL_SHAPESHIFT	:= 10;
const LOOP_SPELL_SHAPESHIFT	:= 10;


////////////////////////////////////////
// Greater 2nd circle of Earth Spells //
////////////////////////////////////////

//Ice Strike
const SPELLID_ICE_STRIKE	:= 92;
const SFX_SPELL_ICE_STRIKE1	:= 0x0117;
const SFX_SPELL_ICE_STRIKE2	:= 0x0118;
const FX_SPELL_ICE_STRIKE	:= 0x3789;
const MOVING_SPELL_ICE_STRIKE	:= 0;
const SPEED_SPELL_ICE_STRIKE	:= 0xA;
const LOOP_SPELL_ICE_STRIKE	:= 0xA;

//Earth Spirir
const SPELLID_EARTH_SPIRIT	:= 93;

//Flame Spirit
const SPELLID_FLAME_SPIRIT	:= 94;

//Storm Spirit
const SPELLID_STORM_SPIRIT	:= 95;

//Water Spirit
const SPELLID_WATER_SPIRIT	:= 96;



////////////////////////////////////
//	Tome of Enchantment       //
////////////////////////////////////

const SPELLID_RITUAL_OF_CHANNEL_MANA	:= 97;
const SPELLID_RITUAL_OF_CONSECRATION	:= 98;
const SPELLID_RITUAL_OF_FREE_MOVEMENT	:= 99;
const SPELLID_RITUAL_OF_QUICK_HEALING	:= 100;
const SPELLID_RITUAL_OF_REGENERATION	:= 101;
const SPELLID_RITUAL_OF_RESILIENCE	:= 102;
const SPELLID_RITUAL_OF_RESTORATION	:= 103;
const SPELLID_RITUAL_OF_VENOM_BANE	:= 104;
const SPELLID_RITUAL_OF_CURSING		:= 105;
const SPELLID_RITUAL_OF_DISENCHANTMENT	:= 106;
const SPELLID_RITUAL_OF_ELEMENTAL_WARD	:= 107;
const SPELLID_RITUAL_OF_MANA_FLUX	:= 108;
const SPELLID_RITUAL_OF_PLANAR_WARD	:= 109;
const SPELLID_RITUAL_OF_SANCTIFYING	:= 110;
const SPELLID_RITUAL_OF_SKILL_INFUSION	:= 111;
const SPELLID_RITUAL_OF_VENOM_MASTERY	:= 112;
const SPELLID_RITUAL_OF_BLOODSEEKING	:= 113;
const SPELLID_RITUAL_OF_HARDENING	:= 114;
const SPELLID_RITUAL_OF_MANA_DIMISSAL	:= 115;
const SPELLID_RITUAL_OF_PHYSICAL_WARD	:= 116;
const SPELLID_RITUAL_OF_PROTECTIVE_AURA	:= 117;
const SPELLID_RITUAL_OF_SPELL_BOUNCING	:= 118;
const SPELLID_RITUAL_OF_SPELL_WARDING	:= 119;
const SPELLID_RITUAL_OF_VITAL_INFUSION	:= 120;
const SPELLID_RITUAL_OF_ADVANCED_THEURGY := 121;
const SPELLID_RITUAL_OF_ATTUNEMENT	:= 122;
const SPELLID_RITUAL_OF_BASIC_THEURGY	:= 123;
const SPELLID_RITUAL_OF_ENHANCEMENT	:= 124;
const SPELLID_RITUAL_OF_IMMUTABILITY	:= 125;
const SPELLID_RITUAL_OF_PERILOUS_THEURGY := 126;
const SPELLID_RITUAL_OF_PURIFICATION	:= 127;
const SPELLID_RITUAL_OF_RACIAL_THEURGY	:= 128;


////////////////////////
//      Highest       //
////////////////////////

const MAX_NORMAL_SPELL		:= 64;
const MAX_NECRO_SPELL		:= 80;
const MAX_EARTH_SPELL		:= 96;
const MAX_RITUAL_SPELL		:= 128;
const SPELLID_HIGHEST		:= 197;
const MAX_HOLY_SPELL		:= 181;
const MAX_SONG_SPELL		:= 197;



//////////////////////////////////////END OF SPELLS///////////////////////////////////