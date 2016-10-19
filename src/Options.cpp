#include "Options.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "boost/tokenizer.hpp"
#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
std::pair<std::string, std::string> at_option_parser( const std::string& s ) {
	if ('@' == s[0])
		return std::make_pair( std::string("response-file"), s.substr(1) );
	else
		return std::pair<std::string, std::string>();
}

Options ParseArgv(int argc, char** argv) {
	boost::program_options::positional_options_description inputFileDescription;
	inputFileDescription.add("input-files", -1);
	
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("response-file", boost::program_options::value<std::string>(), "read in command line options from file (separated by newlines). Can be specified with '@name', too")
		("input-files", boost::program_options::value< std::vector<std::string> >(), "input file(s)")
		("resolution", boost::program_options::value< std::vector<float> >(), "resolutions to output")
		("output", boost::program_options::value<std::string>(), "set output name")
		("maxOutputWidth", boost::program_options::value<int>(), "set maximum output image width")
		("maxOutputHeight", boost::program_options::value<int>(), "set maximum output image height")
		("failOnTooBig", boost::program_options::value<bool>(), "fail build if the maxOutputWidth and maxOutputHeight will be exceeded")
		("rotation-enabled", boost::program_options::value<bool>(), "enable rotation on subimages")
		("trim-enabled", boost::program_options::value<bool>(), "enable trim on subimages")
		("output-pow2", boost::program_options::value<bool>(), "final output image should always be a power of 2")
		("padding", boost::program_options::value<int>(), "padding to insert between each sub image in atlas")
		("boundary-alignment", boost::program_options::value<int>(), "boundary-alignment to align each sub image in atlas")
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
		std::cout << desc << std::endl;
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

	if ( vm.count("maxOutputWidth") ) {
		options.maxOutputWidth = vm["maxOutputWidth"].as<int>();
		std::cout << "options.maxOutputWidth " << options.maxOutputWidth << std::endl;
	}

	if ( vm.count("maxOutputHeight") ) {
		options.maxOutputHeight = vm["maxOutputHeight"].as<int>();
		std::cout << "options.maxOutputHeight " << options.maxOutputHeight << std::endl;
	}

	if ( vm.count("failOnTooBig") ) {
		options.failOnTooBig = vm["failOnTooBig"].as<bool>();
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
		options.resolutions = vm["resolution"].as< std::vector<float> >();
		for( float res : options.resolutions ){
			std::cout << "Outputting resolution " << res << std::endl;
		}
	}
	
	if ( options.inputFiles.empty() ) {
		std::cerr << "No input files specified on command line" << std::endl;
		return Options();
	}
	
	options.valid = true;
	return options;
}

