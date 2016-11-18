
#ifndef INPUT_IMAGE_H
#define INPUT_IMAGE_H

#include <string>
#include "Options.h"
#include "ImageData.h"
#include <map>


class InputImage {
	Options _options;
	int _originalWidth, _originalHeight; // width & height of original resolution, pre-trimmed image
	std::string _name;
	AtlasRect _trimmedRect; // rect containing x, y and w, h of trimmed image in comparison to original
	bool _isTrimmed;
	float _resolution;
	ImageData* _imageData;
	int _baX, _baY;
	
	void _Trim();
	void _AlignBoundary();
public:
	InputImage( const Options& options, const std::string& filename );
	
	void Prep();

	inline ImageData* Data() const { return _imageData; }

	inline int OriginalWidth() const { return _originalWidth; }
	inline int OriginalHeight() const { return _originalHeight; }
	inline const AtlasRect& TrimmedRect() const { return _trimmedRect; }
	inline bool IsTrimmed() const { return _isTrimmed; }

	inline const std::string& Name() const { return _name; }
};

#endif
