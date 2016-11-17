
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
	// 
	_AlignBoundary();
	// }
}


void InputImage::_Trim() {
	_isTrimmed = true;
	// find extremes of image - i.e. how many transparent pixels are to the top, bottom, left and right of image
	_imageData->Trim();
	// then crop to that rect
}


void InputImage::_AlignBoundary() {
	// int left = 0, right = 0, top = 0, bottom = 0;
	
	// // pad out to width / height of multiple of _options.boundaryAlignment
	// if (_options.boundaryAlignment > 0) {
		// // number of pixels we are over the boundary alignment value
		// const auto widthCorrection = CalculateBoundaryAlignment(original->Width(), _options.boundaryAlignment);
		// left = widthCorrection.first;
		// right = widthCorrection.second;
		// const auto heightCorrection = CalculateBoundaryAlignment(original->Height(), _options.boundaryAlignment);
		// top = heightCorrection.first;
		// second = heightCorrection.second;
	// } else if ( _options.padding > 0 ) {
		// top = _options.padding;
		// left = _options.padding;
	// }

	// if ( left > 0 || right > 0 || top > 0 || bottom > 0 ) {
		// original->AddPadding( left, right, top, bottom );
	// }
}
