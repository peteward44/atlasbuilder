#include "Options.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <boost/algorithm/string.hpp>
#include "args/args.hxx"

// Args library from https://github.com/Taywee/args

const std::string VERSION = "2.0.0";

void PrintHelp() {
	std::cout << "usage: atlasbuilder [options] <input files...>" << std::endl;
 	std::cout << "example: atlasbuilder --output=myoutputname input1.png input2.png" << std::endl;
	std::cout << std::endl;
}

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
// std::pair<std::string, std::string> at_option_parser( const std::string& s ) {
	// if ('@' == s[0])
		// return std::make_pair( std::string("response-file"), s.substr(1) );
	// else
		// return std::pair<std::string, std::string>();
// }

std::string FileToString( const std::string& filename ) {
	std::ifstream t( filename.c_str() );
	std::string str;

	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
	return str;
}

void ProcessInputFileEntry( int levelLimit, const std::experimental::filesystem::path& root, std::vector< std::string >& inputFiles, int levelIndex = 0 ) {
	const bool recurse = levelLimit > 0 ? levelLimit > levelIndex : true;
	if ( recurse && std::experimental::filesystem::is_directory( root ) ) {
		for ( const auto& entry: std::experimental::filesystem::directory_iterator( root ) ) {
			ProcessInputFileEntry( levelLimit, entry.path(), inputFiles, levelIndex + 1 );
		}
	} else if ( std::experimental::filesystem::is_regular_file( root ) ) {
		const auto ext = boost::algorithm::to_lower_copy( root.extension().string() );
		if ( ext == ".png" ) {
			inputFiles.push_back( root.string() );
		}
	}
}

Options ParseArgv(int argc, char** argv) {
	// check if a response file was specified
	std::vector<std::string*> responseContents;
	std::vector<char*> responsePointers;
	if ( argc >= 2 ) {
		const std::string response = argv[1];
		if ( response.at( 0 ) == '@' ) {
			responseContents.push_back( new std::string( argv[0] ) );
			responsePointers.push_back( (char*)responseContents[ responseContents.size()-1 ]->c_str() );
			std::string filename = response.substr( 1 );
			std::string line;
			std::ifstream t( filename.c_str() );
			while ( std::getline( t, line ) ) {
				responseContents.push_back( new std::string( line ) );
				responsePointers.push_back( (char*)responseContents[ responseContents.size()-1 ]->c_str() );
			}
			argv = &responsePointers[0];
			argc = responsePointers.size();
		}
	}
	
	args::ArgumentParser parser( "Atlasbuilder ", VERSION );
	
	args::HelpFlag help( parser, "help", "Display this help menu", { 'h', "help" } );
	args::ValueFlag<std::string> output( parser, "output", "set output name", {"output"});

	args::Flag version( parser, "version", "Output version number", {'v', "version"});

	args::ValueFlag<int> outputWidth( parser, "output-width", "set maximum output image width", { "output-width" } );
	args::ValueFlag<int> outputHeight( parser, "output-height", "set maximum output image height", { "output-height" } );

	args::Group xorgroup( parser, "Enable or disable rotation:", args::Group::Validators::AtMostOne );
	args::Flag rotationEnabled( xorgroup, "rotation-enabled", "Enable sub image rotation", {"rotation-enabled"});
	args::Flag rotationDisabled( xorgroup, "rotation-disabled", "Disable sub image rotation", {"rotation-disabled"});
	args::Flag rotateAntiClockwise( parser, "rotation-anticlockwise", "Rotate the sub images anti-clockwise instead of clockwise", {"rotation-anticlockwise"});
	
	args::Group trimGroup( parser, "Enable or disable trim:", args::Group::Validators::AtMostOne );
	args::Flag trimEnabled( trimGroup, "trim-enabled", "Enable sub image trimming", {"trim-enabled"});
	args::Flag trimDisabled( trimGroup, "trim-disabled", "Disable sub image trimming", {"trim-disabled"});

	args::Group xorScaleManifest( parser, "Enable or disable scaling manifest values (defaults to enabled):", args::Group::Validators::AtMostOne );
	args::Flag scaleManifestEnabled( xorScaleManifest, "scale-manifest-enabled", "if using a resolution other than 1, use scaled x,y,w,h values in the manifest", {"scale-manifest-enabled"});
	args::Flag scaleManifestDisabled( xorScaleManifest, "scale-manifest-disabled", "if using a resolution other than 1, use original x,y,w,h values in manifest", {"scale-manifest-disabled"});

	args::Flag failIfTooBig( parser, "fail-if-too-big", "fail build if the output-width and output-height will be exceeded", {"fail-if-too-big"});	
	//args::Flag powerTwo( parser, "output-pow2", "final output image should always be a power of 2", {"output-pow2"});
	
	args::ValueFlag<int> padding( parser, "padding", "padding to insert between each sub image in atlas", { "padding" } );
	args::ValueFlag<int> boundaryAlignment( parser, "boundary-alignment", "boundary-alignment to align each sub image in atlas", { "boundary-alignment" } );
	args::ValueFlag<int> trimBoundaryAlignment( parser, "trim-boundary-alignment", "boundary-alignment to align when trimming", { "trim-boundary-alignment" } );
	
	args::ValueFlag<float> resolution( parser, "resolution", "resolution to output (default is 1.0)", { "resolution" } );
	args::ValueFlag<std::string> manifestFormat( parser, "manifest-format", "format to output the JSON manifest, either 'hash', 'array' or 'legacy'", { "manifest-format" } );
//	args::ValueFlag<std::string> imageFormat( parser, "image-format", "format to output the image, either 'png', 'dds' or 'ktx'", { "image-format" } );
		
	args::Flag noOutputImage( parser, "no-output-image", "Do not output the final image", {"no-output-image"});	
	args::Flag noOutputJson( parser, "no-output-json", "Do not output the final json manifest", {"no-output-json"});
	
	args::ValueFlag<std::string> resizeKernel( parser, "resize-kernel", "Algorithm to use when resizing images. Either 'nearest', 'linear', 'cubic', 'lanczos2' or 'lanczos3'", {"resize-kernel"});
	
	args::Flag recursive( parser, "recursive", "When a directory is specified on the command line, traverse recursively", {"recursive"});	
	args::PositionalList<std::string> inputFiles(parser, "input-files", "Input files to process");
	
	Options options;
		
	try
	{
		parser.ParseCLI( argc, argv );
	}
	catch ( const args::Help& )
	{
		std::cout << parser;
		return options;
	}
	catch ( const args::ParseError& e )
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return options;
	}
	
	if ( version ) {
		std::cout << VERSION << std::endl;
		return options;
	}

	if ( output ) {
		options.outputName = args::get( output );
	}

	if ( outputWidth ) {
		options.maxOutputWidth = args::get( outputWidth );
	}

	if ( outputHeight ) {
		options.maxOutputHeight = args::get( outputHeight );
	}

	if ( failIfTooBig ) {
		options.failOnTooBig = true;
	}

	if ( rotationEnabled ) {
		options.rotationEnabled = true;
	}

	if ( rotationDisabled ) {
		options.rotationEnabled = false;
	}
	
	if ( rotateAntiClockwise ) {
		options.rotateAntiClockwise = true;
	}

	if ( trimEnabled ) {
		options.trimEnabled = true;
	}

	if ( trimDisabled ) {
		options.trimEnabled = false;
	}
	
	if ( scaleManifestEnabled ) {
		options.scaleManifestValues = true;
	}
	
	if ( scaleManifestDisabled ) {
		options.scaleManifestValues = false;
	}
	
	// if ( powerTwo ) {
		// options.finalImageIsPow2 = true;
	// }

	if ( inputFiles ) {
		for (const auto& file: args::get( inputFiles ) ) {
			ProcessInputFileEntry( recursive ? -1 : 1, file, options.inputFiles );
		}
	}
	
	if ( padding ) {
		options.padding = args::get( padding );
	}
	
	if ( boundaryAlignment ) {
		options.boundaryAlignment = args::get( boundaryAlignment );
	}
	
	if ( trimBoundaryAlignment ) {
		options.trimBoundary = args::get( trimBoundaryAlignment );
	}
	
	if ( resolution ) {
		options.resolution = args::get( resolution );
	}
	
	if ( noOutputImage ) {
		options.outputImage = false;
	}
	
	if ( noOutputJson ) {
		options.outputJson = false;
	}

	if ( resizeKernel ) {
		options.resizeKernel = args::get( resizeKernel );
		// TODO: make sure it's a valid value
	}
	
	if ( manifestFormat ) {
		options.manifestFormat = args::get( manifestFormat );
		// TODO: make sure valid value
	}
	
//	if ( imageFormat ) {
//		options.imageFormat = args::get( imageFormat );
//		// TODO: make sure valid value
//	}
	
	if ( options.inputFiles.empty() ) {
		std::cerr << "No input files specified on command line - Use --help option to display possible options" << std::endl;
		return Options();
	}

	options.valid = true;
	return options;
}

