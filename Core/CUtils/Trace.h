#pragma once
#include <cstdlib>
#include <vector>
#include <sstream>

#include <windows.h>

template<typename TChar, typename TTraits = std::char_traits<TChar> >
class TraceStream : public std::basic_stringbuf<TChar,TTraits> 
{
public:
	explicit TraceStream() : _buffer(256) 
	{
		setg(nullptr, nullptr, nullptr);
		setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());
	}

	~TraceStream() 
	{
	}

	static_assert(std::is_same<TChar,char>::value || std::is_same<TChar,wchar_t>::value, "TraceStream only supports char and wchar_t types");

	int sync()
	{
		try
		{
			std::string s(pbase(), pptr());
			OutputDebugStringA(s.c_str());

			setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());
			return 0;
		} 
		catch(...) 
		{
			return -1;
		}
	}

	int_type overflow(int_type c = TTraits::eof())
	{
		auto syncRet = sync();
		if (c != TTraits::eof())
		{
			_buffer[0] = (TChar) c;
			setp(_buffer.data(), _buffer.data() + 1, _buffer.data() + _buffer.size());
		}
		return syncRet == -1 ? TTraits::eof() : 0;
	}
	
	static void sHookStream(std::basic_ostream<TChar, TTraits>& ioStream)
	{
		// static so not properly thread-safe
		static TraceStream sTraceStream;
		ioStream.rdbuf(&sTraceStream);
		ioStream << "Hooked TraceStream" << std::endl;
	}

private:
	std::vector<TChar>		_buffer;
};