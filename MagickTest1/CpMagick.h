// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CpMagick_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CpMagick_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CpMagick_EXPORTS
#define CpMagick_API __declspec(dllexport)
#else
#define CpMagick_API __declspec(dllimport)
#endif

// This class is exported from the CpMagick.dll
class CpMagick_API CCpMagick {
public:
	CCpMagick(void);
	// TODO: add your methods here.
};

extern CpMagick_API int nCpMagick;

CpMagick_API int fnCpMagick(void);
 