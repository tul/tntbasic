// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Graph Library
// CGraphView.cpp
// © Mark Tully 2000
// 25/2/00
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
	A powerplant view which renders a graph
*/

#include		"CGraphView.h"
#include		"StdGraph.h"
#include		<UTextTraits.h>
#include		<LStream.h>
#include		<UDrawingUtils.h>
#include		"Useful Defines.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CGraphView::CGraphView(
	LStream	*inStream) :
	LView(inStream),
	mLaidOut(false)
{
/*	mNodeLightTone.red=mNodeLightTone.blue=mNodeLightTone.green=0xFFFF;
	mNodeDarkTone.red=mNodeDarkTone.green=mNodeDarkTone.blue=0;
	mTextTraits=0;
	mLabelColour=mNodeDarkTone;
	mEdgeColour=mNodeDarkTone;
	mGridXSpace=mGridYSpace=80;
	mNodeDiameter=32; */
	
	#define ReadRGB(x) x.red >> x.green >> x.blue
	
	(*inStream) >> ReadRGB(mNodeLightTone) >> ReadRGB(mNodeDarkTone) >> ReadRGB(mEdgeColour) >> ReadRGB(mLabelColour);
	(*inStream) >> mTextTraits >> mGridXSpace >> mGridYSpace >> mNodeDiameter;
	
	mFrameSize=mNodeDiameter;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LayoutGraph												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Applies a tail patch to resize the image size
void CGraphView::LayoutGraph()
{
	inheritedLayout::LayoutGraph();

	mLaidOut=true;
	
	ResizeImageTo(mFrameSize*2+GetMaxWidth()*mGridXSpace,mFrameSize*2+GetMaxHeight()*mGridYSpace,false);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawNode
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws the node, assumes view is focussed with colours set up
// Node is labelled
void CGraphView::DrawNode(
	CStdGraphNode		&inNode,
	SNodePosition		&inPosition)
{
	const LString				&str=inNode.GetName();
	ConstStringPtr				str2=(ConstStringPtr)str;
	Rect						nodeRect,textBox;

	// Draw node index at nodePos.x, nodePos.y
	GetNodeRect(inPosition,nodeRect);
	GetNodeLabelRect(inPosition,textBox);
	
	::RGBForeColour(&mNodeDarkTone);
	::FrameOval(&nodeRect);
	FInsetRect(nodeRect,2,2);
	::RGBForeColour(&mNodeLightTone);
	::PaintOval(&nodeRect);

	::RGBForeColour(&mLabelColour);
	::TruncString(textBox.right-textBox.left,(StringPtr)str2,truncMiddle);
	::TETextBox(&str2[1],str2[0],&textBox,teCenter);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphView::DrawSelf()
{
	if (GetGraph())
	{
		if (!mLaidOut)
			LayoutGraph();
				
		Rect		frame;
		
		if (CalcLocalFrameRect(frame))
		{
			TArray<SNodePosition>			&array=GetLayout();
			TArrayIterator<SNodePosition>	iter(array);
			SNodePosition					nodePos;
			CGraph::TNodeIndex				index=0;
			Rect							nodeRect,textBox;

			::PenNormal();
			::PenSize(2,2);
			UTextTraits::SetPortTextTraits(mTextTraits);
			
			while (iter.Next(nodePos))
				DrawNode(*GetGraph()->GetNthNode(index++)->GetData(),nodePos);							
			
			// Draw the edges
			CGraph::TEdgeIndex			maxEdges=GetGraph()->CountEdges();
			bool						fsm=false;		// finite state machine settings, causes edges to have arrows, curves & labels

			{
				SGraphLayoutHint	hint;	
				CGraphLayoutHinter	*hinter=dynamic_cast<CGraphLayoutHinter*>(mGraph);
				
				if (hinter)
				{
					hinter->GetHint(hint);
				
					fsm=hint.type==SGraphLayoutHint::kFSM;
				}
			}			

			::RGBForeColour(&mEdgeColour);
			
			for (CGraph::TEdgeIndex edge=0; edge<maxEdges; edge++)
			{
				CStdGraphEdge			*stdEdge=GetGraph()->GetNthEdge(edge)->GetData();
				SNodePosition			a=array[stdEdge->GetNodeA()->GetNodeIndex()+1],b=array[stdEdge->GetNodeB()->GetNodeIndex()+1];

				if (fsm)
					DrawDirectedEdge(a,b,*stdEdge);
				else
					DrawUndirectedEdge(a,b,*stdEdge);				
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawDirectedEdge
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Draws a labelled edge with an arrow head.
// Makes an effort to curve the edges
void CGraphView::DrawDirectedEdge(
	SNodePosition	inA,
	SNodePosition	inB,
	CStdGraphEdge	&inEdge)
{
	SNodePosition	temp;

	if (inA.y>inB.y)
		SwapValues(inA,inB,temp);
				
	// inA is above inB
	{
		Rect				nodeATextBox,nodeBRect,nodeARect;
		SInt16				xStart,yStart,xEnd,yEnd;
		
		GetNodeLabelRect(inA,nodeATextBox);
		GetNodeRect(inB,nodeBRect);
		GetNodeRect(inA,nodeARect);

		// arc shift is the # pixels to move the arc start and end positions around the node
		// radius * sin (45)
		short		radius=mNodeDiameter/2;
		short		arcShift=((float)radius)*0.707106;
		#define		kArrowHeadLength 10
		#define		kReflexiveArrowHeadLength (short)(((float)kArrowHeadLength)*0.707106)
		
		// arch height is the height of the arc in pixels
		short		arcHeight=((float)radius)*0.75;

	
		if (inA.y==inB.y)
		{					
			// same y draw from sides, with inA curve
			Rect		arcRect;

			xStart=nodeARect.left+radius;		// central point in the nodes
			xEnd=nodeBRect.left+radius;
			
			// inA is to the left of inB, curve up wards
			// is it inA reflexive arc?
			if (inA.x==inB.x)
			{
				// Draw reflexive arc
				// inA reflexive arc is drawn as inA circle that intersects with the node and has an arrow head on it
				// the arc is the same diameter as the node
				Rect		arcRect=nodeARect;
				
				FOffset(arcRect,-radius,-radius);
				::FrameArc(&arcRect,180,270);
				
				// Draw arrow head
				::MoveTo(nodeARect.left-kReflexiveArrowHeadLength,nodeARect.top-2+radius-kReflexiveArrowHeadLength);
				::Line(kReflexiveArrowHeadLength,kReflexiveArrowHeadLength);
				::Line(-kReflexiveArrowHeadLength,kReflexiveArrowHeadLength);
				
				// Label the arc
				// the arc label is placed above the arc with inA width max width of mXGridSpace/2 in either
				// direction of the centre of the arc
				const LString		&str=inEdge.GetName();
				Rect				textRect;
				short				numLines=CalcLines(&str[1],str[0],mGridXSpace);
				short				lineHeight=GetLineHeight();
				
				textRect.top=arcRect.top-lineHeight*numLines-5;
				textRect.left=(arcRect.right+arcRect.left)/2-mGridXSpace/2;
				textRect.right=textRect.left+mGridXSpace;
				textRect.bottom=textRect.top+lineHeight*numLines;

				::RGBForeColour(&mLabelColour);							
				UTextDrawing::DrawWithJustification((char*)&str[1],str[0],textRect,teCenter,false);
				::RGBForeColour(&mEdgeColour);
			}
			else if (inA.x<inB.x)
			{
				yStart=nodeARect.top+radius-arcShift;
				yEnd=nodeBRect.top+radius-arcShift;
				xStart+=arcShift;
				xEnd-=arcShift;
				
				FSetRect(arcRect,yEnd-arcHeight,xStart,yEnd+arcHeight,xEnd);
				::FrameArc(&arcRect,-90,180);
				
				::MoveTo(xEnd-1,yEnd-kArrowHeadLength-1);
				::Line(0,kArrowHeadLength);
				::Line(-kArrowHeadLength,0);
				
				// Label the arc
				const LString		&str=inEdge.GetName();
				Rect				textRect;
				short				numLines=CalcLines(&str[1],str[0],nodeBRect.right-nodeARect.left);
				short				lineHeight=GetLineHeight();
				
				textRect.top=yEnd-arcHeight-lineHeight*numLines-5;
				textRect.left=nodeARect.left;
				textRect.right=nodeBRect.right;
				textRect.bottom=textRect.top+lineHeight*numLines;

				::RGBForeColour(&mLabelColour);							
				UTextDrawing::DrawWithJustification((char*)&str[1],str[0],textRect,teCenter,false);
				::RGBForeColour(&mEdgeColour);
			}
			else if (inA.x>inB.x)
			{
				yStart=nodeARect.bottom-radius+arcShift;
				yEnd=nodeBRect.bottom-radius+arcShift;
				xStart-=arcShift;
				xEnd+=arcShift;
				
				FSetRect(arcRect,yEnd-arcHeight,xEnd,yEnd+arcHeight,xStart);
				::FrameArc(&arcRect,90,180);
				
				::MoveTo(xEnd,yEnd+kArrowHeadLength-1);
				::Line(0,-kArrowHeadLength);
				::Line(kArrowHeadLength,0);
				
				// Label the arc
				const LString		&str=inEdge.GetName();
				Rect				textRect;
				short				numLines=CalcLines(&str[1],str[0],nodeARect.right-nodeBRect.left);
				short				lineHeight=GetLineHeight();
				
				textRect.top=yEnd+arcHeight+5;
				textRect.left=nodeBRect.left;
				textRect.right=nodeARect.right;
				textRect.bottom=textRect.top+lineHeight*numLines;

				::RGBForeColour(&mLabelColour);							
				UTextDrawing::DrawWithJustification((char*)&str[1],str[0],textRect,teCenter,false);
				::RGBForeColour(&mEdgeColour);
			}
			
			return;
		}
		else
		{
			// a is above b
		
			xStart=nodeATextBox.left+FRectWidth(nodeATextBox)/2;
			yStart=nodeATextBox.bottom+3;
			xEnd=nodeBRect.left+FRectWidth(nodeBRect)/2;
			yEnd=nodeBRect.top;
			
		
			// General case "curve" with a label. If this case is invoked then a.y != b.y
			::MoveTo(xStart,yStart);
			::LineTo(xEnd,yEnd);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DrawUndirectedEdge
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CGraphView::DrawUndirectedEdge(
	SNodePosition		inA,
	SNodePosition		inB,
	CStdGraphEdge		&inEdge)
{
	SNodePosition		temp;

	if (inA.y>inB.y)
		SwapValues(inA,inB,temp);
	
	// inA is above inB
	{
		Rect				nodeATextBox,nodeBRect,nodeARect;
		SInt16				xStart,yStart,xEnd,yEnd;
		
		GetNodeLabelRect(inA,nodeATextBox);
		GetNodeRect(inB,nodeBRect);
		GetNodeRect(inA,nodeARect);
	
		if (inA.y<inB.y)	// inA is above y or equal to it, check which
		{
			xStart=nodeATextBox.left+FRectWidth(nodeATextBox)/2;
			yStart=nodeATextBox.bottom+3;
			xEnd=nodeBRect.left+FRectWidth(nodeBRect)/2;
			yEnd=nodeBRect.top;
		}
		else if (inA.y==inB.y)
		{
			// same y draw from sides
			xStart=nodeATextBox.left+FRectWidth(nodeATextBox)/2;
			yStart=nodeARect.top+FRectHeight(nodeARect)/2;
			xEnd=nodeBRect.left+FRectWidth(nodeBRect)/2;
			yEnd=nodeBRect.top+FRectHeight(nodeBRect)/2;

			if (inA.x<inB.x)
			{
				xStart+=FRectWidth(nodeARect)/2;
				xEnd-=FRectWidth(nodeBRect)/2;
			}
			else if (inA.x>inB.x)
			{
				xStart-=FRectWidth(nodeARect)/2;
				xEnd+=FRectWidth(nodeBRect)/2;
			}
		}
					
		::MoveTo(xStart,yStart);
		::LineTo(xEnd,yEnd);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNodeRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Return the rectangle occupied by a node
void CGraphView::GetNodeRect(
	const SNodePosition		&inPosition,
	Rect					&outRect)
{
	outRect.top=inPosition.y*mGridYSpace;
	outRect.left=inPosition.x*mGridXSpace;
	outRect.bottom=outRect.top+mNodeDiameter;
	outRect.right=outRect.left+mNodeDiameter;
	
	FOffset(outRect,mFrameSize+(mGridXSpace-mNodeDiameter)/2,mFrameSize+(mGridYSpace-mNodeDiameter)/2);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNodeLabelRect
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Return the rectangle occupied by a nodes text
void CGraphView::GetNodeLabelRect(
	const SNodePosition		&inPosition,
	Rect					&outRect)
{
	short			lineHeight=GetLineHeight();

	GetNodeRect(inPosition,outRect);	

	// extend label to be 1/2 mGridXSpace in either direction of the centre line
	outRect.left-=  (mGridXSpace-mNodeDiameter) /2;
	outRect.right+= (mGridXSpace-mNodeDiameter) /2;

	outRect.top+=(mNodeDiameter+3);
	outRect.bottom=outRect.top+lineHeight;
}