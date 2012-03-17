
/*  A Bison parser, made from basic.yacc
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse basicparse
#define yylex basiclex
#define yyerror basicerror
#define yylval basiclval
#define yychar basicchar
#define yydebug basicdebug
#define yynerrs basicnerrs
#define YYLSP_NEEDED

#define	REM	257
#define	INTEGER	258
#define	STRLITERAL	259
#define	LABEL	260
#define	FLOAT	261
#define	PROCIDENTIFIER	262
#define	LABELIDENTIFIER	263
#define	INTIDENTIFIER	264
#define	STRIDENTIFIER	265
#define	FLOATIDENTIFIER	266
#define	UNDECLAREDPRIMITIVE	267
#define	INTARRAYIDENTIFIER	268
#define	STRARRAYIDENTIFIER	269
#define	FLOATARRAYIDENTIFIER	270
#define	UNDECLAREDARRAY	271
#define	PLUS	272
#define	MINUS	273
#define	DIVIDE	274
#define	TIMES	275
#define	POWER	276
#define	OPENBRACKET	277
#define	CLOSEBRACKET	278
#define	UMINUS	279
#define	EQUALS	280
#define	LT	281
#define	GT	282
#define	LTE	283
#define	GTE	284
#define	COMMA	285
#define	ENDLINE	286
#define	NEQ	287
#define	OPENSQBRACKET	288
#define	CLOSESQBRACKET	289
#define	SEMICOLON	290
#define	IF	291
#define	ELSEIF	292
#define	ELSE	293
#define	ENDIF	294
#define	AND	295
#define	OR	296
#define	NOT	297
#define	WHILE	298
#define	THEN	299
#define	INTEGERTYPE	300
#define	FLOATTYPE	301
#define	STRINGTYPE	302
#define	SHARED	303
#define	GLOBAL	304
#define	NETGLOBAL	305
#define	IMPORTANTNETGLOBAL	306
#define	DATA	307
#define	READ	308
#define	RESTORE	309
#define	WEND	310
#define	REPEAT	311
#define	UNTIL	312
#define	FOREVER	313
#define	FOR	314
#define	TO	315
#define	STEP	316
#define	NEXT	317
#define	GOTO	318
#define	DIM	319
#define	PROCEDURE	320
#define	ENDPROC	321
#define	TNTB_TRUE	322
#define	TNTB_FALSE	323
#define	END	324
#define	BREAK	325
#define	PRINT	326
#define	GRAPHICSMODE	327
#define	GRAPHICSMODEHARDWARE	328
#define	TEXTMODE	329
#define	BUTTON	330
#define	WINDOWMODE	331
#define	WINDOWMODEHARDWARE	332
#define	STATEMENTNAME	333
#define	ARITHFUNCTIONNAME	334
#define	STRFUNCTIONNAME	335
#define	SPRITE	336
#define	SYNCSCREEN	337
#define	VBLSYNCON	338
#define	VBLSYNCOFF	339
#define	FRAMERATE	340
#define	FRAMERATEMAX	341
#define	DRAWPICTRES	342
#define	PASTESPRITE	343
#define	PASTEIMAGE	344
#define	ISPRESSED	345
#define	PROCRESULT	346
#define	PROCRESULTSTR	347
#define	BEEP	348
#define	FADEUP	349
#define	FADEDOWN	350
#define	FADETO	351
#define	HIDEMOUSE	352
#define	SHOWMOUSE	353
#define	PROCRESULTFLOAT	354
#define	GETMOUSEX	355
#define	GETMOUSEY	356
#define	LOADSOUNDS	357
#define	PLAYSOUND	358
#define	LOOPSOUND	359
#define	SOUNDRESULT	360
#define	STOPSOUND	361
#define	STOPCHANNEL	362
#define	SOUNDPLAYING	363
#define	CHANNELPLAYING	364
#define	UNLOADSOUNDS	365
#define	MOVESOUND	366
#define	MOVECHANNEL	367
#define	SOUNDVOLUME	368
#define	SETSOUNDVOLUME	369
#define	SPRITEOFF	370
#define	INITNETWORKING	371
#define	HOSTNETGAME	372
#define	JOINNETGAME	373
#define	GETNETPLAYERNAME	374
#define	COUNTNETPLAYERS	375
#define	GETNETID	376
#define	LEAVENETGAME	377
#define	MORENETEVENTS	378
#define	GETNETEVENTTYPE	379
#define	GETNETEVENTCONTENT	380
#define	NETGAMETERMINATED	381
#define	NETPLAYERLEFT	382
#define	NETPLAYERDISCONNECTED	383
#define	NETCONNECTIONLOST	384
#define	SETNETDATA	385
#define	UPDATENETDATA	386
#define	SETNETWORKINGNORMAL	387
#define	SETNETWORKINGREGISTERED	388
#define	COUNTRESOURCES	389
#define	GETRESOURCENAME	390
#define	RESOURCEEXISTS	391
#define	LOADIMAGES	392
#define	UNLOADIMAGES	393
#define	SETSPRITEBANK	394
#define	SETDEFAULTSPRITEBANK	395
#define	SETSPRITEPRIORITY	396
#define	LOADINPUTBANK	397
#define	POLLINPUT	398
#define	EDITINPUT	399
#define	SUSPENDINPUT	400
#define	RESUMEINPUT	401
#define	LOADMUSIC	402
#define	PLAYMUSIC	403
#define	STOPMUSIC	404
#define	GETMUSICLENGTH	405
#define	GETMUSICPOSITION	406
#define	SETMUSICPOSITION	407
#define	MUSICVOLUME	408
#define	SETMUSICVOLUME	409
#define	MUSICLOOPON	410
#define	MUSICLOOPOFF	411
#define	ISMUSICPLAYING	412
#define	DRAWTEXT	413
#define	TEXTFONT	414
#define	TEXTSIZE	415
#define	TEXTFACE	416
#define	NUMTOSTRING	417
#define	STRINGTONUM	418
#define	TEXTWIDTH	419
#define	TEXTHEIGHT	420
#define	TEXTDESCENT	421
#define	OPENCANVAS	422
#define	CLOSECANVAS	423
#define	TARGETCANVAS	424
#define	PAINTCANVAS	425
#define	COPYCANVAS	426
#define	CANVASFX	427
#define	SPRITECOL	428
#define	SPRITECOLRECT	429
#define	POINTINRECT	430
#define	WAITMOUSEDOWN	431
#define	WAITMOUSEUP	432
#define	WAITMOUSECLICK	433
#define	UP	434
#define	DOWN	435
#define	LEFT	436
#define	RIGHT	437
#define	SPACE	438
#define	SETSPRITETRANSPARENCY	439
#define	GETSPRITETRANSPARENCY	440
#define	SPRITECOLOUROFF	441
#define	SPRITECOLOUR	442
#define	LOADTNTMAP	443
#define	UNLOADTNTMAP	444
#define	SETCURRENTTNTMAP	445
#define	GETTNTMAPWIDTH	446
#define	GETTNTMAPHEIGHT	447
#define	GETTNTMAPLAYERS	448
#define	COUNTOBJECTS	449
#define	GETNTHOBJECTNAME	450
#define	GETNTHOBJECTTYPE	451
#define	GETNTHOBJECTX	452
#define	GETNTHOBJECTY	453
#define	GETNTHOBJECTZ	454
#define	COUNTPOLYGONS	455
#define	INPOLYGON	456
#define	POLYNAME	457
#define	POLYBOUNDSX	458
#define	POLYBOUNDSY	459
#define	POLYBOUNDSWIDTH	460
#define	POLYBOUNDSHEIGHT	461
#define	GETTILE	462
#define	DRAWMAPSECTION	463
#define	SETMAPTILE	464
#define	GETMAPTILEWIDTH	465
#define	GETMAPTILEHEIGHT	466
#define	MAPLINECOL	467
#define	LINE	468
#define	FILLRECT	469
#define	FRAMERECT	470
#define	FILLOVAL	471
#define	FRAMEOVAL	472
#define	FILLPOLY	473
#define	FRAMEPOLY	474
#define	SETPIXELCOLOUR	475
#define	GETPIXEL	476
#define	SETPENCOLOUR	477
#define	GETPENCOLOUR	478
#define	SETPENTRANSPARENCY	479
#define	GETPENTRANSPARENCY	480
#define	GETREDCOMPONENT	481
#define	GETGREENCOMPONENT	482
#define	GETBLUECOMPONENT	483
#define	SETREDCOMPONENT	484
#define	SETGREENCOMPONENT	485
#define	SETBLUECOMPONENT	486
#define	ABS	487
#define	MOD	488
#define	SIN	489
#define	COS	490
#define	TAN	491
#define	INVSIN	492
#define	INVCOS	493
#define	INVTAN	494
#define	RADSIN	495
#define	RADCOS	496
#define	RADTAN	497
#define	INVRADSIN	498
#define	INVRADCOS	499
#define	INVRADTAN	500
#define	CALCULATEANGLE	501
#define	ANGLEDIFFERENCE	502
#define	DEGTORAD	503
#define	RADTODEG	504
#define	SQUAREROOT	505
#define	MIN	506
#define	MAX	507
#define	WRAP	508
#define	BITAND	509
#define	BITOR	510
#define	BITXOR	511
#define	BITTEST	512
#define	SETBIT	513
#define	BITSHIFT	514
#define	CEIL	515
#define	ROUND	516
#define	FLOOR	517
#define	RANDOM	518
#define	SETRANDOMSEED	519
#define	GETTIMER	520
#define	DELAY	521
#define	INVALRECT	522
#define	NEWBUTTON	523
#define	CLEARBUTTONS	524
#define	WAITBUTTONCLICK	525
#define	POLLBUTTONCLICK	526
#define	GETBUTTONCLICK	527
#define	CREATEVIEWPORT	528
#define	CREATEMAPVIEWPORT	529
#define	REMOVEVIEWPORT	530
#define	VIEWPORTOFFSET	531
#define	GETVIEWPORTXOFFSET	532
#define	GETVIEWPORTYOFFSET	533
#define	GETVIEWPORTWIDTH	534
#define	GETVIEWPORTHEIGHT	535
#define	DRAWTRACKINGON	536
#define	DRAWTRACKINGOFF	537
#define	MAKECOLOUR	538
#define	NTHRESOURCEID	539
#define	LOWERCASE	540
#define	UPPERCASE	541
#define	STRINGLENGTH	542
#define	REMOVECHAR	543
#define	GETCHAR	544
#define	LEFTSTR	545
#define	RIGHTSTR	546
#define	MIDSTR	547
#define	COUNTIMAGES	548
#define	IMAGEWIDTH	549
#define	IMAGEHEIGHT	550
#define	IMAGEXOFF	551
#define	IMAGEYOFF	552
#define	SETIMAGEOFFSETS	553
#define	WHITE	554
#define	BLACK	555
#define	RED	556
#define	GREEN	557
#define	BLUE	558
#define	YELLOW	559
#define	COPYIMAGE	560
#define	DELETEIMAGE	561
#define	IMAGECOLLISIONTOLERANCE	562
#define	IMAGECOLLISIONTYPE	563
#define	SETIMAGECOLLISION	564
#define	CREATEFILE	565
#define	FILEOPEN	566
#define	ASKCREATEFILE	567
#define	ASKUSERFILEOPEN	568
#define	FILECLOSE	569
#define	FILEWRITE	570
#define	FILEREADSTRING	571
#define	FILEREADFLOAT	572
#define	FILEREADINT	573
#define	NEWFOLDER	574
#define	DELETEFILE	575
#define	FILEPOS	576
#define	SETFILEPOS	577
#define	FILELENGTH	578
#define	FILEEXISTS	579
#define	FILETYPE	580
#define	SETDIRECTORYSYSTEMPREFERENCES	581
#define	SETDIRECTORYUSERPREFERENCES	582
#define	SETDIRECTORYTEMPORARY	583
#define	SETDIRECTORYGAME	584
#define	SPRITEX	585
#define	SPRITEY	586
#define	SPRITEI	587
#define	MOVESPRITE	588
#define	SPRITEBANK	589
#define	SPRITEMOVING	590
#define	SPRITEANIMATING	591
#define	ANIMSPRITE	592
#define	SETMOANPROG	593
#define	ADDSPRITETOCHAN	594
#define	ADDVIEWPORTTOCHAN	595
#define	CHANMOVE	596
#define	CHANANIM	597
#define	REMOVESPRITEFROMCHAN	598
#define	REMOVEVIEWPORTFROMCHAN	599
#define	AUTOMOANON	600
#define	AUTOMOANOFF	601
#define	AUTOMOANING	602
#define	STEPMOAN	603
#define	MOVEVIEWPORT	604
#define	VIEWPORTMOVING	605
#define	PAUSECHANNEL	606
#define	UNPAUSECHANNEL	607
#define	CHANPAUSED	608
#define	RESETCHAN	609
#define	CHANREG	610
#define	SETCHANREG	611
#define	RESTARTCHAN	612
#define	CHANPLAY	613
#define	SETSPRITEANGLE	614
#define	SPRITEANGLE	615
#define	SETSPRITEXSCALE	616
#define	SPRITEXSCALE	617
#define	SETSPRITEYSCALE	618
#define	SPRITEYSCALE	619
#define	SCALESPRITE	620
#define	SPRITESCALING	621
#define	SETSPRITEXFLIP	622
#define	SETSPRITEYFLIP	623
#define	SPRITEXFLIP	624
#define	SPRITEYFLIP	625
#define	ROTATESPRITE	626
#define	SPRITEROTATING	627
#define	SPRITEACTIVE	628
#define	SPRITECROP	629
#define	SPRITECROPOFF	630
#define	DISABLEBREAK	631
#define	READRAWKEY	632
#define	RAW2ASCII	633
#define	RAW2SCANCODE	634
#define	RAW2STR	635
#define	CHAR2ASCII	636
#define	ASCII2CHAR	637
#define	KEYBOARDREPEATON	638
#define	KEYBOARDREPEATOFF	639
#define	GETKEYBOARDREPEAT	640
#define	CLEARKEYBOARD	641
#define	WAITKEYPRESS	642
#define	RAWMODSHIFT	643
#define	RAWMODCOMMAND	644
#define	RAWMODOPTION	645
#define	RAWMODCAPS	646
#define	RAWMODCONTROL	647
#define	RAWFILTNUMBER	648
#define	RAWFILTPRINT	649
#define	RAWFILTNAV	650
#define	RAWFILTDEL	651
#define	STREDITORCONTENTS	652
#define	SETSTREDITORCONTENTS	653
#define	STREDITORCURSOR	654
#define	SETSTREDITORCURSOR	655
#define	STREDITORINPUT	656

#line 1 "basic.yacc"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Basic.yacc
// © Mark Tully 1999-2000
// 21/12/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or other
*   materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
* ***** END LICENSE BLOCK ***** */

//#define		YYDEBUG		1				// to enable debugging set this to 1 and set bascidebug in main() to true

#define		free(x) std::free(x)		// The bison generated code needs this to compile in cw

#include	"BisonTokenLocation.h"

#define		YYLTYPE		SBisonTokenLoc

#include	"UTBException.h"
#include	"CArithExpression.h"
#include	"CStrExpression.h"
#include	"CGeneralExpression.h"
#include	"UFunctionMgr.h"

#include	"Drawing_Statements.h"
#include	"Networking_Statements.h"	// needed for networking str functions
#include	"Misc_Statements.h"			// needed for resource str functions
#include	"Data_Statements.h"
#include	"Procedural_Statements.h"

#include	"CBreakBlockStatement.h"
#include	"CForStatement.h"
#include	"CJumpStatement.h"
#include	"CConditionalBlock.h"
#include	"CStatementBlock.h"
#include	"CProgram.h"
#include	"CLabelTable.h"
#include	"UCString.h"
#include	"CProcedure.h"
#include	"CPrimitiveStorage.h"
#include	"CVariableSymbol.h"

void AppendEndIfBreaks(
	CStatement			*inStatementList);
CStatement *ExtractInitStatements(
	CVariableNode		*inList);
void FlushExp();
void FlushList();
bool CheckArrayIndexList(
	CProgram			&inProgram,
	CGeneralExpression	*inExp,
	const STextRange	&inListTerminatingLoc);		
CStatement *MakeAssignmentStatement(
	const CPrimitiveStorage	&inLValue,
	CGeneralExpression	*inRValue,
	const STextRange	&inRValueLoc);
CArithExpression *CheckSingleNumExp(
	CGeneralExpression *&inExp,
	const STextRange	&inRange);
int yyerror(
	char *s);
		
// yylex must be declared as external here to allow linker to correctly link up the lexer and parser
extern int yylex();

// Extern the line number so that the parser knows what line an error has occured on
extern int gLineno;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CheckSingleNumExp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the single arith exp if available, if not it logs the error. It always deletes the general expression. Only
// checks the first element, ignores the rest of the list if it exists.
CArithExpression *CheckSingleNumExp(
	CGeneralExpression *&inExp,
	const STextRange	&inRange)
{
	if (CGeneralExpressionErr::ContainsErrors(inExp))
	{
		// assume errors already reported
		delete inExp;
		inExp=0;
		return 0;
	}	
		
	CArithExpression	*exp=NULL;
	
	if (inExp)
	{		
		exp=inExp->ReleaseArithExp();	
		delete inExp;
		inExp=0;
	}
	
	if (!exp)
		LogException_(UTBException::ThrowWithRange(inRange,kTBErr_ExpectedNumExp));
	
	return exp; // <<-- Can be nil
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CheckArrayIndexList
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if the list is a valid comma seperated list. Logs errors and DELETES THE LIST if not.
bool CheckArrayIndexList(
	CProgram			&inProgram,
	CGeneralExpression	*inExp,
	const STextRange	&inListTerminatingLoc)
{
	if (CGeneralExpressionErr::ContainsErrors(inExp))
	{
		delete inExp;
		return false;
	}

	STextRange		range=inListTerminatingLoc;

	Try_
	{
		if (!inExp)
			UTBException::ThrowExpectedNumExp(range);

		for (CGeneralExpression *exp=inExp; exp; exp=exp->TailData())
		{
			// check for number
			if (exp->IsNullExp())	// empty expression?
			{
				if (exp->GetSeperator()!=CGeneralExpression::kUnspecified)
					range=exp->GetSepLoc();
				UTBException::ThrowExpectedNumExp(range);
			}
			else if (!exp->GetArithExp())
			{
				exp->GetExpLoc(range);
				UTBException::ThrowExpectedNumExp(range);
			}
				
			// check for comma
			switch (exp->GetSeperator())
			{
				case CGeneralExpression::kComma:
				case CGeneralExpression::kUnspecified:
					break;
				default:
					UTBException::ThrowExpectedComma(exp->GetSepLoc());
					break;
			}
		}
	}
	Catch_(err)
	{
		delete inExp;
		inProgram.LogError(err);
		return false;
	}
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ExtractInitStatements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Extracts all the initialization statements from a variable list and returns them
CStatement *ExtractInitStatements(
	CVariableNode		*inList)
{
	CStatement		*head=0;
	
	for (CVariableNode *node=inList; node; node=node->TailData())
	{
		head=ConsData(head,node->mInitStatement);
		node->mInitStatement=0L;
	}
	
	return head;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AppendEndIfBreaks										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine takes a list of CConditionalStatementBlocks. For each one it appends a CBreakBlockStatement to the sub
// statement list. This causes the
// if statement to be exited when one of the conditional statement blocks executes, thus stopping any of the following
// conditional statement blocks or the else case being executed. The else case has not been added to inStatementList yet, it
// is just the if and elseifs.
void AppendEndIfBreaks(
	CStatement			*inStatementList)
{	
	for (CStatementBlock *block=dynamic_cast<CStatementBlock*>(inStatementList); block; block=dynamic_cast<CStatementBlock*>(block->TailData()))
	{
		CStatement		*breaker=new CBreakBlockStatement(2);		// break out of two blocks - the condititional and the container block

		UTBException::ThrowIfNoParserMemory(breaker);

		// Now append break to the end of the list
		block->AppendSubStatement(breaker);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeAssignmentStatement								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Makes an assignment statement. Performs type checking on data, if inRValue is null it doesn't create the statement but
// doens't throw an error either. inRValue is adopted by this function.
CStatement *MakeAssignmentStatement(
	const CPrimitiveStorage	&inLValue,
	CGeneralExpression	*inRValue,
	const STextRange	&inRValueLoc)
{
	if (!inRValue)
		return 0;	// earlier parse error screwed rhs of assignment, can't initialize but error will have been logged
		
	StGenExpression				del1(inRValue);
	
	CStatement	*result=0;
	
	// Perform the type check
	switch (inLValue.GetType())
	{
		case CVariableSymbol::kUndeclaredPrimitive:	// can't type check this
			break;
		
		case CVariableSymbol::kInteger:
		case CVariableSymbol::kFloat:
			if (!inRValue->GetArithExp())
			{
				// type error
				UTBException::ThrowExpectedNumExp(inRValueLoc);
			}
			else
			{
				if (inLValue.GetType()==CVariableSymbol::kInteger)
					result=new CIntAssignmentStatement(inLValue.mIntStore,inRValue->ReleaseArithExp());
				else
					result=new CFloatAssignmentStatement(inLValue.mFloatStore,inRValue->ReleaseArithExp());					
			}
			UTBException::ThrowIfNoParserMemory(result);
			break;
			
		case CVariableSymbol::kString:
			if (!inRValue->GetStrExp())
			{
				// type error
				UTBException::ThrowExpectedStrExp(inRValueLoc);
			}
			else
				result=new CStrAssignmentStatement(inLValue.mStrStore,inRValue->ReleaseStrExp());

			UTBException::ThrowIfNoParserMemory(result);
			break;
		
		default:
			SignalPStr_("\pUnknown variable class in MakeAssignmentStatement");
			Throw_(-1);
			break;
	}
	
	return result;
}

#undef yylex
#define yylex PatchedLex
int PatchedLex();

// A handy macro for use during parse tree construction
#define TBStatement_(x)			do {yyval.statement=new x; UTBException::ThrowIfNoParserMemory(yyval.statement); } while (false)
#define TBStrFunction_(x)		do {yyval.strExp=new x; UTBException::ThrowIfNoParserMemory(yyval.strExp); } while (false)
#define TBArithFunction_(x)		do {yyval.arithExp=new x; UTBException::ThrowIfNoParserMemory(yyval.arithExp); } while (false)
#define	TBGeneralExp_(x)		do {yyval.genExp=new x; UTBException::ThrowIfNoParserMemory(yyval.genExp); } while (false)

// Creates exp only if tests are not nil. If one is nil and the others not then the non nil is deallocted and nil is
// returned as the reult.
#define TBMaybeArithExp1_(test,exp)			do {if (test)  { yyval.arithExp=new exp; UTBException::ThrowIfNoParserMemory(yyval.arithExp); } else yyval.arithExp=0L; } while (false)
#define TBMaybeArithExp2_(test,test2,exp)	do {if (test && test2)  { yyval.arithExp=new exp; UTBException::ThrowIfNoParserMemory(yyval.arithExp); } else { yyval.arithExp=0L; delete test; delete test2; } } while (false)
#define TBMaybeStrExp1_(test,exp)			do {if (test)  { yyval.strExp=new exp; UTBException::ThrowIfNoParserMemory(yyval.strExp); } else yyval.strExp=0L; } while (false)
#define TBMaybeStrExp2_(test,test2,exp)		do {if (test && test2)  { yyval.strExp=new exp; UTBException::ThrowIfNoParserMemory(yyval.strExp); } else { yyval.strExp=0L; delete test; delete test2; } } while (false)
#define TBGenExpOrError_(test,exp)			do {if (test) yyval.genExp=new exp; else yyval.genExp=new CGeneralExpressionErr; UTBException::ThrowIfNoParserMemory(yyval.genExp); } while (false)


#line 317 "basic.yacc"
typedef union
{
	class CGeneralExpression	*genExp;
	class CArithExpression		*arithExp;
	class CStrExpression		*strExp;
	class CProgram				*program;
	class CStatement			*statement;
	class CIntegerStorage		*intStorage;
	class CStrStorage			*strStorage;
	class CFloatStorage			*floatStorage;
	class CLabelEntry			*labelEntry;
	class CArrayDimEntry		*arrayEntry;
	class CProcedure			*procedureDef;
	class CVariableNode			*variableNode;
	class CIntegerArraySymbol	*intArray;
	class CConditionalBlock		*conditionalBlock;
	class CForStatement			*forStatement;
	char						*string;
	int							tokenId;			// used to specify a token id for a general statement
	class CVariableSymbol		*variable;
	class CArraySymbol			*array;
	class CPrimitiveStorage		*storage;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		422
#define	YYFLAG		-32768
#define	YYNTBASE	403

#define YYTRANSLATE(x) ((unsigned)(x) <= 656 ? yytranslate[x] : 473)

static const short yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
   117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
   127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
   177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
   187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
   197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
   207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
   217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
   227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
   237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
   247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
   257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
   267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
   277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
   287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
   297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
   307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
   317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
   327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
   337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
   347,   348,   349,   350,   351,   352,   353,   354,   355,   356,
   357,   358,   359,   360,   361,   362,   363,   364,   365,   366,
   367,   368,   369,   370,   371,   372,   373,   374,   375,   376,
   377,   378,   379,   380,   381,   382,   383,   384,   385,   386,
   387,   388,   389,   390,   391,   392,   393,   394,   395,   396,
   397,   398,   399,   400,   401,   402
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    30,    34,    36,    40,    44,    46,
    50,    54,    56,    58,    60,    64,    68,    70,    72,    75,
    79,    84,    86,    88,    92,    96,   100,   104,   106,   110,
   113,   116,   119,   122,   124,   128,   132,   136,   139,   142,
   145,   148,   151,   154,   159,   164,   165,   167,   171,   175,
   179,   182,   189,   192,   199,   201,   206,   211,   215,   219,
   221,   224,   229,   234,   236,   239,   244,   249,   253,   255,
   257,   259,   261,   263,   265,   267,   270,   273,   277,   281,
   285,   289,   293,   297,   301,   303,   307,   311,   315,   319,
   323,   327,   331,   335,   339,   343,   347,   351,   353,   355,
   357,   361,   363,   367,   369,   371,   372,   374,   376,   378,
   380,   384,   388,   392,   396,   400,   404,   408,   412,   416,
   417,   420,   424,   426,   428,   430,   432,   434,   436,   438,
   440,   442,   444,   446,   448,   450,   452,   454,   456,   458,
   460,   462,   464,   467,   469,   473,   475,   479,   481,   485,
   487,   491,   495,   497,   499,   501,   510,   514,   520,   526,
   532,   540,   548,   556,   560,   564,   568,   570,   574,   576,
   580,   584,   588,   592,   594,   598,   600,   604,   608,   612,
   616,   620,   623,   630,   634,   638,   639,   643,   644,   648,
   652,   656,   661,   668,   675,   681,   688,   695,   702,   707,
   708,   711,   714,   716,   719,   722,   727,   732,   734,   738
};

static const short yyrhs[] = {   433,
     0,    46,     0,    48,     0,    47,     0,    10,     0,    11,
     0,    12,     0,    13,     0,    14,     0,    15,     0,    16,
     0,    17,     0,    10,     0,    14,     1,    35,     0,    14,
   432,    35,     0,    11,     0,    15,   432,    35,     0,    15,
     1,    35,     0,    12,     0,    16,   432,    35,     0,    16,
     1,    35,     0,   407,     0,   409,     0,   408,     0,    17,
   432,    35,     0,    17,     1,    35,     0,    13,     0,   405,
     0,   406,    35,     0,   405,    31,   411,     0,   406,    35,
    31,   411,     0,   405,     0,   405,     0,   412,    26,     1,
     0,   412,    26,   431,     0,   406,     1,    35,     0,   406,
   432,    35,     0,   413,     0,   413,    31,   414,     0,   404,
   414,     0,     1,   414,     0,   404,     1,     0,   404,   405,
     0,   416,     0,   417,    31,   416,     0,     1,    31,   416,
     0,   417,    31,     1,     0,   404,   411,     0,     1,   411,
     0,   404,     1,     0,   405,    23,     0,     8,    23,     0,
     1,    23,     0,   419,     1,    24,    32,     0,   419,   432,
    24,    32,     0,     0,   417,     0,    66,     8,    23,     0,
    66,     8,     1,     0,    66,     1,    23,     0,    66,     1,
     0,   422,     1,    24,   433,   424,    32,     0,   422,     1,
     0,   422,   421,    24,   433,   424,    32,     0,    67,     0,
    67,    23,   431,    24,     0,    67,    23,     1,    24,     0,
    79,   432,    32,     0,    79,     1,    32,     0,    80,     0,
    80,     1,     0,    80,    23,   432,    24,     0,    80,    23,
     1,    24,     0,    81,     0,    81,     1,     0,    81,    23,
   432,    24,     0,    81,    23,     1,    24,     0,    23,   428,
    24,     0,     4,     0,     7,     0,   426,     0,    92,     0,
   100,     0,   407,     0,   409,     0,    19,   428,     0,    43,
   428,     0,   428,    18,   428,     0,   428,    19,   428,     0,
   428,    21,   428,     0,   428,    20,   428,     0,   428,    22,
   428,     0,   428,    41,   428,     0,   428,    42,   428,     0,
   429,     0,   428,    26,   428,     0,   428,    33,   428,     0,
   428,    27,   428,     0,   428,    29,   428,     0,   428,    28,
   428,     0,   428,    30,   428,     0,   430,    26,   430,     0,
   430,    33,   430,     0,   430,    27,   430,     0,   430,    29,
   430,     0,   430,    28,   430,     0,   430,    30,   430,     0,
    68,     0,    69,     0,    93,     0,    23,   430,    24,     0,
   408,     0,   430,    18,   430,     0,     5,     0,   427,     0,
     0,   428,     0,   430,     0,    13,     0,   431,     0,   432,
    31,   431,     0,   432,    61,   431,     0,   432,    36,   431,
     0,     1,    31,   431,     0,     1,    61,   431,     0,     1,
    36,   431,     0,   432,    31,     1,     0,   432,    36,     1,
     0,   432,    61,     1,     0,     0,   433,   434,     0,   433,
     1,    32,     0,   470,     0,   459,     0,   464,     0,   465,
     0,   467,     0,   444,     0,   443,     0,   455,     0,   451,
     0,   472,     0,   450,     0,   423,     0,   420,     0,   458,
     0,   437,     0,   439,     0,   441,     0,   453,     0,   435,
     0,    32,     0,   415,    32,     0,    50,     0,   436,   415,
    32,     0,    51,     0,   438,   415,    32,     0,    52,     0,
   440,   415,    32,     0,    49,     0,   442,     1,    32,     0,
   442,   418,    32,     0,   425,     0,   445,     0,   446,     0,
   173,    14,    35,    31,   428,    61,   428,    32,     0,   173,
     1,    32,     0,   131,    10,    31,   428,    32,     0,   131,
    12,    31,   428,    32,     0,   131,    11,    31,   430,    32,
     0,   131,    14,   432,    35,    31,   428,    32,     0,   131,
    16,   432,    35,    31,   428,    32,     0,   131,    15,   432,
    35,    31,   430,    32,     0,   131,     1,    32,     0,   406,
   432,    35,     0,   406,     1,    35,     0,   447,     0,   448,
    31,   447,     0,    65,     0,   449,   448,    32,     0,   449,
     1,    32,     0,    53,   432,    32,     0,    53,     1,    32,
     0,    55,     0,   452,     9,    32,     0,    64,     0,   454,
     9,    32,     0,    37,   431,    32,     0,    37,     1,    32,
     0,    37,   431,    45,     0,    37,     1,    45,     0,   457,
   434,     0,   456,   433,   461,   462,    40,    32,     0,    38,
   431,    32,     0,    38,     1,    32,     0,     0,   460,   433,
   461,     0,     0,    39,    32,   433,     0,    44,   431,    32,
     0,    44,     1,    32,     0,   463,   433,    56,    32,     0,
    57,    32,   433,    58,   431,    32,     0,    57,    32,   433,
    58,     1,    32,     0,    57,    32,   433,    59,    32,     0,
    60,   410,    26,     1,   468,    32,     0,    60,     1,    26,
   432,   468,    32,     0,    60,   410,    26,   432,   468,    32,
     0,   466,   433,   469,    32,     0,     0,    62,   431,     0,
    62,     1,     0,    63,     0,    63,   410,     0,    63,     1,
     0,   410,    26,   431,    32,     0,   410,    26,     1,    32,
     0,   410,     0,   410,    31,   471,     0,    54,   471,    32,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   462,   470,   471,   472,   476,   477,   478,   479,   483,   484,
   485,   486,   490,   494,   499,   510,   514,   524,   530,   534,
   544,   552,   553,   554,   555,   556,   557,   565,   566,   567,
   568,   574,   580,   581,   592,   621,   626,   666,   667,   673,
   674,   679,   691,   693,   694,   695,   696,   700,   701,   702,
   717,   724,   725,   728,   735,   751,   752,   756,   774,   780,
   785,   792,   805,   816,   837,   842,   856,   871,   881,   889,
   896,   902,   914,   922,   929,   941,   953,   972,   973,   974,
   975,   976,   977,   978,   979,   980,   981,   982,   983,   984,
   985,   986,   987,   988,   989,   998,   999,  1000,  1001,  1002,
  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,  1020,
  1021,  1022,  1023,  1024,  1025,  1035,  1036,  1037,  1038,  1041,
  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,  1050,  1058,
  1059,  1060,  1065,  1066,  1066,  1066,  1066,  1066,  1066,  1066,
  1067,  1067,  1067,  1067,  1067,  1067,  1067,  1068,  1068,  1068,
  1068,  1068,  1076,  1088,  1089,  1101,  1102,  1114,  1115,  1129,
  1137,  1145,  1186,  1186,  1186,  1188,  1189,  1193,  1195,  1197,
  1199,  1206,  1213,  1220,  1228,  1244,  1251,  1252,  1255,  1256,
  1267,  1274,  1285,  1292,  1297,  1306,  1311,  1342,  1351,  1357,
  1375,  1381,  1397,  1445,  1454,  1460,  1461,  1479,  1480,  1487,
  1498,  1504,  1537,  1560,  1572,  1591,  1597,  1603,  1643,  1688,
  1689,  1693,  1701,  1702,  1719,  1732,  1756,  1764,  1764,  1768
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","REM","INTEGER",
"STRLITERAL","LABEL","FLOAT","PROCIDENTIFIER","LABELIDENTIFIER","INTIDENTIFIER",
"STRIDENTIFIER","FLOATIDENTIFIER","UNDECLAREDPRIMITIVE","INTARRAYIDENTIFIER",
"STRARRAYIDENTIFIER","FLOATARRAYIDENTIFIER","UNDECLAREDARRAY","PLUS","MINUS",
"DIVIDE","TIMES","POWER","OPENBRACKET","CLOSEBRACKET","UMINUS","EQUALS","LT",
"GT","LTE","GTE","COMMA","ENDLINE","NEQ","OPENSQBRACKET","CLOSESQBRACKET","SEMICOLON",
"IF","ELSEIF","ELSE","ENDIF","AND","OR","NOT","WHILE","THEN","INTEGERTYPE","FLOATTYPE",
"STRINGTYPE","SHARED","GLOBAL","NETGLOBAL","IMPORTANTNETGLOBAL","DATA","READ",
"RESTORE","WEND","REPEAT","UNTIL","FOREVER","FOR","TO","STEP","NEXT","GOTO",
"DIM","PROCEDURE","ENDPROC","TNTB_TRUE","TNTB_FALSE","END","BREAK","PRINT","GRAPHICSMODE",
"GRAPHICSMODEHARDWARE","TEXTMODE","BUTTON","WINDOWMODE","WINDOWMODEHARDWARE",
"STATEMENTNAME","ARITHFUNCTIONNAME","STRFUNCTIONNAME","SPRITE","SYNCSCREEN",
"VBLSYNCON","VBLSYNCOFF","FRAMERATE","FRAMERATEMAX","DRAWPICTRES","PASTESPRITE",
"PASTEIMAGE","ISPRESSED","PROCRESULT","PROCRESULTSTR","BEEP","FADEUP","FADEDOWN",
"FADETO","HIDEMOUSE","SHOWMOUSE","PROCRESULTFLOAT","GETMOUSEX","GETMOUSEY","LOADSOUNDS",
"PLAYSOUND","LOOPSOUND","SOUNDRESULT","STOPSOUND","STOPCHANNEL","SOUNDPLAYING",
"CHANNELPLAYING","UNLOADSOUNDS","MOVESOUND","MOVECHANNEL","SOUNDVOLUME","SETSOUNDVOLUME",
"SPRITEOFF","INITNETWORKING","HOSTNETGAME","JOINNETGAME","GETNETPLAYERNAME",
"COUNTNETPLAYERS","GETNETID","LEAVENETGAME","MORENETEVENTS","GETNETEVENTTYPE",
"GETNETEVENTCONTENT","NETGAMETERMINATED","NETPLAYERLEFT","NETPLAYERDISCONNECTED",
"NETCONNECTIONLOST","SETNETDATA","UPDATENETDATA","SETNETWORKINGNORMAL","SETNETWORKINGREGISTERED",
"COUNTRESOURCES","GETRESOURCENAME","RESOURCEEXISTS","LOADIMAGES","UNLOADIMAGES",
"SETSPRITEBANK","SETDEFAULTSPRITEBANK","SETSPRITEPRIORITY","LOADINPUTBANK","POLLINPUT",
"EDITINPUT","SUSPENDINPUT","RESUMEINPUT","LOADMUSIC","PLAYMUSIC","STOPMUSIC",
"GETMUSICLENGTH","GETMUSICPOSITION","SETMUSICPOSITION","MUSICVOLUME","SETMUSICVOLUME",
"MUSICLOOPON","MUSICLOOPOFF","ISMUSICPLAYING","DRAWTEXT","TEXTFONT","TEXTSIZE",
"TEXTFACE","NUMTOSTRING","STRINGTONUM","TEXTWIDTH","TEXTHEIGHT","TEXTDESCENT",
"OPENCANVAS","CLOSECANVAS","TARGETCANVAS","PAINTCANVAS","COPYCANVAS","CANVASFX",
"SPRITECOL","SPRITECOLRECT","POINTINRECT","WAITMOUSEDOWN","WAITMOUSEUP","WAITMOUSECLICK",
"UP","DOWN","LEFT","RIGHT","SPACE","SETSPRITETRANSPARENCY","GETSPRITETRANSPARENCY",
"SPRITECOLOUROFF","SPRITECOLOUR","LOADTNTMAP","UNLOADTNTMAP","SETCURRENTTNTMAP",
"GETTNTMAPWIDTH","GETTNTMAPHEIGHT","GETTNTMAPLAYERS","COUNTOBJECTS","GETNTHOBJECTNAME",
"GETNTHOBJECTTYPE","GETNTHOBJECTX","GETNTHOBJECTY","GETNTHOBJECTZ","COUNTPOLYGONS",
"INPOLYGON","POLYNAME","POLYBOUNDSX","POLYBOUNDSY","POLYBOUNDSWIDTH","POLYBOUNDSHEIGHT",
"GETTILE","DRAWMAPSECTION","SETMAPTILE","GETMAPTILEWIDTH","GETMAPTILEHEIGHT",
"MAPLINECOL","LINE","FILLRECT","FRAMERECT","FILLOVAL","FRAMEOVAL","FILLPOLY",
"FRAMEPOLY","SETPIXELCOLOUR","GETPIXEL","SETPENCOLOUR","GETPENCOLOUR","SETPENTRANSPARENCY",
"GETPENTRANSPARENCY","GETREDCOMPONENT","GETGREENCOMPONENT","GETBLUECOMPONENT",
"SETREDCOMPONENT","SETGREENCOMPONENT","SETBLUECOMPONENT","ABS","MOD","SIN","COS",
"TAN","INVSIN","INVCOS","INVTAN","RADSIN","RADCOS","RADTAN","INVRADSIN","INVRADCOS",
"INVRADTAN","CALCULATEANGLE","ANGLEDIFFERENCE","DEGTORAD","RADTODEG","SQUAREROOT",
"MIN","MAX","WRAP","BITAND","BITOR","BITXOR","BITTEST","SETBIT","BITSHIFT","CEIL",
"ROUND","FLOOR","RANDOM","SETRANDOMSEED","GETTIMER","DELAY","INVALRECT","NEWBUTTON",
"CLEARBUTTONS","WAITBUTTONCLICK","POLLBUTTONCLICK","GETBUTTONCLICK","CREATEVIEWPORT",
"CREATEMAPVIEWPORT","REMOVEVIEWPORT","VIEWPORTOFFSET","GETVIEWPORTXOFFSET","GETVIEWPORTYOFFSET",
"GETVIEWPORTWIDTH","GETVIEWPORTHEIGHT","DRAWTRACKINGON","DRAWTRACKINGOFF","MAKECOLOUR",
"NTHRESOURCEID","LOWERCASE","UPPERCASE","STRINGLENGTH","REMOVECHAR","GETCHAR",
"LEFTSTR","RIGHTSTR","MIDSTR","COUNTIMAGES","IMAGEWIDTH","IMAGEHEIGHT","IMAGEXOFF",
"IMAGEYOFF","SETIMAGEOFFSETS","WHITE","BLACK","RED","GREEN","BLUE","YELLOW",
"COPYIMAGE","DELETEIMAGE","IMAGECOLLISIONTOLERANCE","IMAGECOLLISIONTYPE","SETIMAGECOLLISION",
"CREATEFILE","FILEOPEN","ASKCREATEFILE","ASKUSERFILEOPEN","FILECLOSE","FILEWRITE",
"FILEREADSTRING","FILEREADFLOAT","FILEREADINT","NEWFOLDER","DELETEFILE","FILEPOS",
"SETFILEPOS","FILELENGTH","FILEEXISTS","FILETYPE","SETDIRECTORYSYSTEMPREFERENCES",
"SETDIRECTORYUSERPREFERENCES","SETDIRECTORYTEMPORARY","SETDIRECTORYGAME","SPRITEX",
"SPRITEY","SPRITEI","MOVESPRITE","SPRITEBANK","SPRITEMOVING","SPRITEANIMATING",
"ANIMSPRITE","SETMOANPROG","ADDSPRITETOCHAN","ADDVIEWPORTTOCHAN","CHANMOVE",
"CHANANIM","REMOVESPRITEFROMCHAN","REMOVEVIEWPORTFROMCHAN","AUTOMOANON","AUTOMOANOFF",
"AUTOMOANING","STEPMOAN","MOVEVIEWPORT","VIEWPORTMOVING","PAUSECHANNEL","UNPAUSECHANNEL",
"CHANPAUSED","RESETCHAN","CHANREG","SETCHANREG","RESTARTCHAN","CHANPLAY","SETSPRITEANGLE",
"SPRITEANGLE","SETSPRITEXSCALE","SPRITEXSCALE","SETSPRITEYSCALE","SPRITEYSCALE",
"SCALESPRITE","SPRITESCALING","SETSPRITEXFLIP","SETSPRITEYFLIP","SPRITEXFLIP",
"SPRITEYFLIP","ROTATESPRITE","SPRITEROTATING","SPRITEACTIVE","SPRITECROP","SPRITECROPOFF",
"DISABLEBREAK","READRAWKEY","RAW2ASCII","RAW2SCANCODE","RAW2STR","CHAR2ASCII",
"ASCII2CHAR","KEYBOARDREPEATON","KEYBOARDREPEATOFF","GETKEYBOARDREPEAT","CLEARKEYBOARD",
"WAITKEYPRESS","RAWMODSHIFT","RAWMODCOMMAND","RAWMODOPTION","RAWMODCAPS","RAWMODCONTROL",
"RAWFILTNUMBER","RAWFILTPRINT","RAWFILTNAV","RAWFILTDEL","STREDITORCONTENTS",
"SETSTREDITORCONTENTS","STREDITORCURSOR","SETSTREDITORCURSOR","STREDITORINPUT",
"program","variableType","primVariable","arrayVariable","intStorage","strStorage",
"floatStorage","storage","extVariableList","initedVariableLValue","initedVariable",
"initedVariableList","typedInitedVariableList","typedPrimVariable","typedPrimVariableList",
"typedExtVariableList","procedureCallHead","procedureCall","paramList","procName",
"procedureDef","endProc","statementCall","intFunctionCall","strFunctionCall",
"numExpression","boolExpression","strExpression","genExp","genExpList","statementBlock",
"statement","varDeclaration","globalHead","globalStatement","netGlobalHead",
"netGlobalStatement","importantNetGlobalHead","importantNetGlobalStatement",
"sharedHead","sharedStatement","functionCall","fxMatrixStatement","setNetDataStatement",
"arrayDimEntry","arrayDimList","dimHead","dimStatement","dataStatement","restoreHead",
"restoreStatement","gotoHead","gotoStatement","ifHead","ifThenHead","ifThenStatement",
"ifStatement","elseIfHead","elseIfs","else","whileHead","whileStatement","repeatStatement",
"forHead","forStatement","stepStatement","nextStatement","assignmentStatement",
"readVarsList","readStatement", NULL
};
#endif

static const short yyr1[] = {     0,
   403,   404,   404,   404,   405,   405,   405,   405,   406,   406,
   406,   406,   407,   407,   407,   408,   408,   408,   409,   409,
   409,   410,   410,   410,   410,   410,   410,   411,   411,   411,
   411,   412,   413,   413,   413,   413,   413,   414,   414,   415,
   415,   415,   416,   417,   417,   417,   417,   418,   418,   418,
   419,   419,   419,   420,   420,   421,   421,   422,   422,   422,
   422,   423,   423,   423,   424,   424,   424,   425,   425,   426,
   426,   426,   426,   427,   427,   427,   427,   428,   428,   428,
   428,   428,   428,   428,   428,   428,   428,   428,   428,   428,
   428,   428,   428,   428,   428,   429,   429,   429,   429,   429,
   429,   429,   429,   429,   429,   429,   429,   429,   429,   430,
   430,   430,   430,   430,   430,   431,   431,   431,   431,   432,
   432,   432,   432,   432,   432,   432,   432,   432,   432,   433,
   433,   433,   434,   434,   434,   434,   434,   434,   434,   434,
   434,   434,   434,   434,   434,   434,   434,   434,   434,   434,
   434,   434,   435,   436,   437,   438,   439,   440,   441,   442,
   443,   443,   444,   444,   444,   445,   445,   446,   446,   446,
   446,   446,   446,   446,   447,   447,   448,   448,   449,   450,
   450,   451,   451,   452,   453,   454,   455,   456,   456,   457,
   457,   458,   459,   460,   460,   461,   461,   462,   462,   463,
   463,   464,   465,   465,   465,   466,   466,   466,   467,   468,
   468,   468,   469,   469,   469,   470,   470,   471,   471,   472
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     1,     3,     3,     1,     3,
     3,     1,     1,     1,     3,     3,     1,     1,     2,     3,
     4,     1,     1,     3,     3,     3,     3,     1,     3,     2,
     2,     2,     2,     1,     3,     3,     3,     2,     2,     2,
     2,     2,     2,     4,     4,     0,     1,     3,     3,     3,
     2,     6,     2,     6,     1,     4,     4,     3,     3,     1,
     2,     4,     4,     1,     2,     4,     4,     3,     1,     1,
     1,     1,     1,     1,     1,     2,     2,     3,     3,     3,
     3,     3,     3,     3,     1,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
     3,     1,     3,     1,     1,     0,     1,     1,     1,     1,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     0,
     2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     1,     3,     1,     3,     1,     3,     1,
     3,     3,     1,     1,     1,     8,     3,     5,     5,     5,
     7,     7,     7,     3,     3,     3,     1,     3,     1,     3,
     3,     3,     3,     1,     3,     1,     3,     3,     3,     3,
     3,     2,     6,     3,     3,     0,     3,     0,     3,     3,
     3,     4,     6,     6,     5,     6,     6,     6,     4,     0,
     2,     2,     1,     2,     2,     4,     4,     1,     3,     3
};

static const short yydefact[] = {   130,
     0,     0,     0,     5,     6,     7,     8,     0,     0,     0,
     0,   152,     0,     0,     2,     4,     3,   160,   154,   156,
   158,     0,     0,   184,     0,     0,   186,   179,     0,     0,
     0,     0,     0,     0,    22,    24,    23,     0,     0,     0,
   145,     0,   144,   163,   131,   151,     0,   147,     0,   148,
     0,   149,     0,   139,   138,   164,   165,     0,   143,   141,
     0,   150,     0,   140,   130,     0,   146,   134,   130,   135,
   136,   130,   137,   133,   142,     5,     6,     7,     8,     9,
    10,    11,    12,    53,   132,    33,     0,     0,    38,    41,
    52,     0,    79,   114,    80,    13,    16,    19,   119,     0,
     0,     0,   108,   109,     0,     0,    82,   110,    83,    84,
   112,    85,    81,   115,   117,    95,   118,   120,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    27,   218,     0,   130,     0,     0,    61,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    42,    40,    51,     0,   153,     0,     0,    63,     0,    44,
    57,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   177,     0,     0,     0,     0,     0,   192,     0,     0,
     0,     0,     0,     0,   116,    14,   116,   116,    86,     0,
     0,     0,    87,    71,     0,    75,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
     0,    18,    17,    21,    20,    26,    25,   189,   191,   188,
   190,   201,   200,   183,   182,     0,   220,     0,     0,     0,
    60,    59,    58,    69,    68,   174,     0,     0,     0,     0,
     0,     0,     0,   167,     0,     0,     0,     0,     0,   130,
     0,    43,     0,   130,   155,   157,   159,   161,    28,     0,
    49,    50,    48,   162,   181,     0,     0,     0,   180,   185,
   187,     0,   130,   198,     0,     0,     0,    36,    37,    34,
    35,    39,   124,   126,   125,    78,   111,     0,     0,     0,
     0,    88,    89,    91,    90,    92,    96,    98,   100,    99,
   101,    97,    93,    94,     0,   113,   102,   104,   106,   105,
   107,   103,   127,   121,   128,   123,   129,   122,   219,     0,
     0,   210,   210,   210,     0,     0,     0,     0,     0,     0,
     0,   217,   216,    54,    55,     0,    46,    47,    45,     0,
     0,    29,   176,   175,   178,     0,     0,     0,     0,     0,
   202,   215,   214,   209,    73,    72,    77,    76,     0,     0,
     0,   205,     0,     0,     0,     0,   168,   170,   169,     0,
     0,     0,     0,    65,     0,     0,    30,     0,   195,   194,
   197,   130,     0,   204,   203,   212,   211,   207,   206,   208,
     0,     0,     0,     0,     0,    62,    64,    31,     0,   193,
   171,   173,   172,     0,     0,     0,   166,    67,    66,     0,
     0,     0
};

static const short yydefgoto[] = {   420,
    33,    34,    87,   110,   111,   112,    38,   271,    88,    89,
    90,    39,   160,   161,   169,    40,    41,   162,    42,    43,
   385,    44,   113,   114,   115,   116,   117,   118,   119,     1,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,   172,   173,    58,    59,    60,    61,    62,
    63,    64,    65,    66,    67,    68,   283,   284,   360,    69,
    70,    71,    72,    73,   374,   287,    74,   134,    75
};

static const short yypact[] = {-32768,
   378,   658,    18,    40,    63,    67,    71,  1198,  1238,  1310,
  1350,-32768,  1858,  1952,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,  1422,   716,-32768,    76,  1164,-32768,-32768,    44,  1456,
   724,    90,  1494,   113,-32768,-32768,-32768,   117,   116,  1528,
-32768,    30,-32768,-32768,-32768,-32768,    11,-32768,    11,-32768,
    11,-32768,    35,-32768,-32768,-32768,-32768,    86,-32768,-32768,
   141,-32768,   149,-32768,-32768,   837,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   134,  1568,   144,   136,-32768,
-32768,   110,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2274,
  2274,  2274,-32768,-32768,  2357,  2402,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  1267,-32768,  1589,-32768,   208,   248,
   256,   287,   299,   305,   339,    64,   112,   170,   186,    -8,
   217,-32768,   143,   191,-32768,   150,   203,   207,    28,   320,
   340,   202,   205,   220,   224,  1640,  1640,  1640,   206,   222,
-32768,-32768,-32768,  1995,-32768,     2,    13,   -16,   142,-32768,
   237,   246,  1714,   250,   254,   261,   583,  1824,   264,   269,
  1680,-32768,   155,   270,   271,   438,   847,-32768,   573,   645,
   405,   433,  1901,  1714,  2231,-32768,  2231,  2231,   258,  1589,
   514,  1041,   258,-32768,  1752,-32768,  1786,  2274,  2274,  2274,
  2274,  2274,  2274,  2274,  2274,  2274,  2274,  2274,  2274,  2274,
    56,    56,    56,    56,    56,    56,    56,   908,-32768,   980,
  1020,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   716,-32768,   513,  1092,  1126,
-32768,-32768,-32768,-32768,-32768,-32768,  2274,    56,  2274,   210,
   436,   470,   476,-32768,   257,   278,   279,   288,   293,-32768,
    66,-32768,    38,-32768,-32768,-32768,-32768,-32768,   301,   298,
-32768,-32768,-32768,-32768,-32768,   571,   605,   382,-32768,-32768,
-32768,  2046,-32768,   300,   306,   249,   310,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    19,   111,   132,
   195,   252,   252,   258,   258,-32768,  1057,  1605,  1605,  1605,
  1605,  1057,  1379,   934,    56,-32768,   325,   325,   325,   325,
   325,   325,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2089,
   312,    33,    37,    33,   778,    16,  2307,   322,   327,   330,
  2274,-32768,-32768,-32768,-32768,   705,-32768,-32768,-32768,   705,
  1714,   332,-32768,-32768,-32768,   341,   345,   438,   350,   324,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    22,   351,
   352,-32768,  2140,   371,   372,   373,-32768,-32768,-32768,  2274,
    56,  2274,   286,   383,   375,   379,-32768,  1714,-32768,-32768,
-32768,-32768,   384,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  2439,    24,  2456,  2274,  2183,-32768,-32768,-32768,   777,-32768,
-32768,-32768,-32768,  2473,   390,   393,-32768,-32768,-32768,   419,
   420,-32768
};

static const short yypgoto[] = {-32768,
   -39,    14,   -57,    -1,     4,     9,   -19,  -155,-32768,-32768,
   -22,    41,  -201,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    73,-32768,-32768,-32768,     5,-32768,   -83,     7,   129,   -63,
   355,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   156,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    79,-32768,-32768,
-32768,-32768,-32768,-32768,  -134,-32768,-32768,   209,-32768
};


#define	YYLAST		2515


static const short yytable[] = {    35,
   171,   176,   159,   133,    36,   179,   137,   260,   180,    37,
   152,   163,   273,   168,   261,    86,   190,   192,   190,   127,
   129,    35,   185,   234,    35,   258,    36,   187,   242,    36,
   158,    37,   185,   211,    37,   167,   259,   187,   348,   211,
    91,   211,   365,   218,   138,   297,    86,   378,   220,   185,
   243,   139,   188,   -56,   187,   412,    15,    16,    17,   347,
    94,   349,   188,   218,    35,   -13,    97,   185,   220,    36,
     9,   238,   187,   221,    37,    15,    16,    17,   315,   188,
    15,    16,    17,    15,    16,    17,   170,   164,   -16,   165,
   149,   166,   -19,   221,   373,   228,   -27,   188,   373,    80,
    81,    82,    83,   150,   189,   191,   193,   135,   229,   270,
   270,    15,    16,    17,   190,   190,   190,   190,   190,   190,
   190,   190,   190,   190,   190,   190,   190,   316,   317,   318,
   319,   320,   321,   322,   366,   153,   106,   121,   123,   125,
   185,   218,   154,   230,   186,   187,   220,   155,   108,   174,
   131,    76,    77,    78,    79,   367,   231,   175,   141,   -32,
   257,   292,   185,   190,   336,   190,   184,   187,   157,   183,
   188,   221,   262,   236,    35,   239,    86,    35,    35,    36,
   269,   269,    36,    36,    37,   278,   279,    37,    37,   291,
    86,   293,   188,   294,   295,   387,   346,    86,   375,   376,
   350,   232,   302,   303,   304,   305,   306,   307,   308,   309,
   310,   311,   312,   313,   314,   182,   133,   233,   368,   358,
   171,   159,   237,   159,   324,   218,   326,   328,   240,   241,
   220,   369,   408,   246,    35,   247,    35,   254,   218,    36,
   185,    36,   219,   220,    37,   187,    37,   218,   235,   362,
   248,   335,   220,   337,   249,   221,   255,   190,    96,    97,
    98,   132,     8,     9,    10,    11,   363,   263,   221,   264,
   188,   200,   201,   202,   251,   252,   253,   221,   185,   202,
  -213,   265,   222,   187,    35,   266,   218,   341,   357,    36,
   223,   220,   267,   270,    37,   274,   190,   402,   190,   277,
   275,   280,   281,   198,   199,   200,   201,   202,   188,   342,
   343,   203,   204,   205,   206,   207,   221,   185,   208,   344,
   190,   224,   187,   299,   345,   301,   209,   210,   409,   218,
   270,   351,   352,   225,   220,   185,   371,   361,   359,   226,
   187,   364,   211,   372,    35,   383,   404,   188,    35,    36,
   185,   244,   380,    36,    37,   187,    35,   381,    37,   221,
   382,    36,   388,   393,   269,   188,    37,   332,   334,   218,
   218,   245,   389,   227,   220,   220,   390,    -1,     2,   397,
   188,   392,   394,   395,   401,     3,   403,     4,     5,     6,
     7,     8,     9,    10,    11,    80,    81,    82,    83,   221,
   221,   269,   398,   399,   400,   405,   406,    35,   414,    12,
   407,   416,    36,   418,    13,   410,   419,    37,   421,   422,
   178,    14,   386,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,   355,    25,   185,   391,    26,     2,   288,
   187,    27,    28,    29,   329,     3,     0,     4,     5,     6,
     7,     8,     9,    10,    11,     0,    30,     0,     0,     0,
     0,     0,     0,   218,     0,   188,   218,   289,   220,    12,
   338,   220,     0,     0,    13,   282,  -196,  -196,     0,     0,
     0,    14,     0,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,   221,    25,     0,   221,    26,     0,     0,
   218,    27,    28,    29,   339,   220,   218,     0,    31,     0,
   340,   220,     0,     2,     0,     0,    30,     0,     0,     0,
     3,     0,     4,     5,     6,     7,     8,     9,    10,    11,
   221,   198,   199,   200,   201,   202,   221,   296,     0,   203,
   204,   205,   206,   207,    12,     0,   208,     0,     0,    13,
    32,     0,     0,     0,   209,   210,    14,     0,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    31,    25,
   330,   331,    26,     2,     0,     0,    27,    28,    29,     0,
     3,     0,     4,     5,     6,     7,     8,     9,    10,    11,
     0,    30,    76,    77,    78,    79,    80,    81,    82,    83,
     0,   185,     0,     0,    12,   353,   187,     0,     0,    13,
    32,     0,     0,     0,   268,     0,    14,     0,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,   285,    25,
     0,   188,    26,     0,     0,   218,    27,    28,    29,   354,
   220,     0,     0,    31,     0,     2,     0,     0,     0,     0,
     0,    30,     3,     0,     4,     5,     6,     7,     8,     9,
    10,    11,     0,     0,     0,   221,     0,    76,    77,    78,
    79,    80,    81,    82,    83,     0,    12,     0,     0,     0,
    84,    13,     0,     0,     0,    32,     0,     0,    14,    85,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     0,    25,     0,    31,    26,     2,     0,   286,    27,    28,
    29,     0,     3,     0,     4,     5,     6,     7,     8,     9,
    10,    11,     0,    30,   142,    96,    97,    98,   132,     8,
     9,    10,    11,   143,   144,   145,    12,   146,   147,   148,
     0,    13,     0,     0,     0,    32,     0,     0,    14,     0,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     0,    25,     0,     0,    26,     0,     0,     0,    27,    28,
    29,   384,     0,     0,     0,    31,     0,     2,     0,     0,
     0,     0,     0,    30,     3,     0,     4,     5,     6,     7,
     8,     9,    10,    11,     0,   198,   199,   200,   201,   202,
     0,     0,     0,   203,   204,   205,   206,   207,    12,   377,
   208,     0,     0,    13,     0,     0,  -199,    32,   209,   210,
    14,     0,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,     0,    25,     0,    31,    26,   177,     0,     0,
    27,    28,    29,     0,     3,     0,     4,     5,     6,     7,
     8,     9,    10,    11,     0,    30,    76,    77,    78,    79,
    80,    81,    82,    83,     0,     0,     0,     0,    12,    84,
     0,     0,     0,    13,     0,     0,     0,    32,     0,     0,
    14,     0,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,     0,    25,     0,     0,    26,     0,     0,     0,
    27,    28,    29,     0,     0,     0,     0,    31,   323,     0,
     0,    93,    94,     0,    95,    30,     0,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,     0,     0,     0,
   101,  -116,     0,     0,     0,     0,     0,     0,  -116,  -116,
     0,     0,  -116,  -116,     0,     0,     0,     0,     0,    32,
   102,   198,   199,   200,   201,   202,     0,     0,     0,   203,
   204,   205,   206,   207,     0,     0,   208,    31,  -116,  -116,
     0,     0,     0,     0,   209,   103,   104,     0,     0,     0,
   325,     0,     0,    93,    94,     0,    95,   105,   106,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,   107,
   108,     0,   101,  -116,     0,     0,     0,   109,     0,    32,
  -116,  -116,     0,     0,  -116,  -116,     0,     0,     0,     0,
   327,     0,   102,    93,    94,     0,    95,     0,     0,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,     0,
  -116,  -116,   101,  -116,     0,     0,     0,   103,   104,     0,
  -116,  -116,     0,     0,  -116,  -116,     0,     0,   211,   105,
   106,     0,   102,     0,   297,     0,   212,   213,   214,   215,
   216,   107,   108,   217,   198,   199,   200,   201,   202,   109,
  -116,  -116,-32768,   204,   205,   206,   207,   103,   104,-32768,
     0,     0,   250,     0,     0,    93,    94,     0,    95,   105,
   106,    96,    97,    98,    99,     8,     9,    10,     0,     0,
   100,   107,   108,     0,   101,     0,     0,     0,     0,   109,
     0,     0,  -116,  -116,     0,     0,   333,  -116,     0,    93,
    94,     0,    95,     0,   102,    96,    97,    98,    99,     8,
     9,    10,     0,     0,   100,     0,     0,     0,   101,     0,
     0,     0,  -116,  -116,     0,     0,  -116,  -116,     0,   103,
   104,  -116,     0,     0,   136,     0,     0,     0,   102,     0,
     0,   105,   106,    96,    97,    98,   132,     8,     9,    10,
    11,     0,     0,   107,   108,     0,  -116,  -116,     0,     0,
     0,   109,     0,   103,   104,     0,     0,     0,    92,     0,
     0,    93,    94,     0,    95,   105,   106,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,   107,   108,     0,
   101,     0,     0,     0,     0,   109,     0,     0,  -116,     0,
     0,     0,  -116,  -116,     0,     0,     0,     0,   120,     0,
   102,    93,    94,     0,    95,     0,     0,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,     0,  -116,     0,
   101,     0,     0,     0,     0,   103,   104,     0,  -116,     0,
     0,     0,  -116,  -116,     0,     0,     0,   105,   106,     0,
   102,     0,     0,     0,   198,   199,   200,   201,   202,   107,
   108,     0,   203,   204,   205,   206,   207,   109,  -116,   208,
     0,     0,     0,     0,     0,   103,   104,   209,   210,     0,
   122,     0,     0,    93,    94,     0,    95,   105,   106,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,   107,
   108,     0,   101,     0,     0,     0,     0,   109,     0,     0,
  -116,     0,     0,     0,  -116,  -116,     0,     0,     0,     0,
   124,     0,   102,    93,    94,     0,    95,     0,     0,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,     0,
  -116,     0,   101,     0,     0,     0,     0,   103,   104,     0,
  -116,     0,     0,     0,  -116,  -116,     0,     0,     0,   105,
   106,     0,   102,     0,     0,     0,   198,   199,   200,   201,
   202,   107,   108,     0,   203,   204,   205,   206,   207,   109,
  -116,   208,     0,     0,     0,     0,     0,   103,   104,     0,
     0,     0,   130,     0,     0,    93,    94,     0,    95,   105,
   106,    96,    97,    98,    99,     8,     9,    10,     0,     0,
   100,   107,   108,     0,   101,     0,     0,     0,     0,   109,
     0,     0,  -116,  -116,     0,     0,   140,  -116,     0,    93,
    94,     0,    95,     0,   102,    96,    97,    98,    99,     8,
     9,    10,     0,     0,   100,     0,     0,     0,   101,     0,
     0,     0,  -116,     0,     0,     0,  -116,  -116,     0,   103,
   104,  -116,     0,     0,   151,     0,     0,     0,   102,     0,
     0,   105,   106,    76,    77,    78,    79,    80,    81,    82,
    83,     0,     0,   107,   108,     0,  -116,     0,     0,     0,
     0,   109,     0,   103,   104,     0,     0,     0,   156,     0,
     0,    93,    94,     0,    95,   105,   106,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,   107,   108,     0,
   101,  -116,     0,     0,     0,   109,     0,     0,  -116,     0,
     0,     0,     0,  -116,     0,     0,     0,     0,   181,     0,
   102,    93,    94,     0,    95,     0,     0,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,     0,  -116,     0,
   101,     0,     0,     0,     0,   103,   104,     0,  -116,     0,
     0,     0,  -116,  -116,     0,     0,   211,   105,   106,     0,
   102,     0,     0,     0,   212,   213,   214,   215,   216,   107,
   108,   217,   198,   199,   200,   201,   202,   109,  -116,     0,
     0,-32768,-32768,-32768,-32768,   103,   104,     0,     0,     0,
   250,     0,     0,    93,    94,     0,    95,   105,   106,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,   107,
   108,     0,   101,     0,     0,     0,     0,   109,     0,     0,
  -116,     0,     0,     0,  -116,  -116,     0,     0,     0,     0,
   276,     0,   102,    93,    94,     0,    95,     0,     0,    96,
    97,    98,    99,     8,     9,    10,     0,     0,   100,     0,
  -116,     0,   101,     0,     0,     0,     0,   103,   104,     0,
  -116,     0,     0,     0,  -116,  -116,     0,     0,     0,   105,
   106,     0,   102,    76,    77,    78,    79,    80,    81,    82,
    83,   107,   108,     0,     0,     0,     0,     0,     0,   109,
  -116,     0,     0,     0,     0,     0,     0,   103,   104,     0,
     0,     0,   298,     0,     0,    93,    94,     0,    95,   105,
   106,    96,    97,    98,    99,     8,     9,    10,     0,     0,
   100,   107,   108,     0,   101,  -116,     0,     0,     0,   109,
     0,     0,  -116,     0,     0,     0,   300,  -116,     0,    93,
    94,     0,    95,     0,   102,    96,    97,    98,    99,     8,
     9,    10,     0,     0,   100,     0,     0,     0,   101,  -116,
     0,     0,  -116,     0,     0,     0,  -116,     0,     0,   103,
   104,  -116,     0,     0,   272,     0,     0,     0,   102,     0,
     0,   105,   106,    76,    77,    78,    79,    80,    81,    82,
    83,     0,     0,   107,   108,     0,  -116,     0,     0,     0,
     0,   109,     0,   103,   104,     0,     0,     0,   126,     0,
     0,    93,    94,     0,    95,   105,   106,    96,    97,    98,
    99,     8,     9,    10,     0,     0,   100,   107,   108,     0,
   101,     0,     0,     0,     0,   109,     0,     0,     0,  -116,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   102,   290,  -116,     0,    93,    94,     0,    95,     0,     0,
    96,    97,    98,    99,     8,     9,    10,     0,     0,   100,
     0,     0,     0,   101,     0,   103,   104,     0,     0,     0,
     0,  -116,  -116,     0,     0,     0,     0,   105,   106,     0,
     0,     0,     0,   102,     0,     0,     0,     0,     0,   107,
   108,     0,   128,     0,     0,    93,    94,   109,    95,     0,
     0,    96,    97,    98,    99,     8,     9,    10,   103,   104,
   100,     0,     0,     0,   101,     0,     0,     0,     0,     0,
   105,   106,     0,  -116,     0,     0,     0,     0,     0,     0,
     0,     0,   107,   108,   102,   256,     0,     0,    93,    94,
   109,    95,     0,     0,    96,    97,    98,    99,     8,     9,
    10,     0,     0,   100,     0,     0,     0,   101,     0,   103,
   104,     0,     0,     0,     0,     0,  -116,     0,     0,     0,
     0,   105,   106,     0,     0,     0,     0,   102,     0,     0,
     0,     0,     0,   107,   108,     0,   356,     0,     0,    93,
    94,   109,    95,     0,     0,    96,    97,    98,    99,     8,
     9,    10,   103,   104,   100,     0,     0,     0,   101,     0,
     0,     0,     0,     0,   105,   106,     0,  -116,     0,     0,
     0,     0,     0,     0,     0,     0,   107,   108,   102,   370,
     0,     0,    93,    94,   109,    95,     0,     0,    96,    97,
    98,    99,     8,     9,    10,     0,     0,   100,     0,     0,
     0,   101,     0,   103,   104,     0,     0,     0,     0,     0,
  -116,     0,     0,     0,     0,   105,   106,     0,     0,     0,
     0,   102,     0,     0,     0,     0,     0,   107,   108,     0,
   396,     0,     0,    93,    94,   109,    95,     0,     0,    96,
    97,    98,    99,     8,     9,    10,   103,   104,   100,     0,
     0,     0,   101,     0,     0,     0,     0,     0,   105,   106,
     0,  -116,     0,     0,     0,     0,     0,     0,     0,     0,
   107,   108,   102,   415,     0,     0,    93,    94,   109,    95,
     0,     0,    96,    97,    98,    99,     8,     9,    10,     0,
     0,   100,     0,     0,     0,   101,  -116,   103,   104,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   105,
   106,     0,     0,     0,     0,   102,     0,     0,     0,     0,
     0,   107,   108,     0,    93,    94,     0,    95,     0,   109,
    96,    97,    98,    99,     8,     9,    10,     0,     0,   100,
   103,   104,     0,   101,     0,     0,     0,     0,     0,     0,
     0,     0,   105,   106,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   102,   107,   108,     0,    93,    94,     0,
    95,     0,   109,    96,    97,    98,     0,     8,     9,    10,
     0,     0,   100,     0,     0,     0,   101,     0,   103,   104,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   105,   106,     0,     0,     0,     0,   102,     0,     0,     0,
     0,     0,   107,   108,   198,   199,   200,   201,   202,     0,
   109,     0,   203,   204,   205,   206,   207,     0,   379,   208,
     0,   103,   104,     0,     0,     0,     0,   209,   210,     0,
     0,     0,     0,   105,   106,     0,     0,   194,     0,     0,
     0,     0,     0,     0,     0,   107,   108,     0,     0,     0,
     0,     0,     0,   109,   -70,   -70,   -70,   -70,   -70,   195,
   -70,     0,   -70,   -70,   -70,   -70,   -70,   -70,   -70,   -70,
     0,   -70,   -70,     0,     0,     0,     0,   -70,   -70,     0,
     0,   -70,   196,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   -70,   -70,   -74,
   -74,   -74,   -74,   -74,   197,   -74,     0,   -74,   -74,   -74,
   -74,   -74,   -74,   -74,   -74,     0,   -74,   -74,     0,     0,
     0,     0,   -74,   -74,     0,     0,   -74,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   198,   199,   200,   201,
   202,     0,   -74,   -74,   203,   204,   205,   206,   207,     0,
   411,   208,     0,   198,   199,   200,   201,   202,     0,   209,
   210,   203,   204,   205,   206,   207,     0,   413,   208,     0,
   198,   199,   200,   201,   202,     0,   209,   210,   203,   204,
   205,   206,   207,     0,   417,   208,     0,     0,     0,     0,
     0,     0,     0,   209,   210
};

static const short yycheck[] = {     1,
    58,    65,    42,    23,     1,    69,    26,    24,    72,     1,
    33,     1,   168,    53,    31,     2,   100,   101,   102,    13,
    14,    23,    31,    32,    26,    24,    23,    36,     1,    26,
     1,    23,    31,    18,    26,     1,    24,    36,     1,    18,
    23,    18,    24,    31,     1,    24,    33,    32,    36,    31,
    23,     8,    61,    24,    36,    32,    46,    47,    48,   261,
     5,   263,    61,    31,    66,    26,    11,    31,    36,    66,
    15,   135,    36,    61,    66,    46,    47,    48,    23,    61,
    46,    47,    48,    46,    47,    48,     1,    47,    26,    49,
     1,    51,    26,    61,    62,    32,    26,    61,    62,    14,
    15,    16,    17,    14,   100,   101,   102,    32,    45,   167,
   168,    46,    47,    48,   198,   199,   200,   201,   202,   203,
   204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
   214,   215,   216,   217,    24,    23,    81,     9,    10,    11,
    31,    31,    26,    32,    35,    36,    36,    32,    93,     9,
    22,    10,    11,    12,    13,    24,    45,     9,    30,    26,
   154,   184,    31,   247,   248,   249,    31,    36,    40,    26,
    61,    61,   159,    31,   176,    26,   163,   179,   180,   176,
   167,   168,   179,   180,   176,    31,    32,   179,   180,   183,
   177,   185,    61,   187,   188,   351,   260,   184,   333,   334,
   264,    32,   198,   199,   200,   201,   202,   203,   204,   205,
   206,   207,   208,   209,   210,    87,   236,    32,    24,   283,
   278,   261,    32,   263,   218,    31,   220,   221,    26,    23,
    36,   315,   388,    32,   236,    31,   238,    32,    31,   236,
    31,   238,    35,    36,   236,    36,   238,    31,    32,     1,
    31,   247,    36,   249,    31,    61,    35,   341,    10,    11,
    12,    13,    14,    15,    16,    17,   286,    31,    61,    24,
    61,    20,    21,    22,   146,   147,   148,    61,    31,    22,
    32,    32,    35,    36,   286,    32,    31,    31,   282,   286,
    35,    36,    32,   351,   286,    32,   380,   381,   382,   171,
    32,    32,    32,    18,    19,    20,    21,    22,    61,    32,
    32,    26,    27,    28,    29,    30,    61,    31,    33,    32,
   404,    35,    36,   195,    32,   197,    41,    42,   392,    31,
   388,    31,    35,    35,    36,    31,   330,    32,    39,    35,
    36,    32,    18,    32,   346,   341,    61,    61,   350,   346,
    31,    32,    31,   350,   346,    36,   358,    31,   350,    61,
    31,   358,    31,    40,   351,    61,   358,   239,   240,    31,
    31,    32,    32,    35,    36,    36,    32,     0,     1,   373,
    61,    32,    32,    32,   380,     8,   382,    10,    11,    12,
    13,    14,    15,    16,    17,    14,    15,    16,    17,    61,
    61,   388,    32,    32,    32,    23,    32,   409,   404,    32,
    32,   405,   409,    24,    37,    32,    24,   409,     0,     0,
    66,    44,   350,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,   278,    57,    31,   358,    60,     1,    35,
    36,    64,    65,    66,   236,     8,    -1,    10,    11,    12,
    13,    14,    15,    16,    17,    -1,    79,    -1,    -1,    -1,
    -1,    -1,    -1,    31,    -1,    61,    31,    35,    36,    32,
    35,    36,    -1,    -1,    37,    38,    39,    40,    -1,    -1,
    -1,    44,    -1,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    61,    57,    -1,    61,    60,    -1,    -1,
    31,    64,    65,    66,    35,    36,    31,    -1,   131,    -1,
    35,    36,    -1,     1,    -1,    -1,    79,    -1,    -1,    -1,
     8,    -1,    10,    11,    12,    13,    14,    15,    16,    17,
    61,    18,    19,    20,    21,    22,    61,    24,    -1,    26,
    27,    28,    29,    30,    32,    -1,    33,    -1,    -1,    37,
   173,    -1,    -1,    -1,    41,    42,    44,    -1,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,   131,    57,
    58,    59,    60,     1,    -1,    -1,    64,    65,    66,    -1,
     8,    -1,    10,    11,    12,    13,    14,    15,    16,    17,
    -1,    79,    10,    11,    12,    13,    14,    15,    16,    17,
    -1,    31,    -1,    -1,    32,    35,    36,    -1,    -1,    37,
   173,    -1,    -1,    -1,    32,    -1,    44,    -1,    46,    47,
    48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    -1,    61,    60,    -1,    -1,    31,    64,    65,    66,    35,
    36,    -1,    -1,   131,    -1,     1,    -1,    -1,    -1,    -1,
    -1,    79,     8,    -1,    10,    11,    12,    13,    14,    15,
    16,    17,    -1,    -1,    -1,    61,    -1,    10,    11,    12,
    13,    14,    15,    16,    17,    -1,    32,    -1,    -1,    -1,
    23,    37,    -1,    -1,    -1,   173,    -1,    -1,    44,    32,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    -1,    57,    -1,   131,    60,     1,    -1,    63,    64,    65,
    66,    -1,     8,    -1,    10,    11,    12,    13,    14,    15,
    16,    17,    -1,    79,     1,    10,    11,    12,    13,    14,
    15,    16,    17,    10,    11,    12,    32,    14,    15,    16,
    -1,    37,    -1,    -1,    -1,   173,    -1,    -1,    44,    -1,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    -1,    57,    -1,    -1,    60,    -1,    -1,    -1,    64,    65,
    66,    67,    -1,    -1,    -1,   131,    -1,     1,    -1,    -1,
    -1,    -1,    -1,    79,     8,    -1,    10,    11,    12,    13,
    14,    15,    16,    17,    -1,    18,    19,    20,    21,    22,
    -1,    -1,    -1,    26,    27,    28,    29,    30,    32,    32,
    33,    -1,    -1,    37,    -1,    -1,    40,   173,    41,    42,
    44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    -1,    57,    -1,   131,    60,     1,    -1,    -1,
    64,    65,    66,    -1,     8,    -1,    10,    11,    12,    13,
    14,    15,    16,    17,    -1,    79,    10,    11,    12,    13,
    14,    15,    16,    17,    -1,    -1,    -1,    -1,    32,    23,
    -1,    -1,    -1,    37,    -1,    -1,    -1,   173,    -1,    -1,
    44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
    54,    55,    -1,    57,    -1,    -1,    60,    -1,    -1,    -1,
    64,    65,    66,    -1,    -1,    -1,    -1,   131,     1,    -1,
    -1,     4,     5,    -1,     7,    79,    -1,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    -1,    -1,    -1,
    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,
    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,    -1,   173,
    43,    18,    19,    20,    21,    22,    -1,    -1,    -1,    26,
    27,    28,    29,    30,    -1,    -1,    33,   131,    61,    62,
    -1,    -1,    -1,    -1,    41,    68,    69,    -1,    -1,    -1,
     1,    -1,    -1,     4,     5,    -1,     7,    80,    81,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    92,
    93,    -1,    23,    24,    -1,    -1,    -1,   100,    -1,   173,
    31,    32,    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,
     1,    -1,    43,     4,     5,    -1,     7,    -1,    -1,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    -1,
    61,    62,    23,    24,    -1,    -1,    -1,    68,    69,    -1,
    31,    32,    -1,    -1,    35,    36,    -1,    -1,    18,    80,
    81,    -1,    43,    -1,    24,    -1,    26,    27,    28,    29,
    30,    92,    93,    33,    18,    19,    20,    21,    22,   100,
    61,    62,    26,    27,    28,    29,    30,    68,    69,    33,
    -1,    -1,     1,    -1,    -1,     4,     5,    -1,     7,    80,
    81,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
    19,    92,    93,    -1,    23,    -1,    -1,    -1,    -1,   100,
    -1,    -1,    31,    32,    -1,    -1,     1,    36,    -1,     4,
     5,    -1,     7,    -1,    43,    10,    11,    12,    13,    14,
    15,    16,    -1,    -1,    19,    -1,    -1,    -1,    23,    -1,
    -1,    -1,    61,    62,    -1,    -1,    31,    32,    -1,    68,
    69,    36,    -1,    -1,     1,    -1,    -1,    -1,    43,    -1,
    -1,    80,    81,    10,    11,    12,    13,    14,    15,    16,
    17,    -1,    -1,    92,    93,    -1,    61,    62,    -1,    -1,
    -1,   100,    -1,    68,    69,    -1,    -1,    -1,     1,    -1,
    -1,     4,     5,    -1,     7,    80,    81,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    92,    93,    -1,
    23,    -1,    -1,    -1,    -1,   100,    -1,    -1,    31,    -1,
    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,     1,    -1,
    43,     4,     5,    -1,     7,    -1,    -1,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    -1,    61,    -1,
    23,    -1,    -1,    -1,    -1,    68,    69,    -1,    31,    -1,
    -1,    -1,    35,    36,    -1,    -1,    -1,    80,    81,    -1,
    43,    -1,    -1,    -1,    18,    19,    20,    21,    22,    92,
    93,    -1,    26,    27,    28,    29,    30,   100,    61,    33,
    -1,    -1,    -1,    -1,    -1,    68,    69,    41,    42,    -1,
     1,    -1,    -1,     4,     5,    -1,     7,    80,    81,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    92,
    93,    -1,    23,    -1,    -1,    -1,    -1,   100,    -1,    -1,
    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,
     1,    -1,    43,     4,     5,    -1,     7,    -1,    -1,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    -1,
    61,    -1,    23,    -1,    -1,    -1,    -1,    68,    69,    -1,
    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    80,
    81,    -1,    43,    -1,    -1,    -1,    18,    19,    20,    21,
    22,    92,    93,    -1,    26,    27,    28,    29,    30,   100,
    61,    33,    -1,    -1,    -1,    -1,    -1,    68,    69,    -1,
    -1,    -1,     1,    -1,    -1,     4,     5,    -1,     7,    80,
    81,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
    19,    92,    93,    -1,    23,    -1,    -1,    -1,    -1,   100,
    -1,    -1,    31,    32,    -1,    -1,     1,    36,    -1,     4,
     5,    -1,     7,    -1,    43,    10,    11,    12,    13,    14,
    15,    16,    -1,    -1,    19,    -1,    -1,    -1,    23,    -1,
    -1,    -1,    61,    -1,    -1,    -1,    31,    32,    -1,    68,
    69,    36,    -1,    -1,     1,    -1,    -1,    -1,    43,    -1,
    -1,    80,    81,    10,    11,    12,    13,    14,    15,    16,
    17,    -1,    -1,    92,    93,    -1,    61,    -1,    -1,    -1,
    -1,   100,    -1,    68,    69,    -1,    -1,    -1,     1,    -1,
    -1,     4,     5,    -1,     7,    80,    81,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    92,    93,    -1,
    23,    24,    -1,    -1,    -1,   100,    -1,    -1,    31,    -1,
    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,     1,    -1,
    43,     4,     5,    -1,     7,    -1,    -1,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    -1,    61,    -1,
    23,    -1,    -1,    -1,    -1,    68,    69,    -1,    31,    -1,
    -1,    -1,    35,    36,    -1,    -1,    18,    80,    81,    -1,
    43,    -1,    -1,    -1,    26,    27,    28,    29,    30,    92,
    93,    33,    18,    19,    20,    21,    22,   100,    61,    -1,
    -1,    27,    28,    29,    30,    68,    69,    -1,    -1,    -1,
     1,    -1,    -1,     4,     5,    -1,     7,    80,    81,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    92,
    93,    -1,    23,    -1,    -1,    -1,    -1,   100,    -1,    -1,
    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,
     1,    -1,    43,     4,     5,    -1,     7,    -1,    -1,    10,
    11,    12,    13,    14,    15,    16,    -1,    -1,    19,    -1,
    61,    -1,    23,    -1,    -1,    -1,    -1,    68,    69,    -1,
    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    80,
    81,    -1,    43,    10,    11,    12,    13,    14,    15,    16,
    17,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,   100,
    61,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,    -1,
    -1,    -1,     1,    -1,    -1,     4,     5,    -1,     7,    80,
    81,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
    19,    92,    93,    -1,    23,    24,    -1,    -1,    -1,   100,
    -1,    -1,    31,    -1,    -1,    -1,     1,    36,    -1,     4,
     5,    -1,     7,    -1,    43,    10,    11,    12,    13,    14,
    15,    16,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,
    -1,    -1,    61,    -1,    -1,    -1,    31,    -1,    -1,    68,
    69,    36,    -1,    -1,     1,    -1,    -1,    -1,    43,    -1,
    -1,    80,    81,    10,    11,    12,    13,    14,    15,    16,
    17,    -1,    -1,    92,    93,    -1,    61,    -1,    -1,    -1,
    -1,   100,    -1,    68,    69,    -1,    -1,    -1,     1,    -1,
    -1,     4,     5,    -1,     7,    80,    81,    10,    11,    12,
    13,    14,    15,    16,    -1,    -1,    19,    92,    93,    -1,
    23,    -1,    -1,    -1,    -1,   100,    -1,    -1,    -1,    32,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    43,     1,    45,    -1,     4,     5,    -1,     7,    -1,    -1,
    10,    11,    12,    13,    14,    15,    16,    -1,    -1,    19,
    -1,    -1,    -1,    23,    -1,    68,    69,    -1,    -1,    -1,
    -1,    31,    32,    -1,    -1,    -1,    -1,    80,    81,    -1,
    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    92,
    93,    -1,     1,    -1,    -1,     4,     5,   100,     7,    -1,
    -1,    10,    11,    12,    13,    14,    15,    16,    68,    69,
    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
    80,    81,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    92,    93,    43,     1,    -1,    -1,     4,     5,
   100,     7,    -1,    -1,    10,    11,    12,    13,    14,    15,
    16,    -1,    -1,    19,    -1,    -1,    -1,    23,    -1,    68,
    69,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,
    -1,    80,    81,    -1,    -1,    -1,    -1,    43,    -1,    -1,
    -1,    -1,    -1,    92,    93,    -1,     1,    -1,    -1,     4,
     5,   100,     7,    -1,    -1,    10,    11,    12,    13,    14,
    15,    16,    68,    69,    19,    -1,    -1,    -1,    23,    -1,
    -1,    -1,    -1,    -1,    80,    81,    -1,    32,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    92,    93,    43,     1,
    -1,    -1,     4,     5,   100,     7,    -1,    -1,    10,    11,
    12,    13,    14,    15,    16,    -1,    -1,    19,    -1,    -1,
    -1,    23,    -1,    68,    69,    -1,    -1,    -1,    -1,    -1,
    32,    -1,    -1,    -1,    -1,    80,    81,    -1,    -1,    -1,
    -1,    43,    -1,    -1,    -1,    -1,    -1,    92,    93,    -1,
     1,    -1,    -1,     4,     5,   100,     7,    -1,    -1,    10,
    11,    12,    13,    14,    15,    16,    68,    69,    19,    -1,
    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    80,    81,
    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    92,    93,    43,     1,    -1,    -1,     4,     5,   100,     7,
    -1,    -1,    10,    11,    12,    13,    14,    15,    16,    -1,
    -1,    19,    -1,    -1,    -1,    23,    24,    68,    69,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,
    81,    -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,
    -1,    92,    93,    -1,     4,     5,    -1,     7,    -1,   100,
    10,    11,    12,    13,    14,    15,    16,    -1,    -1,    19,
    68,    69,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    80,    81,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    43,    92,    93,    -1,     4,     5,    -1,
     7,    -1,   100,    10,    11,    12,    -1,    14,    15,    16,
    -1,    -1,    19,    -1,    -1,    -1,    23,    -1,    68,    69,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    80,    81,    -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,
    -1,    -1,    92,    93,    18,    19,    20,    21,    22,    -1,
   100,    -1,    26,    27,    28,    29,    30,    -1,    32,    33,
    -1,    68,    69,    -1,    -1,    -1,    -1,    41,    42,    -1,
    -1,    -1,    -1,    80,    81,    -1,    -1,     1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    92,    93,    -1,    -1,    -1,
    -1,    -1,    -1,   100,    18,    19,    20,    21,    22,    23,
    24,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    -1,
    -1,    45,     1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    18,
    19,    20,    21,    22,    23,    24,    -1,    26,    27,    28,
    29,    30,    31,    32,    33,    -1,    35,    36,    -1,    -1,
    -1,    -1,    41,    42,    -1,    -1,    45,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    20,    21,
    22,    -1,    61,    62,    26,    27,    28,    29,    30,    -1,
    32,    33,    -1,    18,    19,    20,    21,    22,    -1,    41,
    42,    26,    27,    28,    29,    30,    -1,    32,    33,    -1,
    18,    19,    20,    21,    22,    -1,    41,    42,    26,    27,
    28,    29,    30,    -1,    32,    33,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    41,    42
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 462 "basic.yacc"
{ yyval.program=CProgram::GetParsingProgram(); CProgram::GetParsingProgram()->SetCode(yyvsp[0].statement); CProgram::GetParsingProgram()->SetDC(CProgram::GetParsingProgram()->GetDataStatementScope()->DataStatements()); ;
    break;}
case 2:
#line 471 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kInteger); ;
    break;}
case 3:
#line 472 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kString); ;
    break;}
case 4:
#line 473 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kFloat); ;
    break;}
case 13:
#line 492 "basic.yacc"
{
			yyval.intStorage=static_cast<CIntegerSymbol*>(yyvsp[0].variable);
		;
    break;}
case 14:
#line 496 "basic.yacc"
{
			yyval.intStorage=0L;			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
		;
    break;}
case 15:
#line 501 "basic.yacc"
{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-1].genExp,yylsp[0]))
			{
				yyval.intStorage=new CIntegerArrayItem(static_cast<CIntegerArraySymbol*>(yyvsp[-2].array),yyvsp[-1].genExp);
				UTBException::ThrowIfNoParserMemory(yyval.intStorage);
			}
			else
				yyval.intStorage=0L;
		;
    break;}
case 16:
#line 512 "basic.yacc"
{
			yyval.strStorage=static_cast<CStrSymbol*>(yyvsp[0].variable);
		;
    break;}
case 17:
#line 516 "basic.yacc"
{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-1].genExp,yylsp[0]))
			{
				yyval.strStorage=new CStrArrayItem(static_cast<CStrArraySymbol*>(yyvsp[-2].array),yyvsp[-1].genExp);
				UTBException::ThrowIfNoParserMemory(yyval.strStorage);
			}
			else
				yyval.strStorage=0L;
		;
    break;}
case 18:
#line 526 "basic.yacc"
{
			yyval.strStorage=0L;			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
		;
    break;}
case 19:
#line 532 "basic.yacc"
{
			yyval.floatStorage=static_cast<CFloatSymbol*>(yyvsp[0].variable);
		;
    break;}
case 20:
#line 536 "basic.yacc"
{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-1].genExp,yylsp[0]))
			{
				yyval.floatStorage=new CFloatArrayItem(static_cast<CFloatArraySymbol*>(yyvsp[-2].array),yyvsp[-1].genExp);
				UTBException::ThrowIfNoParserMemory(yyval.floatStorage);
			}
			else
				yyval.floatStorage=0L;
		;
    break;}
case 21:
#line 546 "basic.yacc"
{
			yyval.floatStorage=0L;			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
		;
    break;}
case 22:
#line 553 "basic.yacc"
{ if (!yyvsp[0].intStorage) yyval.storage=0L; else { yyval.storage=new CPrimitiveStorage(yyvsp[0].intStorage); UTBException::ThrowIfNoParserMemory(yyval.storage); } ;
    break;}
case 23:
#line 554 "basic.yacc"
{ if (!yyvsp[0].floatStorage) yyval.storage=0L; else { yyval.storage=new CPrimitiveStorage(yyvsp[0].floatStorage); UTBException::ThrowIfNoParserMemory(yyval.storage); } ;
    break;}
case 24:
#line 555 "basic.yacc"
{ if (!yyvsp[0].strStorage) yyval.storage=0L; else { yyval.storage=new CPrimitiveStorage(yyvsp[0].strStorage); UTBException::ThrowIfNoParserMemory(yyval.storage); } ;
    break;}
case 25:
#line 556 "basic.yacc"
{ delete yyvsp[-1].genExp; yyval.storage=0; ;
    break;}
case 26:
#line 557 "basic.yacc"
{ yyval.storage=0; ;
    break;}
case 27:
#line 558 "basic.yacc"
{ yyval.storage=0L; ;
    break;}
case 28:
#line 566 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[0].variable); UTBException::ThrowIfNoParserMemory(yyval.variableNode); ;
    break;}
case 29:
#line 567 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[-1].array); UTBException::ThrowIfNoParserMemory(yyval.variableNode); ;
    break;}
case 30:
#line 568 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[-2].variable); UTBException::ThrowIfNoParserMemory(yyval.variableNode); ConsData(yyval.variableNode,yyvsp[0].variableNode); ;
    break;}
case 31:
#line 569 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[-3].array); UTBException::ThrowIfNoParserMemory(yyval.variableNode); ConsData(yyval.variableNode,yyvsp[0].variableNode); ;
    break;}
case 32:
#line 575 "basic.yacc"
{ yyval.variable=yyvsp[0].variable; CProgram::GetParsingProgram()->PauseDeclarationMode(); ;
    break;}
case 33:
#line 581 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[0].variable); UTBException::ThrowIfNoParserMemory(yyval.variableNode); ;
    break;}
case 34:
#line 584 "basic.yacc"
{
			yyval.variableNode=0;
			FlushExp();
			yyclearin;
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalExp));			

			// resume declaration mode pause by the reduction of initedVariableLValue
			CProgram::GetParsingProgram()->ResumeDeclarationMode();
		;
    break;}
case 35:
#line 594 "basic.yacc"
{
			yyval.variableNode=0L;
			
			if (CGeneralExpressionErr::ContainsErrors(yyvsp[0].genExp))
				yyval.variableNode=0L;
			else if (yyvsp[-2].variable && !yyvsp[-2].variable->IsUndeclared())
			{
				// Create the variable node
				yyval.variableNode=new CVariableNode(yyvsp[-2].variable);

				Try_
				{
					UTBException::ThrowIfNoParserMemory(yyval.variableNode);
					
					yyval.variableNode->mInitStatement=MakeAssignmentStatement(CPrimitiveStorage(yyvsp[-2].variable),yyvsp[0].genExp,yylsp[0]);
				}
				Catch_(err)
				{
					// log the error
					delete yyval.variableNode;	// don't leak this on error
					yyval.variableNode=0L;		// IMPORTANT!!
					CProgram::GetParsingProgram()->LogError(err);
				}
			}
			
			// resume declaration mode pause by the reduction of initedVariableLValue
			CProgram::GetParsingProgram()->ResumeDeclarationMode();
		;
    break;}
case 36:
#line 623 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));			
			yyval.variableNode=0;
		;
    break;}
case 37:
#line 628 "basic.yacc"
{
			// allow nil lists as this is a declaration
			if (CGeneralExpression::IsNullList(yyvsp[-1].genExp))	// list can be null (I think...)
			{
				yyval.variableNode=new CVariableNode(yyvsp[-2].array); UTBException::ThrowIfNoParserMemory(yyval.variableNode);
				delete yyvsp[-1].genExp;
			}
			else if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-1].genExp,yylsp[0]))
			{
				if (!yyvsp[-1].genExp)
				{
					yyval.variableNode=new CVariableNode(yyvsp[-2].array); UTBException::ThrowIfNoParserMemory(yyval.variableNode);
				}
				else
				{
					// Create an array dimensioning statement
					if (yyvsp[-2].array->GetType()==CVariableSymbol::kUndeclaredArray)
					{
						// undeclared arrays already reported in lexer
						delete yyvsp[-1].genExp;
						yyval.variableNode=0;
					}
					else
					{
						CArrayDimEntry	*de=new CArrayDimEntry(yyvsp[-2].array,yyvsp[-1].genExp);
						UTBException::ThrowIfNoParserMemory(de);
						
						yyval.variableNode=new CVariableNode(yyvsp[-2].array);
						UTBException::ThrowIfNoParserMemory(yyval.variableNode);
						
						yyval.variableNode->mInitStatement=new CDimStatement(de);
						UTBException::ThrowIfNoParserMemory(yyval.variableNode->mInitStatement);
					}
				}
			}
			else
				yyval.variableNode=0;
		;
    break;}
case 39:
#line 668 "basic.yacc"
{ yyval.variableNode=ConsData(yyvsp[-2].variableNode,yyvsp[0].variableNode); ;
    break;}
case 40:
#line 674 "basic.yacc"
{ yyval.variableNode=yyvsp[0].variableNode; CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); ;
    break;}
case 41:
#line 676 "basic.yacc"
{
			yyval.variableNode=0L;
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedATypeName));			
		;
    break;}
case 42:
#line 681 "basic.yacc"
{
			yyval.variableNode=0L;
			CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined);
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_NotAVariable));
			FlushList();
			yyclearin;
		;
    break;}
case 43:
#line 692 "basic.yacc"
{ yyval.variableNode=new CVariableNode(yyvsp[0].variable); UTBException::ThrowIfNoParserMemory(yyval.variableNode); CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); ;
    break;}
case 45:
#line 695 "basic.yacc"
{ yyval.variableNode=ConsData(yyvsp[-2].variableNode,yyvsp[0].variableNode); ;
    break;}
case 46:
#line 696 "basic.yacc"
{ yyval.variableNode=yyvsp[0].variableNode; LogException_(UTBException::ThrowWithRange(yylsp[-2],-10)); ;
    break;}
case 47:
#line 697 "basic.yacc"
{ yyval.variableNode=yyvsp[-2].variableNode; LogException_(UTBException::ThrowWithRange(yylsp[-2],-11)); FlushList(); yyclearin; ;
    break;}
case 48:
#line 701 "basic.yacc"
{ yyval.variableNode=yyvsp[0].variableNode; CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); ;
    break;}
case 49:
#line 702 "basic.yacc"
{ yyval.variableNode=0L; LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedATypeName)); ;
    break;}
case 50:
#line 704 "basic.yacc"
{
			CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined);
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_NotAVariable));
			FlushList();
			yyclearin;
			yyval.variableNode=0;
		;
    break;}
case 51:
#line 719 "basic.yacc"
{
			if (yyvsp[-1].variable->GetType()!=CVariableSymbol::kUndeclaredPrimitive)	// if it's undefined then the error will have already been reported
				LogException_(UTBException::ThrowProcNotDefined(yylsp[-1],yyvsp[-1].variable->GetString()));
				
			yyval.string=0;
		;
    break;}
case 52:
#line 725 "basic.yacc"
{ yyval.string=yyvsp[-1].string; ;
    break;}
case 53:
#line 726 "basic.yacc"
{ yyval.string=0; LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_MangledProcedureCall)); ;
    break;}
case 54:
#line 730 "basic.yacc"
{
			delete yyvsp[-3].string;
			yyval.statement=0L;
			
			LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_IllegalExp));									
		;
    break;}
case 55:
#line 737 "basic.yacc"
{
			StStrDeleter	delboy(yyvsp[-3].string);
			if (!CGeneralExpressionErr::ContainsErrors(yyvsp[-2].genExp))
			{
				yyval.statement=new CCallProc(CProgram::GetParsingProgram(),yyvsp[-3].string,yyvsp[-2].genExp,yylsp[-1]);
				UTBException::ThrowIfNoParserMemory(yyval.statement);
			}
			else
			{
				delete yyvsp[-2].genExp;
				yyval.statement=0;
			}
		;
    break;}
case 56:
#line 752 "basic.yacc"
{ yyval.variableNode=0L; ;
    break;}
case 58:
#line 758 "basic.yacc"
{
			StStrDeleter		delboy(yyvsp[-1].string);
		
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
			{
				LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_CantPutProcsInProcs));
				yyval.procedureDef=0L;
			}
			else
			{
				yyval.procedureDef=new CProcedure(yyvsp[-1].string);
				UTBException::ThrowIfNoParserMemory(yyval.procedureDef);
				
				CProgram::GetParsingProgram()->PushParsingScope(&yyval.procedureDef->mIdentifierScope);
				CProgram::GetParsingProgram()->SetParserInProcedure(yyval.procedureDef);
			}
		;
    break;}
case 59:
#line 776 "basic.yacc"
{
			StStrDeleter		delboy(yyvsp[-1].string);
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_ExpectedOpenBracket));
			yyval.procedureDef=0;		
		;
    break;}
case 60:
#line 782 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalProcedureName));
			yyval.procedureDef=0;
		;
    break;}
case 61:
#line 787 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalProcedureName));
			yyval.procedureDef=0;
		;
    break;}
case 62:
#line 794 "basic.yacc"
{
			if (yyvsp[-5].procedureDef)
			{
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
				delete yyvsp[-5].procedureDef;
			}
			delete yyvsp[-2].statement;
			delete yyvsp[-1].statement;			
			LogException_(UTBException::ThrowWithRange(yylsp[-4],kTBErr_SyntaxError));
			yyval.statement=0L;
		;
    break;}
case 63:
#line 807 "basic.yacc"
{
			if (yyvsp[-1].procedureDef)
			{
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
				delete yyvsp[-1].procedureDef;
			}
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_SyntaxError));
			yyval.statement=0L;		
		;
    break;}
case 64:
#line 818 "basic.yacc"
{
			if (yyvsp[-5].procedureDef)
			{
				LogException_(yyvsp[-5].procedureDef->SetParameters(yylsp[-4],yyvsp[-4].variableNode));
				CProgram::GetParsingProgram()->AddProcDef(yyvsp[-5].procedureDef);
				yyvsp[-5].procedureDef->SetCode(ConsData(yyvsp[-2].statement,yyvsp[-1].statement));
				yyval.statement=0;
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
			}
			else
			{
				delete yyvsp[-4].variableNode;
				delete yyvsp[-2].statement;
				delete yyvsp[-1].statement;
				yyval.statement=0;
			}
		;
    break;}
case 65:
#line 839 "basic.yacc"
{
			yyval.statement=new CEndProcStatement(0,0);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		;
    break;}
case 66:
#line 844 "basic.yacc"
{
			if (!yyvsp[-1].genExp || yyvsp[-1].genExp->IsNullExp() || CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))
				yyval.statement=new CEndProcStatement(0,0);
			else
			{
				if (yyvsp[-1].genExp->GetArithExp())
					yyval.statement=new CEndProcStatement(yyvsp[-1].genExp->ReleaseArithExp(),0);
				else
					yyval.statement=new CEndProcStatement(0,yyvsp[-1].genExp->ReleaseStrExp());
			}
			delete yyvsp[-1].genExp;
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		;
    break;}
case 67:
#line 858 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));

			yyval.statement=new CEndProcStatement(0,0);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		;
    break;}
case 68:
#line 873 "basic.yacc"
{
			if (CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))
			{
				yyval.statement=0;
				delete yyvsp[-1].genExp;
			}
			else		
				yyval.statement=UFunctionMgr::MakeStatement(yyvsp[-2].tokenId,yyvsp[-1].genExp,yylsp[0]);
		;
    break;}
case 69:
#line 883 "basic.yacc"
{
			yyval.statement=0L;
			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));			
		;
    break;}
case 70:
#line 891 "basic.yacc"
{
			STextRange	range(yylsp[0]);
			range.startOffset=range.endOffset;
			range.endOffset++;
			yyval.arithExp=UFunctionMgr::MakeArithFunction(yyvsp[0].tokenId,0L,range,range,yylsp[0],false);
		;
    break;}
case 71:
#line 898 "basic.yacc"
{
			yyval.arithExp=0L; FlushExp(); yyclearin;
			
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_ExpectedOpenBracket));
		;
    break;}
case 72:
#line 904 "basic.yacc"
{
			if (CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))
			{
				yyval.arithExp=0;
				delete yyvsp[-1].genExp;
			}
			else
			{		
				yyval.arithExp=UFunctionMgr::MakeArithFunction(yyvsp[-3].tokenId,yyvsp[-1].genExp,yylsp[-2],yylsp[0],STextRange(yylsp[-3],yylsp[0]),true);
			}
		;
    break;}
case 73:
#line 916 "basic.yacc"
{
			yyval.arithExp=0L;
			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));			
		;
    break;}
case 74:
#line 924 "basic.yacc"
{
			STextRange	range(yylsp[0]);
			range.startOffset=range.endOffset;
			range.endOffset++;
			yyval.strExp=UFunctionMgr::MakeStrFunction(yyvsp[0].tokenId,0L,range,range,yylsp[0],false);
		;
    break;}
case 75:
#line 931 "basic.yacc"
{
			yyval.strExp=0L; FlushExp(); yyclearin;
			
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_ExpectedOpenBracket));
		;
    break;}
case 76:
#line 943 "basic.yacc"
{
			if (CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))
			{
				yyval.strExp=0;
				delete yyvsp[-1].genExp;
			}
			else
			{		
				yyval.strExp=UFunctionMgr::MakeStrFunction(yyvsp[-3].tokenId,yyvsp[-1].genExp,yylsp[-2],yylsp[0],STextRange(yylsp[-3],yylsp[0]),true);
			}
		;
    break;}
case 77:
#line 955 "basic.yacc"
{
			yyval.strExp=0L;
			
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
		;
    break;}
case 78:
#line 973 "basic.yacc"
{ yyval.arithExp=yyvsp[-1].arithExp; ;
    break;}
case 79:
#line 974 "basic.yacc"
{ StStrDeleter delboy(yyvsp[0].string); TBArithFunction_(CInteger(yylsp[0],yyvsp[0].string)); ;
    break;}
case 80:
#line 975 "basic.yacc"
{ StStrDeleter delboy(yyvsp[0].string); TBArithFunction_(CFloat(yylsp[0],yyvsp[0].string)); ;
    break;}
case 81:
#line 976 "basic.yacc"
{ yyval.arithExp=yyvsp[0].arithExp; ;
    break;}
case 82:
#line 977 "basic.yacc"
{ TBArithFunction_(CArithProcResult(yylsp[0],false)); ;
    break;}
case 83:
#line 978 "basic.yacc"
{ TBArithFunction_(CArithProcResult(yylsp[0],true)); ;
    break;}
case 84:
#line 979 "basic.yacc"
{ TBMaybeArithExp1_(yyvsp[0].intStorage,CIntegerIdentifier(yylsp[0],yyvsp[0].intStorage)); ;
    break;}
case 85:
#line 980 "basic.yacc"
{ TBMaybeArithExp1_(yyvsp[0].floatStorage,CFloatIdentifier(yylsp[0],yyvsp[0].floatStorage)); ;
    break;}
case 86:
#line 981 "basic.yacc"
{ TBMaybeArithExp1_(yyvsp[0].arithExp,CUnaryMinusOp(STextRange(yylsp[-1],yylsp[0]),yyvsp[0].arithExp)); ;
    break;}
case 87:
#line 982 "basic.yacc"
{ TBMaybeArithExp1_(yyvsp[0].arithExp,CNotOp(STextRange(yylsp[-1],yylsp[0]),yyvsp[0].arithExp));;
    break;}
case 88:
#line 983 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryArithOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,PLUS)); ;
    break;}
case 89:
#line 984 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryArithOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,MINUS)); ;
    break;}
case 90:
#line 985 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryArithOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,TIMES)); ;
    break;}
case 91:
#line 986 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryArithOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,DIVIDE)); ;
    break;}
case 92:
#line 987 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryArithOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,POWER)); ;
    break;}
case 93:
#line 988 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,AND)); ;
    break;}
case 94:
#line 989 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,OR)); ;
    break;}
case 96:
#line 999 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,EQUALS)); ;
    break;}
case 97:
#line 1000 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,NEQ)); ;
    break;}
case 98:
#line 1001 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,LT)); ;
    break;}
case 99:
#line 1002 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,LTE)); ;
    break;}
case 100:
#line 1003 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,GT)); ;
    break;}
case 101:
#line 1004 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].arithExp,yyvsp[0].arithExp,CBinaryLogicOp(yyvsp[-2].arithExp,yyvsp[0].arithExp,GTE)); ;
    break;}
case 102:
#line 1005 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,EQUALS)); ;
    break;}
case 103:
#line 1006 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,NEQ)); ;
    break;}
case 104:
#line 1007 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,LT)); ;
    break;}
case 105:
#line 1008 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,LTE)); ;
    break;}
case 106:
#line 1009 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,GT)); ;
    break;}
case 107:
#line 1010 "basic.yacc"
{ TBMaybeArithExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CBinaryLogicOp(yyvsp[-2].strExp,yyvsp[0].strExp,GTE)); ;
    break;}
case 108:
#line 1011 "basic.yacc"
{ TBArithFunction_(CInteger(yylsp[0],"1")); ;
    break;}
case 109:
#line 1012 "basic.yacc"
{ TBArithFunction_(CInteger(yylsp[0],"0")); ;
    break;}
case 110:
#line 1021 "basic.yacc"
{ TBStrFunction_(CStrProcResult(yylsp[0])); ;
    break;}
case 111:
#line 1022 "basic.yacc"
{ yyval.strExp=yyvsp[-1].strExp; ;
    break;}
case 112:
#line 1023 "basic.yacc"
{ TBMaybeStrExp1_(yyvsp[0].strStorage,CStrIdentifier(yylsp[0],yyvsp[0].strStorage)); ;
    break;}
case 113:
#line 1024 "basic.yacc"
{ TBMaybeStrExp2_(yyvsp[-2].strExp,yyvsp[0].strExp,CStrConcatOp(yyvsp[-2].strExp,yyvsp[0].strExp)); ;
    break;}
case 114:
#line 1025 "basic.yacc"
{ StStrDeleter delboy(yyvsp[0].string); TBStrFunction_(CStrLiteral(yylsp[0],yyvsp[0].string)); ;
    break;}
case 115:
#line 1026 "basic.yacc"
{ yyval.strExp=yyvsp[0].strExp; ;
    break;}
case 116:
#line 1036 "basic.yacc"
{ TBGeneralExp_(CGeneralExpression()); ;
    break;}
case 117:
#line 1037 "basic.yacc"
{ TBGenExpOrError_(yyvsp[0].arithExp,CGeneralExpression(yyvsp[0].arithExp)); ;
    break;}
case 118:
#line 1038 "basic.yacc"
{ TBGenExpOrError_(yyvsp[0].strExp,CGeneralExpression(yyvsp[0].strExp)); ;
    break;}
case 119:
#line 1039 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); ;
    break;}
case 120:
#line 1042 "basic.yacc"
{ yyval.genExp=yyvsp[0].genExp; ;
    break;}
case 121:
#line 1043 "basic.yacc"
{ yyval.genExp=yyvsp[-2].genExp; if (yyvsp[-2].genExp) yyvsp[-2].genExp->AppendExpression(CGeneralExpression::kComma,yylsp[-1],yyvsp[0].genExp); else yyval.genExp=yyvsp[0].genExp; ;
    break;}
case 122:
#line 1044 "basic.yacc"
{ yyval.genExp=yyvsp[-2].genExp; if (yyvsp[-2].genExp) yyvsp[-2].genExp->AppendExpression(CGeneralExpression::kTo,yylsp[-1],yyvsp[0].genExp); else yyval.genExp=yyvsp[0].genExp; ;
    break;}
case 123:
#line 1045 "basic.yacc"
{ yyval.genExp=yyvsp[-2].genExp; if (yyvsp[-2].genExp) yyvsp[-2].genExp->AppendExpression(CGeneralExpression::kSemiColon,yylsp[-1],yyvsp[0].genExp); else yyval.genExp=yyvsp[0].genExp; ;
    break;}
case 124:
#line 1046 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_IllegalExp)); ;
    break;}
case 125:
#line 1047 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_IllegalExp)); ;
    break;}
case 126:
#line 1048 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_IllegalExp)); ;
    break;}
case 127:
#line 1049 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalExp)); FlushExp(); yyclearin;;
    break;}
case 128:
#line 1050 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalExp)); FlushExp(); yyclearin;;
    break;}
case 129:
#line 1051 "basic.yacc"
{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalExp)); FlushExp(); yyclearin;;
    break;}
case 130:
#line 1059 "basic.yacc"
{ yyval.statement=0L; ;
    break;}
case 131:
#line 1060 "basic.yacc"
{ yyval.statement=ConsData(yyvsp[-1].statement,yyvsp[0].statement); ;
    break;}
case 132:
#line 1061 "basic.yacc"
{ LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_SyntaxError));;
    break;}
case 152:
#line 1068 "basic.yacc"
{ yyval.statement=0L; ;
    break;}
case 153:
#line 1078 "basic.yacc"
{
			if (yyvsp[-1].variableNode)	// var list can be nil if there were errors in parsing it
			{
				yyval.statement=ExtractInitStatements(yyvsp[-1].variableNode);
				delete yyvsp[-1].variableNode;
			}
			else
				yyval.statement=0L;
		;
    break;}
case 154:
#line 1088 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ ;
    break;}
case 155:
#line 1091 "basic.yacc"
{
			if (yyvsp[-1].variableNode)
			{
				yyval.statement=ExtractInitStatements(yyvsp[-1].variableNode);
				CProgram::GetParsingProgram()->AddGlobals(yyvsp[-1].variableNode,yylsp[-2],false,false); /* store all globals in a list in the program */
			}
			else
				yyval.statement=0L;
		;
    break;}
case 156:
#line 1101 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ ;
    break;}
case 157:
#line 1104 "basic.yacc"
{
			if (yyvsp[-1].variableNode)
			{
				yyval.statement=ExtractInitStatements(yyvsp[-1].variableNode);
				CProgram::GetParsingProgram()->AddGlobals(yyvsp[-1].variableNode,yylsp[-2],true,false); /* store all globals in a list in the program */
			}
			else
				yyval.statement=0L;
		;
    break;}
case 158:
#line 1114 "basic.yacc"
{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ ;
    break;}
case 159:
#line 1117 "basic.yacc"
{
			if (yyvsp[-1].variableNode)
			{
				yyval.statement=ExtractInitStatements(yyvsp[-1].variableNode);
				CProgram::GetParsingProgram()->AddGlobals(yyvsp[-1].variableNode,yylsp[-2],true,true); /* store all globals in a list in the program */
			}
			else
				yyval.statement=0L;
		;
    break;}
case 160:
#line 1131 "basic.yacc"
{
			// Must be in a procedure to use shared (error will get logged in sharedStatement)
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
				CProgram::GetParsingProgram()->PauseParsingScope();
/*			CProgram::GetParsingProgram()->SetReportVarNotDecl(false); */
		;
    break;}
case 161:
#line 1139 "basic.yacc"
{
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
				CProgram::GetParsingProgram()->ResumeParsingScope(&CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->mIdentifierScope);

			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_SyntaxError));
			yyval.statement=0L;
		;
    break;}
case 162:
#line 1147 "basic.yacc"
{
			yyval.statement=0L;

			// Must be in a procedure and must also not have declared any variables which are not global.
			if (!CProgram::GetParsingProgram()->IsParserInProcedure())
				LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_SharedMustBeUsedInAProcedure));
			else
			{		
				// Push the scope back and enter all variables as guests
				CIdentifierScope		*scope=&CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->mIdentifierScope;
				
				CProgram::GetParsingProgram()->ResumeParsingScope(scope);

				for (CVariableNode *node=yyvsp[-1].variableNode; node; node=node->TailData())
				{
					// Check if a parameter has the same name, if so we can't share it
					CVariableSymbol		*symbol;
					
					symbol=scope->VarTable()->Exists(node->mSymbol->GetString());
					
					// If the symbol already exists and it's not a guest (ie a global) then it must be a parameter
					// or a previously declared variable. Neither can be shared.
					if (symbol && !scope->VarTable()->IsGuest(symbol))
					{
						if (CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->IsParameter(symbol))
							LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_CantShareParam));
						else						
							LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_AttemptToShareDeclaredVar));

						continue;	// skip this one
					}
					
					scope->VarTable()->EnterGuest(node->mSymbol);
				}
			}
						
			delete yyvsp[-1].variableNode;			
		;
    break;}
case 166:
#line 1189 "basic.yacc"
{  TBStatement_(CCanvasFXStatement(static_cast<CIntegerArraySymbol*>(yyvsp[-6].array),yyvsp[-3].arithExp,yyvsp[-1].arithExp)); ;
    break;}
case 167:
#line 1191 "basic.yacc"
{ yyval.statement=0L; LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp)); ;
    break;}
case 168:
#line 1194 "basic.yacc"
{ TBStatement_(CSetNetDataStatement(yyvsp[-3].variable,yyvsp[-1].arithExp)); ;
    break;}
case 169:
#line 1196 "basic.yacc"
{ TBStatement_(CSetNetDataStatement(yyvsp[-3].variable,yyvsp[-1].arithExp)); ;
    break;}
case 170:
#line 1198 "basic.yacc"
{ TBStatement_(CSetNetDataStatement(yyvsp[-3].variable,yyvsp[-1].strExp)); ;
    break;}
case 171:
#line 1200 "basic.yacc"
{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-4].genExp,yylsp[-3]))
								{
									TBStatement_(CSetNetDataStatement(yyvsp[-5].array,yyvsp[-1].arithExp,yyvsp[-4].genExp));
								}
							;
    break;}
case 172:
#line 1207 "basic.yacc"
{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-4].genExp,yylsp[-3]))
								{
									TBStatement_(CSetNetDataStatement(yyvsp[-5].array,yyvsp[-1].arithExp,yyvsp[-4].genExp));
								}
							;
    break;}
case 173:
#line 1214 "basic.yacc"
{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-4].genExp,yylsp[-3]))
								{
									TBStatement_(CSetNetDataStatement(yyvsp[-5].array,yyvsp[-1].strExp,yyvsp[-4].genExp));
								}
							;
    break;}
case 174:
#line 1221 "basic.yacc"
{ yyval.statement=NULL; LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp)); ;
    break;}
case 175:
#line 1230 "basic.yacc"
{
		// if they use an undeclared variable it will be logged by the lexer, now we must sensibly parse the remaining expression
		if (yyvsp[-2].array->GetType()==CVariableSymbol::kUndeclaredArray)
		{
			delete yyvsp[-1].genExp;
			yyval.arrayEntry=0;
		}
		else if (CheckArrayIndexList(*CProgram::GetParsingProgram(),yyvsp[-1].genExp,yylsp[0]))
		{
			yyval.arrayEntry=new CArrayDimEntry(yyvsp[-2].array,yyvsp[-1].genExp);
			UTBException::ThrowIfNoParserMemory(yyval.arrayEntry);
		}
		else
			yyval.arrayEntry=0L;
	;
    break;}
case 176:
#line 1246 "basic.yacc"
{
		yyval.arrayEntry=0L;
		LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp))		
	;
    break;}
case 178:
#line 1253 "basic.yacc"
{ yyval.arrayEntry=ConsData(yyvsp[-2].arrayEntry,yyvsp[0].arrayEntry); ;
    break;}
case 179:
#line 1255 "basic.yacc"
{ CProgram::GetParsingProgram()->SetInDimStatement(true); ;
    break;}
case 180:
#line 1258 "basic.yacc"
{
		if (!yyvsp[-1].arrayEntry)
			yyval.statement=0L;
		else
		{
			yyval.statement=new CDimStatement(yyvsp[-1].arrayEntry);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		}
		CProgram::GetParsingProgram()->SetInDimStatement(false);
	;
    break;}
case 181:
#line 1269 "basic.yacc"
{
		yyval.statement=0L;
		LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
	;
    break;}
case 182:
#line 1276 "basic.yacc"
{
			yyval.statement=0L;
			if (CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))
				delete yyvsp[-1].genExp;
			else if (yyvsp[-1].genExp)
			{
				CLabelTable::GetCurrentLabelTable()->DataStatementCreated(yyvsp[-1].genExp);
				CProgram::GetParsingProgram()->GetDataStatementScope()->AddDataStatement(yyvsp[-1].genExp);
			}
		;
    break;}
case 183:
#line 1287 "basic.yacc"
{
			yyval.statement=0L;
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
		;
    break;}
case 184:
#line 1294 "basic.yacc"
{
			CProgram::GetParsingProgram()->SetWantLabels(true);
		;
    break;}
case 185:
#line 1299 "basic.yacc"
{
			StStrDeleter		delboy(yyvsp[-1].string);
			CProgram::GetParsingProgram()->SetWantLabels(false);
			yyval.statement=new CRestoreStatement(yyvsp[-1].string,*CProgram::GetParsingProgram(),yylsp[-1]);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		;
    break;}
case 186:
#line 1308 "basic.yacc"
{
			CProgram::GetParsingProgram()->SetWantLabels(true);
		;
    break;}
case 187:
#line 1313 "basic.yacc"
{
			StStrDeleter		delboy(yyvsp[-1].string);
			CProgram::GetParsingProgram()->SetWantLabels(false);
			yyval.statement=new CGotoStatement(yyvsp[-1].string,*CProgram::GetParsingProgram(),yylsp[-1]);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
		;
    break;}
case 188:
#line 1344 "basic.yacc"
{
			CArithExpression	*exp=CheckSingleNumExp(yyvsp[-1].genExp,yylsp[-1]);
						
			if (exp)
				TBStatement_(CConditionalBlock(false,exp));
			else
				yyval.conditionalBlock=0L;
		;
    break;}
case 189:
#line 1353 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedNumExp));
			yyval.conditionalBlock=0;
		;
    break;}
case 190:
#line 1359 "basic.yacc"
{
			CArithExpression	*exp=CheckSingleNumExp(yyvsp[-1].genExp,yylsp[-1]);
						
			if (exp)
			{
				// an statement gets its line num by taking 1 from the line count
				// this is because it expects the ENDLINE token to have been parsed, which
				// causes the line num to be increased. With an if then, the ENDLINE has not
				// been parsed by when the expression is contructed, therefore we need to
				// add one to the line number stored by the parsed expression so far.
				gLineno++;
				TBStatement_(CConditionalBlock(false,exp));
				gLineno--;
			}
			else
				yyval.conditionalBlock=0L;
		;
    break;}
case 191:
#line 1377 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedNumExp));
			yyval.conditionalBlock=0;	
		;
    break;}
case 192:
#line 1383 "basic.yacc"
{
			if (yyvsp[-1].conditionalBlock)
			{
				// we don't need endif breaks and a surrounding statement block for
				// a simple ifThenStatement
				yyvsp[-1].conditionalBlock->SetSubStatements(yyvsp[0].statement);
				yyval.statement=yyvsp[-1].conditionalBlock;
			}
			else
			{
				delete yyvsp[0].statement;
				yyval.statement=0;
			}
		;
    break;}
case 193:
#line 1399 "basic.yacc"
{
			if (yyvsp[-5].conditionalBlock)
			{
				CConditionalBlock	*&conditionalsList=yyvsp[-5].conditionalBlock;
				
				// set the sub statements of the conditional block
				conditionalsList->SetSubStatements(yyvsp[-4].statement);
				
				// append the remainder of the ifstruct
				conditionalsList->Append(yyvsp[-3].statement);
				
				// Now we have a list of conditional blocks, the if and each of the end ifs
				// Apply end if breaks to each so that if one executes it jumps out of the
				// if statement thus avoiding executing the other conditionals
				AppendEndIfBreaks(conditionalsList);

				if (yyvsp[-3].statement)
					yyvsp[-3].statement->Append(yyvsp[-2].statement);
				else
					conditionalsList->Append(yyvsp[-2].statement);
				
				// Finally construct a statement block to enclose all the if statements
				// This groups together the conditionals and gives them somewhere to
				// jump out of when one of them finishes.
				yyval.statement=new CStatementBlock(false,conditionalsList);
				UTBException::ThrowIfNoParserMemory(yyval.statement);
			}
			else
			{
				yyval.statement=0L;
				delete yyvsp[-5].conditionalBlock;
				delete yyvsp[-3].statement;
				delete yyvsp[-2].statement;
			}
		;
    break;}
case 194:
#line 1447 "basic.yacc"
{
			CArithExpression	*exp=CheckSingleNumExp(yyvsp[-1].genExp,yylsp[-1]);
			
			if (exp)
				TBStatement_(CConditionalBlock(false,exp));
			else
				yyval.conditionalBlock=0;
		;
    break;}
case 195:
#line 1456 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedNumExp));
			yyval.conditionalBlock=0L;
		;
    break;}
case 196:
#line 1461 "basic.yacc"
{ yyval.statement=0L; ;
    break;}
case 197:
#line 1463 "basic.yacc"
{
			yyval.statement=yyvsp[-2].conditionalBlock;
			
			if (yyvsp[-2].conditionalBlock)
			{
				yyvsp[-2].conditionalBlock->SetSubStatements(yyvsp[-1].statement);
				yyvsp[-2].conditionalBlock->Append(yyvsp[0].statement);
			}
			else
			{
				delete yyvsp[-1].statement;
				delete yyvsp[0].statement;
			}
		;
    break;}
case 198:
#line 1480 "basic.yacc"
{ yyval.statement=0L; ;
    break;}
case 199:
#line 1481 "basic.yacc"
{ TBStatement_(CStatementBlock(false,yyvsp[0].statement)); ;
    break;}
case 200:
#line 1489 "basic.yacc"
{
			CArithExpression	*exp=CheckSingleNumExp(yyvsp[-1].genExp,yylsp[-1]);
			
			if (exp)
				TBStatement_(CConditionalBlock(true,exp));
			else
			{
				yyval.conditionalBlock=0;
			}
		;
    break;}
case 201:
#line 1500 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedNumExp));
			yyval.conditionalBlock=0;
		;
    break;}
case 202:
#line 1506 "basic.yacc"
{
			if (yyvsp[-3].conditionalBlock)
			{
				yyval.statement=yyvsp[-3].conditionalBlock;
				yyvsp[-3].conditionalBlock->SetSubStatements(yyvsp[-2].statement);
				
				// Append the goto condition instruction to the end of the statement
				// block.
				CStatement		*gotoS=new CJumpStatement(yyval.statement);
				UTBException::ThrowIfNoParserMemory(gotoS);
				
				yyvsp[-3].conditionalBlock->AppendSubStatement(gotoS);
			}
			else
			{
				yyval.statement=0;
				delete yyvsp[-2].statement;
			}
		;
    break;}
case 203:
#line 1539 "basic.yacc"
{
			CArithExpression	*exp=CheckSingleNumExp(yyvsp[-1].genExp,yylsp[-1]);
			
			if (exp)
			{
				CStatementBlock	*block;
				
				yyval.statement=block=new CStatementBlock(true,yyvsp[-3].statement);
				UTBException::ThrowIfNoParserMemory(yyval.statement);
				
				// Now add the conditional jump which gives the looping action
				CStatement		*condJump=new CConditionalJump(exp,block,false);
				UTBException::ThrowIfNoParserMemory(condJump);
				
				block->AppendSubStatement(condJump);
			}
			else
			{
				yyval.statement=0;
				delete yyvsp[-3].statement;
			}
		;
    break;}
case 204:
#line 1562 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_ExpectedNumExp));
			delete yyvsp[-3].statement;
			yyval.statement=0;
		;
    break;}
case 205:
#line 1574 "basic.yacc"
{
			CStatementBlock	*block;
			
			yyval.statement=block=new CStatementBlock(true,yyvsp[-2].statement);
			UTBException::ThrowIfNoParserMemory(yyval.statement);
			
			// Now add the jump which gives the looping action
			CStatement		*jump=new CJumpStatement(block);
			UTBException::ThrowIfNoParserMemory(jump);
			
			block->AppendSubStatement(jump);												
		;
    break;}
case 206:
#line 1593 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[-2],kTBErr_IllegalExp));
			yyval.forStatement=0;
			delete yyvsp[-4].storage;
		;
    break;}
case 207:
#line 1599 "basic.yacc"
{
			LogException_(UTBException::ThrowBadForLoopIndex(yylsp[-4]));
			delete yyvsp[-2].genExp;
			yyval.forStatement=0;
		;
    break;}
case 208:
#line 1605 "basic.yacc"
{
			Try_
			{
				if (!CGeneralExpressionErr::ContainsErrors(yyvsp[-2].genExp))
				{
					UFunctionMgr::SimpleTypeCheck("ata",yyvsp[-2].genExp,yylsp[-1]);
										
					// Check storage
					CIntegerStorage		*store=yyvsp[-4].storage ? yyvsp[-4].storage->mIntStore : 0;

					if (!store)
					{
						if (yyvsp[-4].storage) // if store is defined then it's in error, otherwise it must have been boshed by an earlier parse error and shouldn't be reported again
							LogException_(UTBException::ThrowBadForLoopIndex(yylsp[-4]));
						yyval.forStatement=0L;
					}
					else
					{
						CArithExpression *from=yyvsp[-2].genExp->ReleaseArithExp();
						CArithExpression *to=yyvsp[-2].genExp->TailData()->ReleaseArithExp();

						TBStatement_(CForStatement(store,from,to,yyvsp[-1].arithExp));						
					}

					delete yyvsp[-4].storage;						
					delete yyvsp[-2].genExp;						
				}
				else
					yyval.forStatement=0L;
			}
			Catch_(err)
			{
				CProgram::GetParsingProgram()->LogError(err);
				delete yyvsp[-4].storage;
				delete yyvsp[-2].genExp;
				yyval.forStatement=0L;
			}			
		;
    break;}
case 209:
#line 1645 "basic.yacc"
{
			if (yyvsp[-3].forStatement)
			{
				// if the next statement contains an identifier check it matches
				// if it's an array index we can't statically check it as the array could
				// be indexed by a variable which would get out of its face quickly.
				bool				mismatch=false;
																	
				if (yyvsp[-1].intStorage)	// if next statement var is specified
				{
					CIntegerStorage		*indexVar=yyvsp[-3].forStatement->GetIndexStorage();
					CIntegerArrayItem	*indexArray=dynamic_cast<CIntegerArrayItem*>(indexVar);
					CIntegerArrayItem	*nextArray=dynamic_cast<CIntegerArrayItem*>(yyvsp[-1].intStorage);

					// if both arrays
					if (indexArray && nextArray)
						mismatch = nextArray->GetArray()!=indexArray->GetArray();
					else
						mismatch = indexVar!=yyvsp[-1].intStorage;

					if (mismatch)
						LogException_(UTBException::ThrowForLoopIdentifierMismatch(yylsp[-1]));
				}
			
				if (!mismatch)
				{
					yyval.statement=yyvsp[-3].forStatement;
					yyvsp[-3].forStatement->SetSubStatements(*CProgram::GetParsingProgram()->GetVarTable(),yyvsp[-2].statement);
				}
				else
				{
					delete yyvsp[-3].forStatement,
					delete yyvsp[-2].statement;
					yyval.statement=0L;
				}
			}
			else
			{
				delete yyvsp[-2].statement;
				yyval.statement=0l;
			}
		;
    break;}
case 210:
#line 1689 "basic.yacc"
{ yyval.arithExp=0L; ;
    break;}
case 211:
#line 1691 "basic.yacc"
{
			yyval.arithExp=CheckSingleNumExp(yyvsp[0].genExp,yylsp[0]);
		;
    break;}
case 212:
#line 1695 "basic.yacc"
{
			LogException_(UTBException::ThrowWithRange(yylsp[0],kTBErr_IllegalExp));
			FlushExp();
			yyclearin;
		;
    break;}
case 213:
#line 1702 "basic.yacc"
{ yyval.intStorage=0L; ;
    break;}
case 214:
#line 1704 "basic.yacc"
{
			if (yyvsp[0].storage)
			{
				if (yyvsp[0].storage->GetType()!=CVariableSymbol::kInteger)
				{
					yyval.intStorage=0;
					LogException_(UTBException::ThrowBadForLoopIndex(yylsp[0]));
				}
				else
					yyval.intStorage=yyvsp[0].storage->mIntStore;
				
				delete yyvsp[0].storage;
			}
			else
				yyval.intStorage=0;
		;
    break;}
case 215:
#line 1721 "basic.yacc"
{
			LogException_(UTBException::ThrowBadForLoopIndex(yylsp[0]));
			FlushExp();
			yyclearin;
		;
    break;}
case 216:
#line 1738 "basic.yacc"
{
							if (yyvsp[-3].storage)
							{
								yyval.statement=0;
								if (!CGeneralExpressionErr::ContainsErrors(yyvsp[-1].genExp))								
									LogException_(yyval.statement=MakeAssignmentStatement(*yyvsp[-3].storage,yyvsp[-1].genExp,yylsp[-1]));
								delete yyvsp[-3].storage;
							}
							else
							{
								delete yyvsp[-1].genExp;
								yyval.statement=0;
							}
						;
    break;}
case 217:
#line 1758 "basic.yacc"
{
							yyval.statement=0;
							LogException_(UTBException::ThrowWithRange(yylsp[-1],kTBErr_IllegalExp));
							delete yyvsp[-3].storage;
						;
    break;}
case 218:
#line 1764 "basic.yacc"
{ TBStatement_(CReadStatement(yyvsp[0].storage)); ;
    break;}
case 219:
#line 1765 "basic.yacc"
{ TBStatement_(CReadStatement(yyvsp[-2].storage)); yyval.statement->Append(yyvsp[0].statement); ;
    break;}
case 220:
#line 1768 "basic.yacc"
{ yyval.statement=yyvsp[-1].statement; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 1770 "basic.yacc"


int				gRepeatToken=-1;
YYSTYPE			gRepeatValue;
SBisonTokenLoc	gRepeatLoc;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PatchedLex
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Wraps around the normal lex, can fake a token into the input stream. Faking a token is needed to
// push back a token that has already been matched by the lexer.
int PatchedLex()
{
	int		result=0;

	if (gRepeatToken>=0)
	{
		result=gRepeatToken;
		gRepeatToken=-1;
		yylloc=gRepeatLoc;
		basiclval=gRepeatValue;
	}
	else
		result=basiclex();
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FlushList
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Drops tokens until an end line, unbalanced close bracket.
void FlushList()
{
	SInt32		bracket=0,sqbracket=0;
	int			token;
	bool		done=false;
	YYSTYPE		wasVal=basiclval;
	SBisonTokenLoc	wasLoc=yylloc;

	while (!done)
	{
		switch (token=basiclex())
		{
			case OPENSQBRACKET:
				sqbracket++;
				break;
		
			case OPENBRACKET:
				bracket++;
				break;
			
			case CLOSEBRACKET:
			case CLOSESQBRACKET:
				if (token==CLOSEBRACKET)
					bracket--;
				else
					sqbracket--;
				if ((bracket<=0) && (sqbracket<=0))
				{
					gRepeatToken=token;
					done=true;
				}
				break;
				
			case ENDLINE:
			case 0:	// EOF
				gRepeatToken=token;
				done=true;
				break;
		}
	}
	
	gRepeatLoc=yylloc;
	gRepeatValue=basiclval;
	
	// restore loc and values
	basiclval=wasVal;
	yylloc=wasLoc;
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FlushExp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Drops tokens until an end line, unbalanced close bracket or seperator in an attempt to skip an expression with an error
// in it. Returns the seperator to the input stream by way of a patched version of lex which is able to fake a token.
void FlushExp()
{
	SInt32		bracket=0,sqbracket=0;
	int			token;
	bool		done=false;
	YYSTYPE		wasVal=basiclval;
	SBisonTokenLoc	wasLoc=yylloc;

	while (!done)
	{
		switch (token=basiclex())
		{
			case OPENSQBRACKET:
				sqbracket++;
				break;
		
			case OPENBRACKET:
				bracket++;
				break;
			
			case CLOSEBRACKET:
			case CLOSESQBRACKET:
				if (token==CLOSEBRACKET)
					bracket--;
				else
					sqbracket--;
			case COMMA:
			case SEMICOLON:
			case TO:
				if ((bracket<=0) && (sqbracket<=0))
				{
					gRepeatToken=token;
					done=true;
				}
				break;
				
			case ENDLINE:
			case 0:	// EOF
				gRepeatToken=token;
				done=true;
				break;
		}
	}
	
	gRepeatLoc=yylloc;
	gRepeatValue=basiclval;
	
	// restore loc and values
	basiclval=wasVal;
	yylloc=wasLoc;
}

int yyerror(char * s)
{
//	LogException_(UTBException::ThrowWithRange(yylloc,kTBErr_UnmatchedSyntaxError));
//	SignalPStr_("\pUncategorised syntax error, logging...");
	
	return 1;	// continue
}
