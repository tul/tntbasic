// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CGeneralExpression.h
// © Mark Tully and TNT Software 2000
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

#pragma once

#include	"TNT_Debugging.h"
#include	"UExpressions.h"
#include	"CSimpleListNodeT.h"
#include	"BisonTokenLocation.h"

class CGeneralExpression : public CSimpleListNodeT<CGeneralExpression>
{
	public:
		enum
		{
			kUnspecified,	// shouldn't be set to this, this is just the initial value
			kComma=',',
			kSemiColon=';',
			kTo='t'
		};
		typedef char		TSeperatorClass;
		
	protected:
		TSeperatorClass		mSeperator;
		STextRange			mSepLoc;
		StArithExpression	mArith;
		StStrExpression		mStr;
			
	public:
							CGeneralExpression() : CSimpleListNodeT<CGeneralExpression>(this), mSeperator(kUnspecified) {}
							CGeneralExpression(
								CArithExpression	*inArith) : CSimpleListNodeT<CGeneralExpression>(this), mSeperator(kUnspecified), mArith(inArith) {}
							CGeneralExpression(
								CStrExpression		*inStr)	: CSimpleListNodeT<CGeneralExpression>(this), mSeperator(kUnspecified), mStr(inStr) {}
		virtual				~CGeneralExpression();
		
		TSeperatorClass		GetSeperator() { Assert_((!mTail) || (mSeperator!=kUnspecified)); return mSeperator; }	// can't have a tail element without the seperator being specified
		STextRange			&GetSepLoc()	{ return mSepLoc; }
		void				GetExpLoc(
								STextRange	&outRange);
		
		CArithExpression	*GetArithExp()	{ return mArith.Get(); }
		CStrExpression		*GetStrExp()	{ return mStr.Get(); }
		bool				IsNullExp()		{ return (mArith.Get()==0) && (mStr.Get()==0); }
		static bool			IsNullList(
								CGeneralExpression	*inList);
		CArithExpression	*ReleaseArithExp()	{ return mArith.Release(); }
		CStrExpression		*ReleaseStrExp()	{ return mStr.Release(); }
		
		void				AppendExpression(
								TSeperatorClass		inSep,
								const STextRange	&inRange,
								CGeneralExpression	*inExp)
							{
								// find the last expression in the list and append to that
								CGeneralExpression	*exp=this;
								
								while (exp->TailData())
									exp=exp->TailData();
								
								exp->mSeperator=inSep;
								exp->mSepLoc=inRange;
								exp->SetTail(inExp);
							}
};

class CGeneralExpressionErr : public CGeneralExpression
{
	public:
		static bool			ContainsErrors(
								CGeneralExpression *&inExp);
};