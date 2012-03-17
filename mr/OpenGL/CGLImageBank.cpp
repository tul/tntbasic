// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CGLImageBank.cpp
// © John Treece-Birch 2002
// 5/2/02
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

#include		"TNT_Debugging.h"
#include		"CGLImageBank.h"
#include		<UMemoryMgr.h>
#include		<TArrayIterator.h>
#include		"CGLCanvas.h"
#include		"TNT_Errors.h"
#include		"TNewHandleStream.h"
#if __MWERKS__
#include		<agl.h>
#else
#include		<AGL/agl.h>
#endif
#include		"CGLTextureManager.h"
#include		"CCString.h"
#include		"UDataFolderManager.h"
#include		"CResourceContainer.h"
#include		"UResources.h"
#include		"CResource.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGLSpriteImage::~CGLSpriteImage()
{
	// FIXME MT 12/12/11 : texture id should be stored in GL typed variable not SInt32
	GLuint	texture=mTextureId;

	glDeleteTextures(1,&texture);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Load												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLImageBank::Load(
	CResourceContainer	*inResContainer,
	TResId				inResId,
	CResourceContainer	*outResCacheFile)
{
	StTNTResource		bank(inResContainer,kBLImageBankType,inResId);
	TNewHandleStream	stream(bank->GetReadOnlyDataHandle(),false);
	short				num,version;
	
	// version check
	stream >> version;
	if (version>3)
		Throw_(kTNTErr_UnsupportedDataFormat);

	// count of entries
	stream >> num;
	
	for (short index=0; index<num; index++)
	{
		SInt16					picId;
		RGBColour				maskCol;
		short					xoff,yoff;
		UInt8					collisionTolerance=255;
		SImageFlags				flags;
		long					bits;
		bool					shrinkToContents,centredHotSpot,bitmapCol;
		
		if (version==0)
			stream >> picId >> xoff >> yoff >> bits >> maskCol.red >> maskCol.green >> maskCol.blue;
		else if ((version==1) || (version==2))
		{
			stream >> picId >> xoff >> yoff >> collisionTolerance >> bits >> maskCol.red >> maskCol.green >> maskCol.blue;

			// Extract the bit information into a new style bit field
			flags.SetShrinkToContents((bits & 0x0001)!=0);
			flags.SetHotSpot(((bits & 0x0002) >> 1) ? kCentre : kUseVectorHotspot);
			flags.SetExactCollisions((bits & 0x0004) >> 2);
		}
		else if (version==3)
		{
			stream >> picId >> xoff >> yoff >> collisionTolerance;
			
			// TEMPORARY FIX - SCALE TTRANSPLEVEL INTO A COLLISION TOLERANCE VALUE
			// Should change the mask building routine to use the level really
			// I'm mapping kTransp100 (very sensitive mask) onto 255 here.
			collisionTolerance=((float)(8-collisionTolerance))*(255.0/8.0);
			
			stream >> flags.mBits;
			//stream.ReadBlock(&flags,sizeof(flags));

			SBLSmallRGB		small;
			stream.ReadBlock(&small,sizeof(small));
			maskCol.red=small.red;
			maskCol.green=small.green;
			maskCol.blue=small.blue;
		}

		// Version 1 upwards uses rgb in the range of 0-255
		if (version>1)
		{
			// nb: multiplying by 257 stops the colour masks working for sprite images
			maskCol.red*=256;
			maskCol.green*=256;
			maskCol.blue*=256;
		}

		// Try to load a mask
		CGLCanvas					*maskCanvas=NULL;
		StDeleter<CGLCanvas>		maskCanvasDel;
		Rect						maskRect={0,0,0,0};
		
		if (inResContainer->ResourceExists(kBLImageMaskType,picId))
		{
			StTNTResource		maskPicture(inResContainer,kBLImageMaskType,picId);
			PicHandle			picH=(PicHandle)maskPicture->GetReadOnlyDataHandle();
		
			maskRect=(*picH)->picFrame;
			BigEndianRectToNative(maskRect);
				
			maskCanvas=new CGLCanvas(FRectWidth(maskRect),FRectHeight(maskRect));
			ThrowIfMemFull_(maskCanvas);
			maskCanvasDel.Adopt(maskCanvas);
			
			maskCanvas->DrawPict(picH,maskRect);
		}

		// Load the image
		Rect					rect;
		CGLCanvas				*canvas=NULL;
		StDeleter<CGLCanvas>	delCanvas;
		
		{
			StTNTResource	image(inResContainer,'PICT',picId);			
			PicHandle		picture=(PicHandle)image->GetReadOnlyDataHandle();
			
			rect=(*picture)->picFrame;
			BigEndianRectToNative(rect);
			canvas=new CGLCanvas(FRectWidth(rect),FRectHeight(rect));
			ThrowIfMemFull_(canvas);			
			delCanvas.Adopt(canvas);
					
			canvas->DrawPict(picture,rect);
		}
		
		UInt16		imageWidth=FRectWidth(rect);
		UInt16 		imageHeight=FRectHeight(rect);
	
		// Calculate the hotspot position
		switch (flags.HotSpot())
		{
			case kTopLeft:
				xoff=0;
				yoff=0;
				break;
			case kTop:
				xoff=imageWidth/2;
				yoff=0;
				break;
			case kTopRight:
				xoff=imageWidth;
				yoff=0;
				break;
			case kRight:
				xoff=imageWidth;
				yoff=imageHeight/2;
				break;
			case kBottomRight:
				xoff=imageWidth;
				yoff=imageHeight;
				break;
			case kBottom:
				xoff=imageWidth/2;
				yoff=imageHeight;
				break;
			case kBottomLeft:
				xoff=0;
				yoff=imageHeight;
				break;
			case kLeft:
				xoff=0;
				yoff=imageHeight/2;
				break;
			case kCentre:
				xoff=imageWidth/2;
				yoff=imageHeight/2;
				break;
		}
	
		CGLSpriteImage	*glImage=EncodeImage(canvas,maskCanvas,rect,rect,maskCol,xoff,yoff,flags);
		
		mImages.AddItem(glImage);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadData												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLImageBank::LoadData(
	Handle			inHandle)
{
	TNewHandleStream	stream(inHandle,false);
	short				num,version;
	
	// version check
	stream >> version;
	if ((version<4) || (version>4))
		Throw_(kTNTErr_UnsupportedDataFormat);

	// count of entries
	stream >> num;
	
	for (short index=0; index<num; index++)
	{
		RGBColour				maskCol;
		short					xoff,yoff;
		UInt8					collisionTolerance=255;
		SImageFlags				flags;
		long					bits;
		bool					shrinkToContents,centredHotSpot,bitmapCol;
		CCString				pictureName;
		
		if (version==4)
		{
			stream >> pictureName >> xoff >> yoff >> collisionTolerance;
			
			// TEMPORARY FIX - SCALE TTRANSPLEVEL INTO A COLLISION TOLERANCE VALUE
			// Should change the mask building routine to use the level really
			// I'm mapping kTransp100 (very sensitive mask) onto 255 here.
			collisionTolerance=((float)(8-collisionTolerance))*(255.0/8.0);
			
			stream >> flags.mBits;
			//stream.ReadBlock(&flags,sizeof(flags));

			SBLSmallRGB		small;
			stream.ReadBlock(&small,sizeof(small));
			maskCol.red=small.red;
			maskCol.green=small.green;
			maskCol.blue=small.blue;
		}

		maskCol.red*=257;
		maskCol.green*=257;
		maskCol.blue*=257;

		// Try to load a mask
		CGLCanvas	*maskCanvas=NULL;
		Rect		maskRect={0,0,0,0};
		
		Try_
		{
			FSSpec						spec=UDataFolderManager::MakeSpec(pictureName,kImageMaskDataType);
			PicHandle					maskPicture;
			GraphicsImportComponent		comp=0;
			
			ThrowIfOSErr_(::GetGraphicsImporterForFile(&spec,&comp));
			ThrowIfOSErr_(::GraphicsImportGetAsPicture(comp,&maskPicture));
			::CloseComponent(comp);
		
			maskRect=(*maskPicture)->picFrame;
			BigEndianRectToNative(maskRect);
				
			maskCanvas=new CGLCanvas(FRectWidth(maskRect),FRectHeight(maskRect));
			
			maskCanvas->DrawPict(maskPicture,maskRect);
			
			::DisposeHandle((Handle)maskPicture);
		}

		Catch_(err)
		{
			if (err.GetErrorCode()!=fnfErr)
				throw;
		}

		// Load the image
		PicHandle					picture;
		FSSpec						fileSpec=UDataFolderManager::MakeSpec(pictureName,kImageDataType);
		GraphicsImportComponent		comp=0;
						
		ThrowIfOSErr_(::GetGraphicsImporterForFile(&fileSpec,&comp));
		ThrowIfOSErr_(::GraphicsImportGetAsPicture(comp,&picture));
		::CloseComponent(comp);
		
		Rect		rect=(*picture)->picFrame;
		BigEndianRectToNative(rect);

		CGLCanvas	*canvas=new CGLCanvas(FRectWidth(rect),FRectHeight(rect));
		
		canvas->DrawPict(picture,rect);
		
		::DisposeHandle((Handle)picture);
		
		UInt16		imageWidth=FRectWidth(rect);
		UInt16 		imageHeight=FRectHeight(rect);
	
		// Calculate the hotspot position
		switch (flags.HotSpot())
		{
			case kTopLeft:
				xoff=0;
				yoff=0;
				break;
			case kTop:
				xoff=imageWidth/2;
				yoff=0;
				break;
			case kTopRight:
				xoff=imageWidth;
				yoff=0;
				break;
			case kRight:
				xoff=imageWidth;
				yoff=imageHeight/2;
				break;
			case kBottomRight:
				xoff=imageWidth;
				yoff=imageHeight;
				break;
			case kBottom:
				xoff=imageWidth/2;
				yoff=imageHeight;
				break;
			case kBottomLeft:
				xoff=0;
				yoff=imageHeight;
				break;
			case kLeft:
				xoff=0;
				yoff=imageHeight/2;
				break;
			case kCentre:
				xoff=imageWidth/2;
				yoff=imageHeight/2;
				break;
		}
	
		CGLSpriteImage	*glImage=EncodeImage(canvas,maskCanvas,rect,rect,maskCol,xoff,yoff,flags);
		
		delete canvas;
		
		if (maskCanvas)
			delete maskCanvas;
		
		mImages.AddItem(glImage);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EncodeImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGLSpriteImage *CGLImageBank::EncodeImage(
	CGLCanvas		*inCanvas,
	CGLCanvas		*inMaskCanvas,
	Rect			&inRect,
	Rect			&inMaskRect,
	RGBColour		&inMaskColour,
	SInt16			inXOff,
	SInt16			inYOff,
	SImageFlags		inFlags)
{
	if (inFlags.ShrinkToContents())
	{
		ShrinkToContents(inCanvas,inMaskCanvas,inRect,inMaskColour,inXOff,inYOff);
	}

	UInt16		imageWidth=FRectWidth(inRect);
	UInt16 		imageHeight=FRectHeight(inRect);
	UInt16		x,y,actualWidth=1,actualHeight=1;
	
	while (actualWidth<imageWidth)
		actualWidth*=2;
		
	while (actualHeight<imageHeight)
		actualHeight*=2;
	
	GLubyte		*image=new GLubyte[actualWidth*actualHeight*4];
	ThrowIfMemFull_(image);
	
	// Set all the alphas to invisible
	for (y=0; y<actualHeight; y++)
	{
		for (x=0; x<actualWidth; x++)
		{
			image[(y*actualWidth+x)*4+3]=0;
		}
	}
			
	// Copy the image into the array	
	for (y=0; y<imageHeight; y++)
	{
		for (x=0; x<imageWidth; x++)
		{
			RGBColor	colour;
			
			inCanvas->GetPixelRGB(inRect.left+x,inRect.top+y,colour);
			
			image[(y*actualWidth+x)*4+0]=colour.red/257;
			image[(y*actualWidth+x)*4+1]=colour.green/257;
			image[(y*actualWidth+x)*4+2]=colour.blue/257;
			
			if (inMaskCanvas)
			{
				RGBColor	maskColour;
				
				inMaskCanvas->GetPixelRGB(inMaskRect.left+x,inMaskRect.top+y,maskColour);
					
				image[(y*actualWidth+x)*4+3]=(maskColour.red/257+maskColour.green/257+maskColour.blue/257)/3;
			}
			else
			{
				if ((image[(y*actualWidth+x)*4+0]==inMaskColour.red/257) &&
					(image[(y*actualWidth+x)*4+1]==inMaskColour.green/257) &&
					(image[(y*actualWidth+x)*4+2]==inMaskColour.blue/257))
				{
					image[(y*actualWidth+x)*4+3]=0;
				}
				else
					image[(y*actualWidth+x)*4+3]=255;
			}
		}
	}
	
	SInt32		texture=CGLTextureManager::GetFreeTexture();
	
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,actualWidth,actualHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
	
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
			
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,1.0f); glVertex3f(0.0f,0.0f,0.0f);
		
		glTexCoord2f(0.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f);
		
		glTexCoord2f(1.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f);
		
		glTexCoord2f(1.0f,1.0f); glVertex3f(0.0f,0.0f,0.0f);
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	
	delete [] image;
	
	CGLSpriteImage	*r=new CGLSpriteImage(texture,
							  imageWidth,
							  imageHeight,
							  (float)imageWidth/(float)actualWidth,
							  (float)imageHeight/(float)actualHeight,
							  inXOff,
							  inYOff);
	ThrowIfMemFull_(r);
	return r;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearBank
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Disposes of all images in the bank. MAKE SURE NO SPRITES ARE USING THE IMAGES FIRST!!!!
void CGLImageBank::ClearBank()
{
	{
		TArrayIterator<CSpriteImage*>			imageIt(mImages);
		CSpriteImage							*image;
		
		while (imageIt.Next(image))
		{
			GLuint	texture=((CGLSpriteImage*)image)->GetTextureNumber();
			glDeleteTextures(1,&texture);
			delete image;
		}
	}
	
	mImages.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// inIndex is zero based
// removes image from the bank and deletes it
void CGLImageBank::DeleteImage(
	ArrayIndexT		inIndex)
{
	CSpriteImage	*image=mImages[inIndex+1];

	delete image;

	mImages.RemoveItemsAt(1,inIndex+1);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShrinktoContents
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGLImageBank::ShrinkToContents(
	CGLCanvas		*inCanvas,
	CGLCanvas		*inMaskCanvas,
	Rect			&ioRect,
	RGBColor		&inMaskColour,
	SInt16			&ioXOff,
	SInt16			&ioYOff)
{
	SInt32		x,y;
	RGBColor	colour;
	
	// Clip the top
	for (y=ioRect.top; y<ioRect.bottom; y++)
	{
		for (x=ioRect.left; x<ioRect.right; x++)
		{
			if (inMaskCanvas)
			{
				inMaskCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=0) ||
					(colour.green!=0) ||
					(colour.blue!=0))
				{
					ioYOff-=(y-ioRect.top);
					ioRect.top=y;
					
					y=ioRect.bottom;
					break;
				}
			}
			else
			{
				inCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=inMaskColour.red) ||
					(colour.green!=inMaskColour.green) ||
					(colour.blue!=inMaskColour.blue))
				{
					ioYOff-=(y-ioRect.top);
					ioRect.top=y;
					
					y=ioRect.bottom;
					break;
				}
			}
		}
	}
	
	// Clip the bottom
	for (y=ioRect.bottom-1; y>=ioRect.top; y--)
	{
		for (x=ioRect.left; x<ioRect.right; x++)
		{
			if (inMaskCanvas)
			{
				inMaskCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=0) ||
					(colour.green!=0) ||
					(colour.blue!=0))
				{
					ioRect.bottom=y+1;
					
					y=ioRect.top;
					break;
				}
			}
			else
			{
				inCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=inMaskColour.red) ||
					(colour.green!=inMaskColour.green) ||
					(colour.blue!=inMaskColour.blue))
				{
					ioRect.bottom=y+1;
					
					y=ioRect.top;
					break;
				}
			}
		}
	}
	
	// Clip the left
	for (x=ioRect.left; x<ioRect.right; x++)
	{
		for (y=ioRect.top; y<ioRect.bottom; y++)
		{
			if (inMaskCanvas)
			{
				inMaskCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=0) ||
					(colour.green!=0) ||
					(colour.blue!=0))
				{
					ioXOff-=(x-ioRect.left);
					ioRect.left=x;
					
					x=ioRect.right;
					break;
				}
			}
			else
			{
				inCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=inMaskColour.red) ||
					(colour.green!=inMaskColour.green) ||
					(colour.blue!=inMaskColour.blue))
				{
					ioXOff-=(x-ioRect.left);
					ioRect.left=x;
					
					x=ioRect.right;
					break;
				}
			}
		}
	}
	
	// Clip the right
	for (x=ioRect.right-1; x>=ioRect.left; x--)
	{
		for (y=ioRect.top; y<ioRect.bottom; y++)
		{
			if (inMaskCanvas)
			{
				inMaskCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=0) ||
					(colour.green!=0) ||
					(colour.blue!=0))
				{
					ioRect.right=x+1;
					
					x=ioRect.left;
					break;
				}
			}
			else
			{
				inCanvas->GetPixelRGB(x,y,colour);
				
				if ((colour.red!=inMaskColour.red) ||
					(colour.green!=inMaskColour.green) ||
					(colour.blue!=inMaskColour.blue))
				{
					ioRect.right=x+1;
					
					x=ioRect.left;
					break;
				}
			}
		}
	}
}
