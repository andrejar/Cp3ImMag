#pragma once

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

#define WIN32_LEAN_AND_MEAN   

//#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <Magick++.h>	// ImageMagick include file
#include <iostream> 

using namespace std;
using namespace Magick;

#pragma comment( lib, "CORE_RL_Magick++_" )		// ImageMagick library

// Constants ----------------------------------------------------------------

#define ASPECT_RATIO 1.4142 // Standard ISO 216 relationship between width & height
#define STARTING_SEQUENCE_NUMBER 1; // start counting panels at this number


// Structs and enums --------------------------------------------------------

enum CropStyle {
	CROP_STYLE_GRID = 1, // for human counting
	CROP_STYLE_BRICK,
	CROP_STYLE_HERRINGBONE
};

// Variables ----------------------------------------------------------------

extern CpMagick_API int nCpMagick;


// Functions ----------------------------------------------------------------


extern "C" CpMagick_API HRESULT fnCpMagickEntry(int, int, int, int, int);

CpMagick_API int fnCpMagick(void);


// Classes ------------------------------------------------------------------

class CCropStyleAbstract {
	// ancestor of all crop styles

	std::string strDisplayName;
	bool bRequiresHTML5;
	bool bIsRectilinear;
	int iSparsenessFactor; //TODO: review whether this is needed

protected:
	Image image;

public:
	CCropStyleAbstract(void);
	HRESULT GrabOriginal(string);
	HRESULT RotateOriginal(int);
	HRESULT WriteOriginal(string filename);
};


class CCropStyleRectilinearAbstract : public CCropStyleAbstract {
	// ancestor of all rectilinear crop styles

public:
	int iRows;
	int iCols;
	int iKerf;
	int iRotation;

	CCropStyleRectilinearAbstract(void);
};


class CCropStyleGrid : public CCropStyleRectilinearAbstract {

public:
	CCropStyleGrid(int rows, int cols, int kerf, int rot);
	HRESULT DoCrop(int rows, int cols, int kerf);
};

class CCropStyleBrick : public CCropStyleRectilinearAbstract {

public:
	CCropStyleBrick(int rows, int cols, int kerf, int rot);
	HRESULT DoCrop(int rows, int cols, int kerf);
};

