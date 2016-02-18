#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <iterator>
namespace LocalDB {
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

	// converts a wchar_t based string to a utf8 char based string
	inline std::string convertToUtf8String(const std::wstring& utf16String) {
		size_t nbChars = WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], utf16String.size(), nullptr, 0, nullptr, nullptr);
		std::string result(nbChars, ' ');
		WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], utf16String.size(), &result[0], nbChars, nullptr, nullptr);
		return result;
	}

	// converts a utf8 char based string to a wchar_t based string
	inline std::wstring convertToWideString(const std::string& src) {
		size_t nbChars = MultiByteToWideChar(CP_UTF8, 0, &src[0], src.size(), nullptr, 0);
		std::wstring result(nbChars, L' ');
		MultiByteToWideChar(CP_UTF8, 0, &src[0], src.size(), &result[0], nbChars);
		return result;
	}


}