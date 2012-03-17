// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Graph Library
// CGraphEdgeT.h
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
	An edge is templated to allow linking of arbitary data with it. You can either sub class CGraphEdgeT or put the edge as
	a member within your class. Either way setting the data to the "this" of your class allows you to retrieve the edge data
	easily.
*/

#pragma once

template <class Node,class Edge>
class CGraphT;

template <class Node,class Edge>
class CGraphNode;

template <class Node,class Edge>
class CGraphEdgeT
{
	friend class CGraphT<Node,Edge>;
	
	protected:
		Edge						*mEdgeData;

		CGraphNodeT<Node,Edge>		*mNodeA,*mNodeB;
		bool						mBiDirectional;

		virtual void /*e*/			SetNodes(
										CGraphNodeT<Node,Edge>		*inNodeA,
										CGraphNodeT<Node,Edge>		*inNodeB);

	public:
									CGraphEdgeT(
										Edge						*inEdgeData,				// Many errors in TNT List Lib were due to users accidentally leaving a null data ptr (the default parameter). By not allowing defaults hopefully graph lib will be less prone to error
										bool						inBiDirectional=true);
		virtual						~CGraphEdgeT();

																		// Warning GetGraph() - can return null
		virtual CGraphT<Node,Edge>	*GetGraph() const				{ return mNodeA && mNodeA->GetGraph() ? mNodeA->GetGraph() : (mNodeB && mNodeB->GetGraph() ? mNodeB->GetGraph() : 0L); }

		virtual bool				IsBiDirectional() const			{ return mBiDirectional; }

		virtual CGraphNodeT<Node,Edge>	*GetNodeA()					{ return mNodeA; }
		virtual CGraphNodeT<Node,Edge>	*GetNodeB()					{ return mNodeB; }
		
		virtual Edge				*GetData() const				{ return mEdgeData; }
		virtual void				SetData(
										Edge		*inEdgeData)	{ mEdgeData=inEdgeData; }
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The data for the edge is arbitary
template <class Node,class Edge>
CGraphEdgeT<Node,Edge>::CGraphEdgeT(
	Edge					*inEdgeData,
	bool					inBiDirectional)
{
	mEdgeData=inEdgeData;
	mNodeA=0;
	mNodeB=0;
	mBiDirectional=inBiDirectional;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The destructor unlinks the edge from the parent graph if it exists
template <class Node,class Edge>
CGraphEdgeT<Node,Edge>::~CGraphEdgeT()
{
	SetNodes(0L,0L);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetNodes												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is a private function called by the parent graph after the edge has been installed by CGraphT::AddEdge. It sets the
// the nodes the edge connects to the ones specified. Both can be null or both valid - but "open" edges, edges with one end
// null are not supported.
// Under low memory situations this operation could leave a partially updated graph - if this is a problem then catch
// exceptions from EdgeAdded.
template <class Node,class Edge>
void CGraphEdgeT<Node,Edge>::SetNodes(
	CGraphNodeT<Node,Edge>		*inNodeA,
	CGraphNodeT<Node,Edge>		*inNodeB)
{
	// See if the nodes are different
	if (inNodeA==mNodeA && inNodeB==mNodeB)
		return;

	if ((!inNodeA && inNodeB) || (inNodeA && !inNodeB))
	{
		SignalPStr_("\pNull node passed to CGraphEdgeT::SetNodes()");
		Throw_(paramErr);
	}

	// disconnect from the nodes
	if (mNodeA)
		mNodeA->EdgeRemoved(this);
	if (mNodeB)
		mNodeB->EdgeRemoved(this);
	
	mNodeA=inNodeA;
	mNodeB=inNodeB;
	
	if (mNodeA)
		mNodeA->EdgeAdded(this);
	if (mNodeB)
		mNodeB->EdgeAdded(this);
}
