
#include "main.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/locale/encoding_utf.hpp>
#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
std::string GetLastErrorAsString() {
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL );
	std::string message(messageBuffer, size);
	LocalFree(messageBuffer);
	return message;
}
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
		// Note: Always use UNC under windows
	//	if ( dest.length() > 250 ) {
			if ( tooLong ) {
				*tooLong = true;
			}
			return "\\\\?\\" + dest;
	//	}
	} else {
		delete[] output;
	}
#endif
	return filename;
}

unsigned char* LoadFileBuffer( const std::string& filename, int& length ) {
#ifdef WIN32
	// TODO: needs more error checking
	auto file = CreateFileA( filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	LARGE_INTEGER size;
	GetFileSizeEx(file, &size);
	length = size.QuadPart;
	unsigned char* buffer = new unsigned char[length+1];
	int bytesRead = 0;
	ReadFile( file, buffer, length, reinterpret_cast<LPDWORD>(&bytesRead), NULL );
	CloseHandle( file );
	return buffer;
#else
	FILE* file = fopen( filename.c_str(), "rb" );
	if ( !file ) {
		throw std::runtime_error( "Could not load file" );
	}
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );
	unsigned char* buffer = new unsigned char[length+1];
	const size_t actualRead = fread( buffer, length, 1, file );
	fclose( file );
	buffer[ actualRead+1 ] = 0;
	return buffer;
#endif
}

void SaveFileBuffer( const std::string& filename, const unsigned char* buffer, int length ) {
#ifdef WIN32
	auto file = CreateFileA( filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	int bytesWritten = 0;
	WriteFile( file, buffer, length, reinterpret_cast<LPDWORD>(&bytesWritten), NULL );
	CloseHandle( file );
#else
	FILE* file = fopen( filename.c_str(), "wb" );
	if ( !file ) {
		throw std::runtime_error( "Could not create file" );
	}
	fwrite( buffer, length, 1, file );
	fclose( file );
#endif
}
