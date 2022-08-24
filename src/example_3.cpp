#include <iostream>
#include <Num_Virtual_File.h>

int main( int argc, char **argv )
{
	int from = 0;
	int to = 1;

	if ( argc > 1 )
	{
		from = std::atoi( argv[ 1 ] );
		if ( argc > 2 )
		{
			to = std::atoi( argv[ 2 ] );
		}
	}

	Num_Virtual_File vfile( from, to );
	std::istream     stream( &vfile );
	auto             file_byte_total = vfile.in_avail();
	std::cout << "File size: " << file_byte_total << std::endl;
	
	// Example usage #3
	// split into multiple partitions due to memory-constrainted device.
	int partition_count = 0;
	
	while ( vfile.showmanyc() > 0 )
	{
		char buf[3] = {0};
		vfile.xsgetn( buf, sizeof( buf ) - 1); // -1 for null terminator
		std::cout << "Partition " << ++partition_count << ": '" << std::string(buf) << "'" << std::endl;
	}

	return 0;
}
