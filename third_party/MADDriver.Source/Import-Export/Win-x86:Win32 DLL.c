/* 	
	This is a basic win32 DLL.
	Although not required, this DLL has a DllMain() entry/exit function, which is used when
	any per-process or per-thread initialization (such as memory allocation/release) has to
	be performed.
	
	Exporting DLL functions is accomplished here with the use of the __declspec(dllexport)
	storage class specifier; this prompts the linker to generate a stub file with a .dll.lib
	extension that must be included in the project for the application that uses the DLL.
	
	Importing the functions in the DLL's host application is accomplished by simply declaring
	them as extern, or with the __declspec(dllimport) storage class specifier (The latter 
	method is preferred, since it allows slightly more efficient code to be produced.
	(More information on DLL's can be found in "Advanced Windows: The Developer's Guide
	to the Win32 API for Windows NT 3.5 and Windows 95", by Jeffrey Richter and "Programming
	Windows 95", by Charles Petzold)
	
	Lucien S.
	Metrowerks Tech. Support.
*/
#include <windows.h>
#include <stdio.h>
#define EXP __declspec(dllexport)

BOOL WINAPI DllMain ( HINSTANCE hInst, DWORD wDataSeg, LPVOID lpvReserved );

EXP int RectFrame(HDC hdc, int x1, int y1, int x2, int y2, int t);
EXP int EllipseFrame(HDC hdc, int x1, int y1, int x2, int y2, int t);

					
BOOL WINAPI DllMain( HINSTANCE hInst, DWORD wDataSeg, LPVOID lpReserved )
{
	switch(wDataSeg)
	{
    	case DLL_PROCESS_ATTACH:
    	/*	MessageBox ( GetFocus(),
                  "attaching",
                  "Process attaching to MOD.dll...",
                  MB_OK|MB_SYSTEMMODAL );*/
      		return 1;
	  		break;
		case DLL_PROCESS_DETACH:
		/*	MessageBox ( GetFocus(),
                  "detaching",
                  "Process detaching from MOD.dll...",
                  MB_OK|MB_SYSTEMMODAL );*/
            return 1;
			break;

	  	default:
	  		return 1;
      		break;
   	}
   	
   	return 0;
}