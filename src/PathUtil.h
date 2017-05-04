
#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include <string>

std::string ConvertFilename( const std::string& filename, bool* tooLong = NULL );
unsigned char* LoadFileBuffer( const std::string& filename, int& length );
void SaveFileBuffer( const std::string& filename, const unsigned char* buffer, int length );

#endif
