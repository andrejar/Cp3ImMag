// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAGICKTEST1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAGICKTEST1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MAGICKTEST1_EXPORTS
#define MAGICKTEST1_API __declspec(dllexport)
#else
#define MAGICKTEST1_API __declspec(dllimport)
#endif

// This class is exported from the MagickTest1.dll
class MAGICKTEST1_API CMagickTest1 {
public:
	CMagickTest1(void);
	// TODO: add your methods here.
};

extern MAGICKTEST1_API int nMagickTest1;

MAGICKTEST1_API int fnMagickTest1(void);
 