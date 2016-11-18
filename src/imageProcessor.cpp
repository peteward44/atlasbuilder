
#include "main.h"
#include "imageProcessor.h"
#include "MaxRects.h"
#include <string>
#include <deque>
#include <algorithm>
#include <iostream>


int RoundUpToPowerOf2( int num ) {
	// TODO: probably more efficient way of doing this
	for ( int test = 1; true; test *= 2 ) {
		if ( num <= test )
			return test;
	}
}

bool ExpandSize( const Options& options, int& width, int& height ) {
	if ( options.maxOutputWidth > width && width < height ) {
		width *= 2;
		return true;
	} else if ( options.maxOutputHeight > height ) {
		height *= 2;
		return true;
	} else {
		if ( !options.failOnTooBig ) {
			// has exceeded max size, so expand regardless of limits
			if ( width < height ) {
				width *= 2;
			} else {
				height *= 2;
			}
			return true;
		} else {
			// has exceeded max size and has been set to fail
			return false;
		}
	}
}

// int GetPadding( const Options& options, int coord ) {
	// return coord == 0 ? 0 : options.padding;
// }

OutputImage* process( std::deque<InputImage*>& inputImageList, const Options& options ) {
	const int padding = options.padding;
	
	// sort by given area (width * height)
	std::sort( inputImageList.begin(), inputImageList.end(), [&] ( const InputImage* lhs, const InputImage* rhs ) {
		return lhs->Data()->Area( padding ) > rhs->Data()->Area( padding );
	} );
	
	// Estimate how big the new image should be, by calculating total area required and finding nearest power of 2
	int biggestWidth = 0;
	int biggestHeight = 0;
	int totalArea = 0;
	for ( const InputImage* input : inputImageList ) {
		totalArea += input->Data()->Area( padding );
		const int w = input->Data()->Width() + padding;
		const int h = input->Data()->Height() + padding;
		if ( biggestWidth < w ) {
			biggestWidth = w;
		}
		if ( biggestHeight < h ) {
			biggestHeight = h;
		}
	}
	int finalWidth = RoundUpToPowerOf2( biggestWidth );
	int finalHeight = RoundUpToPowerOf2( biggestHeight );
	int finalArea = finalWidth * finalHeight;
	
	// alternatively increase width / height until it's big enough
	while ( finalArea < totalArea ) {
		if ( ExpandSize( options, finalWidth, finalHeight ) ) {
			finalArea = finalWidth * finalHeight;
		} else {
			// might still be possible to build atlas if exceeded max limits - try it and see
			break;
		}
	}
	
	bool failed;
	OutputImage* outputImage = NULL;
	do {
		failed = false;
		outputImage = new OutputImage( options, finalWidth, finalHeight );
		rbp::MaxRectsBinPack binPacker( finalWidth, finalHeight );

		// Put the images into the bin packer, using the MaxRects algorithm
		for ( const InputImage* input : inputImageList ) {
		// TODO: detect if inserting image fails due to not enough space and handle
			AtlasRect insertedRect = binPacker.Insert( input->Data()->Width() + padding, input->Data()->Height() + padding, rbp::MaxRectsBinPack::RectBestShortSideFit, options.rotationEnabled );
			if ( insertedRect.w == 0 && insertedRect.h == 0 ) {
				// insertion failed - image not big enough. Start again with a larger starting image
				// TODO: account for max output size limit and non-pow2 sizes
				failed = true;
				if ( !ExpandSize( options, finalWidth, finalHeight ) ) {
					throw std::runtime_error( "Input images too large for output image size limits" );
				}
				break;
			}
			std::cout << input->Name() << " pos " << insertedRect.x << "x" << insertedRect.y << " w=" << insertedRect.w << " h=" << insertedRect.h << std::endl;
			const bool isRotated = insertedRect.w != input->Data()->Width() + padding;
			outputImage->AddSubImage( input, isRotated, insertedRect.x + padding, insertedRect.y + padding );
		}
	} while ( failed );
	return outputImage;
}

