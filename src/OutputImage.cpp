
#include "OutputImage.h"
#include "InputImage.h"
#include <algorithm>
#include <iostream>

const int maxOutputWidth = 8192;
const int maxOutputHeight = 8192;


int RoundUpToPowerOf2( int num ) {
	// TODO: probably more efficient way of doing this
	for ( int test = 1; true; test *= 2 )
	{
		if ( num <= test )
			return test;
	}
}



OutputImage::OutputImage(const Options& options)
	: _options( options ), _actualWidth(0), _actualHeight(0), _virtualWidth(0), _virtualHeight(0) {
}

void OutputImage::_AddAvailableRect( const AtlasRect& rect ) {
	if ( rect.IsValid() ) {
		_availableRects.push_back( rect );
	}
}

void OutputImage::InitVirtualSize( int width, int height ) {
	_actualWidth = width;
	_actualHeight = height;
	_virtualWidth = RoundUpToPowerOf2( width );
	_virtualHeight = RoundUpToPowerOf2( height );
	_availableRects.push_back( AtlasRect( 0, 0, _virtualWidth, _virtualHeight ) );
}

bool OutputImage::IncreaseVirtualSize( const Options& options ) {
	if ( _virtualWidth >= options.maxOutputWidth && _virtualHeight >= options.maxOutputHeight ) {
		return false;
	}

	const int spareWidth = _virtualWidth - _actualWidth;
	const int spareHeight = _virtualHeight - _actualHeight;
	// determine if it will be more efficient to increase on right or bottom side
	bool increasingWidth = true;
	if ( _virtualWidth >= options.maxOutputWidth )
		increasingWidth = false; // always expand height if width is maximum
	else if ( _virtualHeight >= options.maxOutputHeight )
		increasingWidth = true; // always expand width if height is maximum
	else
	{
		// calculate area size is we expand both via the width and height, and expand in the direction which will result in less space
		const int expandedSpareWidth = _virtualWidth * 2 + spareWidth;
		const int expandedSpareHeight = _virtualHeight * 2 + spareHeight;
		const int expandedSpareWidthVolume = expandedSpareWidth * _virtualHeight;
		const int expandedSpareHeightVolume = expandedSpareHeight * _virtualWidth;

		increasingWidth = expandedSpareHeightVolume > expandedSpareWidthVolume;
	}
	// then create new available rect which covers new virtual size, and merge it with any available space on that side
	AtlasRect newRect;
	if ( increasingWidth )
		newRect = AtlasRect( _actualWidth, 0, _virtualWidth + spareWidth, _virtualHeight );
	else
		newRect = AtlasRect( 0, _actualHeight, _virtualWidth, _virtualHeight + spareHeight );

	const int newWidth = _virtualWidth * ( increasingWidth ? 2 : 1 );
	const int newHeight = _virtualHeight * ( increasingWidth ? 1 : 2 );

	// and reduce the size of any rectangles which occupied that merged space
	std::deque<AtlasRect> newRects;
	auto eraseIt = std::remove_if( _availableRects.begin(), _availableRects.end(), [&] ( const AtlasRect& rect ) {
		if ( newRect.Contains( rect ) ) {
			// remove this rect - is completely contained by new rect
			return true;
		} else if ( newRect.IntersectsWith( rect ) ) {
			// work out size of new rect
			int diffX = rect.x + rect.w - newRect.x;
			if ( diffX < 0 )
				diffX = 0;
			int diffY = rect.y + rect.h - newRect.y;
			if ( diffY < 0 )
				diffY = 0;
			AtlasRect newSubRect( rect.x, rect.y, rect.w - diffX, rect.h - diffY );
			// remove old rect and put this new rect in it's place
			newRects.push_back( newSubRect );
			return true;
		}
		return false;
	} );
	_availableRects.erase( eraseIt, _availableRects.end() );

	_availableRects.push_back( newRect );
	std::for_each( newRects.begin(), newRects.end(), [&] ( const AtlasRect& rect ) {
		_availableRects.push_back( rect );
	} );
	// sort list
	_SortRects();
	_virtualWidth = newWidth;
	_virtualHeight = newHeight;
	return true;
}


void OutputImage::SplitRect( std::size_t rectIndex, int width, int height ) {
	// remove old rect, add 2 new ones (if we can)
	// Split area into 2 rects, calculate which will generate the largest possible child.
	const AtlasRect target = _availableRects[ rectIndex ];
	_availableRects.erase( _availableRects.begin() + rectIndex );
		
	const AtlasRect rightRectLarge(		target.x + width,	target.y,			target.w - width,				target.h );
	const AtlasRect bottomRectSmall(	target.x,			target.y + height,	target.w - rightRectLarge.w,	target.h - height );
	const AtlasRect bottomRectLarge(	target.x,			target.y + height,	target.w,						target.h - height );
	const AtlasRect rightRectSmall(		target.x + width,	target.y,			target.w - width,				target.h - bottomRectLarge.h );

	const int rightRectLargeArea = rightRectLarge.Area();
	const int bottomRectSmallArea = bottomRectSmall.Area();
	const int rightRectSmallArea = rightRectSmall.Area();
	const int bottomRectLargeArea = bottomRectLarge.Area();

	if ( rightRectLargeArea > bottomRectLargeArea || bottomRectSmallArea > rightRectSmallArea
		|| rightRectLargeArea > rightRectSmallArea || bottomRectSmallArea > bottomRectLargeArea )
	{
		_AddAvailableRect( rightRectLarge );
		_AddAvailableRect( bottomRectSmall );
	}
	else
	{
		_AddAvailableRect( bottomRectLarge );
		_AddAvailableRect( rightRectSmall );
	}
	// sort rect list
	_SortRects();
}


void OutputImage::_SortRects() {
	std::sort( _availableRects.begin(), _availableRects.end(), [] ( const AtlasRect& lhs, const AtlasRect& rhs ) {
		const int val = lhs.Area() - rhs.Area();
		return val > 0;
	} );
}


void OutputImage::AddSubImage( const InputImage* input, bool isRotated, int x, int y ) {
	// copy image data into atlas canvas, and add sub image to list
	AtlasRect rect( x, y, input->Data()->Width(), input->Data()->Height() );
	_subImages.push_back( SubImage( input, isRotated, rect ) );
	// update actual width/height
	if ( !isRotated ) {
		if ( _actualWidth < rect.x + rect.w ) {
			_actualWidth = rect.x + rect.w;
		}
		if ( _actualHeight < rect.y + rect.h ) {
			_actualHeight = rect.y + rect.h;
		}
	} else {
		if ( _actualWidth < rect.x + rect.h ) {
			_actualWidth = rect.x + rect.h;
		}
		if ( _actualHeight < rect.y + rect.w ) {
			_actualHeight = rect.y + rect.w;
		}
	}	
}

void OutputImage::Finalise( const std::string& filename ) {
	int width, height;
	if (_options.finalImageIsPow2) {
		width = _virtualWidth;
		height = _virtualHeight;
	} else {
		width = _actualWidth;
		height = _actualHeight;
	}
	ImageData* data = ImageData::createBlank(width, height);
	std::for_each(_subImages.begin(), _subImages.end(), [&](const SubImage& subImage) {
		std::cout << "Inserting sub image " << subImage.input->Name() << " at " << subImage.rect.x << "x" << subImage.rect.y << " [" << subImage.rect.w << "x" << subImage.rect.h << "]" << std::endl;
		data->InsertSubImage(subImage.input->Data(), subImage.rect, subImage.rotated);
	});
	data->Save(filename);
	delete data;
}
