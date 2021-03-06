
#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include <string>
#include "Options.h"

#include <vips/vips8>

// Abstraction interface for vips
class ImageData {
	vips::VImage _image;
	unsigned char* _fileBuffer;
	int _width, _height;
	std::vector<unsigned char*> _rawData;

	ImageData(const std::string& filename);
	ImageData(int width, int height);
	ImageData(ImageData* original, float resolution, const std::string& resizeKernel);
	
	vips::VImage CreateBlankImage( int width, int height, int bpp = 4, int memsetVal = 0 );
	
public:
	~ImageData();
	inline int Width() const { return _width; }
	inline int Height() const { return _height; }
	inline int Area( int padding = 0 ) const { return ( _width + padding ) * ( _height + padding ); }
	
	AtlasRect Trim( bool commit, int alignBoundary );

	void InsertSubImage(ImageData* data, const AtlasRect& rect, bool isRotated, bool clockwise = true);

	void Save(const std::string& filename);

	uint32_t CalculateCRC32() const;
	
	static void initialise( int argc, char** argv );
	static void shutdown();
	static ImageData* createNewResolution(ImageData* original, float resolution, const std::string& resizeKernel);
	static ImageData* createFromFile(const std::string& filename);
	static ImageData* createBlank(int width, int height);
};

#endif
