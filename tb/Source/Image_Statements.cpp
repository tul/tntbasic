// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Image_Statements.cpp
// © Mark Tully 2001
// 15/3/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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
	Statements for manipulating image banks and their images
	
	CPasteImageStatement	: Can be used to stamp an image at a location
	CCountImagesFunction	: Retuns #Êimages in bank
	CGetImagePropFunction	: Returns an image property such as width/height etc.
	CCopyImageStatement		: Encodes an image from a canvas using either a colour mask or a gs mask
	CLoadImagesStatement	: Loads a bank of images for sprites
	CUnloadImagesStatement	: Unloads a bank of images
*/

#include "Image_Statements.h"
#include "CBLImageBank.h"
#include "CGLImageBank.h"
#include "CProgram.h"
#include "CSpriteManager.h"
#include "CGraphicsContext16.h"
#include "CGraphicsContextGL.h"
#include "CSpriteImage.h"
#include "basic.tab.h"
#include "CBLMask16.h"
#include "UBLEncodedImage16.h"
#include "CBLEncodedImage16.h"
#include "CGLCanvas.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CCopyImageStatement::GetMask												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns either a gs mask or a colour mask
CBLMask16 *CCopyImageStatement::GetMask(
	CProgram	&ioState,
	CBLCanvas	&inCanvas,
	TTBInteger	inW,
	TTBInteger	inH)
{
	CBLMask16	*mask=0;

	if (mUseGSMask)
	{
		Rect		gsRect;
		
		gsRect.top=mMY(ioState);
		gsRect.left=mMX(ioState);
		gsRect.bottom=gsRect.top+inH;
		gsRect.right=gsRect.left+inW;
		
		mask=new CBLGreyScaleMask16(&inCanvas,gsRect,false);
	}
	else
	{
		mask=new CBLColourMask16(mColour(ioState));
	}
	
	ThrowIfMemFull_(mask);
	
	return mask;
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CCopyImageStatement::ExecuteSelf											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CCopyImageStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CGraphicsContext	*graphics=ioState.CheckGraphicsMode();
	Rect				encodeRect;
	
	CCanvas				*canvas=graphics->GetWorkCanvas();
	TTBInteger			w,h;

	encodeRect.top=mY(ioState);
	encodeRect.left=mX(ioState);
	encodeRect.bottom=mY2(ioState);
	encodeRect.right=mX2(ioState);
	
	w=FRectWidth(encodeRect);
	h=FRectHeight(encodeRect);
	
	if (w<0 || h<0)
		return;
	
	CSpriteImage	*spriteImage;
	
	if (graphics->GetType()==CGraphicsContext::kBlastGraphicsContext)
	{
		CCanvas					*maskCanvas=mUseGSMask ? ioState.CheckGraphicsMode()->GetCanvas(mMCanvas(ioState)) : canvas;
		SInt16					outX,outY;
		TBLFlattenedImage16H	imageH=UBLEncodedImage16::EncodeImage(*(CBLCanvas*)canvas,encodeRect,GetMask(ioState,*(CBLCanvas*)maskCanvas,w,h),mShrinkToContents(ioState),outX,outY);

		// compensate for shrink to contents
		(**imageH).xoff=-outX;
		(**imageH).yoff=-outY;
							
		// Create a sprite image with encoded data
		spriteImage=new CBLEncodedImage16(imageH);
		
		if (!spriteImage)
		{
			::DisposeHandle((Handle)imageH);
			ThrowIfMemFull_(spriteImage);
		}
			
		// Build the collision mask?
	}
	else
	{
		RGBColour					colour;
		CImageBank::SImageFlags		flags;
		CGLCanvas					*maskCanvas=mUseGSMask ? (CGLCanvas*)ioState.CheckGraphicsMode()->GetCanvas(mMCanvas(ioState)) : NULL;
		Rect						maskRect;
		
		if (mUseGSMask)
		{
			maskRect.top=mMY(ioState);
			maskRect.left=mMX(ioState);
			maskRect.bottom=mMY(ioState)+h;
			maskRect.right=mMX(ioState)+w;
		}
		else
		{
			BlastColour24ToRGBColour(mColour(ioState),&colour);
			
			maskRect=encodeRect;
		}
		
		ThrowIfMemFull_(spriteImage=CGLImageBank::EncodeImage((CGLCanvas*)canvas,maskCanvas,encodeRect,maskRect,colour,0,0,flags));
	}
	
	// Add to the bank
	CImageBank			*bank=0;
	TTBInteger			bankIndex=mBank.IsNull() ? 128 : mBank(ioState);		// bank index is the index of the bank to create if we don't find one
	
	if (mBank.IsNull())
		bank=ioState.GetSpriteManager().GetDefaultSpriteBank();
	
	if (!bank)
		bank=ioState.GetSpriteManager().GetBankWithOpen(bankIndex);

	TTBInteger			imageIndex=mImage(ioState);
	
	if (imageIndex==-1)
		imageIndex=LArray::index_Last;
	
	bank->InsertImage(imageIndex,spriteImage);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CPasteImageStatement::ExecuteSelf											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pastes the image indicated on the the backdrop. Does nothing if the image doesn't exist
void CPasteImageStatement::ExecuteSelf(
	CProgram			&ioState)
{
	CImageBank		*bank;
	
	if (mBank.IsNull())
		bank=ioState.GetSpriteManager().GetDefaultSpriteBank();
	else
		bank=ioState.GetSpriteManager().GetBank(mBank(ioState));

	CSpriteImage		*image=ioState.GetSpriteManager().GetImage(mImage(ioState),bank);

	if (image)
	{
		CGraphicsContext	*graphics=ioState.CheckGraphicsMode();
		CCanvas				*canvas=graphics->GetWorkCanvas();
		
		if (graphics->GetType()==CGraphicsContext::kBlastGraphicsContext)
		{
			Rect				rect;
			TTBInteger			x=mX(ioState),y=mY(ioState);
			
			FSetRect(rect,0,0,canvas->GetHeight(),canvas->GetWidth());

			((CBLSpriteImage*)image)->DrawWithClip(x,y,*(CBLCanvas*)canvas,rect);
			((CBLSpriteImage*)image)->GetImageFrame(x,y,rect);
			canvas->InvalCanvasRect(rect);
		}
		else
		{
			CGLSpriteImage		*glImage=(CGLSpriteImage*)image;
			CGraphicsContextGL	*graphics=(CGraphicsContextGL*)ioState.CheckGraphicsMode();
			CGLCanvas			*canvas=(CGLCanvas*)graphics->GetWorkCanvas();
			TTBInteger			x=mX(ioState),y=mY(ioState);
			float				screenWidth=graphics->GetWidth()/2.0f;
			float				screenHeight=graphics->GetHeight()/2.0f;
			Rect				destRect,copyRect;
			
			destRect.top=y-glImage->GetYOffset();
			destRect.left=x-glImage->GetXOffset();
			destRect.bottom=destRect.top+glImage->GetHeight();
			destRect.right=destRect.left+glImage->GetWidth();
			
			copyRect=destRect;
			
			canvas->ClipRect(copyRect);
			
			// Copy the background to the screen
			glPixelStorei(GL_UNPACK_ROW_LENGTH,canvas->GetWidth());
			glPixelStorei(GL_UNPACK_SKIP_ROWS,canvas->GetHeight()-destRect.bottom);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,destRect.left);
			
			glRasterPos2f(-1.0f,(graphics->GetHeight()-FRectHeight(copyRect))/screenHeight-1.0f);
			glDrawPixels(FRectWidth(copyRect),FRectHeight(copyRect),GL_RGB,GL_UNSIGNED_BYTE,canvas->GetPixels());
			
			glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
			glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
			
			// Render the sprite
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
			float	top,left,bottom,right;
			
			top=-glImage->GetYOffset();
			left=-glImage->GetXOffset();
			bottom=top+glImage->GetHeight();
			right=left+glImage->GetWidth();
		
			top=-top;
			bottom=-bottom;
		
			glPushMatrix();
			glTranslatef(((destRect.left-copyRect.left)+glImage->GetXOffset())/screenWidth-1.0f,(graphics->GetHeight()-((destRect.top-copyRect.top)+glImage->GetYOffset()))/screenHeight-1.0f,0.0f);
			glScalef((float)2.0f/(float)graphics->GetWidth(),(float)2.0f/(float)graphics->GetHeight(),1.0f);
			
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
				
			glBindTexture(GL_TEXTURE_2D,glImage->GetTextureNumber());
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f,glImage->GetHeightPercent()); glVertex3f(left,bottom,0.0f);
				
				glTexCoord2f(0.0f,0.0f); glVertex3f(left,top,0.0f);
				
				glTexCoord2f(glImage->GetWidthPercent(),0.0f); glVertex3f(right,top,0.0f);
				
				glTexCoord2f(glImage->GetWidthPercent(),glImage->GetHeightPercent()); glVertex3f(right,bottom,0.0f);
			glEnd();
			glPopMatrix();
		
			// Shutdown sprite drawing
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);	
				
			// Copy back to thae canvas
			glPixelStorei(GL_PACK_ROW_LENGTH,canvas->GetWidth());
			glPixelStorei(GL_PACK_SKIP_ROWS,canvas->GetHeight()-destRect.bottom);
			glPixelStorei(GL_PACK_SKIP_PIXELS,destRect.left);
			
			glReadPixels(0,graphics->GetHeight()-FRectHeight(copyRect),FRectWidth(copyRect),FRectHeight(copyRect),GL_RGB,GL_UNSIGNED_BYTE,canvas->GetPixels());
			
			glPixelStorei(GL_PACK_ROW_LENGTH,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CCountImagesFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CCountImagesFunction::EvaluateInt(
	CProgram				&ioState)
{
	TTBInteger			bankId=mBank(ioState);
	
	CImageBank			*bank=ioState.GetSpriteManager().GetBank(bankId);
	
	return bank ? bank->CountImages() : 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetImagePropStatement::Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CSetImagePropStatement::CSetImagePropStatement(
	int					inProp,
	CGeneralExpression	*inExpression) :
	mProp(inProp)
{
	switch (inExpression->Length())
	{
		case 3:	// bank id not included : image,newx,newy
			mImage=inExpression->ReleaseArithExp();
			mNewX=inExpression->TailData()->ReleaseArithExp();
			mNewY=inExpression->TailData()->TailData()->ReleaseArithExp();
			break;
			
		case 4:	// bank id included : image,bank,newx,newy
			mImage=inExpression->ReleaseArithExp();
			mBank=inExpression->TailData()->ReleaseArithExp();
			mNewX=inExpression->TailData()->TailData()->ReleaseArithExp();
			mNewY=inExpression->TailData()->TailData()->TailData()->ReleaseArithExp();
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetImagePropStatement::ExecuteSelf								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetImagePropStatement::ExecuteSelf(
	CProgram		&ioState)
{
	CImageBank			*ibank=mBank.IsNull() ? ioState.GetSpriteManager().GetDefaultSpriteBank() : ioState.GetSpriteManager().GetBank(mBank(ioState));
	TTBInteger			imageId=mImage(ioState);
	CSpriteImage		*image=ioState.GetSpriteManager().GetImage(imageId,ibank);
	
	if (!image)
		return;

	switch (mProp)
	{
		case SETIMAGEOFFSETS:
			image->SetImageOffset(mNewX(ioState),mNewY(ioState));
			break;
			
		default:
			SignalString_("CSetImagePropStatement::ExecuteSelf() unknown prop class");
			break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CGetImagePropFunction::EvaluateInt
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger CGetImagePropFunction::EvaluateInt(
	CProgram			&ioState)
{
	CImageBank			*ibank=mBank.IsNull() ? ioState.GetSpriteManager().GetDefaultSpriteBank() : ioState.GetSpriteManager().GetBank(mBank(ioState));
	TTBInteger			imageId=mImage(ioState);
	
	CSpriteImage		*image=ioState.GetSpriteManager().GetImage(imageId,ibank);
	
	if (!image)
		return 0;

	TBLImageVector		a,b;


	switch (mProp)
	{
		case IMAGEWIDTH:
			image->GetImageSize(a,b);
			return a;
			break;

		case IMAGEHEIGHT:
			image->GetImageSize(a,b);
			return b;
			break;

		case IMAGEXOFF:
			image->GetImageOffset(a,b);
			return a;
			break;

		case IMAGEYOFF:
			image->GetImageOffset(a,b);
			return b;
			break;
		
		case IMAGECOLLISIONTOLERANCE:
			return image->GetCollisionTolerance();
			break;
			
		case IMAGECOLLISIONTYPE:
			return image->GetCollisionType();
			break;
		
		default:
			SignalString_("CGetImagePropFunction::EvaluateInt - Unknown property");
			break;
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CSetImageCollisionToleranceStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CSetImageCollisionStatement::ExecuteSelf(
	CProgram		&ioState)
{
	CImageBank		*ibank=mBank.IsNull() ? ioState.GetSpriteManager().GetDefaultSpriteBank() : ioState.GetSpriteManager().GetBank(mBank(ioState));	
	CSpriteImage	*image=ioState.GetSpriteManager().GetImage(mImage(ioState),ibank);
	
	if (image)
	{
		if (mType(ioState))
			image->CreateCollisionMask(mTolerance(ioState));
		else
			image->DeleteCollisionMask();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CDeleteImageStatement::ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CDeleteImageStatement::ExecuteSelf(
	CProgram		&ioState)
{
	// Delete the image from the bank
	CImageBank		*bank=0;
	
	if (mBank.IsNull())
		bank=ioState.GetSpriteManager().GetDefaultSpriteBank();
	else
		bank=ioState.GetSpriteManager().GetBank(mBank(ioState));
	
	if (bank)
	{
		TTBInteger		imageIndex=mImage(ioState); /* index is zero based */
		
		if (imageIndex>=0 && imageIndex<bank->CountImages())
		{	
			ioState.GetSpriteManager().DeleteImage(imageIndex,bank);
			bank->DeleteImage(imageIndex);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CLoadImagesStatement::ExecuteSelf												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Loads the images from disk and encodes them
void CLoadImagesStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ResID		number;

	if (!mResID.IsNull())
		number=mResID(ioState);
	else
	{
		number=ioState.GetResIdForName(kBLImageBankType,mResName(ioState));

//		Str255		string;
//		CopyCAsPStr((unsigned char*)mResName(ioState).mString,string);
//		UTBResourceManager::GetResourceId(string,kBLImageBankType,number);
	}

	ioState.CheckGraphicsMode()->mSpriteManager.UnloadImages(number);	
	ioState.CheckGraphicsMode()->mSpriteManager.LoadImages(ioState.GetResContainer(),number);
	ioState.CheckGraphicsMode()->mSpriteManager.SetDefaultSpriteBank(number);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CUnloadImagesStatement::ExecuteSelf											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CUnloadImagesStatement::ExecuteSelf(
	CProgram			&ioState)
{
	ResID		number;

	if (!mResID.IsNull())
		number=mResID(ioState);
	else
	{
		number=ioState.GetResIdForName(kBLImageBankType,mResName(ioState));
	}
	
	ioState.CheckGraphicsMode()->mSpriteManager.UnloadImages(number);
}
