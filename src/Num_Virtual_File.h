#include <array>
#include <sstream>

/// Thanks to https://stackoverflow.com/a/7185723/561624
class range
{
public:
	class iterator
	{
		friend class range;
	public:
		long int operator *() const
		{
			return i_;
		}

		const iterator &operator ++()
		{
			++i_; return *this;
		}

		iterator operator ++( int )
		{
			iterator copy( *this ); ++i_; return copy;
		}

		bool operator ==( const iterator &other ) const
		{
			return i_ == other.i_;
		}

		bool operator !=( const iterator &other ) const
		{
			return i_ != other.i_;
		}

	protected:
		iterator( long int start ) : i_ ( start )
		{
		}

	private:
		unsigned long i_;
	};

	iterator begin() const
	{
		return begin_;
	}

	iterator end() const
	{
		return end_;
	}

	range( long int begin, long int end ) : begin_( begin ), end_( end )
	{
	}

private:
	iterator begin_;
	iterator end_;
};

class Number_JSON_Serializer
{
public:
	Number_JSON_Serializer( int value ) :
		m_number( value )
	{
	}

	std::string Serialize() const
	{
		std::stringstream ss;
		ss << "\"" << m_number << "\":" << m_number;
		return ss.str();
	}

	int m_number;
};


/// References used:
/// - https://gist.github.com/addam/9a80975e293b66555ae6026edddd5bb9
/// - https://blog.csdn.net/tangyin025/article/details/50487544
class Num_Virtual_File :
	public std::streambuf
{
public:
	/// Number of bytes reserved for stream putback.
	static constexpr size_t PUTBACKCOUNT = 1;
	/// Number of bytes for the stream buffer.
	static constexpr size_t BUFSIZE = 3;
	static_assert( BUFSIZE > ( PUTBACKCOUNT + 1 ), "Buffer needs enough space for putback count and buffer terminator in addition to the data");

	explicit Num_Virtual_File( int begin, int end ) :
		m_is_prelude_written( false ),
		m_is_epilogue_written( false ),
		m_insert_comma( false ),
		m_begin( begin ), m_end( end + 1 ),
		m_current_ptr( &m_buf.front() ),
		m_range( m_begin, m_end ), m_range_iter( m_range.begin() ),
		m_file_size( 0 ),
		m_bytes_generated( 0 ),
		m_putback( std::max( PUTBACKCOUNT, size_t ( 1 )))
	{
		m_file_size = CalcFileSizeTotal( m_begin, m_end );

		char * endptr = &m_buf.front() + m_buf.size();
		setg( endptr, endptr, endptr );
	}

	// Copying not allowed
	Num_Virtual_File( const Num_Virtual_File & ) = delete;
	Num_Virtual_File & operator=( const Num_Virtual_File & ) = delete;

	std::streamsize xsgetn( char * s, std::streamsize n )
	{
		for ( std::streamsize i = 0; i != n && i < m_file_size; ++i )
		{
			if ( gptr() == egptr() && reload() == traits_type::eof() )
			{
				return i;
			}
			*s++ = *gptr();
			if ( gptr() != egptr() )
			{
				gbump( 1 );
			}
		}
		return n;
	}

	std::streambuf::int_type underflow()
	{
		if ( gptr() == egptr() && reload() == traits_type::eof() )
		{
			return traits_type::eof();
		}
		return traits_type::to_int_type( *gptr() );
	}

	std::streambuf::int_type uflow()
	{
		if ( gptr() == egptr() && reload() == traits_type::eof() )
		{
			return traits_type::eof();
		}

		return traits_type::to_int_type( *m_current_ptr++ );
	}

	std::streambuf::int_type pbackfail( std::streambuf::int_type ch )
	{
		if ( gptr() == egptr() || ( ch != traits_type::eof() && ch != m_current_ptr[-1] ) )
		{
			return traits_type::eof();
		}

		return traits_type::to_int_type( *--m_current_ptr );
	}

	std::streamsize showmanyc()
	{
		return m_file_size - m_bytes_generated;
	}

protected:
	size_t CalcFileSizeTotal( int begin, int end )
	{
		size_t byte_count = 0;
		byte_count++; // "["
		Number_JSON_Serializer ser( m_begin );
		bool insert_comma = false;
		for ( int i = m_begin; i < m_end; ++i )
		{
			if ( insert_comma )
			{
				byte_count++; // ","
			}
			ser.m_number = i;
			std::string tmp = ser.Serialize();
			byte_count += tmp.length();
			insert_comma = true;
		}
		byte_count++; // "]"
		return byte_count;
	}

	std::streambuf::int_type reload()
	{
		if ( gptr() < egptr() ) // buffer not exhausted
		{
			return traits_type::to_int_type( *gptr() );
		}

		if ( m_is_epilogue_written )
		{
			return traits_type::eof();
		}

		char * base = &m_buf.front();
		char * end = &m_buf.back();
		char * start = base;

		if ( eback() == base ) // true when this isn't the first fill
		{
			// make arrangements for putback chars
			std::memmove( base, egptr() - m_putback, m_putback );
			start += m_putback;
		}

		char * bufptr = start;

		// start is now the start of the buffer
		// generate data into buffer

		if ( bufptr < end && !m_is_prelude_written )
		{
			*bufptr = '[';
			bufptr++;
			m_is_prelude_written = true;
		}

		// while ( bufptr < end && m_range_iter != m_range.end() )
		while ( bufptr < end )
		{
			if ( m_serialized_str.empty() || m_serialized_str_iter == m_serialized_str.end() )
			{
				if ( m_range_iter != m_range.end() )
				{
					m_insert_comma = m_range_iter != m_range.begin();
					m_serialized_str = Number_JSON_Serializer( *m_range_iter ).Serialize();
					m_serialized_str_iter = m_serialized_str.begin();
					m_range_iter++;
				}
			}

			if ( bufptr < end && m_insert_comma )
			{
				*bufptr = ',';
				bufptr++;
				m_insert_comma = false;
			}

			// Copy the serialized str to the buffer, but don't overfill the buffer
			while ( bufptr < end && m_serialized_str_iter != m_serialized_str.end() )
			{
				*bufptr = *m_serialized_str_iter;
				bufptr++;
				m_serialized_str_iter++;
			}

			if ( m_serialized_str_iter == m_serialized_str.end() && m_range_iter == m_range.end() )
			{
				break;
			}
		}

		if ( bufptr < end && !m_is_epilogue_written )
		{
			*bufptr = ']';
			bufptr++;
			m_is_epilogue_written = true;
		}

		size_t n = bufptr - start;
		m_current_ptr = bufptr;
		m_bytes_generated += n;
		setg( base, start, start + n );
		return traits_type::to_int_type( *m_current_ptr );
	}

private:
	std::array< char, BUFSIZE > m_buf;
	/// Any constant JSON before iterated data.
	bool m_is_prelude_written;
	/// Any constant JSON after iterated data.
	bool m_is_epilogue_written;
	bool m_insert_comma;
	int m_begin;
	int m_end;
	char * m_current_ptr;
	range m_range;
	range::iterator m_range_iter;
	std::string m_serialized_str;
	std::string::iterator m_serialized_str_iter;
	size_t m_file_size;
	size_t m_bytes_generated;
	const size_t m_putback;
};
