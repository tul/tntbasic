// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapLayers.cpp
// John Treece-Birch
// 22/3/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#include	"CTNTMap.h"
#include	"TNewHandleStream.h"

CMapLayer::CMapLayer(CTNTMap *inTmap,Str32 inName) : link(this)
{
	CopyPStr(inName,name);
	opacity=kMaxOpacity;
	tmap=inTmap;
	tilesData=0L;
	
	ThrowIfMemFull_(tilesData=::NewHandleClear(tmap->GetMapWidth()*tmap->GetMapHeight()*tmap->GetTileDataSize()));
}

CMapLayer::CMapLayer(CTNTMap *inMap,TNewHandleStream &inStream) : link(this)
{
	Size	size;
	SInt32	n,stringLength;
	
	// Load the layer name
	inStream >> stringLength;
	
	name[0]=stringLength;
	
	for (n=0; n<stringLength; n++)
		inStream >> name[n+1];

	name[n+1]=0;
	
	SInt8	visible;
	
	inStream >> visible;
	
	opacity=kMaxOpacity;
	tmap=inMap;
	tilesData=0L;
	
	size=inMap->GetMapWidth()*inMap->GetMapHeight()*inMap->GetTileDataSize();
	
	ThrowIfMemFull_(tilesData=NewHandleClear(size));
	
	if (inMap->GetTileDataSize()==sizeof(char))
	{
		UInt8	tile;
	
		for (UInt16 y=0; y<inMap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<inMap->GetMapWidth(); x++)
			{ 
				inStream >> tile;
				
				SetTile(x,y,tile);
			}
		}
	}
	else
	{
		UInt16	tile;
	
		for (UInt16 y=0; y<inMap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<inMap->GetMapWidth(); x++)
			{ 
				inStream >> tile;
				
				SetTile(x,y,tile);
			}
		}
	}
}

CMapLayer::CMapLayer(CTNTMap *inTmap,CDataStoreStream &inStream) : link(this)
{
	Size	size;

	CopyPStr("\pUntitled",name);
	opacity=kMaxOpacity;
	tmap=inTmap;
	tilesData=0L;
	
	size=tmap->GetMapWidth()*tmap->GetMapHeight()*tmap->GetTileDataSize();
	
	ThrowIfMemFull_(tilesData=NewHandleClear(size));
	
	if (tmap->GetTileDataSize()==sizeof(char))
	{
		UInt8	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{ 
				inStream >> tile;
				
				SetTile(x,y,tile);
			}
		}
	}
	else
	{
		UInt16	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{ 
				inStream >> tile;
				
				SetTile(x,y,tile);
			}
		}
	}
}

CMapLayer::CMapLayer(CTNTMap *inTmap,CMapLayer *copyThis) : link(this)
{
	Size	size;
	Str32	outName;
	
	copyThis->GetName(outName);
	CopyPStr(outName,name);
	
	opacity=copyThis->GetOpacity();
	tmap=inTmap;
	tilesData=0L;
	
	size=tmap->GetMapWidth()*tmap->GetMapHeight()*tmap->GetTileDataSize();
	
	ThrowIfMemFull_(tilesData=NewHandleClear(size));
	
	HLock(tilesData);
	HLock(copyThis->GetTilesData());
	
	BlockMove(*copyThis->GetTilesData(),*tilesData,size);
	
	HUnlock(tilesData);
	HUnlock(copyThis->GetTilesData());
}

CMapLayer::~CMapLayer()
{
	if (tilesData)
		::DisposeHandle(tilesData);
}

#pragma mark -

long CMapLayer::GetTile(long x,long y)
{
	TMapTileIndex	tile=-1;

	if (tmap->GetTileDataSize()==1)
		tile=*((unsigned char*)*tilesData+(y*tmap->GetMapWidth()+x));
	else
		tile=*((unsigned short*)*tilesData+(y*tmap->GetMapWidth()+x));
		
	return tile;
}

void CMapLayer::SetTile(long x,long y,TMapTileIndex tile)
{
	if (tmap->GetTileDataSize()==1)
		*((unsigned char*)*tilesData+(y*tmap->GetMapWidth()+x))=(unsigned char)tile;
	else
		*((unsigned short*)*tilesData+(y*tmap->GetMapWidth()+x))=tile;
}

#pragma mark -

void CMapLayer::LoadInfo(CDataStoreStream &inStream)
{
	inStream >> opacity;
	inStream >> name;
}

void CMapLayer::SaveContent(CNewHandleStream &inStream)
{
	SInt32	n,stringLength;
	
	// Save name
	stringLength=name[0];
	
	inStream << stringLength;
	
	for (n=0; n<stringLength; n++)
		inStream << name[n+1];

	name[n+1]=0;
	
	SInt8	visible=true;
	
	inStream << visible;
	
	// Save tile contents
	if (tmap->GetTileDataSize()==sizeof(char))
	{
		UInt8	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{
				tile=GetTile(x,y);
				
				inStream << tile;
			}
		}
	}
	else
	{
		UInt16	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{
				tile=GetTile(x,y);
				
				inStream << tile;
			}
		}
	}
}

void CMapLayer::SaveContent(CDataStoreStream &inStream)
{
	if (tmap->GetTileDataSize()==sizeof(char))
	{
		UInt8	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{
				tile=GetTile(x,y);
				
				inStream << tile;
			}
		}
	}
	else
	{
		UInt16	tile;
	
		for (UInt16 y=0; y<tmap->GetMapHeight(); y++)
		{
			for (UInt16 x=0; x<tmap->GetMapWidth(); x++)
			{
				tile=GetTile(x,y);
				
				inStream << tile;
			}
		}
	}
}

void CMapLayer::SaveInfo(CDataStoreStream &inStream)
{
	inStream << opacity;
	inStream << name;
}

#pragma mark -

void CMapLayer::Scale(long inNewWidth,long inNewHeight,Size inNewSize,long inLeft,long inTop)
{
	unsigned short 		setx,sety;
	Handle				newTiles=0L;
	
	// Open a new set of tiles
	newTiles=NewHandleClear(inNewWidth*inNewHeight*inNewSize);
	if (!newTiles)
		throw memFullErr;

	HLock(newTiles);

	// Copy the old ones in while scaling
	for (unsigned short y=0; y<tmap->GetMapHeight(); y++)
	{
		for (unsigned short x=0; x<tmap->GetMapWidth(); x++)
		{
			setx=x+inLeft;
			sety=y+inTop;
		
			if ((setx>=0) && (setx<inNewWidth) && (sety>=0) && (sety<inNewHeight))
			{			
				if (inNewSize==sizeof(char))
					*((unsigned char*)*newTiles+(sety*inNewWidth+setx))=(unsigned char)GetTile(x,y);
				else
					*((unsigned short*)*newTiles+(sety*inNewWidth+setx))=GetTile(x,y);
			}
		}
	}
	
	HUnlock(newTiles);
	
	// Get rid of the old one and replace with new one
	DisposeHandle(tilesData);
	tilesData=newTiles;
}