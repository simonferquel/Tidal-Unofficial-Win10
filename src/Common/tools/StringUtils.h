#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <iterator>
namespace tools {
	namespace strings {
		// check if the given string ends with the given character
		// str: string to test
		// value: value to find
		inline bool endsWith(const std::wstring& str, const wchar_t value) {
			if (str.size() == 0) {
				return false;
			}
			return value == str.at(str.size() - 1);
		}

		// appends the given character to the given string if it does not yet end with the same value
		// str: string to append to
		// value: char to append
		inline void appendIfNotEndsWith(std::wstring& str, const wchar_t value) {
			if (!endsWith(str, value)) {
				str.push_back(value);
			}
		}

		inline bool startsWith(Platform::String^ value, Platform::String^ toFind) {
			if (value->Length() < toFind->Length()) {
				return false;
			}
			return (std::equal(value->Begin(), value->Begin() + toFind->Length(), toFind->Begin(), toFind->End()));
		}

		inline std::string convertToUtf8String(const std::wstring& utf16String) {
			auto nbChars = WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], static_cast<int>(utf16String.size()), nullptr, 0, nullptr, nullptr);
			std::string result(nbChars, ' ');
			WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], static_cast<int>(utf16String.size()), &result[0], nbChars, nullptr, nullptr);
			return result;
		}

		inline std::string convertToUtf8String(Platform::String^  utf16String) {
			auto nbChars = WideCharToMultiByte(CP_UTF8, 0, const_cast<wchar_t*>(utf16String->Data()), static_cast<int>(utf16String->Length()), nullptr, 0, nullptr, nullptr);
			std::string result(nbChars, ' ');
			WideCharToMultiByte(CP_UTF8, 0, const_cast<wchar_t*>(utf16String->Data()), static_cast<int>(utf16String->Length()), &result[0], nbChars, nullptr, nullptr);
			return result;
		}

		inline std::wstring convertToWideString(const std::string& src) {
			auto nbChars = MultiByteToWideChar(CP_UTF8, 0, &src[0], static_cast<int>(src.size()), nullptr, 0);
			std::wstring result(nbChars, L' ');
			MultiByteToWideChar(CP_UTF8, 0, &src[0], static_cast<int>(src.size()), &result[0], nbChars);
			return result;
		}

		struct WindowsStreamBuffer : public std::wstreambuf {
			Platform::String^ innerStr;
			WindowsStreamBuffer(Platform::String^ str) : innerStr(str) {
				this->setg(const_cast<wchar_t*>(str->Begin()), const_cast<wchar_t*>(str->Begin()), const_cast<wchar_t*>(str->End()));

			}
		};

		class WindowsWIStream : public std::wistream {
		private:
			WindowsStreamBuffer _buf;
		public:
			WindowsWIStream(Platform::String^ str) : _buf(str), std::wistream(&_buf) {

			}
		};


		inline Platform::String^ toWindowsString(const std::wstring& src) {
			return ref new Platform::String(src.c_str(), static_cast<std::uint32_t>(src.size()));
		}

		inline std::vector<Platform::String^> toWindowsStrings(const std::vector<std::wstring>& src) {
			std::vector<Platform::String^> result;
			result.reserve(src.size());
			std::transform(src.cbegin(), src.cend(), std::back_inserter(result), [](const std::wstring& val) {return toWindowsString(val); });
			return result;
		}

		inline std::wstring toStdString(Platform::String^ str) {
			return std::wstring(str->Begin(), str->End());
		}
		inline std::vector<std::wstring> split(const std::wstring& src, wchar_t separator) {
			auto it = src.begin();
			std::vector<std::wstring> result;
			while (it != src.end()) {
				auto newIt = std::find(it, src.end(), separator);
				if (newIt == src.end()) {
					result.emplace_back(it, newIt);
					break;
				}
				else {
					result.emplace_back(it, newIt);
					it = ++newIt;
				}
			}

			return result;
		}
	}

}