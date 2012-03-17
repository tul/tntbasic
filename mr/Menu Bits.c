// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Menu Bits.c
// Mark Tully
// 30/3/96
// 4/4/97	: Renamed it to Menu Bits.c from Menus.c
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include	"Marks Routines.h"

#define		kAppleMenuRes	128	// Always make your apple 'MENU' resource 128

// This routines returns whether an item is enabled or not. It can only cope with items up to 31
// item zero is the entire menu when running on non-carbon targets
Boolean GetMenuItemState(MenuHandle handle,short itemId)
{
#if TARGET_API_MAC_CARBON
	return IsMenuItemEnabled(handle,itemId);
#else
	Boolean		enabled=true;

	if (itemId<32)
		enabled=((**handle).enableFlags & (1L << itemId)) != 0;

	return enabled;
#endif
}

Boolean FetchTheBar(short theBar)
{	
	MenuHandle		temp=0L;
	Handle			tempH=0L;
	
	tempH=GetNewMBar(theBar);
	if (!tempH)
		return false;
		
	SetMenuBar(tempH);
	DrawMenuBar();

	temp=GetMenuHandle(kAppleMenuRes);
	if (temp)
		AppendResMenu(temp,'DRVR'); // add all the usual bits to the apple menu
	else
		return false;
		
	return true;
}

void ClearMenu(MenuHandle theMenu)
{
	while(CountMenuItems(theMenu))
		DeleteMenuItem(theMenu,1);
}

void SetMenuItem(short resID,short pos,StringPtr newName,Boolean enabled)
{
	MenuHandle	theMenu=0L;
	
	theMenu=GetMenuHandle(resID);
	if (theMenu)
	{
		SetMenuItemText(theMenu,pos,newName);
#if TARGET_API_MAC_CARBON
		if (enabled)
			EnableMenuItem(theMenu,pos);
		else
			DisableMenuItem(theMenu,pos);
#else
		if (enabled)
			EnableItem(theMenu,pos);
		else
			DisableItem(theMenu,pos);
#endif
	}
}

// deselects 
void Mark1MenuItem(MenuHandle menu,short item)
{
	short	count;

	for (count=1; count<=CountMenuItems(menu); count++)
		CheckMenuItem(menu,count,item==count);
}

// if you're going to be changing menu items, make sure they're non purgable or you'll keep losing the changes
MenuHandle InsertSubMenu(short resID)
{
	MenuHandle	temp=0L;
	
	temp=GetMenu(resID);
	if (temp)
		InsertMenu(temp,hierMenu);
	return temp;
}

// appends to a menu ignoring control codes
void BetterAppendMenu(MenuHandle handle,StringPtr string)
{
	AppendMenu(handle,"\p ");
	SetMenuItemText(handle,CountMenuItems(handle),string);
}

// inserts an item into a menu ignoring control codes
void BetterInsertMenuItem(MenuHandle menu,StringPtr text,short afterItem)
{
	InsertMenuItem(menu,"\p ",afterItem);
	SetMenuItemText(menu,afterItem+1,text);
}