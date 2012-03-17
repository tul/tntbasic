// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CMapPolygons.cpp
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

#include	"CMapPolygons.h"

CMapPolygon::CMapPolygon() : link(this)
{
	RGBColour	newColour={0,0,0};
	
	components=0;
	colour=newColour;
	LString::CopyPStr("\pUntitled",name);
}

CMapPolygon::CMapPolygon(Str32 inName,RGBColour inColour) : link(this)
{
	components=0;
	colour=inColour;
	LString::CopyPStr(inName,name);
}

CMapPolygon::CMapPolygon(CDataStoreStream &inStream) : link(this)
{
	CMapPolygonComponent		*component;
	RGBColour				newColour={0,0,0};

	colour=newColour;
	LString::CopyPStr("\pUntitled",name);
	
	inStream >> components;
	
	for (unsigned short n=0; n<components; n++)
	{
		component=new CMapPolygonComponent(inStream);
	
		componentsList.AppendElement(component->GetLink());
	}
}

CMapPolygon::CMapPolygon(CMapPolygon *copyThis) : link(this)
{
	CMapPolygonComponent		*component,*copyComp;

	colour=copyThis->GetColour();
	copyThis->GetName(name);
	components=copyThis->GetComponents();
	
	for (unsigned short n=0; n<components; n++)
	{
		copyComp=copyThis->GetComponent(n);
	
		component=new CMapPolygonComponent();
		
		component->SetArea(copyComp->GetArea());
		component->SetLayer(copyComp->GetLayer());
		
		componentsList.AppendElement(component->GetLink());
	}
}

CMapPolygon::~CMapPolygon()
{
	TListIndexer	componentIndexer(&componentsList);
	while (CMapPolygonComponent *theComp=(CMapPolygonComponent*)componentIndexer.GetNextData())
		delete theComp;
}

#pragma mark -
		
void CMapPolygon::SaveContent(CDataStoreStream &inStream)
{
	inStream << components;
	
	TListIndexer	indexer(&componentsList);
	while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
		component->SaveContent(inStream);
}

void CMapPolygon::LoadInfo(CDataStoreStream &inStream)
{
	inStream >> colour.red >> colour.green >> colour.blue;
	inStream >> name;
}

void CMapPolygon::SaveInfo(CDataStoreStream &inStream)
{
	inStream << colour.red << colour.green << colour.blue;
	inStream << name;
}

#pragma mark -

void CMapPolygon::Scale(TMapTileIndex inWidth,TMapTileIndex inHeight,TMapTileIndex inLeft,TMapTileIndex inTop)
{
	TListIndexer		indexer(&componentsList);
	
	while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
		component->Scale(inWidth,inHeight,inTop,inLeft);
}

void CMapPolygon::MoveLayer(SInt32 inMoveThis,SInt32 inToHere)
{
	TListIndexer		indexer(&componentsList);

	if (inMoveThis<inToHere)
	{
		// Move layer upwards
		while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
		{
			if (component->GetLayer()==inMoveThis)
				component->SetLayer(inToHere);
			else if ((component->GetLayer()>inMoveThis) && (component->GetLayer()<=inToHere))
				component->SetLayer(component->GetLayer()-1);
		}
	}
	else
	{
		// Move layer downwards
		while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
		{
			if (component->GetLayer()==inMoveThis)
				component->SetLayer(inToHere+1);
			else if ((component->GetLayer()>inToHere) && (component->GetLayer()<inMoveThis))
				component->SetLayer(component->GetLayer()+1);
		}
	}
}

void CMapPolygon::DeleteLayer(TMapLayerIndex inLayer)
{
	TListIndexer		indexer(&componentsList);
	
	while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
	{
		if (component->GetLayer()==inLayer)
			delete component;
		else if (component->GetLayer()>inLayer)
			component->SetLayer(component->GetLayer()-1);
	}
}

bool CMapPolygon::In(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z)
{
	for (TMapPolygonComponentIndex c=0; c<components; c++)
	{
		if (GetComponent(c)->In(x,y,z))
			return true;
	}

	return false;
}

void CMapPolygon::AddComponent(Rect &rect,TMapLayerIndex layer)
{
	CMapPolygonComponent	*component=new CMapPolygonComponent(rect,layer);

	componentsList.AppendElement(component->GetLink());
	
	components++;
}

void CMapPolygon::DeleteComponent()
{
	delete GetComponent(components-1);
	
	components--;
}

void CMapPolygon::RemoveComponent(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z)
{
	CMapPolygonComponent	*component;

	for (TMapPolygonComponentIndex c=0; c<GetComponents(); c++)
	{
		component=GetComponent(c);
		
		if (component->In(x,y,z))
		{
			delete component;
			components--;
		}
	}
}

#pragma mark -

CMapPolygonComponent::CMapPolygonComponent() : link(this)
{
	Rect	newRect={0,0,0,0};

	area=newRect;
	layer=0;
}

CMapPolygonComponent::CMapPolygonComponent(Rect &inRect,TMapLayerIndex inLayer) : link(this)
{
	area=inRect;
	layer=inLayer;
}

CMapPolygonComponent::CMapPolygonComponent(CDataStoreStream &inStream) : link(this)
{
	inStream >> area;
	inStream >> layer;
}

bool CMapPolygonComponent::In(TMapTileIndex x,TMapTileIndex y,TMapLayerIndex z)
{
	if ((x<=area.right) && (x>=area.left) && (y<=area.bottom) && (y>=area.top) && (z==layer))
		return true;
		
	return false;
}

void CMapPolygonComponent::SaveContent(CDataStoreStream &inStream)
{
	inStream << area;
	inStream << layer;
}

void CMapPolygonComponent::Scale(TMapTileIndex inWidth,TMapTileIndex inHeight,TMapTileIndex inLeft,TMapTileIndex inTop)
{
	area.left+=inLeft;
	area.right+=inLeft;
	area.top+=inTop;
	area.bottom+=inTop;
	
	// Clip the component
	if (area.left<0)
		area.left=0;
	
	if (area.top<0)
		area.top=0;
	
	if (area.right>inWidth)
		area.right=inWidth;
		
	if (area.bottom>inHeight)
		area.bottom=inHeight;
}

// returns the maximum bounds of a polygon
void CMapPolygon::GetPolygonMaximumBounds(
	TMapTileIndex		&outTop,
	TMapTileIndex		&outLeft,
	TMapTileIndex		&outBottom,
	TMapTileIndex		&outRight)
{
	TListIndexer		indexer(&componentsList);
	bool				firstPass=true;
	Rect				a;
	
	while (CMapPolygonComponent *component=(CMapPolygonComponent*)indexer.GetNextData())
	{
		a=component->GetArea();

		if (firstPass)
		{
			outTop=a.top;
			outLeft=a.left;
			outBottom=a.bottom;
			outRight=a.right;
		}
		else
		{
			outLeft=Lesser(a.left,outLeft);
			outTop=Lesser(a.top,outTop);
			outRight=Greater(a.right,outRight);
			outBottom=Greater(a.bottom,outBottom);
		}
			
		firstPass=false;
	}	
}
