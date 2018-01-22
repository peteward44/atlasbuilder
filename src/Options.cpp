#include "Options.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
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
				std::cout << "line " << responsePointers[ responsePointers.size()-1 ] << std::endl;
			}
			argv = &responsePointers[0];
			argc = responsePointers.size();
		}
	}
	
	args::ArgumentParser parser( "Atlasbuilder ", VERSION );
	
	args::HelpFlag help( parser, "help", "Display this help menu", { 'h', "help" } );
	args::ValueFlag<std::string> output( parser, "output", "set output name", {"output"});

	args::Flag version( parser, "version", "Output version number", {'v', "version"});
	
	args::PositionalList<std::string> inputFiles(parser, "input-files", "Input files to process");

	args::ValueFlag<int> outputWidth( parser, "output-width", "set maximum output image width", { "output-width" } );
	args::ValueFlag<int> outputHeight( parser, "output-height", "set maximum output image height", { "output-height" } );

	args::Group xorgroup( parser, "Enable or disable rotation:", args::Group::Validators::AtMostOne );
	args::Flag rotationEnabled( xorgroup, "rotation-enabled", "Enable sub image rotation", {"rotation-enabled"});
	args::Flag rotationDisabled( xorgroup, "rotation-disabled", "Disable sub image rotation", {"rotation-disabled"});
	
	args::Group trimGroup( parser, "Enable or disable trim:", args::Group::Validators::AtMostOne );
	args::Flag trimEnabled( trimGroup, "trim-enabled", "Enable sub image trimming", {"trim-enabled"});
	args::Flag trimDisabled( trimGroup, "trim-disabled", "Disable sub image trimming", {"trim-disabled"});

	args::Flag failIfTooBig( parser, "fail-if-too-big", "fail build if the output-width and output-height will be exceeded", {"fail-if-too-big"});	
	args::Flag powerTwo( parser, "output-pow2", "final output image should always be a power of 2", {"output-pow2"});
	args::Flag scaleManifestValues( parser, "scale-manifest-values", "if using a resolution other than 1, use scaled x,y,w,h values in the manifest", {"scale-manifest-values"});
	
	args::ValueFlag<int> padding( parser, "padding", "padding to insert between each sub image in atlas", { "padding" } );
	args::ValueFlag<int> boundaryAlignment( parser, "boundary-alignment", "boundary-alignment to align each sub image in atlas", { "boundary-alignment" } );
	args::ValueFlag<int> trimBoundaryAlignment( parser, "trim-boundary-alignment", "boundary-alignment to align when trimming", { "trim-boundary-alignment" } );
	
	args::ValueFlag<float> resolution( parser, "resolution", "resolution to output (default is 1.0)", { "resolution" } );
	args::ValueFlag<std::string> outputImage( parser, "output-image", "resolution to output (default is 1.0)", { "output-image" } );
	args::ValueFlag<std::string> manifestFormat( parser, "manifest-format", "format to output the JSON manifest, either 'hash', 'array' or 'legacy'", { "manifest-format" } );
	
	args::Flag noOutputImage( parser, "no-output-image", "Do not output the final image", {"no-output-image"});	
	args::Flag noOutputJson( parser, "no-output-json", "Do not output the final json manifest", {"no-output-json"});
	
	args::ValueFlag<std::string> resizeKernel( parser, "resize-kernel", "Algorithm to use when resizing images. Either 'nearest', 'linear', 'cubic', 'lanczos2' or 'lanczos3'", {"resize-kernel"});
	
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
	catch (args::ParseError e)
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
		std::cout << "options.outputName " << options.outputName << std::endl;
	}

	if ( outputWidth ) {
		options.maxOutputWidth = args::get( outputWidth );
		std::cout << "options.maxOutputWidth " << options.maxOutputWidth << std::endl;
	}

	if ( outputHeight ) {
		options.maxOutputHeight = args::get( outputHeight );
		std::cout << "options.maxOutputHeight " << options.maxOutputHeight << std::endl;
	}

	if ( failIfTooBig ) {
		options.failOnTooBig = true;
	}

	if ( rotationEnabled ) {
		options.rotationEnabled = true;
		std::cout << "options.rotationEnabled " << options.rotationEnabled << std::endl;
	}

	if ( rotationDisabled ) {
		options.rotationEnabled = false;
		std::cout << "options.rotationEnabled " << options.rotationEnabled << std::endl;
	}

	if ( trimEnabled ) {
		options.trimEnabled = true;
		std::cout << "options.trimEnabled " << options.trimEnabled << std::endl;
	}

	if ( trimDisabled ) {
		options.trimEnabled = false;
		std::cout << "options.trimEnabled " << options.trimEnabled << std::endl;
	}
	
	if ( powerTwo ) {
		options.finalImageIsPow2 = true;
	}

	if ( inputFiles ) {
		for (const auto file: args::get( inputFiles ) ) {
			std::cout << "Input file " << file << std::endl;
			// TODO: check file exists
			options.inputFiles.push_back( file );
		}
	}
	
	if ( padding ) {
		options.padding = args::get( padding );
		std::cout << "options.padding " << options.padding << std::endl;
	}
	
	if ( boundaryAlignment ) {
		options.boundaryAlignment = args::get( boundaryAlignment );
		std::cout << "options.boundaryAlignment " << options.boundaryAlignment << std::endl;
	}
	
	if ( scaleManifestValues ) {
		options.scaleManifestValues = true;
		std::cout << "options.scaleManifestValues " << options.scaleManifestValues << std::endl;
	}
	
	if ( trimBoundaryAlignment ) {
		options.trimBoundary = args::get( trimBoundaryAlignment );
		std::cout << "options.trimBoundary " << options.trimBoundary << std::endl;
	}
	
	if ( resolution ) {
		options.resolution = args::get( resolution );
		std::cout << "options.resolution " << options.resolution << std::endl;
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
		std::cout << "options.resizeKernel " << options.resizeKernel << std::endl;
	}
	
	if ( manifestFormat ) {
		options.manifestFormat = args::get( manifestFormat );
		// TODO: make sure valid value
		std::cout << "options.manifestFormat " << options.manifestFormat << std::endl;
	}
	
	if ( options.inputFiles.empty() ) {
		std::cerr << "No input files specified on command line" << std::endl;
		return Options();
	}

	options.valid = true;
	return options;
}

