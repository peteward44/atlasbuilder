
#ifndef MANIFEST_WRITER_H
#define MANIFEST_WRITER_H

#include <string>

class OutputImage;

void WriteManifest( const OutputImage* output, std::ostream& ostr, const std::string& imageFilename );

#endif
