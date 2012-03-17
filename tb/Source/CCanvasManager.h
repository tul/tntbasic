// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CCanvasManager.h
// © Mark Tully 2000
// 13/4/00
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
#include		"TNTBasic_Types.h"
#include		"CDynamicArrayT.h"

class			CCanvas;
class			CGraphicsContext;

class CCanvasManager
{
	protected:
		CDynamicArrayT<CCanvas>			mCanvases;
		CCanvas							*mMainCanvas;
		CCanvas							*mTargettedCanvas;
		TTBInteger						mTargettedIndex;
		bool							mDrawTrackingOn;
		CGraphicsContext				*mGraphicsContext;
	
		void /*e*/						CheckCanvasIndex(
											TTBInteger	inIndex);
	
	public:
										CCanvasManager(
											CGraphicsContext	*inContext) :
											mMainCanvas(0),
											mDrawTrackingOn(true),
											mTargettedIndex(0),
											mGraphicsContext(inContext)
										{
										}
		virtual							~CCanvasManager();

		GDHandle						GetCanvasDevice();
		
		// Set up only - specify canvas 0, the main canvas, this is then adopted by the manager
		void							SetMainCanvas(
											CCanvas		*inCanvas)		{ mTargettedCanvas=mMainCanvas=inCanvas; mTargettedIndex=0; }
		
		void /*e*/						OpenCanvas(
											TTBInteger	inIndex,
											SInt16		inWidth,
											SInt16		inHeight);
		void							CloseCanvas(
											TTBInteger	inIndex);
		void							CloseAllCanvases();

		void							SetDrawTracking(
											bool		inState);

		bool							IsCanvasOpen(
											TTBInteger	inIndex)		{ return !mCanvases.IsNull(inIndex); }
		CCanvas	/*e*/					*GetCanvas(
											TTBInteger	inIndex);
		TTBInteger						GetTargettedCanvasIndex()		{ return mTargettedIndex; }
		CCanvas							*GetTargettedCanvas()			{ return mTargettedCanvas; }
		void /*e*/						TargetCanvas(
											TTBInteger	inIndex);
};
