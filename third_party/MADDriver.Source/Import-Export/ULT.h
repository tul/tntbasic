/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#if defined(powerc) || defined(__powerc) || defined(__APPLE__)
#pragma options align=mac68k
#endif

typedef struct ULTEvt
{
	Byte			note;
	Byte			ins;
	Byte			eff1;
	Byte			eff2;
	short		var;
} ULTEvt;

typedef struct ULTIns
{
	char		name[ 32];
	char		dosname[ 12];
	long		loopStart;
	long		loopEnd;
	long		sizeStart;
	long		sizeEnd;
	Byte		volume;
	Byte		Bidi;
	short	finetune;
} ULTIns;

typedef struct ULTForm
{
	char				ID[ 15];
	char             		name[ 32];
	char             		reserved;
} ULTForm;

typedef struct ULTSuite
{
	Byte  			NOS;
	ULTIns			*ins;
	
	//-----------
	
	Byte				pattSeq[ 256];
	Byte				NOC;
	Byte				NOP;
} ULTSuite;
#if defined(powerc) || defined(__powerc) || defined(__APPLE__)
#pragma options align=reset
#endif
