// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CBLImageBank.cpp
// © Mark Tully 2000
// 11/1/00
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
	Implements a bank for storing CBLSpriteImages.
*/

#include		"TNT_Debugging.h"
#include		"CBLImageBank.h"
#include		<UMemoryMgr.h>
#include		<TArrayIterator.h>
#include		"CBLCanvas.h"
#include		"TNT_Errors.h"
#include		"TNewHandleStream.h"
#include		"CBLEncodedImage16.h"
#include		"UBLEncodedImage16.h"
#include		"CBLMask16.h"
#include		"UResources.h"
#include		"CResourceContainer.h"
#include		"CResource.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBLImageBank::CBLImageBank()
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CBLImageBank::~CBLImageBank()
{
	ClearBank();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Load													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes the res id of an image bank resource. It parses this and creates CBLEncodedImage16 images from them all, storing
// them at the end of the bank. The outCacheContainer is a res container file to write the encoded images to as they're
// loaded. Pass nil for no caching.
void CBLImageBank::Load(
	CResourceContainer	*inResContainer,
	TResId				inResId,
	CResourceContainer	*outCacheContainer)
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
		TBLFlattenedImage16H	imageH;
		short					picId;
		RGBColour				maskCol;
		short					xoff,yoff;
		UInt8					collisionTolerance=255;
		SImageFlags				flags;
		
		// Image banks < 3 used a 32-bit long with a bit field in it representing certain properties including whether to
		// centre the hotspot
		// Version 3 upwards has more options for the relative positioning of the hotspot and uses an actual C bit field
		// to store the information.
		if (version<3)
		{
			long					bits;
			
			if (version==0)
				stream >> picId >> xoff >> yoff >> bits >> maskCol.red >> maskCol.green >> maskCol.blue;
			else if ((version==1) || (version==2))
				stream >> picId >> xoff >> yoff >> collisionTolerance >> bits >> maskCol.red >> maskCol.green >> maskCol.blue;

			// Extract the bit information into a new style bit field
			flags.SetShrinkToContents((bits & 0x0001)!=0);
			flags.SetHotSpot(((bits & 0x0002) >> 1) ? kCentre : kUseVectorHotspot);
			flags.SetExactCollisions(((bits & 0x0004) >> 2)!=0);
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

		// Version 2 upwards uses rgb in the range of 0-255 inclusive which is closer to the model tb uses
		if (version>=2)
		{
			maskCol.red*=256;
			maskCol.green*=256;
			maskCol.blue*=256;
		}

		// Attempt to load the pre-encoded image
		imageH=UBLEncodedImage16::LoadImage(inResContainer,picId);

		// Load and Encode the image
		if (!imageH)
		{		
			// Load image
			CBLCanvas		*image=LoadImage(inResContainer,picId);
			StDeleter<CBLCanvas>	delC(image);
			
			// Create mask
			CBLMask16		*mask=0L;
			
			if (inResContainer->ResourceExists(kBLImageMaskType,picId))
			{
				Rect			bounds;
				CBLCanvas		*maskCanvas=LoadImageMask(inResContainer,picId);
				
				FSetRect(bounds,0,0,maskCanvas->GetHeight(),maskCanvas->GetWidth());
				
				Try_
				{
					mask=new CBLGreyScaleMask16(maskCanvas,bounds,true);
					ThrowIfMemFull_(mask);
				}
				Catch_(err)
				{
					delete maskCanvas;
					throw;
				}
			}			
			else
			{
				mask=new CBLColourMask16(maskCol);
				ThrowIfMemFull_(mask);
			}
			
			short		outX,outY;
			
			// Encode image
			{
				StDeleter<CBLMask16>	delM(mask);
				Rect					imageBounds;
				
				FSetRect(imageBounds,0,0,image->GetHeight(),image->GetWidth());
				
				// set x and y offsets
				switch (flags.HotSpot())
				{
					case kCentre:
						xoff=image->GetWidth()/2;
						yoff=image->GetHeight()/2;
						break;						
					case kTop:
						xoff=image->GetWidth()/2;
						yoff=0;
						break;
					case kBottom:
						xoff=image->GetWidth()/2;
						yoff=image->GetHeight();
						break;						
					case kLeft:
						xoff=0;
						yoff=image->GetHeight()/2;
						break;						
					case kRight:
						xoff=image->GetWidth();
						yoff=image->GetHeight()/2;
						break;
					case kTopLeft:
						xoff=0;
						yoff=0;
						break;
					case kTopRight:
						xoff=image->GetWidth();
						yoff=0;
						break;						
					case kBottomLeft:
						xoff=0;
						yoff=image->GetHeight();
						break;						
					case kBottomRight:
						xoff=image->GetWidth();
						yoff=image->GetHeight();
						break;
					case kUseVectorHotspot:
						break;
					default:
						Debug_("unknown image hotspot type");
						break;
				}
								
				imageH=UBLEncodedImage16::EncodeImage(*image,imageBounds,mask,flags.ShrinkToContents(),outX,outY);
			}
			
			(**imageH).xoff=xoff-outX;
			(**imageH).yoff=yoff-outY;
			
			// Save this as a pre-encoded image	
			if (outCacheContainer)
				UBLEncodedImage16::SaveImage(outCacheContainer,imageH,picId);
		}
		
		// Create a sprite image with encoded data
		CBLSpriteImage	*spriteImage=new CBLEncodedImage16(imageH);
		ThrowIfMemFull_(spriteImage);
		
		// Build the collision mask
		if (flags.UseExactCollisions())
			spriteImage->CreateCollisionMask(collisionTolerance);
		
		Try_
		{
			mImages.AddItem(spriteImage);
		}
		Catch_(err)
		{
			delete spriteImage;
			throw;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadImageMask													Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns a canvas with the mask of resid specified. Throws if no resource or another error. The canvas depth is 16.
CBLCanvas *CBLImageBank::LoadImageMask(
	CResourceContainer	*inResContainer,
	TResId				inResId)
{

	GWorldPtr		gworld;
	CBLCanvas		*canvas;
	
	{
		StTNTResource		resource(inResContainer,kBLImageMaskType,inResId);
		ThrowIfOSErr_(::NewGWorldWithPicHandle(&gworld,(PicHandle)resource->GetReadOnlyDataHandle(),16)); // <-- Open in 16 bit - only 32 levels of grey but saves us having to write an 8 bit canvas
	}

	Try_
	{
		canvas=CBLCanvas::NewCanvas(gworld,true);
	}
	Catch_(err)
	{
		::DisposeGWorld(gworld);
		throw;
	}
	
	return canvas;
}
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LoadImage														Static /*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns a canvas containing the image specified. Throws if no resource of another error. The canvas depth is dependant on
// the picture.
CBLCanvas *CBLImageBank::LoadImage(
	CResourceContainer	*inResContainer,
	TResId				inResId)
{
	StTNTResource		resource(inResContainer,'PICT',inResId);

	return CBLCanvas::NewCanvasWithPicture((PicHandle)resource->GetReadOnlyDataHandle());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearBank
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Disposes of all images in the bank. MAKE SURE NO SPRITES ARE USING THE IMAGES FIRST!!!!
void CBLImageBank::ClearBank()
{
	{
		TArrayIterator<CSpriteImage*>		imageIt(mImages);
		CSpriteImage						*image;
		
		while (imageIt.Next(image))
			delete image;
	}
	
	mImages.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteImage
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// inIndex is zero based
// removes image from the bank and deletes it
void CBLImageBank::DeleteImage(
	ArrayIndexT		inIndex)
{
	CSpriteImage	*image=mImages[inIndex+1];

	delete image;

	mImages.RemoveItemsAt(1,inIndex+1);
}

