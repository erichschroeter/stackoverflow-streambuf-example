#include <iostream>
#include <sstream>
#include <Num_Virtual_File.h>

int main( int argc, char **argv )
{
	Num_Virtual_File vfile( 1, 2 );
	std::istream     stream( &vfile );
	auto             file_byte_total = vfile.in_avail();
	std::cout << "File size: " << file_byte_total << std::endl;
	
	// Example usage #1
	// generate entire file
	std::stringstream ss;
	ss << stream.rdbuf();
	std::cout << ss.str() << std::endl;

	return 0;
}
