
#include "main.h"
#include "imageProcessor.h"
#include "MaxRects.h"
#include <string>
#include <deque>
#include <algorithm>
#include <iostream>


int RoundUpToPowerOf2( int num ) {
	// TODO: probably more efficient way of doing this
	for ( int test = 1; true; test *= 2 )
	{
		if ( num <= test )
			return test;
	}
}

// int GetPadding( const Options& options, int coord ) {
	// return coord == 0 ? 0 : options.padding;
// }

OutputImage* process( std::deque<InputImage*>& inputImageList, const Options& options ) {
	// sort by given area (width * height)
	std::sort( inputImageList.begin(), inputImageList.end(), [] ( const InputImage* lhs, const InputImage* rhs ) {
		return lhs->Data()->Area() > rhs->Data()->Area();
	} );
	
	// Estimate how big the new image should be, by calculating total area required and finding nearest power of 2
	int biggestWidth = 0;
	int biggestHeight = 0;
	int totalArea = 0;
	for ( const InputImage* input : inputImageList ) {
		totalArea += input->Data()->Area();
		const int w = input->Data()->Width();
		const int h = input->Data()->Height();
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
		if ( finalWidth < finalHeight ) {
			finalWidth *= 2;
		} else {
			finalHeight *= 2;
		}
		finalArea = finalWidth * finalHeight;
	}
	
	OutputImage* outputImage = new OutputImage( options, finalWidth, finalHeight );
	rbp::MaxRectsBinPack binPacker( finalWidth, finalHeight );

	// Put the images into the bin packer, using the MaxRects algorithm
	for ( const InputImage* input : inputImageList ) {
	// TODO: detect if inserting image fails due to not enough space and handle
		AtlasRect insertedRect = binPacker.Insert( input->Data()->Width(), input->Data()->Height(), rbp::MaxRectsBinPack::RectBestShortSideFit );
		std::cout << input->Name() << " pos " << insertedRect.x << "x" << insertedRect.y << std::endl;
		const bool isRotated = insertedRect.w != input->Data()->Width();
		outputImage->AddSubImage( input, isRotated, insertedRect.x, insertedRect.y );
	}
	return outputImage;
}

