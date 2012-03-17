/***************************************************************************************************

BLAST Layer Manager.h
Header for the BLAST++ª engine
© TNT Software 1997
All right reserved
Mark Tully

2/9/97		:	Started it

***************************************************************************************************/

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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
#include		"BLAST Sprite Layer.h"
#include		"BLAST Draw Buffer Base.h"

typedef short BlastLayerIndex;

typedef class BlastLayerManager
{
	private:
		BlastSpriteLayerPtr			*layersList;
		BlastLayerIndex				maxLayers,usedLayers;
		BlastDrawBufferPtr			drawBuffer,backupBuffer;
		Boolean						trackSprites,invalSprites;
		void						*refCon;

	protected:
		// ********* RefCon access *********
		void						SetRefCon(void *refCon);
		void						*GetRefCon();

	public:
		// ********* Constructor/Destructor *********
									BlastLayerManager();
		virtual						~BlastLayerManager();
	
		// ********* Buffer management *********
		virtual void				SetDrawBuffer(BlastDrawBufferPtr drawBuffer);
		virtual BlastDrawBufferPtr	GetDrawBuffer();
		virtual void				SetBackupBuffer(BlastDrawBufferPtr backupBuffer);
		virtual BlastDrawBufferPtr	GetBackupBuffer();

		// ********* Layer manager operations *********
		virtual void				SetSpriteTracking(Boolean on);
		virtual void				SetSpriteInvalling(Boolean on);
		virtual Boolean				GetSpriteTracking();
		virtual Boolean				GetSpriteInvalling();
		virtual BlastLayerIndex		GetMaxLayers();
		virtual BlastLayerIndex		GetUsedLayers();
		virtual Boolean				SetMaxLayers(BlastLayerIndex maxLayers);
		virtual BlastLayerIndex		GetNextFreeLayer();

		// ********* Layer management *********
		virtual BlastLayerIndex		AddLayer(BlastSpriteLayerPtr layer,Boolean setLayerClipRect=true);
		virtual BlastSpriteLayerPtr	GetLayer(BlastLayerIndex index);
		virtual void				SetLayer(BlastLayerIndex index,BlastSpriteLayerPtr layer);
		virtual BlastLayerIndex		GetLayerIndex(BlastSpriteLayerPtr layer);
		virtual void				RemoveLayer(BlastSpriteLayerPtr layer);
	
		// ********* Layer ordering *********
		virtual void				SwapLayers(BlastLayerIndex layer1,BlastLayerIndex layer2);			
		virtual void				SwapLayers(BlastSpriteLayerPtr layerA,BlastSpriteLayerPtr layer2);
	
		// ********* Layer operations *********
		virtual void				MoanAllLayers();
		virtual void				CalcAllLayers();
		virtual void				RenderAllLayers();
		virtual bool				WillRender();
		
		// ********* Backdrop operations *********
		virtual void				StashBackdrop();
		virtual void				RestoreBackdrop();
		virtual void				ForgetBackdrop();
		
} BlastLayerManager, *BlastLayerManagerPtr;