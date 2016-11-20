
#include "main.h"
#include "InputImage.h"
#include <string>
#include <list>
#include <cmath>

int CalculateBoundaryAlignment( int width, int alignment ) {
	// number of pixels we are over the boundary alignment value
	const int overrun = width % alignment;
	if ( overrun > 0 ) {
		return alignment - overrun;
	}
	return 0;
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
	if ( !_options.scaleManifestValues && _options.trimEnabled ) {
		_Trim();
	}
	if ( _options.resolution != 1.0f ) {
		_imageData = ImageData::createNewResolution(_imageData, _options.resolution);
		if ( _options.scaleManifestValues ) {
			_originalWidth = floorf( _originalWidth * _options.resolution );
			_originalHeight = floorf( _originalHeight * _options.resolution );
		}
	}
	if ( _options.scaleManifestValues && _options.trimEnabled ) {
		_Trim();
	}
}


std::pair<int, int> InputImage::CalculatePadding() const {
	int w = 0, h = 0;
	if ( _options.padding > 0 ) {
		w = _options.padding;
		h = _options.padding;
	}
	// pad out to width / height of multiple of _options.boundaryAlignment
	if ( _options.boundaryAlignment > 0 ) {
		const int extraWidth = CalculateBoundaryAlignment(_imageData->Width(), _options.boundaryAlignment);
		const int extraHeight = CalculateBoundaryAlignment(_imageData->Height(), _options.boundaryAlignment);
		if ( extraWidth > w ) {
			w = extraWidth;
		}
		if ( extraHeight > h ) {
			h = extraHeight;
		}
	}
	return std::make_pair( w, h );
}


int InputImage::Area( bool includePadding ) const {
	return Width( includePadding ) * Height( includePadding );
}


int InputImage::Width( bool includePadding ) const {
	const auto padding = CalculatePadding();
	return _imageData->Width() + ( includePadding ? padding.first : 0 );
}


int InputImage::Height( bool includePadding ) const {
	const auto padding = CalculatePadding();
	return _imageData->Height() + ( includePadding ? padding.second : 0 );
}


void InputImage::_Trim() {
	_isTrimmed = true;
	_trimmedRect = _imageData->Trim();
}
