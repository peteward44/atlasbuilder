
#include "atlasbuilder.h"
#include "main.h"
#include "imageProcessor.h"
#include "ManifestWriter.h"
#include "ImageData.h"
#include "PathUtil.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "boost/filesystem.hpp"


void create( const Options& options, std::deque<InputImage*>& inputImages ) {
	for ( InputImage* input : inputImages ) {
		// Applys any trimming, boundary alignment & resolution scaling required
		input->Prep();
	}

	std::cout << "Calculating..." << std::endl;
	auto output = process( inputImages, options );
	std::cout << "Calculation complete " << std::endl;
	if ( options.outputJson || options.outputImage ) {
		try {
			const boost::filesystem::path p( options.outputName );
			const boost::filesystem::path dir = p.parent_path();
			boost::filesystem::create_directories( dir );
		}
		catch ( std::exception& ) {}
	}
	
	const std::string filename = options.outputName;
	const std::string jsonFilename = filename + ".json";
	
	if ( options.outputImage ) {
		std::ostringstream resname;
		std::cout << "Writing output image " << filename << std::endl;
		output->Finalise( filename + ".png" );
	}
	
	if ( options.outputJson ) {
		std::cout << "Writing manifest " << jsonFilename << std::endl;
		// writing manifest directly to a std::ofstream caused crash in -O3 builds (dont know why) so pipe to std::ostringstream and then output using C methods
		std::ostringstream manifestOutput;
		WriteManifest( output, manifestOutput, filename + ".png" );
		FILE* jsonFile = fopen( ConvertFilename( jsonFilename ).c_str(), "wt" );
		const std::string manifest = manifestOutput.str();
		fwrite( manifest.c_str(), manifest.size(), 1, jsonFile );
		fclose( jsonFile );
	}
}


int atlasbuilder( int argc, const char** argv )
{
	int retCode = 0;
	try {
		Options options = ParseArgv( argc, argv );
		
		ImageData::initialise( argc, argv );

		if ( !options.valid ) {
			return 1;
		}
		
		std::deque<InputImage*> inputImages;
		for ( const std::string filename : options.inputFiles ) {
			inputImages.push_back( new InputImage( options, filename ) ); // TODO: mem leak
		}

		create( options, inputImages );
	}
	catch ( std::exception& e ) {
		std::cerr << "Error " << e.what() << std::endl;
		retCode = 1;
	}
	catch ( ... ) {
		std::cerr << "Unknown error" << std::endl;
		retCode = 1;
	}

	try {
		ImageData::shutdown();
	}
	catch ( ... ) {}
	
    return retCode;
}

