/* Implementation of the crop style classes */


//#include "stdafx.h"
#include "CpMagick.h"


// ABSTRACT classes first

CCropStyleAbstract::CCropStyleAbstract() {

}
int CCropStyleAbstract::GrabOriginal(void)
{
	// load the original image
	try {
		image.read( (char*)"lowrider.jpg" );

		//have fun!!!
		image.rotate(30);
		image.write( (char*)"lowrider30.jpg" );
	}
	catch (Exception &error_) {
		//TODO: ETW logging here!
		cout << "Caught exception: " << error_.what() << endl;
		return 1;
	}
	return 0;
};

CCropStyleRectilinearAbstract::CCropStyleRectilinearAbstract() {

};

// Implemented crop styles

CCropStyleGrid::CCropStyleGrid(int rows, int cols, int kerf, int rot) {
	iRows = rows;
	iCols = cols;
	iKerf = kerf;
	iRotation = rot;
};


