/***************************************************************************************************

BLAST Layer Manager.cpp
Extension of BLAST++ª engine
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

#include	"BLAST Layer Manager.h"
#include	"BLAST Sprite Layer.h"
#include	"Useful Defines.h"

BlastLayerManager::BlastLayerManager()
{
	layersList=0L;
	maxLayers=usedLayers=0;
	drawBuffer=backupBuffer=0L;
	trackSprites=true;
	invalSprites=true;
}

BlastLayerManager::~BlastLayerManager()
{
	if (layersList)
		delete [] layersList;
}

void BlastLayerManager::SetDrawBuffer(BlastDrawBufferPtr drawBuffer)
{
	this->drawBuffer=drawBuffer;
}

BlastDrawBufferPtr BlastLayerManager::GetDrawBuffer()
{
	return drawBuffer;
}

void BlastLayerManager::SetBackupBuffer(BlastDrawBufferPtr backupBuffer)
{
	this->backupBuffer=backupBuffer;
}

BlastDrawBufferPtr BlastLayerManager::GetBackupBuffer()
{
	return backupBuffer;
}

void BlastLayerManager::SetSpriteTracking(Boolean on)
{
	trackSprites=on;
}

void BlastLayerManager::SetSpriteInvalling(Boolean on)
{
	invalSprites=on;
}

Boolean BlastLayerManager::GetSpriteTracking()
{
	return trackSprites;
}

Boolean BlastLayerManager::GetSpriteInvalling()
{
	return invalSprites;
}

BlastLayerIndex BlastLayerManager::GetMaxLayers()
{
	return maxLayers;
}

BlastLayerIndex BlastLayerManager::GetUsedLayers()
{
	return usedLayers;
}

Boolean BlastLayerManager::SetMaxLayers(BlastLayerIndex newMaxLayers)
{
	BlastSpriteLayerPtr		*temp;
	
	temp=new BlastSpriteLayerPtr[newMaxLayers];
	if (temp)
	{
		BlastLayerIndex 	counter=0;
		BlastLayerIndex		max=Lesser(maxLayers,newMaxLayers);
		
		// clear the list
		usedLayers=0;
		maxLayers=newMaxLayers;
		for (counter=0; counter<maxLayers; counter++)
			temp[counter]=0L;
		
		// now copy the old layers into the new list
		if (layersList)
		{
			for (; counter<max; counter++)
			{
				if (temp[counter]=layersList[counter])
					usedLayers++;
			}
			
			if (layersList)
				delete [] layersList;
		}
		layersList=temp;
		return true;
	}
	else
		return false;
}

BlastLayerIndex BlastLayerManager::GetNextFreeLayer()
{
	for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
	{
		if (!layersList[counter])
			return counter;
	}
	
	return -1;
}

BlastLayerIndex BlastLayerManager::AddLayer(BlastSpriteLayerPtr layer,Boolean setLayerClipRect)
{
	BlastLayerIndex		index=GetNextFreeLayer();
	
	if (setLayerClipRect)
	{
		Rect		tempRect;
		
		if (drawBuffer)
		{
			drawBuffer->GetBoundsRect(tempRect);
			layer->SetLayerClipRect(tempRect);
		}
	}
	
	if (index!=-1)
		SetLayer(index,layer);
	
	return index;
}

BlastSpriteLayerPtr BlastLayerManager::GetLayer(BlastLayerIndex index)
{
	return layersList[index];
}

void BlastLayerManager::SetLayer(BlastLayerIndex index,BlastSpriteLayerPtr layer)
{
	if (layersList[index] && !layer)
		usedLayers--;
	else if (!layersList[index] && layer)
		usedLayers++;
	layersList[index]=layer;
}

BlastLayerIndex BlastLayerManager::GetLayerIndex(BlastSpriteLayerPtr layer)
{
	BlastLayerIndex		counter;
	
	for (counter=0; counter<maxLayers; counter++)
	{
		if (layer==layersList[counter])
			return counter;
	}
	
	return -1;
}

void BlastLayerManager::RemoveLayer(BlastSpriteLayerPtr layer)
{
	BlastLayerIndex		temp=GetLayerIndex(layer);
	
	if (temp!=-1)
		SetLayer(temp,0L);
}

void BlastLayerManager::SwapLayers(BlastSpriteLayerPtr layerA,BlastSpriteLayerPtr layerB)
{
	BlastLayerIndex		index1,index2;
	
	index1=GetLayerIndex(layerA);
	if (index1==-1)
		return;
	index2=GetLayerIndex(layerB);
	if (index2==-1)
		return;
	SwapLayers(index1,index2);
}
		
void BlastLayerManager::SwapLayers(BlastLayerIndex layer1,BlastLayerIndex layer2)
{
	BlastSpriteLayerPtr	temp;

	SwapValues(layersList[layer1],layersList[layer2],temp);
}

void BlastLayerManager::MoanAllLayers()
{
	for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
	{
		if (layersList[counter])
			layersList[counter]->MoanAllSprites();
	}
}

bool BlastLayerManager::WillRender()
{
	for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
	{
		if (layersList[counter] && layersList[counter]->WillRender())
			return true;
	}

	return false;
}

void BlastLayerManager::CalcAllLayers()
{
	if (trackSprites)
	{
		for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
		{
			if (layersList[counter])
				layersList[counter]->CalcAllSprites(backupBuffer);
		}
	}
	else
	{
		for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
		{
			if (layersList[counter])
				layersList[counter]->CalcAllSprites(0L);
		}
	}
}

void BlastLayerManager::RenderAllLayers()
{
	// inval the sprites if we should and if we haven't already calced the layers visability
	Boolean		doIt=(invalSprites && (!trackSprites));

	for (BlastLayerIndex counter=0; counter<maxLayers; counter++)
	{
		if (layersList[counter])
			layersList[counter]->RenderAllSprites(drawBuffer,doIt);
	}
	
	if (invalSprites && trackSprites && backupBuffer)
	{
		void		*backupBlitList;
		
		// merge the rects list from the backup buffer into the draw buffer
		backupBlitList=backupBuffer->LLGetBlitList();
		drawBuffer->LLMergeBlitList(backupBlitList);
	}
}

void BlastLayerManager::StashBackdrop()
{
	if (!backupBuffer)
		return;
	backupBuffer->BlitAllRectsDS();
}

// draw buffer's blit list should be empty when this is called
void BlastLayerManager::RestoreBackdrop()
{
	if (!backupBuffer)
		return;

	backupBuffer->BlitAllRectsSD();
	if (invalSprites)
	{
		void		*drawList,*backupList;
	
		// perform a low level swap of the blit lists
		drawBuffer->ClearBlitList();		
		drawList=drawBuffer->LLGetBlitList();
		backupList=backupBuffer->LLGetBlitList();
		drawBuffer->LLSetBlitList(backupList);
		backupBuffer->LLSetBlitList(drawList);
	}
	else
		backupBuffer->ClearBlitList();
}

void BlastLayerManager::ForgetBackdrop()
{
	if (!backupBuffer)
		return;
	backupBuffer->ClearBlitList();
}

void BlastLayerManager::SetRefCon(void *refCon)
{
	this->refCon=refCon;
}

void *BlastLayerManager::GetRefCon()
{
	return refCon;
}
