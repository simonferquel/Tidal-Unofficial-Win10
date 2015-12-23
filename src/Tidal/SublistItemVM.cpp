#include "pch.h"
#include "SublistItemVM.h"
#include <tools/StringUtils.h>
#include <Api/ImageUriResolver.h>

Tidal::SublistItemVM::SublistItemVM(const api::SublistInfo & sublistInfo) :
	_sublistInfo(sublistInfo)
{
	Name = tools::strings::toWindowsString(sublistInfo.name);
	Path = tools::strings::toWindowsString(sublistInfo.path);
	if (sublistInfo.image.size() > 0) {
		ImageUrl = api::resolveImageUri(sublistInfo.image, 342, 342);
		HeadingUrl = api::resolveImageUri(sublistInfo.image, 2048, 330);
		GenreImageUrl = api::resolveImageUri(sublistInfo.image, 460, 306);
	}
}
