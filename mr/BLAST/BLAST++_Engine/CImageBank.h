// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// CImageBank.h
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

#pragma once

#include		<TArray.h>
#include		"CSpriteImage.h"
#include		"ResourceTypes.h"

const ResType	kBLImageBankType='ImBk';
const ResType	kBLImageMaskType='IMsk';	// Just a relabled pict resource, 256 greyscale.

// Version 0 : initial
// Version 1 : Added collision tolerance and interpreted more flags
// Version 2 : Changed mask col to be 0-255
// Version 3 : Added interpretation of more flags for hotspot positioning, changed to a SBLSmallRGB for the mask colour

const SInt16	kBLImageBankVersion=3;		// Current version

class CResourceContainer;

class CImageBank
{
	protected:
		TArray<CSpriteImage*>			mImages;
		
	public:
		struct SImageFlags
		{
			enum
			{
				kShrinkToContents		= (1<<31),
				kUseExactCollisions		= (1<<30),

				kHotSpotBits1			= (1<<23),
				kHotSpotBits2			= (1<<22),
				kHotSpotBits3			= (1<<21),
				kHotSpotBits4			= (1<<20),

				kHotSpotBits			= (kHotSpotBits1|kHotSpotBits2|kHotSpotBits3|kHotSpotBits4)
			};

#if 1
			UInt32						mBits;
#else
			union
			{
				UInt32					serialise;
				struct
				{
					unsigned int		shrinkToContents : 1;		// Generally on, if using the images rect for collision detection then you might want to leave it off
					unsigned int		useExactCollisions : 1;		// Decides whether to use the image bounding rect or a pixel perfect collision detection method, for alpha masks the mask tolerance is used to decide what's a hit and a miss
					unsigned int		unused1 : 6;				// pad to byte for easier template editing
					unsigned int		hotspotMode : 4;			// See hotspot mode below
					unsigned int		unused : 20;				// Total size : 32 bits
				};
			};
			
			SImageFlags() :
				shrinkToContents(true),
				useExactCollisions(false),
				hotspotMode(1),
				unused(0)
				{}
#endif

			int							HotSpotBit1()					{ return (mBits&kHotSpotBits1)!=0; }
			int							HotSpotBit2()					{ return (mBits&kHotSpotBits2)!=0; }
			int							HotSpotBit3()					{ return (mBits&kHotSpotBits3)!=0; }
			int							HotSpotBit4()					{ return (mBits&kHotSpotBits4)!=0; }

			bool						ShrinkToContents()				{ return (mBits&kShrinkToContents)!=0; }
			void						SetShrinkToContents(bool inS)	{ mBits=(mBits&~kShrinkToContents)|(inS?kShrinkToContents:0); }

			bool						UseExactCollisions()			{ return (mBits&kUseExactCollisions)!=0; }
			void						SetExactCollisions(bool inS)	{ mBits=(mBits&~kUseExactCollisions)|(inS?kUseExactCollisions:0); }

			int							HotSpot()						{ return (HotSpotBit1()<<3)|(HotSpotBit2()<<2)|(HotSpotBit3()<<1)|(HotSpotBit4()); }
			void						SetHotSpot(
											int			inBits)
										{
											int			bits=
												((inBits&1)?kHotSpotBits4:0) |
												((inBits&2)?kHotSpotBits3:0) |
												((inBits&4)?kHotSpotBits2:0) |
												((inBits&8)?kHotSpotBits1:0);
											mBits=(mBits&~kHotSpotBits)|bits;
										}

										SImageFlags() :
											mBits(kShrinkToContents)
										{
											SetHotSpot(kTopLeft);
										}
		};
		enum
		{
			kUseVectorHotspot,					// Use xoff and yoff
			kTopLeft,							// Relative positions
			kTop,
			kTopRight,
			kRight,
			kBottomRight,
			kBottom,
			kBottomLeft,
			kLeft,
			kCentre
		};
	
		virtual							~CImageBank()					{ }
	
		ArrayIndexT						CountImages()					{ return mImages.GetCount(); }
		
		// Gets the image specified, index is zero based between 0 and CountImages()-1
		CSpriteImage					*&operator[](
											ArrayIndexT		inIndex)	{ return mImages[inIndex+1]; }
		
		virtual void					ClearBank()=0;

		virtual void					DeleteImage(
											ArrayIndexT		inImageIndex)=0;
				bool					ContainsImage(
											CSpriteImage	*inImage);
		
		virtual SInt32					GetImageIndex(
											CSpriteImage	*inImage);
											
		// zero based index
				void /*e*/				InsertImage(
											ArrayIndexT		inIndex,
											CSpriteImage	*inImage);
		
		virtual void /*e*/				Load(
											CResourceContainer	*inResC,
											TResId				inResId,
											CResourceContainer	*outCacheContainer=NULL)=0;
};
