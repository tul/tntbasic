// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// TNT Errors.h
// © Mark Tully and TNT Software 1999
// 29/8/99
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
	Error codes used in the TNT Library
*/

#pragma once

enum
{
	kTNTErr_BaseErrorCode				= 15800,
	// TNT Tagged data store format
	kTNTErr_TagNotFound					= 15800,			// Could not find the specified data tag
	
	// TNT Graph library
	kTNTErr_NodesInSeperateGraphs		= 15815,			// Attempt to add an edge linking two nodes in different graphs
	kTNTErr_NullGraph					= 15816,			// Attempt to add an edge linking nodes which aren't in a graph
	kTNTErr_IndexOutOfBounds			= 15817,			// Used when an edge/node index is too high

	kTNTErr_UnregisteredPersistorClass	= 15820,			// Can't reanimate, bad persistor class id
	kTNTErr_DuplicateReanimator			= 15821,			// A second reanimator with the same id was registered
	
	// TNT Music Engine
	kTNTErr_NoMusicLoaded				= 15825,			// Attempt to play music when there's none loaded
	
	// TNT Resources
	kTNTErr_StaleResourceReference		= 15840,			// a resource reference has been released and is no longer valid for reference
	kTNTErr_ResourceAlreadyExists		= 15841,			// attempt to create a resource where one already exists (can you believe apple doesn't have a proper error code for this??)
	
	// Misc
	kTNTErr_InternalError				= -1,				// Use when things are happening which shouldn't! General bail
	kTNTErr_AssertionFailed				= -2,				// An assert has failed
	kTNTErr_UnsupportedDataFormat		= -3,				// Use when loading a resource which is too new to be parsed or a in a similar situation
	kTNTErr_STLException				= -4,				// STL has thrown an exception which has been caught and rethrown as a TNT library exception
};

