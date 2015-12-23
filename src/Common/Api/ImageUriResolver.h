#pragma once
#include <string>
namespace api {
	Platform::String^ resolveImageUri(const std::wstring& id, int width, int height);
}