#include "pch.h"
#include "TraceUtils.h"
#include <windows.h>
namespace tools {
	std::streamsize DebugStreamBuf::xsputn(const wchar_t * s, std::streamsize n)
	{
#ifdef _DEBUG
		std::wstring wstr(s, static_cast<uint32_t>(n));
		OutputDebugString(wstr.c_str());
#endif
		return n;
	}

	wint_t DebugStreamBuf::overflow(wint_t c)
	{
		if (c == traits_type::eof()) {
			return c;
		}
		else {
			wchar_t wc = c;
			xsputn(&wc, 1);
			return c;
		}
	}

	std::streamsize TraceStreamBuf::xsputn(const wchar_t * s, std::streamsize n)
	{
		std::wstring wstr(s, static_cast<uint32_t>(n));
		OutputDebugString(wstr.c_str());
		return n;
	}

	wint_t TraceStreamBuf::overflow(wint_t c)
	{
		if (c == traits_type::eof()) {
			return c;
		}
		else {
			wchar_t wc = c;
			xsputn(&wc, 1);
			return c;
		}
	}


	DebugStream debug_stream;
	TraceStream trace_stream;
}