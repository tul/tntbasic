// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UFunctionMgr.cpp
// © Mark Tully 2000
// 29/6/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

/*
	A collection of routines for managing functions in TNT Basic
*/

#include	"UFunctionMgr.h"
#include	"CGeneralExpression.h"
#include	"UTBException.h"
#include	"basic.tab.h"
#include	"CProgram.h"

#include	"Collision_Statements.h"
#include	"Button_Statements.h"
#include	"IO_Statements.h"
#include	"Misc_Statements.h"
#include	"Sound_Statements.h"
#include	"Gamma_Statements.h"
#include	"Drawing_Statements.h"
#include	"Mouse_Statements.h"
#include	"Networking_Statements.h"
#include	"Graphics_Statements.h"
#include	"Sprite_Statements.h"
#include	"Input_Statements.h"
#include	"Music_Statements.h"
#include	"TNTMap_Statements.h"
#include	"Maths_Statements.h"
#include	"Viewport_Statements.h"
#include	"Image_Statements.h"
#include	"File_Statements.h"
#include	"CBreakBlockStatement.h"
#include	"MOAN_Statements.h"
#include	"Root.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SimpleTypeCheck											Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Checks the parameter list supplied matches a set template, if not it throws an error.
// example:
//	Line x1,y1 to x2,y2
//  has template "a,ata,a"
// Key
//  A	Maybe arith expression
//  S	Maybe str expression
//	a	Arithmetic expression
//	s	String expression
//	,	item seperator
//	;	item seperator
//	t	to item seperator
void UFunctionMgr::SimpleTypeCheck(
	const char			*inParamTemplate,
	CGeneralExpression	*inExp,
	const STextRange	&inListTerminatingLoc)	// location of the token which terminates the inExp list - hilited when there's not enough params
{
	STextRange		range=inListTerminatingLoc;
	bool			allDone=false;
	bool			rangeIsSep=true;

	for (CGeneralExpression *exp=inExp; exp; exp=exp->TailData())
	{
		// load up the range of the current expression for error reporting purposes
		if (exp->IsNullExp())	// if none, load up the seperator
		{
			if (exp->GetSeperator()!=CGeneralExpression::kUnspecified)
				range=exp->GetSepLoc();
			rangeIsSep=true;
		}
		else
		{
			exp->GetExpLoc(range);
			rangeIsSep=false;
		}

		switch (*inParamTemplate++)
		{
			case 'A':	// maybe arith expression
				if (exp->GetStrExp())
				{
					// doh, arithmetic expression should be here
					UTBException::ThrowExpectedNumExp(range);
				}
				break;

			case 'S':	// maybe str expression
				if (exp->GetArithExp())
				{
					// doh, arithmetic expression should be here
					UTBException::ThrowExpectedStrExp(range);
				}
				break;
						
			case 'a':	// arithmetic expression
				if (!exp->GetArithExp())
				{
					// doh, arithmetic expression should be here
					UTBException::ThrowExpectedNumExp(range);
				}
				break;
			
			case 's':	// string expression
				if (!exp->GetStrExp())
				{
					// doh, arithmetic expression should be here
					UTBException::ThrowExpectedStrExp(range);
				}
				break;
			
			case 0:		// end of template but still data remainging
				UTBException::ThrowUnexpectedParameter(range);
				break;
			
			default:
				SignalPStr_("\pUFunctionMgr::SimpleTypeCheck - Bad param template");
				Throw_(-1);
				break;
		}
		
		if (exp->GetSeperator()!=CGeneralExpression::kUnspecified)
		{
			range=exp->GetSepLoc();
			rangeIsSep=true;
		}
		else
		{
			// figure were the seperator should be
			if (rangeIsSep==false)
			{
				range.startOffset=range.endOffset;
				range.endOffset++;
				rangeIsSep=true;
			}
		}
		
		switch (*inParamTemplate++)
		{	
			case 0:		// end of template - still data remaining?
				switch (exp->GetSeperator())
				{
					case CGeneralExpression::kUnspecified:	// this is ok
						allDone=true;
						break;
					
					case CGeneralExpression::kComma:
						UTBException::ThrowUnexpectedComma(range);
						break;
						
					case CGeneralExpression::kSemiColon:
						UTBException::ThrowUnexpectedSemiColon(range);
						break;

					case CGeneralExpression::kTo:
						UTBException::ThrowUnexpectedTo(range);
						break;
					
					default:
						SignalPStr_("\pUFunctionMgr::SimpleTypeCheck - Unknown sep class");
						Throw_(-1);
						break;
				}
				break;

			case ',':	// , item seperator
				if (exp->GetSeperator()!=CGeneralExpression::kComma)	// if it's not a comma
				{														// allow unspecified seperators only if the following parameter is optional
					if (((*inParamTemplate=='A') || (*inParamTemplate=='S')) && (exp->GetSeperator()==CGeneralExpression::kUnspecified))
						;
					else
						UTBException::ThrowExpectedComma(range);
				}
				break;

			case ';':	// ; item seperator
				if (exp->GetSeperator()!=CGeneralExpression::kSemiColon)	// if it's not a semi
				{														// allow unspecified seperators only if the following parameter is optional
					if (((*inParamTemplate=='A') || (*inParamTemplate=='S')) && (exp->GetSeperator()==CGeneralExpression::kUnspecified))
						;
					else
						UTBException::ThrowExpectedSemiColon(range);
				}
				break;
				
			case 't':	// to item seperator
				if (exp->GetSeperator()!=CGeneralExpression::kTo)	// if it's not a to
				{														// allow unspecified seperators only if the following parameter is optional
					if (((*inParamTemplate=='A') || (*inParamTemplate=='S')) && (exp->GetSeperator()==CGeneralExpression::kUnspecified))
						;
					else
						UTBException::ThrowExpectedTo(range);
				}
				break;
				
			default:
				SignalPStr_("\pUFunctionMgr::SimpleTypeCheck - Bad param template");
				Throw_(-1);
				break;
		}
	}
	
	if (!allDone)		// template longer than param list
	{
		while (*inParamTemplate)
		{
			switch (*inParamTemplate)
			{
				case 'A':
				case 'S':
					break;
					
				case 'a':
					UTBException::ThrowExpectedNumExp(range);
					break;
					
				case 's':
					UTBException::ThrowExpectedStrExp(range);
					break;

				// ignore trailing seps if they preceed an exp					
				case ',':
					if ((*(inParamTemplate+1)!='A') && (*(inParamTemplate+1)!='S'))
						UTBException::ThrowExpectedComma(range);
					break;
					
				case ';':
					if ((*(inParamTemplate+1)!='A') && (*(inParamTemplate+1)!='S'))
						UTBException::ThrowExpectedSemiColon(range);
					break;
					
				case 't':
					if ((*(inParamTemplate+1)!='A') && (*(inParamTemplate+1)!='S'))
						UTBException::ThrowExpectedTo(range);
					break;
					
				default:
					SignalPStr_("\pUFunctionMgr::SimpleTypeCheck - Bad param template");
					Throw_(-1);
					break;
			}
			
			inParamTemplate++;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FunctionSimpleTypeCheck
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFunctionMgr::FunctionSimpleTypeCheck(
	const char			*inParamTemplate,
	CGeneralExpression	*inExp,
	const STextRange	&inListStart,
	const STextRange	&inListTerm,
	bool				inHadBrackets)
{
	if (inParamTemplate[0] && !inHadBrackets)
	{
		UTBException::ThrowWithRange(inListStart,kTBErr_ExpectedOpenBracket);
	}
	else
	{
		SimpleTypeCheck(inParamTemplate,inExp,inListTerm);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeStrFunction											Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CStrExpression *UFunctionMgr::MakeStrFunction(
	int					inTokenId,
	CGeneralExpression	*inExp,
	const STextRange	&inListStart,
	const STextRange	&inEndLineLoc,
	const STextRange	&inPos,
	bool				inHadBrackets)
{
	CStrExpression		*result=0L;
	
	Try_
	{
		switch (inTokenId)
		{
			case LEFTSTR:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSubStrFunction(inPos,CSubStrFunction::kLeftStr,inExp);
				break;
				
			case RIGHTSTR:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSubStrFunction(inPos,CSubStrFunction::kRightStr,inExp);
				break;
				
			case MIDSTR:
				FunctionSimpleTypeCheck("s,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSubStrFunction(inPos,CSubStrFunction::kMidStr,inExp);
				break;
				
			case NUMTOSTRING:
				FunctionSimpleTypeCheck("a,A",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNumToStrFunction(inPos,inExp);
				break;
			
			case LOWERCASE:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CLowerCaseFunction(inPos,inExp);
				break;
				
			case UPPERCASE:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CUpperCaseFunction(inPos,inExp);
				break;
				
			case REMOVECHAR:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRemoveCharFunction(inPos,inExp);
				break;
			
			case GETCHAR:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetCharFunction(inPos,inExp);
				break;
				
			case GETRESOURCENAME:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetResourceNameFunction(inPos,inExp);
				break;
				
			case GETNETPLAYERNAME:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNetPlayerNameFunction(inPos,inExp ? inExp->ReleaseArithExp() : 0L);
				break;
			
			case FILEREADSTRING:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileReadStringFunction(inExp,inPos);
				break;
				
			case FILETYPE:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileTypeFunction(inExp,inPos);
				break;
				
			case POLYNAME:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CPolyNameFunction(inExp,inPos);
				break;
				
			case GETNTHOBJECTNAME:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNthObjectNameFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case RAW2STR:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRaw2StringFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case ASCII2CHAR:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CAscii2CharFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case STREDITORCONTENTS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetStrEdContentsFunction(inPos);
				break;
			
			default:
				SignalPStr_("\pUFunctionMgr::MakeStrFunction - Unknown token id, cannot create function");
				Throw_(-1);
				break;
		}
		
		UTBException::ThrowIfNoParserMemory(result);
	}
	
	Catch_(err)	
	{
		// Log the error
		CProgram::GetParsingProgram()->LogError(err);
	}

	delete inExp;
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeArithFunction											Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CArithExpression *UFunctionMgr::MakeArithFunction(
	int					inTokenId,
	CGeneralExpression	*inExp,
	const STextRange	&inListStart,
	const STextRange	&inEndLineLoc,
	const STextRange	&inPos,
	bool				inHadBrackets)
{
	CArithExpression*	result=0L;

	Try_
	{
		switch (inTokenId)
		{
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊButtons
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETBUTTONCLICK:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetButtonClickFunction(inPos);
				break;
			
			case POLLBUTTONCLICK:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CPollButtonClickFunction(inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊCollisions
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case SPRITECOL:
				FunctionSimpleTypeCheck("a,atA",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteColFunction(inExp,inPos);
				break;
			
			case SPRITECOLRECT:
				FunctionSimpleTypeCheck("a,a,ata,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteColRectFunction(inExp,inPos);
				break;
				
			case POINTINRECT:
				FunctionSimpleTypeCheck("a,a,a,ata,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CPointInRectFunction(inExp,inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊDrawing
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETPIXEL:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetPixelFunction(inExp,inPos);
				break;
			
			case GETPENCOLOUR:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetPenColourFunction(inPos);
				break;
			
			case GETPENTRANSPARENCY:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetPenTransparencyFunction(inPos);
				break;
			
			case GETREDCOMPONENT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetRedComponentFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case SETREDCOMPONENT:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSetRedComponentFunction(inExp,inPos);
				break;
			
			case GETGREENCOMPONENT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetGreenComponentFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case SETGREENCOMPONENT:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSetGreenComponentFunction(inExp,inPos);
				break;
			
			case GETBLUECOMPONENT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetBlueComponentFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case SETBLUECOMPONENT:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSetBlueComponentFunction(inExp,inPos);
				break;
			
			case MAKECOLOUR:
				FunctionSimpleTypeCheck("a,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMakeColourFunction(inExp,inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊFile Access
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case CREATEFILE:
				FunctionSimpleTypeCheck("s,s,S",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCreateFileFunction(inExp,inPos);
				break;
				
			case FILEOPEN:
				FunctionSimpleTypeCheck("s,a,s,S",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileOpenFunction(inExp,inPos);
				break;
				
			case ASKCREATEFILE:
				FunctionSimpleTypeCheck("s,S",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CAskCreateFileFunction(inExp,inPos);
				break;
				
			case ASKUSERFILEOPEN:
				FunctionSimpleTypeCheck("a,s,S",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CAskFileOpenFunction(inExp,inPos);
				break;
			
			case FILEREADINT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileReadIntFunction(inExp,inPos);
				break;
				
			case FILEREADFLOAT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileReadFloatFunction(inExp,inPos);
				break;
				
			case FILEPOS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFilePosFunction(inExp,inPos);
				break;
				
			case FILELENGTH:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileLengthFunction(inExp,inPos);
				break;
				
			case FILEEXISTS:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFileExistsFunction(inExp,inPos);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊInput
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case STREDITORCURSOR:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetStrEdCursorFunction(inPos);
				break;
				
			case READRAWKEY:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CReadRawKeyFunction(inPos);
				break;
				
			case RAW2ASCII:
			case RAW2SCANCODE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRawKeyConvertFunction(inTokenId,inExp->ReleaseArithExp(),inPos);
				break;
				
			case RAWMODSHIFT:
			case RAWMODOPTION:
			case RAWMODCAPS:
			case RAWMODCONTROL:
			case RAWMODCOMMAND:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRawKeyCheckModsFunction(inTokenId,inExp->ReleaseArithExp(),inPos);
				break;
				
			case CHAR2ASCII:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CChar2AsciiFunction(inExp->ReleaseStrExp(),inPos);
				break;
				
			case GETKEYBOARDREPEAT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetKeyboardRepeatFunction(inPos);				
				break;
			
			case ISPRESSED:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CIsPressedFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case UP:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CUpFunction(inPos);
				break;
				
			case DOWN:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CDownFunction(inPos);
				break;
				
			case LEFT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CLeftFunction(inPos);
				break;
				
			case RIGHT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRightFunction(inPos);
				break;
				
			case SPACE:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpaceFunction(inPos);
				break;
			
			case BUTTON:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CButtonFunction(inPos);
				break;
				
			case POLLINPUT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CPollInputFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMaps
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case POLYBOUNDSX:
			case POLYBOUNDSY:
			case POLYBOUNDSWIDTH:
			case POLYBOUNDSHEIGHT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CPolyBoundsFunction(inTokenId,inExp,inPos);
				break;

			case GETTILE:
				FunctionSimpleTypeCheck("a,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetTileFunction(inExp,inPos);
				break;

			case GETMAPTILEWIDTH:				
			case GETTNTMAPWIDTH:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetTNTMapWidthFunction(inTokenId==GETMAPTILEWIDTH,inPos);
				break;
				
			case GETMAPTILEHEIGHT:
			case GETTNTMAPHEIGHT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetTNTMapHeightFunction(inTokenId==GETMAPTILEHEIGHT,inPos);
				break;
				
			case GETTNTMAPLAYERS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetTNTMapLayersFunction(inPos);
				break;
				
			case COUNTOBJECTS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCountObjectsFunction(inPos);
				break;
			
			case GETNTHOBJECTTYPE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNthObjectTypeFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case GETNTHOBJECTX:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNthObjectXFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case GETNTHOBJECTY:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNthObjectYFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case GETNTHOBJECTZ:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNthObjectZFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case COUNTPOLYGONS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCountPolygonsFunction(inPos);
				break;
				
			case INPOLYGON:
				FunctionSimpleTypeCheck("a,a,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInPolygonFunction(inExp,inPos);
				break;
				
			case MAPLINECOL:
				FunctionSimpleTypeCheck("a,ata,a,a,ata",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMapLineColFunction(inExp,inPos);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMaths
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case ABS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CAbsFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case MOD:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CModFunction(inExp,inPos);
				break;
			
			case SQUAREROOT:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSquareRootFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case SIN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSinFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case COS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCosFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case TAN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CTanFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case INVSIN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvSinFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case INVCOS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvCosFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case INVTAN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvTanFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case RADSIN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRadSinFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case RADCOS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRadCosFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case RADTAN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRadTanFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case INVRADSIN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvRadSinFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case INVRADCOS:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvRadCosFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case INVRADTAN:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CInvRadTanFunction(inExp->ReleaseArithExp(),inPos);
				break;
			
			case CALCULATEANGLE:
				FunctionSimpleTypeCheck("a,ata,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCalculateAngleFunction(inExp,inPos);
				break;
				
			case ANGLEDIFFERENCE:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CAngleDifferenceFunction(inExp,inPos);
				break;
				
			case RADTODEG:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRadToDegFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case DEGTORAD:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CDegToRadFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case CEIL:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCeilFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case ROUND:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRoundFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case FLOOR:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CFloorFunction(inExp->ReleaseArithExp(),inPos);
				break;
				
			case RANDOM:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CRandomFunction(inExp,inPos);
				break;
			
			case MAX:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMaxFunction(inExp,inPos);
				break;
			
			case MIN:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMinFunction(inExp,inPos);
				break;
			
			case WRAP:
				FunctionSimpleTypeCheck("a,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CWrapFunction(inExp,inPos);
				break;
				
			case BITAND:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CBitAndFunction(inExp,inPos);
				break;
				
			case BITOR:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CBitOrFunction(inExp,inPos);
				break;
				
			case BITXOR:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CBitXorFunction(inExp,inPos);
				break;
				
			case BITTEST:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CBitSetFunction(inExp,inPos);
				break;
				
			case SETBIT:
				FunctionSimpleTypeCheck("a,a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSetBitFunction(inExp,inPos);
				break;
				
			case BITSHIFT:
				FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CBitShiftFunction(inExp,inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMiscellaneous
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETTIMER:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetTimerFunction(inPos);
				break;
			
			case COUNTRESOURCES:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCountResourcesFunction(inExp->ReleaseStrExp(),inPos);
				break;
			
			case NTHRESOURCEID:
				FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNthResourceIDFunction(inExp,inPos);
				break;
			
			case RESOURCEEXISTS:
				Try_
				{
					FunctionSimpleTypeCheck("s,a",inExp,inListStart,inEndLineLoc,inHadBrackets);
					result=new CResourceExistsFunction(inExp,inPos);
				}
				
				Catch_(err)
				{
					FunctionSimpleTypeCheck("s,s",inExp,inListStart,inEndLineLoc,inHadBrackets);
					result=new CResourceExistsFunction(inExp,inPos);
				}
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMouse
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETMOUSEX:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetMouseXFunction(inPos);
				break;
				
			case GETMOUSEY:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetMouseYFunction(inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMusic
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETMUSICLENGTH:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetMusicLength(inPos);
				break;
			
			case GETMUSICPOSITION:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetMusicPlayPosition(inPos);
				break;
				
			case MUSICVOLUME:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMusicVolumeFunction(inPos);
				break;
				
			case ISMUSICPLAYING:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMusicPlayingFunction(inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊNetworking
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case HOSTNETGAME:
				FunctionSimpleTypeCheck("a,s,s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CHostNetworkFunction(inExp,inPos);
				break;
				
			case JOINNETGAME:
				FunctionSimpleTypeCheck("S",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CJoinNetworkFunction(inExp,inPos);
				break;
			
			case MORENETEVENTS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMoreNetEventsFunction(inPos);
				break;
			
			case GETNETEVENTTYPE:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNetEventTypeFunction(inPos);
				break;
		
			case GETNETEVENTCONTENT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNetEventContentFunction(inPos);
				break;
				
			case COUNTNETPLAYERS:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCountNetPlayersFunction(inPos);
				break;
			
			case GETNETID:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetNetIDFunction(inPos);
				break;
			
			case NETGAMETERMINATED:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNetGameTerminatedFunction(inPos);
				break;
			
			case NETPLAYERLEFT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNetPlayerLeftFunction(inPos);
				break;
				
			case NETPLAYERDISCONNECTED:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNetPlayerDisconnectedFunction(inPos);
				break;
				
			case NETCONNECTIONLOST:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CNetConnectionLostFunction(inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊSounds
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case SOUNDRESULT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSoundResultFunction(inPos);
				break;
				
			case SOUNDPLAYING:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSoundPlayingFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
			
			case CHANNELPLAYING:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CChannelPlayingFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case SOUNDVOLUME:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSoundVolumeFunction(inPos);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊSprites
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case SPRITEANGLE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteAngleFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;

			case SPRITEXSCALE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteXScaleFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case SPRITEYSCALE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteYScaleFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
			
			case SPRITEXFLIP:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteXFlipFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case SPRITEYFLIP:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteYFlipFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case SPRITEACTIVE:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CSpriteActiveFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos);
				break;
				
			case GETSPRITETRANSPARENCY:
			case SPRITEX:
			case SPRITEY:
			case SPRITEBANK:
			case SPRITEI:
			case SPRITEMOVING:
			case SPRITEANIMATING:
			case SPRITESCALING:
			case SPRITEROTATING:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetSpriteAttributeFunction(inExp ? inExp->ReleaseArithExp() : 0L,inPos,inTokenId);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMoan
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case CHANMOVE:
			case CHANANIM:
			case CHANPAUSED:
			case CHANPLAY:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CChanAttributesFunction(inExp->ReleaseArithExp(),inPos,inTokenId);
				break;
			
			case AUTOMOANING:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CMoanAttributesFunction(inPos,inTokenId);
				break;

			case CHANREG:
				Try_
				{
					FunctionSimpleTypeCheck("a,a",inExp,inListStart,inEndLineLoc,inHadBrackets);		// local channel vers: chan, reg
					result = new CGetChannelRegFunction(CGetChannelRegFunction::kLocalNum,inExp,inPos);
				}
				Catch_(err)
				{
					Try_
					{
						FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);			// global channel vers: reg
						result = new CGetChannelRegFunction(CGetChannelRegFunction::kGlobalNum,inExp,inPos);
					}
					Catch_(err)
					{
						Try_
						{
							FunctionSimpleTypeCheck("a,s",inExp,inListStart,inEndLineLoc,inHadBrackets);			// local channel vers: chan,reg
							result = new CGetChannelRegFunction(CGetChannelRegFunction::kLocalStr,inExp,inPos);					
						}
						Catch_(err)
						{
							FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);			// global channel vers: reg
							result = new CGetChannelRegFunction(CGetChannelRegFunction::kGlobalStr,inExp,inPos);											
						}
					}
				}
				break;

			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊImages
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case COUNTIMAGES:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CCountImagesFunction(inExp->ReleaseArithExp(),inPos);
				break;

			case IMAGEWIDTH:
			case IMAGEHEIGHT:
			case IMAGEXOFF:
			case IMAGEYOFF:
			case IMAGECOLLISIONTOLERANCE:
			case IMAGECOLLISIONTYPE:
				{
					FunctionSimpleTypeCheck("a,A",inExp,inListStart,inEndLineLoc,inHadBrackets);
					CArithExpression	*image=inExp->ReleaseArithExp();
					CArithExpression	*bank=inExp->TailData() ? inExp->TailData()->ReleaseArithExp() : 0;
					result=new CGetImagePropFunction(inTokenId,bank,image,inPos);
				}
				break;

			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊColours
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case WHITE:
			case BLACK:
			case RED:
			case GREEN:
			case BLUE:
			case YELLOW:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CConstColourFunction(inTokenId,inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊStrings
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case STRINGLENGTH:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CStringLengthFunction(inPos,inExp);
				break;
			
			case STRINGTONUM:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CStrToNumFunction(inExp,inPos);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊViewports
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GETVIEWPORTXOFFSET:
			case GETVIEWPORTYOFFSET:
			case GETVIEWPORTWIDTH:
			case GETVIEWPORTHEIGHT:
			case VIEWPORTMOVING:
				FunctionSimpleTypeCheck("a",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetViewportAttributeFunction(inExp->ReleaseArithExp(),inPos,inTokenId);
				break;

			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊText
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case TEXTWIDTH:
				FunctionSimpleTypeCheck("s",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetDrawTextDim(inTokenId,inExp->ReleaseStrExp(),inPos);
				break;
				
			case TEXTHEIGHT:
			case TEXTDESCENT:
				FunctionSimpleTypeCheck("",inExp,inListStart,inEndLineLoc,inHadBrackets);
				result=new CGetDrawTextDim(inTokenId,NULL,inPos);
				break;
			
			default:
				SignalPStr_("\pUFunctionMgr::MakeArithFunction - Unknown token id, cannot create function");
				Throw_(-1);
				break;
		}
		
		UTBException::ThrowIfNoParserMemory(result);
	}
	
	Catch_(err)	
	{
		// Log the error
		CProgram::GetParsingProgram()->LogError(err);
	}

	delete inExp;
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CanGoFullScreen
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// tests if a program is allowed to use full screen mode
static bool CanGoFullScreen(
	bool			inWantsToGoFullScreen)
{
	bool			should=inWantsToGoFullScreen;

	if (should && !CApplication::GetApplication()->AllowedToGoFullScreen())
	{
		ECHO("Program tried to go full screen but was prevented\n");
		should=false;
	}
	return should;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeStatement												Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The statement creation proc from TNT Basic. It creates a statement of the type specified by the token id and returns it
// to the parser.
// Important:
//		If the statement constructor throws the list of exps is deleted, so if you throw DO NOT DELETE THE INPUT LIST or you
//		will crash.
CStatement *UFunctionMgr::MakeStatement(
	int					inTokenId,
	CGeneralExpression	*inExp,
	const STextRange	&inEndLineLoc)
{
	CStatement*			result=0;

	Try_
	{
		switch (inTokenId)
		{	
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊButtons
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case NEWBUTTON:
				SimpleTypeCheck("a,a,a,a,a,a,A",inExp,inEndLineLoc);
				result=new CNewButtonStatement(inExp);
				break;
				
			case CLEARBUTTONS:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CClearButtonsStatement();
				break;
		
			case WAITBUTTONCLICK:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CWaitButtonClickHeadStatement();
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊCanvases/Graphics
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case GRAPHICSMODE:
			case WINDOWMODE:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CGraphicsModeStatement(inExp,CanGoFullScreen(inTokenId==GRAPHICSMODE));
				break;
				
			case GRAPHICSMODEHARDWARE:
			case WINDOWMODEHARDWARE:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CGraphicsModeHardwareStatement(inExp,CanGoFullScreen(inTokenId==GRAPHICSMODEHARDWARE));
				break;

			case TEXTMODE:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CTextModeStatement();
				break;
				
			case SYNCSCREEN:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSyncScreenStatement();
				break;
				
			case VBLSYNCON:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetVBLSyncStatement(true);
				break;
				
			case VBLSYNCOFF:
				result=new CSetVBLSyncStatement(false);
				break;
				
			case FRAMERATE:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetFrameRateStatement(inExp->ReleaseArithExp());
				break;
				
			case FRAMERATEMAX:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetFrameRateStatement(0);
				break;
				
			case OPENCANVAS:
				SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);
				result=new COpenCanvasStatement(inExp);
				break;
				
			case CLOSECANVAS:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CCloseCanvasStatement(inExp);
				break;

			case TARGETCANVAS:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CTargetCanvasStatement(inExp->ReleaseArithExp());
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊControl Flow
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case END:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CEndProgramStatement();
				break;
		
			case BREAK:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CBreakLoopStatement(inExp ? inExp->ReleaseArithExp() : 0);
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊDrawing
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case LINE:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CLineStatement(inExp);
				break;
				
			case FILLRECT:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CFillRectStatement(inExp);
				break;
				
			case FRAMERECT:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CFrameRectStatement(inExp);
				break;
				
			case FILLOVAL:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CFillOvalStatement(inExp);
				break;
				
			case FRAMEOVAL:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CFrameOvalStatement(inExp);
				break;
				
			case FILLPOLY:
				result=new CFillPolyStatement(inExp,inEndLineLoc);
				break;
				
			case FRAMEPOLY:
				result=new CFramePolyStatement(inExp,inEndLineLoc);
				break;
				
			case SETPIXELCOLOUR:
				SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);
				result=new CSetPixelColourStatement(inExp);
				break;
				
			case SETPENCOLOUR:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetPenColourStatement(inExp->ReleaseArithExp());
				break;
				
			case SETPENTRANSPARENCY:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetPenTransparencyStatement(inExp->ReleaseArithExp());
				break;
			
			case INVALRECT:
				SimpleTypeCheck("a,ata,a",inExp,inEndLineLoc);
				result=new CInvalRectStatement(inExp);
				break;
				
			case DRAWTEXT:
				SimpleTypeCheck("a,a,s",inExp,inEndLineLoc);
				result=new CDrawTextStatement(inExp);
				break;
				
			case TEXTSIZE:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CTextSizeStatement(inExp->ReleaseArithExp());
				break;
				
			case TEXTFACE:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CTextFaceStatement(inExp->ReleaseArithExp());
				break;
				
			case TEXTFONT:
				SimpleTypeCheck("s",inExp,inEndLineLoc);
				result=new CTextFontStatement(inExp->ReleaseStrExp());
				break;
				
			case PAINTCANVAS:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CPaintAllStatement();
				break;

			case COPYCANVAS:
				Try_
				{
					SimpleTypeCheck("ata",inExp,inEndLineLoc);
					result=new CCopyCanvasStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("a,a,a,a,ata,a,a",inExp,inEndLineLoc);
					result=new CCopyCanvasStatement(inExp);
				}
				break;
			
			case DRAWPICTRES:
				Try_
				{
					SimpleTypeCheck("a,A,A",inExp,inEndLineLoc);
					result=new CDrawPictResStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s,A,A",inExp,inEndLineLoc);
					result=new CDrawPictResStatement(inExp);
				}
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊFile Access
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case FILECLOSE:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CFileCloseStatement(inExp ? inExp->ReleaseArithExp() : 0L);
				break;
				
			case FILEWRITE:
				Try_
				{
					SimpleTypeCheck("a,a",inExp,inEndLineLoc);
					result=new CFileWriteNumberStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("a,s",inExp,inEndLineLoc);
					result=new CFileWriteStringStatement(inExp);
				}
				break;
				
			case SETFILEPOS:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetFilePosStatement(inExp);
				break;
				
			case SETDIRECTORYUSERPREFERENCES:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetDirectoryUserPreferencesStatement();
				break;
				
			case SETDIRECTORYTEMPORARY:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetDirectoryTemporaryStatement();
				break;
				
			case SETDIRECTORYSYSTEMPREFERENCES:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetDirectorySystemPreferencesStatement();
				break;
				
			case SETDIRECTORYGAME:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetDirectoryGameStatement();
				break;
				
			case NEWFOLDER:
				SimpleTypeCheck("s",inExp,inEndLineLoc);
				result=new CNewFolderStatement(inExp);
				break;
				
			case DELETEFILE:
				SimpleTypeCheck("s",inExp,inEndLineLoc);
				result=new CDeleteFileStatement(inExp);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊGamma
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case FADEUP:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				if (CApplication::GetApplication()->AllowedToDoFades())
				{
					result=new CFadeUpStatement(inExp ? inExp->ReleaseArithExp() : 0L);
				}
				else
				{
					result=new CStatement();
					ECHO("Program prevented from doing fade");
				}
				break;
				
			case FADEDOWN:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				if (CApplication::GetApplication()->AllowedToDoFades())
				{
					result=new CFadeDownStatement(inExp ? inExp->ReleaseArithExp() : 0L);
				}
				else
				{
					result=new CStatement();
					ECHO("Program prevented from doing fade");
				}
				break;
				
			case FADETO:
				SimpleTypeCheck("a,a,a,a,a",inExp,inEndLineLoc);
				if (CApplication::GetApplication()->AllowedToDoFades())
				{
					result=new CFadeToStatement(inExp);
				}
				else
				{
					result=new CStatement();
					ECHO("Program prevented from doing fade");
				}
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊInput
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case LOADINPUTBANK:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CLoadInputBankStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CLoadInputBankStatement(inExp);
				}
				break;
				
			case EDITINPUT:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CEditInputStatement();
				break;
				
			case SUSPENDINPUT:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSuspendInputStatement();
				break;
				
			case RESUMEINPUT:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CResumeInputStatement();
				break;
				
			case WAITMOUSEDOWN:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CWaitMouseDownStatement();
				break;
				
			case WAITMOUSEUP:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CWaitMouseUpStatement();
				break;
				
			case WAITMOUSECLICK:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CWaitMouseClickStatement();
				break;
				
			case WAITKEYPRESS:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CWaitKeyPressStatement();
				break;
				
			case CLEARKEYBOARD:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CClearKeyboardStatement();
				break;
				
			case KEYBOARDREPEATON:
			case KEYBOARDREPEATOFF:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetKeyboardRepeatStatement(inTokenId==KEYBOARDREPEATON);
				break;
				
			case SETSTREDITORCONTENTS:
				SimpleTypeCheck("s",inExp,inEndLineLoc);
				result=new CSetStrEdContentsStatement(inExp->ReleaseStrExp());
				break;
				
			case SETSTREDITORCURSOR:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetStrEdCursorStatement(inExp->ReleaseArithExp());
				break;
				
			case STREDITORINPUT:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CStrEdInputStatement(inExp->ReleaseArithExp());
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊIO
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case PRINT:
				result=new CPrintStatement(inExp);	// <-- Checks parameters in the constructor
				inExp=0;
				break;

			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊDraw Invaling
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case DRAWTRACKINGON:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CDrawTrackingStatement(true);
				break;
				
			case DRAWTRACKINGOFF:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CDrawTrackingStatement(false);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMaps
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case LOADTNTMAP:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CLoadTNTMapStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CLoadTNTMapStatement(inExp);
				}
				break;
			
			case UNLOADTNTMAP:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CUnloadTNTMapStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CUnloadTNTMapStatement(inExp);
				}
				break;
			
			case SETCURRENTTNTMAP:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CSetCurrentTNTMapStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CSetCurrentTNTMapStatement(inExp);
				}
				break;
				
			case DRAWMAPSECTION:
				SimpleTypeCheck("a,ata,a,ata,a",inExp,inEndLineLoc);
				result=new CDrawMapSectionStatement(inExp);
				break;
				
			case SETMAPTILE:
				SimpleTypeCheck("a,a,a,a",inExp,inEndLineLoc);
				result=new CSetMapTileStatement(inExp);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMaths
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case SETRANDOMSEED:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetRandomSeedStatement(inExp->ReleaseArithExp());
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMiscellaneous
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ				
			case DELAY:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CDelayHeadStatement(inExp->ReleaseArithExp());
				break;
				
			case DISABLEBREAK:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CDisableBreakStatement();
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMouse
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case HIDEMOUSE:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CHideMouseStatement();
				break;
				
			case SHOWMOUSE:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CShowMouseStatement();
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMusic
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case LOADMUSIC:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CLoadMusicStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CLoadMusicStatement(inExp);
				}
				break;
			
			case PLAYMUSIC:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CPlayMusicStatement();
				break;
			
			case STOPMUSIC:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CStopMusicStatement();
				break;
			
			case SETMUSICPOSITION:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetMusicPlayPosition(inExp->ReleaseArithExp());
				break;
				
			case SETMUSICVOLUME:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetMusicVolumeStatement(inExp->ReleaseArithExp());
				break;
				
			case MUSICLOOPON:
			case MUSICLOOPOFF:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetMusicLoopStatement(inTokenId==MUSICLOOPON);
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊNetworking
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case INITNETWORKING:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CInitNetworkingStatement();
				break;
			
			case LEAVENETGAME:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CLeaveNetGameStatement();
				break;
				
			case UPDATENETDATA:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CUpdateNetDataStatement();
				break;
			
			case SETNETWORKINGNORMAL:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetNetworkingNormalStatement();
				break;
				
			case SETNETWORKINGREGISTERED:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetNetworkingRegisteredStatement();
				break;
			
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊSound
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case BEEP:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CBeepStatement(inExp ? inExp->ReleaseArithExp() : 0L);
				break;
				
			case UNLOADSOUNDS:
				SimpleTypeCheck("",inExp,inEndLineLoc);	// currently don't support >1 concurrent sound bank
				result=new CUnloadSoundsStatement();
				break;
		
			case LOADSOUNDS:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CLoadSoundsStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CLoadSoundsStatement(inExp);
				}
				break;
				
			case PLAYSOUND:
				SimpleTypeCheck("a,A,A,A",inExp,inEndLineLoc);
				result=new CPlaySoundStatement(inExp);
				break;
				
			case LOOPSOUND:
				SimpleTypeCheck("a,A,A",inExp,inEndLineLoc);
				result=new CLoopSoundStatement(inExp);
				break;
				
			case STOPSOUND:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CStopSoundStatement(inExp->ReleaseArithExp());
				break;
				
			case STOPCHANNEL:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CStopChannelStatement(inExp->ReleaseArithExp());
				break;
			
			case MOVESOUND:
				SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);
				result=new CMoveSoundStatement(inExp);
				break;
				
			case MOVECHANNEL:
				SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);
				result=new CMoveChannelStatement(inExp);
				break;
				
			case SETSOUNDVOLUME:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSetSoundVolumeStatement(inExp);
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊSprites
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case SETDEFAULTSPRITEBANK:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CSetDefaultSpriteBankStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CSetDefaultSpriteBankStatement(inExp);
				}
				break;
			
			case SPRITEOFF:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CKillSpriteStatement(inExp ? inExp->ReleaseArithExp() : 0L);
				break;
				
			case SPRITE:
				SimpleTypeCheck("a,A,A,A",inExp,inEndLineLoc);
				result=new CSetSpriteStatement(inExp);
				break;
							
			case PASTESPRITE:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CPasteSpriteStatement(inExp->ReleaseArithExp());
				break;
				
			case SETSPRITETRANSPARENCY:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteTransparencyStatement(inExp);
				break;
				
			case SPRITECOLOUR:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSpriteColourStatement(inExp);
				break;
				
			case SPRITECOLOUROFF:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSpriteColourOffStatement(inExp->ReleaseArithExp());
				break;
				
			case SETSPRITEBANK:
				Try_
				{
					SimpleTypeCheck("a,a",inExp,inEndLineLoc);
					result=new CSetSpriteBankStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("a,s",inExp,inEndLineLoc);
					result=new CSetSpriteBankStatement(inExp);
				}
				break;
		
			case SETSPRITEPRIORITY:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpritePriorityStatement(inExp);
				break;
			
			case SETSPRITEANGLE:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteAngleStatement(inExp);
				break;

			case SETSPRITEXSCALE:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteXScaleStatement(inExp);
				break;
				
			case SETSPRITEYSCALE:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteYScaleStatement(inExp);
				break;
				
			case SETSPRITEXFLIP:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteXFlipStatement(inExp);
				break;
				
			case SETSPRITEYFLIP:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CSetSpriteYFlipStatement(inExp);
				break;
				
			case SPRITECROP:
				SimpleTypeCheck("a,a,ata,a",inExp,inEndLineLoc);
				result=new CSpriteCropStatement(inExp);
				break;
				
			case SPRITECROPOFF:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CSpriteCropOffStatement(inExp);
				break;
				
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊMoan
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case MOVESPRITE:
			case MOVEVIEWPORT:
				SimpleTypeCheck("a,a,a,a",inExp,inEndLineLoc);
				result=new CMoveSpriteStatement(inTokenId==MOVESPRITE,inExp);
				break;
				
			case ROTATESPRITE:
				SimpleTypeCheck("a,A,a",inExp,inEndLineLoc);
				result=new CRotateSpriteStatement(inExp);				
				break;
				
			case SCALESPRITE:
				SimpleTypeCheck("a,a,a,a",inExp,inEndLineLoc);
				result=new CScaleSpriteStatement(inExp);
				break;
				
			case PAUSECHANNEL:
			case UNPAUSECHANNEL:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CSetChanAttributeStatement(inExp ? inExp->ReleaseArithExp() : 0,inTokenId,inTokenId==PAUSECHANNEL);
				break;
				
			case RESETCHAN:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CResetMoanChannelStatement(inExp ? inExp->ReleaseArithExp() : 0);
				break;
				
			case RESTARTCHAN:
				SimpleTypeCheck("A",inExp,inEndLineLoc);
				result=new CRestartMoanChannelStatement(inExp ? inExp->ReleaseArithExp() : 0);				
				break;
				
			case SETCHANREG:
				Try_
				{
					SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);		// local channel vers: chan, reg
					result = new CSetChannelRegStatement(CSetChannelRegStatement::kLocalNum,inExp);
				}
				Catch_(err)
				{
					Try_
					{
						SimpleTypeCheck("a,a",inExp,inEndLineLoc);			// global channel vers: reg
						result = new CSetChannelRegStatement(CSetChannelRegStatement::kGlobalNum,inExp);
					}
					Catch_(err)
					{
						Try_
						{
							SimpleTypeCheck("a,s,a",inExp,inEndLineLoc);			// local channel vers: chan,reg
							result = new CSetChannelRegStatement(CSetChannelRegStatement::kLocalStr,inExp);					
						}
						Catch_(err)
						{
							SimpleTypeCheck("s,a",inExp,inEndLineLoc);			// global channel vers: reg
							result = new CSetChannelRegStatement(CSetChannelRegStatement::kGlobalStr,inExp);											
						}
					}
				}
				break;
			
			case ANIMSPRITE:
				// NB: type check done in constructor
				result=new CAnimSpriteStatement(inExp,inEndLineLoc);
				break;

			case SETMOANPROG:
				SimpleTypeCheck("a,s",inExp,inEndLineLoc);
				result=new CSetMoanProgramStatement(inExp);
				break;
				
			case ADDSPRITETOCHAN:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CEditChanObsStatement(inExp,true,true);
				break;
				
			case ADDVIEWPORTTOCHAN:
				SimpleTypeCheck("a,a",inExp,inEndLineLoc);
				result=new CEditChanObsStatement(inExp,false,true);
				break;
				
			case REMOVESPRITEFROMCHAN:
				SimpleTypeCheck("a,A",inExp,inEndLineLoc);
				result=new CEditChanObsStatement(inExp,true,false);
				break;

			case REMOVEVIEWPORTFROMCHAN:
				SimpleTypeCheck("a,A",inExp,inEndLineLoc);
				result=new CEditChanObsStatement(inExp,false,false);
				break;
				
			case AUTOMOANON:
			case AUTOMOANOFF:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CSetMoanAttributesStatement(inTokenId,inTokenId==AUTOMOANON);
				break;
				
			case STEPMOAN:
				SimpleTypeCheck("",inExp,inEndLineLoc);
				result=new CStepMoanStatement();
				break;

			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊImages
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case LOADIMAGES:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CLoadImagesStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CLoadImagesStatement(inExp);
				}
				break;
			
			case UNLOADIMAGES:
				Try_
				{
					SimpleTypeCheck("a",inExp,inEndLineLoc);
					result=new CUnloadImagesStatement(inExp);
				}
				
				Catch_(err)
				{
					SimpleTypeCheck("s",inExp,inEndLineLoc);
					result=new CUnloadImagesStatement(inExp);
				}
				break;
			
			case PASTEIMAGE:
				SimpleTypeCheck("a,a,a,A",inExp,inEndLineLoc);
				result=new CPasteImageStatement(inExp);
				break;

			case COPYIMAGE:
				Try_
				{
					// colour mask
					SimpleTypeCheck("a,a,a,a,a,ata,A",inExp,inEndLineLoc);		// x,y,w,h,c,shrink to id,[bank]
					result=new CCopyImageStatement(inExp,false);
				}
				Catch_(err)
				{
					// greyscale mask
					SimpleTypeCheck("a,a,a,a,a,a,a,ata,A",inExp,inEndLineLoc);	// x,y,w,h,mcanvas,mx,my,shrink to id,[bank]
					result=new CCopyImageStatement(inExp,true);
				}
				break;
				
			case DELETEIMAGE:
				SimpleTypeCheck("a,A",inExp,inEndLineLoc);
				result=new CDeleteImageStatement(inExp);
				break;
			
			case SETIMAGECOLLISION:
				SimpleTypeCheck("a,a,a,A",inExp,inEndLineLoc);
				result=new CSetImageCollisionStatement(inExp);
				break;
			
			case SETIMAGEOFFSETS:
				SimpleTypeCheck("a,a,a,A",inExp,inEndLineLoc);					// image,x,y V image,bank,x,y
				result=new CSetImagePropStatement(SETIMAGEOFFSETS,inExp);
				break;
		
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			//		¥ÊViewports
			// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
			case CREATEVIEWPORT:
				SimpleTypeCheck("a,a,ata,a",inExp,inEndLineLoc);
				result=new CCreateViewportStatement(inExp,false);
				break;
				
			case CREATEMAPVIEWPORT:
				SimpleTypeCheck("a,a,a,ata,a",inExp,inEndLineLoc);
				result=new CCreateViewportStatement(inExp,true);
				break;
		
			case REMOVEVIEWPORT:
				SimpleTypeCheck("a",inExp,inEndLineLoc);
				result=new CRemoveViewportStatement(inExp->ReleaseArithExp());
				break;
		
			case VIEWPORTOFFSET:
				SimpleTypeCheck("a,a,a",inExp,inEndLineLoc);
				result=new CViewportOffsetStatement(inExp);
				break;
		
			default:
				SignalPStr_("\pUFunctionMgr::MakeStatement - Unknown token id, cannot create statement");
				Throw_(-1);
				break;
		}
		
		UTBException::ThrowIfNoParserMemory(result);
	}
	
	Catch_(err)	
	{
		// Log the error
		CProgram::GetParsingProgram()->LogError(err);
	}

	delete inExp;
	
	return result;
}
