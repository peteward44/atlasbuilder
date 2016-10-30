
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
	AtlasRect rect( x, y, input->Data()->Width(), input->Data()->Height() );
	_subImages.push_back( SubImage( input, isRotated, rect ) );
	// update actual width/height
	if ( !isRotated ) {
		if ( _actualWidth < rect.x + rect.w ) {
			_actualWidth = rect.x + rect.w;
		}
		if ( _actualHeight < rect.y + rect.h ) {
			_actualHeight = rect.y + rect.h;
		}
	} else {
		if ( _actualWidth < rect.x + rect.h ) {
			_actualWidth = rect.x + rect.h;
		}
		if ( _actualHeight < rect.y + rect.w ) {
			_actualHeight = rect.y + rect.w;
		}
	}	
}

void OutputImage::Finalise( const std::string& filename ) {
	int width, height;
	width = _w;
	height = _h;
	ImageData* data = ImageData::createBlank(width, height);
	std::for_each(_subImages.begin(), _subImages.end(), [&](const SubImage& subImage) {
		std::cout << "Inserting sub image " << subImage.input->Name() << " at " << subImage.rect.x << "x" << subImage.rect.y << " [" << subImage.rect.w << "x" << subImage.rect.h << "]" << std::endl;
		data->InsertSubImage(subImage.input->Data(), subImage.rect, subImage.rotated);
	});
	data->Save(filename);
	delete data;
}
