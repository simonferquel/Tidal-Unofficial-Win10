#pragma once
#include <string>
#include "StringUtils.h"
namespace LocalDB {

	inline wchar_t pathSeparator() {
		return L'\\';
	}

	// convert a path relative to the local storage root to an absolute filepath
	inline std::wstring makeLocalStoragePath(const wchar_t* relativePath) {

		std::wstring result(Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data());
		appendIfNotEndsWith(result, pathSeparator());
		result.append(relativePath);
		return result;
	}
}