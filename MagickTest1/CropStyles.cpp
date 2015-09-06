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

	// now the horizontal panel dimension, in pixels
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


// ------------------------------------------------------------------------------------------
// ----------------------- Herringbone ------------------------------------------------------
// ------------------------------------------------------------------------------------------

CCropStyleHerringbone::CCropStyleHerringbone(int rows, int kerf, int rot) {
	iRows = rows;
	// iCols = cols; // no need for col count
	iKerf = kerf;
	iRotation = rot;
};

HRESULT CCropStyleHerringbone::DoCrop(int rows, int kerf) {
	// produce herringbone pattern

	// get image stats
	const int iXtot = image.size().width();
	printf("Image width is: %d\n", iXtot);
	const int iYtot = image.size().height();
	printf("Image height is: %d\n", iYtot);



	// key herringbone equations
	//	iYavg = iYtot / ((1 + ASPECT_RATIO) /2)
	//  iXtot = (iXpan * cols) + ((cols-1) * iKpels)

	// calculate appropriate panel dimensions, in pixels

	// first the horizontal panel dimensions
	// horizontal panel outer
	const int iHYout = (iYtot / rows) / ((float)(1 + ASPECT_RATIO) / 2);
	printf("Horizontal panel outer Y is: %d\n", iHYout);
	const int iHXout = iHYout * ASPECT_RATIO;
	printf("Horizontal panel outer X is: %d\n", iHXout);

	// vertical panel outer just swaps the X and Y dimensions
	const int iVYout = iHXout;
	printf("Vertical panel outer Y is: %d\n", iVYout);
	const int iVXout = iHYout;
	printf("Vertical panel outer X is: %d\n", iVXout);

	// panel (inner) dimensions
	const int iVYpan = iVYout - (iVYout * ((float)kerf / 100));
	printf("Vertical panel inner Y is: %d\n", iVYpan);
	const int iVXpan = iVXout - (iVXout * ((float)kerf / 100));
	printf("Vertical panel inner X is: %d\n", iVXpan);

	const int iHYpan = iVXpan;
	printf("Horizontal panel inner Y is: %d\n", iHYpan);
	const int iHXpan = iVYpan;
	printf("Horizontal panel inner X is: %d\n", iHXpan);
	

	// the kerf in pixels, as opposed to a percentage.
	// calculated as half of the difference between inner and outer
	const int iKTBpels = (iVYout - iVYpan) / 2; // top and bottom kerf
	printf("Top and bottom kerf is: %d\n", iKTBpels);
	const int iKLRpels = (iVXout - iVXpan) / 2;	// left and right kerf
	printf("Left and right kerf is: %d\n", iKLRpels);


	// cut up individual panels until the original image space is exhausted

	int iXext = 0;	// horizontal high water mark
	int iYext = 0;	// vertical high water mark

	//int iXBfir = 0;	// X coord of Bottom left vertex of first panel
	//int iYBfir = 0;	// Y coort of Bottom left vertex of first panel
	//int iXTfir = 0;	// X coord of Top right vertex of first panel
	//int iYTfir = 0;	// Y coort of Top right vertex of first panel

	int iXBleft = 0;	// X coord of Bottom left vertex of leftmost panel
	int iYBleft = 0;	// Y coort of Bottom left vertex of leftmost panel
	//int iXTleft = 0;	// X coord of Top right vertex of leftmost panel
	//int iYTleft = 0;	// Y coort of Top right vertex of leftmost panel

	// start far to the left and down, to account for upward "drift" in A4 herringbone pattern
	// use the bigger of the two panel dimensions, in order to be conservative
	if (iXtot >= iYtot)
		iXext = iYext = iXBleft = iYBleft = -iXtot;
	else
		iXext = iYext = iXBleft = iYBleft = -iYtot;
	printf("Starting coords for the loops are: (%d, %d)\n", iXext, iYext);

	int iSeq = STARTING_SEQUENCE_NUMBER;
	Image imgPanel; // the small panel which is being cropped out of the original image
	bool bLeftmostHorizontal = true;	// the first panel can be horizontal, arbitrarily
	bool bCurrentHorizontal = bLeftmostHorizontal;		// current is leftmost, at the start


	while ( iYext < iYtot ) {						// outer loop
		// reset leftmost type
		bCurrentHorizontal = bLeftmostHorizontal;
		printf("Leftmost panel is horizontal: %s\n", bLeftmostHorizontal ? "true" : "false");
		// move leftmost vertex up a level
		if (bLeftmostHorizontal) {	// panel type is horizontal
			iXBleft = iXBleft + (iHXout - iVXout);
			iYBleft = iYBleft + iHYout;
		}
		else {	// panel type is vertical
			// iXBleft does not change
			iYBleft = iYBleft + iVYout;	// add a vertical outer height
		}
		printf("Leftmost bottom vertex for the row: (%d, %d)\n", iXBleft, iYBleft);
		// rewind to left edge of row

		while (iXext < iXtot) {	// inner loop	
			// calculate outer vertices
			if (bCurrentHorizontal) {	// panel type is horizontal
				

			}
			else {	// panel type is vertical

			}

			iXext++; //TODO: remove

			//imgPanel = image;	// reset panel image to original image
			//printf("Cutting a panel at: (%d, %d)\n", iXext, iYext);
			//imgPanel.crop(Geometry(iXpan, iYpan, iXext, iYext));
			//TODO: mark new panel on layout map
			// now commit the new panel to disk
			//imgPanel.write("lowrider_BRICK_" + to_string(iSeq) + ".jpg");

			//advance extents by one panel forward

			

			// increment panel sequence number
			iSeq++;
			// toggle panel orientation
			bCurrentHorizontal = !bCurrentHorizontal;
		}
		// done with that row. Advance one row higher
		bLeftmostHorizontal = !bLeftmostHorizontal;	// toggle leftmost type
		iYext++; //TODO: remove
	}

	return S_OK;
};