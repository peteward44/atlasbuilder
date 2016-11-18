#ifndef MAIN_H
#define MAIN_H

#include <cstdio>
#include <cmath>


struct AtlasRect {
	AtlasRect( int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0 )
		: x( x_ ), y( y_ ), w( w_ ), h( h_ )
	{}
	int x, y, w, h;
	inline int Area() const { return w * h; }
	inline bool IsValid() const {
		return x >= 0 && y >= 0 && w > 0 && h > 0;
	}
	inline bool PointInside( int px, int py ) const {
		return x <= px && x + w >= px && y <= py && y + h >= py;
	}
	inline bool Contains( const AtlasRect& rect ) const {
		return PointInside( rect.x, rect.y ) && PointInside( rect.x + rect.w, rect.y + rect.h );
	}
	inline bool IntersectsWith( const AtlasRect& rect ) const {
		return PointInside( rect.x, rect.y ) || PointInside( rect.x, rect.y + rect.h ) || PointInside( rect.x + rect.w, rect.y ) || PointInside( rect.x + rect.w, rect.y + rect.h );
	}
	inline void Scale( float resolution ) {
		x = floorf( x * resolution );
		y = floorf( y * resolution );
		w = floorf( w * resolution );
		h = floorf( h * resolution );
	}
};


#endif
