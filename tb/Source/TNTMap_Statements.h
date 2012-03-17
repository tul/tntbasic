// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// TNTMap_Statements.h
// © John Treece-Birch 2000
// 19/5/00
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

#include		"CStatement.h"
#include		"CArithExpression.h"
#include		"CGeneralExpression.h"
#include		"UExpressions.h"
#include		"UTNTMapManager.h"
#include		"CStrExpression.h"

class CLoadTNTMapStatement : public CStatement
{
	protected:
		StArithExpression	mResID;
		StStrExpression		mResName;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CLoadTNTMapStatement(
								CGeneralExpression	*inExp)
							{
								if (inExp->GetArithExp())
									mResID=inExp->ReleaseArithExp();
								else
									mResName=inExp->ReleaseStrExp();
							}
};

class CUnloadTNTMapStatement : public CStatement
{
	protected:
		StArithExpression	mResID;
		StStrExpression		mResName;
	
		void 				ExecuteSelf(
								CProgram			&ioState);

	public:
							CUnloadTNTMapStatement(
								CGeneralExpression	*inExp)
							{
								if (inExp->GetArithExp())
									mResID=inExp->ReleaseArithExp();
								else
									mResName=inExp->ReleaseStrExp();
							}
};

class CSetCurrentTNTMapStatement : public CStatement
{
	protected:
		StArithExpression	mResID;
		StStrExpression		mResName;
	
		void 				ExecuteSelf(
								CProgram			&ioState);

	public:
							CSetCurrentTNTMapStatement(
								CGeneralExpression	*inExp)
							{
								if (inExp->GetArithExp())
									mResID=inExp->ReleaseArithExp();
								else
									mResName=inExp->ReleaseStrExp();
							}
};

class CPolyNameFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mPolyIndex;
			
	public:
								CPolyNameFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inRange) :
									CFixedLocStrExpression(inRange)
								{
									mPolyIndex=inExp->ReleaseArithExp();
								}
									
		CCString				&Evaluate(
									CProgram			&ioState);
};


class CPolyBoundsFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mPolyIndex;
		int						mDimension;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CPolyBoundsFunction(
									int					inPolyDimension,
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mDimension(inPolyDimension)
								{
									mPolyIndex=inExp->ReleaseArithExp();
								}
};

class CGetTileFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mX,mY,mZ;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetTileFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mY=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mZ=inExp->ReleaseArithExp();
								}
};

class CSetMapTileStatement : public CStatement
{
	protected:
		StArithExpression	mX,mY,mZ,mTile;
	
		void /*e*/			ExecuteSelf(
								CProgram			&ioState);

	public:
							CSetMapTileStatement(
								CGeneralExpression	*inExpression)
							{
								mX=inExpression->ReleaseArithExp();
								
								inExpression=inExpression->TailData();
								mY=inExpression->ReleaseArithExp();
								
								inExpression=inExpression->TailData();
								mZ=inExpression->ReleaseArithExp();
								
								inExpression=inExpression->TailData();
								mTile=inExpression->ReleaseArithExp();
							}
};

class CGetTNTMapWidthFunction : public CFixedLocArithExpression
{
	protected:
		bool					mTileProperties;
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CGetTNTMapWidthFunction(
									bool				inGetTileProperties,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mTileProperties(inGetTileProperties)
									{}
};

class CGetTNTMapHeightFunction : public CFixedLocArithExpression
{
	protected:
		bool					mTileProperties;
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
	
	public:								
								CGetTNTMapHeightFunction(
									bool				inGetTileProperties,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mTileProperties(inGetTileProperties)									
									{}
};

class CGetTNTMapLayersFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
	
	public:						
								CGetTNTMapLayersFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CCountObjectsFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);

	public:
								CCountObjectsFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CGetNthObjectNameFunction : public CFixedLocStrExpression
{
	protected:
		StArithExpression		mIndex;
									
	public:
								CGetNthObjectNameFunction(
									CArithExpression	*inIndex,
									const STextRange	&inPos) :
									CFixedLocStrExpression(inPos),
									mIndex(inIndex)
									{}
									
		CCString				&Evaluate(
									CProgram			&ioState);
};

class CGetNthObjectTypeFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mIndex;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNthObjectTypeFunction(
									CArithExpression	*inIndex,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mIndex(inIndex)
									{}
};

class CGetNthObjectXFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mIndex;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNthObjectXFunction(
									CArithExpression	*inIndex,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mIndex(inIndex)
									{}
};

class CGetNthObjectYFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mIndex;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNthObjectYFunction(
									CArithExpression	*inIndex,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mIndex(inIndex)
									{}
};

class CGetNthObjectZFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mIndex;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CGetNthObjectZFunction(
									CArithExpression	*inIndex,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos),
									mIndex(inIndex)
									{}
};

class CCountPolygonsFunction : public CFixedLocArithExpression
{
	protected:
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CCountPolygonsFunction(
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
									{}
};

class CInPolygonFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mIndex,mX,mY,mZ;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CInPolygonFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mX=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mY=inExp->ReleaseArithExp();
									
									inExp=inExp->TailData();
									mZ=inExp->ReleaseArithExp();

									inExp=inExp->TailData();
									mIndex=inExp->ReleaseArithExp();
								}
};

class CMapLineColFunction : public CFixedLocArithExpression
{
	protected:
		StArithExpression		mStartX,mStartY,mEndX,mEndY,mLayer,mStartTile,mEndTile;
	
		TTBInteger				EvaluateInt(
									CProgram			&ioState);
									
	public:
								CMapLineColFunction(
									CGeneralExpression	*inExp,
									const STextRange	&inPos) :
									CFixedLocArithExpression(inPos)
								{
									mStartX=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mStartY=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mEndX=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mEndY=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mLayer=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mStartTile=inExp->ReleaseArithExp();
									inExp=inExp->TailData();
									
									mEndTile=inExp->ReleaseArithExp();
								}
};

class CDrawMapSectionStatement : public CStatement
{
	protected:
		StArithExpression	mLeft,mTop,mRight,mBottom,mLayer,mX,mY;
	
		void 				ExecuteSelf(
								CProgram			&ioState);

	public:
							CDrawMapSectionStatement(
								CGeneralExpression	*inExp)
							{
								mLeft=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mTop=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mRight=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mBottom=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mLayer=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mX=inExp->ReleaseArithExp();
								
								inExp=inExp->TailData();
								mY=inExp->ReleaseArithExp();
								
							}
};