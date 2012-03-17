/***************************************************************************************************

BLAST Images.c
Extension of BLASTª engine
Mark Tully
15/3/96
3/4/96
30/4/96

This provides procedures for manipulating image data for your sprites handling all the banks 
and all the memory management issues which people like to avoid. It was going to be part of the
file BLAST Sprites.c but the image section got too fat.

Explaination of the routine is provided before each one in this case as the image managment
routines are many.
***The basic theory is as follows.***
Have a pointer to a list of banks. This pointer is gBankList.
At the beginning of the game, this is inited to point to a list of bank slots (which will be
empty). This is done by InitBanks().
Each bank is allocated and it's address stored in the relavent slot. The slot is of course the
bank number. The allocating is done by AllocateBank(). Once the bank is allocated, images can
be put into it. The bank is of finite size and can only store so many images. Errors will
happen when the bank is full, not crashes. The image storage engine is pretty strict on error
checking. Each image take up a slot inside the bank. These images are then used to display the
sprite.
Images are stored in resources of type 'IMAG' although the constant kImageResType should be used.
Entire banks can be stored as resources of type 'BANK' (use kBankResType). The banks can be
edited in ResEdit using the BANK template. The bank resource merely lists all the images in
the same resource file which that bank consists of. The bank resource itself contains NO IMAGE
DATA. The entire bank can be loaded using LoadBankFromDisk() which will load the bank resource
and each image in the bank resource into the bank of your choice. The bank (in memory that is)
will be sized to the correct size although the option of appending the images is availible.
See the discription under LoadBankFromDisk().

Very bad news to have one IMAG resource in more than one bank simultaneously, very bad news...
This is because as the resource manager is used to manage the image handles, releasing the
image from the first bank will release it from the others, but it WON'T BE REGISTERED AS RELEASED
which means that when the other banks are disposed of that relased image will be released again
which will result in a crash. If the same image is being used in two banks it is wasteful
anyway so whats the problem? If it is absolulely necessary you'll have to dupe the resources
and load a different copy into each bank.
Oh yeah, same goes for TINT resources.

SECTION 1 - Routines for manipulating banks
SECTION 2 - Routines for manipulating images

***************************************************************************************************/

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

#include	"BLAST.h"

// Globals
BankPtr			*gBankList=0L;
short			gMaxBanks=0,gBanksUsed=0;

// ********* SECTION 1 - Routines for manipulating banks *********

// Call this before using any image manipulation routines.
// Num is the maximum number of banks you wish to use.
// It can be called again if the max goes up but all banks currently in use will be disposed of.
// Every bank you init here only takes up 4 bytes to give yourself loads of them just don't
// allocate them all (See AllocateBank).
Boolean InitBanks(short num)
{
	if (gBankList) // already exists?
		DisposeBankList();
	gMaxBanks=num;
	gBanksUsed=0;
	gBankList=(BankPtr *)NewPtrClear(sizeof(BankPtr)*num);
	return (gBankList!=0L);
}

// Free up ALL memory used by this module (called by ShutDownBlast)
void ShutDownSpriteModule()
{
	DisposeBankList();
	DisposeSpriteList();
}

// Gets rid of all the banks and the bank list which was created by InitBanks
void DisposeBankList()
{
	DisposeAllBanks();
	if (gBankList)
		DisposePtr((Ptr)gBankList);
	gBankList=0L;
	gMaxBanks=0;
}

// Disposes of every bank by freeing up all images in the bank then releasing it's memory
void DisposeAllBanks()
{
	short count;

	for (count=0; count<gMaxBanks; count++)
		DisposeBank(count);
}

// Allocates space in bank bankNum for a number of images. If the bank has already been allocated
// it is disposed of (all images go) and allocated at the new size. (beware the memory frag).
// passing -1 as the bankNum means allocate in the next free bank. The bank number is then
// returned. A return of -1 means failiure.
short AllocateBank(short bankNum,short images)
{	
	if (gBanksUsed>=gMaxBanks)
		return -1;
		
	if (!gBankList)
		return -1;
	
	if (images==0)
		return -1;
	
	if (bankNum==-1)
		bankNum=NextFreeBank();
		
	// if the bank exists, release it, otherwise allocate it
	if (gBankList[bankNum])
		DisposeBank(bankNum); // NB: gBanksUsed has just dropped by 1
		
	gBankList[bankNum]=(BankPtr)NewPtrClear(sizeof(Bank));
	if (!gBankList[bankNum])
		return -1;
	
	gBankList[bankNum]->maxImages=images;
	// allocate the image memory aswell
	gBankList[bankNum]->imageList = (ImageHandle *)NewPtrClear(sizeof(ImageHandle)*images);
	if (!gBankList[bankNum]->imageList)
	{
		DisposePtr((Ptr)gBankList[bankNum]);
		gBankList[bankNum]=0L;
		return -1;
	}
	
	gBanksUsed++;
	
	return bankNum;
}

// Finds the first empty bank by walking the bank list
short NextFreeBank()
{
	short	bankNum;
	
	if (!gBankList)
		return -1;
	
	if (gBanksUsed>=gMaxBanks)
		return -1;
	
	// search through bank list for a free bank
	for (bankNum=0; bankNum<gMaxBanks; bankNum++)
	{
		if (!gBankList[bankNum]) // free slot? then stop...
			break;
	}
		
	return bankNum;
}

// Disposes of the specifed bank by releasing all images then freeing it's memory and marking the
// bank slot as free. Returns false if the bankNum was invalid.
Boolean DisposeBank(short bankNum)
{
	short count;
	
	if (!gBankList)
		return false;
		
	if (bankNum>=gMaxBanks || !gBankList[bankNum])
		return false;
		
	for (count=0; count<gBankList[bankNum]->maxImages; count++)
		DisposeImage(bankNum,count);		// Dispose of each image in the bank
	
	if (gBankList[bankNum]->bankTint) // release the tint
	{
		BetterDisposeHandle((Handle)gBankList[bankNum]->bankTint);
		gBankList[bankNum]->bankTint=0L;
	}
	
	DisposePtr((Ptr)gBankList[bankNum]->imageList);
	DisposePtr((Ptr)gBankList[bankNum]);
	gBankList[bankNum]=0L;
	gBanksUsed--;

	return true;
}

// Swaps two bank pointers around in the bankList so that to all intents all images have moved
// bank. bankA would then be known as bankB and vice versa.
// It returns false if there was a parameter error. True for success. Not both banks have to
// have been allocated but they must both be a valid bank (ie within the number of banks
// Allocated by InitBanks())
Boolean SwapBanks(short bankA,short bankB)
{
	BankPtr		temp;
	
	if (!gBankList)
		return false;
	
	if (bankA>=gMaxBanks)
		return false;
		
	if (bankB>=gMaxBanks)
		return false;
	
	temp=gBankList[bankA];
	gBankList[bankA]=gBankList[bankB];
	gBankList[bankB]=temp;
	return true;
}

// returns how many images are in a bank, 0 if bank is invalid or empty
short ImagesInBank(short bankNum)
{
	if (!gBankList)
		return 0;
		
	if (bankNum>=gMaxBanks)
		return 0;
		
	if (!gBankList[bankNum])
		return 0;

	return gBankList[bankNum]->usedImages;
}

// High level routine to load an entire bank from a resource
// The boolean append allows you to add images to already existing bank.
// if appending, a -1 will be returned if the bank is full but some images will have been appended
// appending will slot the sprite images into the next availible space in the bank, so if you
// have blanks in the current bank before appending then these will be filled before more are
// created. This is probably a bad thing for you as you won't know where you're images are
// unless you know which ones are free before hand etc.
// If not appending then the bank will be disposed of if it exists.
// -1 CANNOT be passed as the bankNum in this case. (If you really need to place it in the next
// availible bank use NextFreeBank() and pass the result to this as the bank pos)
// Disk Bank structure
// List of shorts
// 0	:	Bank type
// 1	:	Number of shorts after this one
// 2	:	Either the first image to load in if frag loading or the lower bounds of the images
//			to load in if continuous loading.
// 3	:	Either the second image if frag loading or the upper boundry if continuous loading
// 4+	:	Only used if frag loading. These are the rest of the images in the bank.

Boolean LoadBankFromDisk(short resID,short bankNum,Boolean append)
{
	DiskBankHandle		diskBank=0L;
	short				count,numInBank;
	
	if (!gBankList)
		return false;
		
	if (bankNum>=gMaxBanks)
		return false;
		
	if (!append && gBankList[bankNum])	// already exists, replace
		DisposeBank(bankNum);

	// Get the bank resource
	diskBank=(DiskBankHandle)Get1Resource(kBankResType,resID);
	if (!diskBank)
		return false;
	
	switch ((*diskBank)[0]) 	// bank[0] holds type
	{
		case kLoadContinuous: // load images between bank[2] and [3]
	
			numInBank=((*diskBank)[3]-(*diskBank)[2])+1; // +1 as it's inclusive of start and finish IDs
	
			// allocate the bank to put the images in
			if (AllocateBank(bankNum,numInBank)==-1)
			{
				ReleaseResource((Handle)diskBank);
				return false;
			}
			
			for (count=0; count<numInBank; count++)
			{
				if (LoadImageIntoBank((*diskBank)[2]+count,bankNum,-1)==-1) // load start image + offset
				{
					ReleaseResource((Handle)diskBank);
					return false;
				}
			}
			break;
			
		case kLoadFragmented:
			
			numInBank=(*diskBank)[1]; // number of images in the bank
			// allocate the bank to put the images in
			if (AllocateBank(bankNum,numInBank)==-1)
			{
				ReleaseResource((Handle)diskBank);
				return false;
			}
			
			for (count=0; count<numInBank; count++)
			{
				if (LoadImageIntoBank((*diskBank)[2+count],bankNum,-1)==-1)
				{
					ReleaseResource((Handle)diskBank);
					return false;
				}
			}
			break;
		
		default:
			ReleaseResource((Handle)diskBank);
			return false;
	
	}
	
	ReleaseResource((Handle)diskBank);
	return true;
}

// ********* SECTION 2 - Routines for manipulating images *********

// finds the next free image in the specified bank by walking the image list of the bank
short NextFreeImage(short bank)
{
	short	image;
	
	if (!gBankList)
		return -1;
	
	if (bank>=gMaxBanks || !gBankList[bank])
		return -1;
	
	if (gBankList[bank]->usedImages>=gBankList[bank]->maxImages)
		return -1; // none free

	// search through bank list for a free bank
	for (image=0; image<gBankList[bank]->maxImages; image++)
	{
		if (!gBankList[bank]->imageList[image]) // free slot? then stop...
			break;
	}
	
	return image;
}

// Releases the memory taken up by the image in the specified bank. The image slot is marked as
// free and other images can be loaded into that slot. If the image does not exist then it does
// nothing.
void DisposeImage(short bank,short image)
{	
	if (!ImageExists(bank,image))
		return;
		
	//HUnlock((Handle)gBankList[bank]->imageList[image]);
	ReleaseResource((Handle)gBankList[bank]->imageList[image]);
	gBankList[bank]->imageList[image]=0L;
	gBankList[bank]->usedImages--;
}

// Loads an image from the first resource file in the resource chain (see CurResFile() in TR).
// if it was not found or could not be loaded a 0L is returned.
ImageHandle	LoadImage(short resID)
{
	ImageHandle		hello=(ImageHandle)Get1Resource(kImageResType,resID);

	if (!hello)
		return 0L;
	
	//HLockHi((Handle)hello); // Lock it in high mem to stop mem frag
	return hello;
}

// Swaps two images about either cross bank or in the same bank. A valid image can be swapped
// with an empty image. Both banks must be legal (ie have been allocated). Returns false if there
// was a parameter error.
Boolean SwapImages(short bankA,short imageA,short bankB,short imageB)
{
	ImageHandle		temp;
	// can't use imageExists to check for existance of images as want to be able to swap nil images
	// around (which don't exist). Just check they're legal.
	if (!gBankList)
		return false;
		
	if (bankA>=gMaxBanks || (!gBankList[bankA]))
		return false;
	
	if (bankB>=gMaxBanks || (!gBankList[bankB]))
		return false;
		
	if (imageA>=gBankList[bankA]->maxImages)
		return false;
	
	if (imageB>=gBankList[bankB]->maxImages)
		return false;

	// First work out which usedImage counts are going up, down, or staying the same
	if (gBankList[bankA]->imageList[imageA] && !gBankList[bankB]->imageList[imageB]) // if a exists and b doesn't
	{
		gBankList[bankA]->usedImages--;
		gBankList[bankB]->usedImages++;
	}
	else if (gBankList[bankB]->imageList[imageB] && !gBankList[bankA]->imageList[imageA]) // if b exists and a doesn't
	{
		gBankList[bankB]->usedImages--;
		gBankList[bankA]->usedImages++;
	}
	// if they both exist or both don't exist then no change to usedImage counters
	
	// do the swap
	temp=gBankList[bankA]->imageList[imageA];
	gBankList[bankA]->imageList[imageA]=gBankList[bankB]->imageList[imageB];
	gBankList[bankB]->imageList[imageB]=temp;
		
	return true;
}

// Loads an image from the first res file into the bank specifed at the position specified.
// if the image already exists, it is disposed of. Then replaced. A -1 can be passed as the
// image pos but not as the bank. A -1 is returned if the load was not sucessful for any reason.
// Otherwise the pos the image was loaded into is returned.
short LoadImageIntoBank(short resID,short bank,short pos)
{
	short		ret=-1;
	ImageHandle	temp=LoadImage(resID);
	
	if (temp)
	{
		ret=PutImageInBank(bank,pos,temp);
		if (ret==-1)
		{
			//HUnlock((Handle)temp);
			ReleaseResource((Handle)temp);
		}
	}
	
	return ret;
}

// Places the imageHandle (obtained from load image) into the bank specified at the pos specified
// if the image already exists in that pos it is disposed of. A pos of -1 means next availible
// slot in the bank. Returns -1 for error (bank full/image is null) else it returns the image
// position.
// cannot pass -1 as bank, but can as image
short PutImageInBank(short bank,short pos,ImageHandle theImage)
{	
	Boolean		added=false;

	if (!theImage)
		return -1;
			
	if (!gBankList)		// allocated the banks yet?
		return -1;		// you fool!
		
	if (bank>=gMaxBanks || (!gBankList[bank]))
		return -1;

	if (pos==-1)
	{
		if (gBankList[bank]->usedImages >= gBankList[bank]->maxImages) // only check if adding
			return -1;
		
		// otherwise, next free space
		pos=NextFreeImage(bank);
		added=true;
	}

	if (gBankList[bank]->imageList[pos]) // taken up?
		DisposeImage(bank,pos); // replace it
	
	// right we have space
	gBankList[bank]->imageList[pos]=theImage;
	
	if (added)
		return gBankList[bank]->usedImages++; // added a new image or replaced an old one
	else
		return gBankList[bank]->usedImages;
}

// Fetched an image from the bank specified. 0L is returned if it doesn't exist.
// if detach it is specifed then the ImageHandle (which is property of the resource manager)
// is detached and it's entry in the bank is set to 0L.
// you specify detach then it is up to you to dipose of the returned handle with DisposeHandle
ImageHandle	GetImageFromBank(short bank,short image,Boolean detachIt)
{
	ImageHandle	moo=0L;
	
	if (!ImageExists(bank,image))
		return 0L;
		
	moo=gBankList[bank]->imageList[image];

	if (detachIt)
	{
		DetachResource((Handle)moo);
		gBankList[bank]->imageList[image]=0L;
		gBankList[bank]->usedImages--;
	}
		
	return moo;
}


// Returns true is the image in that bank is taken
// returns false if it doesn't exist or if there is a param error (bank not exist/no allocated
// or image out of range)
Boolean ImageExists(short bank,short image)
{
	if (!gBankList)
		return false;
		
	if (bank>=gMaxBanks || (!gBankList[bank]))
		return false;
		
	if (image>=gBankList[bank]->maxImages)
		return false;
	
	if (!gBankList[bank]->imageList[image])
		return false;
		
	return true;
}

// ********* SECTION 3 - Routines for manipulating Tints

short LoadTintIntoBank(short resID,short bank)
{
	TintHandle	temp=0L;
	
	if (!gBankList)		// allocated the banks yet?
		return -1;		// you fool!
		
	if (bank>=gMaxBanks || (!gBankList[bank]))
		return -1;
	
	temp=LoadTint(resID);
	
	if (temp)
	{
		// if there was already one there get rid of it
		if (gBankList[bank]->bankTint && gBankList[bank]->bankTint!=temp)
		{
//			HUnlock((Handle)gBankList[bank]->bankTint);
			ReleaseResource((Handle)gBankList[bank]->bankTint);
		}
		gBankList[bank]->bankTint=temp;
	}
	else
		return -1;
	
	return 0; // ok
}

TintHandle LoadTint(short resID)
{
	TintHandle		hello=(TintHandle)Get1Resource(kTintResType,resID);

	if (!hello)
		return 0L;
	
//	HLockHi((Handle)hello); // Lock it in high mem to stop mem frag
	return hello;
}
