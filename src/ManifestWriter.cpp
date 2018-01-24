
#include "main.h"
#include "ManifestWriter.h"
#include "InputImage.h"
#include "OutputImage.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <experimental/filesystem>

/*
// JSON HASH

{"frames": {

"image1":
{
    "frame": {"x":249,"y":205,"w":213,"h":159},
    "rotated": false,
    "trimmed": true,
    "spriteSourceSize": {"x":0,"y":0,"w":213,"h":159},
    "sourceSize": {"w":231,"h":175}
},
"image2":
{
    "frame": {"x":20,"y":472,"w":22,"h":21},
    "rotated": false,
    "trimmed": false,
    "spriteSourceSize": {"x":0,"y":0,"w":22,"h":21},
    "sourceSize": {"w":22,"h":21}
}},
"meta": {
    "app": "https://github.com/urraka/texpack",
    "image": "atlas.png",
    "size": {"w":650,"h":497}
    }
}

// JSON ARRAY

{"frames": [

{
    "filename": "image1",
    "frame": {"x":249,"y":205,"w":213,"h":159},
    "rotated": false,
    "trimmed": true,
    "spriteSourceSize": {"x":0,"y":0,"w":213,"h":159},
    "sourceSize": {"w":231,"h":175}
},
{
    "filename": "image2",
    "frame": {"x":29,"y":472,"w":22,"h":21},
    "rotated": false,
    "trimmed": false,
    "spriteSourceSize": {"x":0,"y":0,"w":22,"h":21},
    "sourceSize": {"w":22,"h":21}
}],
"meta": {
    "app": "https://github.com/urraka/texpack",
    "image": "atlas.png",
    "size": {"w":650,"h":497}
    }
}
*/


std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void RectToJSON( const AtlasRect& rect, std::ostream& ostr ) {
	ostr << "{\"x\":" << rect.x << ",\"y\":" << rect.y << ",\"w\":" << rect.w << ",\"h\":" << rect.h << "}";
}


std::string BoolName( bool b ) {
	return b ? "true" : "false";
}


void LegacyFormat( const OutputImage* output, std::ostream& ostr, const std::string& filename ) {
	const auto& subImages = output->SubImages();
	ostr << "{";
	ostr << "\"properties\":{";
	ostr << "\"width\":" << output->Width() << ",";
	ostr << "\"height\":" << output->Height() << ",";
	ostr << "\"image\": \"" << filename << "\"";
	ostr << "},";
	ostr << "\"subImages\":{\"frames\":{";
	std::size_t index = 0;
	std::for_each( subImages.begin(), subImages.end(), [&] ( const SubImage& subImage ) {
		ostr << "\"" << ReplaceAll( subImage.input->Name(), "\\", "/" ) << "\":{";
		ostr << "\"frame\":";
		RectToJSON(subImage.manifestRect, ostr);
		ostr << ",";
		ostr << "\"rotated\":" << BoolName( subImage.rotated ) << ",";
		ostr << "\"trimmed\":" << BoolName( subImage.input->IsTrimmed() ) << ",";
		ostr << "\"sourceSize\":";
		ostr << "{\"w\":" << subImage.input->OriginalWidth() << ",\"h\":" << subImage.input->OriginalHeight() << "}";
		ostr << ",";
		ostr << "\"spriteSourceSize\":";
		RectToJSON( subImage.input->TrimmedRect(), ostr );
		ostr << "}";
		if ( index < subImages.size()-1 ) {
			ostr << ",";
		}
		index++;
	} );
	ostr << "}}";
	ostr << "}";	
}


void HashFormat( const OutputImage* output, std::ostream& ostr, const std::string& filename, bool isArray = false ) {
/*

{"frames": {

"image1":
{
    "frame": {"x":249,"y":205,"w":213,"h":159},
    "rotated": false,
    "trimmed": true,
    "spriteSourceSize": {"x":0,"y":0,"w":213,"h":159},
    "sourceSize": {"w":231,"h":175}
},
"image2":
{
    "frame": {"x":20,"y":472,"w":22,"h":21},
    "rotated": false,
    "trimmed": false,
    "spriteSourceSize": {"x":0,"y":0,"w":22,"h":21},
    "sourceSize": {"w":22,"h":21}
}},
"meta": {
    "app": "https://github.com/urraka/texpack",
    "image": "atlas.png",
    "size": {"w":650,"h":497}
    }
}
*/
	const auto& subImages = output->SubImages();
	ostr << "{";
	ostr << "\"meta\":{";
	ostr << "\"app\": \"https://github.com/peteward44/atlasbuilder\",";
	ostr << "\"image\": \"" << filename << "\",";
	ostr << "\"size\": {\"w\":" << output->Width() << ", \"h\": " << output->Height() << "}";
	ostr << "},";
	ostr << "\"frames\":";
	ostr << ( isArray ? "[" : "{" );
	std::size_t index = 0;
	std::for_each( subImages.begin(), subImages.end(), [&] ( const SubImage& subImage ) {
		if ( isArray ) {
			ostr << "{ \"filename\": \"" << ReplaceAll( subImage.input->Name(), "\\", "/" ) << "\",";
		} else {
			ostr << "\"" << ReplaceAll( subImage.input->Name(), "\\", "/" ) << "\":{";
		}
		ostr << "\"frame\":";
		RectToJSON(subImage.manifestRect, ostr);
		ostr << ",";
		ostr << "\"rotated\":" << BoolName( subImage.rotated ) << ",";
		ostr << "\"trimmed\":" << BoolName( subImage.input->IsTrimmed() ) << ",";
		ostr << "\"sourceSize\":";
		ostr << "{\"w\":" << subImage.input->OriginalWidth() << ",\"h\":" << subImage.input->OriginalHeight() << "}";
		ostr << ",";
		ostr << "\"spriteSourceSize\":";
		RectToJSON( subImage.input->TrimmedRect(), ostr );
		ostr << "}";
		if ( index < subImages.size()-1 ) {
			ostr << ",";
		}
		index++;
	} );
	ostr << ( isArray ? "]" : "}" );
	ostr << "}";
}


void WriteManifest( const OutputImage* output, std::ostream& ostr, const std::string& imageFilename, const std::string& outputFormat ) {
	const std::experimental::filesystem::path p( imageFilename );
	const std::string filename = ReplaceAll( p.filename().string(), "\\", "/" );

	if ( outputFormat == "legacy" ) {
		LegacyFormat( output, ostr, filename );
	} else if ( outputFormat == "array" ) {
		HashFormat( output, ostr, filename, true );
	} else {
		HashFormat( output, ostr, filename, false );
	}
}
