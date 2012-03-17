// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Sprite_Statements.h
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

#include	"CStatement.h"
#include	"CArithExpression.h"
#include	"CGeneralExpression.h"

class CKillSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CKillSpriteStatement(
									CArithExpression	*inExp) :
									mSprite(inExp)
									{}
									
		virtual					~CKillSpriteStatement()
									{}
};


class CSetSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mNewX,mNewY,mNewI;
	
	public:
								CSetSpriteStatement(
									CGeneralExpression	*inExpression)
								{
									int index=0;
								
									// we have a list of maybe arith exps. We copy each expression into the appropiate
									// data member depending on it's index in the list
									for (CGeneralExpression *exp=inExpression; exp; exp=exp->TailData())
									{
										switch (index++)
										{
											case 0: mSprite=exp->ReleaseArithExp(); break;
											
											case 1: mNewX=exp->ReleaseArithExp(); break;
											
											case 2: mNewY=exp->ReleaseArithExp(); break;
											
											case 3: mNewI=exp->ReleaseArithExp(); break;
										}
									}
								}
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CPasteSpriteStatement : public CStatement
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CPasteSpriteStatement(
									CArithExpression	*inSprite) :
									mSprite(inSprite)
								{
								}
									
		virtual					~CPasteSpriteStatement()
									{};
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteTransparencyStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mTransp;
		
	public:
								CSetSpriteTransparencyStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTransp=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CGetSpriteAttributeFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
		int						mAttribute;
	
	public:
								CGetSpriteAttributeFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos,
									int					inAtt) :
									CFixedLocArithExpression(inPos),
									mSprite(inSprite),
									mAttribute(inAtt)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSpriteColourStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mColour;
		
	public:
								CSpriteColourStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mColour=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSpriteColourOffStatement : public CStatement
{
	protected:
		StArithExpression		mSprite;
		
	public:
								CSpriteColourOffStatement(
									CArithExpression	*inSprite) :
									mSprite(inSprite)
									{}
		virtual					~CSpriteColourOffStatement()
									{}
		
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteBankStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mResID;
		StStrExpression			mResName;
		
	public:
								CSetSpriteBankStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									
									if (inExpression->GetArithExp())
										mResID=inExpression->ReleaseArithExp();
									else
										mResName=inExpression->ReleaseStrExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpritePriorityStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mPriority;
		
	public:
								CSetSpritePriorityStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mPriority=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteAngleStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mAngle;
		
	public:
								CSetSpriteAngleStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mAngle=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteXScaleStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mScale;
		
	public:
								CSetSpriteXScaleStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mScale=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteYScaleStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mScale;
		
	public:
								CSetSpriteYScaleStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mScale=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteXFlipStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mValue;
		
	public:
								CSetSpriteXFlipStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mValue=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSetSpriteYFlipStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mValue;
		
	public:
								CSetSpriteYFlipStatement(
									CGeneralExpression	*inExpression)
								{									
									mSprite=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mValue=inExpression->ReleaseArithExp();
								}
											
		virtual void			ExecuteSelf(
									CProgram			&ioState);
};

class CSpriteAngleFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteAngleFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos,true),
									mSprite(inSprite)
									{}
		
		virtual TTBFloat		EvaluateFloat(
									CProgram			&ioState);
};

class CSpriteXScaleFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteXScaleFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos,true),
									mSprite(inSprite)
									{}
		
		virtual TTBFloat		EvaluateFloat(
									CProgram			&ioState);
};

class CSpriteYScaleFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteYScaleFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos,true),
									mSprite(inSprite)
									{}
		
		virtual TTBFloat		EvaluateFloat(
									CProgram			&ioState);
};

class CSpriteXFlipFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteXFlipFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mSprite(inSprite)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSpriteYFlipFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteYFlipFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mSprite(inSprite)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSpriteActiveFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mSprite;
	
	public:
								CSpriteActiveFunction(
									CArithExpression	*inSprite,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mSprite(inSprite)
									{}
		
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
};

class CSpriteCropStatement : public CStatement
{
	protected:
		StArithExpression		mSprite,mTop,mLeft,mBottom,mRight;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CSpriteCropStatement(
									CGeneralExpression	*inExp)
								{
									mSprite=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mLeft=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mTop=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mRight=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mBottom=inExp->ReleaseArithExp();
								}
};

class CSpriteCropOffStatement : public CStatement
{
	protected:
		StArithExpression		mSprite;
		
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CSpriteCropOffStatement(
									CGeneralExpression	*inExpression)
								{
									mSprite=inExpression->ReleaseArithExp();
								}
};