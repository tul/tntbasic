// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Graph Library
// StdGraph.h
// © Mark Tully 2000
// 26/2/00
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
	Implementations of standard graph nodes which just have a text label
*/

#pragma once

class CStdGraphEdge;
class CStdGraphNode;
class CStdGraph;

#include		"CGraphNodeT.h"
#include		"CGraphEdgeT.h"
#include		"CGraphT.h"

class CStdGraph : public CGraphT<CStdGraphNode,CStdGraphEdge>
{
	protected:
		LStr255								mName;

	public:
											CStdGraph() {}
											CStdGraph(
												const LString	&inStr) : mName(inStr) {}
		virtual const LString				&GetName()			{ return mName; }
		virtual void						SetName(
												const unsigned char *inStr) { mName=inStr; }
};

class CStdGraphNode : public CGraphNodeT<CStdGraphNode,CStdGraphEdge>
{
	protected:
		LStr255								mName;
			
	public:
											CStdGraphNode() : CGraphNodeT(this) {}
											CStdGraphNode(
												const LString		&inStr) : mName(inStr), CGraphNodeT(this) {}
		virtual const LString				&GetName()			{ return mName; }
		virtual LString						&GetNameNonConst()	{ return mName; }
};

class CStdGraphEdge : public CGraphEdgeT<CStdGraphNode,CStdGraphEdge>
{
	protected:
		LStr255								mName;

	public:
											CStdGraphEdge(
												const LString		&inStr) : mName(inStr), CGraphEdgeT(this) {}
											CStdGraphEdge() : CGraphEdgeT(this) {}
		virtual const LString				&GetName()			{ return mName; }
		virtual LString						&GetNameNonConst()	{ return mName; }
};