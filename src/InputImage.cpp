
#include "main.h"
#include "InputImage.h"
#include <string>
#include <list>
#include <cmath>


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
	// bool gotRect = false;
	// if ( _options.trimEnabled && !_options.scaleManifestValues ) {
		// _isTrimmed = true;
		// _trimmedRect = _imageData->Trim( false, true );
		// gotRect = true;
	// }
	if ( _options.trimEnabled && !_options.scaleManifestValues ) {
		_isTrimmed = true;
		_trimmedRect = _imageData->Trim( true, true );
	}
	if ( _options.resolution != 1.0f ) {
		_imageData = ImageData::createNewResolution(_imageData, _options.resolution);
		if ( _options.scaleManifestValues ) {
			_originalWidth = (int)floorf( ((float)_originalWidth) * _options.resolution );
			_originalHeight = (int)floorf( ((float)_originalHeight) * _options.resolution );
		}
	}
	if ( _options.trimEnabled && _options.scaleManifestValues ) {
		_isTrimmed = true;
		_trimmedRect = _imageData->Trim( true, true );
	}
	// if ( gotRect ) {
		// _imageData->Trim( _trimmedRect );
	// }
}


std::pair<int, int> InputImage::CalculatePadding() const {
	int w = 0, h = 0;
	if ( _options.padding > 0 ) {
		w = _options.padding;
		h = _options.padding;
	}
	// pad out to width / height of multiple of _options.boundaryAlignment
	if ( _options.boundaryAlignment > 0 ) {
		w = CalculateBoundaryAlignment(_imageData->Width(), _options.boundaryAlignment, _options.padding);
		h = CalculateBoundaryAlignment(_imageData->Height(), _options.boundaryAlignment, _options.padding);
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
