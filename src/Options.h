
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
		failOnTooBig = false;
		padding = 2;
		boundaryAlignment = 0;
		resolution = 1.0f;
		scaleManifestValues = true;
		trimBoundary = 0;
		outputImage = true;
		outputJson = true;
		resizeKernel = "linear";
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
	bool outputImage;
	bool outputJson;
	std::string resizeKernel;
};

Options ParseArgv(int argc, const char** argv);

#endif
