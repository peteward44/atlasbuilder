
#include "main.h"
#include "imageProcessor.h"
#include "ManifestWriter.h"
#include "ImageData.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "boost/filesystem.hpp"


void create( const Options& options, std::deque<InputImage*>& inputImages, float resolution ) {
	for ( InputImage* input : inputImages ) {
		// trim all input images if enabled
		if ( options.trimEnabled ) {
			input->Trim();
		}
		if (options.boundaryAlignment > 0) {
			input->AlignBoundary();
		}
		// adjust resolution on all inputs
		input->SelectResolution( resolution );
	}

	std::cout << "Calculating..." << std::endl;
	auto outputs = process( inputImages, options );
	int index = 0;
	std::cout << "Calculation complete " << std::endl;
	for ( OutputImage* output : outputs ) {
		try {
			const boost::filesystem::path p( options.outputName );
			const boost::filesystem::path dir = p.parent_path();
			boost::filesystem::create_directories( dir );
		}
		catch ( std::exception& ) {}
		
		std::string filename = options.outputName;
		if (index > 0) {
			filename += "_" + index;
		}
		std::ostringstream resname;
		resname << std::setprecision(2) << resolution;
		filename += "@" + resname.str() + "x";
		std::cout << "Writing output image " << filename << std::endl;
		output->Finalise( filename + ".png" );
		
		const std::string jsonFilename = filename + ".json";
		std::cout << "Writing manifest " << jsonFilename << std::endl;
		// writing manifest directly to a std::ofstream caused crash in -O3 builds (dont know why) so pipe to std::ostringstream and then output using C methods
		std::ostringstream manifestOutput;
		WriteManifest( output, manifestOutput, filename + ".png" );
		FILE* jsonFile = fopen( jsonFilename.c_str(), "wt" );
		const std::string manifest = manifestOutput.str();
		fwrite( manifest.c_str(), manifest.size(), 1, jsonFile );
		fclose( jsonFile );
		
		index++;
	}
}


int main( int argc, char** argv )
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

		for ( const float res : options.resolutions ) {
			create( options, inputImages, res );
		}
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

