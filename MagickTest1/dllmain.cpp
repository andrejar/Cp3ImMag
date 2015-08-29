 // dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include "CpMagick.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	char* MagickDllPath = "S:\\ImageMagick\\VisualMagick\\bin"; //TODO: not correct for the server!!!

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		// place to initialise Magick?
		// beware loader issues!
		Magick::InitializeMagick( MagickDllPath );

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

