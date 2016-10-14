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
	std::deque<AtlasRect> _availableRects;
	Options _options;
	int _actualWidth, _actualHeight;
	int _virtualWidth, _virtualHeight;
	std::deque<SubImage> _subImages;
	void _AddAvailableRect( const AtlasRect& rect );
	void _SortRects();
public:
	OutputImage( const Options& options );
	void InitVirtualSize( int width, int height );
	bool IncreaseVirtualSize( const Options& options );
	void SplitRect( std::size_t rectIndex, int width, int height );
	void AddSubImage( const InputImage* input, bool isRotated, int x, int y );
	void Finalise(const std::string& filename);
	inline int VirtualWidth() const { return _virtualWidth; }
	inline int VirtualHeight() const { return _virtualHeight; }
	inline const std::deque<AtlasRect>& AvailableRects() const { return _availableRects; }
	inline const std::deque<SubImage>& SubImages() const { return _subImages; }
};

#endif
