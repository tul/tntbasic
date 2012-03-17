// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CArithExpression.h
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

#include							"TNTBasic_Types.h"
//#include							"Integer_Storage.h"
#include							"CSimpleListNodeT.h"
#include							"UExpressions.h"
#include							"BisonTokenLocation.h"

class								CProgram;
class								CFloatStorage;

class CArithExpression
{
	protected:
		bool						mIsFloat;

	public:
									CArithExpression(
										bool	inIsFloat=false) :
										mIsFloat(inIsFloat)
										{}
		virtual						~CArithExpression() {}
		
		
		bool						IsFloat()			{ return mIsFloat; }
		virtual bool				ContainsErrors()	{ return false; }

		virtual void				GetExpLoc(
										STextRange	&outLoc) = 0;
		virtual void				OffsetLocByLines(
										TTBInteger	inLineOffset) = 0;

		virtual TTBFloat /*e*/		EvaluateFloat(
										CProgram	&inState);
		virtual TTBInteger /*e*/	EvaluateInt(
										CProgram	&inState);
};

// An arithmetic expression with a fixed location in the source text
class CFixedLocArithExpression : public CArithExpression
{
	protected:
		STextRange					mRange;
	
	public:
									CFixedLocArithExpression(
										const STextRange	&inRange,
										bool				inIsFloat=false) :
										CArithExpression(inIsFloat),
										mRange(inRange)
										{}

		virtual void				GetExpLoc(
										STextRange	&outLoc)	{ outLoc=mRange; }
		virtual void				OffsetLocByLines(
										TTBInteger	inLineOffset) { mRange.line+=inLineOffset; }
};

class CInteger : public CFixedLocArithExpression
{
	protected:
		TTBInteger						mInt;
		
	public:
										CInteger(
											const STextRange &inRange,
											char		*inText);
		TTBInteger /*e*/				EvaluateInt(
											CProgram	&inState)					{ return mInt; }
};

class		CIntegerStorage;

class CIntegerIdentifier : public CFixedLocArithExpression
{
	protected:
		CIntegerStorage					*mIntSymbol;
		
	public:
										CIntegerIdentifier(
											const STextRange &inRange,
											CIntegerStorage	*inSymbol) : CFixedLocArithExpression(inRange) { mIntSymbol=inSymbol; }
						
		TTBInteger /*e*/				EvaluateInt(
											CProgram	&inState);
};

class CBinaryArithOp : public CArithExpression
{
	protected:
		CArithExpression			*mLeft,*mRight;
		int							mOp;
	
	public:
		/*e*/						CBinaryArithOp(
										CArithExpression	*inLeft,
										CArithExpression	*inRight,
										int					inOpTokenId);
		virtual						~CBinaryArithOp();

		TTBInteger /*e*/			EvaluateInt(
										CProgram	&inState);
		TTBFloat /*e*/				EvaluateFloat(
										CProgram	&inState);

		virtual void				GetExpLoc(
										STextRange	&outLoc)	{ STextRange left,right; mLeft->GetExpLoc(left); mRight->GetExpLoc(right); outLoc=STextRange(left,right); }
		virtual void				OffsetLocByLines(
										TTBInteger	inLineOffset) { mLeft->OffsetLocByLines(inLineOffset); mRight->OffsetLocByLines(inLineOffset); }
										
};

class		CStrExpression;

class CBinaryLogicOp : public CArithExpression
{
	protected:
		CArithExpression			*mLeft,*mRight;
		CStrExpression				*mLeftStr,*mRightStr;
		int							mOp;
	
	public:
		/*e*/						CBinaryLogicOp(
										CArithExpression	*inLeft,
										CArithExpression	*inRight,
										int					inOpTokenId);

		/*e*/						CBinaryLogicOp(
										CStrExpression		*inLeft,
										CStrExpression		*inRight,
										int					inOpTokenId);

		virtual						~CBinaryLogicOp();

		TTBInteger /*e*/			EvaluateInt(
										CProgram			&inState);

		virtual void				GetExpLoc(
										STextRange	&outLoc)	{ STextRange left,right; mLeft->GetExpLoc(left); mRight->GetExpLoc(right); outLoc=STextRange(left,right); }
		virtual void				OffsetLocByLines(
										TTBInteger	inLineOffset) { mLeft->OffsetLocByLines(inLineOffset); mRight->OffsetLocByLines(inLineOffset); }
};

class CUnaryMinusOp : public CFixedLocArithExpression
{
	protected:
		CArithExpression			*mExp;

	public:
									CUnaryMinusOp(
										const STextRange &inRange,
										CArithExpression	*inExp) :
										CFixedLocArithExpression(inRange,inExp->IsFloat()),
										mExp(inExp)
										{}
		virtual						~CUnaryMinusOp()				{ delete mExp; }
		
		TTBInteger /*e*/			EvaluateInt(
										CProgram			&inState) { return -mExp->EvaluateInt(inState); }
		TTBFloat /*e*/				EvaluateFloat(
										CProgram			&inState) { return -mExp->EvaluateFloat(inState); }
};

class CNotOp : public CFixedLocArithExpression
{
	protected:
		CArithExpression			*mExp;
		
	public:
									CNotOp(
										const STextRange &inRange,
										CArithExpression	*inExp) :
										CFixedLocArithExpression(inRange) { mExp=inExp; }
		virtual						~CNotOp()						{ delete mExp; }
		
		TTBInteger /*e*/			EvaluateInt(
										CProgram	&inState)		{ return mExp->EvaluateInt(inState)==0; }
};

class CFloat : public CFixedLocArithExpression
{
	protected:
		TTBFloat			mFloat;
	
	public:
							CFloat(
								const STextRange &inRange,
								const char *inCStr);

		 TTBFloat /*e*/		EvaluateFloat(
								CProgram	&inState) { return mFloat; }
};

class CFloatIdentifier : public CFixedLocArithExpression
{
	protected:
		CFloatStorage					*mSymbol;
		
	public:
										CFloatIdentifier(
											const STextRange &inRange,
											CFloatStorage	*inSymbol) : CFixedLocArithExpression(inRange,true) { mSymbol=inSymbol; }
						
		TTBFloat /*e*/					EvaluateFloat(
											CProgram	&inState);
};
