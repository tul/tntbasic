%{				// %{ indicates C code %}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Basic.lex
// © Mark Tully 1999
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

// This is a lexer for the TNT Basic language

// Section 1 - Declarations and definintions section

#ifndef WritersBlock

#include	<cstdlib>
#include	<iostream>
#include	"CLabelTable.h"
#include	"CProgram.h"
#include	"UCString.h"
#include	"CVariableSymbol.h"
#include	"BisonTokenLocation.h"

#endif

#include	"basic.tab.h"

#define FLEX_SUPPORT_NAMESPACE TBParser
#define FLEX_SUPPORT_SCAN_BUFFER    yy_scan_buffer
#define FLEX_SUPPORT_CURRENT_BUFFER YY_CURRENT_BUFFER
#include	"CFlexBufferSupport.cpp"
#undef FLEX_SUPPORT_SCAN_BUFFER
#undef FLEX_SUPPORT_CURRENT_BUFFER
#undef FLEX_SUPPORT_NAMESPACE

int gLineno = 0;			// Keep track of the current line number to report lexing/parsing errors


int yylex();

#ifndef WritersBlock
extern SBisonTokenLoc yylloc;

CVariableSymbol *LookUpVariable(
	char							*inStr,
	CVariableSymbol::ESymbolType	inType,
	bool							inUndeclAsArray);
void StripLastChar(
	char	*ioStr);
SBisonTokenLoc GetCurrentTokenLoc();
void EndLineParserReset();
char *DupeAndStripEscapeChrs(
	char		*inStr);
#endif

extern "C" int basicwrap();
int basicwrap()
{
	return 1;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EndLineParserReset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resets the lexer tie in variables to allow parsing to recover better from errors
void EndLineParserReset()
{
	#ifndef WritersBlock
		CProgram::GetParsingProgram()->SetDeclarationMode(CVariableSymbol::kUndefined);	// reset var decl mode
		CProgram::GetParsingProgram()->SetWantLabels(false);
		CProgram::GetParsingProgram()->SetInDimStatement(false);
		CProgram::GetParsingProgram()->SetReportVarNotDecl(true);
		CProgram::GetParsingProgram()->ClearPausedDeclModes();
	#endif
}

#ifndef WritersBlock

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetCurrentTokenLoc
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the location of the current token match.
SBisonTokenLoc GetCurrentTokenLoc()
{
	SBisonTokenLoc	loc;
	
	loc.first_line=loc.last_line=gLineno;
	loc.first_column=TBParser::GetCurrentLexOffset();
	loc.last_column=loc.first_column+yyleng;
	
	return loc;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DupeAndStripEscapeChrs
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes a cstring, duplicates it and removes and \ that aren't followed by another \ chr
char *DupeAndStripEscapeChrs(
	char		*inStr)
{
	int			origLen=std::strlen(inStr);
	char		*outStr=new char[origLen+1];
	int			source=0,dest=0;
	
	ThrowIfMemFull_(outStr);
	
	while (inStr[source])
	{
		outStr[dest]=inStr[source];
		if (inStr[source]=='\\' && inStr[source+1]!='\\')
			dest--;		// overwrite
		
		source++;
		dest++;
	}
	
	
	outStr[dest]=0;

	return outStr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StripLastChar
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes a cstring and drops the last character and any white space preceeding it. This converts strs of the form
// "x[" , "x [", "x(" etc into "x"
void StripLastChar(
	char	*ioStr)
{
	UInt16		len=std::strlen(ioStr);
	
	while (--len)
	{
		ioStr[len]=0;	// move null terminator
		
		// test next char to see if it's white space, if not break
		if (ioStr[len-1]!=' ' && ioStr[len-1]!='\t')
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LookUpVariable									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// if the type is kUndefined then the variable is looked up only. If it's not there then an error is logged.
// if the type is defined then the variable is entered into the table with the specified type, if it already exists with
// a DIFFERENT type then an error is logged.
CVariableSymbol *LookUpVariable(
	char							*inStr,
	CVariableSymbol::ESymbolType	inType,
	bool							inUndeclaredAsArray)
{
	CVariableSymbol		*symbol=0;

	Try_
	{	
		symbol=CProgram::GetParsingProgram()->GetVarTable()->Exists(inStr);

		// Not in a declaration statement
		if (inType==CVariableSymbol::kUndefined)
		{
			if (!symbol)
			{
				// Undeclared variable - enter it with the undeclared type to stop further errors				
				symbol=CProgram::GetParsingProgram()->GetVarTable()->Enter(inStr,inUndeclaredAsArray ? CVariableSymbol::kUndeclaredArray : CVariableSymbol::kUndeclaredPrimitive);
				UTBException::ThrowUndeclaredVariable(GetCurrentTokenLoc(),inStr);
			}
		}
		else
		{
			// declaring new variables, if it already exists with the same type then that's OK
			// if redefined with a new type, throw an error, unless it wasn't declared before in which case declare it now..
			if (symbol)
			{
				if (symbol->GetType()!=inType)
				{
					// type redefined, was it undeclared before?
					if ((symbol->GetType()!=CVariableSymbol::kUndeclaredPrimitive) && (symbol->GetType()!=CVariableSymbol::kUndeclaredArray))
						UTBException::ThrowVariableRedeclared(GetCurrentTokenLoc(),symbol,inType);
//					else
//						symbol->RefineType(inType);	// stop further errors
				}
			}
			else
				symbol=CProgram::GetParsingProgram()->GetVarTable()->Enter(inStr,inType);				
		}
	}
	Catch_(err)
	{
		if ((ErrCode_(err)!=kTBErr_UndeclaredVariable) || CProgram::GetParsingProgram()->ReportVarNotDecl())
			CProgram::GetParsingProgram()->LogError(err);
	}
				
	return symbol;
}

	#define				ReturnToken_(id) \
						do \
						{ \
							yylloc.first_line=yylloc.last_line=gLineno; \
							yylloc.first_column=TBParser::GetCurrentLexOffset(); \
							yylloc.last_column=yylloc.first_column+yyleng; \
							return id; \
						} while (false)
	#define				ReturnStatement_(id) \
						do \
						{ \
							yylloc.first_line=yylloc.last_line=gLineno; \
							yylloc.first_column=TBParser::GetCurrentLexOffset(); \
							yylloc.last_column=yylloc.first_column+yyleng; \
							basiclval.tokenId=id; \
							return STATEMENTNAME; \
						} while (false)
	#define				ReturnArithFunc_(id) \
						do \
						{ \
							yylloc.first_line=yylloc.last_line=gLineno; \
							yylloc.first_column=TBParser::GetCurrentLexOffset(); \
							yylloc.last_column=yylloc.first_column+yyleng; \
							basiclval.tokenId=id; \
							return ARITHFUNCTIONNAME; \
						} while (false)
	#define				ReturnStrFunc_(id) \
						do \
						{ \
							yylloc.first_line=yylloc.last_line=gLineno; \
							yylloc.first_column=TBParser::GetCurrentLexOffset(); \
							yylloc.last_column=yylloc.first_column+yyleng; \
							basiclval.tokenId=id; \
							return STRFUNCTIONNAME; \
						} while (false)
	
#else
	#define				ReturnStrFunc_(id) return id
	#define				ReturnArithFunc_(id) return id
	#define				ReturnToken_(id) return id
	#define				ReturnStatement_(id) return id
#endif

/*
	When to use 'eoc'
	-----------------
	The 'eoc' expression is short for end-of-command. It is used to ensure that a pattern is
	only matched if it is not part of another word.
	You need this when a one command name is the same as the beginning of another. Then add
	\{eoc} to the end of the longer pattern

	For example
	sprite
	sprite col
	
	If you type:
	sprite collisionId,x,y,i
	Then that is interpretted as 'sprite col' 'lisionId' ',' 'x' ',' 'y' ',' 'i'
	This is because 'sprite' is the same as 'sprite col' beginning. To fix it, you require
	that the longer pattern, in this case 'sprite col' is followed by a \{eoc} element. This
	ensures that 'sprite col' cannot be matched if it is followed by a letter.

	This only happens with commands that have spaces in them, words that contain other
	commands are not affected, for example:
	spritely
	is never interpreted as 'sprite' 'ly' as the lexer sees 'spritely' as a match (as an
	identifier) and sees it is longer than 'sprite' 'ly' and so returns it.	
	
	Rule
	----
	If in doubt you can always just add it /{eoc}, I don't think it will do any harm. It might
	bloat the lexer tables and make tb fatter, but it shouldn't affect correctness.
	
	eoc definition
	--------------
	It's supposed to capture any char that seperates a word, in otherwords a char that isn't
	a letter or a digit. But using ^ in a trailing context (that's the \ bit) doesn't work
	for some patterns, so you have to define it manually.
	I think I got most of them, but you can add to this if you think of any more.
*/

%}

digit				[0-9]
integer				{digit}+
whitespace			[ \t]
newl				[\n]
letter				[a-zA-Z_]
identifier			{letter}({letter}|{digit})*
strchar				[^"\n]|\\\"
str					{strchar}*
strend				[\"\n]
rem					("'"|"Rem ").*
maybewhitespace		{whitespace}*
eoc					[ \t\n\'\(\)\+\-\*\/\^,;\.\:=\[\]<>]

%%

{rem}				{
						#ifdef WritersBlock
							return REM;
						#endif
					}

"\""{str}{strend}	{ 
						#ifndef WritersBlock
						basiclval.string=DupeAndStripEscapeChrs(yytext);
						#endif
						ReturnToken_(STRLITERAL);
					}			/* A string literal can contain any text at all, it is opened with a " and terminated with either a " or newline */

"="					{ ReturnToken_(EQUALS); }
"<>"				{ ReturnToken_(NEQ); }
"<="				{ ReturnToken_(LTE); }
">="				{ ReturnToken_(GTE); }
"<"					{ ReturnToken_(LT); }
">"					{ ReturnToken_(GT); }
"+"					{ ReturnToken_(PLUS); }
"-"					{ ReturnToken_(MINUS); }
"/"					{ ReturnToken_(DIVIDE); }
"*"					{ ReturnToken_(TIMES); }
"^"					{ ReturnToken_(POWER); }
"("					{ ReturnToken_(OPENBRACKET); }
")"					{ ReturnToken_(CLOSEBRACKET); }
"["					{ ReturnToken_(OPENSQBRACKET); }
"]"					{
						#ifndef WritersBlock
						CProgram::GetParsingProgram()->ResumeDeclarationMode();
						#endif
						ReturnToken_(CLOSESQBRACKET);
					}
","					{ ReturnToken_(COMMA); }
";"					{ ReturnToken_(SEMICOLON); }
":"					{ EndLineParserReset(); ReturnToken_(ENDLINE); }
"True"				{ ReturnToken_(TNTB_TRUE); }
"False"				{ ReturnToken_(TNTB_FALSE); }

"Print"				{ ReturnStatement_(PRINT); }

"Disable Break"			{ ReturnStatement_(DISABLEBREAK); }
"Timer"					{ ReturnArithFunc_(GETTIMER); }
"Delay"					{ ReturnStatement_(DELAY); }
"Resource Exists"		{ ReturnArithFunc_(RESOURCEEXISTS); }
"Resource Count"		{ ReturnArithFunc_(COUNTRESOURCES); }
"Resource Name"			{ ReturnStrFunc_(GETRESOURCENAME); }
"Nth Resource ID"		{ ReturnArithFunc_(NTHRESOURCEID); }

"Beep"					{ ReturnStatement_(BEEP); }
"Load Sounds"			{ ReturnStatement_(LOADSOUNDS); }
"Unload Sounds"			{ ReturnStatement_(UNLOADSOUNDS); }
"Play Sound"			{ ReturnStatement_(PLAYSOUND); }
"Loop Sound"			{ ReturnStatement_(LOOPSOUND); }
"Sound Result"			{ ReturnArithFunc_(SOUNDRESULT); }
"Stop Sound"			{ ReturnStatement_(STOPSOUND); }
"Stop Channel"			{ ReturnStatement_(STOPCHANNEL); }
"Sound Playing"			{ ReturnArithFunc_(SOUNDPLAYING); }
"Channel Playing"		{ ReturnArithFunc_(CHANNELPLAYING); }
"Move Sound"			{ ReturnStatement_(MOVESOUND); }
"Move Channel"			{ ReturnStatement_(MOVECHANNEL); }
"Sound Volume"			{ ReturnArithFunc_(SOUNDVOLUME); }
"Set Sound Volume"		{ ReturnStatement_(SETSOUNDVOLUME); }

"Load Music"			{ ReturnStatement_(LOADMUSIC); }
"Play Music"			{ ReturnStatement_(PLAYMUSIC); }
"Stop Music"			{ ReturnStatement_(STOPMUSIC); }
"Music Length"			{ ReturnArithFunc_(GETMUSICLENGTH); }
"Music Position"		{ ReturnArithFunc_(GETMUSICPOSITION); }
"Set Music Position"	{ ReturnStatement_(SETMUSICPOSITION); }
"Music Volume"			{ ReturnArithFunc_(MUSICVOLUME); }
"Set Music Volume"		{ ReturnStatement_(SETMUSICVOLUME); }
"Music Loop On"			{ ReturnStatement_(MUSICLOOPON); }
"Music Loop Off"		{ ReturnStatement_(MUSICLOOPOFF); }
"Music Playing"			{ ReturnArithFunc_(ISMUSICPLAYING); }

"Draw Tracking On"		{ ReturnStatement_(DRAWTRACKINGON); }
"Draw Tracking Off"		{ ReturnStatement_(DRAWTRACKINGOFF); }

"Paint Canvas"			{ ReturnStatement_(PAINTCANVAS); }
"Open Canvas"			{ ReturnStatement_(OPENCANVAS); }
"Close Canvas"			{ ReturnStatement_(CLOSECANVAS); }
"Copy Canvas"			{ ReturnStatement_(COPYCANVAS); }
"Canvas"				{ ReturnStatement_(TARGETCANVAS); }
"FX Matrix"				{ ReturnToken_(CANVASFX); }

"Text Width"			{ ReturnArithFunc_(TEXTWIDTH); }
"Text Height"			{ ReturnArithFunc_(TEXTHEIGHT); }
"Text Descent"			{ ReturnArithFunc_(TEXTDESCENT); }
"Draw Text"				{ ReturnStatement_(DRAWTEXT); }
"Text Font"				{ ReturnStatement_(TEXTFONT); }
"Text Size"				{ ReturnStatement_(TEXTSIZE); }
"Text Face"				{ ReturnStatement_(TEXTFACE); }

"Fade Up"				{ ReturnStatement_(FADEUP); }
"Fade Down"				{ ReturnStatement_(FADEDOWN); }
"Fade To"				{ ReturnStatement_(FADETO); }

"Show Mouse"			{ ReturnStatement_(SHOWMOUSE); }
"Hide Mouse"			{ ReturnStatement_(HIDEMOUSE); }
"Mouse X"				{ ReturnArithFunc_(GETMOUSEX); }
"Mouse Y"				{ ReturnArithFunc_(GETMOUSEY); }

"Initialise Networking"		{ ReturnStatement_(INITNETWORKING); }
"Initialize Networking"		{ ReturnStatement_(INITNETWORKING); }
"Host Net Game"				{ ReturnArithFunc_(HOSTNETGAME); }
"Join Net Game"				{ ReturnArithFunc_(JOINNETGAME); }
"Leave Net Game"			{ ReturnStatement_(LEAVENETGAME); }

"Set Networking Normal"		{ ReturnStatement_(SETNETWORKINGNORMAL); }
"Set Networking Registered" { ReturnStatement_(SETNETWORKINGREGISTERED); }

"Set Net Data"				{ ReturnToken_(SETNETDATA); }
"Update Net Data"			{ ReturnStatement_(UPDATENETDATA); }

"More Net Events"			{ ReturnArithFunc_(MORENETEVENTS); }
"Net Event Type"			{ ReturnArithFunc_(GETNETEVENTTYPE); }
"Net Event Content"			{ ReturnArithFunc_(GETNETEVENTCONTENT); }

"Net ID"					{ ReturnArithFunc_(GETNETID); }
"Net Player Count"			{ ReturnArithFunc_(COUNTNETPLAYERS); }
"Net Player Name"			{ ReturnStrFunc_(GETNETPLAYERNAME); }

"NetGameTerminated"			{ ReturnArithFunc_(NETGAMETERMINATED); }
"NetPlayerLeft"				{ ReturnArithFunc_(NETPLAYERLEFT); }
"NetPlayerDisconnected"		{ ReturnArithFunc_(NETPLAYERDISCONNECTED); }
"NetConnectionLost"			{ ReturnArithFunc_(NETCONNECTIONLOST); }

"Load Map"					{ ReturnStatement_(LOADTNTMAP); }
"Unload Map"				{ ReturnStatement_(UNLOADTNTMAP); }
"Set Current Map"			{ ReturnStatement_(SETCURRENTTNTMAP); }

"Map Tile"					{ ReturnArithFunc_(GETTILE); }
"Set Map Tile"				{ ReturnStatement_(SETMAPTILE); }

"Map Width"					{ ReturnArithFunc_(GETTNTMAPWIDTH); }
"Map Height"				{ ReturnArithFunc_(GETTNTMAPHEIGHT); }
"Map Layers"				{ ReturnArithFunc_(GETTNTMAPLAYERS); }
"Map Tile Width"/{eoc}		{ ReturnArithFunc_(GETMAPTILEWIDTH); }
"Map Tile Height"/{eoc}		{ ReturnArithFunc_(GETMAPTILEHEIGHT); }

"Map Object Count"			{ ReturnArithFunc_(COUNTOBJECTS); }
"Map Object Name"			{ ReturnStrFunc_(GETNTHOBJECTNAME); } 
"Map Object Type"			{ ReturnArithFunc_(GETNTHOBJECTTYPE); }
"Map Object X"				{ ReturnArithFunc_(GETNTHOBJECTX); }
"Map Object Y"				{ ReturnArithFunc_(GETNTHOBJECTY); }
"Map Object Z"				{ ReturnArithFunc_(GETNTHOBJECTZ); }

"Map Polygon Count"			{ ReturnArithFunc_(COUNTPOLYGONS); }
"In Map Polygon"			{ ReturnArithFunc_(INPOLYGON); }
"Map Polygon Name"			{ ReturnStrFunc_(POLYNAME); }
"Map Polygon X"				{ ReturnArithFunc_(POLYBOUNDSX); }
"Map Polygon Y"				{ ReturnArithFunc_(POLYBOUNDSY); }
"Map Polygon Width"			{ ReturnArithFunc_(POLYBOUNDSWIDTH); }
"Map Polygon Height"		{ ReturnArithFunc_(POLYBOUNDSHEIGHT); }

"Draw Map Section"			{ ReturnStatement_(DRAWMAPSECTION); }
"Map Line Col"				{ ReturnArithFunc_(MAPLINECOL); } 

"Line"						{ ReturnStatement_(LINE); }
"Fill Rect"					{ ReturnStatement_(FILLRECT); }
"Frame Rect"				{ ReturnStatement_(FRAMERECT); }
"Fill Oval"					{ ReturnStatement_(FILLOVAL); }
"Frame Oval"				{ ReturnStatement_(FRAMEOVAL); }
"Fill Poly"					{ ReturnStatement_(FILLPOLY); }
"Frame Poly"				{ ReturnStatement_(FRAMEPOLY); }
"Set Pixel Colour"			{ ReturnStatement_(SETPIXELCOLOUR); }
"Set Pixel Color"			{ ReturnStatement_(SETPIXELCOLOUR); }
"Pixel Colour"				{ ReturnArithFunc_(GETPIXEL); }
"Pixel Color"				{ ReturnArithFunc_(GETPIXEL); }

"Pen Colour"				{ ReturnArithFunc_(GETPENCOLOUR); }
"Pen Color"					{ ReturnArithFunc_(GETPENCOLOUR); }
"Set Pen Colour"			{ ReturnStatement_(SETPENCOLOUR); }
"Set Pen Color"				{ ReturnStatement_(SETPENCOLOUR); }

"Pen Transparency"			{ ReturnArithFunc_(GETPENTRANSPARENCY); }
"Set Pen Transparency"		{ ReturnStatement_(SETPENTRANSPARENCY); }

"Red Component"				{ ReturnArithFunc_(GETREDCOMPONENT); }
"Green Component"			{ ReturnArithFunc_(GETGREENCOMPONENT); }
"Blue Component"			{ ReturnArithFunc_(GETBLUECOMPONENT); }
"Set Red Component"			{ ReturnArithFunc_(SETREDCOMPONENT); }
"Set Green Component"		{ ReturnArithFunc_(SETGREENCOMPONENT); }
"Set Blue Component"		{ ReturnArithFunc_(SETBLUECOMPONENT); }

"Make Colour"				{ ReturnArithFunc_(MAKECOLOUR); }
"Make Color"				{ ReturnArithFunc_(MAKECOLOUR); }

"Inval Rect"				{ ReturnStatement_(INVALRECT); }

"Mouse Button"			{ ReturnArithFunc_(BUTTON); }
"Pressed"				{ ReturnArithFunc_(ISPRESSED); }

"Up"					{ ReturnArithFunc_(UP); }
"Down"					{ ReturnArithFunc_(DOWN); }
"Left"					{ ReturnArithFunc_(LEFT); }
"Right"					{ ReturnArithFunc_(RIGHT); }
"Space"					{ ReturnArithFunc_(SPACE); }

"keyboard raw key"				{ ReturnArithFunc_(READRAWKEY); }
"raw key ascii"					{ ReturnArithFunc_(RAW2ASCII); }
"raw key char"					{ ReturnStrFunc_(RAW2STR); }
"raw key scancode"				{ ReturnArithFunc_(RAW2SCANCODE); }
"raw key modifier shift"		{ ReturnArithFunc_(RAWMODSHIFT); }
"raw key modifier control"		{ ReturnArithFunc_(RAWMODCONTROL); }
"raw key modifier command"		{ ReturnArithFunc_(RAWMODCOMMAND); }
"raw key modifier option"		{ ReturnArithFunc_(RAWMODOPTION); }
"raw key modifier caps lock"	{ ReturnArithFunc_(RAWMODCAPS); }
"raw key filter number"			{ ReturnArithFunc_(RAWFILTNUMBER); }
"raw key filter printable"		{ ReturnArithFunc_(RAWFILTPRINT); }
"raw key filter navigation"		{ ReturnArithFunc_(RAWFILTNAV); }
"raw key filter delete"			{ ReturnArithFunc_(RAWFILTDEL); }
"char to ascii"					{ ReturnArithFunc_(CHAR2ASCII); }
"ascii to char"					{ ReturnStrFunc_(ASCII2CHAR); }
"keyboard repeat on"			{ ReturnStatement_(KEYBOARDREPEATON); }
"keyboard repeat off"			{ ReturnStatement_(KEYBOARDREPEATOFF); }
"keyboard repeat"				{ ReturnArithFunc_(GETKEYBOARDREPEAT); }
"clear keyboard"				{ ReturnStatement_(CLEARKEYBOARD); }
"wait key press"				{ ReturnStatement_(WAITKEYPRESS); }

"string editor contents"		{ ReturnStrFunc_(STREDITORCONTENTS); }
"set string editor contents"	{ ReturnStatement_(SETSTREDITORCONTENTS); }
"string editor cursor"			{ ReturnArithFunc_(STREDITORCURSOR); }
"set string editor cursor"		{ ReturnStatement_(SETSTREDITORCURSOR); }
"string editor input"			{ ReturnStatement_(STREDITORINPUT); }

"Wait Mouse Down"		{ ReturnStatement_(WAITMOUSEDOWN); }
"Wait Mouse Up"			{ ReturnStatement_(WAITMOUSEUP); }
"Wait Mouse Click"		{ ReturnStatement_(WAITMOUSECLICK); }

"Load Input"			{ ReturnStatement_(LOADINPUTBANK); }
"Poll Input"			{ ReturnArithFunc_(POLLINPUT); }
"Edit Input"			{ ReturnStatement_(EDITINPUT); }
"Deactivate Input"		{ ReturnStatement_(SUSPENDINPUT); }
"Activate Input"		{ ReturnStatement_(RESUMEINPUT); }

"Abs"					{ ReturnArithFunc_(ABS); }
"Mod"					{ ReturnArithFunc_(MOD); }
"Sqrt"					{ ReturnArithFunc_(SQUAREROOT); }

"Sin"					{ ReturnArithFunc_(SIN); }
"Cos"					{ ReturnArithFunc_(COS); }
"Tan"					{ ReturnArithFunc_(TAN); }
"InvSin"				{ ReturnArithFunc_(INVSIN); }
"InvCos"				{ ReturnArithFunc_(INVCOS); }
"InvTan"				{ ReturnArithFunc_(INVTAN); }

"RadSin"				{ ReturnArithFunc_(RADSIN); }
"RadCos"				{ ReturnArithFunc_(RADCOS); }
"RadTan"				{ ReturnArithFunc_(RADTAN); }
"InvRadSin"				{ ReturnArithFunc_(INVRADSIN); }
"InvRadCos"				{ ReturnArithFunc_(INVRADCOS); }
"InvRadTan"				{ ReturnArithFunc_(INVRADTAN); }

"Calculate Angle"		{ ReturnArithFunc_(CALCULATEANGLE); }
"Angle Difference"		{ ReturnArithFunc_(ANGLEDIFFERENCE); }

"Degrees"				{ ReturnArithFunc_(RADTODEG); }
"Radians"				{ ReturnArithFunc_(DEGTORAD); }

"Ceil"					{ ReturnArithFunc_(CEIL); }
"Round"					{ ReturnArithFunc_(ROUND); }
"Floor"					{ ReturnArithFunc_(FLOOR); }

"Bit And"				{ ReturnArithFunc_(BITAND); }
"Bit Or"				{ ReturnArithFunc_(BITOR); }
"Bit Xor"				{ ReturnArithFunc_(BITXOR); }
"Bit Test"				{ ReturnArithFunc_(BITTEST); }
"Bit Set"				{ ReturnArithFunc_(SETBIT); }
"Bit Shift"				{ ReturnArithFunc_(BITSHIFT); }

"Max"					{ ReturnArithFunc_(MAX); }
"Min"					{ ReturnArithFunc_(MIN); }
"Wrap"					{ ReturnArithFunc_(WRAP); }

"Random"				{ ReturnArithFunc_(RANDOM); }
"Set Random Seed"		{ ReturnStatement_(SETRANDOMSEED); }

"Graphics Mode"					{ ReturnStatement_(GRAPHICSMODE); }
"Graphics Mode Hardware"		{ ReturnStatement_(GRAPHICSMODEHARDWARE); }
"Window Mode"					{ ReturnStatement_(WINDOWMODE); }
"Window Mode Hardware"			{ ReturnStatement_(WINDOWMODEHARDWARE); }
"Text Mode"						{ ReturnStatement_(TEXTMODE); }
"Draw Frame"					{ ReturnStatement_(SYNCSCREEN); }
"VBL Sync On"					{ ReturnStatement_(VBLSYNCON); }
"VBL Sync Off"					{ ReturnStatement_(VBLSYNCOFF); }
"Set Framerate Max"/{eoc}		{ ReturnStatement_(FRAMERATEMAX); }
"Set Framerate"					{ ReturnStatement_(FRAMERATE); }
"Sprite Off"/{eoc}				{ ReturnStatement_(SPRITEOFF); }
"Sprite"						{ ReturnStatement_(SPRITE); }
"Paste Sprite"					{ ReturnStatement_(PASTESPRITE); }
"Set Image Bank"				{ ReturnStatement_(SETSPRITEBANK); }
"Image Bank"					{ ReturnArithFunc_(SPRITEBANK); }
"Move Sprite" 					{ ReturnStatement_(MOVESPRITE); }
"Sprite X"/{eoc}				{ ReturnArithFunc_(SPRITEX); }
"Sprite Y"/{eoc}				{ ReturnArithFunc_(SPRITEY); }
"Sprite Image"/{eoc}			{ ReturnArithFunc_(SPRITEI); }
"Sprite Moving"/{eoc}			{ ReturnArithFunc_(SPRITEMOVING); }
"Animate Sprite"				{ ReturnStatement_(ANIMSPRITE); }
"Sprite Animating"/{eoc}		{ ReturnArithFunc_(SPRITEANIMATING); }
"Sprite Exists"/{eoc}			{ ReturnArithFunc_(SPRITEACTIVE); 
/*
commented out as don't want to separate the features of software mode and hardware mode needlessly
software mode can do these features through layer clipping, ideally hardware mode should do it the same way but i don't have time
to redo it all right now so we'll just leave this feature out
"Sprite Crop"					{ ReturnStatement_(SPRITECROP); }
"Sprite Crop Off"				{ ReturnStatement_(SPRITECROPOFF);*/ }

"Set Channel Program"			{ ReturnStatement_(SETMOANPROG); }
"Add Sprite To Channel"			{ ReturnStatement_(ADDSPRITETOCHAN); }
"Add Viewport To Channel"		{ ReturnStatement_(ADDVIEWPORTTOCHAN); }
"Remove Sprite From Channel"	{ ReturnStatement_(REMOVESPRITEFROMCHAN); }
"Remove Viewport From Channel"	{ ReturnStatement_(REMOVEVIEWPORTFROMCHAN); }
"Channel Moving"				{ ReturnArithFunc_(CHANMOVE); }
"Channel Animating"				{ ReturnArithFunc_(CHANANIM); }
"Channel Running"				{ ReturnArithFunc_(CHANPLAY); }
"Auto Moan On"					{ ReturnStatement_(AUTOMOANON); }
"Auto Moan Off"					{ ReturnStatement_(AUTOMOANOFF); }
"Auto Moaning"					{ ReturnArithFunc_(AUTOMOANING); }
"Step Moan"						{ ReturnStatement_(STEPMOAN); }
"Pause Channel"					{ ReturnStatement_(PAUSECHANNEL); }
"Resume Channel"				{ ReturnStatement_(UNPAUSECHANNEL); }
"Channel Paused"				{ ReturnArithFunc_(CHANPAUSED); }
"Reset Channel"					{ ReturnStatement_(RESETCHAN); }
"Restart Channel"				{ ReturnStatement_(RESTARTCHAN); }
"Set Channel Register"			{ ReturnStatement_(SETCHANREG); }
"Channel Register"				{ ReturnArithFunc_(CHANREG); }

"Load Images"					{ ReturnStatement_(LOADIMAGES); }
"Unload Images"					{ ReturnStatement_(UNLOADIMAGES); }
"Set Default Image Bank"		{ ReturnStatement_(SETDEFAULTSPRITEBANK); }
"Paste Image"					{ ReturnStatement_(PASTEIMAGE); }
"Copy Image"					{ ReturnStatement_(COPYIMAGE); }
"Delete Image"					{ ReturnStatement_(DELETEIMAGE); }
"Count Images"					{ ReturnArithFunc_(COUNTIMAGES); }
"Image Width"					{ ReturnArithFunc_(IMAGEWIDTH); }
"Image Height"					{ ReturnArithFunc_(IMAGEHEIGHT); }
"Image X Offset"				{ ReturnArithFunc_(IMAGEXOFF); }
"Image Y Offset"				{ ReturnArithFunc_(IMAGEYOFF); }
"Set Image Offsets"				{ ReturnStatement_(SETIMAGEOFFSETS); }
"Set Image Collision"			{ ReturnStatement_(SETIMAGECOLLISION); }
"Image Collision Tolerance"		{ ReturnArithFunc_(IMAGECOLLISIONTOLERANCE); }
"Image Collision Type"			{ ReturnArithFunc_(IMAGECOLLISIONTYPE); }

"White"				{ ReturnArithFunc_(WHITE); }
"Black"				{ ReturnArithFunc_(BLACK); }
"Red"				{ ReturnArithFunc_(RED); }
"Green"				{ ReturnArithFunc_(GREEN); }
"Blue"				{ ReturnArithFunc_(BLUE); }
"Yellow"			{ ReturnArithFunc_(YELLOW); }

"Set Sprite Rotation"			{ ReturnStatement_(SETSPRITEANGLE); }
"Sprite Rotation"/{eoc}			{ ReturnArithFunc_(SPRITEANGLE); }
"Rotate Sprite"					{ ReturnStatement_(ROTATESPRITE); }
"Sprite Rotating"				{ ReturnArithFunc_(SPRITEROTATING); }

"Set Sprite X Scale"			{ ReturnStatement_(SETSPRITEXSCALE); }
"Set Sprite Y Scale"			{ ReturnStatement_(SETSPRITEYSCALE); }
"Sprite X Scale"/{eoc}			{ ReturnArithFunc_(SPRITEXSCALE); }
"Sprite Y Scale"/{eoc}			{ ReturnArithFunc_(SPRITEYSCALE); }
"Scale Sprite"					{ ReturnStatement_(SCALESPRITE); }
"Sprite Scaling"				{ ReturnArithFunc_(SPRITESCALING); }

"Set Sprite Transparency"		{ ReturnStatement_(SETSPRITETRANSPARENCY); }
"Sprite Transparency"/{eoc}		{ ReturnArithFunc_(GETSPRITETRANSPARENCY); }

"Set Sprite X Flip"				{ ReturnStatement_(SETSPRITEXFLIP); }
"Set Sprite Y Flip"				{ ReturnStatement_(SETSPRITEYFLIP); }
"Sprite X Flip"/{eoc}			{ ReturnArithFunc_(SPRITEXFLIP); }
"Sprite Y Flip"/{eoc}			{ ReturnArithFunc_(SPRITEYFLIP); }

"Set Sprite Colour Off"/{eoc}	{ ReturnStatement_(SPRITECOLOUROFF); }
"Set Sprite Color Off"/{eoc}	{ ReturnStatement_(SPRITECOLOUROFF); }
"Set Sprite Colour"				{ ReturnStatement_(SPRITECOLOUR); }
"Set Sprite Color"				{ ReturnStatement_(SPRITECOLOUR); }

"Set Sprite Priority"			{ ReturnStatement_(SETSPRITEPRIORITY); }

"Draw Picture"					{ ReturnStatement_(DRAWPICTRES); }

"Create File"					{ ReturnArithFunc_(CREATEFILE); }
"Open File"						{ ReturnArithFunc_(FILEOPEN); }
"Ask Create File"				{ ReturnArithFunc_(ASKCREATEFILE); }
"Ask Open File"					{ ReturnArithFunc_(ASKUSERFILEOPEN); }
"Close File"					{ ReturnStatement_(FILECLOSE); }

"New Folder"					{ ReturnStatement_(NEWFOLDER); }
"Delete File"					{ ReturnStatement_(DELETEFILE); }

"File Write"					{ ReturnStatement_(FILEWRITE); }
"File Read String"				{ ReturnStrFunc_(FILEREADSTRING); }
"File Read Float"				{ ReturnArithFunc_(FILEREADFLOAT); }
"File Read Int"					{ ReturnArithFunc_(FILEREADINT); }

"File Exists"					{ ReturnArithFunc_(FILEEXISTS); }
"File Type"						{ ReturnStrFunc_(FILETYPE); }

"File Position"					{ ReturnArithFunc_(FILEPOS); }
"Set File Position"				{ ReturnStatement_(SETFILEPOS); }
"File Length"					{ ReturnArithFunc_(FILELENGTH); }

"Set Directory User Preferences"	{ ReturnStatement_(SETDIRECTORYUSERPREFERENCES); }
"Set Directory Temporary"			{ ReturnStatement_(SETDIRECTORYTEMPORARY); }
"Set Directory System Preferences"	{ ReturnStatement_(SETDIRECTORYSYSTEMPREFERENCES); }
"Set Directory Game"				{ ReturnStatement_(SETDIRECTORYGAME); }

"Data"				{ ReturnToken_(DATA); }
"Read"				{ ReturnToken_(READ); }
"Restore"			{ ReturnToken_(RESTORE); }

"Dim"				{ ReturnToken_(DIM); }

"while"				{ ReturnToken_(WHILE); }
"wend"				{ ReturnToken_(WEND); }

"for"				{ ReturnToken_(FOR); }
"to"				{ ReturnToken_(TO); }
"next"				{ ReturnToken_(NEXT); }
"step"				{ ReturnToken_(STEP); }

"repeat"			{ ReturnToken_(REPEAT); }
"until"				{ ReturnToken_(UNTIL); }
"forever"			{ ReturnToken_(FOREVER); }

"if"				{ ReturnToken_(IF); }
"then"				{ ReturnToken_(THEN); }
"else if"			{ ReturnToken_(ELSEIF); }
"else"				{ ReturnToken_(ELSE); }
"end if"			{ ReturnToken_(ENDIF); }

"break"				{ ReturnStatement_(BREAK); }

"goto"				{ ReturnToken_(GOTO); }

"As Str"			{ ReturnStrFunc_(NUMTOSTRING); }
"As String"			{ ReturnStrFunc_(NUMTOSTRING); }
"As Number"			{ ReturnArithFunc_(STRINGTONUM); }
"Lower Case"		{ ReturnStrFunc_(LOWERCASE); }
"Upper Case"		{ ReturnStrFunc_(UPPERCASE); }
"String Length"		{ ReturnArithFunc_(STRINGLENGTH); }
"Str Length"		{ ReturnArithFunc_(STRINGLENGTH); }
"Get Char"			{ ReturnStrFunc_(GETCHAR); }
"Remove Char"		{ ReturnStrFunc_(REMOVECHAR); }
"Left Str"			{ ReturnStrFunc_(LEFTSTR); }
"Right Str"			{ ReturnStrFunc_(RIGHTSTR); }
"Mid Str"			{ ReturnStrFunc_(MIDSTR); }

"Sprite Col"/{eoc}	{ ReturnArithFunc_(SPRITECOL); }
"Sprite Col Rect"/{eoc}	{ ReturnArithFunc_(SPRITECOLRECT); }
"Point In rect"/{eoc}	{ ReturnArithFunc_(POINTINRECT); }

"Procedure"			{ ReturnToken_(PROCEDURE); }
"End Proc"			{ ReturnToken_(ENDPROC); }
"Result Integer"	{ ReturnToken_(PROCRESULT); }
"Result Int"		{ ReturnToken_(PROCRESULT); }
"Result Float"		{ ReturnToken_(PROCRESULTFLOAT); }
"Result String"		{ ReturnToken_(PROCRESULTSTR); }
"Result Str"		{ ReturnToken_(PROCRESULTSTR); }
"Shared"			{ ReturnToken_(SHARED); }
"Global"			{ ReturnToken_(GLOBAL); }
"Network Global"			{ ReturnToken_(NETGLOBAL); }
"Important Network Global"	{ ReturnToken_(IMPORTANTNETGLOBAL); }

"and"				{ ReturnToken_(AND); }
"or"				{ ReturnToken_(OR); }
"not"				{ ReturnToken_(NOT); }

"End"				{ ReturnStatement_(END); }

"New Button"		{ ReturnStatement_(NEWBUTTON); }
"Clear Buttons"		{ ReturnStatement_(CLEARBUTTONS); }
"Wait Button Click"	{ ReturnStatement_(WAITBUTTONCLICK); }
"Poll Button Click"	{ ReturnArithFunc_(POLLBUTTONCLICK); }
"Button Click"		{ ReturnArithFunc_(GETBUTTONCLICK); }

"Open Map Viewport"			{ ReturnStatement_(CREATEMAPVIEWPORT); }
"Open Viewport"				{ ReturnStatement_(CREATEVIEWPORT); }
"Close Viewport"			{ ReturnStatement_(REMOVEVIEWPORT); }
"Set Viewport Offset"		{ ReturnStatement_(VIEWPORTOFFSET); }
"Viewport X Offset"			{ ReturnArithFunc_(GETVIEWPORTXOFFSET); }
"Viewport Y Offset"			{ ReturnArithFunc_(GETVIEWPORTYOFFSET); }
"Viewport Width"			{ ReturnArithFunc_(GETVIEWPORTWIDTH); }
"Viewport Height"			{ ReturnArithFunc_(GETVIEWPORTHEIGHT); }
"Move Viewport"				{ ReturnStatement_(MOVEVIEWPORT); }
"Viewport Moving"			{ ReturnArithFunc_(VIEWPORTMOVING); }

"string"			{ ReturnToken_(STRINGTYPE); }
"str"				{ ReturnToken_(STRINGTYPE); }
"float"				{ ReturnToken_(FLOATTYPE); }
"integer"			{ ReturnToken_(INTEGERTYPE); }
"int"				{ ReturnToken_(INTEGERTYPE); }

{identifier}":"		{
						#ifndef WritersBlock
						CProgram::GetParsingProgram()->GetLabelTable()->Enter(yytext);
						#endif
					}

{identifier}{maybewhitespace}"[" {
	/* An array identifier is an identifier followed by an open square bracket */
	#ifndef WritersBlock

		// Dupe the string and strip the [ and any ws
		CCString	str(yytext);
		StripLastChar(str);

		CVariableSymbol::ESymbolType	type=CProgram::GetParsingProgram()->GetDeclarationMode();
		
		// the type is only a primitive type, as we are declaring an array we lift the type into an array type
		switch (type)
		{
			case CVariableSymbol::kInteger:
				type=CVariableSymbol::kIntegerArray;
				break;
				
			case CVariableSymbol::kString:
				type=CVariableSymbol::kStringArray;
				break;
			
			case CVariableSymbol::kFloat:
				type=CVariableSymbol::kFloatArray;
				break;
				
			case CVariableSymbol::kUndefined:		// means we aren't declaring it, we're looking it up
				break;
				
			default:
				SignalPStr_("\pUnknown variable type in lexer array rule!");
				Throw_(-1);
				break;
		}

		// if we're declaring an array then we must suspend declare mode while declaring the index else we'll redeclare
		// all the variables in there. Resume it again at the closebracket.
		CProgram::GetParsingProgram()->PauseDeclarationMode();

		// This can throw an error if the variable is redefined with a different type or something like that
		CVariableSymbol	*sym=LookUpVariable(str,type,true);
		basiclval.array=static_cast<CArraySymbol*>(sym);
		
		switch (basiclval.array->GetType())
		{
			case CVariableSymbol::kIntegerArray:
				ReturnToken_(INTARRAYIDENTIFIER);
				break;
				
			case CVariableSymbol::kStringArray:
				ReturnToken_(STRARRAYIDENTIFIER);
				break;
				
			case CVariableSymbol::kFloatArray:
				ReturnToken_(FLOATARRAYIDENTIFIER);
				break;
				
			case CVariableSymbol::kUndeclaredArray:
				ReturnToken_(UNDECLAREDARRAY);
				break;
				
			default:
				// The variable we are attempting to dimension/lookup isn't an array!
				basiclval.variable=sym;

				// don't log if undeclared
				if (basiclval.variable->GetType()!=CVariableSymbol::kUndeclaredPrimitive)
					LogException_(UTBException::ThrowNotAnArray(sym,CProgram::GetParsingProgram()->IsInDimStatement(),GetCurrentTokenLoc()));
				switch (basiclval.variable->GetType())
				{
					case CVariableSymbol::kInteger:
						ReturnToken_(INTIDENTIFIER);
						break;
						
					case CVariableSymbol::kFloat:
						ReturnToken_(FLOATIDENTIFIER);
						break;
						
					case CVariableSymbol::kString:
						ReturnToken_(STRIDENTIFIER);
						break;
					
					case CVariableSymbol::kUndeclaredPrimitive:
						ReturnToken_(UNDECLAREDPRIMITIVE);
						break;
					
					default:
						SignalPStr_("\pUnknown variable class in array lex rule");
						Throw_(-1);
						break;
				}
				break;
		}
	#endif
}

{identifier} {
	/* An identifier is an identfier which isn't followed by a : or [ */
	/* this identifier could refer to either a procedure,label or an identifier, a procedure list is built as part of the */
	/* preprocessing stage so we can check against that list to see if it's a procedure or not */
	#ifndef WritersBlock
		if (CProgram::GetParsingProgram()->WantLabels())	// in label mode
		{
			basiclval.string=UCString::Duplicate(yytext);
			ReturnToken_(LABELIDENTIFIER);
		}
		else if (CProgram::GetParsingProgram()->IsProcedure(yytext))
		{
			// it's a procedure, if we're in declaration mode throw an error saying can't declare variable with procedures
			// name
			if (CProgram::GetParsingProgram()->GetDeclarationMode()!=CVariableSymbol::kUndefined)
			{
				LogException_(UTBException::ThrowCantDefineVarWithProcName(GetCurrentTokenLoc(),yytext));
				basiclval.variable=0;
				ReturnToken_(UNDECLAREDPRIMITIVE);
			}
			else
			{
				basiclval.string=UCString::Duplicate(yytext);
				ReturnToken_(PROCIDENTIFIER);
			}
		}
		else
		{		
			// This can throw an error if the variable is redefined with a different type or something like that
			basiclval.variable=LookUpVariable(yytext,CProgram::GetParsingProgram()->GetDeclarationMode(),false);
			
			switch (basiclval.variable->GetType())
			{
				case CVariableSymbol::kInteger:
					ReturnToken_(INTIDENTIFIER);
					break;
					
				case CVariableSymbol::kFloat:
					ReturnToken_(FLOATIDENTIFIER);
					break;
					
				case CVariableSymbol::kString:
					ReturnToken_(STRIDENTIFIER);
					break;

				case CVariableSymbol::kIntegerArray:
				case CVariableSymbol::kStringArray:
				case CVariableSymbol::kFloatArray:
					// Array's shouldn't be getting returned here as they should be captured by the array identifier rule above
					// If an array is returned here it means that the array is being used without it's [] after it's name which is illegal 
					LogException_(UTBException::ThrowExpectedArrayIndex(GetCurrentTokenLoc(),basiclval.variable));
					ReturnToken_(UNDECLAREDPRIMITIVE);
					break;
					
				case CVariableSymbol::kUndeclaredArray:
				case CVariableSymbol::kUndeclaredPrimitive:
					// These error's would have already been logged, the parser takes no action when an UNDECLAREDPRIMITIVE is returned
					ReturnToken_(UNDECLAREDPRIMITIVE);
					break;

									
				default:
					SignalPStr_("\pUnknown variable class in lexer!");
					Throw_(-1);
					break;
			}		
		}
	#endif
}


{integer}"."{integer} {
						#ifndef WritersBlock
						basiclval.string=UCString::Duplicate(yytext);
						#endif
						ReturnToken_(FLOAT);
					}
{integer}			{
						#ifndef WritersBlock
						basiclval.string=UCString::Duplicate(yytext);
						#endif
						ReturnToken_(INTEGER);
					}
{whitespace}		{}
{newl}				{
						EndLineParserReset();
						gLineno++;
						#ifndef WritersBlock	// Can't use ReturnStatement as it must return line no -1 as the line index
							yylloc.first_line=yylloc.last_line=gLineno-1; yylloc.first_column=TBParser::GetCurrentLexOffset(); yylloc.last_column=yylloc.first_column+yyleng;
						#endif
						return ENDLINE;
					}

.					{
						#ifndef WritersBlock
							// cerr << "Unexpected character: " << yytext << " at line " << gLineno << "\n";
							LogException_(UTBException::ThrowUnexpectedCharacter(GetCurrentTokenLoc()));
						#endif
					}
%%
