#ifndef OUTPUTIMAGE_H
#define OUTPUTIMAGE_H

#include "main.h"
#include <string>
#include <deque>
#include "Options.h"

class InputImage;

struct SubImage {
	inline SubImage( const InputImage* input_, bool rotated_, const AtlasRect& rect_ )
		: input( input_ ), rotated( rotated_ ), rect( rect_ )
	{}
	const InputImage* input;
	bool rotated;
	AtlasRect rect;
};


class OutputImage {
	Options _options;
	int _w, _h;
	int _actualWidth, _actualHeight;
	std::deque<SubImage> _subImages;
	void _SortRects();
public:
	OutputImage( const Options& options, int w, int h );
	void AddSubImage( const InputImage* input, bool isRotated, int x, int y );
	void Finalise(const std::string& filename);
	inline const std::deque<SubImage>& SubImages() const { return _subImages; }
	inline int Width() const { return _w; }
	inline int Height() const { return _h; }
	
};

#endif
