// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CStrExpression.h
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

#pragma once

#include							"CCString.h"
#include							"String_Storage.h"
#include							"UExpressions.h"
#include							"BisonTokenLocation.h"

class CStrExpression
{
	protected:
		CCString					mString;

	public:
									CStrExpression(
										const char			*inString="") : mString(inString) {}
		virtual						~CStrExpression() {}

		virtual void				GetExpLoc(
										STextRange	&outLoc) = 0;

		virtual CCString /*e*/		&Evaluate(
										CProgram			&inState)		{ return mString; }
};

// An str expression with a fixed location in the source text
class CFixedLocStrExpression : public CStrExpression
{
	protected:
		STextRange					mRange;
	
	public:
									CFixedLocStrExpression(
										const STextRange	&inRange,
										const char			*inString="") :
										CStrExpression(inString),
										mRange(inRange)
										{}

		virtual void				GetExpLoc(
										STextRange	&outLoc)	{ outLoc=mRange; }
};

class CStrIdentifier : public CFixedLocStrExpression
{
	protected:
		CStrStorage						*mStrStorage;
		
	public:
										CStrIdentifier(
											const STextRange	&inRange,
											CStrStorage			*inSymbol) :
											CFixedLocStrExpression(inRange)
										{
											mStrStorage=inSymbol;
										}
						
		virtual CCString /*e*/			&Evaluate(
											CProgram			&inState)	{ return mStrStorage->Access(inState); }
};

class CStrLiteral : public CFixedLocStrExpression
{
	public:
									CStrLiteral(
										const STextRange	&inRange,
										const char			*inText);
};

class CStrConcatOp : public CStrExpression
{
	protected:
		CStrExpression				*mLeft,*mRight;
	
	public:
		/*e*/						CStrConcatOp(
										CStrExpression		*inLeft,
										CStrExpression		*inRight);
		virtual						~CStrConcatOp();

		virtual void				GetExpLoc(
										STextRange	&outLoc)
									{
										STextRange	left,right;
										
										mLeft->GetExpLoc(left);
										mRight->GetExpLoc(right);
										
										outLoc.line=left.line;
										outLoc.startOffset=left.startOffset;
										outLoc.endOffset=right.startOffset;
									}

		virtual CCString /*e*/		&Evaluate(
										CProgram			&inState);
};

class CNumToStrFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression			mVal,mDecimalPlaces;
	
	public:
									CNumToStrFunction(
										const STextRange	&inRange,
										CGeneralExpression	*inExp) :
										CFixedLocStrExpression(inRange)
										{
											mVal=inExp->ReleaseArithExp();
											if (inExp->TailData())
												mDecimalPlaces=inExp->TailData()->ReleaseArithExp();
										}

		CCString /*e*/				&Evaluate(
										CProgram			&inState);
};

class CStrToNumFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression				mInString;
	
		TTBFloat					EvaluateFloat(
										CProgram			&ioState);

	public:
									CStrToNumFunction(
										CGeneralExpression		*inExp,
										const STextRange		&inPos) :
										CFixedLocArithExpression(inPos)
									{
										mInString=inExp->ReleaseStrExp();
										mIsFloat=true;
									}								
};

class CLowerCaseFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression				mInString;
	
	public:
									CLowerCaseFunction(
										const STextRange	&inRange,
										CGeneralExpression	*inExp) :
										CFixedLocStrExpression(inRange)
									{
										mInString=inExp->ReleaseStrExp();
									}

		CCString /*e*/				&Evaluate(
										CProgram			&inState);
};

class CUpperCaseFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression				mInString;
	
	public:
									CUpperCaseFunction(
										const STextRange	&inRange,
										CGeneralExpression	*inExp) :
										CFixedLocStrExpression(inRange)
									{
										mInString=inExp->ReleaseStrExp();
									}

		CCString /*e*/				&Evaluate(
										CProgram			&inState);
};

class CRemoveCharFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression				mInString;
		StArithExpression			mIndex;
	
	public:
									CRemoveCharFunction(
										const STextRange	&inRange,
										CGeneralExpression	*inExp) :
										CFixedLocStrExpression(inRange)
									{
										mInString=inExp->ReleaseStrExp();
										inExp=inExp->TailData();
										
										mIndex=inExp->ReleaseArithExp();
									}

		CCString 					&Evaluate(
										CProgram			&inState);
};

class CGetCharFunction : public CFixedLocStrExpression
{
	protected:
		StStrExpression				mInString;
		StArithExpression			mIndex;
	
	public:
									CGetCharFunction(
										const STextRange	&inRange,
										CGeneralExpression	*inExp) :
										CFixedLocStrExpression(inRange)
									{
										mInString=inExp->ReleaseStrExp();
										inExp=inExp->TailData();
										
										mIndex=inExp->ReleaseArithExp();
									}

		CCString 					&Evaluate(
										CProgram			&inState);
};

class CSubStrFunction : public CFixedLocStrExpression
{
	public:
		enum EMode
		{
			kLeftStr,
			kRightStr,
			kMidStr
		};
		
	protected:
		StStrExpression		mInputString;
		StArithExpression	mStart;
		StArithExpression	mLen;
		EMode				mMode;
		
		CCString			&Evaluate(
								CProgram			&ioState);								
	
	public:
							CSubStrFunction(
								const STextRange	&inRange,
								EMode				inMode,
								CGeneralExpression	*inExp);
};