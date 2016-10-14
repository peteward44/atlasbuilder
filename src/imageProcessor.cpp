
#include "main.h"
#include "imageProcessor.h"
#include <string>
#include <deque>
#include <algorithm>
#include <iostream>


int GetPadding( const Options& options, int coord ) {
	return coord == 0 ? 0 : options.padding;
}

bool ImageFitsInRect( const Options& options, const AtlasRect& rect, const InputImage* input, bool& isRotated ) {
	if ( rect.w >= input->Data()->Width() + GetPadding( options, rect.x ) && rect.h >= input->Data()->Height() + GetPadding( options, rect.y ) ) {
		isRotated = false;
		return true;
	} else if ( options.rotationEnabled && rect.w >= input->Data()->Height() + GetPadding( options, rect.x ) && rect.h >= input->Data()->Width() + GetPadding( options, rect.y ) ) {
		isRotated = true;
		return true;
	} else {
		isRotated = false;
		return false;
	}
}


bool InsertImage( const Options& options, const InputImage* input, OutputImage* output ) {
	const auto& availableRects = output->AvailableRects();

	for( std::size_t index=0; index<availableRects.size(); ++index ) {
		const AtlasRect& rect = availableRects[index];
		bool isRotated = false;
		if ( ImageFitsInRect( options, rect, input, isRotated ) ) {
			const int padx = GetPadding( options, rect.x );
			const int pady = GetPadding( options, rect.y );
			const int imgx = rect.x + padx;
			const int imgy = rect.y + pady;
			
			// remove old rect, add 2 new ones
			// when storing a square inside a larger square, will always leave 2 other rectangles, one on the bottom
			// and one on the right. These rects are added to a list and then used to see if any future input images can fit
			const int width = input->Data()->Width() + padx;
			const int height = input->Data()->Height() + pady;
			output->SplitRect( index, isRotated ? height : width, isRotated ? width : height );
			output->AddSubImage( input, isRotated, imgx, imgy );
			return true;
		}
	}

	return false;
}


bool AddSubImage( const Options& options, const InputImage* input, OutputImage* output, bool allowResize ) {
	// attempt to fill a small power of 2 texture first, then expand it gradually up to the maximum size
	// if the sub images don't fit. mVirtualSize is the size of the current rect we are trying to fill
	if ( output->AvailableRects().size() == 0 ) {
		// first time this output bitmap has been used, try using the smallest possible virtual texture size
		output->InitVirtualSize( input->Data()->Width(), input->Data()->Height() );
	}
	if ( allowResize ) {
		while ( !InsertImage( options, input, output ) )
		{
			// no images could be inserted in current image using current virtual size.
			// increase virtual size and try again
			if ( !output->IncreaseVirtualSize( options ) )
				return false; // cannot fit anymore images into this output image
		}
		return true;
	}
	else
		return InsertImage( options, input, output );
}


bool AddSubImageIfPossible( const Options& options, const InputImage* input, const std::deque<OutputImage*>& outputImageList ) {
	// try adding to existing output bitmaps without resizing them
	for ( auto it = outputImageList.begin(), itend = outputImageList.end(); it != itend; ++it ) {
		if ( AddSubImage( options, input, *it, false ) ) {
			return true;
		}
	}
	// if not successful then try adding by resizing
	for ( auto it = outputImageList.begin(), itend = outputImageList.end(); it != itend; ++it ) {
		if ( AddSubImage( options, input, *it, true ) ) {
			return true;
		}
	}
	return false;
}


std::deque<OutputImage*> process( std::deque<InputImage*>& inputImageList, const Options& options ) {
	// sort by given area (width * height)
	std::sort( inputImageList.begin(), inputImageList.end(), [] ( const InputImage* lhs, const InputImage* rhs ) {
//		std::cout << "Processing " << lhs->Name() << " and " << rhs->Name() << std::endl;
		return lhs->Data()->Area() > rhs->Data()->Area();
	} );

	std::deque<OutputImage*> outputImageList;
	std::size_t inputIndex = 0;
	while ( inputIndex < inputImageList.size() ) {
		const InputImage* input = inputImageList[inputIndex];
		const bool added = AddSubImageIfPossible( options, input, outputImageList );
		if ( !added ) {
			// add a new output bitmap if an existing space can't be found
			auto newOutput = new OutputImage( options );
			outputImageList.push_back( newOutput );
		} else {
			// successfully added - move on
			inputIndex++;
		}
	}
	return outputImageList;
}

