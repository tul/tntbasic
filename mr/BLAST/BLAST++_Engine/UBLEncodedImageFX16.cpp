// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// UBLEncodedImageFX16.cpp
// © Mark Tully and John Treece-Birch 2000
// 21/3/00
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
	Special decoders for decoding 16 bit data with special effects such as overall transparency etc
*/

#include		"UBLEncodedImageFX16.h"
#include		"BLCalcTransp16.h"
#include		"CBlCanvas.h"

BlastColour16			gSpriteColour;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeRedImageNoClip16									Static
//		¥ DecodeRedImageClip16										Static
//		¥ DecodeRedImageClip										Static
//		¥ DecodeRedImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes the image with a global red tint and 50% global transp

// 0111 1100 0000 0000

#define			BL_WRITEPIXEL(outDestPtr,inColour)				*(outDestPtr)=(inColour & 0x7C00)

//#define			BL_TRANSP0CASE			case kTransp6: case kTransp0:
//#define			BL_TRANSP6CASE			case kTransp0:
//#define			BL_TRANSP12CASE			case kTransp0:
//#define			BL_TRANSP25CASE			case kTransp0:
#define			BL_TRANSP50CASE			case kTransp0:
#define			BL_TRANSP75CASE			case kTransp6:
#define			BL_TRANSP87CASE			case kTransp12:
#define			BL_TRANSP93CASE			case kTransp25:
#define			BL_TRANSP100CASE		case kTransp50: case kTransp75: case kTransp87: case kTransp93: case kTransp100:

#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeRedImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeRedImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeRedImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeRedImageNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL

//#undef			BL_TRANSP0CASE
//#undef			BL_TRANSP6CASE
//#undef			BL_TRANSP12CASE
//#undef			BL_TRANSP25CASE
#undef			BL_TRANSP50CASE
#undef			BL_TRANSP75CASE
#undef			BL_TRANSP87CASE
#undef			BL_TRANSP93CASE
#undef			BL_TRANSP100CASE

#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeGreenImageNoClip16									Static
//		¥ DecodeGreenImageClip16									Static
//		¥ DecodeGreenImageClip										Static
//		¥ DecodeGreenImageNoClip									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes the image with a global green tint and 50% global transp

// 0000 0011 1110 0000
#define			BL_WRITEPIXEL(outDestPtr,inColour)				*(outDestPtr)=(inColour & 0x03E0)

//#define			BL_TRANSP0CASE			case kTransp6: case kTransp0:
//#define			BL_TRANSP6CASE			case kTransp0:
//#define			BL_TRANSP12CASE			case kTransp0:
//#define			BL_TRANSP25CASE			case kTransp0:
#define			BL_TRANSP50CASE			case kTransp0:
#define			BL_TRANSP75CASE			case kTransp6:
#define			BL_TRANSP87CASE			case kTransp12:
#define			BL_TRANSP93CASE			case kTransp25:
#define			BL_TRANSP100CASE		case kTransp50: case kTransp75: case kTransp87: case kTransp93: case kTransp100:

#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeGreenImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeGreenImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeGreenImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeGreenImageNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL

//#undef			BL_TRANSP0CASE
//#undef			BL_TRANSP6CASE
//#undef			BL_TRANSP12CASE
//#undef			BL_TRANSP25CASE
#undef			BL_TRANSP50CASE
#undef			BL_TRANSP75CASE
#undef			BL_TRANSP87CASE
#undef			BL_TRANSP93CASE
#undef			BL_TRANSP100CASE

#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

#define			BL_TRANSP0CASE			case kTransp0:
#define			BL_TRANSP6CASE			case kTransp6:
#define			BL_TRANSP12CASE			case kTransp12:
#define			BL_TRANSP25CASE			case kTransp25:
#define			BL_TRANSP50CASE			case kTransp50:
#define			BL_TRANSP75CASE			case kTransp75:
#define			BL_TRANSP87CASE			case kTransp87:
#define			BL_TRANSP93CASE			case kTransp93:
#define			BL_TRANSP100CASE		case kTransp100:

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode6ImageNoClip16										Static
//		¥ Decode6ImageClip16										Static
//		¥ Decode6ImageClip											Static
//		¥ Decode6ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 6% opacity ie nearly not there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode6ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode6ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode6ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode6ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp616(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode12ImageNoClip16										Static
//		¥ Decode12ImageClip16										Static
//		¥ Decode12ImageClip											Static
//		¥ Decode12ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 12% opacity ie nearly not there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode12ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode12ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode12ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode12ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp1216(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode25ImageNoClip16										Static
//		¥ Decode25ImageClip16										Static
//		¥ Decode25ImageClip											Static
//		¥ Decode25ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 25% opacity ie nearly not there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode25ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode25ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode25ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode25ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp2516(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode50ImageNoClip16										Static
//		¥ Decode50ImageClip16										Static
//		¥ Decode50ImageClip											Static
//		¥ Decode50ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 50% opacity
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode50ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode50ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode50ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode50ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp5016(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode75ImageNoClip16										Static
//		¥ Decode75ImageClip16										Static
//		¥ Decode75ImageClip											Static
//		¥ Decode75ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 75% opacity
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode75ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode75ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode75ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode75ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp7516(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode87ImageNoClip16										Static
//		¥ Decode87ImageClip16										Static
//		¥ Decode87ImageClip											Static
//		¥ Decode87ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 87% opacity
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode87ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode87ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode87ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode87ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp8716(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode93ImageNoClip16										Static
//		¥ Decode93ImageClip16										Static
//		¥ Decode93ImageClip											Static
//		¥ Decode93ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 93% opacity ie nearly all there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode93ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode93ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode93ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode93ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp9316(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode63ImageNoClip16										Static
//		¥ Decode63ImageClip16										Static
//		¥ Decode63ImageClip											Static
//		¥ Decode63ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 63% opacity ie nearly all there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode63ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode63ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode63ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode63ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp6316(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode38ImageNoClip16										Static
//		¥ Decode38ImageClip16										Static
//		¥ Decode38ImageClip											Static
//		¥ Decode38ImageNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decodes at 38% opacity ie nearly all there
#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode38ImageClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode38ImageNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode38ImageClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode38ImageNoClip
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=BLCalcTransp3816(*outDestPtr,inColour)

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

#undef			BL_TRANSP0CASE
#undef			BL_TRANSP6CASE
#undef			BL_TRANSP12CASE
#undef			BL_TRANSP25CASE
#undef			BL_TRANSP50CASE
#undef			BL_TRANSP75CASE
#undef			BL_TRANSP87CASE
#undef			BL_TRANSP93CASE
#undef			BL_TRANSP100CASE

// Shadow decoders decode the sprite as a black shadow of the specified percentage. The lower the number the darker the
// shadow, 6 being almost totally black

#define			BL_TRANSP0CASE			case kTransp6: case kTransp12: case kTransp25: case kTransp50: case kTransp75: case kTransp87: case kTransp93: case kTransp0:
#define			BL_TRANSP100CASE		case kTransp100:

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode6ShadowNoClip16										Static
//		¥ Decode6ShadowClip16										Static
//		¥ Decode6ShadowClip											Static
//		¥ Decode6ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol616(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode6ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode6ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode6ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode6ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode12ShadowNoClip16									Static
//		¥ Decode12ShadowClip16										Static
//		¥ Decode12ShadowClip										Static
//		¥ Decode12ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol1216(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode12ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode12ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode12ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode12ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode25ShadowNoClip16									Static
//		¥ Decode25ShadowClip16										Static
//		¥ Decode25ShadowClip										Static
//		¥ Decode25ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol2516(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode25ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode25ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode25ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode25ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode50ShadowNoClip16									Static
//		¥ Decode50ShadowClip16										Static
//		¥ Decode50ShadowClip										Static
//		¥ Decode50ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol5016(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode50ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode50ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode50ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode50ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode75ShadowNoClip16									Static
//		¥ Decode75ShadowClip16										Static
//		¥ Decode75ShadowClip										Static
//		¥ Decode75ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol7516(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode75ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode75ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode75ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode75ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode87ShadowNoClip16									Static
//		¥ Decode87ShadowClip16										Static
//		¥ Decode87ShadowClip										Static
//		¥ Decode87ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol8716(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode87ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode87ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode87ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode87ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Decode93ShadowNoClip16									Static
//		¥ Decode93ShadowClip16										Static
//		¥ Decode93ShadowClip										Static
//		¥ Decode93ShadowNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)		*outDestPtr=BLDarkenCol9316(*outDestPtr)

#define			BL_DECODECLIP16			UBLEncodedImageFX16::Decode93ShadowClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::Decode93ShadowNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::Decode93ShadowClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::Decode93ShadowNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

#undef			BL_TRANSP0CASE
#undef			BL_TRANSP100CASE

// Colour Decoders
#define			BL_TRANSP0CASE			case kTransp0:
#define			BL_TRANSP6CASE			case kTransp6:
#define			BL_TRANSP12CASE			case kTransp12:
#define			BL_TRANSP25CASE			case kTransp25:
#define			BL_TRANSP50CASE			case kTransp50:
#define			BL_TRANSP75CASE			case kTransp75:
#define			BL_TRANSP87CASE			case kTransp87:
#define			BL_TRANSP93CASE			case kTransp93:
#define			BL_TRANSP100CASE		case kTransp100:

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColourNoClip16										Static
//		¥ DecodeColourClip16										Static
//		¥ DecodeColourClip											Static
//		¥ DecodeColourNoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(gSpriteColour)
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColourClip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColourNoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColourClip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColourNoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour93NoClip16									Static
//		¥ DecodeColour93Clip16										Static
//		¥ DecodeColour93Clip										Static
//		¥ DecodeColour93NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp9316((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour93Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour93NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour93Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour93NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour87NoClip16									Static
//		¥ DecodeColour87Clip16										Static
//		¥ DecodeColour87Clip										Static
//		¥ DecodeColour87NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp8716((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour87Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour87NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour87Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour87NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour75NoClip16									Static
//		¥ DecodeColour75Clip16										Static
//		¥ DecodeColour75Clip										Static
//		¥ DecodeColour75NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp7516((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour75Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour75NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour75Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour75NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour63NoClip16									Static
//		¥ DecodeColour63Clip16										Static
//		¥ DecodeColour63Clip										Static
//		¥ DecodeColour63NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp6316((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour63Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour63NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour63Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour63NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour50NoClip16									Static
//		¥ DecodeColour50Clip16										Static
//		¥ DecodeColour50Clip										Static
//		¥ DecodeColour50NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp5016((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour50Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour50NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour50Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour50NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour38NoClip16									Static
//		¥ DecodeColour38Clip16										Static
//		¥ DecodeColour38Clip										Static
//		¥ DecodeColour38NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp3816((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour38Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour38NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour38Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour38NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour25NoClip16									Static
//		¥ DecodeColour25Clip16										Static
//		¥ DecodeColour25Clip										Static
//		¥ DecodeColour25NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp2516((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour25Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour25NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour25Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour25NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour12NoClip16									Static
//		¥ DecodeColour12Clip16										Static
//		¥ DecodeColour12Clip										Static
//		¥ DecodeColour12NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp1216((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour12Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour12NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour12Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour12NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DecodeColour6NoClip16										Static
//		¥ DecodeColour6Clip16										Static
//		¥ DecodeColour6Clip											Static
//		¥ DecodeColour6NoClip										Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(BLCalcTransp616((*outDestPtr),gSpriteColour))
#define			BL_DECODECLIP16			UBLEncodedImageFX16::DecodeColour6Clip16
#define			BL_DECODENOCLIP16		UBLEncodedImageFX16::DecodeColour6NoClip16
#define			BL_DECODECLIP			UBLEncodedImageFX16::DecodeColour6Clip
#define			BL_DECODENOCLIP			UBLEncodedImageFX16::DecodeColour6NoClip

#include		"DecodeImage16.t"

#undef			BL_WRITEPIXEL
#undef			BL_DECODECLIP16
#undef			BL_DECODENOCLIP16
#undef			BL_DECODECLIP
#undef			BL_DECODENOCLIP

#undef			BL_TRANSP0CASE
#undef			BL_TRANSP6CASE
#undef			BL_TRANSP12CASE
#undef			BL_TRANSP25CASE
#undef			BL_TRANSP50CASE
#undef			BL_TRANSP75CASE
#undef			BL_TRANSP87CASE
#undef			BL_TRANSP93CASE
#undef			BL_TRANSP100CASE