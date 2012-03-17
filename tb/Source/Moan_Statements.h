// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Moan_Statements.h
// © Mark Tully 2002
// 17/2/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#include	"CStatement.h"
#include	"CArithExpression.h"

class CChanAttributesFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mChan;
		int						mAttribute;
	
	public:
								CChanAttributesFunction(
									CArithExpression	*inChan,
									const STextRange	&inPos,
									int					inAtt) :
									CFixedLocArithExpression(inPos),
									mChan(inChan),
									mAttribute(inAtt)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CMoanAttributesFunction : public CFixedLocArithExpression
{
	protected:
		int						mAttribute;
		
	public:
								CMoanAttributesFunction(
									const STextRange	&inPos,
									int					inAtt) :
									CFixedLocArithExpression(inPos),
									mAttribute(inAtt)
									{}
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);							
};

class CStepMoanStatement : public CStatement
{
	protected:
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);		
	
	public:
								CStepMoanStatement() {}
};

class CSetMoanAttributesStatement : public CStatement
{
	protected:
		int						mAttribute;
		TTBInteger				mValue;

		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
		
	public:
								CSetMoanAttributesStatement(
									int					inAtt,
									TTBInteger			inValue) :
									mAttribute(inAtt),
									mValue(inValue)
									{
									}
};


class CMoveSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mXDelta,mYDelta,mSpeed;
		bool					mIsSprite;		// true = sprite, false = viewport

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CMoveSpriteStatement(
									bool				inIsSprite,
									CGeneralExpression	*inExp);
};

class CRotateSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mRotateDelta,mSpeed;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CRotateSpriteStatement(
									CGeneralExpression	*inExp);
};

class CScaleSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mXDelta,mYDelta,mSpeed;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CScaleSpriteStatement(
									CGeneralExpression	*inExp);
};

class CAnimSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mRepeatCount;
		TTBInteger				mAnimFrames;
		Handle					mExpressionsList;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CAnimSpriteStatement(
									CGeneralExpression	*inExp,
									const STextRange	&inRange);
		
		virtual					~CAnimSpriteStatement();
};

class CSetMoanProgramStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId;
		StStrExpression			mProgram;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
	
	public:
								CSetMoanProgramStatement(
									CGeneralExpression	*inExp);
};

class CSetChanAttributeStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId;
		int						mAttribute,mValue;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
	
	public:
								CSetChanAttributeStatement(
									CArithExpression	*inChannelId,
									int					inTokenId,
									int					inValue);
};

class CResetMoanChannelStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
	public:
								CResetMoanChannelStatement(
									CArithExpression	*inChannelId) :
									mChannelId(inChannelId)
									{}
};

class CRestartMoanChannelStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
	public:
								CRestartMoanChannelStatement(
									CArithExpression	*inChannelId) :
									mChannelId(inChannelId)
									{}
};

class CSetChannelRegStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId;		// can be null for global reg
		StArithExpression		mRegId;			// 0-9 or 0-25 depending on whether in global mode
		StStrExpression			mRegIdStr;		// str version can be specified instead
		StArithExpression		mValue;

		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
		
	public:
		enum EType
		{
			kLocalStr,
			kGlobalStr,
			kLocalNum,
			kGlobalNum
		};
		
	protected:
		EType					mType;
		
	public:
	
								CSetChannelRegStatement(
									EType				inParamForm,
									CGeneralExpression	*inParams);
};

class CGetChannelRegFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mChannelId;		// can be null for global reg
		StArithExpression		mRegId;			// 0-9 or 0-25 depending on whether in global mode
		StStrExpression			mRegIdStr;		// str version can be specified instead
		
	public:
		enum EType
		{
			kLocalStr,
			kGlobalStr,
			kLocalNum,
			kGlobalNum
		};
		
	protected:
		EType					mType;
		
	public:
								CGetChannelRegFunction(
									EType				inParamForm,
									CGeneralExpression	*inParams,
									const STextRange	&inRange);

		virtual TTBInteger /*e*/EvaluateInt(
									CProgram		&ioState);
};

class CEditChanObsStatement : public CStatement
{
	protected:
		StArithExpression		mChannelId,mOb;
		bool					mIsSprite,mAddIt;
		StStrExpression			mProgram;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram		&ioState);
	
	public:
								CEditChanObsStatement(
									CGeneralExpression	*inExp,
									bool				inIsSprite,
									bool				inAddIt);
};