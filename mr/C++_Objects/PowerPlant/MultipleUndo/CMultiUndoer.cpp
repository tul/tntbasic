// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CMultiUndoer.cpp
// © Mark Tully and TNT Software 1998
// 17/11/98
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

// Implements multiple undo for powerplant. Differs from the LUndoer class in that it never calls an LActions Finalize
// member as it is unclear exactly when to call this and most users of the finalize function will make assumptions
// about the context of the call. It is different for CMultiUndoer.

// USAGE NOTES:
// ------------
// If your actions are well structured then you should have no problem with multiple undo. If you fall for any of the
// problems below then beware.
//		¥	Storing a reference to a commander in your action then using PostAction(nil) to delete the action in the
//			commanders destructor.
//		¥	Modifying mIsDone and then calling UpdateCommandStatus to change the state of an action from Undo to Redo
// PowerPlants LTypingAction does both of these and it confuses the hell out of CMultiUndoer.

// LEditField and friends don't like multiple undo
// -----------------------------------------------
// It should be realised that any classes which use actions with the assumption that there will be only one LAction instantated
// at a time could have a bit of trouble with multiple undo.
// An example of this is with the LEditField. Some classes such as LEditableOutlineItem dynamically create an LEditField when
// editing is needed. The LEditField creates actions which store a reference to the LEditField. If the action survives longer
// than the LEditField (which it will do in the case of undo where the actions are being stored outside of the
// LEditField eg in the supercommanders LUndoer/CMultiUndoer attachments) then a crash would occur when the action is
// eventually deleted or is attempted to be done or undone. This is because the LEditField has been deleted and has left the
// action with a dangling ptr. LEditField stops this from happening by calling PostAction(nil) which clears the undo stack.
// This is fine for single undo, but it's not good for multiple undo as it takes out all the actions not just the ones with
// the dangling ptrs. This will not be appreciated by your users.
// The suggestion is to use a seperate undo attachment for these LEditFields.
// Another problem LEditField has it that it changes the mIsDone field directly so it can use the same typing action for
// both undo and redo. This really confuses CMultiUndoer as it remains uninformed that a previsously undone action is now
// suddenly done again. As the action is still in the undo stack the undo menu item remains set. LUndoer checks the mIsDone
// field every time FindCommandStatus is called and changes the menu item appropiately. To do this in CMultiUndoer I would
// have to move actions between stacks depending on the state of their mIsDone fields. I won't be implementing this as it
// would really mess things up so. The LTypingAction suite seems like a big hack which only works with single undo
// effectively. LEditField, LEditText and LEditTextView have all the same problems.
// So overall usage of PostAction(nil) is depreciated as it deletes all actions instead of just the ones related to the
// commander which is calling it. Modifying the mIsDone field directly is also depreciated.

// Additional actions shouldn't be posted during a DoRedo call as DoRedo is just doing an action which has already been
// done. If actions are introduced then it's quite likely to be a bug as it will clear the remainder of the redo stack
// and stop the user from continuing their redo exploits. The same also applies to a DoUndo call. To aid in detecting this
// problem Signals are made if a postaction is received during a DoUndo or DoRedo call.

#include			"MR Debug.h"
#include			"CMultiUndoer.h"
#include			<TArrayIterator.h>
#include			<LString.h>
#include			<LCommander.h>
#include			<LStream.h>
#include			<LAction.h>
#include			<UMemoryMgr.h>

bool 				CMultiUndoer::sWarnOnPostAction=false;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CMultiUndoer::CMultiUndoer(ArrayIndexT inMaxUndoSteps)
{
	mMaxUndoSteps=inMaxUndoSteps;
	mNumActionsInProgress=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor [Stream]
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CMultiUndoer::CMultiUndoer(LStream *inStream) : LAttachment(inStream)
{
	(*inStream) >> mMaxUndoSteps;
	mNumActionsInProgress=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Clears the stacks of undo and redo objects
CMultiUndoer::~CMultiUndoer()
{
	ClearUndoStack();
	ClearRedoStack();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PopUndoAction
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes the last undoable action from the stack and returns it, returns 0L if there are no items on the stack
LAction *CMultiUndoer::PopUndoAction()
{
	LAction			*action=0L;
	
	mUndoStack.FetchItemAt(mUndoStack.GetCount(),action);
	mUndoStack.RemoveItemsAt(1,mUndoStack.GetCount());
	
	if (action && !mUndoStack.GetCount())
		LCommander::SetUpdateCommandStatus(true);

	return action;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PopRedoAction
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Removes the last redoable action from the stack and returns it
LAction *CMultiUndoer::PopRedoAction()
{
	LAction			*action=0L;
	
	mRedoStack.FetchItemAt(mRedoStack.GetCount(),action);
	mRedoStack.RemoveItemsAt(1,mRedoStack.GetCount());

	if (action && !mRedoStack.GetCount())
		LCommander::SetUpdateCommandStatus(true);

	return action;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PeekUndoAction
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the last undo action without removing it from the stack.
LAction *CMultiUndoer::PeekUndoAction()
{
	LAction			*action=0L;
	
	mUndoStack.FetchItemAt(mUndoStack.GetCount(),action);

	return action;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PeekRedoAction
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the last redo action without removing it from the stack.
LAction *CMultiUndoer::PeekRedoAction()
{
	LAction			*action=0L;
	
	mRedoStack.FetchItemAt(mRedoStack.GetCount(),action);

	return action;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PushUndoAction									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pushes an undoable action onto the stack, if the undo limit is exceeded then the oldest items are thrown away.
void CMultiUndoer::PushUndoAction(LAction *inAction)
{
	mUndoStack.AddItem(inAction);
	
	if (mUndoStack.GetCount()==1)
		LCommander::SetUpdateCommandStatus(true);

	// Check if we've gone over the max undo steps. If we have it will only be by one, as only one item is pushed at a time.
	if (mUndoStack.GetCount()>mMaxUndoSteps)
	{
		// Actions can only be discarded if they aren't being executed. An action will be being executed if it's Redo
		// proc has posted extra actions. The number of actions on the stack which are non deleteable (as they are in progress)
		// is stored in mNumActionsInProgress. If mNumActionsInProgress==mMaxUndoSteps then you can't post as the max
		// steps has been hit and you can't do anything about it. If this becomes a problem look at the collating undoer
		// instead.
		if (mMaxUndoSteps==mNumActionsInProgress)
			Throw_(insufficientStackErr);		// This error code is quite close to what we want. The undo stack is full.
	
		// Delete the first action in the stack (which is the oldest item)
		LAction			*oldestAction;
		
		mUndoStack.FetchItemAt(1,oldestAction);
		mUndoStack.RemoveItemsAt(1,1);
		
		delete oldestAction;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PushRedoAction									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Pushes a redoable action onto the stack
void CMultiUndoer::PushRedoAction(LAction *inAction)
{
	mRedoStack.AddItem(inAction);
	
	if (mRedoStack.GetCount()==1)
		LCommander::SetUpdateCommandStatus(true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ExecuteSelf										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Intercepts the postaction messsage and records it.
// Intercepts the undo and redo messages and performs them.
// Intercepts the find command status messages for undo and redo and processes them.
void CMultiUndoer::ExecuteSelf(MessageT inMessage,void *ioParam)
{
	switch (inMessage)
	{
		case msg_PostAction:		// New Action to perform
			PostAction(static_cast<LAction*>(ioParam));
			SetExecuteHost(false);	// If the action was performed OK then don't execute the host
			break;
		
		case msg_CommandStatus:
			{
				// Enable and set text for "undo" and "redo" menu items
				SCommandStatus *status = static_cast<SCommandStatus*> (ioParam);
			
				if (status->command==cmd_Undo)
				{
					FindUndoStatus(status);
					SetExecuteHost(false);
				}
				else if (status->command==cmd_Redo)
				{
					FindRedoStatus(status);
					SetExecuteHost(false);				
				}
				else
					SetExecuteHost(true);
			}
			break;
		
		case cmd_Undo:				// Undo the action
			DoUndo();
			SetExecuteHost(false);
			break;

		case cmd_Redo:				// Redo the action
			DoRedo();
			SetExecuteHost(false);
			break;

		default:
			SetExecuteHost(true);
			break;
	}
}


// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DoRedo											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs a redo from the stack and then puts it on the undo stack.
// Throws if there's an error peforming the op such as empty stack.
void CMultiUndoer::DoRedo()
{
	LAction			*redoAction=PopRedoAction();
	
	ThrowIfNil_(redoAction);

	Try_
	{
		StValueChanger<bool>	blockAction(sWarnOnPostAction,true);
	
		redoAction->Redo();
	}
	Catch_(err)
	{
		// Error, add it back to the redo stack
		PushRedoAction(redoAction);
		throw;
	}
	
	// Once undone the action is moved to the other stack
	PushUndoAction(redoAction);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DoUndo											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Performs an undo from the stack and then puts it on the undo stack.
// Throws if there's an error peforming the op such as empty stack.
void CMultiUndoer::DoUndo()
{
	LAction			*undoAction=PopUndoAction();
	
	ThrowIfNil_(undoAction);

	Try_
	{
		StValueChanger<bool>	blockAction(sWarnOnPostAction,true);

		undoAction->Undo();
	}
	Catch_(err)
	{
		// Error, add it back to the undo stack
		PushUndoAction(undoAction);
		throw;
	}
	
	// Once undone the action is moved to the other stack
	PushRedoAction(undoAction);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ PostAction										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called to perform and record an action. If the action is non-recordable then the undo stack is cleared.
// When an action is posted the redo stack is cleared.
// This differs from LUndoer in that it doesn't call Finalize for LActions before deleting them.
// Passing a value of nil commits the last action, this means it clears the undo stack.
// A little trick is needed here as there is a potential problem
// The action must be stored onto the undo stack before it's executed. This is because executing it may generate further
// actions which too must be stored - after the original. If however too many actions are pushed from inside the original
// action execution - the original action could get pushed off the Undo stack and deleted before it returns. This is bad.
// To get around this the number of actions which are currently stacked recursively is monitored. If this reaches the
// max undo steps then no more undo actions can be discarded.
// There is an additional check to check actions aren't getting posted from inside an executing action which has already
// been run once. Actions should only emit further actions on their first application. After the first application there
// could be other actions following it and emitting more would clear these actions - hence you should only emit them on
// first application.
void CMultiUndoer::PostAction(LAction *inAction)
{
	if (sWarnOnPostAction)
		SignalPStr_("\pCMultiUndoer::PostAction() - An sub action has been spawned by an action which has already been applied once.");

	StValueChanger<ArrayIndexT>		changer(mNumActionsInProgress,mNumActionsInProgress+1);

	bool		wasPushedOntoUndo=false;
	
	// First try and pust the action onto the stack. The action is recorded before it's executed because if calling Redo
	// generates more actions they must be stored after this one.
	if (inAction && inAction->IsPostable())
	{
		// Add the action to the undo stack and we're done
		PushUndoAction(inAction);
		wasPushedOntoUndo=true;
	}
	else
	{
		// Action is not postable so clear the undo stack, we can't do ANY previous undos now as the program state
		// can't be restored.
		ClearUndoStack();
	}
	
	Try_
	{
		// Perform the action then record it
		if (inAction)
			inAction->Redo();
	}
	Catch_(err)
	{
		// Error on executing action - remove from the undo queue and delete it
		if (wasPushedOntoUndo)
			PopUndoAction();
			
		delete inAction;
		throw;
	}
	
	ClearRedoStack();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindUndoStatus									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used to find the state of undo.
void CMultiUndoer::FindUndoStatus(SCommandStatus *ioStatus)
{
	// Is there's an action available?
	*ioStatus->enabled=mUndoStack.GetCount()!=0;
	
	if (*ioStatus->enabled)
	{
		Str255	dummyString;
		
		PeekUndoAction()->GetDescription(dummyString,ioStatus->name);
	}

	if (!(*ioStatus->enabled)) // Set text to "Can't Undo"
		::GetIndString(ioStatus->name, STRx_UndoEdit, str_CantRedoUndo);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FindRedoStatus									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Used to find the state of redo.
void CMultiUndoer::FindRedoStatus(SCommandStatus *ioStatus)
{
	// Is there's an action available?
	*ioStatus->enabled=mRedoStack.GetCount()!=0;
	
	if (*ioStatus->enabled)
	{
		Str255	dummyString;
		
		PeekRedoAction()->GetDescription(ioStatus->name,dummyString);
	}

	if (!(*ioStatus->enabled)) // Set text to "Can't Redo"
		::GetIndString(ioStatus->name, STRx_RedoEdit, str_CantRedoUndo);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearUndoStack
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Deletes all the LActions in the undo stack and clears it.
void CMultiUndoer::ClearUndoStack()
{
	TArrayIterator<LAction*>			indexer(mUndoStack);
	LAction								*action;
	
	while (indexer.Next(action))
		delete action;

	mUndoStack.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ClearRedoStack
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Deletes all the LActions in the redo stack and clears it.
void CMultiUndoer::ClearRedoStack()
{
	TArrayIterator<LAction*>			indexer(mRedoStack);
	LAction								*action;
	
	while (indexer.Next(action))
		delete action;

	mRedoStack.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetMaxUndoSteps
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Gets the maximum number of undo steps which is stored at any time.
ArrayIndexT CMultiUndoer::GetMaxUndoSteps()
{
	return mMaxUndoSteps;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetMaxUndoSteps
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets an upper limit on the number of undo steps stored at any time. It discards undo steps if there are more than is
// specified by the new max.
void CMultiUndoer::SetMaxUndoSteps(ArrayIndexT inNewMax)
{
	mMaxUndoSteps=inNewMax;
	
	if (mUndoStack.GetCount()>mMaxUndoSteps)
	{
		// Delete some
		TArrayIterator<LAction*>			indexer(mUndoStack);
		LAction								*action;
		ArrayIndexT							numToDelete=mUndoStack.GetCount()-mMaxUndoSteps;
		
		while (indexer.Next(action) && numToDelete--)
			delete action;
			
		mUndoStack.RemoveItemsAt(mUndoStack.GetCount()-mMaxUndoSteps,1);
	}
}





