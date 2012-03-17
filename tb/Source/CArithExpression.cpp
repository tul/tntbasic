// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CArithExpression.cpp
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

/*
	Code for building parse trees for arithmetic expressions
	
	The basic arithmetic expression class encapsulates either an integer or a float expression. It has two evaluate
	functions, one which evaluates to an integer and one to a float. It doesn't matter what the type of the data in the
	expression is (either an integer or a float) as it will be automatically converted.
	
	See ArithExpressions.doc for more info.
*/

#include		"TNT_Debugging.h"
#include		"CArithExpression.h"
#include		"CStrExpression.h"
#include		"TNTBasic_Errors.h"
#include		"CStatement.h"
#include		"CProgram.h"
#include		"basic.tab.h"
#include		<cstdlib>
#include		<string>
#include		<LException.h>

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CIntegerIdentifier::EvaluateInt							/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CIntegerIdentifier::EvaluateInt(
	CProgram			&inState)
{
	return mIntSymbol->GetValue(inState);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CInteger::Constructor										/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Checks for overflow when converting the text
CInteger::CInteger(
	const STextRange &inRange,
	char		*inText) :
	CFixedLocArithExpression(inRange)
{
	mInt=std::atoi(inText);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryArithOp::Constructor								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Throws if the binary op is unrecongised
CBinaryArithOp::CBinaryArithOp(
	CArithExpression	*inLeft,
	CArithExpression	*inRight,
	int					inOpTokenId)
{
	mLeft=inLeft;
	mRight=inRight;
	AssertThrow_(mLeft);
	AssertThrow_(mRight);

	mIsFloat=mLeft->IsFloat() || mRight->IsFloat();

	mOp=inOpTokenId;
	
	switch (inOpTokenId)
	{
		case PLUS:
		case MINUS:
		case TIMES:
		case DIVIDE:
		case POWER:
			break;
			
		default:
			Throw_(kTBErr_UnrecognisedBinaryOp);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryArithOp::Destructor								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBinaryArithOp::~CBinaryArithOp()
{
	delete mLeft;
	delete mRight;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryArithOp::EvaluateInt								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs the binary op indicated on the numbers. Throws if overflow.
TTBInteger CBinaryArithOp::EvaluateInt(
	CProgram		&inState)
{
	if (mIsFloat)
		return (TTBInteger)EvaluateFloat(inState);
	else
	{
		TTBInteger		left,right,result;
		
		left=mLeft->EvaluateInt(inState);
		right=mRight->EvaluateInt(inState);

		switch (mOp)
		{
			case PLUS:
				result=left+right;
				break;
				
			case TIMES:
				result=left*right;
				break;
				
			case MINUS:
				result=left-right;
				break;
				
			case DIVIDE:
				result=left/right;
				break;
				
			case POWER:
				result=(TTBInteger)::pow(left,right);
				break;
			
			default:
				Throw_(-1);
				break;
		}
		
		return result;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryArithOp::EvaluateFloat								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs the binary op indicated on the numbers. Throws if overflow.
TTBFloat CBinaryArithOp::EvaluateFloat(
	CProgram		&inState)
{
	TTBFloat		left,right,result;
	
	left=mLeft->EvaluateFloat(inState);
	right=mRight->EvaluateFloat(inState);

	switch (mOp)
	{
		case PLUS:
			result=left+right;
			break;
			
		case TIMES:
			result=left*right;
			break;
			
		case MINUS:
			result=left-right;
			break;
			
		case DIVIDE:
			result=left/right;
			break;
			
		case POWER:
			result=::powf(left,right);
			break;
		
		default:
			Throw_(-1);
			break;
	}
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryLogicOp::Constructor								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Throws if the binary op is unrecongised
CBinaryLogicOp::CBinaryLogicOp(
	CArithExpression	*inLeft,
	CArithExpression	*inRight,
	int					inOpTokenId)
{
	mLeftStr=mRightStr=0;
	mLeft=inLeft;
	mRight=inRight;
	AssertThrow_(mLeft);
	AssertThrow_(mRight);

	mOp=inOpTokenId;
	
	switch (inOpTokenId)
	{
		case AND:
		case OR:
		case EQUALS:
		case NEQ:
		case GT:
		case LT:
		case GTE:
		case LTE:
			break;
			
		default:
			Throw_(kTBErr_UnrecognisedBinaryOp);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryLogicOp::Constructor								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Throws if the binary op is unrecongised
CBinaryLogicOp::CBinaryLogicOp(
	CStrExpression		*inLeftStr,
	CStrExpression		*inRightStr,
	int					inOpTokenId)
{
	mLeft=mRight=0L;
	mLeftStr=inLeftStr;
	mRightStr=inRightStr;
	AssertThrow_(mLeftStr);
	AssertThrow_(mRightStr);

	mOp=inOpTokenId;
	
	switch (inOpTokenId)
	{
//		case AND:
//		case OR:
		case EQUALS:
		case NEQ:
		case GT:
		case LT:
		case GTE:
		case LTE:
			break;
			
		default:
			Throw_(kTBErr_UnrecognisedBinaryOp);
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryLogicOp::Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBinaryLogicOp::~CBinaryLogicOp()
{
	delete mLeft;
	delete mRight;
	delete mLeftStr;
	delete mRightStr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CBinaryLogicOp::EvaluateInt								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs the binary op indicated on the numbers or strings.
TTBInteger CBinaryLogicOp::EvaluateInt(
	CProgram		&inState)
{
	if (mLeft)		// floats/ints?
	{
		if (mLeft->IsFloat() || mRight->IsFloat())
		{
			TTBFloat		left,right;
			
			left=mLeft->EvaluateFloat(inState);
			right=mRight->EvaluateFloat(inState);

			switch (mOp)
			{
				case EQUALS:
					return left==right;
					break;
					
				case NEQ:
					return left!=right;
					break;
					
				case GT:
					return left>right;
					break;
					
				case LT:
					return left<right;
					break;
					
				case GTE:
					return left>=right;
					break;
					
				case LTE:
					return left<=right;
					break;
			}
		}
		else
		{

			switch (mOp)
			{
				case AND:
					return mLeft->EvaluateInt(inState) && mRight->EvaluateInt(inState);
					break;
					
				case OR:
					return mLeft->EvaluateInt(inState) || mRight->EvaluateInt(inState);
					break;
					
				case EQUALS:
					return mLeft->EvaluateInt(inState)==mRight->EvaluateInt(inState);
					break;
					
				case NEQ:
					return mLeft->EvaluateInt(inState)!=mRight->EvaluateInt(inState);
					break;
					
				case GT:
					return mLeft->EvaluateInt(inState)>mRight->EvaluateInt(inState);
					break;
					
				case LT:
					return mLeft->EvaluateInt(inState)<mRight->EvaluateInt(inState);
					break;
					
				case GTE:
					return mLeft->EvaluateInt(inState)>=mRight->EvaluateInt(inState);
					break;
					
				case LTE:
					return mLeft->EvaluateInt(inState)<=mRight->EvaluateInt(inState);
					break;
			}
		}
	}
	else	// strings
	{
		int				compresult=std::strcmp(mLeftStr->Evaluate(inState),mRightStr->Evaluate(inState));
		
		switch (mOp)
		{
//			case AND:
//				break;
				
//			case OR:
//				break;
				
			case EQUALS:
				return compresult==0;
				break;
				
			case NEQ:
				return compresult!=0;
				break;
				
			case GT:
				return compresult>0;
				break;
				
			case LT:
				return compresult<0;
				break;
				
			case GTE:
				return compresult>=0;
				break;
				
			case LTE:
				return compresult<=0;
				break;
		}		
	}

	Throw_(kTBErr_UnrecognisedBinaryOp);
		
	return 0;		// error if here
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CArithExpression::EvaluateInt								/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to provide an integer value for the expression
// By default it attempts to conver the float into an integer
TTBInteger CArithExpression::EvaluateInt(
	CProgram	&inState)
{
	if (IsFloat())
		return (TTBInteger)EvaluateFloat(inState);		// auto truncate to an integer
	else
	{
		//ThrowStr_(-1,"\pCArithExpression::EvaluateInt() not overidden");
		return 0;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CArithExpression::EvaluateFloat							/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to provide an float value for the expression
// By default it attempts to conver the int into a float
TTBFloat CArithExpression::EvaluateFloat(
	CProgram	&inState)
{
	if (IsFloat())
	{
		//ThrowStr_(-1,"\pCArithExpression::EvaluateFloat() not overidden");
		return 0;
	}
	else
		return (TTBFloat)EvaluateInt(inState);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFloat::Constructor										/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CFloat::CFloat(
	const STextRange &inRange,
	const char		*inText) :
	CFixedLocArithExpression(inRange,true)
{
	mFloat=std::atof(inText);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFloatIdentifier::EvaluateFloat							/*e*/								
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBFloat CFloatIdentifier::EvaluateFloat(
	CProgram			&inState)
{
	return mSymbol->GetValue(inState);
}