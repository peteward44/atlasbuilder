
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
	_image = original->_image.resize( resolution );
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
void ImageData::initialise( int argc, char** argv ) {
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

void ImageData::AddPadding(int left, int right, int top, int bottom) {
	// surrounds image with transparent pixels, used for boundary alignment
	// probably more efficient way of doing this but do it this way for now as it works seamlessly with resolution downsampling
	std::cout << "Adding padding l=" << left << " r=" << right << " t=" << top << " b=" << bottom << std::endl;
	VImage image = CreateBlankImage( _width + left + right, _height + top + bottom );
	_image = image.insert( _image, left, top );
	_width = _width + left + right;
	_height = _height + top + bottom;
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
