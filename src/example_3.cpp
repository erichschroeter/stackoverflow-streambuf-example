#include <iostream>
#include <Num_Virtual_File.h>

int main( int argc, char **argv )
{
	Num_Virtual_File vfile( 1, 2 );
	std::istream     stream( &vfile );
	auto             file_byte_total = vfile.in_avail();
	std::cout << "File size: " << file_byte_total << std::endl;
	
	// Example usage #3
	// split into multiple partitions due to memory-constrainted device.
	int partition_count = 0;
	
	while ( vfile.showmanyc() > 0 )
	{
		char buf[3] = {0};
		vfile.xsgetn( buf, sizeof( buf - 1));
		std::cout << "Partition " << ++partition_count << ": '" << std::string(buf) << "'" << std::endl;
	}

	return 0;
}
