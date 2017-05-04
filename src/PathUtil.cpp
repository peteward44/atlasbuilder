
#include "main.h"
#include <boost/locale/encoding_utf.hpp>
#include <fstream>
#ifdef WIN32
#include <windows.h>
#endif


std::wstring utf8_to_wstring(const std::string& str) {
    return boost::locale::conv::utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str) {
    return boost::locale::conv::utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

std::string ConvertFilename( const std::string& filename, bool* tooLong ) {
	if ( tooLong ) {
		*tooLong = false;
	}
#ifdef WIN32
	// Use GetFullPathNameW to convert possible relative path to absolute path, then prepend with "\\?" to make it UNC compatible which
	// allows us to get around 260 char path limit
	const std::wstring wideFilename = utf8_to_wstring( filename );
	wchar_t* output = new wchar_t[4096];
	if ( GetFullPathNameW( wideFilename.c_str(), 4096, output, NULL ) > 0 ) {
		std::wstring wideOutput = output;
		std::string dest = wstring_to_utf8( wideOutput );   
		delete[] output;
		if ( dest.length() > 250 ) {
			if ( tooLong ) {
				*tooLong = true;
			}
			return "\\\\?\\" + dest;
		}
	} else {
		delete[] output;
	}
#endif
	return filename;
}

unsigned char* LoadFileBuffer( const std::string& filename, int& length ) {
	std::ifstream t;
	t.open( filename.c_str(), std::ios::binary );
	t.seekg(0, std::ios::end);
	length = t.tellg();
	t.seekg(0, std::ios::beg);
	unsigned char* buffer = new unsigned char[length];
	t.read(reinterpret_cast<char*>(buffer), length);
	t.close();
	return buffer;
}

void SaveFileBuffer( const std::string& filename, const unsigned char* buffer, int length ) {
	std::ofstream t;
	t.open( filename.c_str(), std::ios::binary | std::ios::ate );
	t.write( const_cast<char*>( reinterpret_cast<const char*>(buffer) ), length );
	t.close();
}
