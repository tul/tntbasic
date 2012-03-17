// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// UBLEncodedImage16.h
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

#include				"Blast_Types.h"
#include				"BLAST++.h"
#include				"CBLMask16.h"
#include				"ResourceTypes.h"

const ResType	kBLEncodedImage16ResType_BigEndian='EImg';
const ResType	kBLEncodedImage16ResType_LittleEndian='Eimg';

#if TARGET_RT_LITTLE_ENDIAN
const ResType	kBLEncodedImage16ResType=kBLEncodedImage16ResType_LittleEndian;
#else
const ResType	kBLEncodedImage16ResType=kBLEncodedImage16ResType_BigEndian;
#endif

// A flattened image can be saved to a resource or file.
struct SFlattenedImage16
{
	UInt16				version;
	TBLImageVector		width;
	TBLImageVector		height;
	TBLImageVector		xoff;
	TBLImageVector		yoff;
};

// EncodeToken/Transparency levels used by masks
typedef			UInt32 		TEncodeToken;
enum
{			
	kTransp100	= CBLMask16::kTransp100,		// not-there
	kTransp93	= CBLMask16::kTransp93,
	kTransp87	= CBLMask16::kTransp87,
	kTransp75	= CBLMask16::kTransp75,
	kTransp50	= CBLMask16::kTransp50,
	kTransp25	= CBLMask16::kTransp25,
	kTransp12	= CBLMask16::kTransp12,
	kTransp6	= CBLMask16::kTransp6,
	kTransp0	= CBLMask16::kTransp0,			// totally opaque
	
	kNotInRun	= 255,							// a null value - never written into encoded data
	kEndImage	= 101,							// end of the image
	kSkipRows	= 102							// end of a line(s)
};

class CBLCanvas;
class CResourceContainer;

#include		"DecodeImage16.h"

class UBLEncodedImage16
{
	public:	
		static TBLFlattenedImage16H	/*e*/	EncodeImage(
												CBLCanvas		&inImage,
												Rect			&inImageRect,
												CBLMask16		*inMask,
												Boolean			inShrinkToContents,
												SInt16			&outXChange,
												SInt16			&outYChange);

		static TBLFlattenedImage16H	/*e*/	LoadImage(
												CResourceContainer	*inResContainer,
												TResId				inResId);

		static void	/*e*/					SaveImage(
												CResourceContainer		*inResContainer,
												TBLFlattenedImage16H	inDataHandle,
												TResId					inResId);

		BL_MAKEDECODERPROTOTYPES16(DecodeImage);
/*
		Above produces:
		DecodeImageClip16
		DecodeImageNoClip16
		DecodeImage
		DecodeImageNoClip
*/
						
		static void							ShrinkToContents(
												CBLCanvas				&inImage,
												Rect				&inImageRect,
												CBLMask16			*inMask,
												SInt16				&outXChange,
												SInt16				&outYChange);

};
