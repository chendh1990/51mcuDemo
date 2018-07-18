#ifndef __SMS_STRING_H
#define __SMS_STRING_H
//////////////////////////
//{
//English 
//French
//Russian
//German
//Chinese
//}
///////////////////////////////////////////////////////////////////////////////////////////////

code unsigned char * code String_OK[]=
{
	"004F004B",
	"004F004B",	
	"041E041A",
//	"004F004B",
	"8BBE7F6E6210529FFF01",
};

///////////////////////////////////////////////////////
code unsigned char * code String_System_Disarmed[]=
{
	"00530079007300740065006D002000640069007300610072006D00650064002E",
	"005300790073007400E8006D00650020006400E9007300610072006D00E9002E",	
	"04210438044104420435043C043000200432044B043A043B044E04470435043D0430002E",
//	"00530079007300740065006D0020006400650061006B0074006900760069006500720074002E",
	"7CFB7EDF5DF264A49632",
};

code unsigned char * code String_System_armed[]=
{
	"00530079007300740065006D002000610072006D00650064002E",
	"005300790073007400E8006D0065002000610072006D00E9002E",	
	"04210438044104420435043C043000200432043A043B044E04470435043D0430002E",
//	"00530079007300740065006D00200061006B0074006900760069006500720074002E",
	"7CFB7EDF5DF25E039632",
};

code unsigned char * code String_System_In_Home_armed[]=
{
	"00530079007300740065006D00200069006E00200048006F006D00650020004D006F00640065002E",
	"005300790073007400E8006D006500200065006E0020004D006F006400650020004D006100690073006F006E002E",	
	"04210438044104420435043C0430002020130020043200200440043504360438043C043500200414043E043C002E",
//	"00530079007300740065006D00200069006E00200048006F006D00650020004D006F006400750073002E",
	"7CFB7EDF57285BB66A215F0F5E039632",
};
//////////////////////////////////////////////////////
code unsigned char * code String_Operation_failure[]=
{
	"004F007000650072006100740069006F006E0020006600610069006C007500720065",
	"004F007000650072006100740069006F006E0020006600610069006C007500720065",//HJC ??	
	"041D04350442",///hjc  ???
	"004F007000650072006100740069006F006E0020006600610069006C007500720065",//???hjc
};

code unsigned char * code String_System[]=
{
	"00530079007300740065006D",
	"005300790073007400E8006D0065",	
	"04210438044104420435043C0430",
//	"00530079007300740065006D",
	"7CFB7EDF72B66001",
};

code unsigned char * code String_Armed[]=
{
	"00410072006D00650064",
	"00610072006D00E9",	
	"0432043A043B044E04470435043D0430",
//	"0061006B0074006900760069006500720074",
	"5E039632",
};

code unsigned char * code String_Disarmed[]=
{
	"00440069007300610072006d00650064",
	"006400E9007300610072006D00E9",	
	"0412042B041A041B",
//	"004400650061006B0074006900760069006500720074",
	"64A49632",
};

code unsigned char * code String_Home_armed[]=
{
	"0048006F006D00650020006D006F00640065",
	"00410072006D00E90020006D006F006400650020006D006100690073006F006E",	
	"0414043E043C002E0440043504360438043C00200412041A041B",
//	"004800650069006D00200061006B0074006900760069006500720074",
	"57285BB66A215F0F",
};


code unsigned char * code String_AC_power[]=
{
	"0041004300200070006F007700650072",
	"0041006C0069006D0065006E0074006100740069006F006E00200073006500630074006500750072",	
	"041F043804420430043D04380435",
//	"005300740072006F006D",
	"75356E9072B66001",
};

code unsigned char * code String_AC_failure[]=
{
	"0041004300200050006F0077006500720020004600610069006C007500720065",
	"00500061006E006E006500200063006F007500720061006E0074",	
	"041D043504420020043F043804420430043D0438044F002000410043",
//	"005300740072006F006D00610075007300660061006C006C",
	"4E3B673A75356E9065AD5F008FDE63A5",
};

code unsigned char * code String_AC_restored[]=
{
	"0041004300200050006F00770065007200200052006500730074006F00720065",
	"0043006F007500720061006E007400200072006500730074",	
	"0412043E0437043E0431043D002E0020043F043804420430043D0438044F",
//	"005300740072006F006D0077006900650064006500720068006500720073",
	"4E3B673A75356E906062590D8FDE63A5",
};

code unsigned char * code String_on[]=
{
	"006F006E",
	"006F00750069",	
	"0412041A041B",
//	"00650069006E006700650073006300680061006C007400650074",
	"8FDE63A5",
};

code unsigned char * code String_off[]=
{
	"006F00660066",
	"006E006F006E",	
	"0412042B041A041B",
//	"006100750073",
	"65AD5F00",
};

code unsigned char * code String_Phone_numbers[]=
{
	"00500068006F006E00650020006E0075006D0062006500720073",
	"004E0075006D00E90072006F00730020006400650020007400E9006C00E900700068006F006E0065",	
	"04220435043B002E",
//	"00540045004C",
	"50A85B5862A58B6675358BDD53F77801",
};

code unsigned char * code String_SMS_Numbers[]=
{
	"0053004D00530020006E0075006D0062006500720073",
	"004E0075006D00E90072006F007300200053004D0053",
	"0053004D0053",
//	"0053004D0053",
	"5B5850A877ED4FE1901A77E575358BDD53F77801",
};

code unsigned char * code String_Zones_Names[]=
{
	"005A006F006E006500730020006E0061006D00650073",
	"004E006F006D007300200064006500730020007A006F006E00650073",
	"041D0430043704320430043D0438044F00200437043E043D044B",
//	"005A006F006E0065006E006E0061006D0065",
	"9632533A547D540D",
};

code unsigned char * code String_RFID_Tags[]=
{
	"00520065006E0061006D00650020005200460049004400200074006100670073",
	"00520065006E006F006D006D006500720020006C00650073002000620061006400670065007300200052004600490044",	
	"04180437043C0435043D04380442044C0020043D0430043704320430043D043804350020043E0442043C04350442043A043800200052004600490044",
//	"00520046004900440020005400610067007300200075006D00620065006E0065006E006E0065006E",
	"005200460049004400200054006100670020547D540D",
};

code unsigned char * code String_Entry_Delay_Time[]=
{
	"0045006E007400720079002000640065006C00610079002000740069006D006500280030002D0033003000300073006500630029",
	"004400E9006C006100690020006400270065006E0074007200E9006500280030002D0033003000300073006500630029",
	"041E0436043804340430043D043804350020043F04400438002004320445043E0434043500280030002D00330030003004410435043A002E0029",
//	"00450069006E00670061006E00670020005600650072007A00F60067006500720075006E00670073007A00650069007400280030002D00330030003000730029",
	"62A58B665EF665F600280030002D00330030003079D20029",
};

code unsigned char * code String_Exit_Delay_Time[]=
{
	"0045007800690074002000640065006C00610079002000740069006D006500280030002D0033003000300073006500630029",
	"004400E9006C0061006900200064006500200073006F007200740069006500280030002D0033003000300073006500630029",
	"041E0436043804340430043D043804350020043F0440043800200432044B0445043E0434043500280030002D00330030003004410435043A002E0029",
//	"00410075007300670061006E00670020005600650072007A00F60067006500720075006E00670073007A00650069007400280030002D00330030003000730029",
	"5E0396325EF665F600280030002D00330030003079D20029",
};

code unsigned char * code String_Disarmed_Password[]=
{
	"00440069007300610072006D002000700061007300730077006F007200640028003400200064006900670069007400730029",
	"0043006F006400650020006400650020006400E9007300610072006D0065006D0065006E0074002800340020006300680069006600660072006500730029",	
	"041F04300440043E043B044C00200434043B044F00200432044B043A043B044E04470435043D0438044F002000280034002004460438044404400029",
//	"004400650061006B0074006900760069006500720075006E0067007300700061007300730077006F00720074002800340020005A00690066006600650072006E0029",
	"64A496325BC67801002800344F4D0029",
};

code unsigned char * code String_Siren_volume[]=
{
	"0053006900720065006E00200076006F006C0075006D006500280030003D004D007500740065002C0031003D00480069006700680029",
	"0056006F006C0075006D00650020006400650020006C0061002000730069007200E8006E006500280030003D006D007500650074002C0031003D0066006F007200740029",	
	"04130440043E043C043A043E04410442044C00200441043804400435043D044B00280030003D0432044B043A043B002E002C0031003D04330440043E043C043A043E0029",
//	"0053006900720065006E0065006E006C0061007500740073007400E40072006B006500280030003D007300740075006D006D002C0031003D0068006F006300680029",
	"8B6694C300280030003D5173002C0031003D5F000029",
};

code unsigned char * code String_Siren_Ring_Time[]=
{
	"0053006900720065006E002000720069006E00670069006E0067002000740069006D006500280031002D0039006D0069006E0029",
	"00440075007200E900650020006400650020006C0061002000730069007200E8006E006500280031002D0039006D0069006E0029",	
	"041F0440043E0434043E043B043604380442002D0442044C00200441043804400435043D044B002000280031002D0039043C0438043D002E0029",
//	"0053006900720065006E0065005F0054006F006E0064006100750065007200280031002D0039006D0069006E0029",
	"9E2354CD65F695F400280031002D00395206949F0029",
};

code unsigned char * code String_Zone[]=
{
	"005A006F006E0065002D",
	"005A006F006E0065002D",	
	"0417043E043D0430002D",
//	"005A006F006E0065006E002D",
	"9632533A",
};

code unsigned char * code String_Alarm[]=
{
	"0041006C00610072006D",
	"0041006C00610072006D0065",	
	"0422044004350432043E04330430",
//	"0041006C00610072006D",
	"89E653D162A58B66",
};

code unsigned char * code String_Tamper[]=
{
	"00540061006D007000650072",
	"005300610062006F0074006100670065",	
	"04120441043A0440044B044204380435",
//	"005300610062006F0074",
	"963262C6",
};

code unsigned char * code String_Low_BAT[]=
{
	"004C006F00770020004200410054",
	"0042006100740074002E00200066006100690062006C0065",	
	"041D04380437043A002E004200410054",
//	"0042004100540020006E00690065",
	"75356C604F4E538B",
};

code unsigned char * code String_Wired_Z[]=
{
	"005700690072006500640020005A",
	"0041006C00610072006D00650020005A",	
	"041F0440043E0432043E0434002E005A",
//	"005600650072006B00610062002E0020005A",
	"67097EBF9632533A",
};

code unsigned char * code String_RC[]=
{
	"00520043002D",
	"00520043002D",
	"00520043002D",
//	"00520043002D",
	"906563A75668002D",
};

code unsigned char * code String_SOS[]=
{
	"0053004F0053",
	"0053004F0053",
	"0053004F0053",
//	"0053004F0053",
	"0053004F00537D27602562A58B66",
};

code unsigned char * code String_Host_Low_BAT[]=
{
	"0048006F007300740020004C006F00770020004200410054",
	"0042006100740074006500720069006500200066006100690062006C0065",	
	"041D04380437043A002E044104380433043D0430043B0020004200410054",
//	"0048006F0073007400200042004100540020006C006500650072",
	"4E3B673A75356C604F4E538B",
};

code unsigned char * code String_Host_Tamper[]=
{
	"0048006F00730074002000540061006D007000650072",
	"005300610062006F0074006100670065002000700061006E006E006500610075",	
	"0413043B002E0434043004420447002E04320441043A0440002D044F",
//	"0048006F007300740020005300610062006F0074006100670065",
	"4E3B673A963262C662A58B66",
};

code unsigned char * code String_tag[]=
{
	"007400610067002D",
	"00420061006400670065002D",	
	"007400610067002D",
//	"007400610067002D",
	"005200460049004400200054006100670020"
};

code unsigned char * code String_Host_SOS[]=
{
	"0048006F0073007400200053004F0053",
	"0053004F0053",
	"0053004F0053",
//	"0048006F0073007400200053004F0053",
	"4E3B673A0053004F00537D27602562A58B66",
};
////////////////////////////////////////////////////////
code unsigned char * code String_Digit_Unicode[]=
{
	"0030", "0031", "0032", "0033", "0034", "0035", "0036", "0037", "0038", "0039", 
};

code unsigned char * code String_Space=
{
	"0020",
};

code unsigned char * code String_Colon=
{
	"003A",
};

code unsigned char * code String_Dot=
{
	"002E",
};

code unsigned char * code String_Enter=
{
	"000A",
};

#endif 

