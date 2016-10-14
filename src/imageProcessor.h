#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <deque>
#include "InputImage.h"
#include "OutputImage.h"
#include "Options.h"

std::deque<OutputImage*> process( std::deque<InputImage*>& inputImageList, const Options& options );

#endif

