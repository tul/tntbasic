// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CBLEncodedImage16.h
// © Mark Tully 2000
// 7/1/00
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

#include		"BLAST_Types.h"
#include		"CBLSpriteImage.h"
#include		"CBLCollisionMask.h"

class CBLEncodedImage16 : public CBLSpriteImage
{
	protected:
		TBLFlattenedImage16H			mEncodedData;
		bool							mOwnsData;

		void 							BuildBitMask(
											UInt8*				outMask[8],
											SFlattenedImage16	*inData,
											UInt8				inTolerance,
											UInt8				inCharWidth);
											
		void							SetMaskBit(
											UInt8				*inPtr,
											TBLImageVector		inX,
											TBLImageVector		inY,
											UInt8				inCharWidth);

	public:
										CBLEncodedImage16(
											TBLFlattenedImage16H	inEncodedData=0L,
											bool					inAdopt=true);
		virtual							~CBLEncodedImage16();

		virtual void					GetImageSize(
											TBLImageVector			&outWidth,
											TBLImageVector			&outHeight);

		virtual void					GetImageOffset(
											TBLImageVector			&outX,
											TBLImageVector			&outY);

		virtual void					SetImageOffset(
											TBLImageVector			inX,
											TBLImageVector			inY);
		
		virtual TBLFlattenedImage16H	GetEncodedData()			{return mEncodedData;};
		
		virtual void					DrawNoClip(
											TBLImageVector			inX,
											TBLImageVector			inY,
											CBLCanvas				&inCanvas);
											
		virtual void					DrawWithClip(
											TBLImageVector			inX,
											TBLImageVector			inY,
											CBLCanvas				&inCanvas,
											const Rect				&inClipRect);
											
		virtual void					CreateCollisionMask(
											SInt16					inTolerance);

		virtual void					DeleteCollisionMask();
		
		virtual bool					RectCollision(
											Rect					&inRect);

		virtual bool					SpriteCollision(
											CSpriteImage			&inOther,
											Rect					&rectA,
											Rect					&rectB);
											
		virtual bool					BitmapCollisions()				{ return mColMask; }
};