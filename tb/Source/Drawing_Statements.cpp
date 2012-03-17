// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Drawing_Statements.cpp
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

/*
	Statements for drawing to the current canvas
	
	CDrawingStatement		: Generic drawing statement which automatically draws changes to screen after execution
	
	CDrawPictResStatement	: Loads a pict resource to the screen at the specified coords (coords optional)
	CPaintAllStatement		: Clears the screen in the current back colour
	CCopyCanvasStatement	: Copies a block of image from one screen to the other OR copies the entire screen
	CCanvasFXStatement		: Applies a matrix to the canvas
	CDrawTextStatement		: Draws some text
	CTextFontStatement		: Changes text font
	CTextSizeStatement		: Changes text size
	CTextFaceStatement		: Changes text face
*/

#include		"Drawing_Statements.h"
#include		"Program_Resources.h"
#include		"CArithExpression.h"
#include		"CProgram.h"
#include		"CBLCanvas16.h"
#include		"CGLCanvas.h"
#include		"CBLMatrix.h"
#include		"CGraphicsContext.h"
#include		"Graphics_Statements.h"
#include		"CStackFrame.h"
#include		"basic.tab.h"
#include		"UResources.h"
#include		"CResource.h"

SInt32			sArray[50];

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CDrawTextStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CDrawTextStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode()->GetWorkCanvas()->DrawText(mX(ioState),mY(ioState),mStr(ioState));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CTextFontStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTextFontStatement::ExecuteSelf(
	CProgram			&ioState)
{
	short		fnum;
	LStr255		str(mFName(ioState).mString);	// convert result from a c to a pstr

	::GetFNum(str,&fnum);	// if the font doesn't exist then fnum returns 0. 0 is the system font, we'll let that do as a substitute

	ioState.CheckGraphicsMode()->GetWorkCanvas()->SetTextFont(fnum);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CTextSizeStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTextSizeStatement::ExecuteSelf(
	CProgram			&ioState)
{
	TTBInteger			size=mSize(ioState);
	
	if (size<=0)
		UTBException::ThrowBadFontSize();

	ioState.CheckGraphicsMode()->GetWorkCanvas()->SetTextSize(size);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CTextFaceStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CTextFaceStatement::ExecuteSelf(
	CProgram			&ioState)
{
	TTBInteger			face=mFace(ioState);
	
	face&=127;	// ignore bits that aren't defined

	ioState.CheckGraphicsMode()->GetWorkCanvas()->SetTextFace(face);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CLoadPictResStatement::ExecuteSelf								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Depending on the constructor evoked there's 3 things we could do now
void CDrawPictResStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ResID			resourceId;
	
	if (!mPicId.IsNull())
	{
		resourceId=mPicId(ioState);
	}
	else
	{
//		Str255		string;
//		CopyCAsPStr((unsigned char*)mPicName(ioState).mString,string);
		
		resourceId=ioState.GetResIdForName('PICT',mPicName(ioState));
//		UTBResourceManager::GetResourceId(string,'PICT',resourceId);
	}
	
//	StProgramResource	resource('PICT',resourceId);
	StTNTResource		resource(ioState.GetResContainer(),'PICT',resourceId);
	
	Rect				picRect=(**(PicHandle)resource->GetReadOnlyDataHandle()).picFrame;
	
	BigEndianRectToNative(picRect);

	ZeroRectCorner(picRect);
	
	if (mCanvasId.IsNull())
	{
		CCanvas				*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
		TTBInteger			x=0,y=0;
		
		if (!mX.IsNull())
			x=mX(ioState);
		if (!mY.IsNull())
			y=mY(ioState);
	
		FOffset(picRect,x,y);
	
		canvas->DrawPict((PicHandle)resource->GetReadOnlyDataHandle(),picRect);
	}
	else
	{
		// If the screen is open, just draw into it at 0,0 - otherwise open a screen of the correct size
		CGraphicsContext		*graphics=ioState.CheckGraphicsMode();
		TTBInteger				canvasId=mCanvasId(ioState);
		
		if (!graphics->mCanvasManager.IsCanvasOpen(canvasId))
			graphics->mCanvasManager.OpenCanvas(canvasId,picRect.right,picRect.bottom);

		graphics->mCanvasManager.GetCanvas(canvasId)->DrawPict((PicHandle)resource->GetReadOnlyDataHandle(),picRect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CPaintAllStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Clears the screen with the current fore colour
void CPaintAllStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode()->GetWorkCanvas()->PaintAll();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CCopyCanvasStatement::ExecuteSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Copies image data between canvases
void CCopyCanvasStatement::ExecuteSelf(
	CProgram			&ioState)
{
	TTBInteger			fromCanvasI=mFromCanvas(ioState);
	TTBInteger			toCanvasI=mToCanvas(ioState);
	CGraphicsContext	*graphics=ioState.CheckGraphicsMode();
	CCanvas				*fromCanvas=graphics->mCanvasManager.GetCanvas(fromCanvasI);
	CCanvas				*toCanvas=graphics->mCanvasManager.GetCanvas(toCanvasI);
	Rect				copyRect;
	SInt16				destX,destY;

	if (mTop.IsNull())		// if top is null then so are all the coords, just copy from one screen to the other
	{
		FSetRect(copyRect,0,0,fromCanvas->GetHeight(),fromCanvas->GetWidth());
		destX=destY=0;
	}
	else
	{
		copyRect.top=mTop(ioState);
		copyRect.left=mLeft(ioState);
		copyRect.bottom=mBottom(ioState);
		copyRect.right=mRight(ioState);

		// Validate rect
		ValidateRect(copyRect);
			
		Rect			sourceClipRect;
		
		FSetRect(sourceClipRect,0,0,fromCanvas->GetHeight(),fromCanvas->GetWidth());
		
		// Clip source rect so that it's with the source canvas
		if (NewClipRect(&copyRect,&sourceClipRect)==csClippedOff) // Clipped off?
			return;
		
		// Calc the dest x and y coords
		destX=mDestX(ioState);
		destY=mDestY(ioState);
	}

	toCanvas->Copy(fromCanvas,copyRect,destX,destY);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CCanvasFXStatement::ExecuteSelf										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Applies a matrix as the image is transferred
void CCanvasFXStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CGraphicsContext	*graphics=ioState.CheckGraphicsMode();
	
	if (graphics->GetType()==CGraphicsContext::kBlastGraphicsContext)
	{
		CCanvas				*source=graphics->mCanvasManager.GetCanvas(mFromCanvas(ioState));
		CCanvas				*dest=graphics->mCanvasManager.GetCanvas(mToCanvas(ioState));
		Rect				sourceRect;

		// Lookup the array store for the array symbol passed
		CIntegerArrayStore	*arrayStore;	
		arrayStore=&mMatrix->GetStackFrame(ioState)->mStorage.GetIntArray(mMatrix->GetIndex());

		// Calc the source rect
		FSetRect(sourceRect,0,0,source->GetHeight(),source->GetWidth());

		// Must be arity 2
		switch (arrayStore->GetArity())
		{
			case 2:		break;	// cool
			case 0:		UTBException::ThrowArrayNotDimensioned(); break;
			default:	UTBException::ThrowMatrixMustBe2D(); break;
		}
		
		// Get the data
		TArray<TTBInteger>	rawData=arrayStore->GetRawData();
		StArrayLocker		lock(rawData);
			
		CBLMatrix			matrix(arrayStore->GetDimension(0),arrayStore->GetDimension(1),6,(TTBInteger*)*rawData.GetItemsHandle());
		
	//	FInsetRect(sourceRect,10,10);
		
		matrix.CalcNormalScale();
		
		((CBLCanvas*)dest)->ApplyMatrix(matrix,*(CBLCanvas*)source,sourceRect,0,0);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CLineStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CLineStatement::ExecuteSelf(
	CProgram			&ioState)
{	
	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	
	canvas->DrawLine(mX1(ioState),mY1(ioState),mX2(ioState),mY2(ioState));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFillRectStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFillRectStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	Rect			rect;
	
	FSetRect(rect,mTop(ioState),mLeft(ioState),mBottom(ioState),mRight(ioState));
	ValidateRect(rect);
	
	canvas->FillRect(rect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFrameRectStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFrameRectStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CCanvas				*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	Rect				rect;
	
	FSetRect(rect,mTop(ioState),mLeft(ioState),mBottom(ioState),mRight(ioState));
	ValidateRect(rect);
	
	canvas->FrameRect(rect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFillOvalStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFillOvalStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CCanvas				*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	Rect				rect;
	
	FSetRect(rect,mTop(ioState),mLeft(ioState),mBottom(ioState),mRight(ioState));
	ValidateRect(rect);
	
	canvas->FillOval(rect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFrameOvalStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFrameOvalStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	Rect			rect;
	
	FSetRect(rect,mTop(ioState),mLeft(ioState),mBottom(ioState),mRight(ioState));
	ValidateRect(rect);
	
	canvas->FrameOval(rect);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFillPolyStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CFillPolyStatement::CFillPolyStatement(
	CGeneralExpression		*inExp,
	const STextRange		&inListTerminatingLoc)
{
	SInt32				amount=0;
	CCoOrdPair			*pair,*nextPair;
	CArithExpression	*expA,*expB;
	STextRange			range=inListTerminatingLoc;

	while (inExp)
	{
		// If there are more than 25 coords then throw
		if (amount>=25)
			UTBException::ThrowTooManyCoOrds();
	
		// Get the current position in the expression
		if (inExp->IsNullExp())
		{
			if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
				range=inExp->GetSepLoc();
		}
		else
			inExp->GetExpLoc(range);
	
		// Check the first parameter is a number
		if (!inExp->GetArithExp())
			UTBException::ThrowExpectedNumExp(range);
	
		// Followed by a comma
		if (inExp->GetSeperator()!=CGeneralExpression::kComma)
			UTBException::ThrowExpectedComma(range);
		
		expA=inExp->ReleaseArithExp();
		
		// Check the second parameter exists
		if (!inExp->TailData())
			UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
		else
			inExp=inExp->TailData();
		
		// Check the second parameter is a number
		if (!inExp->GetArithExp())
		{
			if (inExp->IsNullExp())	// if none, load up the seperator
			{
				if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
					range=inExp->GetSepLoc();
			}
			else
				inExp->GetExpLoc(range);
				
			UTBException::ThrowExpectedNumExp(range);
		}
		
		expB=inExp->ReleaseArithExp();
		
		// Add the next coord pair to the list
		nextPair=new CCoOrdPair(expA,expB);
		
		if (amount==0)
			mCoOrds=nextPair;
		else
			pair->Append(nextPair);
		
		pair=nextPair;
		amount++;
		
		// Check the next parameter is "to"
		if (inExp->GetSeperator()==CGeneralExpression::kTo)
		{			
			if (!inExp->TailData())
				UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
		}
		else if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
		{
			UTBException::ThrowExpectedTo(inExp->GetSepLoc());
		}
		
		inExp=inExp->TailData();
	}
	
	if (amount<1)
		UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFillPolyStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFillPolyStatement::ExecuteSelf(
	CProgram			&ioState)
{
	SInt32				amount=0;
	CCoOrdPair			*inCoOrds=mCoOrds;

	while (inCoOrds)
	{			
		sArray[amount++]=inCoOrds->GetX(ioState);
		sArray[amount++]=inCoOrds->GetY(ioState);
		
		inCoOrds=inCoOrds->TailData();
	}

	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	
	canvas->FillPoly(sArray,amount/2);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFramePolyStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CFramePolyStatement::CFramePolyStatement(
	CGeneralExpression		*inExp,
	const STextRange		&inListTerminatingLoc)
{
	SInt32				amount=0;
	CCoOrdPair			*pair,*nextPair;
	CArithExpression	*expA,*expB;
	STextRange			range=inListTerminatingLoc;

	while (inExp)
	{
		// If there are more than 25 coords then throw
		if (amount>=25)
			UTBException::ThrowTooManyCoOrds();
	
		// Get the current position in the expression
		if (inExp->IsNullExp())
		{
			if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
				range=inExp->GetSepLoc();
		}
		else
			inExp->GetExpLoc(range);
	
		// Check the first parameter is a number
		if (!inExp->GetArithExp())
			UTBException::ThrowExpectedNumExp(range);
	
		// Followed by a comma
		if (inExp->GetSeperator()!=CGeneralExpression::kComma)
			UTBException::ThrowExpectedComma(range);
		
		expA=inExp->ReleaseArithExp();
		
		// Check the second parameter exists
		if (!inExp->TailData())
			UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
		else
			inExp=inExp->TailData();
		
		// Check the second parameter is a number
		if (!inExp->GetArithExp())
		{
			if (inExp->IsNullExp())	// if none, load up the seperator
			{
				if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
					range=inExp->GetSepLoc();
			}
			else
				inExp->GetExpLoc(range);
				
			UTBException::ThrowExpectedNumExp(range);
		}
		
		expB=inExp->ReleaseArithExp();
		
		// Add the next coord pair to the list
		nextPair=new CCoOrdPair(expA,expB);
		
		if (amount==0)
			mCoOrds=nextPair;
		else
			pair->Append(nextPair);
		
		pair=nextPair;
		amount++;
		
		// Check the next parameter is "to"
		if (inExp->GetSeperator()==CGeneralExpression::kTo)
		{			
			if (!inExp->TailData())
				UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
		}
		else if (inExp->GetSeperator()!=CGeneralExpression::kUnspecified)
		{
			UTBException::ThrowExpectedTo(inExp->GetSepLoc());
		}
		
		inExp=inExp->TailData();
	}
	
	if (amount<1)
		UTBException::ThrowExpectedNumExp(inListTerminatingLoc);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CFramePolyStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CFramePolyStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CCoOrdPair			*inCoOrds=mCoOrds;
	SInt32				amount=0;

	while (inCoOrds)
	{			
		sArray[amount++]=inCoOrds->GetX(ioState);
		sArray[amount++]=inCoOrds->GetY(ioState);
		
		inCoOrds=inCoOrds->TailData();
		
		if (amount>=50)
			UTBException::ThrowTooManyCoOrds();
	}

	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	
	canvas->FramePoly(sArray,amount/2);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetPixelColourStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetPixelColourStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CGraphicsContext*	context=ioState.CheckGraphicsMode();
	CCanvas				*canvas=context->GetWorkCanvas();
	TTBInteger			x=mX(ioState);
	TTBInteger			y=mY(ioState);

	if (context->GetType()==CGraphicsContext::kBlastGraphicsContext)
	{
		((CBLCanvas*)canvas)->SetPixel16(x,y,mNewColour(ioState));
		
		// Set pixel statements don't inval the canvas
		if (((CBLCanvas*)canvas)->GetInvalCanvasAsDraw())
		{
			Rect			rect={y,x,y+1,x+1};
			((CBLCanvas*)canvas)->InvalCanvasRect(rect);
		}
	}
	else
	{
		((CGLCanvas*)canvas)->SetPixel24(x,y,mNewColour(ioState));
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetPixelFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetPixelFunction::EvaluateInt(
	CProgram			&ioState)
{
	CGraphicsContext	*context=ioState.CheckGraphicsMode();
	CCanvas				*canvas=context->GetWorkCanvas();
	
	if (context->GetType()==CGraphicsContext::kBlastGraphicsContext)
		return ((CBLCanvas*)canvas)->GetPixel16(mX(ioState),mY(ioState));
	
	return ((CGLCanvas*)canvas)->GetPixel24(mX(ioState),mY(ioState));
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetPenColourStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetPenColourStatement::ExecuteSelf(
	CProgram			&ioState)
{
	RGBColour		theColour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mColour(ioState),&theColour);
	else
		BlastColour16ToRGBColour(mColour(ioState),&theColour);
		
	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	
	canvas->SetForeColourRGB(theColour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetPenColourFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetPenColourFunction::EvaluateInt(
	CProgram			&ioState)
{
	RGBColour		theColour={0xFFFF,0xFFFF,0xFFFF};

	CCanvas			*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
	
	canvas->GetForeColourRGB(theColour);
	
	if (ioState.InHardwareMode())
		return RGBColourToBlastColour24(&theColour);
	else
		return RGBColourToBlastColour16(&theColour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetPenTransparencyStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetPenTransparencyStatement::ExecuteSelf(
	CProgram			&ioState)
{
	TTBInteger		value=mTransp(ioState);

	if (value<0)
		value=0;
	else if (value>100)
		value=100;

	ioState.CheckGraphicsMode()->GetWorkCanvas()->SetOpacity(100-value);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetPenTransparencyFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetPenTransparencyFunction::EvaluateInt(
	CProgram			&ioState)
{
	return 100-ioState.CheckGraphicsMode()->GetWorkCanvas()->GetOpacity();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetRedComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CSetRedComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	colour.red=mComponent(ioState)*257;
	
	if (ioState.InHardwareMode())
		return RGBColourToBlastColour24(&colour);
	else
		return RGBColourToBlastColour16(&colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetRedComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetRedComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	return colour.red/257;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetGreenComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CSetGreenComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	colour.green=mComponent(ioState)*257;
	
	if (ioState.InHardwareMode())
		return RGBColourToBlastColour24(&colour);
	else
		return RGBColourToBlastColour16(&colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetGreenComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetGreenComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	return colour.green/257;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetBlueComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CSetBlueComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	colour.blue=mComponent(ioState)*257;
	
	if (ioState.InHardwareMode())
		return RGBColourToBlastColour24(&colour);
	else
		return RGBColourToBlastColour16(&colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetBlueComponentFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetBlueComponentFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;
	
	if (ioState.InHardwareMode())
		BlastColour24ToRGBColour(mOriginalColour(ioState),&colour);
	else
		BlastColour16ToRGBColour(mOriginalColour(ioState),&colour);

	return colour.blue/257;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CMakeColourFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CMakeColourFunction::EvaluateInt(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode();
	
	RGBColour	colour;	
	
	colour.red=mRedComponent(ioState)*257;
	colour.green=mGreenComponent(ioState)*257;
	colour.blue=mBlueComponent(ioState)*257;
	
	if (ioState.InHardwareMode())
		return RGBColourToBlastColour24(&colour);
	else
		return RGBColourToBlastColour16(&colour);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetDrawTextDim::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetDrawTextDim::EvaluateInt(
	CProgram			&ioState)
{
	Rect		r;

	if (!mStr.IsNull())	
		ioState.CheckGraphicsMode()->GetWorkCanvas()->CalcTextRect(0,0,mStr(ioState),r);
	else
		ioState.CheckGraphicsMode()->GetWorkCanvas()->CalcTextRect(0,0," ",r);

	switch (mToken)
	{
		case TEXTWIDTH:
			return FRectWidth(r);
			break;
			
		case TEXTHEIGHT:
			return FRectHeight(r);
			break;
			
		case TEXTDESCENT:
			return r.bottom;
			break;
			
		default:
			Throw_(-1);
			break;
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CInvalRectStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CInvalRectStatement::ExecuteSelf(
	CProgram			&ioState)
{
	if (ioState.CheckGraphicsMode()->GetType()==CGraphicsContext::kBlastGraphicsContext)
	{
		Rect			rect;
		
		rect.top=mTop(ioState);
		rect.left=mLeft(ioState);
		rect.bottom=mBottom(ioState);
		rect.right=mRight(ioState);
		
		ValidateRect(rect);
		
		CCanvas		*canvas=ioState.CheckGraphicsMode()->GetWorkCanvas();
		
		((CBLCanvas*)canvas)->InvalCanvasRect(rect);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CDrawTrackingStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CDrawTrackingStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ioState.CheckGraphicsMode()->mCanvasManager.SetDrawTracking(mTrackingOn);
}
