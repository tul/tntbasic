
//
//	RLDisplay.cpp
//
//  © Ian R Ollmann (iano@cco.caltech.edu)	2000
//		All Rights Reserved
//
//	Please see accompanying documentation for duplication, distribution and licensing terms.
//
//

//
//	Most of the functions contained in RLDisplay.cpp are wrapper functions that do a bit of 
//	parameter checking before passing the call on to the appropriate C++ class method of similar name. 
//	This provides a bit more safety and allows our C and pascal using friends to use this library.
//
//

// Modified by Mark Tully 11/6/06 to work directly with Mach rather than doing so indirectly via function ptrs.

#include "RLCocoaMachODisplay.h"
#include <DriverServices.h>

static Handle LoadDITL();

#define DEBUG_INIT_ROUTINES	0

#if	DEBUG_INIT_ROUTINES
	#include <stdio.h>
	
	void CheckForError( int line, const char *file, int error )
	{
	
		if( noErr != error )
		{
			char	cString[256];
			Str255	pString;
		
			sprintf( cString, "Found Error # %i at line %i of file: %s\n", error, line, file ); 			
			CopyCStringToPascal( cString, pString );
			
			DebugStr( pString );
		}		
	}
	
	#define CHECKERR( err )	CheckForError( __LINE__, __FILE__, err )	
#else
	#define CHECKERR( err )
#endif

Boolean	isInitted = false;			//true if the library has loaded itself. Generally remains false until the first call to MakeNewRLDisplay
//Boolean isAltiVecPresent = false;	//true if running on a machine with a G4 onboard
//FSSpec 	ourLib = { 0, 0, "\p" }; //The FSSpec for this library
//Handle	gDITL = NULL;			//A handle to a dialog item list for the dialog box for confirming that a resolution works
NumVersion	gRLVersion = { 1, 0x1B, finalStage, 0 };	//1.1 -- I'll try to update this default when I remember to

//Inits a new RLDisplay object of the appropriate type and returns a pointer to it.
//It is meant to be a C++ factory function that deals with the environment specific 
//instantiation of RLDisplay subclasses.
RLDisplay *MakeNewRLDisplay( const Rect *where ) throw (OSStatus )
{
	RLDisplay	*display = NULL;

	//Init the appropriate display object
	display = new CocoaDisplay( where );
	
	return display;
}

#pragma mark -

// MT : this is a compatibility routine to do stuff that used to be done in the cfm initialiser of rezlib
static Handle LoadDITL()
{
	//Load the dialog item list for our AskIfNewResolutionWorks() function dialog box
	static Handle		s_ditl=NULL;
	if (!s_ditl)
	{
		//Load our DITL
		s_ditl = Get1Resource( 'DITL', 27398 );
		if( s_ditl )
		{
			DEBUGMESSAGE( "\"AskIfNewResolutionWorks\" DITL loaded." );
			DetachResource( s_ditl );
		}
		else
			DEBUGMESSAGE( "Error: Failed to load DITL." );
		
		OSStatus err = ResError();
		CHECKERR( err );
		if( err )
			DEBUGMESSAGE( "Found Resource Manager error # " << err << " after attempting to load the \"AskIfNewResolutionWorks\" DITL. " );	
	}
	return s_ditl;
}

#pragma mark -

RLDisplay::~RLDisplay()
{
}

#pragma mark -

//
//
//	This next section has the definitions for our public C API.  Typically these functions dont do very much
//	except do a bit of parameter checking. The work is really done by the C++ methods by the same name.
//
//

//	Make a new ScreenRef that corresponds to the rectangle indicated by the where parameter
ScreenRef	NewScreenRef( const Rect *where, OSStatus *error )
{
	RLDisplay *display = NULL;

	// MakeNewRLDisplay may throw exceptions so we have to catch them here so 
	// that we dont inflict them on our C loving public!
	try
	{
		display = MakeNewRLDisplay( where );
		
		// In the particular case of operator new failing, we just get a NULL 
		// pointer back, so generate the exception ourselves.
		if( ! display )
		{
			DEBUGMESSAGE( "Screen Ref allocation failed -- not enough memory!" );
			throw OSStatus( rlOutOfMemory );
		}

		if( error )
			*error = noErr;
	}
	catch( OSStatus err )
	{
		if( error )
			*error = err;	
	}

	return ScreenRef( display );
}

// Free memory associated with a ScreenRef
void		DisposeScreenRef( ScreenRef display )
{
	delete (RLDisplay *) display;
}

//Get a list of available display devices as their bounding rectangles in global coords
Rect *GetDisplayList( unsigned long *listEntryCount )
{
	return GetCocoaDisplayList( listEntryCount );
}

void		DisposeDisplayList( Rect *list )
{
	DisposeCocoaDisplayList( list );
}


// Poll the display for its current resolution, color depth and other settings
OSStatus		GetCurrentSetting( ScreenRef screen, RLInfo *info )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return rlInvalidScreenRef;
	
	if( NULL == info )
		return paramErr;
	
	if( info->rlInfoType != 0 )
	{
		DEBUGMESSAGE("Be sure to set the RLInfo.rlInfoType field to zero before calling GetCurrentSetting()!" );
		return paramErr;
	}
			
	return display->GetCurrentScreenSetting( info );	
}

//Poll the display for the color depths available at this resolution
UInt32		GetColorDepthsAtThisResolution( ScreenRef screen )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return 0;
	
	return display->GetColorDepthsAtThisResolution();	
}

//Poll the display for the color depths available at this resolution
RLInfo		*GetResolutionList( ScreenRef screen, unsigned long *listEntryCount )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
	{
		if( listEntryCount )
			*listEntryCount = 0;
		return NULL;
	}
	
	return display->GetResolutionList( listEntryCount );	
}

//Dispose a RLInfo list obtained from GetResolutionList()
void		DisposeResolutionList( ScreenRef screen, RLInfo *list )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return;
	
	return display->DisposeResolutionList( list );	
}


RetraceToken 	EmulateVerticalRetraceProcedure( ScreenRef screen, 
												 UInt32	refreshRate, 
												 VerticalRetraceProcPtr proc, 
												 long userData )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return (RetraceToken) display->EmulateVBL( refreshRate, proc, userData );	
}


//Set the indicated screen to the color depth provided. Return an error if that color depth is not available.
OSStatus		SetColorDepth( ScreenRef screen, UInt32 colorDepth )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->SetColorDepth( colorDepth );	
}

//Set the indicated screen to the resolution indicated. Return an error if that rez is not available.
OSStatus		SetResolution( ScreenRef screen, UInt32 resolutionID )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->SetResolution( resolutionID );	
}

//Switch to the resolution and color depth indicated
OSStatus		SetResolutionAndColorDepth( ScreenRef screen, UInt32 resolutionID, UInt32 colorDepth )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->SetResolutionAndColorDepth( resolutionID, colorDepth );	
}

//
//	AskIfNewResolutionWorks() creates a dialog box in the center of the screen. The dialog asks the 
//	user if the current display setting works. This is necessary because a number of display settings
//	listed by the OS dont actually work and leave the user with a black screen.  The dialog has a 5
//	second timeout. If the user does not hit ok within 5 seconds the cancel item is chosen automatically
//	for him. This feature allows the user to do nothing (which he will probably do if confronted by a 
//	black screen) and still be ok.  The method that I have employed to do the timeout requires Appearances
//	1.1. I believe this was introduced with OS 8.5. If you want to support back to OS 8.1, then you will
//	have to do your own modal dialog event proc that implements a timeout. The dialog has not default 
//	button by default. Cmd-period and esc trigger the cancel button.
//
//
OSStatus		AskIfNewResolutionWorks( ScreenRef screen )
{
	DEBUGMESSAGE( "Querying user whether the new resolution works...." );

	if( ! screen )
		return noErr;

	//Read the new screen dimensions
	RLDrawInfo 	screenData;
	Rect		dialogBounds = {0,0,130, 340};
	OSStatus 		error = GetCurrentScreenDrawingInfo( screen, &screenData );
	if( error )
	{
		DEBUGMESSAGE( "Unable to get current screen drawing information. Got back error # " << error );
		return error;
	}
	
	//Make a copy of our dialog item list. This will be destroyed when the dialog is destroyed.
	Handle ditlCopy = LoadDITL();
	HandToHand( &ditlCopy );
	
	//Center the dialog rect on the screen
	{
		SInt32 horizontalOffset = (SInt32( screenData.bounds.right) + SInt32( screenData.bounds.left ) - SInt32(dialogBounds.right) +  SInt32(dialogBounds.left) ) / 2;
		SInt32 verticalOffset = (SInt32( screenData.bounds.bottom) + SInt32( screenData.bounds.top ) - SInt32(dialogBounds.bottom) +  SInt32(dialogBounds.top) ) / 2;
		dialogBounds.left += horizontalOffset;
		dialogBounds.right += horizontalOffset;
		dialogBounds.top += verticalOffset;
		dialogBounds.bottom += verticalOffset;
	}
		
	//Init a new dialog hidden
	DialogPtr dialog =  NewFeaturesDialog( NULL, &dialogBounds, "\pResolution Verification", true, kWindowModalDialogProc,
									(WindowPtr) -1L, false, TickCount(), ditlCopy, 0 );
	if( ! dialog )
	{
		DEBUGMESSAGE( "Unable to init the \"AskIfNewResolutionWorks\" dialog window. Perhaps there is insufficient free memory or the DITL did not load properly at library startup?" );
		return rlOutOfMemory;
	}
	
	//Make sure the dialog cancel item is button # 2	
	SetDialogCancelItem( dialog, 2 );

	//Set dialog to timeout after 5 seconds	
	SetDialogTimeout( dialog, 2, 5 );

	for( bool done = false; !done; )
	{
		short itemHit = 0;
		ModalDialog ( NULL, &itemHit );  

		switch( itemHit )
		{
			case 2:	//cancel
				DEBUGMESSAGE( "The user hit cancel or the dialog timed out. The new resolution is probably not good." );
				done = true;
				error = rlRezNotFound;
				break;

			case 3:	//ok
				DEBUGMESSAGE( "The user hit ok. The new resolution seems to be Okay!" );
				done = true;
				error = noErr;
				break;		
		}	
	}
	
	DisposeDialog( dialog );
	
	return error;
}


// Copy some parameters relevant to directly drawing to the screen into the RlDrawInfo struct provided
OSStatus		GetCurrentScreenDrawingInfo( ScreenRef screen, RLDrawInfo *info )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->GetCurrentDrawingInfo(  info );	
}

//Loop, wasting lots of CPU time, until the screen enters its vertical retrace
OSStatus		WaitForVerticalRetrace( ScreenRef screen )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->WaitForVerticalRetrace();	
}

//Install a callback function that is called every time the screen enters retrace. The userData parameter is passed
//unchanged to the callback function
RetraceToken	InstallVerticalRetraceProcedure( ScreenRef screen, VerticalRetraceProcPtr proc, long userData )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( NULL == display || NULL == proc )
		return kNoRetraceProcInstalled;
	
	return (RetraceToken) display->InstallVerticalRetraceProcedure( proc, userData );	
}

//Dispose of a retrace callback installed using InstallVerticalRetraceProcedure().
void		DisposeVerticalRetraceProcedure( RetraceToken token )
{
	RetraceProc *proc = (RetraceProc*) token;
	if( NULL == proc )
		return;
	
	RLDisplay	*owner = proc->GetOwner();
	if( NULL == owner )
	{
		DEBUGMESSAGE("Error: detected a retrace procedure without an owner!" );
		return;
	}	
	
	owner->DisposeVerticalRetraceProcedure( proc );
}

//Reads the gamma table from the screen and copies the data into the struct passed in by the user
OSStatus GetGamma( ScreenRef screen, RLGammaTable *gamma )
{
	RLDisplay *display = (RLDisplay *) screen;
	
	if( ! display )
		return rlInvalidScreenRef;
	
	return  display->GetGamma( gamma );	

}

//Reads the struct passed in by the user and copies  the gamma table to the display. May be slow since
//it may have to wait for retrace before the driver can accept the new gamma
OSStatus	SetGamma( ScreenRef screen, const RLGammaTable *gamma )
{
	RLDisplay *display = (RLDisplay *) screen;

	if( ! display )
		return noErr;
	
	return  display->SetGamma( gamma );	
}

OSStatus	FadeToColor(  ScreenRef screen, const RGBColor *color, float seconds )
{
	register double secondsCopy = seconds;
	RLDisplay *display = (RLDisplay *) screen;

	if( ! display )
		return rlInvalidScreenRef;

	//Generate a gamma table from the provided color
	RLGammaTable	newGamma;
	float red = float( color->red ) / 65535.0;
	float green = float( color->green ) / 65535.0;
	float blue = float( color->blue ) / 65535.0;
	for( int i = 0; i < kGammaTableEntryCount; i++ )
	{
		newGamma.red[i] = red;
		newGamma.green[i] = green;
		newGamma.blue[i] = blue;	
	}
	
	return FadeToGamma( screen, &newGamma, secondsCopy );
}

/* These functions will fade multiple monitors simultaneously		*/
/* Do a gamma fade to the color provided over the time period desired	*/
OSStatus FadeToColorMultiple( 	ScreenRef *screens, 
								unsigned int count, 
								const RGBColor *color, 
								float seconds )
{
	RLGammaTable	*newGammas = (RLGammaTable*) NewPtr( count * sizeof( RLGammaTable ) );
	if( NULL == newGammas )
		return rlOutOfMemory;
	
	//Generate new gamma tables from the provided color
	float red = float( color->red ) / 65535.0;
	float green = float( color->green ) / 65535.0;
	float blue = float( color->blue ) / 65535.0;

	for( int i = 0; i < count; i++ )
	{
		for( int j = 0; j < kGammaTableEntryCount; j++ )
		{
			newGammas[i].red[j] = red;
			newGammas[i].green[j] = green;
			newGammas[i].blue[j] = blue;	
		}
	}
	
	OSStatus err = FadeToGammaMultiple( screens, count, newGammas, seconds );

	DisposePtr( (Ptr) newGammas );

	return err;
}




double	GetCurrentTime( void );
//A quick little function for rapidly determining an accurate time
double	GetCurrentTime( void )
{
	union
	{
		Nanoseconds		ns;
		UInt64			u64;
	}time;
	time.ns = AbsoluteToNanoseconds( UpTime() );
	return double( time.u64 ) * 1e-9;
}

OSStatus	FadeToGamma( ScreenRef screen, const RLGammaTable *newGamma, float seconds )
{
	if( ! newGamma )
		return paramErr;

	DEBUGMESSAGE( "Fading to new gamma over " << seconds << " seconds." );

	//Calculate the starting and ending time for out fade
	double startTime = GetCurrentTime();
	double endTime = startTime + seconds;
	
	//Determine our starting gamma
	RLGammaTable	oldGamma, currentGamma;
	OSStatus error = GetGamma( screen, &oldGamma );
	if( error )
		return error;

	//Loop until we run out of time, resetting the gamma as fast as possible
	for( double currentTime = GetCurrentTime(); currentTime < endTime && ! error; currentTime = GetCurrentTime() )
	{
		//The fraction is the percentage of time that we have spend so far in this loop as a value between 0 and 1.0
		double fraction = (currentTime - startTime) / seconds;	
		
		//Calculate the new gamma based on the amount of time spent so far
		//(1-fraction)startGamma + fraction * endGamma = startGamma + fraction( endGamma - startGamma)
		for( int i = 0; i < kGammaTableEntryCount; i++ )
		{
			currentGamma.red[i] = oldGamma.red[i] + fraction * (newGamma->red[i] - oldGamma.red[i]);
			currentGamma.green[i] = oldGamma.green[i] + fraction * (newGamma->green[i] - oldGamma.green[i]);
			currentGamma.blue[i] = oldGamma.blue[i] + fraction * (newGamma->blue[i] - oldGamma.blue[i]);		
		}
		
		error = SetGamma( screen, &currentGamma );
	}
	
	if( error )
		DEBUGMESSAGE( "Got error # " << error << " doing a gamma fade in FadeToGamma(). Attempting to achieve the final gamma...." );
		
	return SetGamma( screen, newGamma );
}

/* Do a gamma fade to the gamma table provided over the time period desired	*/
OSStatus		FadeToGammaMultiple( ScreenRef *screen, unsigned int count, const RLGammaTable *newGamma, float seconds )
{
	int i;
	OSStatus error = noErr;

	if( ! newGamma )
		return paramErr;

	//Calculate the starting and ending time for out fade
	double startTime = GetCurrentTime();
	double endTime = startTime + seconds;
	
	//Determine our starting gamma
	RLGammaTable	*oldGamma = (RLGammaTable*) NewPtr( sizeof( RLGammaTable ) * count );
	RLGammaTable	*currentGamma = (RLGammaTable*) NewPtr( sizeof( RLGammaTable ) * count );
	
	if( NULL == oldGamma || NULL == currentGamma )
	{
		if( NULL != oldGamma )
			DisposePtr( Ptr( oldGamma ) );
		if( NULL != currentGamma )
			DisposePtr( Ptr( currentGamma ) );	
	
		return rlOutOfMemory;
	}
		
	for( i = 0; i < count; i++ )
	{
		error = GetGamma( screen[i], &oldGamma[i] );
		if( noErr != error )
			goto exit;
	}
	

	//Loop until we run out of time, resetting the gamma as fast as possible
	for( double currentTime = GetCurrentTime(); currentTime < endTime; currentTime = GetCurrentTime() )
	{
		//The fraction is the percentage of time that we have spend so far in this loop as a value between 0 and 1.0
		double fraction = (currentTime - startTime) / seconds;	
		
		for( i = 0; i < count; i++ )
		{
			
			//Calculate the new gamma based on the amount of time spent so far
			//(1-fraction)startGamma + fraction * endGamma = startGamma + fraction( endGamma - startGamma)
			for( int j = 0; j < kGammaTableEntryCount; j++ )
			{
				currentGamma[i].red[j] = oldGamma[i].red[j] + fraction * (newGamma[i].red[j] - oldGamma[i].red[j]);
				currentGamma[i].green[j] = oldGamma[i].green[j] + fraction * (newGamma[i].green[j] - oldGamma[i].green[j]);
				currentGamma[i].blue[j] = oldGamma[i].blue[j] + fraction * (newGamma[i].blue[j] - oldGamma[i].blue[j]);		
			}
			
			error = SetGamma( screen[i], &currentGamma[i] );
	
			if( noErr != error )
				goto finish;
		}

	}

finish:	
	if( error )
		DEBUGMESSAGE( "Got error # " << error << " doing a gamma fade in FadeToGamma(). Attempting to achieve the final gamma...." );
		
	for( i = 0; i < count; i++ )
		 SetGamma( screen[i], &newGamma[i] );

exit:
	DisposePtr( Ptr( oldGamma ) );
	DisposePtr( Ptr( currentGamma ) );

	return error;
}

OSStatus		AcquireDesktop( ScreenRef screen, WindowRef *window, const RGBColor *background )
{
	RLDisplay	*display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->AcquireDesktop( window, background );
}

/* Revert to the normal windowing mode	*/
OSStatus		ReleaseDesktop( ScreenRef screen )
{
	RLDisplay	*display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->ReleaseDesktop();
}

#pragma mark -

//New in RezLib 1.1

//Save the desktop icon positions
OSStatus		SaveIconPositions( ScreenRef screen, Handle *desktopInfo )
{
	RLDisplay	*display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->SaveIconPositions( desktopInfo );
}

OSStatus		RestoreIconPositions( ScreenRef screen, Handle desktopInfo )
{
	RLDisplay	*display = (RLDisplay *) screen;
	
	if( ! display )
		return noErr;
	
	return display->RestoreIconPositions( desktopInfo );
}

OSStatus		GetRezLibVersion( NumVersion	*version )
{
	if( NULL == version )
		return paramErr;
		
	BlockMoveData( &gRLVersion, version, sizeof( NumVersion ) );
	
	return noErr;	
}


#pragma mark -

//
//	The RetraceProc class family installs a callback that is called when the display device does its vertical
//	retrace.  The RetraceProc class itself is a virtual base class, not functional on its own
//

// Calls the callback if there is one
void RetraceProc::RetraceFunction()
{
	if( proc )
		(*proc)( userData );
}
			
RetraceProc::~RetraceProc()
{
	proc = NULL;
}

RetraceProc::RetraceProc( RLDisplay *the_owner, VerticalRetraceProcPtr the_proc, long the_Data ) throw (OSStatus)
{
	proc = the_proc;
	userData = the_Data;
	owner = the_owner;
}


