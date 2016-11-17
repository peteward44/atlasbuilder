
#include "main.h"
#include "ManifestWriter.h"
#include "InputImage.h"
#include "OutputImage.h"
#include <algorithm>
#include <string>
#include <iostream>
#include "boost/filesystem.hpp"


void RectToJSON( const AtlasRect& rect, std::ostream& ostr ) {
	ostr << "{\"x\":" << rect.x << ",\"y\":" << rect.y << ",\"w\":" << rect.w << ",\"h\":" << rect.h << "}";
}


std::string BoolName( bool b ) {
	return b ? "true" : "false";
}


void WriteManifest( const OutputImage* output, std::ostream& ostr, const std::string& imageFilename ) {
	const boost::filesystem::path p( imageFilename );
	const boost::filesystem::path filename = p.leaf();

	const auto& subImages = output->SubImages();
	ostr << "{";
	ostr << "\"properties\":{";
	ostr << "\"width\":" << output->Width() << ",";
	ostr << "\"height\":" << output->Height() << ",";
	ostr << "\"image\":" << filename;
	ostr << "},";
	ostr << "\"subImages\":{\"frames\":{";
	std::size_t index = 0;
	std::for_each( subImages.begin(), subImages.end(), [&] ( const SubImage& subImage ) {
		ostr << "\"" << subImage.input->Name() << "\":{";
		ostr << "\"frame\":";
		RectToJSON(subImage.rect, ostr);
		ostr << ",";
		ostr << "\"rotated\":" << BoolName( subImage.rotated ) << ",";
		ostr << "\"trimmed\":" << BoolName( subImage.input->IsTrimmed() ) << ",";
		ostr << "\"sourceSize\":";
		ostr << "{\"w\":" << subImage.input->Data()->OriginalWidth() << ",\"h\":" << subImage.input->Data()->OriginalHeight() << "}";
		ostr << ",";
		ostr << "\"spriteSourceSize\":";
		RectToJSON( AtlasRect( subImage.input->Data()->X(), subImage.input->Data()->Y(), subImage.input->Data()->Width(), subImage.input->Data()->Height() ), ostr);
		ostr << "}";
		if ( index < subImages.size()-1 ) {
			ostr << ",";
		}
		index++;
	} );
	ostr << "}}";
	ostr << "}";
}
