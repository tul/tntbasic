// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Input_Statements.h
// © Mark Tully 2000
// 1/1/00
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

#include		"TNTBasic_Types.h"
#include		"CArithExpression.h"
#include		"CStrExpression.h"
#include		"CStatement.h"
#include		"Marks Routines.h"
#include		"CInputManager.h"
#include		"CProgram.h"
#include		"UExpressions.h"
#include		"CApplication.h"

class CWaitMouseDownStatement : public CStatement
{
	public:
		void						ExecuteSelf(CProgram &inState);
};

class CWaitMouseUpStatement : public CStatement
{
	public:		
		void						ExecuteSelf(CProgram &inState);
};

class CWaitMouseClickStatement : public CWaitMouseDownStatement
{
	public:
									CWaitMouseClickStatement();
};

/*
class CWaitMouseClickStatement : public CStatement
{
	public:		
		void						ExecuteSelf(CProgram &inState)
									{
										if (!Button())
											inState.SetPC(this);
										else
										{
											while (Button())
												;
												
											#if TB_FlushEventsAllTheFeckinTime
											::FlushEvents(kUserInputEventMask,0L);
											#endif
										}
									}
};*/

class CButtonFunction : public CFixedLocArithExpression
{
	public:
									CButtonFunction(
										const STextRange &inPos) :
										CFixedLocArithExpression(inPos)
									{}
	
		TTBInteger					EvaluateInt(CProgram &inState) 			{ return Button(); }
};

class CIsPressedFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression			mKey;
		
	public:
		virtual TTBInteger			EvaluateInt(CProgram &inState);

									CIsPressedFunction(
										CArithExpression	*inKey,
										const STextRange	&inPos) : 
										CFixedLocArithExpression(inPos),
										mKey(inKey)
										{}
};

class CUpFunction : public CFixedLocArithExpression
{
	public:
									CUpFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
	
		virtual TTBInteger 			EvaluateInt(CProgram &inState);
};

class CDownFunction : public CFixedLocArithExpression
{
	public:
									CDownFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
										
		virtual TTBInteger 			EvaluateInt(CProgram &inState);
};

class CLeftFunction : public CFixedLocArithExpression
{
	public:
									CLeftFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
										
		virtual TTBInteger 			EvaluateInt(CProgram &inState);
};

class CRightFunction : public CFixedLocArithExpression
{
	public:
									CRightFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
										
		virtual TTBInteger 			EvaluateInt(CProgram &inState);
};

class CSpaceFunction : public CFixedLocArithExpression
{
	public:
									CSpaceFunction(
										const STextRange	&inPos) :
										CFixedLocArithExpression(inPos)
										{}
										
		virtual TTBInteger 			EvaluateInt(CProgram &inState);
};

class CLoadInputBankStatement : public CStatement
{
	protected:
		StArithExpression		mResID;
		StStrExpression			mResName;
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
		
	public:
								CLoadInputBankStatement(
									CGeneralExpression		*inParams)
								{
									if (inParams->GetArithExp())
										mResID=inParams->ReleaseArithExp();
									else
										mResName=inParams->ReleaseStrExp();
								}
};

class CPollInputFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mInputDevice;

		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CPollInputFunction(
									CArithExpression			*inDevice,
									const STextRange			&inPos) :
									CFixedLocArithExpression(inPos),
									mInputDevice(inDevice)
								{}
};

class CEditInputStatement : public CStatement
{
	public:
		virtual void /*e*/			ExecuteSelf(CProgram &inState);
};

class CSuspendInputStatement : public CStatement
{
	public:
		virtual void /*e*/			ExecuteSelf(CProgram &inState);
};

class CResumeInputStatement : public CStatement
{
	public:
		virtual void /*e*/			ExecuteSelf(CProgram &inState);
};

/*
class CTestControl : public CArithExpression
{
	protected:
		CArithExpression			*mExpression;

	public:
									CTestControl(
										CArithExpression *inExpression) :
										mExpression(inExpression)
										{}
		virtual						~CTestControl();
	
		virtual TTBInteger /*e*/	/*Evaluate();
}*/

class CReadRawKeyFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
	
	public:
								CReadRawKeyFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CRawKeyCheckModsFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mRawKey;
		UInt32					mModToken;
		
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
		
	public:
								CRawKeyCheckModsFunction(
									UInt32				inModToken,
									CArithExpression	*inRaw,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mModToken(inModToken),
									mRawKey(inRaw)
									{}
};

class CRawKeyConvertFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mRawKey;
		UInt32					mConvertTo;
		
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
	
	public:
								CRawKeyConvertFunction(
									UInt32				inConvertTo,
									CArithExpression	*inRawKey,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mRawKey(inRawKey),
									mConvertTo(inConvertTo)
									{}
};

class CRawKeyFilterFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mRawKey;
		UInt32					mFilterToken;
		
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
		
	public:
								CRawKeyFilterFunction(
									UInt32				inFilterToken,
									CArithExpression	*inRawKey,
									const STextRange	&inRange) :
									CFixedLocArithExpression(inRange),
									mRawKey(inRawKey),
									mFilterToken(inFilterToken)
									{}
};
									

class CRaw2StringFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mRawKey;
	
	public:
								CRaw2StringFunction(
									CArithExpression	*inRawKey,
									const STextRange	&inPos) :
									CFixedLocStrExpression(inPos),
									mRawKey(inRawKey)
									{}
									
		CCString				&Evaluate(
									CProgram		&ioState);
};

class CChar2AsciiFunction : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mChar;

		TTBInteger				EvaluateInt(
									CProgram		&ioState);
			
	public:
								CChar2AsciiFunction(
									CStrExpression		*inChar,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mChar(inChar)
									{}
};
		
class CAscii2CharFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mAscii;
		
	public:
								CAscii2CharFunction(
									CArithExpression	*inAscii,
									const STextRange	&inPos) :
									CFixedLocStrExpression(inPos),
									mAscii(inAscii)
									{}
									
		CCString				&Evaluate(
									CProgram		&ioState);
};

class CGetKeyboardRepeatFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
			
	public:
								CGetKeyboardRepeatFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CSetKeyboardRepeatStatement : public CStatement
{
	protected:
		bool						mOn;
		
	public:
									CSetKeyboardRepeatStatement(
										bool		inOn) :
										mOn(inOn)
										{}
										
		void						ExecuteSelf(
										CProgram	&inState);
};

class CClearKeyboardStatement : public CStatement
{
	public:
		void						ExecuteSelf(
										CProgram	&ioState);
};

class CWaitKeyPressStatement : public CStatement
{
	public:
		void						ExecuteSelf(
										CProgram	&ioState);
};

class CSetStrEdContentsStatement : public CStatement
{
	protected:
		StStrExpression				mNewContents;
		
	public:
									CSetStrEdContentsStatement(
										CStrExpression	*inNewContents) :
										mNewContents(inNewContents)
										{}
		
		void						ExecuteSelf(
										CProgram	&ioState);
};

class CSetStrEdCursorStatement : public CStatement
{
	protected:
		StArithExpression			mNewCursor;
		
	public:
									CSetStrEdCursorStatement(
										CArithExpression	*inNewCursor) :
										mNewCursor(inNewCursor)
										{}
		
		void						ExecuteSelf(
										CProgram	&ioState);
};

class CStrEdInputStatement : public CStatement
{
	protected:
		StArithExpression			mRawKey;
		
	public:
									CStrEdInputStatement(
										CArithExpression	*inRawKey) :
										mRawKey(inRawKey)
										{}
		
		void						ExecuteSelf(
										CProgram	&ioState);
};

class CGetStrEdCursorFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram		&ioState);
			
	public:
								CGetStrEdCursorFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetStrEdContentsFunction : public CFixedLocStrExpression
{
	public:
								CGetStrEdContentsFunction(
									const STextRange	&inPos) :
									CFixedLocStrExpression(inPos)
									{}
									
		CCString				&Evaluate(
									CProgram		&ioState);
};
