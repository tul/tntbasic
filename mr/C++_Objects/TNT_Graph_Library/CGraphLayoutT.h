// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Graph Library
// CGraphLayoutT.h
// © Mark Tully 2000
// 24/2/00
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
	A templated class for laying out graphs
*/

template <class Node,class Edge>
class CGraphT;

struct SNodePosition
{
	SInt32						x,y;
	
								SNodePosition(
									SInt32		inX=0,
									SInt32		inY=0) : x(inX), y(inY) {}
};

template <class Node,class Edge>
class CGraphLayoutT
{
	protected:
		CGraphT<Node,Edge>		*mGraph;
		bool					mOwnsGraph;
		TArray<SNodePosition>	mLayout;
		SInt32					mMaxWidth,mMaxHeight;
		
	public:	
		/*e*/					CGraphLayoutT(
									CGraphT<Node,Edge>	*inGraph=0,
									bool				inAdoptGraph=true);
		virtual					~CGraphLayoutT();
		
		virtual void /*e*/		LayoutGraph();	

		virtual void /*e*/		LayoutVHierarchy();
		virtual void /*e*/		LayoutGeneralGraph();
		virtual void /*e*/		LayoutNodeRingGraph();


		SInt32					GetMaxWidth()			{ return mMaxWidth; }
		SInt32					GetMaxHeight()			{ return mMaxHeight; }
		TArray<SNodePosition>	&GetLayout()			{ return mLayout; }
		CGraphT<Node,Edge>		*GetGraph()				{ return mGraph; }
		void					SetGraph(
									CGraphT<Node,Edge>	*inGraph,
									bool				inAdopt);
};

#include		"CGraphT.h"
#include		"CGraphLayoutHinter.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Makes a note of the graph to be laid out
template <class Node,class Edge>
CGraphLayoutT<Node,Edge>::CGraphLayoutT(
	CGraphT<Node,Edge>		*inGraph,
	bool					inAdoptGraph) :
	mMaxWidth(0), mMaxHeight(0), mGraph(0), mOwnsGraph(false)
{
	SetGraph(inGraph,inAdoptGraph);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If the graph is owned by this object it's deleted
template <class Node,class Edge>
CGraphLayoutT<Node,Edge>::~CGraphLayoutT()
{
	if (mOwnsGraph)
		delete mGraph;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetGraph
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the graph
template <class Node,class Edge>
void CGraphLayoutT<Node,Edge>::SetGraph(
	CGraphT<Node,Edge>	*inGraph,
	bool				inAdopt)
{
	if (mOwnsGraph)
		delete mGraph;
	mGraph=inGraph;
	mOwnsGraph=inAdopt;

	mLayout.RemoveItemsAt(mLayout.GetCount(),1);
	
	if (mGraph)
	{
		// Insert enough node position structures to represent the entire graph
		SNodePosition		nullPosition;
		mLayout.InsertItemsAt(mGraph->CountNodes(),0,nullPosition);
		
		LayoutGraph();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LayoutVHierarchy												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Lays out a graph which has a root node and then all subsequent nodes have a level and a single parent.
// Node 0 is the root
// A nodes sub nodes are on the end of outgoing edges.
template <class Node,class Edge>
void CGraphLayoutT<Node,Edge>::LayoutVHierarchy()
{
	// Assume node index 0 is the root - perhaps not a great assumption
	CGraphNodeT<Node,Edge>					*root=reinterpret_cast< CGraphNodeT<Node,Edge> * >(mGraph->GetNthNode(0));
	
	if (root)
	{
		SInt32								maxWidth=1;
		ArrayIndexT							nodeIndex=0;
		SNodePosition						nodePosition;
		TArray<CGraphNodeT<Node,Edge>*>		open1;		
		TArray<CGraphNodeT<Node,Edge>*>		open2;		
		TArray<CGraphNodeT<Node,Edge>*>		*expandThisLevel=&open1;	// list of all the nodes which need to be expanded on this level
		TArray<CGraphNodeT<Node,Edge>*>		*expandNextLevel=&open2;	// list of all the nodes which need to be expanded on the next level
		
		// Place the root node
		mLayout.AssignItemsAt(1,nodeIndex+1,nodePosition);
		expandThisLevel->AddItem(root);
		
		while (expandThisLevel->GetCount()>0)
		{
			// place all nodes at this level
			TArrayIterator<CGraphNodeT<Node,Edge>*>	iter(*expandThisLevel);
			CGraphNodeT<Node,Edge>					*node;
			
			while (iter.Next(node))
			{
				mLayout.AssignItemsAt(1,node->GetNodeIndex()+1,nodePosition);
				nodePosition.x++;
								
				// for all sub goals add them to the list of items to be expanded on the next pass
				CGraph::TEdgeIndex		max=node->CountEdges();
				
				for (CGraph::TEdgeIndex index=0; index<max; index++)
				{
					CGraphEdgeT<Node,Edge>	*edge=node->GetNthEdge(index);
					
					// each out going edge goes to a sub node
					if (edge->GetNodeA()==node)				
						expandNextLevel->AddItem(edge->GetNodeB());
				}
			}

			// remember the max width of the graph so far so we can balance it later
			if (nodePosition.x>maxWidth)
				maxWidth=nodePosition.x;
			nodePosition.x=0;	// reset x position of graph node
			nodePosition.y++;
			
			// Clear this level and swap with the next level
			expandThisLevel->RemoveItemsAt(expandThisLevel->GetCount(),1);
			{
				TArray<CGraphNodeT<Node,Edge>*>		*temp=expandThisLevel;
				expandThisLevel=expandNextLevel;
				expandNextLevel=temp;
			}
		}
		
		mMaxWidth=maxWidth;
		mMaxHeight=nodePosition.y;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LayoutGeneralGraph											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Lays out a graph in a general manner. It arranges the nodes in a circle essensially.
template <class Node,class Edge>
void CGraphLayoutT<Node,Edge>::LayoutGeneralGraph()
{
	if (!mGraph)
		return;

	CGraph::TNodeIndex		count=mGraph->CountNodes();
	SNodePosition			nodePosition;
	
	// special case the first few
	switch (count)
	{
		case 1:	// place in top left
			mLayout.AssignItemsAt(1,1,nodePosition);		// node at 0,0
			mMaxWidth=mMaxHeight=1;
			break;
		
		case 2: // 2 horizontally
			mLayout.AssignItemsAt(1,1,nodePosition);		// node at 0,0
			nodePosition.x++;
			mLayout.AssignItemsAt(1,2,nodePosition);		// node at 1,0			
			mMaxWidth=2;
			mMaxHeight=1;
			break;

		case 3: // form a triangle
			nodePosition.x=1;
			mLayout.AssignItemsAt(1,1,nodePosition);		// node at 1,0
			nodePosition.x=0;
			nodePosition.y=1;
			mLayout.AssignItemsAt(1,2,nodePosition);		// node at 0,1		
			nodePosition.x=2;
			mLayout.AssignItemsAt(1,3,nodePosition);		// node at 2,1
			mMaxWidth=3;
			mMaxHeight=2;
			break;
			
		case 4:	// form a square
			mLayout.AssignItemsAt(1,1,nodePosition);		// node at 0,0
			nodePosition.x=1;
			mLayout.AssignItemsAt(1,2,nodePosition);		// node at 1,0		
			nodePosition.y=1;
			mLayout.AssignItemsAt(1,3,nodePosition);		// node at 1,1
			nodePosition.x=0;
			mLayout.AssignItemsAt(1,4,nodePosition);		// node at 0,1
			mMaxWidth=mMaxHeight=2;
			break;
			
		default:
			LayoutNodeRingGraph();
			break;
	}		
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LayoutNodeRingGraph											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Lays out the nodes in a ring around a central point, the general idea is that there will be no crossing edges if the graph
// is in a ring. However, it has the disadvantage that if there are a lot of nodes the resulting layout is unnecessarily
// large.
template <class Node,class Edge>
void CGraphLayoutT<Node,Edge>::LayoutNodeRingGraph()
{
	static const double kCircularSpacing=1.5;
	static const double kPi=3.141592654;

	if (!mGraph || !mGraph->CountNodes())
		return;
		
	CGraph::TNodeIndex		numNodes=mGraph->CountNodes();
	double					radius = (numNodes*kCircularSpacing)/(2.0*kPi);
	
	// if the radius is even, make it odd then it has a meaningful centre pos
	if ((((SInt32)nearbyint(radius))&1)==0)
		radius+=1.0;
	
	double					angle=0.0;
	double					step = 2.0*kPi / (double)numNodes;
	SNodePosition			centreNode(std::nearbyint(radius),std::nearbyint(radius)),nodePos;
	
	mMaxWidth=mMaxHeight=std::nearbyint(radius)*2+1;	// +1 as the max width is the maximum used value + 1
	
	for (CGraph::TNodeIndex index=0; index<numNodes; index++)
	{
		nodePos.x=centreNode.x-std::nearbyint(sin(angle)*radius);
		nodePos.y=centreNode.y-std::nearbyint(cos(angle)*radius);
		mLayout.AssignItemsAt(1,index+1,nodePos);
		angle+=step;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ LayoutGraph													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
template <class Node,class Edge>
void CGraphLayoutT<Node,Edge>::LayoutGraph()
{
	if (!mGraph)
		return;
		
	// Look for a hint in the graph to see what kind of graph it is
	SGraphLayoutHint	hint;
	
	CGraphLayoutHinter	*hinter=dynamic_cast<CGraphLayoutHinter*>(mGraph);
	
	if (hinter)
	{
		hinter->GetHint(hint);
	
		switch (hint.type)
		{
			case SGraphLayoutHint::kVHierarchy:
				LayoutVHierarchy();
				break;
				
			default:
				LayoutGeneralGraph();
				break;
		}
	}
	else
		LayoutGeneralGraph();		// Future improvement : determine the type of graph some other way?
	
}