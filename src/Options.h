
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
		rotateAntiClockwise = false;
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
		manifestFormat = "hash";
	}

	bool valid;
	std::vector<std::string> inputFiles;
	float resolution;
	std::string outputName;
	int maxOutputWidth, maxOutputHeight;
	bool rotationEnabled;
	bool rotateAntiClockwise;
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
	std::string manifestFormat;
};

Options ParseArgv(int argc, char** argv);

#endif
