%{
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

%}

%union
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
};

// The tokens used by the parser (same as the ones used by the lexer)
%token REM			// only used by writers' block
%token <string>		INTEGER STRLITERAL LABEL FLOAT PROCIDENTIFIER LABELIDENTIFIER
%token <variable>	INTIDENTIFIER STRIDENTIFIER FLOATIDENTIFIER UNDECLAREDPRIMITIVE
%token <array>		INTARRAYIDENTIFIER STRARRAYIDENTIFIER FLOATARRAYIDENTIFIER UNDECLAREDARRAY
%token PLUS MINUS DIVIDE TIMES POWER OPENBRACKET CLOSEBRACKET UMINUS EQUALS LT GT LTE GTE COMMA ENDLINE NEQ
%token OPENSQBRACKET CLOSESQBRACKET SEMICOLON

// From here on is syntax hilighted in blue
%token IF ELSEIF ELSE ENDIF AND OR NOT WHILE THEN INTEGERTYPE FLOATTYPE STRINGTYPE SHARED GLOBAL NETGLOBAL IMPORTANTNETGLOBAL DATA READ RESTORE
%token WEND REPEAT UNTIL FOREVER FOR TO STEP NEXT GOTO DIM PROCEDURE ENDPROC TNTB_TRUE TNTB_FALSE END BREAK

// From here on is syntax hilighted in granite
%token PRINT GRAPHICSMODE GRAPHICSMODEHARDWARE TEXTMODE BUTTON WINDOWMODE WINDOWMODEHARDWARE
%token <tokenId> STATEMENTNAME ARITHFUNCTIONNAME STRFUNCTIONNAME
%token SPRITE SYNCSCREEN VBLSYNCON VBLSYNCOFF FRAMERATE FRAMERATEMAX DRAWPICTRES PASTESPRITE PASTEIMAGE
%token ISPRESSED PROCRESULT PROCRESULTSTR BEEP FADEUP FADEDOWN FADETO HIDEMOUSE SHOWMOUSE PROCRESULTFLOAT
%token GETMOUSEX GETMOUSEY LOADSOUNDS PLAYSOUND LOOPSOUND SOUNDRESULT STOPSOUND STOPCHANNEL SOUNDPLAYING CHANNELPLAYING UNLOADSOUNDS
%token MOVESOUND MOVECHANNEL SOUNDVOLUME SETSOUNDVOLUME SPRITEOFF

%token INITNETWORKING HOSTNETGAME JOINNETGAME GETNETPLAYERNAME COUNTNETPLAYERS GETNETID LEAVENETGAME
%token MORENETEVENTS GETNETEVENTTYPE GETNETEVENTCONTENT NETGAMETERMINATED NETPLAYERLEFT NETPLAYERDISCONNECTED NETCONNECTIONLOST
%token SETNETDATA UPDATENETDATA SETNETWORKINGNORMAL SETNETWORKINGREGISTERED

%token COUNTRESOURCES GETRESOURCENAME RESOURCEEXISTS
%token LOADIMAGES UNLOADIMAGES SETSPRITEBANK SETDEFAULTSPRITEBANK SETSPRITEPRIORITY
%token LOADINPUTBANK POLLINPUT EDITINPUT SUSPENDINPUT RESUMEINPUT
%token LOADMUSIC PLAYMUSIC STOPMUSIC GETMUSICLENGTH GETMUSICPOSITION SETMUSICPOSITION MUSICVOLUME SETMUSICVOLUME MUSICLOOPON
%token MUSICLOOPOFF ISMUSICPLAYING
%token DRAWTEXT TEXTFONT TEXTSIZE TEXTFACE NUMTOSTRING STRINGTONUM TEXTWIDTH TEXTHEIGHT TEXTDESCENT
%token OPENCANVAS CLOSECANVAS TARGETCANVAS PAINTCANVAS COPYCANVAS CANVASFX
%token SPRITECOL SPRITECOLRECT POINTINRECT WAITMOUSEDOWN WAITMOUSEUP WAITMOUSECLICK UP DOWN LEFT RIGHT SPACE
%token SETSPRITETRANSPARENCY GETSPRITETRANSPARENCY SPRITECOLOUROFF SPRITECOLOUR
%token LOADTNTMAP UNLOADTNTMAP SETCURRENTTNTMAP
%token GETTNTMAPWIDTH GETTNTMAPHEIGHT GETTNTMAPLAYERS
%token COUNTOBJECTS GETNTHOBJECTNAME GETNTHOBJECTTYPE GETNTHOBJECTX GETNTHOBJECTY GETNTHOBJECTZ
%token COUNTPOLYGONS INPOLYGON POLYNAME POLYBOUNDSX POLYBOUNDSY POLYBOUNDSWIDTH POLYBOUNDSHEIGHT GETTILE DRAWMAPSECTION SETMAPTILE GETMAPTILEWIDTH GETMAPTILEHEIGHT
%token MAPLINECOL
%token LINE FILLRECT FRAMERECT FILLOVAL FRAMEOVAL FILLPOLY FRAMEPOLY
%token SETPIXELCOLOUR GETPIXEL SETPENCOLOUR GETPENCOLOUR SETPENTRANSPARENCY GETPENTRANSPARENCY
%token GETREDCOMPONENT GETGREENCOMPONENT GETBLUECOMPONENT SETREDCOMPONENT SETGREENCOMPONENT SETBLUECOMPONENT
%token ABS MOD SIN COS TAN INVSIN INVCOS INVTAN RADSIN RADCOS RADTAN INVRADSIN INVRADCOS INVRADTAN CALCULATEANGLE ANGLEDIFFERENCE DEGTORAD RADTODEG SQUAREROOT MIN MAX WRAP
%token BITAND BITOR BITXOR BITTEST SETBIT BITSHIFT
%token CEIL ROUND FLOOR RANDOM SETRANDOMSEED GETTIMER DELAY
%token INVALRECT NEWBUTTON CLEARBUTTONS WAITBUTTONCLICK POLLBUTTONCLICK GETBUTTONCLICK
%token CREATEVIEWPORT CREATEMAPVIEWPORT REMOVEVIEWPORT VIEWPORTOFFSET GETVIEWPORTXOFFSET GETVIEWPORTYOFFSET GETVIEWPORTWIDTH GETVIEWPORTHEIGHT
%token DRAWTRACKINGON DRAWTRACKINGOFF MAKECOLOUR NTHRESOURCEID
%token LOWERCASE UPPERCASE STRINGLENGTH REMOVECHAR GETCHAR LEFTSTR RIGHTSTR MIDSTR
%token COUNTIMAGES IMAGEWIDTH IMAGEHEIGHT IMAGEXOFF IMAGEYOFF SETIMAGEOFFSETS
%token WHITE BLACK RED GREEN BLUE YELLOW COPYIMAGE DELETEIMAGE IMAGECOLLISIONTOLERANCE IMAGECOLLISIONTYPE SETIMAGECOLLISION
%token CREATEFILE FILEOPEN ASKCREATEFILE ASKUSERFILEOPEN FILECLOSE FILEWRITE FILEREADSTRING FILEREADFLOAT FILEREADINT NEWFOLDER DELETEFILE
%token FILEPOS SETFILEPOS FILELENGTH FILEEXISTS FILETYPE
%token SETDIRECTORYSYSTEMPREFERENCES SETDIRECTORYUSERPREFERENCES SETDIRECTORYTEMPORARY SETDIRECTORYGAME
%token SPRITEX SPRITEY SPRITEI MOVESPRITE SPRITEBANK SPRITEMOVING SPRITEANIMATING ANIMSPRITE
%token SETMOANPROG ADDSPRITETOCHAN ADDVIEWPORTTOCHAN CHANMOVE CHANANIM REMOVESPRITEFROMCHAN REMOVEVIEWPORTFROMCHAN
%token AUTOMOANON AUTOMOANOFF AUTOMOANING STEPMOAN MOVEVIEWPORT VIEWPORTMOVING PAUSECHANNEL UNPAUSECHANNEL CHANPAUSED
%token RESETCHAN CHANREG SETCHANREG RESTARTCHAN CHANPLAY
%token SETSPRITEANGLE SPRITEANGLE SETSPRITEXSCALE SPRITEXSCALE SETSPRITEYSCALE SPRITEYSCALE SCALESPRITE SPRITESCALING
%token SETSPRITEXFLIP SETSPRITEYFLIP SPRITEXFLIP SPRITEYFLIP ROTATESPRITE SPRITEROTATING SPRITEACTIVE SPRITECROP SPRITECROPOFF DISABLEBREAK
%token READRAWKEY RAW2ASCII RAW2SCANCODE RAW2STR CHAR2ASCII ASCII2CHAR KEYBOARDREPEATON KEYBOARDREPEATOFF GETKEYBOARDREPEAT CLEARKEYBOARD WAITKEYPRESS
%token RAWMODSHIFT RAWMODCOMMAND RAWMODOPTION RAWMODCAPS RAWMODCONTROL
%token RAWFILTNUMBER RAWFILTPRINT RAWFILTNAV RAWFILTDEL
%token STREDITORCONTENTS SETSTREDITORCONTENTS STREDITORCURSOR SETSTREDITORCURSOR STREDITORINPUT

// The type of various elements in the parse tree
%type <program>				program
%type <statement>			statementBlock statementCall
%type <statement>			statement functionCall assignmentStatement ifStatement elseIfs
%type <statement>			else whileStatement ifThenStatement
%type <statement>			repeatStatement forStatement gotoStatement readStatement restoreStatement
%type <statement>			readVarsList dataStatement dimStatement
%type <statement>			procedureDef procedureCall endProc
%type <statement>			globalStatement netGlobalStatement importantNetGlobalStatement sharedStatement varDeclaration fxMatrixStatement setNetDataStatement
%type <string>				procedureCallHead

%type <conditionalBlock>	ifHead elseIfHead whileHead ifThenHead
%type <forStatement>		forHead

%type <variableNode>		extVariableList initedVariableList typedInitedVariableList typedPrimVariableList initedVariable typedExtVariableList paramList typedPrimVariable
%type <array>				arrayVariable
%type <variable>			primVariable initedVariableLValue
%type <intStorage>			nextStatement intStorage
%type <floatStorage>		floatStorage
%type <strStorage>			strStorage
%type <storage>				storage

%type <arithExp>			numExpression boolExpression stepStatement intFunctionCall
%type <strExp>				strExpression strFunctionCall

%type <genExp>				genExp genExpList

%type <arrayEntry>			arrayDimList arrayDimEntry
%type <procedureDef>		procName

/* Allocate the priorities and the associativity of the operators. Priority
is indicated by the order of declaration - lowest first. Non-associative
operators can be declared with %nonassoc. Note that we declare a priority 
for UMINUS, even though this symbol is never returned by the lexer. This is
used in a rule below to over-ride the standard priority for MINUS when it is
used as a unary minus operator. */

%left OR
%left AND

%nonassoc EQUALS NEQ
%nonassoc LT LTE GT GTE

%left PLUS MINUS
%left TIMES DIVIDE

%right NOT UMINUS

%left POWER

%start program

%%

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥¥¥¥ TNT Basic Core ¥¥¥
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
program			: statementBlock 							{ $$=CProgram::GetParsingProgram(); CProgram::GetParsingProgram()->SetCode($1); CProgram::GetParsingProgram()->SetDC(CProgram::GetParsingProgram()->GetDataStatementScope()->DataStatements()); }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Variables and storage classes
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The primitive types of variable
// Upon reduction it sets the parser state so that future new variables are entered with the correct type
// eg. int or str etc
variableType:
		INTEGERTYPE		{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kInteger); } |
		STRINGTYPE		{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kString); }  |
		FLOATTYPE		{ /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kFloat); }

// A variable of primitive type
primVariable:
		INTIDENTIFIER |
		STRIDENTIFIER |
		FLOATIDENTIFIER |
		UNDECLAREDPRIMITIVE
		
// An array of any type		
arrayVariable:
		INTARRAYIDENTIFIER |
		STRARRAYIDENTIFIER |
		FLOATARRAYIDENTIFIER |
		UNDECLAREDARRAY

// Storage is either a variable or a dereferenced array, ie a primitive storage element.. It can be nil.
intStorage:
		INTIDENTIFIER
		{
			$$=static_cast<CIntegerSymbol*>($1);
		} |
		INTARRAYIDENTIFIER error CLOSESQBRACKET
		{
			$$=0L;			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
		} |
		INTARRAYIDENTIFIER genExpList CLOSESQBRACKET
		{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$2,@3))
			{
				$$=new CIntegerArrayItem(static_cast<CIntegerArraySymbol*>($1),$2);
				UTBException::ThrowIfNoParserMemory($$);
			}
			else
				$$=0L;
		}				
strStorage:
		STRIDENTIFIER
		{
			$$=static_cast<CStrSymbol*>($1);
		} |
		STRARRAYIDENTIFIER genExpList CLOSESQBRACKET
		{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$2,@3))
			{
				$$=new CStrArrayItem(static_cast<CStrArraySymbol*>($1),$2);
				UTBException::ThrowIfNoParserMemory($$);
			}
			else
				$$=0L;
		} |
		STRARRAYIDENTIFIER error CLOSESQBRACKET
		{
			$$=0L;			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
		}	
floatStorage:
		FLOATIDENTIFIER
		{
			$$=static_cast<CFloatSymbol*>($1);
		} |
		FLOATARRAYIDENTIFIER genExpList CLOSESQBRACKET
		{
			if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$2,@3))
			{
				$$=new CFloatArrayItem(static_cast<CFloatArraySymbol*>($1),$2);
				UTBException::ThrowIfNoParserMemory($$);
			}
			else
				$$=0L;
		} |
		FLOATARRAYIDENTIFIER error CLOSESQBRACKET
		{
			$$=0L;			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
		}
		
/* Generic storage type for any type of data, it can be nil if an error occurred in an array storage member */
storage:
		intStorage			{ if (!$1) $$=0L; else { $$=new CPrimitiveStorage($1); UTBException::ThrowIfNoParserMemory($$); } } |
		floatStorage		{ if (!$1) $$=0L; else { $$=new CPrimitiveStorage($1); UTBException::ThrowIfNoParserMemory($$); } } |
		strStorage			{ if (!$1) $$=0L; else { $$=new CPrimitiveStorage($1); UTBException::ThrowIfNoParserMemory($$); } } |
		UNDECLAREDARRAY genExpList CLOSESQBRACKET { delete $2; $$=0; } |
		UNDECLAREDARRAY error CLOSESQBRACKET { $$=0; } |
		UNDECLAREDPRIMITIVE { $$=0L; }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Type lists
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Generates a symbol list which can include array type variables (but not references into arrays, simply "array[]")
// eg. a,b,d[],e[],f etc
extVariableList:
		primVariable										{ $$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$); } |
		arrayVariable CLOSESQBRACKET						{ $$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$); } |
		primVariable COMMA extVariableList					{ $$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$); ConsData($$,$3); } |
		arrayVariable CLOSESQBRACKET COMMA extVariableList	{ $$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$); ConsData($$,$4); }

// Reduces the left hand side of an inited variable assignment, it turns off declaration mode so that the following
// assignment can be parsed without variable references being interpreted as redeclarations.
// The primitive variable can be an undecalred variable, remember this if passing it onto other functions
initedVariableLValue:
		primVariable										{ $$=$1; CProgram::GetParsingProgram()->PauseDeclarationMode(); }

// Generates a list of extended symbols with optional initialization values. Allows simple types = expressions and array
// with or without dimension specification.
// eg. a=10,b=20,c[5] etc.
initedVariable:
		primVariable										{ $$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$); } |
//		arrayVariable										{ $$=0L; } |	// redeclared variable, already reported by lexer
		initedVariableLValue EQUALS error
		{
			$$=0;
			FlushExp();
			yyclearin;
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));			

			// resume declaration mode pause by the reduction of initedVariableLValue
			CProgram::GetParsingProgram()->ResumeDeclarationMode();
		} |
		initedVariableLValue EQUALS genExp
		{
			$$=0L;
			
			if (CGeneralExpressionErr::ContainsErrors($3))
				$$=0L;
			else if ($1 && !$1->IsUndeclared())
			{
				// Create the variable node
				$$=new CVariableNode($1);

				Try_
				{
					UTBException::ThrowIfNoParserMemory($$);
					
					$$->mInitStatement=MakeAssignmentStatement(CPrimitiveStorage($1),$3,@3);
				}
				Catch_(err)
				{
					// log the error
					delete $$;	// don't leak this on error
					$$=0L;		// IMPORTANT!!
					CProgram::GetParsingProgram()->LogError(err);
				}
			}
			
			// resume declaration mode pause by the reduction of initedVariableLValue
			CProgram::GetParsingProgram()->ResumeDeclarationMode();
		} |
		arrayVariable error CLOSESQBRACKET
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));			
			$$=0;
		} |
		arrayVariable genExpList CLOSESQBRACKET
		{
			// allow nil lists as this is a declaration
			if (CGeneralExpression::IsNullList($2))	// list can be null (I think...)
			{
				$$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$);
				delete $2;
			}
			else if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$2,@3))
			{
				if (!$2)
				{
					$$=new CVariableNode($1); UTBException::ThrowIfNoParserMemory($$);
				}
				else
				{
					// Create an array dimensioning statement
					if ($1->GetType()==CVariableSymbol::kUndeclaredArray)
					{
						// undeclared arrays already reported in lexer
						delete $2;
						$$=0;
					}
					else
					{
						CArrayDimEntry	*de=new CArrayDimEntry($1,$2);
						UTBException::ThrowIfNoParserMemory(de);
						
						$$=new CVariableNode($1);
						UTBException::ThrowIfNoParserMemory($$);
						
						$$->mInitStatement=new CDimStatement(de);
						UTBException::ThrowIfNoParserMemory($$->mInitStatement);
					}
				}
			}
			else
				$$=0;
		}	
initedVariableList:
		initedVariable |	// <-Can be nil
		initedVariable COMMA initedVariableList							{ $$=ConsData($1,$3); }

// typed optionally initialized variable list, eg
// int a,b=10
// str f,g[3,4],e="hello"
typedInitedVariableList:	// <-Can be nil
		variableType initedVariableList 								{ $$=$2; CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); } |
		error initedVariableList
		{
			$$=0L;
			LogException_(UTBException::ThrowWithRange(@1,kTBErr_ExpectedATypeName));			
		} |
		variableType error
		{
			$$=0L;
			CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined);
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_NotAVariable));
			FlushList();
			yyclearin;
		}

// a list of type var, type var etc. Used for procedure parameters in proc def.
// eg. int a,int b,str c
typedPrimVariable:
		variableType primVariable										{ $$=new CVariableNode($2); UTBException::ThrowIfNoParserMemory($$); CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); }
typedPrimVariableList:
		typedPrimVariable 												|
		typedPrimVariableList COMMA typedPrimVariable					{ $$=ConsData($1,$3); } |
		error COMMA typedPrimVariable									{ $$=$3; LogException_(UTBException::ThrowWithRange(@1,-10)); } |
		typedPrimVariableList COMMA error								{ $$=$1; LogException_(UTBException::ThrowWithRange(@1,-11)); FlushList(); yyclearin; }

// a list of type var used for shared statements, no initializations allowed
typedExtVariableList:
		variableType extVariableList									{ $$=$2; CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined); } |
		error extVariableList											{ $$=0L; LogException_(UTBException::ThrowWithRange(@1,kTBErr_ExpectedATypeName)); }  |
		variableType error
		{
			CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined);
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_NotAVariable));
			FlushList();
			yyclearin;
			$$=0;
		}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Procedures
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A procedure call refers to an identifier which could be an variable, a procedure or undeclared id. It catches incorrect
// use of procedures and other errors
procedureCallHead:
		primVariable OPENBRACKET
		{
			if ($1->GetType()!=CVariableSymbol::kUndeclaredPrimitive)	// if it's undefined then the error will have already been reported
				LogException_(UTBException::ThrowProcNotDefined(@1,$1->GetString()));
				
			$$=0;
		} |
		PROCIDENTIFIER OPENBRACKET										{ $$=$1; } |
		error OPENBRACKET												{ $$=0; LogException_(UTBException::ThrowWithRange(@1,kTBErr_MangledProcedureCall)); }

procedureCall:
		procedureCallHead error CLOSEBRACKET ENDLINE
		{
			delete $1;
			$$=0L;
			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));									
		} |
		procedureCallHead genExpList CLOSEBRACKET ENDLINE
		{
			StStrDeleter	delboy($1);
			if (!CGeneralExpressionErr::ContainsErrors($2))
			{
				$$=new CCallProc(CProgram::GetParsingProgram(),$1,$2,@3);
				UTBException::ThrowIfNoParserMemory($$);
			}
			else
			{
				delete $2;
				$$=0;
			}
		}
					
paramList:
		/*null*/ { $$=0L; } |
		typedPrimVariableList

// proc name pushes the new parsing scope before parsing the params list
procName:
		PROCEDURE PROCIDENTIFIER OPENBRACKET
		{
			StStrDeleter		delboy($2);
		
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
			{
				LogException_(UTBException::ThrowWithRange(@2,kTBErr_CantPutProcsInProcs));
				$$=0L;
			}
			else
			{
				$$=new CProcedure($2);
				UTBException::ThrowIfNoParserMemory($$);
				
				CProgram::GetParsingProgram()->PushParsingScope(&$$->mIdentifierScope);
				CProgram::GetParsingProgram()->SetParserInProcedure($$);
			}
		} |
		PROCEDURE PROCIDENTIFIER error
		{
			StStrDeleter		delboy($2);
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_ExpectedOpenBracket));
			$$=0;		
		} |
		PROCEDURE error OPENBRACKET
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalProcedureName));
			$$=0;
		} |
		PROCEDURE error
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalProcedureName));
			$$=0;
		}
		
procedureDef:
		procName error CLOSEBRACKET statementBlock endProc ENDLINE
		{
			if ($1)
			{
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
				delete $1;
			}
			delete $4;
			delete $5;			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_SyntaxError));
			$$=0L;
		} |
		procName error
		{
			if ($1)
			{
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
				delete $1;
			}
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_SyntaxError));
			$$=0L;		
		} |
		procName paramList CLOSEBRACKET statementBlock endProc ENDLINE
		{
			if ($1)
			{
				LogException_($1->SetParameters(@2,$2));
				CProgram::GetParsingProgram()->AddProcDef($1);
				$1->SetCode(ConsData($4,$5));
				$$=0;
				CProgram::GetParsingProgram()->PopParsingScope();
				CProgram::GetParsingProgram()->SetParserInProcedure(0L);
			}
			else
			{
				delete $2;
				delete $4;
				delete $5;
				$$=0;
			}
		}

endProc:
		ENDPROC
		{
			$$=new CEndProcStatement(0,0);
			UTBException::ThrowIfNoParserMemory($$);
		} |
		ENDPROC OPENBRACKET genExp CLOSEBRACKET
		{
			if (!$3 || $3->IsNullExp() || CGeneralExpressionErr::ContainsErrors($3))
				$$=new CEndProcStatement(0,0);
			else
			{
				if ($3->GetArithExp())
					$$=new CEndProcStatement($3->ReleaseArithExp(),0);
				else
					$$=new CEndProcStatement(0,$3->ReleaseStrExp());
			}
			delete $3;
			UTBException::ThrowIfNoParserMemory($$);
		} |
		ENDPROC OPENBRACKET error CLOSEBRACKET
		{
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));

			$$=new CEndProcStatement(0,0);
			UTBException::ThrowIfNoParserMemory($$);
		}
		

		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Functions/Statements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// General functions and statements provided by TNT Basic. A statement is a function with no return type.
statementCall:
		STATEMENTNAME genExpList ENDLINE
		{
			if (CGeneralExpressionErr::ContainsErrors($2))
			{
				$$=0;
				delete $2;
			}
			else		
				$$=UFunctionMgr::MakeStatement($1,$2,@3);
		} |
		STATEMENTNAME error ENDLINE
		{
			$$=0L;
			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));			
		}

intFunctionCall:
		ARITHFUNCTIONNAME
		{
			STextRange	range(@1);
			range.startOffset=range.endOffset;
			range.endOffset++;
			$$=UFunctionMgr::MakeArithFunction($1,0L,range,range,@1,false);
		} |
		ARITHFUNCTIONNAME error
		{
			$$=0L; FlushExp(); yyclearin;
			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedOpenBracket));
		} |
		ARITHFUNCTIONNAME OPENBRACKET genExpList CLOSEBRACKET
		{
			if (CGeneralExpressionErr::ContainsErrors($3))
			{
				$$=0;
				delete $3;
			}
			else
			{		
				$$=UFunctionMgr::MakeArithFunction($1,$3,@2,@4,STextRange(@1,@4),true);
			}
		} |
		ARITHFUNCTIONNAME OPENBRACKET error CLOSEBRACKET
		{
			$$=0L;
			
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));			
		}

strFunctionCall:
		STRFUNCTIONNAME
		{
			STextRange	range(@1);
			range.startOffset=range.endOffset;
			range.endOffset++;
			$$=UFunctionMgr::MakeStrFunction($1,0L,range,range,@1,false);
		} |
		STRFUNCTIONNAME error
		{
			$$=0L; FlushExp(); yyclearin;
			
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedOpenBracket));
		} /*|
		STRFUNCTIONNAME OPENBRACKET genExpList error ENDLINE
		{
			$$=0L; FlushExp(); yyclearin;
			
			LogException_(UTBException::ThrowWithRange(@4,kTBErr_ExpectedOpenBracket));
		}*/ |
		STRFUNCTIONNAME OPENBRACKET genExpList CLOSEBRACKET
		{
			if (CGeneralExpressionErr::ContainsErrors($3))
			{
				$$=0;
				delete $3;
			}
			else
			{		
				$$=UFunctionMgr::MakeStrFunction($1,$3,@2,@4,STextRange(@1,@4),true);
			}
		} |
		STRFUNCTIONNAME OPENBRACKET error CLOSEBRACKET
		{
			$$=0L;
			
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));
		}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥¥¥ Expressions ¥¥¥
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Arithmetic expressions
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// a numerical expression - any expression which can be evaluated as a number (float or integer).
// Boolean expressions can also be seen as an integer which is not==0
// TNT Basic treats integers, floats and booleans all under the numerical expression category.
// numExpression can return 0L if a parse error has occurred.
// You should use a genExp over a numExpression and then check it's type for errors.
numExpression :		
		OPENBRACKET numExpression CLOSEBRACKET		{ $$=$2; } |
		INTEGER										{ StStrDeleter delboy($1); TBArithFunction_(CInteger(@1,$1)); } |
		FLOAT										{ StStrDeleter delboy($1); TBArithFunction_(CFloat(@1,$1)); } |
		intFunctionCall								{ $$=$1; } |
		PROCRESULT									{ TBArithFunction_(CArithProcResult(@1,false)); } |
		PROCRESULTFLOAT								{ TBArithFunction_(CArithProcResult(@1,true)); } |
		intStorage									{ TBMaybeArithExp1_($1,CIntegerIdentifier(@1,$1)); } |
		floatStorage								{ TBMaybeArithExp1_($1,CFloatIdentifier(@1,$1)); } |
		MINUS numExpression %prec UMINUS			{ TBMaybeArithExp1_($2,CUnaryMinusOp(STextRange(@1,@2),$2)); } |
		NOT numExpression							{ TBMaybeArithExp1_($2,CNotOp(STextRange(@1,@2),$2));} |
		numExpression PLUS numExpression 			{ TBMaybeArithExp2_($1,$3,CBinaryArithOp($1,$3,PLUS)); } |
		numExpression MINUS numExpression			{ TBMaybeArithExp2_($1,$3,CBinaryArithOp($1,$3,MINUS)); } |
		numExpression TIMES numExpression 			{ TBMaybeArithExp2_($1,$3,CBinaryArithOp($1,$3,TIMES)); } |
		numExpression DIVIDE numExpression 			{ TBMaybeArithExp2_($1,$3,CBinaryArithOp($1,$3,DIVIDE)); } |
		numExpression POWER numExpression			{ TBMaybeArithExp2_($1,$3,CBinaryArithOp($1,$3,POWER)); } |
		numExpression AND numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,AND)); } |
		numExpression OR numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,OR)); } |
		boolExpression

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Boolean expressions
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// These are extracted seperately as >= == NEQ <= > < <> can't distribute in the same way as operators like +,-,*,/,&,| as
// there is no order of precedence.
// It can return 0L if a parse error has occurred.
boolExpression :
		numExpression EQUALS numExpression			{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,EQUALS)); } |
		numExpression NEQ numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,NEQ)); } |
		numExpression LT numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,LT)); } |
		numExpression LTE numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,LTE)); } |
		numExpression GT numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,GT)); } |
		numExpression GTE numExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,GTE)); } |
		strExpression EQUALS strExpression			{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,EQUALS)); } |
		strExpression NEQ strExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,NEQ)); } |
		strExpression LT strExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,LT)); } |
		strExpression LTE strExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,LTE)); } |
		strExpression GT strExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,GT)); } |
		strExpression GTE strExpression				{ TBMaybeArithExp2_($1,$3,CBinaryLogicOp($1,$3,GTE)); } |
		TNTB_TRUE									{ TBArithFunction_(CInteger(@1,"1")); } |
		TNTB_FALSE									{ TBArithFunction_(CInteger(@1,"0")); }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ String expressions
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// a string expression - any expression which can be evaluated as a string
// It can return 0L if a parse error has occurred.
// You shouldn't use strExpression directly, instead use genExp and check it's type and for errors.
strExpression :
		PROCRESULTSTR							{ TBStrFunction_(CStrProcResult(@1)); } |
		OPENBRACKET strExpression CLOSEBRACKET	{ $$=$2; } |
		strStorage								{ TBMaybeStrExp1_($1,CStrIdentifier(@1,$1)); } |
		strExpression PLUS strExpression		{ TBMaybeStrExp2_($1,$3,CStrConcatOp($1,$3)); } |
		STRLITERAL								{ StStrDeleter delboy($1); TBStrFunction_(CStrLiteral(@1,$1)); } |
		strFunctionCall							{ $$=$1; }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ General expressions
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The genExp type should be used to receive an expression. You should have a duplicate of your rule with "error" replacing
// genExp. If the rule with the genExp is matched you should call CGeneralExpressionErr::ContainsErrors($x) to check the
// resultant expression. If it contains errors then delete it and take error action (generally propagate nil as errors have
// been reported). Exactly the same rules apply for genExpList.
genExp:
		/*null*/								{ TBGeneralExp_(CGeneralExpression()); } |
		numExpression							{ TBGenExpOrError_($1,CGeneralExpression($1)); } |
		strExpression							{ TBGenExpOrError_($1,CGeneralExpression($1)); } |
		UNDECLAREDPRIMITIVE						{ TBGeneralExp_(CGeneralExpressionErr()); }

genExpList:
		genExp									{ $$=$1; } |
		genExpList COMMA genExp					{ $$=$1; if ($1) $1->AppendExpression(CGeneralExpression::kComma,@2,$3); else $$=$3; } |
		genExpList TO genExp					{ $$=$1; if ($1) $1->AppendExpression(CGeneralExpression::kTo,@2,$3); else $$=$3; } |
		genExpList SEMICOLON genExp				{ $$=$1; if ($1) $1->AppendExpression(CGeneralExpression::kSemiColon,@2,$3); else $$=$3; } |
		error COMMA genExp						{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@1,kTBErr_IllegalExp)); } |
		error TO genExp							{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@1,kTBErr_IllegalExp)); } |
		error SEMICOLON genExp					{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@1,kTBErr_IllegalExp)); } |
		genExpList COMMA error					{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp)); FlushExp(); yyclearin;} |
		genExpList SEMICOLON error				{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp)); FlushExp(); yyclearin;} |		
		genExpList TO error						{ TBGeneralExp_(CGeneralExpressionErr()); LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp)); FlushExp(); yyclearin;}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Statements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A list of statements (can be empty)
// The actions in this cons a statmentList (0 or more statements) to an existing statement. Note the list can be nil.
statementBlock:
		/* null */ 								{ $$=0L; } |
		statementBlock statement				{ $$=ConsData($1,$2); } |
		statementBlock error ENDLINE			{ LogException_(UTBException::ThrowWithRange(@2,kTBErr_SyntaxError));}
//		error statement							{}

// A statement
statement:
		assignmentStatement | ifStatement | whileStatement | repeatStatement | forStatement | functionCall | sharedStatement |
		gotoStatement | dataStatement | readStatement | dimStatement | procedureDef | procedureCall | ifThenStatement |
		globalStatement | netGlobalStatement | importantNetGlobalStatement | restoreStatement | varDeclaration | ENDLINE { $$=0L; }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Variables and Globals
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The variable decl statement allows variables to be introduced with typing and optional initialization. The initialization
// statements are extracted and returned in order and the variable list is deleted as it is entered into the table
// upon construction.
varDeclaration:
		typedInitedVariableList ENDLINE
		{
			if ($1)	// var list can be nil if there were errors in parsing it
			{
				$$=ExtractInitStatements($1);
				delete $1;
			}
			else
				$$=0L;
		}

globalHead: GLOBAL { /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ }
globalStatement:
		globalHead typedInitedVariableList ENDLINE
		{
			if ($2)
			{
				$$=ExtractInitStatements($2);
				CProgram::GetParsingProgram()->AddGlobals($2,@1,false,false); /* store all globals in a list in the program */
			}
			else
				$$=0L;
		}

netGlobalHead : NETGLOBAL { /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ }
netGlobalStatement:
		netGlobalHead typedInitedVariableList ENDLINE
		{
			if ($2)
			{
				$$=ExtractInitStatements($2);
				CProgram::GetParsingProgram()->AddGlobals($2,@1,true,false); /* store all globals in a list in the program */
			}
			else
				$$=0L;
		}

importantNetGlobalHead : IMPORTANTNETGLOBAL { /*CProgram::GetParsingProgram()->SetReportVarNotDecl(false);*/ }
importantNetGlobalStatement:
		importantNetGlobalHead typedInitedVariableList ENDLINE
		{
			if ($2)
			{
				$$=ExtractInitStatements($2);
				CProgram::GetParsingProgram()->AddGlobals($2,@1,true,true); /* store all globals in a list in the program */
			}
			else
				$$=0L;
		}

// To share variables simply pop back to the previous variable scope (the root scope) and let the following variable
// list get declared in that. Then restore the scope and enter all the variables as guests.		
sharedHead:
		SHARED
		{
			// Must be in a procedure to use shared (error will get logged in sharedStatement)
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
				CProgram::GetParsingProgram()->PauseParsingScope();
/*			CProgram::GetParsingProgram()->SetReportVarNotDecl(false); */
		}
sharedStatement:
		sharedHead error ENDLINE
		{
			if (CProgram::GetParsingProgram()->IsParserInProcedure())
				CProgram::GetParsingProgram()->ResumeParsingScope(&CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->mIdentifierScope);

			LogException_(UTBException::ThrowWithRange(@2,kTBErr_SyntaxError));
			$$=0L;
		} |
		sharedHead typedExtVariableList ENDLINE
		{
			$$=0L;

			// Must be in a procedure and must also not have declared any variables which are not global.
			if (!CProgram::GetParsingProgram()->IsParserInProcedure())
				LogException_(UTBException::ThrowWithRange(@1,kTBErr_SharedMustBeUsedInAProcedure));
			else
			{		
				// Push the scope back and enter all variables as guests
				CIdentifierScope		*scope=&CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->mIdentifierScope;
				
				CProgram::GetParsingProgram()->ResumeParsingScope(scope);

				for (CVariableNode *node=$2; node; node=node->TailData())
				{
					// Check if a parameter has the same name, if so we can't share it
					CVariableSymbol		*symbol;
					
					symbol=scope->VarTable()->Exists(node->mSymbol->GetString());
					
					// If the symbol already exists and it's not a guest (ie a global) then it must be a parameter
					// or a previously declared variable. Neither can be shared.
					if (symbol && !scope->VarTable()->IsGuest(symbol))
					{
						if (CProgram::GetParsingProgram()->GetCurrentlyParsingProc()->IsParameter(symbol))
							LogException_(UTBException::ThrowWithRange(@2,kTBErr_CantShareParam));
						else						
							LogException_(UTBException::ThrowWithRange(@2,kTBErr_AttemptToShareDeclaredVar));

						continue;	// skip this one
					}
					
					scope->VarTable()->EnterGuest(node->mSymbol);
				}
			}
						
			delete $2;			
		}

functionCall	:	statementCall | fxMatrixStatement | setNetDataStatement

fxMatrixStatement :	CANVASFX INTARRAYIDENTIFIER CLOSESQBRACKET COMMA numExpression TO numExpression ENDLINE
							{  TBStatement_(CCanvasFXStatement(static_cast<CIntegerArraySymbol*>($2),$5,$7)); } |
					CANVASFX error ENDLINE
							{ $$=0L; LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp)); }

setNetDataStatement	: SETNETDATA INTIDENTIFIER COMMA numExpression ENDLINE
							{ TBStatement_(CSetNetDataStatement($2,$4)); }
					| SETNETDATA FLOATIDENTIFIER COMMA numExpression ENDLINE
							{ TBStatement_(CSetNetDataStatement($2,$4)); }
					| SETNETDATA STRIDENTIFIER COMMA strExpression ENDLINE
							{ TBStatement_(CSetNetDataStatement($2,$4)); }
					| SETNETDATA INTARRAYIDENTIFIER genExpList CLOSESQBRACKET COMMA numExpression ENDLINE
							{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$3,@4))
								{
									TBStatement_(CSetNetDataStatement($2,$6,$3));
								}
							}
					| SETNETDATA FLOATARRAYIDENTIFIER genExpList CLOSESQBRACKET COMMA numExpression ENDLINE
							{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$3,@4))
								{
									TBStatement_(CSetNetDataStatement($2,$6,$3));
								}
							}
					| SETNETDATA STRARRAYIDENTIFIER genExpList CLOSESQBRACKET COMMA strExpression ENDLINE
							{
								if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$3,@4))
								{
									TBStatement_(CSetNetDataStatement($2,$6,$3));
								}
							}
					| SETNETDATA error ENDLINE
							{ $$=NULL; LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp)); }
					

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Arrays
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A single array dim entry - can be nil if no error or undeclared array variable 
arrayDimEntry:
	arrayVariable genExpList CLOSESQBRACKET
	{
		// if they use an undeclared variable it will be logged by the lexer, now we must sensibly parse the remaining expression
		if ($1->GetType()==CVariableSymbol::kUndeclaredArray)
		{
			delete $2;
			$$=0;
		}
		else if (CheckArrayIndexList(*CProgram::GetParsingProgram(),$2,@3))
		{
			$$=new CArrayDimEntry($1,$2);
			UTBException::ThrowIfNoParserMemory($$);
		}
		else
			$$=0L;
	} |
	arrayVariable error CLOSESQBRACKET
	{
		$$=0L;
		LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp))		
	}
// A list of array dim entries, comma seperated. Can be nil.
arrayDimList:
	arrayDimEntry |
	arrayDimList COMMA arrayDimEntry		{ $$=ConsData($1,$3); }
	
dimHead: DIM	{ CProgram::GetParsingProgram()->SetInDimStatement(true); }
dimStatement:
	dimHead arrayDimList ENDLINE
	{
		if (!$2)
			$$=0L;
		else
		{
			$$=new CDimStatement($2);
			UTBException::ThrowIfNoParserMemory($$);
		}
		CProgram::GetParsingProgram()->SetInDimStatement(false);
	} |
	dimHead error ENDLINE
	{
		$$=0L;
		LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
	}

dataStatement:
		DATA genExpList ENDLINE
		{
			$$=0L;
			if (CGeneralExpressionErr::ContainsErrors($2))
				delete $2;
			else if ($2)
			{
				CLabelTable::GetCurrentLabelTable()->DataStatementCreated($2);
				CProgram::GetParsingProgram()->GetDataStatementScope()->AddDataStatement($2);
			}
		} |
		DATA error ENDLINE
		{
			$$=0L;
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
		}

restoreHead:
		RESTORE
		{
			CProgram::GetParsingProgram()->SetWantLabels(true);
		}	// makes the lexer return label names instead of identifiers
restoreStatement:
		restoreHead LABELIDENTIFIER ENDLINE
		{
			StStrDeleter		delboy($2);
			CProgram::GetParsingProgram()->SetWantLabels(false);
			$$=new CRestoreStatement($2,*CProgram::GetParsingProgram(),@2);
			UTBException::ThrowIfNoParserMemory($$);
		}

gotoHead:
		GOTO
		{
			CProgram::GetParsingProgram()->SetWantLabels(true);
		}	// makes the lexer return label names instead of identifiers
gotoStatement:
		gotoHead LABELIDENTIFIER ENDLINE
		{
			StStrDeleter		delboy($2);
			CProgram::GetParsingProgram()->SetWantLabels(false);
			$$=new CGotoStatement($2,*CProgram::GetParsingProgram(),@2);
			UTBException::ThrowIfNoParserMemory($$);
		}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊIf statements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
/*
	If statement construction : this is what is constructed
		CStatementBlock
			(CConditionalBlock (expr)			\
				Statements						 - This list of conditional blocks is made of the if and the else ifs
				EndIfBreakStatement) +			/
			[CStatementBlock					\
				Statements]						/	This comes from the else statement

	The end if break statement ensures that if one of the conditionals executes then none of the others inside the outer
	statement block do. It's not needed on the else statement as the else statment is the last anyway so there is are no
	following conditional groups to skip.
*/
// If statements can either be gradually reduced (requires more non-terminals) or reduced in one big effort (requires fewer
// non-terminals). When reducing in one big effort however the code which record the source line number gets confused as
// when the reduction occurs the line number is pointing at the last line of the code and so they all get the same line
// number.
// If exp statements (ELSE IF exp statements)* [ELSE statments] ENDIF
// The ifHead nonterm is necessary in order to correctly record the src line number when the nt is reduced
ifHead:
		IF genExp ENDLINE	
		{
			CArithExpression	*exp=CheckSingleNumExp($2,@2);
						
			if (exp)
				TBStatement_(CConditionalBlock(false,exp));
			else
				$$=0L;
		} |
		IF error ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedNumExp));
			$$=0;
		}
ifThenHead:
		IF genExp THEN
		{
			CArithExpression	*exp=CheckSingleNumExp($2,@2);
						
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
				$$=0L;
		} |
		IF error THEN
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedNumExp));
			$$=0;	
		}
ifThenStatement:
		ifThenHead statement
		{
			if ($1)
			{
				// we don't need endif breaks and a surrounding statement block for
				// a simple ifThenStatement
				$1->SetSubStatements($2);
				$$=$1;
			}
			else
			{
				delete $2;
				$$=0;
			}
		}
ifStatement:
		ifHead statementBlock elseIfs else ENDIF ENDLINE
		{
			if ($1)
			{
				CConditionalBlock	*&conditionalsList=$1;
				
				// set the sub statements of the conditional block
				conditionalsList->SetSubStatements($2);
				
				// append the remainder of the ifstruct
				conditionalsList->Append($3);
				
				// Now we have a list of conditional blocks, the if and each of the end ifs
				// Apply end if breaks to each so that if one executes it jumps out of the
				// if statement thus avoiding executing the other conditionals
				AppendEndIfBreaks(conditionalsList);

				if ($3)
					$3->Append($4);
				else
					conditionalsList->Append($4);
				
				// Finally construct a statement block to enclose all the if statements
				// This groups together the conditionals and gives them somewhere to
				// jump out of when one of them finishes.
				$$=new CStatementBlock(false,conditionalsList);
				UTBException::ThrowIfNoParserMemory($$);
			}
			else
			{
				$$=0L;
				delete $1;
				delete $3;
				delete $4;
			}
		}/* |
		ifHead statementBlock elseIfs else error ENDLINE
		{
			delete $1;
			delete $2;
			delete $3;
			delete $4;
			
			LogException_(UTBException::ThrowWithRange(@5,kTBErr_MissingEndIf));
		}*/

// (ELSE IF exp statements)*
elseIfHead:
		ELSEIF genExp ENDLINE
		{
			CArithExpression	*exp=CheckSingleNumExp($2,@2);
			
			if (exp)
				TBStatement_(CConditionalBlock(false,exp));
			else
				$$=0;
		} |
		ELSEIF error ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedNumExp));
			$$=0L;
		}
elseIfs:
		/* null */ 						{ $$=0L; } |
		elseIfHead statementBlock elseIfs
		{
			$$=$1;
			
			if ($1)
			{
				$1->SetSubStatements($2);
				$1->Append($3);
			}
			else
			{
				delete $2;
				delete $3;
			}
		}

// [ELSE statements]
else:
		/* null */ 						{ $$=0L; } |
		ELSE ENDLINE statementBlock		{ TBStatement_(CStatementBlock(false,$3)); }

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊWhile statement
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// WHILE exp statements WEND
whileHead:
		WHILE genExp ENDLINE
		{
			CArithExpression	*exp=CheckSingleNumExp($2,@2);
			
			if (exp)
				TBStatement_(CConditionalBlock(true,exp));
			else
			{
				$$=0;
			}
		} |
		WHILE error ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_ExpectedNumExp));
			$$=0;
		}
whileStatement:
		whileHead statementBlock WEND ENDLINE
		{
			if ($1)
			{
				$$=$1;
				$1->SetSubStatements($2);
				
				// Append the goto condition instruction to the end of the statement
				// block.
				CStatement		*gotoS=new CJumpStatement($$);
				UTBException::ThrowIfNoParserMemory(gotoS);
				
				$1->AppendSubStatement(gotoS);
			}
			else
			{
				$$=0;
				delete $2;
			}
		} /*|
		whileHead statementBlock error ENDLINE
		{
			delete $1;
			delete $2;
			LogException_(UTBException::ThrowWithRange(@3,kTBErr_MissingWend));
			$$=0;
		}*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊRepeat statements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// REPEAT statements (UNTIL exp | FOREVER)
repeatStatement:
		REPEAT ENDLINE statementBlock UNTIL genExp ENDLINE
		{
			CArithExpression	*exp=CheckSingleNumExp($5,@5);
			
			if (exp)
			{
				CStatementBlock	*block;
				
				$$=block=new CStatementBlock(true,$3);
				UTBException::ThrowIfNoParserMemory($$);
				
				// Now add the conditional jump which gives the looping action
				CStatement		*condJump=new CConditionalJump(exp,block,false);
				UTBException::ThrowIfNoParserMemory(condJump);
				
				block->AppendSubStatement(condJump);
			}
			else
			{
				$$=0;
				delete $3;
			}
		} |
		REPEAT ENDLINE statementBlock UNTIL error ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@5,kTBErr_ExpectedNumExp));
			delete $3;
			$$=0;
		}/* |
		REPEAT ENDLINE statementBlock error ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@4,kTBErr_MissingUntilForever));
			delete $3;
			$$=0;
		} */|
		REPEAT ENDLINE statementBlock FOREVER ENDLINE
		{
			CStatementBlock	*block;
			
			$$=block=new CStatementBlock(true,$3);
			UTBException::ThrowIfNoParserMemory($$);
			
			// Now add the jump which gives the looping action
			CStatement		*jump=new CJumpStatement(block);
			UTBException::ThrowIfNoParserMemory(jump);
			
			block->AppendSubStatement(jump);												
		}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊFor statements
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// FOR intStorage EQUALS numExpression TO numExpression [STEP numExpression] statements NEXT [intStorage]
forHead:
		FOR storage EQUALS error stepStatement ENDLINE
		{
			LogException_(UTBException::ThrowWithRange(@4,kTBErr_IllegalExp));
			$$=0;
			delete $2;
		} |
		FOR error EQUALS genExpList stepStatement ENDLINE
		{
			LogException_(UTBException::ThrowBadForLoopIndex(@2));
			delete $4;
			$$=0;
		} |
		FOR storage EQUALS genExpList stepStatement ENDLINE
		{
			Try_
			{
				if (!CGeneralExpressionErr::ContainsErrors($4))
				{
					UFunctionMgr::SimpleTypeCheck("ata",$4,@5);
										
					// Check storage
					CIntegerStorage		*store=$2 ? $2->mIntStore : 0;

					if (!store)
					{
						if ($2) // if store is defined then it's in error, otherwise it must have been boshed by an earlier parse error and shouldn't be reported again
							LogException_(UTBException::ThrowBadForLoopIndex(@2));
						$$=0L;
					}
					else
					{
						CArithExpression *from=$4->ReleaseArithExp();
						CArithExpression *to=$4->TailData()->ReleaseArithExp();

						TBStatement_(CForStatement(store,from,to,$5));						
					}

					delete $2;						
					delete $4;						
				}
				else
					$$=0L;
			}
			Catch_(err)
			{
				CProgram::GetParsingProgram()->LogError(err);
				delete $2;
				delete $4;
				$$=0L;
			}			
		}
forStatement:
		forHead statementBlock nextStatement ENDLINE
		{
			if ($1)
			{
				// if the next statement contains an identifier check it matches
				// if it's an array index we can't statically check it as the array could
				// be indexed by a variable which would get out of its face quickly.
				bool				mismatch=false;
																	
				if ($3)	// if next statement var is specified
				{
					CIntegerStorage		*indexVar=$1->GetIndexStorage();
					CIntegerArrayItem	*indexArray=dynamic_cast<CIntegerArrayItem*>(indexVar);
					CIntegerArrayItem	*nextArray=dynamic_cast<CIntegerArrayItem*>($3);

					// if both arrays
					if (indexArray && nextArray)
						mismatch = nextArray->GetArray()!=indexArray->GetArray();
					else
						mismatch = indexVar!=$3;

					if (mismatch)
						LogException_(UTBException::ThrowForLoopIdentifierMismatch(@3));
				}
			
				if (!mismatch)
				{
					$$=$1;
					$1->SetSubStatements(*CProgram::GetParsingProgram()->GetVarTable(),$2);
				}
				else
				{
					delete $1,
					delete $2;
					$$=0L;
				}
			}
			else
			{
				delete $2;
				$$=0l;
			}
		}

stepStatement:
		/*null*/					{ $$=0L; } |
		STEP genExp
		{
			$$=CheckSingleNumExp($2,@2);
		} |
		STEP error
		{
			LogException_(UTBException::ThrowWithRange(@2,kTBErr_IllegalExp));
			FlushExp();
			yyclearin;
		}

nextStatement:
		NEXT						{ $$=0L; } |
		NEXT storage
		{
			if ($2)
			{
				if ($2->GetType()!=CVariableSymbol::kInteger)
				{
					$$=0;
					LogException_(UTBException::ThrowBadForLoopIndex(@2));
				}
				else
					$$=$2->mIntStore;
				
				delete $2;
			}
			else
				$$=0;
		} |
		NEXT error
		{
			LogException_(UTBException::ThrowBadForLoopIndex(@2));
			FlushExp();
			yyclearin;
		}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Value manipulation
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// an assigment sets an identfier or string identfier to a numerical expression or string expression respectively
// type checking is performed on the assignment and errors thrown if a type mismatch occurs
assignmentStatement :	/*UNDECLAREDPRIMITIVE EQUALS genExp ENDLINE
						{
							delete $3;
							$$=0L;	// errornous statement, ignore it, error reported already.
						} |*/
						storage EQUALS genExp ENDLINE
						{
							if ($1)
							{
								$$=0;
								if (!CGeneralExpressionErr::ContainsErrors($3))								
									LogException_($$=MakeAssignmentStatement(*$1,$3,@3));
								delete $1;
							}
							else
							{
								delete $3;
								$$=0;
							}
						}/* |
						UNDECLAREDPRIMITIVE EQUALS error ENDLINE
						{
							$$=0L;
							LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));
						}*/ |
						storage EQUALS error ENDLINE
						{
							$$=0;
							LogException_(UTBException::ThrowWithRange(@3,kTBErr_IllegalExp));
							delete $1;
						}						

readVarsList		:	storage							{ TBStatement_(CReadStatement($1)); } |
						storage COMMA readVarsList		{ TBStatement_(CReadStatement($1)); $$->Append($3); }

// A read statement returns a statement LIST rather than a statement, ie the tail may != nil						
readStatement		:	READ readVarsList ENDLINE		{ $$=$2; }

%%

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
