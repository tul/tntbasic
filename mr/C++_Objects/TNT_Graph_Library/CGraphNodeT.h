// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Graph Library
// CGraphNodeT.h
// © Mark Tully 1999
// 18/11/99
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

/*
	A node is templated to allow linking of arbitary data with it. You can either sub class CGraphNodeT or but the node as
	a member within your class. Either way setting the data to the "this" of your class allows you to retrieve the node data
	easily.
*/

#pragma once

#include		"CGraphT.h"			// Needs the CGraph::* typedefs

template <class Node,class Edge>
class CGraphT;

template <class Node,class Edge>
class CGraphEdgeT;

template <class Node,class Edge>
class CGraphNodeT
{
	friend class CGraphT<Node,Edge>;
	friend class CGraphEdgeT<Node,Edge>;
	
	protected:
		Node						*mNodeData;
		CGraphT<Node,Edge>			*mParentGraph;

		TArray< CGraphEdgeT<Node,Edge> *>	mConnectedEdges;	// Future improvement : Turn off storing the edges in each node
															// just allow storage in graph. Slower access but less RAM taken.
		
		virtual void				SetGraph(
										CGraphT<Node,Edge> *inGraph);

		virtual void				ReactToNewGraph();

		virtual void /*e*/			EdgeAdded(
										CGraphEdgeT<Node,Edge>	*inEdge);
		virtual void /*e*/			EdgeRemoved(
										CGraphEdgeT<Node,Edge>	*inEdge);

	public:
									CGraphNodeT(
										Node		*inNodeData);
		virtual						~CGraphNodeT();

		virtual CGraphT<Node,Edge>	*GetGraph() const				{ return mParentGraph; }	// Warning - can be null
		CGraph::TNodeIndex			GetNodeIndex();
		
		virtual Node				*GetData() const				{ return mNodeData; }
		virtual void				SetData(
										Node		*inNodeData)	{ mNodeData=inNodeData; }

		virtual CGraph::TEdgeIndex	CountReflexiveEdges();
		virtual CGraph::TEdgeIndex	CountEdges()					{ return mConnectedEdges.GetCount(); }
		virtual CGraphEdgeT<Node,Edge> /*e*/ *GetNthEdge(
										CGraph::TEdgeIndex	inEdgeIndex);
		virtual CGraphEdgeT<Node,Edge> *GetNthEdgeTo(
											Node				*inDestNode,
											CGraph::TEdgeIndex	inIndex);
										
		Node						*operator->()					{ return GetData(); }
};

#include		"CGraphT.h"
#include		"CGraphEdgeT.h"
#include		"CGraphNodeT.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The data for the node is arbitary
template <class Node,class Edge>
CGraphNodeT<Node,Edge>::CGraphNodeT(
	Node			*inNodeData)
{
	mNodeData=inNodeData;
	mParentGraph=0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The destructor unlinks the node from the parent graph if it exists
template <class Node,class Edge>
CGraphNodeT<Node,Edge>::~CGraphNodeT()
{
	if (mParentGraph)
		mParentGraph->NodeDeleted(this);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNodeIndex											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the index of the node in it's parent graph
template <class Node,class Edge>
CGraph::TNodeIndex CGraphNodeT<Node,Edge>::GetNodeIndex()
{
	if (!mParentGraph)
		Throw_(kTNTErr_NullGraph);
	
	return mParentGraph->FindIndexOfNode(this);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReactToNewGraph
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This function goes through all the edges which are connect to this node and deletes them if they're not in the new graph
// (New graph may be nil)
template <class Node,class Edge>
void CGraphNodeT<Node,Edge>::ReactToNewGraph()
{
	TArrayIterator<CGraphEdgeT<Node,Edge>*>	iterator(mConnectedEdges);
	CGraphEdgeT<Node,Edge>					*edge;
	
	while (iterator.Next(edge))
	{
		if (edge->GetGraph()!=mParentGraph)
		{
			mConnectedEdges.Remove(edge);
			if (edge->GetGraph())
				edge->GetGraph()->RemoveEdge(edge);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetGraph
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is a private function called by the parent graph after the node has been installed by CGraphT::AddNode. It sets the
// graph to the one specified - removing the node from any existing graph it's in.
template <class Node,class Edge>
void CGraphNodeT<Node,Edge>::SetGraph(
	CGraphT<Node,Edge>		*inGraph)
{
	if (mParentGraph!=inGraph)
	{
		mParentGraph=inGraph;
		ReactToNewGraph();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EdgeAdded											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is a private routine called by an edge to connect it to the node. Just because an edge is in the connected edge
// list doens't mean that's it's an outgoing edge however.
// If this==inEdge->GetNodeA() || (this==inEdge->GetNodeB() && inEdge->IsBiDirectional()) then it's an outgoing.
// If (this==inEdge->GetNodeA() && inEdge->IsBiDirectional()) || (this==inEdge->GetNodeB()) then it's an incoming
// If inEdge->IsBiDirectional() then it's both
template <class Node,class Edge>
void CGraphNodeT<Node,Edge>::EdgeAdded(
	CGraphEdgeT<Node,Edge>	*inEdge)
{
	mConnectedEdges.AddItem(inEdge);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ EdgeRemoved
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes an edge from the list of connected edges
template <class Node,class Edge>
void CGraphNodeT<Node,Edge>::EdgeRemoved(
	CGraphEdgeT<Node,Edge>	*inEdge)
{
	mConnectedEdges.Remove(inEdge);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNthEdge										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the nth connected edge. See the EdgeAdded routine on how to see if an edge is incoming, outgoing, or both.
// throws if index out of bounds. Zero based index.
template <class Node,class Edge>
CGraphEdgeT<Node,Edge> *CGraphNodeT<Node,Edge>::GetNthEdge(
	CGraph::TEdgeIndex	inEdgeIndex)
{
	if (inEdgeIndex>=CountEdges())
		Throw_(kTNTErr_IndexOutOfBounds);
	
	return mConnectedEdges[inEdgeIndex+1];
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetNthEdgeTo
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the nth edge to the specified node, nil for none
template <class Node,class Edge>
CGraphEdgeT<Node,Edge> *CGraphNodeT<Node,Edge>::GetNthEdgeTo(
	Node				*inDestNode,
	CGraph::TEdgeIndex	inIndex)
{
	CGraph::TEdgeIndex	max=CountEdges();
	CGraph::TEdgeIndex	found=0;

	for (CGraph::TEdgeIndex index=1; index<=max; index++)
	{
		if (mConnectedEdges[index]->GetNodeB()==inDestNode)
		{
			if (found++==inIndex)
				return mConnectedEdges[index];
		}
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountReflexiveEdges
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the number of edges that link this node to itself
template <class Node,class Edge>
CGraph::TEdgeIndex CGraphNodeT<Node,Edge>::CountReflexiveEdges()
{
	CGraph::TEdgeIndex	max=CountEdges();
	CGraph::TEdgeIndex	found=0;

	for (CGraph::TEdgeIndex index=1; index<=max; index++)
	{
		if (mConnectedEdges[index]->GetNodeA()==this && mConnectedEdges[index]->GetNodeB()==this)
			found++;
	}
	
	return found;
}