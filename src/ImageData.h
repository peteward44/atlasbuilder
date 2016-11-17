
#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include <string>
#include "Options.h"

#include <vips/vips8>

// Abstraction interface for vips
class ImageData {
	vips::VImage _image;
	int _x, _y;
	int _width, _height;
	int _owidth, _oheight;
	std::vector<unsigned char*> _rawData;

	ImageData(const std::string& filename);
	ImageData(int width, int height);
	ImageData(ImageData* original, float resolution);
	
	vips::VImage CreateBlankImage( int width, int height, int bpp = 4, int memsetVal = 0 );
	
public:
	~ImageData();
	inline int X() const { return _x; }
	inline int Y() const { return _y; }
	inline int Width() const { return _width; }
	inline int Height() const { return _height; }
	inline int OriginalWidth() const { return _owidth; }
	inline int OriginalHeight() const { return _oheight; }
	inline int Area() const { return _width * _height; }
	
	void Trim();

	void AddPadding(int left, int right, int top, int bottom);
	void InsertSubImage(ImageData* data, const AtlasRect& rect, bool isRotated);

	void Save(const std::string& filename);

	static void initialise( int argc, char** argv );
	static void shutdown();
	static ImageData* createNewResolution(ImageData* original, float resolution);
	static ImageData* createFromFile(const std::string& filename);
	static ImageData* createBlank(int width, int height);
};

#endif
