/** @file MaxRectsBinPack.cpp
	@author Jukka Jylänki
	@brief Implements different bin packer algorithms that use the MAXRECTS data structure.
	This work is released to Public Domain, do whatever you want with it.
*/
#include <utility>
#include <iostream>
#include <limits>

#include <cassert>
#include <cstring>
#include <cmath>

#include "MaxRects.h"

bool IsContainedIn(const AtlasRect &a, const AtlasRect &b)
{
	return a.x >= b.x && a.y >= b.y 
		&& a.x+a.w <= b.x+b.w 
		&& a.y+a.h <= b.y+b.h;
}

namespace rbp {

using namespace std;

MaxRectsBinPack::MaxRectsBinPack()
:binWidth(0),
binHeight(0)
{
}

MaxRectsBinPack::MaxRectsBinPack(int width, int height)
{
	Init(width, height);
}

void MaxRectsBinPack::Init(int width, int height)
{
	binWidth = width;
	binHeight = height;

	AtlasRect n;
	n.x = 0;
	n.y = 0;
	n.w = width;
	n.h = height;

	usedRectangles.clear();

	freeRectangles.clear();
	freeRectangles.push_back(n);
}

AtlasRect MaxRectsBinPack::Insert(int width, int height, FreeRectChoiceHeuristic method, bool rotationEnabled)
{
	AtlasRect newNode;
	// Unused in this function. We don't need to know the score after finding the position.
	int score1 = std::numeric_limits<int>::max();
	int score2 = std::numeric_limits<int>::max();
	switch(method)
	{
		case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2, rotationEnabled); break;
		case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2, rotationEnabled); break;
		case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1, rotationEnabled); break;
		case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1, rotationEnabled); break;
		case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2, rotationEnabled); break;
	}
		
	if (newNode.h == 0)
		return newNode;

	size_t numRectanglesToProcess = freeRectangles.size();
	for(size_t i = 0; i < numRectanglesToProcess; ++i)
	{
		if (SplitFreeNode(freeRectangles[i], newNode))
		{
			freeRectangles.erase(freeRectangles.begin() + i);
			--i;
			--numRectanglesToProcess;
		}
	}

	PruneFreeList();

	usedRectangles.push_back(newNode);
	return newNode;
}

// void MaxRectsBinPack::Insert(std::vector<AtlasRect> &rects, std::vector<AtlasRect> &dst, FreeRectChoiceHeuristic method)
// {
	// dst.clear();

	// while(rects.size() > 0)
	// {
		// int bestScore1 = std::numeric_limits<int>::max();
		// int bestScore2 = std::numeric_limits<int>::max();
		// int bestRectIndex = -1;
		// AtlasRect bestNode;

		// for(size_t i = 0; i < rects.size(); ++i)
		// {
			// int score1;
			// int score2;
			// AtlasRect newNode = ScoreRect(rects[i].w, rects[i].h, method, score1, score2);

			// if (score1 < bestScore1 || (score1 == bestScore1 && score2 < bestScore2))
			// {
				// bestScore1 = score1;
				// bestScore2 = score2;
				// bestNode = newNode;
				// bestRectIndex = i;
			// }
		// }

		// if (bestRectIndex == -1)
			// return;

		// PlaceRect(bestNode);
		// rects.erase(rects.begin() + bestRectIndex);
	// }
// }

void MaxRectsBinPack::PlaceRect(const AtlasRect &node)
{
	size_t numRectanglesToProcess = freeRectangles.size();
	for(size_t i = 0; i < numRectanglesToProcess; ++i)
	{
		if (SplitFreeNode(freeRectangles[i], node))
		{
			freeRectangles.erase(freeRectangles.begin() + i);
			--i;
			--numRectanglesToProcess;
		}
	}

	PruneFreeList();

	usedRectangles.push_back(node);
	//		dst.push_back(bestNode); ///\todo Refactor so that this compiles.
}

AtlasRect MaxRectsBinPack::ScoreRect(int width, int height, FreeRectChoiceHeuristic method, int &score1, int &score2, bool rotationEnabled) const
{
	AtlasRect newNode;
	score1 = std::numeric_limits<int>::max();
	score2 = std::numeric_limits<int>::max();
	switch(method)
	{
	case RectBestShortSideFit: newNode = FindPositionForNewNodeBestShortSideFit(width, height, score1, score2, rotationEnabled); break;
	case RectBottomLeftRule: newNode = FindPositionForNewNodeBottomLeft(width, height, score1, score2, rotationEnabled); break;
	case RectContactPointRule: newNode = FindPositionForNewNodeContactPoint(width, height, score1, rotationEnabled); 
		score1 = -score1; // Reverse since we are minimizing, but for contact point score bigger is better.
		break;
	case RectBestLongSideFit: newNode = FindPositionForNewNodeBestLongSideFit(width, height, score2, score1, rotationEnabled); break;
	case RectBestAreaFit: newNode = FindPositionForNewNodeBestAreaFit(width, height, score1, score2, rotationEnabled); break;
	}

	// Cannot fit the current rectangle.
	if (newNode.h == 0)
	{
		score1 = std::numeric_limits<int>::max();
		score2 = std::numeric_limits<int>::max();
	}

	return newNode;
}

/// Computes the ratio of used surface area.
float MaxRectsBinPack::Occupancy() const
{
	unsigned long usedSurfaceArea = 0;
	for(size_t i = 0; i < usedRectangles.size(); ++i)
		usedSurfaceArea += usedRectangles[i].w * usedRectangles[i].h;

	return (float)usedSurfaceArea / (binWidth * binHeight);
}

AtlasRect MaxRectsBinPack::FindPositionForNewNodeBottomLeft(int width, int height, int &bestY, int &bestX, bool rotationEnabled) const
{
	AtlasRect bestNode;
	memset(&bestNode, 0, sizeof(AtlasRect));

	bestY = std::numeric_limits<int>::max();
	bestX = std::numeric_limits<int>::max();

	for(size_t i = 0; i < freeRectangles.size(); ++i)
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if (freeRectangles[i].w >= width && freeRectangles[i].h >= height)
		{
			int topSideY = freeRectangles[i].y + height;
			if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].x < bestX))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = width;
				bestNode.h = height;
				bestY = topSideY;
				bestX = freeRectangles[i].x;
			}
		}
		if (rotationEnabled && freeRectangles[i].w >= height && freeRectangles[i].h >= width)
		{
			int topSideY = freeRectangles[i].y + width;
			if (topSideY < bestY || (topSideY == bestY && freeRectangles[i].x < bestX))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = height;
				bestNode.h = width;
				bestY = topSideY;
				bestX = freeRectangles[i].x;
			}
		}
	}
	return bestNode;
}

AtlasRect MaxRectsBinPack::FindPositionForNewNodeBestShortSideFit(int width, int height, 
	int &bestShortSideFit, int &bestLongSideFit, bool rotationEnabled) const
{
	AtlasRect bestNode;
	memset(&bestNode, 0, sizeof(AtlasRect));

	bestShortSideFit = std::numeric_limits<int>::max();
	bestLongSideFit = std::numeric_limits<int>::max();

	for(size_t i = 0; i < freeRectangles.size(); ++i)
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if (freeRectangles[i].w >= width && freeRectangles[i].h >= height)
		{
			int leftoverHoriz = abs(freeRectangles[i].w - width);
			int leftoverVert = abs(freeRectangles[i].h - height);
			int shortSideFit = min(leftoverHoriz, leftoverVert);
			int longSideFit = max(leftoverHoriz, leftoverVert);

			if (shortSideFit < bestShortSideFit || (shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = width;
				bestNode.h = height;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}

		if (rotationEnabled && freeRectangles[i].w >= height && freeRectangles[i].h >= width)
		{
			int flippedLeftoverHoriz = abs(freeRectangles[i].w - height);
			int flippedLeftoverVert = abs(freeRectangles[i].h - width);
			int flippedShortSideFit = min(flippedLeftoverHoriz, flippedLeftoverVert);
			int flippedLongSideFit = max(flippedLeftoverHoriz, flippedLeftoverVert);

			if (flippedShortSideFit < bestShortSideFit || (flippedShortSideFit == bestShortSideFit && flippedLongSideFit < bestLongSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = height;
				bestNode.h = width;
				bestShortSideFit = flippedShortSideFit;
				bestLongSideFit = flippedLongSideFit;
			}
		}
	}
	return bestNode;
}

AtlasRect MaxRectsBinPack::FindPositionForNewNodeBestLongSideFit(int width, int height, 
	int &bestShortSideFit, int &bestLongSideFit, bool rotationEnabled) const
{
	AtlasRect bestNode;
	memset(&bestNode, 0, sizeof(AtlasRect));

	bestShortSideFit = std::numeric_limits<int>::max();
	bestLongSideFit = std::numeric_limits<int>::max();

	for(size_t i = 0; i < freeRectangles.size(); ++i)
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if (freeRectangles[i].w >= width && freeRectangles[i].h >= height)
		{
			int leftoverHoriz = abs(freeRectangles[i].w - width);
			int leftoverVert = abs(freeRectangles[i].h - height);
			int shortSideFit = min(leftoverHoriz, leftoverVert);
			int longSideFit = max(leftoverHoriz, leftoverVert);

			if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = width;
				bestNode.h = height;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}

		if (rotationEnabled && freeRectangles[i].w >= height && freeRectangles[i].h >= width)
		{
			int leftoverHoriz = abs(freeRectangles[i].w - height);
			int leftoverVert = abs(freeRectangles[i].h - width);
			int shortSideFit = min(leftoverHoriz, leftoverVert);
			int longSideFit = max(leftoverHoriz, leftoverVert);

			if (longSideFit < bestLongSideFit || (longSideFit == bestLongSideFit && shortSideFit < bestShortSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = height;
				bestNode.h = width;
				bestShortSideFit = shortSideFit;
				bestLongSideFit = longSideFit;
			}
		}
	}
	return bestNode;
}

AtlasRect MaxRectsBinPack::FindPositionForNewNodeBestAreaFit(int width, int height, 
	int &bestAreaFit, int &bestShortSideFit, bool rotationEnabled) const
{
	AtlasRect bestNode;
	memset(&bestNode, 0, sizeof(AtlasRect));

	bestAreaFit = std::numeric_limits<int>::max();
	bestShortSideFit = std::numeric_limits<int>::max();

	for(size_t i = 0; i < freeRectangles.size(); ++i)
	{
		int areaFit = freeRectangles[i].w * freeRectangles[i].h - width * height;

		// Try to place the rectangle in upright (non-flipped) orientation.
		if (freeRectangles[i].w >= width && freeRectangles[i].h >= height)
		{
			int leftoverHoriz = abs(freeRectangles[i].w - width);
			int leftoverVert = abs(freeRectangles[i].h - height);
			int shortSideFit = min(leftoverHoriz, leftoverVert);

			if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = width;
				bestNode.h = height;
				bestShortSideFit = shortSideFit;
				bestAreaFit = areaFit;
			}
		}

		if (rotationEnabled && freeRectangles[i].w >= height && freeRectangles[i].h >= width)
		{
			int leftoverHoriz = abs(freeRectangles[i].w - height);
			int leftoverVert = abs(freeRectangles[i].h - width);
			int shortSideFit = min(leftoverHoriz, leftoverVert);

			if (areaFit < bestAreaFit || (areaFit == bestAreaFit && shortSideFit < bestShortSideFit))
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = height;
				bestNode.h = width;
				bestShortSideFit = shortSideFit;
				bestAreaFit = areaFit;
			}
		}
	}
	return bestNode;
}

/// Returns 0 if the two intervals i1 and i2 are disjoint, or the length of their overlap otherwise.
int CommonIntervalLength(int i1start, int i1end, int i2start, int i2end)
{
	if (i1end < i2start || i2end < i1start)
		return 0;
	return min(i1end, i2end) - max(i1start, i2start);
}

int MaxRectsBinPack::ContactPointScoreNode(int x, int y, int width, int height) const
{
	int score = 0;

	if (x == 0 || x + width == binWidth)
		score += height;
	if (y == 0 || y + height == binHeight)
		score += width;

	for(size_t i = 0; i < usedRectangles.size(); ++i)
	{
		if (usedRectangles[i].x == x + width || usedRectangles[i].x + usedRectangles[i].w == x)
			score += CommonIntervalLength(usedRectangles[i].y, usedRectangles[i].y + usedRectangles[i].h, y, y + height);
		if (usedRectangles[i].y == y + height || usedRectangles[i].y + usedRectangles[i].h == y)
			score += CommonIntervalLength(usedRectangles[i].x, usedRectangles[i].x + usedRectangles[i].w, x, x + width);
	}
	return score;
}

AtlasRect MaxRectsBinPack::FindPositionForNewNodeContactPoint(int width, int height, int &bestContactScore, bool rotationEnabled) const
{
	AtlasRect bestNode;
	memset(&bestNode, 0, sizeof(AtlasRect));

	bestContactScore = -1;

	for(size_t i = 0; i < freeRectangles.size(); ++i)
	{
		// Try to place the rectangle in upright (non-flipped) orientation.
		if (freeRectangles[i].w >= width && freeRectangles[i].h >= height)
		{
			int score = ContactPointScoreNode(freeRectangles[i].x, freeRectangles[i].y, width, height);
			if (score > bestContactScore)
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = width;
				bestNode.h = height;
				bestContactScore = score;
			}
		}
		if (rotationEnabled && freeRectangles[i].w >= height && freeRectangles[i].h >= width)
		{
			int score = ContactPointScoreNode(freeRectangles[i].x, freeRectangles[i].y, height, width);
			if (score > bestContactScore)
			{
				bestNode.x = freeRectangles[i].x;
				bestNode.y = freeRectangles[i].y;
				bestNode.w = height;
				bestNode.h = width;
				bestContactScore = score;
			}
		}
	}
	return bestNode;
}

bool MaxRectsBinPack::SplitFreeNode(AtlasRect freeNode, const AtlasRect &usedNode)
{
	// Test with SAT if the rectangles even intersect.
	if (usedNode.x >= freeNode.x + freeNode.w || usedNode.x + usedNode.w <= freeNode.x ||
		usedNode.y >= freeNode.y + freeNode.h || usedNode.y + usedNode.h <= freeNode.y)
		return false;

	if (usedNode.x < freeNode.x + freeNode.w && usedNode.x + usedNode.w > freeNode.x)
	{
		// New node at the top side of the used node.
		if (usedNode.y > freeNode.y && usedNode.y < freeNode.y + freeNode.h)
		{
			AtlasRect newNode = freeNode;
			newNode.h = usedNode.y - newNode.y;
			freeRectangles.push_back(newNode);
		}

		// New node at the bottom side of the used node.
		if (usedNode.y + usedNode.h < freeNode.y + freeNode.h)
		{
			AtlasRect newNode = freeNode;
			newNode.y = usedNode.y + usedNode.h;
			newNode.h = freeNode.y + freeNode.h - (usedNode.y + usedNode.h);
			freeRectangles.push_back(newNode);
		}
	}

	if (usedNode.y < freeNode.y + freeNode.h && usedNode.y + usedNode.h > freeNode.y)
	{
		// New node at the left side of the used node.
		if (usedNode.x > freeNode.x && usedNode.x < freeNode.x + freeNode.w)
		{
			AtlasRect newNode = freeNode;
			newNode.w = usedNode.x - newNode.x;
			freeRectangles.push_back(newNode);
		}

		// New node at the right side of the used node.
		if (usedNode.x + usedNode.w < freeNode.x + freeNode.w)
		{
			AtlasRect newNode = freeNode;
			newNode.x = usedNode.x + usedNode.w;
			newNode.w = freeNode.x + freeNode.w - (usedNode.x + usedNode.w);
			freeRectangles.push_back(newNode);
		}
	}

	return true;
}

void MaxRectsBinPack::PruneFreeList()
{
	/* 
	///  Would be nice to do something like this, to avoid a Theta(n^2) loop through each pair.
	///  But unfortunately it doesn't quite cut it, since we also want to detect containment. 
	///  Perhaps there's another way to do this faster than Theta(n^2).
	if (freeRectangles.size() > 0)
		clb::sort::QuickSort(&freeRectangles[0], freeRectangles.size(), NodeSortCmp);
	for(size_t i = 0; i < freeRectangles.size()-1; ++i)
		if (freeRectangles[i].x == freeRectangles[i+1].x &&
		    freeRectangles[i].y == freeRectangles[i+1].y &&
		    freeRectangles[i].w == freeRectangles[i+1].w &&
		    freeRectangles[i].h == freeRectangles[i+1].h)
		{
			freeRectangles.erase(freeRectangles.begin() + i);
			--i;
		}
	*/

	/// Go through each pair and remove any rectangle that is redundant.
	for(size_t i = 0; i < freeRectangles.size(); ++i)
		for(size_t j = i+1; j < freeRectangles.size(); ++j)
		{
			if (IsContainedIn(freeRectangles[i], freeRectangles[j]))
			{
				freeRectangles.erase(freeRectangles.begin()+i);
				--i;
				break;
			}
			if (IsContainedIn(freeRectangles[j], freeRectangles[i]))
			{
				freeRectangles.erase(freeRectangles.begin()+j);
				--j;
			}
		}
}

}