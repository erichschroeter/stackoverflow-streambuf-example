#include <iostream>
#include <Num_Virtual_File.h>

int main( int argc, char **argv )
{
	Num_Virtual_File vfile( 1, 2 );
	std::istream     stream( &vfile );
	auto             file_byte_total = vfile.in_avail();
	std::cout << "File size: " << file_byte_total << std::endl;
	
	// Example usage #2
	// generate known limited amount
	char buf[32] = {0};
	vfile.xsgetn( buf, sizeof( buf ));
	std::cout << std::string( buf ) << std::endl;

	return 0;
}
