
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
	// trim all input images if enabled
	if ( _options.trimEnabled ) {
		_Trim();
	}
	_AlignBoundary();
	if ( _options.resolution != 1.0f ) {
		_imageData = ImageData::createNewResolution(_imageData, _options.resolution);
		_trimmedRect.Scale( _options.resolution );
		_originalWidth = floorf( _originalWidth * _options.resolution );
		_originalHeight = floorf( _originalHeight * _options.resolution );
	}
}


void InputImage::_Trim() {
	_isTrimmed = true;
	_trimmedRect = _imageData->Trim();
}


void InputImage::_AlignBoundary() {
	int left = 0, right = 0, top = 0, bottom = 0;
	
	// pad out to width / height of multiple of _options.boundaryAlignment
	if (_options.boundaryAlignment > 0) {
		// number of pixels we are over the boundary alignment value
		const auto widthCorrection = CalculateBoundaryAlignment(_imageData->Width(), _options.boundaryAlignment);
		left = widthCorrection.first;
		right = widthCorrection.second;
		const auto heightCorrection = CalculateBoundaryAlignment(_imageData->Height(), _options.boundaryAlignment);
		top = heightCorrection.first;
		bottom = heightCorrection.second;
	}
	if ( left > 0 || right > 0 || top > 0 || bottom > 0 ) {
		_imageData->AddPadding( left, right, top, bottom );
	}
}
