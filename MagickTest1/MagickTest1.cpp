// MagickTest1.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MagickTest1.h"


// This is an example of an exported variable
MAGICKTEST1_API int nMagickTest1=0;

// added variables-------------------------------------

char pathDrop[MAX_PATH];


// This is an example of an exported function.
MAGICKTEST1_API int fnMagickTest1(void)
{
	return 42;
}

// added exports-------------------------------------

extern "C" MAGICKTEST1_API void fnPopMsgBox(void) {
	MessageBox(	NULL, // no owner window
				L"MagickTest did this!", // message to be displayed
				L"MagickTest MessageBox()", // caption
				MB_OK | MB_ICONINFORMATION );
}

// This is the constructor of a class that has been exported.
// see MagickTest1.h for the class definition
CMagickTest1::CMagickTest1()
{
	return;
}
