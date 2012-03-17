// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// BLAST++
// DecodeImage16.t
// © Mark Tully 2000
// 22/2/00
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
		This is a template file which allows a decoders to be generated for a 16 bit encoded image. You will have
		to make sure you include the necessary includes before including this file.
		Set the macros up before including this file and it's probably a good idea to undef them afterwards.

		// This macro define how a pixel is written to the destination. The pixel colour should be written
		// to destPtr. If you need to operate on the values then you may copy them into variables
		// temp1 and temp2 which are two variables reserved for that purpose.
		// By altering this from it's default setting you can apply global tints to the colour being written.

		// This example is the standard version and it simply outputs the code to ptr
		#define			BL_WRITEPIXEL(outDestPtr,inColour)	*(outDestPtr)=(inColour)

		// These macros allow the code which is called for a given token to be altered. Note that when you redefine these
		// variables you MUST cover all the cases from kTransp0 to kTransp100 otherwise the decoder will not work for all
		// images. Particularly if it reads a token which is undefined in your case handlers. If you want a token to do
		// nothing put it in the BL_TRANSP100CASE.
		
		// For instances for a global transparency shift towards more transparency you would do:
//		#define			BL_TRANSP0CASE			Rem out so that no code is generated for the totally opaque case
		#define			BL_TRANSP6CASE			case kTransp0:
		#define			BL_TRANSP12CASE			case kTransp6:
		#define			BL_TRANSP25CASE			case kTransp12:
		#define			BL_TRANSP50CASE			case kTransp25:
		#define			BL_TRANSP75CASE			case kTransp50:
		#define			BL_TRANSP87CASE			case kTransp75:
		#define			BL_TRANSP93CASE			case kTransp87:
		#define			BL_TRANSP100CASE		case kTransp100: case kTransp93:

		// For a normal decode use:		
		#define			BL_TRANSP0CASE			case kTransp0:
		#define			BL_TRANSP6CASE			case kTransp6:
		#define			BL_TRANSP12CASE			case kTransp12:
		#define			BL_TRANSP25CASE			case kTransp25:
		#define			BL_TRANSP50CASE			case kTransp50:
		#define			BL_TRANSP75CASE			case kTransp75:
		#define			BL_TRANSP87CASE			case kTransp87:
		#define			BL_TRANSP93CASE			case kTransp93:
		#define			BL_TRANSP100CASE		case kTransp100:
		
		// Naming the decoders
		// This file generates 4 decoders. These are named
		// It generates
		//		BL_DECODECLIP16					// 16-bit clipper - takes a 16-bit abc rec
		//		BL_DECODERNOCLIP16				// 16-bit no clipper
		//		BL_DECODERCLIP					// canvas based depth independant clipper
		//		BL_DECODERNOCLIP				// canvas based depth independant no clipper
		
		// So define a name for each of them and then include DecodeImage16.h
		
		// Prototypes can be produced by including DecodeImage16.h and using the BL_MAKEDECODERPROTOTYPES16()
*/

// Depth independant decoder. Creates decoder for a canvas which clips to the rect passed
void BL_DECODECLIP(
	SFlattenedImage16	*inData,
	CBLCanvas				&outCanvas,
	TBLImageVector		inX,
	TBLImageVector		inY,
	const Rect			&inClipRect)
{
	if (outCanvas.GetDepth()==16)
		BL_DECODECLIP16 (inData,&outCanvas.GetABCRec(),inX,inY,inClipRect);
	else
	{
		// Generic decoder would go here
	}
}

// Clips to the canvas only - accelerated for 16 bit
void BL_DECODENOCLIP(
	SFlattenedImage16	*inData,
	CBLCanvas				&outCanvas,
	TBLImageVector		inX,
	TBLImageVector		inY)
{
	if (outCanvas.GetDepth()==16)
		BL_DECODENOCLIP16 (inData,&outCanvas.GetABCRec(),inX,inY);
	else
	{
		// Generic decoder would go here
	}
}

// Assumes the ABCRec is a 16bit, assumes the decoded image will fit on said rec.
void BL_DECODENOCLIP16(
	SFlattenedImage16	*inData,
	ABCPtr				inRec,
	TBLImageVector		inX,
	TBLImageVector		inY)
{
	if (inData->version!=0)
		return;

	UInt32			opCode,opCount;
	TBLColour16		*dataPtr=(TBLColour16*)(((unsigned char*)inData)+sizeof(SFlattenedImage16));
	TBLColour16		temp1,temp2;

	unsigned char	*destPtr,*destRowStart;
	unsigned long	srcRB,destRB;
	
	destRB=inRec->rec.rowBytes;
	destPtr=destRowStart=inRec->rec.baseAddr+destRB*(inY-inData->yoff)+(inX-inData->xoff)*sizeof(short);
	
	do
	{
		opCount=opCode=*(UInt32*)dataPtr;
		dataPtr=(TBLColour16*)(((unsigned char*)dataPtr)+sizeof(UInt32));
		opCount&=0x00FFFFFF;
		opCode>>=24;
						
		// switch on opCode, try most likely first
		switch (opCode)
		{
			// 100% opaque
#ifdef BL_TRANSP0CASE
			BL_TRANSP0CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						BL_WRITEPIXEL((short*)destPtr,(*dataPtr));
						
						((short*)destPtr)++;
						dataPtr++;
					}
				}
				break;
#endif

			// totally transparent, if another token is mapped to the transparent option then the data ptr will need to be
			// altered as well, hence the dataPtr+=opCount statement which is guarded by a run time test which will
			// hopefully be optimised away when the macros are expanded
#ifdef BL_TRANSP100CASE
			BL_TRANSP100CASE
				if (opCode!=kTransp100)
					(short*)dataPtr+=opCount;
					
				(short*)destPtr+=opCount;
				break;
#endif

			case kSkipRows:
				destRowStart+=destRB*opCount;
				destPtr=destRowStart;
				break;

#ifdef BL_TRANSP93CASE
			BL_TRANSP93CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp9316(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifdef BL_TRANSP87CASE
			BL_TRANSP87CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp8716(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifdef BL_TRANSP75CASE
			BL_TRANSP75CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp7516(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifndef BL_BLEND50
	#define BL_BLEND50 BLCalcTransp5016
#endif

#ifdef BL_TRANSP50CASE
			BL_TRANSP50CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BL_BLEND50(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifdef BL_TRANSP25CASE
			BL_TRANSP25CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp2516(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifdef BL_TRANSP12CASE
			BL_TRANSP12CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp1216(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

#ifdef BL_TRANSP6CASE
			BL_TRANSP6CASE
				if (opCode==kTransp100)
					((short*)destPtr)+=opCount;
				else
				{
					opCount++;
					while (--opCount)
					{
						temp1=*dataPtr++;
						temp2=*((short*)destPtr);
						
						BL_WRITEPIXEL((short*)destPtr,BLCalcTransp616(temp1,temp2));
						((short*)destPtr)++;
					}
				}
				break;
#endif

			case kEndImage:
				return;
				break;
				
			default:
				return;			// unrecognised token
				break;
		}
	} while (true);
}

// Assumes the inRec is 16 and clips to inClipRect which is assumed to fit on said rec
void BL_DECODECLIP16(
	SFlattenedImage16	*inData,
	ABCPtr				inRec,
	TBLImageVector		inX,
	TBLImageVector		inY,
	const Rect			&inClipRect)
{
	if (inData->version!=0)
		return;

	TBLImageVector	xStart=inX-inData->xoff,y=inY-inData->yoff;
	TBLImageVector	x=xStart;
	SInt32			opCode,opCount;
	TBLColour16		*dataPtr=(TBLColour16*)(((unsigned char*)inData)+sizeof(SFlattenedImage16)),temp1,temp2;
	unsigned char	*destPtr,*destRowStart;
	long			rightClip=0;
	unsigned long	destRB=inRec->rec.rowBytes;
	bool			decode;
	
	destPtr=destRowStart=inRec->rec.baseAddr+destRB*y+x*sizeof(short);
	
	do
	{
		dataPtr+=rightClip;
		x+=rightClip;
	
		opCount=opCode=*(UInt32*)dataPtr;
		dataPtr=(TBLColour16*)(((unsigned char*)dataPtr)+sizeof(UInt32));
		opCount&=0x00FFFFFF;
		opCode>>=24;
		
		// if it is off the bottom then we don't need to decode any more
		if (y>=inClipRect.bottom)
			return;
		
		rightClip=0;
		decode=false;
		
		// only decode if we are on the screen or skipping rows	
		if (y>=inClipRect.top)
		{
			decode=true;
		
			if ((opCode!=kSkipRows) && (opCode!=kEndImage) && (opCode!=kTransp100))
			{
				if ((x>inClipRect.right) || ((x+opCount)<inClipRect.left))
				{
					decode=false;
					x+=opCount;
					dataPtr+=opCount;
					(short*)destPtr+=opCount;
				}
				else
				{
					if ((x+opCount)>=inClipRect.right)
					{
						rightClip=opCount-(inClipRect.right-x);
						opCount-=rightClip;
					}
					
					if (x<inClipRect.left)
					{
						(short*)destPtr+=(inClipRect.left-x);
						dataPtr+=(inClipRect.left-x);
						opCount-=(inClipRect.left-x);
						x=inClipRect.left;
					}
				}
			}
		
			if (decode)
			{
				// switch on opCode, try most likely first
				switch (opCode)
				{
#ifdef BL_TRANSP0CASE
					BL_TRANSP0CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{	
								BL_WRITEPIXEL((short*)destPtr,(*dataPtr));
						
								((short*)destPtr)++;
								dataPtr++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP100CASE
					BL_TRANSP100CASE
						if (opCode!=kTransp100)
							((short*)dataPtr)+=opCount;
						(short*)destPtr+=opCount;
						x+=opCount;
						break;
#endif

#ifdef BL_TRANSP93CASE
					BL_TRANSP93CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp9316(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP87CASE
					BL_TRANSP87CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp8716(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP75CASE
					BL_TRANSP75CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp7516(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP50CASE
					BL_TRANSP50CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp5016(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP25CASE
					BL_TRANSP25CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp2516(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP12CASE
					BL_TRANSP12CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp1216(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

#ifdef BL_TRANSP6CASE
					BL_TRANSP6CASE
						if (opCode==kTransp100)
						{
							((short*)destPtr)+=opCount;
							x+=opCount;
						}
						else
						{
							x+=opCount;
							opCount++;
							while (--opCount)
							{
								temp1=*dataPtr++;
								temp2=*((short*)destPtr);
								
								BL_WRITEPIXEL((short*)destPtr,BLCalcTransp616(temp1,temp2));
								((short*)destPtr)++;
							}
						}
						break;
#endif

					case kSkipRows:
						x=xStart;
						y+=opCount;					
						destRowStart+=destRB*opCount;
						destPtr=destRowStart;
						break;

					case kEndImage:
						return;
						break;
						
					default:
						return;			// unrecognised token
						break;
				}
			}
		}
		else
		{
			switch (opCode)
			{				
				case kSkipRows:
					x=xStart;
					y+=opCount;					
					destRowStart+=destRB*opCount;
					destPtr=destRowStart;
					break;
					
				case kEndImage:
					return;
					break;
			
				default:
					if (opCode!=kTransp100)
						(short*)dataPtr+=opCount;
					break;
			}
		}
		
	} while (true);
}