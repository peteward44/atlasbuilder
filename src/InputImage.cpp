
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
	_resolutionData.insert(std::make_pair(_resolution, _imageData));
	_originalWidth = _imageData->Width();
	_originalHeight = _imageData->Height();
	_trimmedRect = AtlasRect( 0, 0, _originalWidth, _originalHeight );
}


void InputImage::Trim() {
	_isTrimmed = true;
	// TODO: find extremes of image - i.e. how many transparent pixels are to the top, bottom, left and right of image
	// then crop to that rect
	throw new std::runtime_error("Trim operation unsupported at this time");
}


void InputImage::AlignBoundary() {
	auto it = _resolutionData.find(1.0f);
	ImageData* original = it->second;

	// pad out to width / height of multiple of _options.boundaryAlignment

	// number of pixels we are over the boundary alignment value
	auto widthCorrection = CalculateBoundaryAlignment(original->Width(), _options.boundaryAlignment);
	auto heightCorrection = CalculateBoundaryAlignment(original->Height(), _options.boundaryAlignment);

	if ( widthCorrection.first > 0 || widthCorrection.second > 0 || heightCorrection.first > 0 || heightCorrection.second > 0 ) {
		original->AddPadding(widthCorrection.first, widthCorrection.second, heightCorrection.first, heightCorrection.second);
	}
}


void InputImage::SelectResolution( float res ) {
	_resolution = res;
	// create new image data if not already exist
	auto it = _resolutionData.find(res);
	if (it != _resolutionData.end()) {
		_imageData = it->second;
	} else {
		_imageData = ImageData::createNewResolution(_resolutionData[1.0f], res);
		_resolutionData.insert(std::make_pair(res, _imageData));
	}
}


ImageData* InputImage::Data(float res) const {
	if (res == 0.0f) {
		res = _resolution;
	}
	return _resolutionData.find( res )->second;
}
