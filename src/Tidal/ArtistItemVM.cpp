#include "pch.h"
#include "ArtistItemVM.h"
#include <Api/ImageUriResolver.h>
#include <tools/StringUtils.h>
#include <Shell.xaml.h>
#include <ArtistPage.xaml.h>
#include "XboxUI/XboxShell.xaml.h"
#include "XboxUI/XboxArtistPage.xaml.h"
void Tidal::ArtistItemVM::Go()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell && Id != 0) {
		shell->Frame->Navigate(ArtistPage::typeid, Id);
	}
	else {
		auto xbShell = dynamic_cast<XboxShell^>(Windows::UI::Xaml::Window::Current->Content);
		if (xbShell && Id != 0) {
			xbShell->Frame->Navigate(XboxArtistPage::typeid, Id);
		}
	}
}
Tidal::ArtistItemVM::ArtistItemVM(api::ArtistInfo & artistInfo)
{
	_info = artistInfo;
	Id = artistInfo.id;
	Name = tools::strings::toWindowsString(artistInfo.name);
	if (artistInfo.picture.size() > 0) {
		ImageUrl = api::resolveImageUri(artistInfo.picture, 320, 214);
		SearchImageUrl = api::resolveImageUri(artistInfo.picture, 160, 160);
	}
	else {
		ImageUrl = api::resolveImageUri(L"1e01cdb6-f15d-4d8b-8440-a047976c1cac", 320, 214);
		SearchImageUrl = api::resolveImageUri(L"1e01cdb6-f15d-4d8b-8440-a047976c1cac", 160, 160);
	}
}
