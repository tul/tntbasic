// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Drawing_Statements.h
// © Mark Tully and John Treece-Birch 2000
// 5/1/00
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
#include	"UExpressions.h"
#include	"UTBException.h"

class CCoOrdPair : public CSimpleListNodeT<CCoOrdPair>
{
	protected:
		StArithExpression			mX,mY;
		
	public:
								CCoOrdPair(
									CArithExpression	*inX,
									CArithExpression	*inY) :
									CSimpleListNodeT<CCoOrdPair>(this),
									mX(inX),
									mY(inY)
									{}
									
		virtual					~CCoOrdPair()
									{
										delete mTail;
									}
								
		TTBInteger				GetX(
									CProgram			&ioState)			{ return mX(ioState); };
		TTBInteger				GetY(
									CProgram			&ioState)			{ return mY(ioState); };
};

class CDrawPictResStatement : public CStatement
{
	protected:
		StArithExpression		mPicId,mX,mY,mCanvasId;
		StStrExpression			mPicName;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
		
	public:
								CDrawPictResStatement(
									CGeneralExpression	*inExp)
								{
									switch (inExp->Length())
									{
										case 1:
											if (inExp->GetArithExp())
												mPicId=inExp->ReleaseArithExp();
											else
												mPicName=inExp->ReleaseStrExp();
											break;
											
										case 2:
											if (inExp->GetArithExp())
												mPicId=inExp->ReleaseArithExp();
											else
												mPicName=inExp->ReleaseStrExp();
											
											inExp=inExp->TailData();
											mCanvasId=inExp->ReleaseArithExp();
											break;
											
										case 3:
											if (inExp->GetArithExp())
												mPicId=inExp->ReleaseArithExp();
											else
												mPicName=inExp->ReleaseStrExp();
											
											inExp=inExp->TailData();
											mX=inExp->ReleaseArithExp();
											
											inExp=inExp->TailData();
											mY=inExp->ReleaseArithExp();
											break;
									}
								}
};

class CPaintAllStatement : public CStatement
{
	protected:
		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
};

class CIntegerArraySymbol;

class CCanvasFXStatement : public CStatement
{
	protected:
		CIntegerArraySymbol		*mMatrix;
		StArithExpression		mFromCanvas,mToCanvas;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);	
	public:
								CCanvasFXStatement(
									CIntegerArraySymbol	*inMatrix,
									CArithExpression	*inFromCanvas,
									CArithExpression	*inToCanvas) :
									mFromCanvas(inFromCanvas),
									mToCanvas(inToCanvas),
									mMatrix(inMatrix)
									{}
};

class CCopyCanvasStatement : public CStatement
{
	protected:
		StArithExpression		mFromCanvas,mToCanvas;
		StArithExpression		mTop,mLeft,mBottom,mRight;
		StArithExpression		mDestX,mDestY;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);
	
	public:
								CCopyCanvasStatement(
									CGeneralExpression	*inExp)
								{
									if (inExp->Length()==2)
									{										
										mFromCanvas=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mToCanvas=inExp->ReleaseArithExp();
									}
									else if (inExp->Length()==8)
									{
										mFromCanvas=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mLeft=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mTop=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mRight=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mBottom=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mToCanvas=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mDestX=inExp->ReleaseArithExp();
										
										inExp=inExp->TailData();
										mDestY=inExp->ReleaseArithExp();
									}
								}
};

class CDrawTextStatement : public CStatement
{
	protected:
		StArithExpression		mX,mY;
		StStrExpression			mStr;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);

	public:
								CDrawTextStatement(
									CGeneralExpression	*inExp)
								{									
									mX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mY=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mStr=inExp->ReleaseStrExp();
								}
};

class CTextFontStatement : public CStatement
{
	protected:
		StStrExpression			mFName;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);

	public:
								CTextFontStatement(
									CStrExpression		*inStr) :
									mFName(inStr)
									{}
};

class CTextSizeStatement : public CStatement
{
	protected:
		StArithExpression		mSize;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);

	public:
								CTextSizeStatement(
									CArithExpression	*inSize) :
									mSize(inSize)
									{}
};

class CTextFaceStatement : public CStatement
{
	protected:
		StArithExpression		mFace;

		virtual void /*e*/		ExecuteSelf(
									CProgram			&ioState);

	public:
								CTextFaceStatement(
									CArithExpression	*inFace) :
									mFace(inFace)
									{}
};

class CLineStatement : public CStatement
{
	protected:
		StArithExpression		mX1,mY1,mX2,mY2;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CLineStatement(
									CGeneralExpression	*inExpression)
								{								
									mX1=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mY1=inExpression->ReleaseArithExp();
								
									inExpression=inExpression->TailData();
									mX2=inExpression->ReleaseArithExp();
								
									inExpression=inExpression->TailData();
									mY2=inExpression->ReleaseArithExp();
								}
};

class CFillRectStatement : public CStatement
{
	protected:
		StArithExpression		mTop,mLeft,mBottom,mRight;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFillRectStatement(
									CGeneralExpression	*inExpression)
								{
									mLeft=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTop=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mRight=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mBottom=inExpression->ReleaseArithExp();
								}
};

class CFrameRectStatement : public CStatement
{
	protected:
		StArithExpression		mTop,mLeft,mBottom,mRight;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFrameRectStatement(
									CGeneralExpression	*inExpression)
								{
									mLeft=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTop=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mRight=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mBottom=inExpression->ReleaseArithExp();
								}
};

class CFillOvalStatement : public CStatement
{
	protected:
		StArithExpression		mTop,mLeft,mBottom,mRight;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFillOvalStatement(
									CGeneralExpression	*inExpression)
								{
									mLeft=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTop=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mRight=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mBottom=inExpression->ReleaseArithExp();
								}
};

class CFrameOvalStatement : public CStatement
{
	protected:
		StArithExpression		mTop,mLeft,mBottom,mRight;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFrameOvalStatement(
									CGeneralExpression	*inExpression)
								{
									mLeft=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTop=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mRight=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mBottom=inExpression->ReleaseArithExp();
								}
};

class CFillPolyStatement : public CStatement
{
	protected:
		CCoOrdPair				*mCoOrds;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFillPolyStatement(CGeneralExpression	*inExpression,
												   const STextRange		&inListTerminatingLoc);
		
		virtual					~CFillPolyStatement()
									{
										delete mCoOrds;
									}
	
};

class CFramePolyStatement : public CStatement
{
	protected:
		CCoOrdPair				*mCoOrds;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CFramePolyStatement(CGeneralExpression	*inExpression,
													const STextRange	&inListTerminatingLoc);
		
		virtual					~CFramePolyStatement()
									{
										delete mCoOrds;
									}
};

class CSetPixelColourStatement : public CStatement
{
	protected:
		StArithExpression		mNewColour,mX,mY;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CSetPixelColourStatement(
									CGeneralExpression	*inExpression)
								{									
									mX=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mY=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mNewColour=inExpression->ReleaseArithExp();
								}
};

class CGetDrawTextDim : public CFixedLocArithExpression
{
	protected:
		StStrExpression			mStr;
		int						mToken;

		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);		
	public:
								CGetDrawTextDim(
									int					inToken,
									CStrExpression		*inStrExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mStr(inStrExp),
									mToken(inToken)
									{}								
};

class CGetPixelFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mX,mY;

		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);

	public:
								CGetPixelFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mY=inExp->ReleaseArithExp();
								}
};

class CSetPenColourStatement : public CStatement
{
	protected:
		StArithExpression		mColour;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CSetPenColourStatement(
									CArithExpression	*inColour) :
									mColour(inColour)
									{}
};

class CGetPenColourFunction : public CFixedLocArithExpression
{
	protected:	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);

	public:
								CGetPenColourFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CSetPenTransparencyStatement : public CStatement
{
	protected:
		StArithExpression		mTransp;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CSetPenTransparencyStatement(
									CArithExpression	*inTransp) :
									mTransp(inTransp)
									{}
};

class CGetPenTransparencyFunction : public CFixedLocArithExpression
{
	protected:	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);

	public:
								CGetPenTransparencyFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CSetRedComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour,mComponent;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CSetRedComponentFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mOriginalColour=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mComponent=inExp->ReleaseArithExp();
								}
};

class CGetRedComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetRedComponentFunction(
									CArithExpression	*inOriginalColour,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mOriginalColour(inOriginalColour)
									{}
};

class CSetGreenComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour,mComponent;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CSetGreenComponentFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mOriginalColour=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mComponent=inExp->ReleaseArithExp();
								}
};

class CGetGreenComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetGreenComponentFunction(
									CArithExpression	*inOriginalColour,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mOriginalColour(inOriginalColour)
									{}
};

class CSetBlueComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour,mComponent;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CSetBlueComponentFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mOriginalColour=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mComponent=inExp->ReleaseArithExp();
								}
};

class CGetBlueComponentFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mOriginalColour;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetBlueComponentFunction(
									CArithExpression	*inOriginalColour,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mOriginalColour(inOriginalColour)
									{}
};

class CMakeColourFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mRedComponent,mGreenComponent,mBlueComponent;
	
		virtual TTBInteger		EvaluateInt(
									CProgram			&ioState);
									
	public:
								CMakeColourFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mRedComponent=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mGreenComponent=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mBlueComponent=inExp->ReleaseArithExp();
								}
};

class CDrawTrackingStatement : public CStatement
{
	protected:
		bool					mTrackingOn;
	
		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CDrawTrackingStatement(
									bool	inOn) :
									mTrackingOn(inOn)
									{}
};

class CInvalRectStatement : public CStatement
{
	protected:
		StArithExpression		mTop,mLeft,mBottom,mRight;

		virtual void			ExecuteSelf(
									CProgram			&ioState);

	public:
								CInvalRectStatement(
									CGeneralExpression	*inExpression)
								{
									mLeft=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mTop=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mRight=inExpression->ReleaseArithExp();
									
									inExpression=inExpression->TailData();
									mBottom=inExpression->ReleaseArithExp();									
								}
};
