
#include "main.h"
#include "InputImage.h"
#include <string>
#include <list>
#include <cmath>

std::pair< int, int > CalculateBoundaryAlignment( int width, int alignment ) {
	// number of pixels we are over the boundary alignment value
	const int overrun = width % alignment;
	if (overrun > 0) {
		// number of pixels we need to add to both left / right to make it right
		const int additional = alignment - overrun;
		const int left = (int)floorf((float)additional / 2.0f);
		const int right = additional - left;
		return std::make_pair(left, right);
	}
	return std::make_pair(0, 0);
}


InputImage::InputImage( const Options& options, const std::string& filename )
	: _options( options )
	, _originalWidth(0)
	, _originalHeight(0)
	, _name( filename )
	, _isTrimmed( false )
	, _resolution( 1.0f )
	, _imageData( NULL )
{
	_imageData = ImageData::createFromFile(filename);
	_originalWidth = _imageData->Width();
	_originalHeight = _imageData->Height();
	_trimmedRect = AtlasRect( 0, 0, _originalWidth, _originalHeight );
}


void InputImage::Prep() {
	if ( _options.resolution != 1.0f ) {
		_imageData = ImageData::createNewResolution(_imageData, _options.resolution);
	}
	// trim all input images if enabled
	if ( _options.trimEnabled ) {
		_Trim();
	}
	// if (options.boundaryAlignment > 0) {
		// input->_AlignBoundary();
	// }
}


void InputImage::_Trim() {
	_isTrimmed = true;
	// find extremes of image - i.e. how many transparent pixels are to the top, bottom, left and right of image
	_imageData->Trim();
	// then crop to that rect
}


void InputImage::_AlignBoundary() {
	// auto it = _resolutionData.find(1.0f);
	// ImageData* original = it->second;

	// // pad out to width / height of multiple of _options.boundaryAlignment

	// // number of pixels we are over the boundary alignment value
	// auto widthCorrection = CalculateBoundaryAlignment(original->Width(), _options.boundaryAlignment);
	// auto heightCorrection = CalculateBoundaryAlignment(original->Height(), _options.boundaryAlignment);

	// if ( widthCorrection.first > 0 || widthCorrection.second > 0 || heightCorrection.first > 0 || heightCorrection.second > 0 ) {
		// original->AddPadding(widthCorrection.first, widthCorrection.second, heightCorrection.first, heightCorrection.second);
	// }
}
