#pragma once
#include <streambuf>
#include <ostream>
namespace tools {

	class DebugStreamBuf : public std::wstreambuf {
	public:
		virtual std::streamsize xsputn(const wchar_t* s, std::streamsize n) override;
	protected:
		virtual wint_t overflow(wint_t c = traits_type::eof()) override;
	};

	class TraceStreamBuf : public std::wstreambuf {
	public:
		virtual std::streamsize xsputn(const wchar_t* s, std::streamsize n) override;
	protected:
		virtual wint_t overflow(wint_t c = traits_type::eof()) override;
	};

	class DebugStream : public std::wostream {
	private:
		DebugStreamBuf buf;
	public:
		DebugStream() : std::wostream(&buf, false)
		{}
	};

	class TraceStream : public std::wostream {
	private:
		TraceStreamBuf buf;
	public:
		TraceStream() : std::wostream(&buf, false)
		{}

	};

	extern DebugStream debug_stream;
	extern TraceStream trace_stream;

	inline std::wostream& operator  << (std::wostream& s, Platform::String^ str) {
		if (str) {
			return s << str->Data();
		}
		else {
			return s << L"{null}";
		}
	}
#ifndef _DEBUG
	template <typename T>
	inline DebugStream& operator  << (DebugStream& s, T&&) {
		return s;
	}
#endif

}