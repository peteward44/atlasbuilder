
#include "main.h"
#include "ImageData.h"
#include <string>
#include <list>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace vips;


bool g_isInitialised = false;


ImageData::ImageData( const std::string& filename ) {
	try {
		_image = VImage::new_from_file( filename.c_str(), VImage::option()->set( "access",  VIPS_ACCESS_RANDOM /* VIPS_ACCESS_SEQUENTIAL_UNBUFFERED */ ) );
		if ( _image.bands() == 1 ) {
			// grayscale images / no alpha
			std::vector< vips::VImage > bands;
			bands.push_back( _image );
			bands.push_back( _image );
			bands.push_back( _image );
			_image = _image.bandjoin( bands );
		}
		if ( _image.bands() == 2 ) {
			// grayscale image / with alpha
			auto gray = _image.extract_band( 0 );
			auto alpha = _image.extract_band( 1 );
			std::vector< vips::VImage > bands;
			bands.push_back( gray );
			bands.push_back( gray );
			bands.push_back( gray );
			bands.push_back( alpha );
			_image = _image.bandjoin( bands );
		}
		if ( _image.bands() == 3 ) {
			// tried to do this by only using _image = _image.bandjoin( 255 ) which apparently should work,
			// but instead of increasing band count from 3 -> 4 it would go from 3 -> 6... gave up and just create our own single-band image instead
			VImage alphaOnly = CreateBlankImage( _image.width(), _image.height(), 1, 255 );
			_image = _image.bandjoin( alphaOnly );
		}
		_width = _image.width();
		_height = _image.height();
	}
	catch ( std::exception& e ) {
		std::cerr << "Error loading " << e.what() << std::endl;
		throw std::runtime_error( "Could not load input file " + filename );
	}
}

ImageData::ImageData(int width, int height) {
	_width = width;
	_height = height;
	_image = CreateBlankImage( width, height );
}

ImageData::ImageData(ImageData* original, float resolution) {
	_image = original->_image.resize( resolution/*, VImage::option()->set( "kernel", VIPS_KERNEL_LINEAR )*/ );
	_width = _image.width();
	_height = _image.height();
}

ImageData::~ImageData() {
	std::for_each( _rawData.begin(), _rawData.end(), [] ( unsigned char* data ) {
		delete[] data;
	} );
	_rawData.clear();
}


VImage ImageData::CreateBlankImage( int width, int height, int bpp, int memsetVal ) {
	const int size = width * height * bpp;
	unsigned char* rawData = new unsigned char[ size ];
	memset( rawData, memsetVal, size );
	VImage image = VImage::new_from_memory( rawData, size, width, height, bpp, VIPS_FORMAT_UCHAR );
	_rawData.push_back( rawData );
	return image;
}


// static
void ImageData::initialise( int argc, const char** argv ) {
	// VIPS_INIT outputs warnings on -Wall - disable them as there's nothing i can do
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#pragma GCC diagnostic ignored "-Wformat"
	if( VIPS_INIT( argv[0] ) )
		throw std::runtime_error( "Unable to initialise libvips" );
#pragma GCC diagnostic pop
	g_isInitialised = true;
}

// static
void ImageData::shutdown() {
	if ( g_isInitialised ) {
		vips_shutdown();
		g_isInitialised = false;
	}
}

// static
ImageData* ImageData::createNewResolution(ImageData* original, float resolution) {
	return new ImageData( original, resolution );
}

// static
ImageData* ImageData::createFromFile(const std::string& filename) {
	return new ImageData(filename);
}

// static
ImageData* ImageData::createBlank(int width, int height) {
	return new ImageData( width, height );
}

void ImageData::InsertSubImage(ImageData* data, const AtlasRect& rect, bool isRotated) {
	// inserts given sub image in the rect x,y position
	if ( !isRotated ) {
		_image = _image.insert( data->_image, rect.x, rect.y );
	} else {
		VImage image = data->_image.rot( VIPS_ANGLE_D270 );
		_image = _image.insert( image, rect.x, rect.y );
	}
}

void ImageData::Save(const std::string& filename) {
	std::cout << "Saving to " << filename << std::endl;
	_image.write_to_file( filename.c_str() );
}

AtlasRect ImageData::Trim( bool commit, int alignBoundary ) {
	// adjusted from https://github.com/jcupitt/libvips/issues/233
	// sum rows and columns, then search for the first non-zero sum in each
	VImage mask = _image.extract_band( 3 ); // extract alpha only
	VImage rows;
	VImage columns = mask.profile( &rows );
	int top = columns.min();
	int left = rows.min();
	VImage flippedMask = mask.flip( VIPS_DIRECTION_HORIZONTAL ).flip( VIPS_DIRECTION_VERTICAL );
	VImage frows;
	VImage fcolumns = flippedMask.profile( &frows );
	int bottom = fcolumns.min();
	int right = frows.min();
	if ( alignBoundary > 0 ) {
		const int boundary = alignBoundary;
		int leftover = top % boundary;
		if ( leftover > 0 ) {
			top -= leftover;
			if ( top < 0 ) {
				top = 0;
			}
		}
		leftover = left % boundary;
		if ( leftover > 0 ) {
			left -= leftover;
			if ( left < 0 ) {
				left = 0;
			}
		}
		const int absright = _width - right;
		leftover = absright % boundary;
		if ( leftover > 0 ) {
			right -= boundary - leftover;
			if ( right < 0 ) {
				right = 0;
			}
		}
		const int absbottom = _height - bottom;
		leftover = absbottom % boundary;
		if ( leftover > 0 ) {
			bottom -= boundary - leftover;
			if ( bottom < 0 ) {
				bottom = 0;
			}
		}
	}
	const int width = _width - right - left;
	const int height = _height - bottom - top;
	if ( commit ) {
		_image = _image.extract_area( left, top, width, height );
		_width = width;
		_height = height;
	}
	return AtlasRect( left, top, width, height );
}

