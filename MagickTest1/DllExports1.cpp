// DllExports1 : Defines AA-defined exported functions for the DLL application.
//

#include "stdafx.h"
#include "MagickTest1.h"

extern char pathDrop[MAX_PATH];


__declspec(dllexport) void AaExport01 (void) {
	int x = 0;
}

__declspec(dllexport) int AaSetPath (char inputPath[3]) {

	if ( "" != inputPath )
	{	
		strcpy_s( pathDrop, inputPath );
		
		MessageBox(	NULL, // no owner window
				L"why",//(LPCWSTR)pathDrop, // message to be displayed
				L"pathDrop is now:", // caption
				MB_OK | MB_ICONINFORMATION );
		return S_OK;

	}
	else {

		MessageBox(	NULL, // no owner window
				L"inputPath is NULL", // message to be displayed
				L"Error", // caption
				MB_OK | MB_ICONERROR );
		return E_INVALIDARG;
	}

}