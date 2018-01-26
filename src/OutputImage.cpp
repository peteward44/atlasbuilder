
#include "OutputImage.h"
#include "InputImage.h"
#include <algorithm>
#include <iostream>

const int maxOutputWidth = 8192;
const int maxOutputHeight = 8192;


OutputImage::OutputImage(const Options& options, int w, int h)
	: _options( options ), _w( w ), _h( h ), _actualWidth(0), _actualHeight(0) {
}


void OutputImage::AddSubImage( const InputImage* input, bool isRotated, int x, int y ) {
	// copy image data into atlas canvas, and add sub image to list
	AtlasRect insertionRect( x, y, input->Width( true ), input->Height( true ) );
	AtlasRect manifestRect( x, y, input->Data()->Width(), input->Data()->Height() );
	_subImages.push_back( SubImage( input, isRotated, insertionRect, manifestRect ) );
	// update actual width/height
	if ( !isRotated ) {
		if ( _actualWidth < insertionRect.x + insertionRect.w ) {
			_actualWidth = insertionRect.x + insertionRect.w;
		}
		if ( _actualHeight < insertionRect.y + insertionRect.h ) {
			_actualHeight = insertionRect.y + insertionRect.h;
		}
	} else {
		if ( _actualWidth < insertionRect.x + insertionRect.h ) {
			_actualWidth = insertionRect.x + insertionRect.h;
		}
		if ( _actualHeight < insertionRect.y + insertionRect.w ) {
			_actualHeight = insertionRect.y + insertionRect.w;
		}
	}	
}

void OutputImage::Finalise( const std::string& filename ) {
	int width, height;
	width = _w;
	height = _h;
	ImageData* data = ImageData::createBlank(width, height);
	std::for_each(_subImages.begin(), _subImages.end(), [&](const SubImage& subImage) {
		std::cout << "Inserting sub image " << subImage.input->Name() << " at " << subImage.insertionRect.x << "x" << subImage.insertionRect.y << " [" << subImage.insertionRect.w << "x" << subImage.insertionRect.h << "]" << std::endl;
		data->InsertSubImage(subImage.input->Data(), subImage.insertionRect, subImage.rotated, !_options.rotateAntiClockwise);
	});
	data->Save(filename);
	delete data;
}
