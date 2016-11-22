
#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>


struct Options {
	Options() {
		valid = false;
		outputName = "default";
		maxOutputWidth = maxOutputHeight = 8192;
		trimEnabled = true;
		rotationEnabled = true;
		finalImageIsPow2 = false;
		failOnTooBig = true;
		padding = 6;
		boundaryAlignment = 16;
		resolution = 1.0f;
		scaleManifestValues = true;
		trimBoundary = 4;
	}

	bool valid;
	std::vector<std::string> inputFiles;
	float resolution;
	std::string outputName;
	int maxOutputWidth, maxOutputHeight;
	bool rotationEnabled;
	bool trimEnabled;
	bool finalImageIsPow2;
	bool failOnTooBig;
	int padding;
	int boundaryAlignment;
	bool scaleManifestValues;
	int trimBoundary;
};

Options ParseArgv(int argc, const char** argv);

#endif
