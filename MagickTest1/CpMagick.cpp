// CpMagick.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CpMagick.h"


// This is an example of an exported variable
CpMagick_API int nCpMagick=0;

// added variables-------------------------------------

char pathDrop[MAX_PATH];


// This is an example of an exported function.
CpMagick_API int fnCpMagick(void)
{
	return 42;
}

// added exports-------------------------------------

extern "C" CpMagick_API void fnPopMsgBox(void) {
	MessageBox(	NULL, // no owner window
				L"MagickTest did this!", // message to be displayed
				L"MagickTest MessageBox()", // caption
				MB_OK | MB_ICONINFORMATION );
}

// This is the constructor of a class that has been exported.
// see CpMagick.h for the class definition
CCpMagick::CCpMagick()
{
	return;
}
