/* Implementation of the crop style classes */


//#include "stdafx.h"
#include "CpMagick.h"


// ABSTRACT classes first

CCropStyleAbstract::CCropStyleAbstract() {

}
HRESULT CCropStyleAbstract::GrabOriginal(void)
{
	// load the original image
	try {
		image.read(sPathToImage + sFileName);
		//image.read( orig );
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
}

std::string CCropStyleAbstract::getPathToImage(void)
{
	return sPathToImage;
};

void CCropStyleAbstract::setPathToImage(string argPath)
{
	sPathToImage = argPath;
}
std::string CCropStyleAbstract::getFileName(void)
{
	return sFileName;
}
void CCropStyleAbstract::setFileName(string argName)
{
	sFileName = argName;
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
			OutputDebugString(L"INFO: ++++ ++++ About to crop a panel...\n");
			imgPanel.crop(Geometry(iXpan, iYpan, iXext, iYext));
			OutputDebugString(L"INFO: ---- ---- Cropped a panel.\n");
			//TODO: mark new panel on layout map
			// now commit the new panel to disk
			OutputDebugString(L"INFO: ++++ About to write a panel to disk...\n");
			imgPanel.write( getPathToImage() + getFileName() + "_GRID_" + to_string(iSeq) + ".jpg");
			OutputDebugString(L"INFO: ---- Wrote panel to disk.\n");

			//advance extents by one panel width
			iXext += iXpan;
			//add kerf if appropriate
			if ((iXext + iKpels) <= iXtot)
				iXext += iKpels;
			// increment panel sequence number
			OutputDebugString(L"INFO: incrementing panel sequence number.\n");
			iSeq++;
		}
		// done with that row. Increment iYext
		iYext += iYpan;
		// add kerf if appropriate
		if ((iYext + iKpels) <= iYtot)
			iYext += iKpels;
		OutputDebugString(L"INFO: row completed.\n");
	}
	OutputDebugString(L"INFO: About to return from DoCrop()...\n");
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
			imgPanel.write(getPathToImage() + getFileName() + "_BRICK_" + to_string(iSeq) + ".jpg");

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

	int iXhigh = 0;	// horizontal high water mark
	int iYhigh = 0;	// vertical high water mark

	//int iXBfir = 0;	// X coord of Bottom left vertex of first panel
	//int iYBfir = 0;	// Y coord of Bottom left vertex of first panel
	//int iXTfir = 0;	// X coord of Top right vertex of first panel
	//int iYTfir = 0;	// Y coord of Top right vertex of first panel

	int iXfirst = 0;	// X coord of bottom left vertex of leftmost panel
	int iYfirst = 0;	// Y coord of bottom left vertex of leftmost panel
	int iXnext = 0;		// X coord of bottom left vertex of next horizontal panel
	int iYnext = 0;		// Y coord of bottom left vertex of next horizontal panel

	// start far to the left and down, to account for upward "drift" in A4 herringbone pattern
	// use the bigger of the two panel dimensions, in order to be conservative
	if (iXtot >= iYtot)
		iXhigh = iYhigh = iXfirst = iYfirst = iXnext = iYnext = -iXtot;
	else
		iXhigh = iYhigh = iXfirst = iYfirst = iXnext = iYnext = -iYtot;
	printf("Starting coords for the loops are: (%d, %d)\n", iXhigh, iYhigh);

	int iSeq = STARTING_SEQUENCE_NUMBER;
	Image imgPanel; // the small panel which is being cropped out of the original image
	bool bLeftmostHorizontal = true;	// the first panel can be horizontal, arbitrarily
	bool bCurrentHorizontal = bLeftmostHorizontal;		// current is leftmost, at the start


	while ( iYhigh < iYtot ) {						// outer loop
		printf("======= OUTER ==============\n\n");
		// reset leftmost type
		bCurrentHorizontal = bLeftmostHorizontal;
		printf("Leftmost panel is horizontal: %s\n", bLeftmostHorizontal ? "true" : "false");


		// rewind to left edge of row


		while (iXhigh < iXtot) {	// inner loop	
			printf("======= INNER ==============\n\n");
			printf("Bottom-left coords for the candidate outline are: (%d, %d)\n", iXnext, iYnext);
			// calculate top-right and inner vertices
			int iXBLinner, iYBLinner, iXTRinner, iYTRinner;
			if (bCurrentHorizontal) {	// panel type is horizontal
				iXhigh = iXnext + iHXout;
				iYhigh = iYnext + iHYout;
				iXBLinner = iXnext + iKTBpels;
				iYBLinner = iYnext + iKLRpels;
				iXTRinner = iXhigh - iKTBpels;
				iYTRinner = iYhigh - iKLRpels;
			}
			else {	// panel type is vertical
				iXhigh = iXnext + iVXout;
				iYhigh = iYnext + iVYout;
				iXBLinner = iXnext + iKLRpels;
				iYBLinner = iYnext + iKTBpels;
				iXTRinner = iXhigh - iKLRpels;
				iYTRinner = iYhigh - iKTBpels;
			}
			printf("Top right coords for the candidate outline are: (%d, %d)\n", iXhigh, iYhigh);
			printf("Bottom-left inner panel coords are: (%d, %d)\n", iXBLinner, iYBLinner);
			printf("Top-right inner panel coords are: (%d, %d)\n", iXTRinner, iYTRinner);

			// determine whether the bottom-left and top-right inner vertices
			// are within image boundaries, in which case - cut a panel!
			if ((iXBLinner >= 0) &&	(iYBLinner >= 0) && (iXTRinner <= iXtot) &&	(iYTRinner <= iYtot)) {
				printf("----- CUT ----- CUT ----- CUT -----\n");
				try {
					imgPanel = image;	// reset panel image to original image
					imgPanel.crop(Geometry((iXTRinner - iXBLinner), (iYTRinner - iYBLinner), iXBLinner, iYBLinner));
					//TODO: mark new panel on layout map
					// now commit the new panel to disk
					//imgPanel.write("lowrider_HERRINGBONE_" + to_string(iSeq) + ".jpg");
					imgPanel.write(getPathToImage() + getFileName() + "_HERRINGBONE_" + to_string(iSeq) + ".jpg");
				}
				catch (Exception &error_) {
					cout << "Caught exception during rotation: " << error_.what() << endl;
					//return E_FAIL;
				}
				// increment panel sequence number
				iSeq++;
			}
		
			// move BL vertex one panel to the right
			if (bCurrentHorizontal) {	// panel type is horizontal
				iXnext += iHXout;
				// iYnext does not change
			}
			else {	// panel type is vertical
				iXnext += iVXout;
				iYnext += (iVYout - iHYout);
			}
			// toggle panel orientation
			bCurrentHorizontal = !bCurrentHorizontal;
		}
		// done with that row. Advance one row higher
		if (bLeftmostHorizontal) {	// panel type is horizontal
			iXfirst = iXfirst + (iHXout - iVXout);
			iYfirst = iYfirst + iHYout;
		}
		else {	// panel type is vertical
				// iXBleft does not change
			iYfirst = iYfirst + iVYout;	// add a vertical outer height
		}
		printf("Leftmost bottom vertex for the row: (%d, %d)\n", iXfirst, iYfirst);
		// rewind next pointer
		iXnext = iXhigh = iXfirst;
		iYnext = iYfirst;

		bLeftmostHorizontal = !bLeftmostHorizontal;	// toggle leftmost type

	}

	return S_OK;
};