
//
//	RLCococaDisplay.cpp
//
//  © Ian R Ollmann (iano@cco.caltech.edu)	2000 - 2002
//		All Rights Reserved
//
//	Please see accompanying documentation for duplication, distribution and licensing terms.
//
//

// Modified by Mark Tully 11/6/06 to work directly with Mach rather than doing so indirectly via function ptrs.

#define VERBOSE_LOGGING false

#include "RLCocoaMachODisplay.h"
#include <CFBundle.h>
#include <CFNumber.h>
#include <string.h>
#include <stdio.h>
#include <Multiprocessing.h>
#include <math.h>
#include <limits.h>
#include <Movies.h>

#if TARGET_RT_BIG_ENDIAN
#include <mach/ppc/thread_act.h>
#else
#include <mach/i386/thread_act.h>
#endif

#define ADD_MOUSE_FEATURES 	0

#define kMyDisplaySafeMode 				0x00000001
#define kMyDisplayDefaultMode 			0x00000002
#define kMyDisplaySafeForFullScreenMode 0x20000000

//Utility functions
double AbsoluteToSeconds( AbsoluteTime time );
AbsoluteTime SecondsToAbsolute( double seconds );

#define kUseRoundRobinScheduling        FALSE

OSStatus ExtractModeDictionaryInfo( const CFDictionaryRef mode, RLInfo *info );
kern_return_t  GetStdThreadSchedule( mach_port_t    machThread,
                                     int            *priority,
                                     boolean_t      *isTimeshare );

kern_return_t  RescheduleStdThread( mach_port_t    machThread,
                                    int            newPriority,
                                    boolean_t      isTimeshare );


//pascal Boolean	RLDialogFilterFunction( DialogRef theDialog, EventRecord *theEvent, DialogItemIndex *itemHit);


//The hack:
//
//Here I have done something extremely naughty. I have hard coded the strings to hold the same
//value as those defined extern in CGDirectDisplay.h. This will break if Apple changes those names.
//The problem is that in order to properly import them from the OS, a separate mach-o based 
//library has to be written. This opens us up a host of mach-o lib not found type problems. 
//The informal word from the CoreGraphics team is that these constants are not likely to change
//in the future. Thus, my take on the problem is that this is a shared lib for a reason. If an
//OS revision appears that breaks the current revision, a new shlib can quickly be put out that resolves
//the problem. In the mean time, chances are there will be fewer support calls with this approach 
//which allows for a single library. (Also, somewhat cynically put: Apple will take the fall instead 
//of you if a new OS Rev appears to be the culprit. >;-)
//
// Note 3/15/01: According to comments in CGDirectDisplay.h, this hack is now the *official*
// way of doing things.
//
CFStringRef kRLDisplayWidth = CFSTR("Width");
CFStringRef kRLDisplayHeight = CFSTR("Height");
CFStringRef kRLDisplayMode = CFSTR("Mode");
CFStringRef kRLDisplayBitsPerPixel = CFSTR( "BitsPerPixel" );
CFStringRef kRLDisplayRefreshRate = CFSTR("RefreshRate");
CFStringRef kRLDisplayIOFlags = CFSTR( "IOFlags" );

CFBundleRef		appServicesBundle = NULL;
CFBundleRef		infoExtractorBundle = NULL;


//
// LoadDirectDisplayAPI loads in the CoreGraphics framework and acquires function pointers to most of hte
// direct display API (see CGDirectDisplay.h)
//
OSStatus LoadDirectDisplayAPI( const FSSpec &ourLocation )
{
    return noErr;    
}



// This releases the CoreGraphics bundle
void CleanUpDirectDisplayAPI( void )
{
}

// Makes an array of Rect's, each of which corresponds to the bounding box for a display device
// in global coordinates
Rect *GetCocoaDisplayList( unsigned long *listEntryCount )
{
	const CGDisplayCount kMaxDisplayCount = 256;
	CGDirectDisplayID	displayList[kMaxDisplayCount];
	CGDisplayCount		actualCount = 0;
	
	//Get a copy of the list of displays
	CGDisplayErr  err = CGGetActiveDisplayList( kMaxDisplayCount, displayList, &actualCount );
	if( err )
		DEBUGMESSAGE( "Got error # " << err << " attempting to fetch the active display list from CoreGraphics." );
	
	Rect	*array = NULL;

	//Allocate an array to hold the bounding rectangles
	if( actualCount > 0 )
		array = (Rect *) NewPtr( sizeof( Rect ) * actualCount );
	
	//Copy the bounding rectangles into the array
	if( array )
	{
		for( int i = 0; i < actualCount; i++ )
		{	
			CGRect size = CGDisplayBounds(displayList[i]);
			array[i].top = (short)size.origin.y;
			array[i].left = (short)size.origin.x;
			array[i].bottom = (short)size.size.height + array[i].top;
			array[i].right = (short)size.size.width + array[i].left;
		}
	}
	else
		DEBUGMESSAGE( "Out of memory error when trying to fetch a list of displays." );
		
	//Record the size of the array
	if( listEntryCount )
		*listEntryCount = actualCount;

	return array;
}

// Frees an array allocated by GetCocoaDisplayList
void DisposeCocoaDisplayList( Rect *list )
{
	if( list )
		DisposePtr( Ptr( list ) );
}


#pragma mark -

//
//	ExtractModeDictionaryInfo takes a CFDictionaryRef to a display mode and 
//	copies the data from it into an RLInfo struct for easier use
//
//
OSStatus ExtractModeDictionaryInfo( const CFDictionaryRef mode, RLInfo *info )
{
	//Zero the input struct
	memset( info, 0, sizeof( *info ) );

	//Extract the data that we need from the dictionary
	CFNumberRef data = (const CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayMode ); 
	bool success = false;
	if( data )
	{
		success = CFNumberGetValue( data, kCFNumberSInt32Type, &info->resolutionID );
		if( ! success )
			DEBUGMESSAGE( "Failed to extract resolution mode. RezLib may need to be updated." );
	}

	data = (CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayRefreshRate ); 
	if( data )
	{
		float	refresh = 0;
		success = CFNumberGetValue( data, kCFNumberFloat32Type, &refresh );
		if( success )
			info->refreshRate = UInt32(refresh * 65536.0);
		else
			DEBUGMESSAGE( "Failed to extract refresh rate. RezLib may need to be updated." );
	}	
	
	data = (CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayHeight ); 
	if( data )
	{
		success = CFNumberGetValue( data, kCFNumberSInt32Type, &info->height );
		if( ! success )
			DEBUGMESSAGE( "Failed to extract resolution height. RezLib may need to be updated." );
	}
	
	data = (CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayWidth ); 
	if( data )
	{
		success = CFNumberGetValue( data, kCFNumberSInt32Type, &info->width );
		if( ! success )
			DEBUGMESSAGE( "Failed to extract resolution width. RezLib may need to be updated." );
	}

	data = (CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayBitsPerPixel ); 
	if( data )
	{
		success = CFNumberGetValue( data, kCFNumberSInt32Type, &info->colorDepths );
		if( ! success )
			DEBUGMESSAGE( "Failed to extract bitdepth. RezLib may need to be updated." );
	}
	
	data = (CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayIOFlags );
	if( data )
	{
		SInt32 flags = 0;
		success = CFNumberGetValue( data, kCFNumberSInt32Type, &flags );
		if( ! success )
			DEBUGMESSAGE( "Failed to extract video mode flags. RezLib may need to be updated." );
	
 		if( flags & kDisplayModeStretchedFlag )
 			info->flags |= rlDisplayModeIsStretched;
 
 		if( flags & kDisplayModeRequiresPanFlag )
 			info->flags |= rlDisplayModeRequresPanning;
 		
 		if( flags & kDisplayModeInterlacedFlag )
 			info->flags |= rlDisplayModeInterlaced;
 		
// 		if( flags & kDisplayModeSafeFlag )
		if( flags & kMyDisplaySafeMode || flags & kMyDisplaySafeForFullScreenMode )
 			info->isSafe = true;
 		else
 			info->isSafe = false;
 	}
 	else
 	{
 		DEBUGMESSAGE( "Failed to retrieve video mode flags for this resolution. Whether the mode is safe is unknown." );
	}

	//Since the API doesn't give us a name for the resolution, we prepare one ourselves
	char temp[256] = "";
	if( info->refreshRate )
		sprintf( temp, "%i x %i, %1.1f Hz", info->width, info->height, double( info->refreshRate) / 65536.0 );
	else
		sprintf( temp, "%i x %i", info->width, info->height );
	int length = strlen( temp );
	if( length > 31 )
		length = 31;
	BlockMove( temp, info->name + 1, length );
	info->name[0] = length;
	
	return noErr;
}



//
// AcquireDesktop hides all other windows, control strips, menu bars, docks and other whatnot from the screen
//
// Unfortunately, we dont have a GDHandle for our display, so we cant just use BeginFullScreen()
OSStatus CocoaDisplay::AcquireDesktop( WindowRef *window, const RGBColor *backgroundColor )
{
	const RGBColor black = {0,0,0};
	OSStatus error = noErr;

	//If we have already acquired the desktop, exit
	if( desktopRestoreContext )
	{
		DEBUGMESSAGE( "Error: Desktop found to be already acquired. " );
		return noErr;
	}
		
	//Set the background color to black if there is none
	if( ! backgroundColor )
		backgroundColor = &black;

	//Find the correct GDevice that matches those coords
	GDHandle classicDisplay = GetDisplay( &error );
	
	//We are probably ok if classicDisplay still holds NULL at this point, since that will go to the main display
	if( NULL == classicDisplay )
		DEBUGMESSAGE( "Warning: RezLib unable to identify the correct GDHandle for this display. Acquiring the main display. Error:" << error );
	
	//Do everything	
	error = BeginFullScreen( &desktopRestoreContext, classicDisplay, NULL, NULL, &blankingWindow, (RGBColor*) backgroundColor, fullScreenAllowEvents  );

	if( blankingWindow )
	{
		GrafPtr	oldPort;
		GetPort( &oldPort );
		SetPort( GetWindowPort( blankingWindow ) );
		RGBBackColor( backgroundColor );
		
		//Resize the window to match the size of the display -- OS X RC is a bit broken this way
		if( noErr == error )
			SizeWindow( blankingWindow, classicDisplay[0]->gdRect.right - classicDisplay[0]->gdRect.left, classicDisplay[0]->gdRect.bottom - classicDisplay[0]->gdRect.top, true ); 
		else
			DEBUGMESSAGE( "Got error # " << error << " trying to repair blanking window size. The Blanking window may be set to the wrong size." );

		SetPort( oldPort );
	}

	//Log the result to our debug file
	if( error )
		DEBUGMESSAGE( "Error: got error # " << error << " from BeginFullScreen()." );
	else
	{
		if( window )
			*window = blankingWindow;

		DEBUGMESSAGE( "Desktop acquired." );
	}
	return error;
	
}
/*	backgroundColor;
	if( (*RLDisplayIsCaptured)( display) )
		goto exit;
		
	//Capture the display -- hides 99.999% of the junk
	CGDisplayErr err = noErr; //(*RLDisplayCapture)( display);
	if( err )
	{
		DEBUGMESSAGE( "Got error # " << err << " attempting to capture the display. Acqure Desktop failed. " );
		return err;
	}
	
	CGrafPtr grafPtr = (*RLCreateNewPortForCGDisplayID)( (UInt32) display );
	blankingWindow = GetWindowForPort( grafPtr );
	
//Make a blanking window for compatability with MacOS classic.
	//Determine the bounds of the display in global coords
	CGRect screenBounds = (*RLDisplayBounds)(display);
	Rect	windowBounds;
	windowBounds.left = screenBounds.origin.x;
	windowBounds.top = screenBounds.origin.y;
	windowBounds.right = windowBounds.left + screenBounds.size.width;
	windowBounds.bottom = windowBounds.top + screenBounds.size.height;

	//Make a blanking window
	blankingWindow = NewCWindow( 	NULL, 			//allocate on the heap
									&windowBounds, 	//Use the bounds of the display for the window bounds
									"\p", 			//no window name
									false, 			//not visible initially  
									plainDBox,		//window has no borders or title bar (these might wanted into adjacent displays)
									WindowRef(-1L), //make the window front most
									false,			//no close box
									0				//Set the refcon to NULL
								);
								
	//Erase the window to the background color, or black if none is provided
	if( blankingWindow ) 
	{
		GrafPtr	oldPort = NULL;
		
		GetPort( &oldPort );
		SetPort( GetWindowPort(blankingWindow) );
		
		if( backgroundColor )
			RGBBackColor( backgroundColor );
		else
		{
			RGBColor black = {0,0,0};
			RGBBackColor( &black );
		}
		
		EraseRect( &windowBounds );
		SetPort( oldPort );
		ShowWindow( blankingWindow );
	}
	else
		DEBUGMESSAGE( "Failed to allocate a blanking window while acquiring the desktop!" );
exit:	
	//Return a pointer to the blanking window	
	if( window )
		*window = blankingWindow;
	
	return noErr;
*/

void CocoaDisplay::DisposeResolutionList( RLInfo *list ) const
{
	if( list )
		DisposePtr( Ptr( list ) );
}

OSStatus CocoaDisplay::DisposeVerticalRetraceProcedure( RetraceProc *proc )
{
	delete proc;
	return noErr;
}

// Copies low level drawing information for the current display setting to the RLDrawInfo struct
OSStatus CocoaDisplay::GetCurrentDrawingInfo( RLDrawInfo *info ) const
{
	memset( info, 0, sizeof( *info ) );

	CGRect bounds = CGDisplayBounds( display );
	info->bounds.left = (short)bounds.origin.x;
	info->bounds.top = (short)bounds.origin.y;
	info->bounds.right = short(bounds.origin.x + bounds.size.width );
	info->bounds.bottom = short(bounds.origin.y + bounds.size.height );
	info->rowBytes = CGDisplayBytesPerRow( display );
	info->bitsPerPixel = CGDisplayBitsPerPixel( display);
	info->baseAddr = (Ptr) CGDisplayBaseAddress( display );
	
	return noErr;
}

//Copies high level information about the current display setting to the RLInfo struct it takes as an argument
OSStatus CocoaDisplay::GetCurrentScreenSetting( RLInfo *info ) const
{
	RLInfo temp;

	if( ! info || info->rlInfoType != 0 )
		return paramErr;	

	//Get the current display mode
	CFDictionaryRef	currentMode = CGDisplayCurrentMode( display);
	if( ! currentMode )
	{
		DEBUGMESSAGE( "currentMode is NULL! Error." );

		return rlUnableToFetchModeList;
	}
	OSStatus err = ExtractModeDictionaryInfo( currentMode, info );
	if( err )
		return err;

	//Unfortunately, the RLInfo.resolutionID under OS X is a rather fuzzy concept.
	//There are actually several different CoreGraphics resolutions that correspond 
	//to the same resolutionID. These are typically different color depth settings.
	//The mode list array that we return publicly uses the rez ID for the first CoreGraphics
	//resolution as the resolutionID for the entire set of color depths. So that it 
	//matches what we return here, we cant just return the current mode that we just 
	//acquired above. We have to search the entire list for similar display modes that 
	//appear prior to it in the list and use that display ID instead.
	CFArrayRef modeList = CGDisplayAvailableModes( display);
	if( ! modeList )
		return rlUnableToFetchModeList;
	
	CFIndex count = CFArrayGetCount( modeList );
	for( int i = 0; i < count; i++ )
	{
		CFDictionaryRef dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );
		err = ExtractModeDictionaryInfo( dictionary, &temp );
		if
		( 
			! err										&&
			temp.height == info->height					&&
			temp.width == info->width					&&
			temp.refreshRate == info->refreshRate 		&&
			(temp.flags & rlDisplayModeMatchingBits) == (info->flags & rlDisplayModeMatchingBits)
		)
		{
			info->resolutionID = temp.resolutionID;
			break;	
		}
	}
	
	return err;
}

//
//	Returns the color depths available at this resolution and refresh rate in a UInt32 as a bit field.
//	If both 8 and 32 bit color are available, the result is 8 | 32 = 40.
//
UInt32 CocoaDisplay::GetColorDepthsAtThisResolution( void ) const
{
	UInt32	depth = 0;
	RLInfo currentSetting, info;
	
	memset( &currentSetting, 0, sizeof( RLInfo ) );
	memset( &info, 0, sizeof( RLInfo ) );
	
	//Read in the current setting so we know what resolution/refresh rate to be looking for
	OSStatus err = GetCurrentScreenSetting( &currentSetting );
	if( err )
		return 0;
	
	//Fetch a list of all available modes
	CFArrayRef modeList = CGDisplayAvailableModes( display);
	if( ! modeList )
	{
		DEBUGMESSAGE( "Unable to fetch a list of available display modes when checking the color depths available at this resolution. Returning 0 for available color depths.");
		return 0;
	}
	
	//Get the list of the mode list
	CFIndex count = CFArrayGetCount( modeList );
	
	//Iterate through the mode list
	for( int i = 0; i < count; i++ )
	{
		//Get the ith mode in the mode list
		CFDictionaryRef dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );
		
		//Read it
		err = ExtractModeDictionaryInfo( dictionary, &info );
		
		//If the mode matches our current height, width and refresh rate, add its color depth to the result
		if
		( 
			! err												&&
			currentSetting.height == info.height				&&
			currentSetting.width == info.width					&&
			currentSetting.refreshRate == info.refreshRate 		&&
			(currentSetting.flags & rlDisplayModeMatchingBits) == (info.flags & rlDisplayModeMatchingBits)
		)
			depth |= info.colorDepths;
	}
	
	return depth;
}

GDHandle CocoaDisplay::GetDisplay( OSStatus *error ) const
{
	GDHandle classicDisplay;
	const Boolean kActiveOnly = true;

	if( error != NULL )
		*error = noErr;	


	//Get the current screen location in global coordinates	
	RLDrawInfo screenData;
	OSStatus err = GetCurrentDrawingInfo( &screenData );
	if( err )
	{
		if( error != NULL )
			*error = err;
		return NULL;
	}

	for( classicDisplay = DMGetFirstScreenDevice(kActiveOnly ); classicDisplay; classicDisplay = DMGetNextScreenDevice( classicDisplay, kActiveOnly ) )
	{
		if
		( 	
			screenData.bounds.left == classicDisplay[0]->gdRect.left 		&&
			screenData.bounds.top == classicDisplay[0]->gdRect.top 			&&
			screenData.bounds.right == classicDisplay[0]->gdRect.right 		&&
			screenData.bounds.bottom == classicDisplay[0]->gdRect.bottom  		
		)
			break;
	}
	
	return classicDisplay;
}



//
//	Copies the gamma table currently in use by the display to the RLGammaTable provided by the user
//
//

OSStatus CocoaDisplay::GetGamma( RLGammaTable *gamma ) const
{
	if( ! gamma )
		return noErr;

	CGTableCount count = 0;
	
	//Get the gamma table
	CGDisplayErr error = CGGetDisplayTransferByTable( display, kGammaTableEntryCount, gamma->red, gamma->green, gamma->blue, &count ); 
	if( error )
	{
		DEBUGMESSAGE("Got error # " << error << " attempting to get the current gamma from CoreGraphics" );
		return error;
	}
	
	//Make the table 256 entries per color if it isn't already
	if( count != kGammaTableEntryCount )
	{
		float temp[kGammaTableEntryCount];
	
		//Do reds with interpolation
		for( int i = 0; i < kGammaTableEntryCount; i++ )
		{
			float x = float( i ) * float( count ) / float( kGammaTableEntryCount );
			int low = (int)(x);
			int high = (int)ceil(x);
			float fragment = x - float(low);
		
			temp[i] = (gamma->red[high] - gamma->red[low]) * fragment + gamma->red[low]; 
		}
		BlockMoveData( temp, gamma->red, sizeof( float) * kGammaTableEntryCount );

		//Do greens with interpolation
		for( int i = 0; i < kGammaTableEntryCount; i++ )
		{
			float x = float( i ) * float( count ) / float( kGammaTableEntryCount );
			int low = int(x);
			int high = (int)ceil(x);
			float fragment = x - float(low);
		
			temp[i] = (gamma->green[high] - gamma->green[low]) * fragment + gamma->green[low]; 
		}
		BlockMoveData( temp, gamma->green, sizeof( float) * kGammaTableEntryCount );

		//Do blue with interpolation
		for( int i = 0; i < kGammaTableEntryCount; i++ )
		{
			float x = float( i ) * float( count ) / float( kGammaTableEntryCount );
			int low = int(x);
			int high = (int)ceil(x);
			float fragment = x - float(low);
		
			temp[i] = (gamma->blue[high] - gamma->blue[low]) * fragment + gamma->blue[low]; 
		}
		BlockMoveData( temp, gamma->blue, sizeof( float) * kGammaTableEntryCount );
	}
	
	return noErr;
}

//
// Allocates an array that holds all possible resolutions and refresh rates for this display
//
RLInfo *CocoaDisplay::GetResolutionList( unsigned long *listCount ) const
{
	CFArrayRef modeList = CGDisplayAvailableModes( display);
	if( ! modeList )
	{
		DEBUGMESSAGE( "Unable to read the available display modes for this device. CGDisplayAvailableModes returned NULL. ");
		return NULL;
	}
		
	CFIndex count = CFArrayGetCount( modeList );
	int	i;
	
	//Allocate an array big enough to hold every rez, though we will probably only use 1/3 that much space
	//Perhaps a more sensible use of storage could be devised in the future
	RLInfo *list = (RLInfo *) NewPtr( count * sizeof( RLInfo ) );
	RLInfo info;
	UInt32 listSize = 0;
	
	memset( list, 0, sizeof( RLInfo ) );
	
	//Iterate through the list, one entry per height, width, refresh rate combo. Multiple color 
	//depths fall into one entry
	if( list )
	{
		for( i = 0; i < count; i++ )
		{
			//Look up the next display setting and extract out the display info for it
			CFDictionaryRef dictionary = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );		
			OSStatus err = ExtractModeDictionaryInfo(  dictionary, &info );
			if( err )
				continue;
			
			//Search the existing list for any resolution matches
			SInt32 match = -1;
			for( int j = 0; j < listSize; j++ )
				if( info.height == list[j].height	&&
					info.width == list[j].width		&&
					info.refreshRate == list[j].refreshRate &&
					(info.flags & rlDisplayModeMatchingBits) == (list[j].flags & rlDisplayModeMatchingBits)
					
				 )
				{
					match = j;
					break;
				}

			//If a match is found, add our color depth to it
			if( match > -1 )
				list[ match ].colorDepths |= info.colorDepths;
			else
			{//Otherwise record a new entry in the list
				list[ listSize ] = info;
				listSize++;
			}
		}	
	}
	else
		DEBUGMESSAGE( "Out of memory error allocating storage to hold the list of resolutions. Returning NULL. " );
		
	//record the list size
	if( listCount )
		*listCount = listSize;
		
	return list;
}

RetraceProc *CocoaDisplay::EmulateVBL( UInt32 refresh, VerticalRetraceProcPtr proc, long userData ) 
{
	CocoaRetraceProc *retraceProc = NULL;
	
	try
	{
		retraceProc = new CocoaRetraceProc( this, refresh, display, proc, userData );
	}
	catch( OSStatus error )
	{
		DEBUGMESSAGE( "Encountered error # " << error << " attempting to install a Cocoa vertical retrace procedure." );
	}
	
	return (RetraceProc *) retraceProc;
}



RetraceProc *CocoaDisplay::InstallVerticalRetraceProcedure( VerticalRetraceProcPtr proc, long userData ) 
{
	CocoaRetraceProc *retraceProc = NULL;
	
	try
	{
		retraceProc = new CocoaRetraceProc( this, display, proc, userData );
	}
	catch( OSStatus error )
	{
		DEBUGMESSAGE( "Encountered error # " << error << " attempting to install a Cocoa vertical retrace procedure." );
	}
	
	return (RetraceProc *) retraceProc;
}



//
// Undoes the damage done by Acquire desktop and returns the display to its original state
//
OSStatus CocoaDisplay::ReleaseDesktop( void )
{
	if( ! desktopRestoreContext )
	{
		DEBUGMESSAGE( "Error: Desktop was never acquired or already released." );
		return noErr;
	}

	//Restore the desktop
	OSErr err = EndFullScreen( desktopRestoreContext, 0 );                          

	if( ! err )
	{
		DEBUGMESSAGE( "Desktop Released." );
		desktopRestoreContext = NULL;
		blankingWindow = NULL;
	}
	else
	{
		DEBUGMESSAGE( "Got error # " << err << " attempting to release the desktop using EndFullScreen()." );
	}

	return err;

/*	if( blankingWindow )
		DisposeWindow( blankingWindow );
	blankingWindow = NULL;
	
	return  (*RLDisplayRelease)( display);
*/
}


OSStatus CocoaDisplay::RestoreIconPositions( Handle desktopInfo ) const
{
	if( NULL == desktopInfo )
		return paramErr;	

	DisposeHandle( desktopInfo );
	
	return noErr;
}

OSStatus CocoaDisplay::SaveIconPositions( Handle *desktopInfo ) const
{
	if( NULL == desktopInfo )
		return paramErr;	

	//I thought about returning -1 here, but
	//best not to lie about what is a handle and what isn't
	*desktopInfo = NewHandle(0);

	return noErr;
}
	

OSStatus CocoaDisplay::SetColorDepth( UInt32 newColorDepth )
{
	RLInfo currentSetting, info;
	
	memset( &currentSetting, 0, sizeof( RLInfo ) );
	memset( &info, 0, sizeof( RLInfo ) );
	
	CFArrayRef modeList = CGDisplayAvailableModes( display);
	if( ! modeList )
		return rlUnableToFetchModeList;
	
	OSStatus err = GetCurrentScreenSetting( &currentSetting );
	if( err )
		return err;
	
	if( currentSetting.colorDepths == newColorDepth )
		return noErr;
	
	CFIndex count = CFArrayGetCount( modeList );
	bool modeFound = false;
	CFDictionaryRef newSetting;
	for( int i = 0; i < count; i++ )
	{
		newSetting = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );
		err = ExtractModeDictionaryInfo( newSetting, &info );
		if
		( 
			! err												&&
			currentSetting.height == info.height				&&
			currentSetting.width == info.width					&&
			currentSetting.refreshRate == info.refreshRate 		&&
			newColorDepth == info.colorDepths					&&
			(currentSetting.flags & rlDisplayModeMatchingBits) == (info.flags & rlDisplayModeMatchingBits)
		)
		{
			modeFound = true;
			break;
		}
	}
	
	if( ! modeFound )
		return rlColorDepthNotAvailable;
				
	return SwitchMode( newSetting );
}

OSStatus  CocoaDisplay::SetGamma( const RLGammaTable *gamma )
{
	CGDisplayErr error = CGSetDisplayTransferByTable( display, kGammaTableEntryCount, gamma->red, gamma->green, gamma->blue );
	
	return error;
}


OSStatus CocoaDisplay::SetResolution( long opaqueModeID )
{
	RLInfo currentInfo;
	
	memset( &currentInfo, 0, sizeof( RLInfo ) );

	//Record the current setting, so we know what color depth to use	
	OSStatus err = GetCurrentScreenSetting( &currentInfo );
	if( err )
		return rlUnableToFetchModeList;

	return SetResolutionAndColorDepth( opaqueModeID, currentInfo.colorDepths );
}

OSStatus CocoaDisplay::SetResolutionAndColorDepth( long resolutionID, UInt32 newColorDepth )
{
	RLInfo approximateInfo, newInfo;
	OSStatus	err = noErr;

	memset( &approximateInfo, 0, sizeof( RLInfo ) );
	memset( &newInfo, 0, sizeof( RLInfo ) );


	CFArrayRef modeList = CGDisplayAvailableModes( display);
	if( ! modeList )
		return rlUnableToFetchModeList;

	//Find the dimensions and refresh rate the caller is looking for
	CFIndex count = CFArrayGetCount( modeList );
	bool modeFound = false;
	CFDictionaryRef newSetting = NULL;
	for( int i = 0; i < count; i++ )
	{
		newSetting = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );
		err = ExtractModeDictionaryInfo( newSetting, &approximateInfo  );
		if( ! err && resolutionID == approximateInfo.resolutionID )
		{
			modeFound = true;
			break;
		}
	}
	
	if( ! modeFound )
	{
		DEBUGMESSAGE( newColorDepth << " bit color mode not found with resolutionID # " << resolutionID );
		return rlRezNotFound;
	}
	
	modeFound = false;
	
	//Find the exact setting at the right color depth
	for( int i = 0; i < count; i++ )
	{
		newSetting = (CFDictionaryRef) CFArrayGetValueAtIndex( modeList, i );
		err = ExtractModeDictionaryInfo(  newSetting, &newInfo);
		if( ! err && newInfo.height == approximateInfo.height &&  newInfo.width == approximateInfo.width && 
			newInfo.refreshRate == approximateInfo.refreshRate && newInfo.colorDepths == newColorDepth 	&&
			(newInfo.flags & rlDisplayModeMatchingBits) == (approximateInfo.flags & rlDisplayModeMatchingBits)
		)
		{
			modeFound = true;
			break;
		}
	}
	
	if( ! modeFound )
		return rlColorDepthNotAvailable;
	
	return SwitchMode( newSetting );
}

OSStatus CocoaDisplay::SwitchMode( const CFDictionaryRef newMode )
{
	bool wasCaptured = CGDisplayIsCaptured( display );
	CGDisplayErr err = (CGError) CGDisplayNoErr;

#if DEBUG	
	DEBUGMESSAGE( "Switching Display mode..." );
	RLInfo mode;
	memset( &mode, 0, sizeof( RLInfo ));
	if( noErr == GetCurrentScreenSetting( &mode ) )
		DEBUGMESSAGE( "   Old Mode: " << mode.name << " using " << mode.colorDepths << " bit color  (id = " << mode.resolutionID << ").\n" );
#endif

	
	//Capture the screen to guarantee synchronous execution
	if( ! wasCaptured )
		err = CGDisplayCapture( display );
		
	//Switch mode
	OSStatus error = CGDisplaySwitchToMode( display, newMode );

#if DEBUG	
	if( error )
		DEBUGMESSAGE( "Error # " << error << " attempting to switch to the new display mode or color depth." );

	RLInfo mode2;
	memset( &mode2, 0, sizeof( RLInfo ));

	if( noErr == GetCurrentScreenSetting( &mode2 ) )
		DEBUGMESSAGE( "   New Mode: " << mode2.name << " using " << mode2.colorDepths << " bit color  (id = " << mode2.resolutionID << ").\n" );
#endif

	//Release the screen if we just captured it
	if( ! wasCaptured && ! err )
		CGDisplayRelease(display);

	return error;
}

OSStatus CocoaDisplay::WaitForVerticalRetrace( void )
{

    CFDictionaryRef mode = CGDisplayCurrentMode( display );
    if( NULL == mode )
    	return rlUnableToFetchModeList;
	RLInfo info;
	memset( &info, 0, sizeof( RLInfo ));

	OSStatus error = ExtractModeDictionaryInfo( mode, &info );
	if( error )
		return error;
	if( 0 == info.refreshRate )
		return rlScreenHasNoRefreshRate;
		
    double linesPerSecond = (double) info.refreshRate * (double) info.height * (1.0 / 65536.0);
	double target = (double) info.height;

    //Figure out the first delay so we start off about right
    double position = CGDisplayBeamPosition( display );
    if( position > target  )
        position = 0;
    double adjustment = (target - position) / linesPerSecond; 
    AbsoluteTime nextTime = AddAbsoluteToAbsolute( UpTime(), SecondsToAbsolute( adjustment ) );

	MPDelayUntil( &nextTime );
//Old icky spinlock code
/*	CGRect	bounds = (*RLDisplayBounds)( display );
	CGBeamPosition position, oldPosition;
	
	position = oldPosition = 0;
	
	//Yep, spinlocks are ugly, but I haven't found any other way that will return control when it is supposed to yet.
	do
	{
		oldPosition = position;
		position = (*RLDisplayBeamPosition)( display );
	}
	while( position >= oldPosition );
*/
	return noErr;
}

CocoaDisplay::CocoaDisplay( const Rect *where ) throw (OSStatus) : RLDisplay( where )
{
//	DEBUGMESSAGE( "CocoaDisplay::CocoaDisplay()" );
	display = NULL;
	blankingWindow = NULL;
	desktopRestoreContext = NULL;
	OSStatus 	err = noErr;

//Find the display
	{
		CGRect 	screenRect = { {0.0, 0.0}, {1.0, 1.0}};
		//If no location is provided, use the origin, which is always the main display
		if( where )	
		{
			screenRect.origin.x = where->left;
			screenRect.origin.y = where->top;
			screenRect.size.width = float( long(where->right) - long(where->left));
			screenRect.size.height = float( long( where->bottom) - long( where->top ));
		}

		CGDisplayCount count = 0;
		CGDisplayErr err = CGGetDisplaysWithRect( screenRect, 1, &display, &count );
		if( err )
			throw OSStatus( rlInvalidDisplayLocation );	
	}

//Read in the Display Mode List	
	{
		CFArrayRef modeList = CGDisplayAvailableModes( display);
		if( ! modeList )
			throw OSStatus( rlUnableToFetchModeList );
	}
//	DEBUGMESSAGE( "CocoaDisplay::CocoaDisplay() completed" );

	//Exercise MPDelayUntil so that when we really need it, nothing will have to be loaded in or set up
	AbsoluteTime nextTime = UpTime();
    MPDelayUntil( &nextTime );
}

CocoaDisplay::~CocoaDisplay()
{
	if( blankingWindow )
		ReleaseDesktop();
		
}

#pragma mark -



CocoaRetraceProc::CocoaRetraceProc(  RLDisplay *the_owner, UInt32 refresh, CGDirectDisplayID	the_display, VerticalRetraceProcPtr the_proc, long the_Data ) throw (OSStatus)
	: RetraceProc( the_owner, the_proc, the_Data )
{
	display = the_display;
	task = 0;
	error = noErr;
	emulatedRefreshRate = refresh;
	
	//Queue up a task to emulate VBL
	OSStatus err = MPCreateTask( MPFixedTimeTaskProcedure, this, 16384, NULL, NULL, NULL, 0, &task );

	if( err ) 
		throw OSStatus( err );

	if( error ) 
		throw OSStatus( error );
}

CocoaRetraceProc::CocoaRetraceProc( RLDisplay *the_owner, CGDirectDisplayID	the_display, VerticalRetraceProcPtr the_proc, long the_Data ) throw (OSStatus)
	: RetraceProc( the_owner, the_proc, the_Data )
{
	display = the_display;
	task = 0;
	error = noErr;

	//Test for bad display type
	SInt32 refreshRate = 0;
    CFDictionaryRef mode = CGDisplayCurrentMode( the_display );
    CFNumberRef data = (const CFNumberRef) CFDictionaryGetValue( mode, kRLDisplayRefreshRate );
    Boolean isGood = CFNumberGetValue( data, kCFNumberSInt32Type, &refreshRate );
	if( 0 == refreshRate || ! isGood )
		throw OSStatus( rlScreenHasNoRefreshRate );	
	
	//Queue up a task to emulate VBL
	OSStatus err = MPCreateTask( MPTaskProcedure, this, 16384, NULL, NULL, NULL, 0, &task );

	if( err ) 
		throw OSStatus( err );

	if( error ) 
		throw OSStatus( error );

}

CocoaRetraceProc::~CocoaRetraceProc()
{
	OSStatus error = noErr;
	if( task )
		error = MPTerminateTask( task, noErr );		

	if( error )
		DEBUGMESSAGE( "Got error " << error << " terminating the screen retrace procedure." );
		
	task = 0;
	
	DEBUGMESSAGE( "VBL task destroyed." );
}

/* In the words of Apple:

Yes, CGDirectDisplay is thread-safe. Note that multiple threads trying
to set different display modes at the same time may have unexpected 
results.

If you do use a secondary thread to block for a particular beam
position, you may want to carefully examine how you hand off control
to the primary thread.  Variable latencies and scheduling overhead
could add up to most of a vertical sweep period, particularly on
iMacs at lower resolution (less than 10 msec/vertical sweep).

Also note that most Carbon and Cocoa windows are drawn buffered, so
beam synch tricks on the client side are mostly useless.  The server
tries to do some beam synch tricks now for flushing.  This will
improve with changes in latency estimation and device driver support
for embedded sync commands in the PM4 control streams.

*/

// Fortunately for me, with this architecture, dealing with handoff is 
// somebody else's problem!  I am almost giddy at the thought of dodging
// that bullet ... well almost. Actually, the problem here is one of 
// getting ourselves out of the way. We dont want to senselessly spin waiting
// for the kernel to switch us out due to a timeout. So we have to make sure that
// we are blocked 99.99999% of the time and only executing code when we have 
// to. 

OSStatus CocoaRetraceProc::MPTaskProcedure( void * data )
{
//	AbsoluteTime 	lastTime = UpTime();
    AbsoluteTime	nextTime, wakeTime;
	CocoaRetraceProc *obj = (CocoaRetraceProc *) data;
	if( ! obj )
		return -1;
	
	//Load the data we need into the stack so that it doesn't disappear out from under us
	CGDirectDisplayID display = obj->display;		//Load in our display reference
    CFDictionaryRef mode = CGDisplayCurrentMode( display );
	RLInfo info;
	memset( &info, 0, sizeof( RLInfo ));
	obj->error = ExtractModeDictionaryInfo( mode, &info );
    double position = 0; 
    double target = info.height;
    double interval = 65536.0 / info.refreshRate;
    double skipTime = 1.5 * interval;
    double linesPerSecond = (double) info.refreshRate * (double) info.height * (1.0 / 65536.0);
    double adjustment = 0;
    double error;
    const double kDampeningConst = 1.0; //0.3;

	
 	DEBUGMESSAGE( "VBL task initated at " << AbsoluteToSeconds( UpTime() ) << " seconds." );
 	
 	//Reschedule ourselves to use round-robin fixed priority scheduling. This has more predictable
 	//temporal behavior, since we dont get strange priority degredations that force us to sit on the
 	//fence for a while. While we are at it, grab a small priority boost. 
 	const int kCurrentThread = 0;
	kern_return_t sched_error = RescheduleStdThread( kCurrentThread, kDefaultVBLPriority, kUseRoundRobinScheduling );

#if DEBUG
	{
		if( sched_error != 0 )
			DEBUGMESSAGE( "Failed to adjust VBL priority to " << kDefaultVBLPriority << ", round robin. Error: " << sched_error );
		else
		{
			int priority = 0;
			boolean_t style = kUseTimeShareScheduling;
			
			sched_error = GetStdThreadSchedule( kCurrentThread, &priority, &style );
			
			if( sched_error != 0 )
				DEBUGMESSAGE( "Could not verify that the VBL task was correctly rescheduled. Error: " << sched_error );
			else
			{
				if( style == kUseTimeShareScheduling )
					DEBUGMESSAGE( "VBL task rescheduled to priority " << priority << " but still is using time sharing scheduling. :/" );
				else
					DEBUGMESSAGE( "VBL task rescheduled to priority " << priority << " using a round-robin scheduling method." );								
			}
		}
		
	}
#endif //DEBUG

 
    //Warm up MPDelayUntil -- the first call seems to take 200 milliseconds for some reason
    nextTime = AddAbsoluteToAbsolute( UpTime(), SecondsToAbsolute( 0.01 ) );
    MPDelayUntil( &nextTime );

   //Figure out the first delay so we start off about right
    position = CGDisplayBeamPosition( display );
    if( position > target  )
        position = 0;
    adjustment = (target - position) / linesPerSecond; 
    nextTime = AddAbsoluteToAbsolute( UpTime(), SecondsToAbsolute( adjustment ) );
    adjustment = 0;
    
    DEBUGMESSAGE( "Targetting first VBL to happen at " << AbsoluteToSeconds( nextTime ) );

	//Loop indefinitely. We may be killed externally or may exit in case of an error.
	while( noErr == obj->error )
 	{
       MPDelayUntil( &nextTime );
  
        position = CGDisplayBeamPosition( display );
        wakeTime = UpTime();

        //If we are right on target, use the position to figure out where we should have been
        error = AbsoluteToSeconds(wakeTime) - AbsoluteToSeconds( nextTime);

#if DEBUG
        //Make noise if we skip whole vbl periods
        if( error > skipTime )
           DEBUGMESSAGE( "¥¥¥VBL delayed at " << AbsoluteToSeconds( wakeTime ) << " seconds. Kernel did not return control in a timely fashion!\n\tRacing to catch up. Currently behind by " << error - interval << " seconds. Message may repeat until we catch up." );
#endif        
#if VERBOSE_LOGGING            
       DEBUGMESSAGE( "Beam Position is " << position << " at time " <<  AbsoluteToSeconds(wakeTime) << "\t  (VBL supposed to have fired at time " <<  AbsoluteToSeconds(nextTime)<< ".)" );
#endif        

        if( fabs( error ) < interval * 0.3 )
        {
            //Correct for the timing error -- linesPerSecond isn't quite right because it doesnt 
            //account for retrace times. It is probably too small, which is okay
            //Commented out since this had the effect of causing the VBL to fire 30 scan lines early
 //           position -= error * linesPerSecond;

            //If we are late give a gentle nudge back towards the early direction
            if( position < target * 0.5 )
            	position += 10;
            else//Wrap pixel positions into range
    	        position -= target;
        
            adjustment = kDampeningConst * position / linesPerSecond; 
 #if VERBOSE_LOGGING            
      		DEBUGMESSAGE( "Tweaking VBL interval by " << adjustment << "seconds." );
#endif        
        }
        else
            adjustment = 0.0;
        
        nextTime = AddAbsoluteToAbsolute( nextTime, SecondsToAbsolute( interval - adjustment ) );
        
        //Call the user's retrace proc
		obj->RetraceFunction();

#if VERBOSE_LOGGING            
       DEBUGMESSAGE( "Targetting next VBL to fire at" <<  AbsoluteToSeconds(nextTime)  );
#endif        

	}
	
 	DEBUGMESSAGE( "VBL task terminated at " << AbsoluteToSeconds( UpTime() ) << " seconds. Returning error # " << error << "." );

	obj->task = 0;
	
	return noErr;
}

OSStatus CocoaRetraceProc::MPFixedTimeTaskProcedure( void * data )
{
    AbsoluteTime	nextTime, step;
	CocoaRetraceProc *obj = (CocoaRetraceProc *) data;
	OSStatus error = 0;
	union
	{
		Nanoseconds	ns;
		UInt64		i;
	}period;


	if( ! obj )
		return -1;
	
	//Load the data we need into the stack so that it doesn't disappear out from under us
    double interval = 65536.0 / obj->emulatedRefreshRate;
	period.i = UInt64(interval * 1e9);
	step = NanosecondsToAbsolute( period.ns );
	
 	DEBUGMESSAGE( "Emulated VBL task initated at " << AbsoluteToSeconds( UpTime() ) << " seconds." );
 	
 	//Reschedule ourselves to use round-robin fixed priority scheduling. This has more predictable
 	//temporal behavior, since we dont get strange priority degredations that force us to sit on the
 	//fence for a while. While we are at it, grab a small priority boost. 
 	const int kCurrentThread = 0;
	kern_return_t sched_error = RescheduleStdThread( kCurrentThread, kDefaultVBLPriority, kUseRoundRobinScheduling );

 
   //Figure out the first delay so we start off about right
    nextTime = AddAbsoluteToAbsolute( UpTime(), step );
    
	//Loop indefinitely. We may be killed externally or may exit in case of an error.
	while( noErr == obj->error )
 	{
		MPDelayUntil( &nextTime );


#if VERBOSE_LOGGING            
       DEBUGMESSAGE( "Beam Position is " << position << " at time " <<  AbsoluteToSeconds(UpTime()) << "\t  (VBL supposed to have fired at time " <<  AbsoluteToSeconds(nextTime)<< ".)" );
#endif        

		nextTime = AddAbsoluteToAbsolute( nextTime, step );

		//Call the user's retrace proc
		obj->RetraceFunction();
	}
	
 	DEBUGMESSAGE( "VBL task terminated at " << AbsoluteToSeconds( UpTime() ) << " seconds. Returning error # " << error << "." );

	obj->task = 0;
	
	return error;
}

double AbsoluteToSeconds( AbsoluteTime time )
{
    union
    {
        UInt64	i;
        Nanoseconds ns;
    }temp;
    temp.ns = AbsoluteToNanoseconds( time );
    return 0.000000001 * (double) temp.i;
}

AbsoluteTime SecondsToAbsolute( double seconds )
{
    union
    {
        UInt64	i;
        Nanoseconds ns;
    }temp;

    temp.i = UInt64(seconds * 1000000000.0);
    return NanosecondsToAbsolute( temp.ns );
}

// Some credit for the next two routines go to Michel Colman, who
// pointed out a similar route through the pthread API.

//Polls a (non-realtime) thread to find out how it is scheduled
//Results are undefined of an error is returned. Otherwise, the
//priority is returned in the address pointed to by the priority 
//parameter, and whether or not the thread uses timeshare scheduling
//is returned at the address pointed to by the isTimeShare parameter 
kern_return_t  GetStdThreadSchedule( mach_port_t    machThread,
                                     int            *priority,
                                     boolean_t      *isTimeshare ) 
{
    kern_return_t                       result = 0;
    thread_extended_policy_data_t       timeShareData;
    thread_precedence_policy_data_t     precidenceData;
    mach_msg_type_number_t		structItemCount;
    boolean_t				fetchDefaults = false;
 
    memset( &timeShareData, 0, sizeof( thread_extended_policy_data_t ));
    memset( &precidenceData, 0, sizeof( thread_precedence_policy_data_t ));

	machThread=mach_thread_self();
	
//	if( NULL == RLThreadPolicyGet )
//		return rlDirectDisplayAPINotAvailable;

    if( NULL != isTimeshare )
    {
        structItemCount = THREAD_EXTENDED_POLICY_COUNT;
        result = thread_policy_get( machThread, THREAD_EXTENDED_POLICY, 
										(thread_policy_t)&timeShareData, &structItemCount, &fetchDefaults );
        *isTimeshare = timeShareData.timeshare;
        if( 0 != result )
            return result;
    }
    
    if( NULL != priority )
    {
        fetchDefaults = false;
        structItemCount = THREAD_PRECEDENCE_POLICY_COUNT;
        result = thread_policy_get( machThread, THREAD_PRECEDENCE_POLICY, 
										(thread_policy_t)&precidenceData, &structItemCount, &fetchDefaults );
        *priority = precidenceData.importance;
    }

    return result;
}   


// Reschedules the indicated thread according to new parameters:
//
// machThread           The mach thread id. Pass 0 for the current thread.
// newPriority          The desired priority.
// isTimeShare          false for round robin (fixed) priority,
//                      true for timeshare (normal) priority
//
// A standard new thread usually has a priority of 0 and uses the
// timeshare scheduling scheme. Use pthread_mach_thread_np() to
// to convert a pthread id to a mach thread id
kern_return_t  RescheduleStdThread( mach_port_t    machThread,
                                    int            newPriority,
                                    boolean_t      isTimeshare )
{
    kern_return_t                       result = 0;
    thread_extended_policy_data_t       timeShareData;
    thread_precedence_policy_data_t     precidenceData;

    //Set up some variables that we need for the task
    precidenceData.importance = newPriority;
    timeShareData.timeshare = isTimeshare;
	
	machThread=mach_thread_self();
	
    //Set the scheduling flavor. We want to do this first, since doing so
    //can alter the priority
    result = thread_policy_set(	machThread,
									THREAD_EXTENDED_POLICY,
									(thread_policy_t)&timeShareData,
									THREAD_EXTENDED_POLICY_COUNT );

    if( 0 != result )
        return result;
 
    //Now set the priority
    return   thread_policy_set(	machThread,
									THREAD_PRECEDENCE_POLICY,
									(thread_policy_t)&precidenceData,
									THREAD_PRECEDENCE_POLICY_COUNT );
        
}


