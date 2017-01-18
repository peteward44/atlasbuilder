#include "Options.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "boost/tokenizer.hpp"
#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"

const std::string VERSION = "1.0.2";

void PrintHelp( const boost::program_options::options_description& desc ) {
	std::cout << "usage: atlasbuilder [options] <input files...>" << std::endl;
 	std::cout << "example: atlasbuilder --output=myoutputname input1.png input2.png" << std::endl;
	std::cout << std::endl;
	std::cout << desc << std::endl;
}

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
std::pair<std::string, std::string> at_option_parser( const std::string& s ) {
	if ('@' == s[0])
		return std::make_pair( std::string("response-file"), s.substr(1) );
	else
		return std::pair<std::string, std::string>();
}

Options ParseArgv(int argc, const char** argv) {
	boost::program_options::positional_options_description inputFileDescription;
	inputFileDescription.add("input-files", -1);
	
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("version", "display version")
		("response-file", boost::program_options::value<std::string>(), "read in command line options from file (separated by newlines). Can be specified with '@name', too")
		("input-files", boost::program_options::value< std::vector<std::string> >(), "input file(s)")
		("resolution", boost::program_options::value<float>(), "resolution to output (default is 1.0)")
		("output", boost::program_options::value<std::string>(), "set output name")
		("output-width", boost::program_options::value<int>(), "set maximum output image width")
		("output-height", boost::program_options::value<int>(), "set maximum output image height")
		("fail-if-too-big", boost::program_options::value<bool>(), "fail build if the output-width and output-height will be exceeded")
		("rotation-enabled", boost::program_options::value<bool>(), "enable rotation on subimages")
		("trim-enabled", boost::program_options::value<bool>(), "enable trim on subimages")
//		("output-pow2", boost::program_options::value<bool>(), "final output image should always be a power of 2")
		("padding", boost::program_options::value<int>(), "padding to insert between each sub image in atlas")
		("boundary-alignment", boost::program_options::value<int>(), "boundary-alignment to align each sub image in atlas")
		("scale-manifest-values", boost::program_options::value<bool>(), "if using a resolution other than 1, use scaled x,y,w,h values in the manifest")
		("trim-boundary-alignment", boost::program_options::value<int>(), "boundary-alignment to align when trimming")
		("output-image", boost::program_options::value<bool>(), "Set to false to not output the final image")
		("output-json", boost::program_options::value<bool>(), "Set to false to not output the final JSON file")
	;

	boost::program_options::variables_map vm;
	auto parser = boost::program_options::command_line_parser( argc, argv )
		.options( desc )
		.extra_parser( at_option_parser )
		.positional( inputFileDescription )
		.run();
	boost::program_options::store( parser, vm );
	boost::program_options::notify( vm );    

	if ( vm.count("help") ) {
		PrintHelp( desc );
		return Options();
	}

	if ( vm.count("version") ) {
		std::cout << VERSION << std::endl;
		return Options();
	}

	if ( vm.count("response-file") ) {
		// Load the file and tokenize it
		FILE* in = fopen( vm["response-file"].as<std::string>().c_str(), "rt" );
		if ( !in ) {
			throw std::runtime_error( "Could not open the response file" );
		}
		// Read the whole file into a string
		std::stringstream ss;
		char c;
		do {
			c = fgetc( in );
			if ( c != EOF ) {
				ss << c;
			}
		} while (c != EOF);
		fclose( in );
		// Split the file content on new lines
		boost::char_separator<char> sep("\n\r");
		std::string ResponsefileContents( ss.str() );
		boost::tokenizer<boost::char_separator<char> > tok( ResponsefileContents, sep );
		std::vector<std::string> args;
		std::copy( tok.begin(), tok.end(), std::back_inserter( args ) );
		// Parse the file and store the options
		boost::program_options::store( boost::program_options::command_line_parser(args).options( desc ).positional( inputFileDescription ).run(), vm );
	}

	Options options;
	
	if ( vm.count("output") ) {
		options.outputName = vm["output"].as<std::string>();
		std::cout << "options.outputName " << options.outputName << std::endl;
	}

	if ( vm.count("output-width") ) {
		options.maxOutputWidth = vm["output-width"].as<int>();
		std::cout << "options.maxOutputWidth " << options.maxOutputWidth << std::endl;
	}

	if ( vm.count("output-height") ) {
		options.maxOutputHeight = vm["output-height"].as<int>();
		std::cout << "options.maxOutputHeight " << options.maxOutputHeight << std::endl;
	}

	if ( vm.count("fail-if-too-big") ) {
		options.failOnTooBig = vm["fail-if-too-big"].as<bool>();
		std::cout << "options.failOnTooBig " << options.failOnTooBig << std::endl;
	}
	
	if ( vm.count("rotation-enabled") ) {
		options.rotationEnabled = vm["rotation-enabled"].as<bool>();
		std::cout << "options.rotationEnabled " << options.rotationEnabled << std::endl;
	}

	if ( vm.count("trim-enabled") ) {
		options.trimEnabled = vm["trim-enabled"].as<bool>();
		std::cout << "options.trimEnabled " << options.trimEnabled << std::endl;
	}

	if ( vm.count("output-pow2") ) {
		options.finalImageIsPow2 = vm["output-pow2"].as<bool>();
		std::cout << "options.finalImageIsPow2 " << options.finalImageIsPow2 << std::endl;
	}

	if ( vm.count("padding") ) {
		options.padding = vm["padding"].as<int>();
		std::cout << "options.padding " << options.padding << std::endl;
	}

	if ( vm.count("boundary-alignment") ) {
		options.boundaryAlignment = vm["boundary-alignment"].as<int>();
		std::cout << "options.boundaryAlignment " << options.boundaryAlignment << std::endl;
	}
	
	if ( vm.count("scale-manifest-values") ) {
		options.scaleManifestValues = vm["scale-manifest-values"].as<bool>();
		std::cout << "options.scaleManifestValues " << options.scaleManifestValues << std::endl;
	}
	
	if ( vm.count("trim-boundary-alignment") ) {
		options.trimBoundary = vm["trim-boundary-alignment"].as<int>();
		std::cout << "options.trimBoundary " << options.trimBoundary << std::endl;
	}
	
	if ( vm.count( "input-files" ) ) {
		auto files = vm["input-files"].as< std::vector<std::string> >();
		for( std::string file : files ) {
			boost::trim( file );
			if ( file.length() > 0 ) {
				std::cout << "Input file " << file << std::endl;
				// TODO: check file exists
				options.inputFiles.push_back( file );
			}
		}
	}
	
	if ( vm.count( "resolution" ) ) {
		options.resolution = vm["resolution"].as<float>();
		std::cout << "Outputting resolution " << options.resolution << std::endl;
	}

	if ( vm.count( "output-image" ) ) {
		options.outputImage = vm["output-image"].as<bool>();
	}

	if ( vm.count( "output-json" ) ) {
		options.outputJson = vm["output-json"].as<bool>();
	}
	
	if ( options.inputFiles.empty() ) {
		std::cerr << "No input files specified on command line" << std::endl;
		PrintHelp( desc );
		return Options();
	}
	
	options.valid = true;
	return options;
}

