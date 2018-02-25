#ifndef OUTPUTIMAGE_H
#define OUTPUTIMAGE_H

#include "main.h"
#include <string>
#include <deque>
#include "Options.h"

class InputImage;

struct SubImage {
	inline SubImage( const InputImage* input_, bool rotated_, const AtlasRect& insertionRect_, const AtlasRect& manifestRect_ )
		: input( input_ ), rotated( rotated_ ), insertionRect( insertionRect_ ), manifestRect( manifestRect_ )
	{}
	const InputImage* input;
	bool rotated;
	AtlasRect insertionRect; // Rect that will be passed to the binpacker to insert into final image (includes padding / boundary alignment)
	AtlasRect manifestRect; // Rect that is written to the manifest (does not include padding / alignment)
};


class OutputImage {
	Options _options;
	int _w, _h;
	int _actualWidth, _actualHeight;
	std::deque<SubImage> _subImages;
	void _SortRects();
public:
	OutputImage( const Options& options, int w, int h );
	int AddSubImage( const InputImage* input, bool isRotated, int x, int y );
	int AddDuplicatedSubImage( const InputImage* input, const InputImage* duplicate );
	void Finalise(const std::string& filename);
	inline const std::deque<SubImage>& SubImages() const { return _subImages; }
	inline int Width() const { return _w; }
	inline int Height() const { return _h; }
	
};

#endif
