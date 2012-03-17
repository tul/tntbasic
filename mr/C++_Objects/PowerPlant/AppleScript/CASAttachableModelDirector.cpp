// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CASAttachableModel.cpp
// © Mark Tully and TNT Software 1998-1999
// 11/11/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

#include	"MR Debug.h"
#include	<UAppleEventsMgr.h>
#include	"CASAttachableModelDirector.h"
#include	"CASAttachableModelObject.h"
#include	"CASIChain.h"
#include	"TNT AS Registry.h"
#include	"UASSubroutinesAEHandler.h"
#include	"UScripts.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊConstructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Passing a default model means that the call backs are automatically installed.
CASAttachableModelDirector::CASAttachableModelDirector(LModelObject *inDefaultModel)
{
	mLastActive=mLastOrigCall=0;
	mPreviousActiveUPP=mCurrentActiveUPP=0L;
	LModelObject::SetDefaultModel(inDefaultModel);
	ConnectToScriptingComponent();
	InstallCallBacks();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊConstructor											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Default constructor, you must call InstallCallBacks yourself if you use this version
CASAttachableModelDirector::CASAttachableModelDirector()
{
	mLastActive=mLastOrigCall=0;
	mPreviousActiveUPP=mCurrentActiveUPP=0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊDestructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CASAttachableModelDirector::~CASAttachableModelDirector()
{
	DisconnectFromScriptingComponent();
	if (mResumeDispatchUPP)
		::DisposeAEEventHandlerUPP(mResumeDispatchUPP);

	if (mPreviousActiveUPP)
		::DisposeOSAActiveUPP(mPreviousActiveUPP);
	if (mCurrentActiveUPP)
		::DisposeOSAActiveUPP(mCurrentActiveUPP);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊConnectToScriptingComponent							/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine opens the connection to the scripting component returned by GetScripingComponent
void CASAttachableModelDirector::ConnectToScriptingComponent()
{
	ComponentDescription 	descr;
	Component           	component;

	descr.componentType         = kOSAComponentType;
	descr.componentSubType      = (OSType) 0;
	descr.componentManufacturer = (OSType) 0;
	descr.componentFlags        = kOSASupportsCompiling + 
								  kOSASupportsGetSource + 
								  kOSASupportsConvenience + 
								  kOSASupportsEventHandling;
	descr.componentFlagsMask    = descr.componentFlags;
	
	// Just use the first component which matches our criteria
	component=FindNextComponent(nil,&descr);
		
	if (!component)
		Throw_(errOSACantOpenComponent);
	else
	{
		mScriptingComponent=OpenComponent(component);
																								
		if (!mScriptingComponent)
			Throw_(errOSACantOpenComponent);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊDisonnectFromScriptingComponent
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void CASAttachableModelDirector::DisconnectFromScriptingComponent()
{
	if (mScriptingComponent)
	{
		CloseComponent(mScriptingComponent);
		mScriptingComponent=0L;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊGetScriptingComponent
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the connection to the scripting component which is needed for working with scripts and the OSA. It is allocated
// by the constructor.
ComponentInstance CASAttachableModelDirector::GetScriptingComponent()
{
	return mScriptingComponent;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊInstallCallBacks										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Attempts to install the predispatch handler
void CASAttachableModelDirector::InstallCallBacks()
{
	LModelDirector::InstallCallBacks();
	
	ThrowIfOSErr_(AEInstallSpecialHandler(keyPreDispatch,NewAEEventHandlerUPP(CASAttachableModelDirector::PreDispatchHandler),false));

	// Allocate a UPP for the call back handler
	mResumeDispatchUPP=NewAEEventHandlerUPP(ResumeDispatchHandler);
	ThrowIfMemFull_(mResumeDispatchUPP);
	
	// Get the previous UPP
	ThrowIfOSErr_(::OSAGetActiveProc(mScriptingComponent,&mPreviousActiveUPP,&mPreviousRefCon));
	
	// Create a new UPP
	mCurrentActiveUPP=NewOSAActiveUPP(ActiveHandler);
	
	// Install the UPP
	ThrowIfOSErr_(::OSASetActiveProc(mScriptingComponent,mCurrentActiveUPP,0L));
	
	UASSubroutinesAEHandler::InstallCallBacks();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊObtainEventNumber										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Override this to get the event number from a different source than the installed handlers refcon
SInt32 CASAttachableModelDirector::ObtainEventNumber(		DescType				inEventClass,
															DescType				inEventId)
{
	SInt32				eventNumber;
	AEEventHandlerUPP	handler;

	// Detect the as sub routine calls explicity.
	// This is done as some application use subroutines in prehandler only and so the main handler has no subroutine
	// handler entry.
	if (inEventClass==kASAppleScriptSuite && inEventId==kASSubroutineEvent)
		return ae_ASSubroutine;
	
	Try_
	{
		ThrowIfOSErr_(::AEGetEventHandler(inEventClass,inEventId,&handler,&eventNumber,false));
	}
	Catch_(err)
	{
		if (ErrCode_(err)==errAEHandlerNotFound)
			Throw_(errAEEventNotHandled);
		throw;
	}
	
	return eventNumber;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊObtainEventNumber										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine gets the event number (typically the refcon of an apple event handler loaded from an aedt resource) for a
// given event. If the event has no handler (and it's not a special event such as AppleScripts call sub routine event) it
// throws errAEEventNotHandled.
SInt32 CASAttachableModelDirector::ObtainEventNumber(const AppleEvent &inAppleEvent)
{
	Size				theSize;
	DescType			eventClass,eventId,theType;

	// Extract the class and ID of the event from the AppleEvent
	ThrowIfOSErr_(::AEGetAttributePtr(&inAppleEvent,keyEventClassAttr,typeType,&theType,(Ptr)&eventClass,sizeof(eventClass),&theSize));
	ThrowIfOSErr_(::AEGetAttributePtr(&inAppleEvent,keyEventIDAttr,typeType,&theType,(Ptr)&eventId,sizeof(eventId),&theSize)); 
	
	return ObtainEventNumber(eventClass,eventId);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊHandleResumeDispatch									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine handles the action taken when a script calls the continue command of AppleScript.
// The refcon passed to this routine is actually a linkedlist of CASAttachableModelObjects. Each time this is called, it
// pops the top most model off the list and executes the event it it's context (which could recursively call this routine
// again). When the list is empty and the event hasn't been handled it throws errAEEventNotHandled which should lead to
// the standard apple event handler getting called.
// The passed list is destroyed as this routine executes. If an error is thrown then the list will not be completely
// disposed of.
void CASAttachableModelDirector::HandleResumeDispatch(	const AppleEvent	&inAppleEvent,
														AppleEvent			&outAEReply,
														CASIChain			*ioIChain)
{
	CASAttachableModelObject	*targetObject;
	AEEventHandlerUPP			oldResumeProc;
	long						oldRefCon;
	OSErr						err=noErr;

	ThrowIfOSErr_(::OSAGetResumeDispatchProc(GetAttachableModelDirector()->GetScriptingComponent(),&oldResumeProc,&oldRefCon));
	
	Try_
	{	
		targetObject=ioIChain->PopModelObject();
		if (targetObject)
		{
			// Call the object's script if it has one
			Try_
			{
				StAEDescriptor				theResult;

				if (ioIChain->mIChain.CountElements())
					err=::OSASetResumeDispatchProc(GetScriptingComponent(),mResumeDispatchUPP,(SInt32)ioIChain);
				else
					err=::OSASetResumeDispatchProc(GetScriptingComponent(),(AEEventHandlerUPP)kOSAUseStandardDispatch,kOSADontUsePhac);

				if (err==noErr)
					targetObject->ExecuteEventInAttachmentAll(inAppleEvent,outAEReply,theResult,ioIChain->GetAENumber());
			
				// Put result code in Reply
				if ( (theResult.mDesc.descriptorType != typeNull) &&
					 (outAEReply.descriptorType != typeNull) )
				{	 
					err=::AEPutParamDesc(&outAEReply, keyAEResult, &theResult.mDesc);
					ThrowIfOSErr_(err);
				}
			}
			Catch_(err)
			{
				if (ErrCode_(err)==errAEEventNotHandled)	// If the apple event has not been handled then do a "continue" up the inheritance chain
				{
					// If the event wasn't handled then pass up to the next element in the chain.
					// If there are no more elements then it throws an event not handled error which will cause the standard
					// handler to be invoked
					if (ioIChain->mIChain.CountElements())
						HandleResumeDispatch(inAppleEvent,outAEReply,ioIChain);
					else
						throw;
				}
				else
					throw;
			}
		}
		else
		{
			// Call the default apple event handler
			ThrowIfOSErr_(errAEEventNotHandled);
		}
	}
	Catch_(err)
	{
		::OSASetResumeDispatchProc(GetAttachableModelDirector()->GetScriptingComponent(),oldResumeProc,oldRefCon);
		throw;
	}
	::OSASetResumeDispatchProc(GetAttachableModelDirector()->GetScriptingComponent(),oldResumeProc,oldRefCon);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊHandlePreDispatchOfEvent								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is the pre-dispatch handler which you override to provide additional functionality.
// The default action is to resolve the token, dynamic cast to a CASAttachableModelObject, get it's script and call it.
void CASAttachableModelDirector::HandlePreDispatchOfEvent(	const AppleEvent		&inAppleEvent,
															AppleEvent				&outAEReply,
															SInt32					inRefCon)
{
	SInt32				eventNumber=ObtainEventNumber(inAppleEvent);
	StAEDescriptor		directObj;
	StAEDescriptor		directObjToken;
	OSErr				err;
	
	// Get the key-direct object and attempt to resolve it into an attachable model object token
	directObj.GetOptionalParamDesc(inAppleEvent, keyDirectObject,typeWildCard);
	
	if (directObj.mDesc.descriptorType != typeNull)
	{
		// Direct object exists, attempt to resolve into a token
		err=Resolve(directObj.mDesc,directObjToken.mDesc);
	}
	
	// Do we have an object?
	if ( (directObj.mDesc.descriptorType == typeNull) ||
		 (err == errAENotAnObjSpec) ||
		 (err == errAENoSuchObject) ||
		 (err == errAEDescNotFound) )
	{
		// No object, can we get the object from the subject attribute?
		if (directObj.mDesc.descriptorType!=typeObjectSpecifier)
		{
			directObj.Dispose();
			err=::AEGetAttributeDesc(&inAppleEvent,keySubjectAttr,typeWildCard,&directObj.mDesc);
			if (!err)
				err=Resolve(directObj.mDesc,directObjToken.mDesc);
		}
	}
	
	if ( (directObj.mDesc.descriptorType == typeNull) ||
		 (err == errAENotAnObjSpec) ||
		 (err == errAENoSuchObject) ||
		 (err == errAEDescNotFound) )
	{		
		// Direct object parameter not present or is not an object specifier. Let the default model handle it.
		CASAttachableModelObject			*modelObj=dynamic_cast<CASAttachableModelObject*>(LModelObject::GetDefaultModel());

		if (modelObj)
		{				
			// Execute the event using the inheritance chain specified
			modelObj->PreHandleAppleEvent(inAppleEvent,outAEReply,eventNumber);
		}
		else
			Throw_(errAEEventNotHandled);
	}
	else if (err==noErr)
	{	
		// Process this event using the Token representing the direct direct
		PreHandleTokenizedEvent(inAppleEvent,directObjToken.mDesc,outAEReply,eventNumber);
	}
	else
		Throw_(err);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊPreHandleTokenizedEvent								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This routine calls the attached script (if available) for the token(s) specified in the inDirectObjToken parameter.
// It doesn't collect the results of the individual tokenized calls yet.
void CASAttachableModelDirector::PreHandleTokenizedEvent(	const AppleEvent	&inAppleEvent,
															AEDesc				&inDirectObjToken,
															AppleEvent			&outReply,
															SInt32				inRefCon)
{
	CASAttachableModelObject	*theResponder;
	OSErr						err = noErr;

	if (inDirectObjToken.descriptorType!=typeAEList)
	{
		theResponder=dynamic_cast<CASAttachableModelObject*>(LModelObject::GetModelFromToken(inDirectObjToken));
		if (theResponder)
			theResponder->PreHandleAppleEvent(inAppleEvent,outReply,inRefCon);
		else
			Throw_(errAEEventNotHandled);
	}
	else
	{
		SInt32		count;
	
		// Direct object is a list of ModelObjects
		::AECountItems(&inDirectObjToken,&count);
		
		for (SInt32 i = 1; i <= count; i++)
		{
			StAEDescriptor		token;
			DescType			bogusKeyword;
			
			err = ::AEGetNthDesc(&inDirectObjToken,i,typeWildCard,&bogusKeyword,&token.mDesc);
			ThrowIfOSErr_(err);
			
			theResponder=dynamic_cast<CASAttachableModelObject*>(LModelObject::GetModelFromToken(token));
			if (theResponder)
			{
				theResponder->PreHandleAppleEvent(inAppleEvent,outReply,inRefCon);
//				UAEDesc::AddDesc(&outResult,0,resultDesc);
			}
			else
				Throw_(errAEEventNotHandled);
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊResumeDispatchHandler									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This handler simply maps onto the object non-static version
pascal OSErr CASAttachableModelDirector::ResumeDispatchHandler(	const AppleEvent 	*inAppleEvent,
																AppleEvent			*outReplyEvent,
																SInt32				inRefCon)
{
	OSErr	err = noErr;
			
	Try_
	{
		CASAttachableModelDirector		*handler;
		
		handler=GetAttachableModelDirector();
		
		if (handler)
			handler->HandleResumeDispatch(*inAppleEvent,*outReplyEvent,(CASIChain*)inRefCon);
	}
	Catch_(inErr)
	{
		err=ErrCode_(inErr);
	}		
	catch (...)
	{
		err=errAEEventNotHandled;
	}

	return err;	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊActiveHandler											Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Maps onto the non-static version.
pascal OSErr CASAttachableModelDirector::ActiveHandler(
	SInt32				inRefCon)
{
	CASAttachableModelDirector		*handler;
	
	handler=GetAttachableModelDirector();
	
	Try_
	{
		if (handler)
			handler->HandleActive(inRefCon);		
	}
	Catch_(err)
	{
		return ErrCode_(err);
	}
	
	return noErr;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊHandleActive											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called periodically by the scripting component during compilation of scripts and execution. By default calls the
// scripting components ActiveHandler then the Tickle() proc. A limit was introduce to stop the original active handler
// getting called so often. Now it only calls the other if this proc is getting called with a frequency of at least once
// every three seconds, and then it only calls the original at once every one second.
// This is to stop us spending so much time in other apps except in the most active loops as it was killing pyramids load
// time.
void CASAttachableModelDirector::HandleActive(
	SInt32				inRefCon)
{
	// Call the original - but at maximum once every 2 seconds
	if ((TickCount()-mLastActive)<3*60)			// if we we're last called within 3 seconds
	{
		// and if the last call we made to the original active proc was more than 1 seconds ago
		if ((TickCount()-mLastOrigCall)>1*60)
		{
			ThrowIfOSErr_(InvokeOSAActiveUPP(mPreviousRefCon,mPreviousActiveUPP));
			mLastOrigCall=TickCount();
		}
	}
	
	mLastActive=TickCount();

	// Call the override
	Tickle();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊTickle												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called periodically during script compilation and execution by the scripting component as well as the default handler.
// Can be used to animate the cursor and stuff.
void CASAttachableModelDirector::Tickle()
{
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊPreDispatchHandler									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This handler simply maps onto the objects non-static version
pascal OSErr CASAttachableModelDirector::PreDispatchHandler(const AppleEvent*	inAppleEvent,
															AppleEvent*			outAEReply,
															SInt32				inRefCon)
{
	OSErr	err = noErr;
			
	Try_
	{
		CASAttachableModelDirector		*handler;
		
		handler=GetAttachableModelDirector();
		
		if (handler)
			handler->HandlePreDispatchOfEvent(*inAppleEvent,*outAEReply,inRefCon);
	}
	Catch_(inErr)
	{
		err=ErrCode_(inErr);
	}		
	catch (...)
	{
		err=errAEEventNotHandled;
	}

	LModelObject::FinalizeLazies();

	return err;	
}
