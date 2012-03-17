// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CJumpStatement.h
// © Mark Tully 1999
// 24/12/99
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

#include			"CStatement.h"
#include			"CLinkableObject.h"

class				CArithExpression;
class				CLabelTable;

class CJumpStatement : public CStatement
{
	protected:
		CStatement			*mDest;
	
	public:
							CJumpStatement(
								CStatement		*inDest)		{ mDest=inDest; }

		virtual void /*e*/	ExecuteSelf(
								CProgram	&ioState);
};

class CGotoStatement : public CJumpStatement, public CLinkableObject
{
	protected:
		CCString			mLabelName;
		CLabelTable			*mLabelTable;
		STextRange			mLabelLocation;			// need this so we can indicate an error when linking
	
	public:
							CGotoStatement(
								const char 			*inStr,
								CProgram			&inProg,
								const STextRange	&inLabelRange);
		void /*e*/			Link(
								CProgram	&inState);							
};

class CConditionalJump : public CJumpStatement
{
	private:
		typedef CJumpStatement	inheritedJump;

	protected:
		CArithExpression	*mCondition;
		bool				mJumpState;
		
	public:
							CConditionalJump(
								CArithExpression		*inCond,
								CStatement				*inDest,
								bool					inJumpState=true) : CJumpStatement(inDest), mJumpState(inJumpState), mCondition(inCond) {}

		virtual				~CConditionalJump();
		
		virtual void /*e*/	ExecuteSelf(
								CProgram	&ioState);		
};