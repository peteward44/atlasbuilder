
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


OutputImage* process( std::deque<InputImage*>& inputImageList, const Options& options ) {
	// sort by given area (width * height)
	std::sort( inputImageList.begin(), inputImageList.end(), [&] ( const InputImage* lhs, const InputImage* rhs ) {
		return lhs->Area( true ) > rhs->Area( true );
	} );
	
	// Estimate how big the new image should be, by calculating total area required and finding nearest power of 2
	int biggestWidth = 0;
	int biggestHeight = 0;
	int totalArea = 0;
	for ( const InputImage* input : inputImageList ) {
		totalArea += input->Area( true );
		const int w = input->Width( true );
		const int h = input->Height( true );
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
	
	// detect identical frames
	std::map< uint32_t, InputImage* > crcs; // map of crc vs. input image
	for ( InputImage* input : inputImageList ) {
		std::cout << "observing "<< input->Name() << std::endl;
		const auto crc = input->Data()->CalculateCRC32();
		std::cout << input->Name() << " crc " << crc << std::endl;
		const auto it = crcs.find( crc );
		if ( it != crcs.end() ) {
			std::cout << "Duplicate found " << input->Name() << std::endl;
			input->SetDuplicate( it->second );
		} else {
			crcs.insert( std::make_pair( crc, input ) );
		}
	}

	bool failed;
	OutputImage* outputImage = NULL;
	do {
		failed = false;
		outputImage = new OutputImage( options, finalWidth, finalHeight );
		// subtract padding for padding on left and top sides of image
		rbp::MaxRectsBinPack binPacker( finalWidth - options.padding * 2, finalHeight - options.padding * 2 );

		// Put the images into the bin packer, using the MaxRects algorithm
		for ( const InputImage* input : inputImageList ) {
			if ( input->GetDuplicate() == NULL ) {
				// TODO: detect if inserting image fails due to not enough space and handle
				AtlasRect insertedRect = binPacker.Insert( input->Width( true ), input->Height( true ), rbp::MaxRectsBinPack::RectBestShortSideFit, options.rotationEnabled );
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
				const bool isRotated = insertedRect.w != input->Width( true );
				outputImage->AddSubImage( input, isRotated, insertedRect.x + options.padding, insertedRect.y + options.padding );
			} else {
				std::cout << input->Name() << " duplicated by " << input->GetDuplicate()->Name() << std::endl;
				outputImage->AddDuplicatedSubImage( input, input->GetDuplicate() );
			}
		}
	} while ( failed );
	return outputImage;
}

