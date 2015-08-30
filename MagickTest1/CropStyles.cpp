/* Implementation of the crop style classes */


//#include "stdafx.h"
#include "CpMagick.h"


// ABSTRACT classes first

CCropStyleAbstract::CCropStyleAbstract() {

}
HRESULT CCropStyleAbstract::GrabOriginal(string orig)
{
	// load the original image
	try {
		image.read( orig );
		//TODO: check original image minimum requirements!!!
	}
	catch (Exception &error_) {
		//TODO: ETW logging here!
		cout << "Caught exception: " << error_.what() << endl;
		return E_FAIL;
	}
	return S_OK;
}
HRESULT CCropStyleAbstract::RotateOriginal(int rot)
{	
	try {
		image.rotate(rot);
	}
	catch (Exception &error_) {
		cout << "Caught exception during rotation: " << error_.what() << endl;
		return E_FAIL;
	}
	return S_OK;
};

HRESULT CCropStyleAbstract::WriteOriginal(string filename) {
// TODO: just a test function for now
	try {
		image.write( filename );
	}
	catch (Exception &error_) {
		cout << "Caught exception during write: " << error_.what() << endl;
		return E_FAIL;
	}
	return S_OK;
};


CCropStyleRectilinearAbstract::CCropStyleRectilinearAbstract() {

};

// Implemented crop styles

// ------------------------------------------------------------------------------------------
// ----------------------- Grid -------------------------------------------------------------
// ------------------------------------------------------------------------------------------

CCropStyleGrid::CCropStyleGrid(int rows, int cols, int kerf, int rot) {
	iRows = rows;
	iCols = cols;
	iKerf = kerf;
	iRotation = rot;
};

HRESULT CCropStyleGrid::DoCrop(int rows, int cols, int kerf) {
	// Most basic crop style - produce a regular grid of panels

	// get image stats
	const int iXtot = image.size().width();
	printf("Image width is: %d\n", iXtot);
	const int iYtot = image.size().height();
	printf("Image height is: %d\n", iYtot);

	// key grid equations
	//	iYtot = (iYpan * rows) + ((rows-1) * iYpan * (kerf/100))
	//  iXtot = (iXpan * cols) + ((cols-1) * iKpels)

	// calculate appropriate panel dimensions, in pixels
	
	// first the vertical panel dimension
	const int iYpan = iYtot / ( rows + ((rows - 1) * (float)kerf / 100) ); 
	printf("Panel height is: %d\n", iYpan);

	// the kerf in pixels, as opposed to a percentage. Based on panel height.
	const int iKpels = iYpan * ((float)kerf / 100); 
	printf("Kerf in pixels is: %d\n", iKpels);
	
	// now the horiznotal panel dimension, in pixels
	// this should in fact be an ASPECT_RATIO multiple
	//const int iXpan = (iXtot - (cols - 1)*iKpels) / cols; // in case a flexible aspect is required
	const int iXpan = iYpan * ASPECT_RATIO;
	printf("Panel width is: %d\n", iXpan);

	// cut up individual panels until the original image space is exhausted

	int iXext = 0;	// horizontal high water mark
	int iYext = 0;	// vertical high water mark
	int iSeq = STARTING_SEQUENCE_NUMBER;

	Image imgPanel;

	for ( int iYloop = 0; iYloop < rows; iYloop++ ) {	// outer loop
		// reset the horizontal high water mark
		iXext = 0;
		while ((iXext + iXpan) <= iXtot) {	// inner loop	
			imgPanel = image;	// reset panel image to original image
			printf("Cutting a panel at: (%d, %d)\n", iXext, iYext);
			imgPanel.crop(Geometry(iXpan, iYpan, iXext, iYext));
			//TODO: mark new panel on layout map
			// now commit the new panel to disk
			imgPanel.write("lowrider_GRID_" + to_string(iSeq) + ".jpg");

			//advance extents by one panel width
			iXext += iXpan;
			//add kerf if appropriate
			if ((iXext + iKpels) <= iXtot)
				iXext += iKpels;
			// increment panel sequence number
			iSeq++;
		}
		// done with that row. Increment iYext
		iYext += iYpan;
		// add kerf if appropriate
		if ((iYext + iKpels) <= iYtot)
			iYext += iKpels;
	}

	return S_OK;
};

// ------------------------------------------------------------------------------------------
// ----------------------- Brick ------------------------------------------------------------
// ------------------------------------------------------------------------------------------

CCropStyleBrick::CCropStyleBrick(int rows, int cols, int kerf, int rot) {
	iRows = rows;
	iCols = cols;
	iKerf = kerf;
	iRotation = rot;
};

HRESULT CCropStyleBrick::DoCrop(int rows, int cols, int kerf) {
	// produce brickwork

	// get image stats
	const int iXtot = image.size().width();
	printf("Image width is: %d\n", iXtot);
	const int iYtot = image.size().height();
	printf("Image height is: %d\n", iYtot);

	// key grid equations
	//	iYtot = (iYpan * rows) + ((rows-1) * iYpan * (kerf/100))
	//  iXtot = (iXpan * cols) + ((cols-1) * iKpels)

	// calculate appropriate panel dimensions, in pixels

	// first the vertical panel dimension
	const int iYpan = iYtot / (rows + ((rows - 1) * (float)kerf / 100));
	printf("Panel height is: %d\n", iYpan);

	// the kerf in pixels, as opposed to a percentage. Based on panel height.
	const int iKpels = iYpan * ((float)kerf / 100);
	printf("Kerf in pixels is: %d\n", iKpels);

	// now the horiznotal panel dimension, in pixels
	// this should in fact be an ASPECT_RATIO multiple
	//const int iXpan = (iXtot - (cols - 1)*iKpels) / cols; // in case a flexible aspect is required
	const int iXpan = iYpan * ASPECT_RATIO;
	printf("Panel width is: %d\n", iXpan);

	// cut up individual panels until the original image space is exhausted

	int iXext = 0;	// horizontal high water mark
	int iYext = 0;	// vertical high water mark
	int iSeq = STARTING_SEQUENCE_NUMBER;

	Image imgPanel;

	for (int iYloop = 0; iYloop < rows; iYloop++) {	// outer loop
		// reset the horizontal high water mark,
		// and indent every other row by half a brick
		if (0 == (iYloop % 2) ) { iXext = 0; }
		else { iXext = (iXpan + iKpels) / 2; }

		while ((iXext + iXpan) <= iXtot) {	// inner loop	
			imgPanel = image;	// reset panel image to original image
			printf("Cutting a panel at: (%d, %d)\n", iXext, iYext);
			imgPanel.crop(Geometry(iXpan, iYpan, iXext, iYext));
			//TODO: mark new panel on layout map
			// now commit the new panel to disk
			imgPanel.write("lowrider_BRICK_" + to_string(iSeq) + ".jpg");

			//advance extents by one panel width
			iXext += iXpan;
			//add kerf if appropriate
			if ((iXext + iKpels) <= iXtot)
				iXext += iKpels;
			// increment panel sequence number
			iSeq++;
		}
		// done with that row. Increment iYext
		iYext += iYpan;
		// add kerf if appropriate
		if ((iYext + iKpels) <= iYtot)
			iYext += iKpels;
	}

	return S_OK;
};
