#include "pch.h"
#include "ImageUriResolver.h"
#include "Config.h"
#include "../tools/StringUtils.h"

Platform::String^ api::resolveImageUri(const std::wstring & id, int width, int height)
{
	std::wstring path = id;
	for (auto& c : path) {
		if (c == L'-') {
			c = L'/';
		}
	}
	path.push_back(L'/');
	path.append(std::to_wstring(width));
	path.push_back(L'x');
	path.append(std::to_wstring(height));
	path.append(L".jpg");
	return config::imageLocationPrefix() + tools::strings::toWindowsString(path);
}
