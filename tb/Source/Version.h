// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// Version.h
// © Mark Tully 2000
// 21/1/00
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
	Stable version history

	21/12/00		Mark  : Just do some compiling revision, hmmm... I'll just write an arithmetic expression parser. Well that
							was easy, I'll add a print statement...hmm..if I add a few more statements then I could go somewhere
							with this...
	
	0.01 (21/1/00)	Mark  : History started - John joins in
	0.02 (23/1/00)  Mark  : Added procedures (fixed bugs) + stopped window appearing unless it has been used or option is
							held down
					John  : Added fading, beep & new timing code
	0.03			John  : Added sound, mouse hiding and mouse coordinates
	0.04 (4/2/00)	Mark  : Stuck a pp environment around the interpreter allowing you to open a TNT basic file which it then
							runs. Still needs a lot of work but at least gives us double click execution of TNT Basic files.
							Also made the screen fade up & show the cursor if a run time exception occurs
							Also can press esc to goto menus, terminate the current program or restart it
							Fixed a bug with the array implementation where they weren't being stored in the stack frame
							meaning we couldn't recurse on them.
					John  : Resets Kaboom on program termination, pauses and resumes sounds on program pause/resume
	
	0.05 (15/3/00)	Mark  : Updated Splat and TNT Basic Templates to work with the new CBLImageBank code which had the
							shrink to contents. Improved the caching code for the image bank so that a seperate file could
							be used for the cached encoded images.
							Added preprocessing of the source file to stop case sensitivity of language keywords
							Moved to a new console instead of SIOUX
	
	0.06 (19/3/00)	Mark  : Figured why it was crashing when parsing files containing © or Ñ characters. You have to tick use
							8-bit scanner in the flex prefs. Also I spotted the case insensitive option which kinda made
							writing the case insensitive preprocessor a waste of time. Doh.
							Corrected a minor bug where errors weren't getting output to the new console.

	0.07 (20/3/00)  Mark  : Reports the line number and prints the source code of a runtime error now which is a bit better
							than just printing an error code which could have come from anywhere.
							Got rid of the Preprocess code and changed it to use the case insens option from the lexer instead
							Fixed VBL syncing - it had got broken by two assignments to mNextBlitTime, one in the
							CGraphicsContext16 constructor and one in the SetFrameRate method. As far as I could see they
							weren't needed anyway so I got rid.
							Corrected a couple of bugs in the Draw Frame and SetVBLSync statements which didn't check that
							it was in graphics mode before using the context.
							Added an explicit load images command and stopped the auto loading of image bank 128.
							Fixed a memory leak where encoding images weren't getting disposed of when the bank was deleted. (Duh!)
							Shows the cursor properly when EndExecution is called

	0.08 (21/3/00)	John  : Started to implement networking.
					Mark  : Implemented shared and global variables. Fixed a small bug where the last CR wasn't getting added
							to the end of the file correctly before parsing.
							
	0.09 (3/4/00)	John  : Killed some bugs.  Progressed on Networking. Finished Input Sprocket.
					Mark  : Fixed procedures bug.
	
	0.10 (10/4/00)	John  : Fixed array negative index bug
					Mark  : Fixed a shift reduce conflict in the Host Network Game function. From now on, all FUNCTIONS which
							take parameters must use brackets. Eg, Poll Input 1 became Poll Input(1) otherwise expressions
							get out of their face and cause loadsa shift reduce conflicts
							Also fixed a bug where an ENDLINE was for some reason missing from the GLOBAL statement which
							caused errors when parsing Splat.
	
	0.11 (12/4/00)	Mark  : Promoted the canvas classes to be part of BLAST, renaming them CBLCanvas from CCanvas.
	     (16/4/00)			Added music,text, multiple canvases, matrix fx
							Implemented good reporting of linker errors, first use of parameterised error messages
							Added a hide/show FPS menu item
	     
							BUGS FIXED
							ÑÑÑÑÑÑÑÑÑÑ
							Fixed a bug where graphics stopped working if you paused and switched to another app; made
							CGraphicsContext16 reallocate the screen ABCRec when resuming.
							Fixed a bug in if structures where the program would terminate if there were 2 end ifs in a row.

							MEMORY LEAKS PLUGGED
							ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
							Fixed a bug where CSpriteManager::ClearImageBank wasn't clearing the image bank
							Fixed leak where image handles weren't getting released
							Fixed leak where CCanvas class wasn't disposing of it's ABCRec

							New Commands
							ÑÑÑÑÑÑÑÑÑÑÑÑ
							End					Exits the program
							Sprite Off [num]		Turns sprite num off or all sprites off if num not specified.
							Clear Canvas			Clears the canvas in the current back colour (defaults to black)
							Load Music resid		Loads a MADI music into the driver
							Play Music			Sets the currently loaded music playing
							Stop Music			Stops the music if playing
							Get Music Length		Returns the length in ticks of the music
							Get Music Position	Returns the index into the music in ticks
							Set Music Position	Changes the index into the music (ticks)
							Open Canvas			Opens a canvas
							Close Canvas			Closes a canvas
							Canvas				Targets a canvas for future drawing operations
							Copy Canvas			Image transfer from one canvas to another
							FX Matrix			Apply a matrix to a canvas
							Clear Canvas			Erase the canvas
							Draw Picture			Now takes an optional screen which is opened to the size of the picture
							true/false			Finally put them in
							Text Size			Any size >=0
							Text Face			MacOS style parameter
							Text Font			Specify font by name
							Draw Text			Draws text at the current settings
							Str$					Convert an int to a str
		
	0.12 (24/4/00) Mark   : Fixed leaks to do with virtual destructors. Did you know that if you delete a base class ptr the
							sub class's destructor is only called if the base classes was virtual? Of course you did. But
							did you realise that this means all your bases classes which DON'T have destructors are going to
							need an empty virtual destructor declaring for them?
							Fixed gworld leak everytime a mask was used to encode an image, fixed ° mem leak with parse tree
							not getting deleted beyond statement 1. Fixed arith expression leaks in logic and arith ops
							and loads of not deleting the arith expression bugs in statements.
							Added support for DPads to the input sprocket class
	
	0.13 (25/4/00) Mark	  : Added support for floating point numbers, can do assignment, arith and binary comparisons but
							you can't currently cast to integer, pass/return from procs, put in data/read statements,
							declare as shared or global.
							
	0.14 (1/5/00)  Mark	  : Fixed bugs with StArithExpression and co which were introduced at 0.12
				   John   : Added ms timing code to the fade commands
				   
	0.15 (19/5/00) John   : Added tmap import and access etc.
	
	0.16 (20/5/00) Mark	  : Finished off floats, including Data statements, arrays, parameter passing, procedure results,
							globals and implicit conversions.
				   John   : Fixed a bug in networking, fixed bug in timing code for fades
				   
	0.17 (7/6/00)  Mark	  : Fixed precendence bug with the combining of boolean and numeric expressions
							Rearranged tokens and put in some compile time conditional code in the lexer for writers' block
				   John   : Added maths commands for tan, sin, cos and their inverses in both degrees and radians
				 		   Added delay and get timer. Changed networking to be able to send ints, floats and strings and
				 		   added some resource exist commands and changed fades to use 0-255 component fade colours.
				 		   
	0.18 (9/6/00)  John	  : Fixed networking and optimised it.
							Added drawing functions.
				   Mark	  : Can now run resource files which executes 'Basc' 128 if available. Also stopped polling the
							keyboard as often for cmd-. and esc in the interpret loop, should speed things up a bit.
							Changed handling of OpenDoc AE's so that they retrieve a return to app parameter allowing them
							to switch back to an app after execution is complete.
							
	0.19 (13/6/00) Mark	  : Fixed run time exception propagation from conditional blocks which was leading to unsourced rt
							exceptions
		 (14/6/00)		  : Fixed error reporting for "for" "if" "while" where they indicated the wrong line if the evaluation
		 					of their expression had an error.
		 					Weakened the comparison of the variable declared as the induction var and the one declared in the
		 					next statement of a for loop so that array items could be used as induction vars without error.
		 					No longer switches process back if the text window is open
		 					Added one line if statements
		 					Fixed a bug in the random statement
		 					Fixed a bug in the globals statement for lists
		 		   John   : Removed Up,Down,Left and Right and replaced with Is Pressed
		 		   			Removed optional canvas parameter from drawing commands
		 		   			Renamed Ceil, Round and Floor to not have hashes
		 		   		   
	0.20 (16/6/00) Mark	  : Fixed random number gen to gen max number
	
	0.21 (18/6/00) Mark	  : Rewrote the type system. Must now declare all variables and you no longer need to append $ or #
		 	...				to vars. Allow initialization on global lines too.
		 (23/6/00)	   		Renamed "Param" etc to "Result Int" "Result Str" "Result Float".
							Added local data statements for procedures
							Added forward referencing gotos
							Added restore command for data statements
							Renamed DegToRad as Radians and RadToDeg as Degrees
							Removed # and $ from commands
				   John   : Added sprite transparency and colour
							Add Paste Sprite command
							Removed Get Keys
							Started Collisions
	
	M1   (23/6/00) Mark   : Fixed variables being case sensitive all of a sudden.
	M2	 (24/6/00) Mark	  : Fixed problem with declaring array variables with dimensions read from other variables
	M3	 (25/6/00) Mark	  : Dropped automatic drawing mode, decided it was more hassel than it was worth.
							Tidied up Canvas class in light of this.
							Fixed a bug with variable storage which crashed macs
							Added a preprocessor which generates a list of procedures allowing the basic lexer to differentiate
							between vars and procs easier.
	0.22 (25/6/00)
	
	M1	 (25/6/00) Mark	  : Renamed the CStringNode class to CProcNode and extended it with proc specific info
	M2	 (29/6/00) Mark	  : Began the restructing of the way TNT Basic handles its language functions so that error handling
							is possible
	M3	 (2/7/00)  Mark	  : Finished the new parser and lexer changes, now tokens locations can be tracked from the parser
							and error reporting can log an error, underline the exact offending token and then resume parsing,
							only does it for a limited number of statements at the moment.
				   John	  : Added sprite collisions.
							Added Wait Mouse Down and Wait Mouse Up.
							Fixed a bug with input sprocket loading reources.
							Fixed a bug with sprite indexes being zero.
	0.23 (3/7/00)
	
	M1	 (9/7/00)  Mark   : Fixed a bug in inited variable lists when doing int x=mystrvar and the like
							Fixed signalling which had become disabled due to botched pch-es
							Finished the simple type check procedure
							Fixed a bug with floating point maths where if assigning the result to an int accuracy could be
							so lost that the answer was just wrong.
							Fixed crap error message produced when load sounds couldn't find a resource
	
	M2	 (12/7/00) Mark   : Seriously hurt myself trying to get error recovery working. We can now parse, detect errors and
							recover from pretty much all errors in expressions.
							Things which use the expressions now need fixing, I've fixed general statements, procedure
							calls, variable assignment (in declarations and explict assign statements) and array indexing.

	J1	 (13/7/00) John	  : Did buttons, moved statements over and fixed bugs in input sprocket and fading.
	
	0.24 (13/7/00)
	
	M1	 (14/7/00) Mark	  : More to error handling, can now parse errors in ifs, whiles, repeats and data statements.
	
	M2	 (15/7/00) Mark   : Fixed bugs in procedures, shared statements and did even more to error reporting. (Yawn)
	
	J1   (15/7/00) John   : Moved all the arith functions and str functions over to type checking system. (Yawn °)
							Added Button Polling.
							Fixed various update bugs.
							Improved Wait Mouse Up and Wait Mouse Down.
							
	0.25 (15/7/00)
	
	J1	 (22/7/00) John	  : Added viewports and scrolling.
							Added pen transparency.
							Added custom drawing of lines and rects.
							Fixed optional paramter type checking.
							Fixed missing parameter error reporting.
							Added Draw Map Section command.
							
	M1   (17/7/00) Mark   : Fixed a bug in the construction of the close canvas statmement (added check for nil)
							Fixed bug in for loops
							Added more error handling to for loop parser
							Turned off surpression of var not decl statements in var decl lines as code like
							int x=y wasn't logging the errors it should have been doing.
							
	0.26 (25/7/00)
	
	M1   (26/7/00) Mark   : Improved the parsing of optional parameters to statements making it ignore trailing commas and stuff
							Added Draw Tracking On/Off commands

	J1   (26/7/00) John   : Fixed a bug with collisions.
							Changed some operations to use the work canvas.
	
	0.27 (26/7/00)
	
	M1   (27/7/00) Mark   : Fixed a bug with redeclared variable errors getted incorrectly handled

	J1	 (27/7/00) John   : Add Up,Down,Left,Right and Space
							Fixed a bug with sprites
							Changed names of functions
							Changed image bank format to take mask colours in range 0 to 255

	0.28
	
	0.29   "       Mark   : Started support for build exec, fixed bugs
				   John   : Fixed bugs, removed set creator and moved to prog info
				   
	M1   (28/7/00) Mark   : Finished the build app code, can now make standalone applets from Hieroglyph
	
	J1	 (30/7/00) John	  : Sped up encoding.
							Changed Maps and Buttons to use expanding array.
							Added multiple sprite banks.
							Fixed a bug with input sprocket losing data.
							Added transparency to frame poly
							
	0.30 (30/7/00)
	
	J1   (1/8/00)  John   : Added transparency to fill oval and frame oval.
	
	J2	 (26/9/00) John	  : Added transparency to fill poly.
							Updated command names to the new standard.
							Added Nth Resource Id, Mod and Make Colour commands.
					
	M1			   Mark   : Updated to PP2.1 and Pro6
							Did FP version of Mod
	
	0.31 (28/9/00)
	
	0.32 (6/10/00) John	  : Fixed a bug with edit input where the background wasn't getting refreshed
	
	J1   (14/10/00) John  : Fixed a bug with failing to load TNT Maps occasionally
							Fixed a bug with loasing track of sprites with multiple canvases
							Added a square root command
							Fixed a bug with buttons appearing in front of sprites
							Added some string functions
							Added sprite priorities.
							Began carbonating and moved over to RezLib from dsp
							Added set map tile and the ability to specify resources by name
	
	M1	(4/12/00)   Mark  : Merged a modified RezLib into the app
	
	0.33 (4/12/00)
	
	M1	(25/12/00)	Mark  : Changed error handling code to collect errors and stick em on the reply ae
							Fixed crashing bug when running from Hieroglyph.
	
	J1	(26/12/00)	John  : Fixed update events.
							Fixed 63 and 38 decoding with alpha channel (Oh no we didn't!)
	
	0.34 (26/12/00)
	
	M1 (27/12/00)	Mark  : TB switches to being the frontmost process before running it's program after a
							successful parse
							Now only surpresses error output if being ran from glyph, shows errors if ran from finder
	M2						Fixed a bug with turning vbl syncing off resetting fr to 60
							Added bkgconsole debugging calls - run BkgConsole to see debug info or open text file
							System Folder:Console:TNT Basic.MAIN
							Fixed a bug with tb spawning blanking windows whenever it was paused.
							Made tb restore the prev resolution when it is paused
							Made tb handle requests from glyph to run another program when one is loaded and paused
							
	J1 (7/1/01)		John  : Improved sound to support active panning	
					John  : Fixed transparency decoders and added Wait Mouse Click command
	
	0.35 (31/12/00)
	
	M1	(3/1/01)	Mark  : Now waits for a resume event before running its program. This elimiates the rare event of running
							the program in the background and also fixes a problem with the cursor not being inited.
							
	0.36 (9/1/01)
	
	J1 (14/1/01)	John  : Fixed bugs with sound corrupting memory when playing.
							Stopped the screen fading during a pause when not in graphics mode.
							Added Mp3 music playing.

	M1 (1/3/01)		Mark  : Fixed lazy evaluation in expressions
							Altered the identifier rule to pick up missing [ after array identifiers
							Fixed mem leak : CGeneralExpression now delete's its tail
							Can now declare an undimensioned array again - eg int n[]
							Fixed array declaration bug, you can now once more dimension an undimensioned but declared array,
							now also capture more mangled dim statement errors
							You can now redimension arrays by using the dim command

	0.37 (7/3/01)


	M1 (14/3/01)	Mark  : Changed the blast sprite transp ingredient to be called an opacity ingredient because transp=100
							was being seen as completely opaque instead of transparent. Same for the pen transp on canvases.
							Change code so that where you say "set sprite transparency 100" it sez "set sprite transparency 0"
							and vice versa.
							Same for set pen transparency
							
							Added some function for working with image banks:
							Count Images (bank)
							Image Width (image,[bank])
							Image Height (image,[bank])
							Image X Offset (image,[bank])
							Image Y Offset (image,[bank])

		(15/3/01)	Mark  : Added white, black, red, green, blue and yellow functions as colour presets. Makes it easier to
							draw in the common colours and makes code more readable when doing so 
							Fixed mem leak with exact collision masks
							Added:
							copy image x1,y1,x2,y2,maskcolour to image,[bank]
							copy image x1,y1,x2,y2,maskcanvas,maskx,masky to image,[bank]
							
	M2 (23/3/01)	Mark  : Added:
							set image offsets image,[bank],xoff,yoff		   
							break [numexits=1]
	
	M3 (26/3/01)	Mark  : Updated the about box (we really should db-ize this)
							Bug fixes
							Removed crasher in elseif parsing
							Logs unexpected characters in the error log
							Logs variable name/procedure name conflicts in the error log

					John  : CInputManager template format simplfied
							Fixed crasher when encoding small images

	0.38 (26/3/01)
	
	J1 (3/4/01)		John  : Added volume controls for sound and music
							Fixed crasher encoding invisible sprites
							Fixed a bug when playing mods
	
	M1 (5/4/01)		Mark  : Fixed crash if app name too long (stupid bug in third party debugging library - tsk...)
							Fixed scroll bar not being enabled when a lot of text is output
							If too much text is output to the console then it now throws away the oldest test instead of
							crashing
							Doesn't flush events after mouse testing operations, define TB_FlushEventsAllTheFeckinTime in
							a header to reenable it
							Added test code ae handler
	
	0.39 (5/4/01)
	
	M1	(7/4/01)	Mark  : Fixed another procedure var name signal if the procedure name was declared as a var and assigned
							on the same line.
							Stopped copy image interpretting x2,y2 as width and height and correct bug where image was put
							in wrong location in the bank
							Made set music volume and get music volume work for mods
		(9/4/01)			Changed our random number generator to the stl one so our numbers aren't predictable random numbers
							anymore :)
							
	M2	(15/4/01)	Mark  : Fixed bug with text output switching port from the graphics context and screwing up mouse coord
							tracking from then on
							Inits the cursor when entering graphics mode to catch problems with the I bar cursor being left on
							if the tb text console was open
							Fixed encoding orgin bug when gs mask was used with non zero origin and shrink to contents set to true
							
   	J1 (24/4/01)	John  : Fixed the Unload Images crash when displaying an image being unloaded
   							Added delete image command and collision tolerance accessors
   	
   	0.40 (9/4/01)

	M1 (14/5/01)	Mark  : Delete image was a bit broke. Fixed a bug where it got the image index confused with the bank id,
							added a range check to the image index to stop a crash and made it actually deleted the image (!).							
							Fixed a bug in unload images and delete image where it was assuming that the image was from the
							sprite's current bank which needn't be true if the sprite's bank has been changed with set sprite
							bank.
							Finally moved load and unload images to their rightful homeland - Image_Statements

	J1 (15/5/01)	John  : Added file access commands
	
	M1 (18/5/01)	Mark  : Updated to newer parser support stuff from MR - some of tb's parsing functions are now in a namespace
							called TBParser
							Also changed to carbon MSL C++ Runtime.Lib. Now we crash at a different point under os x. :)
	
	J2 (18/5/01)	John  : Changed Kaboom to use Sound Result
							Changed File Access to be more Mac OS X like by using both types of preferences


	0.41 (1/6/01)
	
	M1 (6/6/01)		Mark  : Made in run under x
							Needed new runtime libs, new rezlib and new madlib.
							Under x it's crippled - no input, networking and graphics don't get to screen.
							Not sure if music works - it certainly doesn't support as many formats.
							At least our console works :)
							
	J1 (8/6/01)		John  : Changed names of some file functions and changed some functions to statements and stuff.

	0.42 (8/6/01)
	
	M1 (15/6/01)	Mark  : Messed with port updating under x
	
	J1 (20/6/01)	John  : Fixed lack of updating in graphics mode after network and file dialogs are dismissed.

	M2 (17/7/01)	Mark  : Fixed bug with line drawing not invalling its rect properly
							Fixed a bug in for loop termination which could exit the program if there was no statement in the
							same scope after the loops "next"

	0.43 (20/7/01)
	
	J1 (21/8/01)	John  : Carbonated networking.
							Fixed unpause lock up bug.
							Conditionalised the quit menu item.
	
	0.44 (19/9/01)
	
	J1 (29/10/01)	John  : Added a couple of flushport calls to ensure the screen was updated
	
	M1 (30/10/01)	Mark  : Moved to CW 7: added a couple of PP files
							Wrote a makefile in the parser folder to run bison and flex on the tb parser files under os x.
							Did this cos the cw plugs don't work under cw on x. To use, go to the terminal and cd into the
							parser folder of tb's source, then type "make".
							Under classic, you can still use Flex and Bison by making the target "Classic - Make Parsers".
							Any files you add to the project should be added to "TNT Basic" and not "Classic - Make Parsers".							
	M2 (2/11/01)	Mark  : Moved over to TNT Music Engine, mixing mod playback in the process
	M3 (5/11/01)	Mark  : Added music looping commands and an is music playing function. Fixed bug in RezLib that was
							crashing us under 9. Made volume nad music looping be program level variables, ie applied to
							all musics loaded by the program rather than just the current one.
							Functions:
									int		music playing			' is music currently playing or has it finished?
							Statements:
											music loop on			' music will restart when done
											music loop off			' NB: applies to all musics loaded from this point on
																	'     as do the music volume settings as of TB 0.44M3
							* New feature *
							You can now take screen shots of the graphic context with cmd-f12 or by using the "Take screenshot"
							menu command. The files are saved into the user's "Pictures" folder under OS X as Preview.app
							documents, and onto the Desktop under 9 as Picture Viewer documents.

	0.45 (5/11/01)				
	
	M1 (19/11/01)			New scrolling text about box
	M2						Moved error codes into a separate resource file so they could be added to tbb.
							Added URegistration class with nags if using tb too much without registering
	M3						Put in reg dbox and did name validation and saving. Just need to display regged name in box now
							Prefs are saved in a hidden file called ".globule" (==URegistration::kHiddenFile)
	M4 (21/11/01)			Now displays regged name and supports a query registration ae	
	J1 (22/11/01)			Americanis(z)ed the lexer
	
	0.46 (22/11/01)
	
	M1 (28/11/01)			Now auto installs help if it's in the same folder as tb
							Added help menu to get to user guide and tb help centre
							Merged open play into project
	
	M2 (29/11/01)			Really really merged open play into the project
							Made find folder calls more compatible to work on 8.6
	
	1.00 (29/11/01)			Bump revved for public consumption
	
	1.00 (2/12/01)			Fixed "Register" menu item being enabled in built apps
	(build 2)				Fixed about box spelling error
	
	1.00M1 (10/12/01)		Fixed rare parser bug which crashed tb. Was caused by failiure to init $$ to zero when a statement
							to assign an undeclared variable was encountered.
							Fixed corrupted text in about box under os 9 that was caused by unlocked handle
							Added code to save and restore desktop icon positions, but it didn't seem to be causing a problem
							on my machine anyway, hope this has fixed it.
							Fixed set image collisions incorrect type check.
							Now avoids selecting unsafe or stretched resolutions
	
	1.00M2 (18/12/01)		Changed it back to selecting unsafe resolutions, but only selecting stretched as a last resort
							Added new error code for when a rez is unavailable

	1.00M3 (19/12/01)		Merged with john's changes: new commands min, max, wrap and as number.
							More error messages from input sprocket. Corrected 1 off error in isp polling commands.
	
	1.01b1 (20/12/01)		Stable beta 1
	
	1.01b1M1 (21/12/01)		Updated UHelp class to replace existing aliases in the help folder if they don't refer to the correct
							folders. This is needed because people who run tb 1.01 over tb 1.00 don't want to be using the help
							from 1.00. Especially if they've deleted it :)
							Fixed problem with undefined label errors not having a range associated with them, this caused a signal
							to be raised in TBB when reporting them.
							*** Confirmed that the save desktop icons code doesn't work, it's not implemented in RezLib! ***
							Fixed sprite rect based collision bugs. Sprites now have to overlap before a collision is registered

	1.01b1M2 (8/1/02)		Fixed mouse coords bug when window was smaller than screen. Mouse X, Mouse Y and buttons were affected
							Fixed blanking window not being black under OS X when window smaller than screen
							Fixed bug where it would nag you even if you'd registered
							Added an infix power operator, ^
							Now uses a simple XOR encryption method to hide source code in built apps
	
	1.01b2 (8/1/02)			Stable beta 2

	1.01	(10/1/02)		Changed take screenshot shortcut to cmd-F11 cos it cmd-F12 stopped working under OS 10.1.2
							Fixed dpad use which simply didn't work with the new style input resources. That's only
							gone unnoticed for about a year...
	
	1.01M1 (21/1/02)		Now uses a carbon events main loop to receive events, this gives us more cpu time on OS X.
							Blocking until draw time is now done using either receive next event or semaphores.
							Waiting for mouse clicks/ups/down is also done via carbon events
							Delay can now be cancelled/escaped and it blocks nicely
							Wait Button Click now tracks mouse movments/clicks using carbon events, so it only uses cpu
							when it needs to.
							Now uses a carbon timer for the music engine tickling.
							Side effect - No more spinning wheel cursor on OS X							

	1.01M2 (22/1/02)		Fixed type check of set map tile statement, it was documented and implemented to take 4 args but
							typed checked to 3.
							Fixed mem leak in get file type and file read string functions, which were allocating and leaking
							CCStrings.
							New commands:
							string = Map Polygon Name (int)		Returns name of nth polygon
							int = Map Polygon X (int)			Returns left most boundary of polygon
							int = Map Polygon Y (int)
							int = Map Polygon Width (int)
							int = Map Polygon Height (int)

	1.02b1 (23/1/02)		Stable beta 1
	
	1.02b1M1 (23/1/02)		Fixed a bug in the lexer where commands that were subsets of other commands weren't split proper
							e.g.
							sprite collisionId,x,y,i
							was resulting in
							'sprite col' 'lisionId' ',' 'x' ',' 'y' ',' 'i'
							Have to modify all rules that complete contain another rule, see note on use of 'eoc' in the
							.lex file.

	1.02b1M2 (29/1/02)		Fixed crash with 'sprite off -1' (aka 'unload images' bug)
							Removed triple buffering under X
							Enabled blast copy world to world for speed boost
							(At some version previously:) Fixed sprite collision detection across layers, this fixed buttons
							in viewports.
							Enabled blast copying of doubles if source and dest are aligned. Should give good boost for larger
							copies.
							Collision detection is now more efficient at handling detections with non-existant sprites
	
	1.02b1M3 (30/1/02)		Fixed pausing and unpausing on OS X
							Added 2 new commands
							int = Map Tile Width				Returns width in pixels of tiles in current map
							int = Map Tile Height				Returns 0 if no map loaded
	
	1.02b2					Stable beta 2
	
	1.02					Changed nagging to every 5 runs instead of every 20 cos we're not getting enough regs.
							Bump revved for release
	
	1.02M1 (6/2/02)			Added tile scrolling (currently hard coded and has no sprite support)
	1.02M2 (7/2/02)			Finished tile scrolling, new command to document:
								open map viewport canvasToUse,layerToRender,x1,y1 to x2,y2
	
	1.02M3 (17/2/02)		Added moan channels to TB sprites
							New commands:
								sprite x, sprite y, sprite image, image bank : accessors for sprite attributes
								move sprite sprite,xdist,ydist,speed	: moan async anim command
								
	1.02M4 (21/2/02)		Move sprite command now works, needs testing in particular
								negative speed movements
								checking the sprite always moves specified number of pixels
								see if sqrt can be eliminated
	
	1.02M5 (24/2/02)		Removed sqrt from move sprite command for a couple of common situations
							Added 'sprite moving' and 'sprite animating' functions.
							Added 'animate sprite' command, needs stress testing some more.
	
	1.02M6 (17/3/02)		Fixed bug in BlastCopyAligned
							Added: channel moving, channel animating, add viewport to channel, remove sprite from channel,
							remove viewport from channel, auto moan on, auto moan off, auto moaning and step moan.

	1.02M7 (22/3/02)		Finally finished implementing and testing all commands from my Moan.doc list, moan is
							(hopefully) all finished now.
							Added workaround for odd bug in Mac OS X 10.1.3 where cmd-. and esc stopped working if another
							key was pressed first. Needs a proper fix, see CApplication::DoTask for more.
							Need to merge in john's networking fixes.
	
	1.02M8 (23/3/02)		Added a work around to the parsing error caused by a missing 'end proc', need to sort the
							parser error messages out for when there's missing 'end *'
	
	1.02M9 (29/3/02)		Fixed bug in moan compiler with negative moan move speeds
							Added 'channel running' accessor
							Fixed bug with escaped " in strings
	
	1.02M10 (30/3/02)		Upped event checking to every 20ms, that's 50 times a second
	
	1.03b1 (30/3/02)		Merged john's networking changes in, left ogl stuff for now.
	
	1.03b1M1 (4/5/02)		Fixed bug with declaring globals inside procedures
							Fixed bug with playing mp3s that were open in glyph
							Fixed bug with fps sprite using last set font instead of system fond
							Displays proper error on missing end proc statements
							Captures more garbled proc name patterns
							Fixed array index out of bounds being reported on the wrong line if occuring in the condition
							part of an if then statement
	
	1.03	(17/4/02)		Bump revved for release
	
	1.03M1	(29/4/02)		Fixed BlastCopyDoubleAligned16 bug
	
	1.03M1J1 (17/5/02)		Merged with John's OGL stuff
	
	1.03M2J1 (9/7/02)		Minor updates to the file stuff (case insensitive on the format string)
							Clear dirty rgn before all RNE calls, set it to full screen after
	
	1.03M3J1 (9/7/02)		Added 'text width' 'text height' 'text descent' commands
	
	1.03M4J1 (13/7/02)		as string now works with floats. Optional second parameter for num decimal places
	
	1.03M5J1 (18/7/02)		Bump rev for patch release of another possible flickering sprites fix.
	
	1.03M6J1 (30/7/02)		Fixed screen shots under software mode and OS X. Test under 9 and fix screen shots
							for non-sprites under hardware.
	
	1.03M7J1 (4/8/02)		Finished rotate and scale moan commands. Improved Move moan command to take float params for speed.
							as number now works with floats
	
	1.03M7J2 (6/8/02)		Merged Johns new networking stuff in, along with some OGL bug fixes
	
	1.03M8J2 (15/8/02)		Fixed bug with use of multiple unary minus in moan compiler
	
	1.1		(15/8/02)		Bump revved for release
	
	1.1M1	(6/9/02)		Moved to CW 8
	
	1.11	(5/10/02)		Gfx bug fix and John's collision bug fix
							Fixed crash with unload images when buttons were using doomed bank
	
	1.11M1	(24/12/02)		Possible fix for crash on resume on some machines -- when will I start
							remembering to lock the port before blitting....
							
	1.11M2	(28/12/02)		Fixed 'Set Sound Volume' which had never worked
							Fixed 'Disable Break' which had never worked on OS X
							Now selects highest scan rate res when 2 reses with the same dimensions
							are found
							Added 'left str' 'right str' and 'mid str' string accessor commands
	
	1.11M3	(2/1/03)		CGraphicsContext constructor locks screen around 'EraseAll' call - shouldn't make any difference?
							Can now handle errors from the CApplication constructor without crashing.
	
	1.11M4	(4/1/03)		Fixed colour constants 'red', 'green', 'blue' etc which were all broke for software mode
							Fixed fps font for hardware mode to use right res id
							Now backs up any console files from previous runs that weren't closed (ie any tb sessions that crashed)
							These files are appended to the file 'TNT Basic.MAIN.crash' in the consoles folder.
	
	1.11M5  (5/1/03)		Fixed a two big fat memory leaks introduced with opengl stuff, no canvases were being
							deleted and no images were being unloaded, noticed a 600Mb leak after running on os x for a while!
			
	1.11M6	(9/1/03)		Fixed a bug with input on os 9, where if load input had an error it threw the run time error
							without re-enabling keyboard and mouse - oops!
	
	1.11M7	(10/1/03)		Stopped logging of routine exceptions
	
	1.11M8	(13/1/03)		Fixed the UKaboom::LoadBank crash! Woohoo!
							Caused by UKaboom::Reset() not setting the sound handles to NULL when it released them, then
							ClearSounds() being called from LoadBank() releasing all handles that were non-null. The double
							release wasn't a problem most of the time as the handles were invalid and the os would ignore
							the request, but occasionally, it would hit a valid resource handle, in our case the SnBk resource!
							This could well have been causing other crashes too.
	
	1.11M9  (14/1/03)		Fixed long delay on quit/pause on os x. It was because our carbon timer that ran the game was
							scheduled to run every millisecond, but only ran once then locked in for the duration and ran
							the game. But I think the os was secretly scheduling a new call every millisecond while we were
							running and as soon as it quit it unleased them all and caused a long delay.
							Fixed by setting the timing interval to 0.
	
	1.12	(17/1/03)		Bump revved for release
	
	1.12J1	(25/2/03)		New Commands
							- Point In Rect
							- Map Line Col
							- Map Object Name
							- Calculate Angle
							- Angle Difference
							- Sprite Active
							- Sprite Crop
							- Bit Xor, Bit Or, Bit And, Set Bit, Bit Set

							Bug Fixes
							- Fixed hardware graphics mode resuming from pause
							- Fixed Set Sprite Colour under hardware mode
							- Fixed Fill Poly
							- Fixed Set Pixel Colour when using transparency
							- Fixed Genesis window resizing

	1.12M1J1 (1/4/03)		Fixed bug in sqrtf
	1.12M2J1 (23/7/03)		Fixed bug in open canvas that could leave the mTargettedCanvas ptr dandling
	1.12M3J1 (28/7/03)		Fixed crash bug with 'end proc' statements with () but no return value specified
	1.12M4J1 (29/7/03)		Fixed crash in delete image when deleting non existant image
							Fixed crash in for loop if using undeclared identifier in 'next' statement
	1.12M5J1 (3/8/03)		Now clears an image bank when the load images command is used
	1.12M6J1 (4/10/03)		Added lots of keyboard access stuff. TB now has a keyboard input queue.
							New commands to access the queue:
							"keyboard raw key" - reads a raw key from tb's new keyboard input buffer. a raw key is a UInt32 containing keyscancode, charcode, modifiers and some tb flags (currently just a flag saying if the key is an auto repeating key)
							"raw key ascii" - extract ascii code from a raw code
							"raw key char" - extract char str from raw code
							"raw key scancode" - extract scancode from raw code
							"char to ascii" - convert a char str to ascii number
							"ascii to char" - convert an ascii number to  a char string
							"set keyboard repeat on" - turn on/off keyboard auto repeating
							"set keyboard repeat off"		
							"keyboard repeat" - see if keyboard auto repeating is on
							"clear keyboard" - flush tb's keyboard input queue
							"wait key press" - wait until there's a key in tb's keyboard input queue (will probably want to call 'clear keyboard' before calling this)
							More to come...
	1.12M7J1 (16/10/03)		Added string editor, an editable text buffer that a tb program can send raw keys to to edit some
							text.
							"string editor contents" - reads the current contents
							"set string editor contents" - sets the current contents
							"string editor cursor" - reads the current cursor position
							"set string editor cursor" - sets the current cursor position
							"string editor input" - puts a key into the buffer at the current cursor position
							Also now uses carbon events to read the keyboard under os x
							Workaround for keytranslate() returning incorrect values for num keypad
							Possible fix for no gfx on emac
							
	1.12M8J1 (19/10/03)		Now saves 'show fps' option in preferences
							Fixed menu checking not unchecking bug
							
	1.12M9J1 (27/10/03)		Fixed mResolvedProc error cause by calling procedures from inside malformed if statements
							(now better at spotting missing conditions on if/while/until etc statements)
							Fixed 'map polygon name' and 'in map polygon' crash if passing a non-existent polyon index
	
	1.12M10J1 (27/10/03)	Added 'Bit Shift' command
	
	1.12M11J1 (2/11/03)		Added raw key filter commands
								raw key filter number
								raw key filter printable
								raw key filter navigation
								raw key filter delete
	
	1.2 (3/11/03)			Bump revved for releaes
	
	1.2M1 (9/11/03)			Fixed option, control and cmd keys not working with pressed command under os x
	1.2M2 (8/2/04)			Fixed crash when loading 256 colour images
	1.2M3 (17/3/04)			Fixed key release events getting lost if released while a file open dialog was on screen
	1.2M4 (29/3/04)			Fixed frame oval crash if using transp pen and drew off the edge of the canvas
	1.2M5 (14/6/05)			Program now uses CResourceContainer classes to access resources
							Done for all resource types, but sounds, music and input haven't yet been tested.
	1.2M6 (11/7/05)			Now builds app using CResourceContainer classes. Can build mac res file version, bundle folder is todo
							Improved error reporting for missing resources, now states resource type and id in error message
	1.2M7 (6/8/05)			Now customises the built apps plst to make the data files attached to it be proper data files, stops
							packages appearing as folders
							(need custom icons)
	1.2M8 (13/8/05)			Added first pass at window mode
	1.2M9 (8/9/05)			Fixed glitches in gl window + full screen mode due to pausing on os x
	1.2M10 (10/10/05)		Now updates FREF + BNDL + owner resources so that icons are correctly set for external data files in built apps under os 9
	1.3 (16/10/05)			Bump revved for release
	1.3M1 (29/1/06)			Fixed sounds not working in .tbproj files
							Fixed control key not registering
							Fixed image offsets being broken for corner presets
							Fixed file exist leaving files open and thus making them non-deletable (OS X doesn't close files when process exits either??!!!)
	1.3M2 (30/1/06)			Fixed mem leak in gsis map loading
							Fixed as string not working with user specified precisions
	1.31					Fixed building an app from a project with resources that collide with resources in the TB app generates dumb error messages.
							Now much more informative.
							Bump revved for release.
	1.31M1 (12/6/06)		Got it compiling in xcode, various minor source changes for gcc4 compliance
	1.4 (24/1/12)			Updated to work on Intel properly - now officially an OS X only app (finally...)
	1.41 (14/2/12)			Now able to load tbprojs again
							Should run on OS 10.5 Leopard now
	1.42 (7/4/12)			Fixed sounds not playing from tbproj files under Lion
*/

// Version info
#define	kTBLangVersion	"TNT Basic Language revision 8"	// (TB 1.3) update this whenever you change the lexer, will allow us to see if writers block needs updating

#define	verMajor		1
#define	verSub			4
#define	verSubSub		2
#define	verUnderName	kTBLangVersion // this goes under the name

#define	verVersionText	"1.42, © TNT Software 1999-2012" // This goes by the Version: field in the get info box
#define	verShortText	"1.42"
#define mainFunc	  	tb_1_42				// encode version into the name of 'main' so that the version number will be visible in the OS X crash logs
#define verStage		final

#define verShortTextP	"\p1.42"
