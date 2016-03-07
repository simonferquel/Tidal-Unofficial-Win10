#include "pch.h"
#include "PlaylistResumeItemVM.h"
#include "Api/PlaylistResume.h"
#include "tools/StringUtils.h"
#include "Api/ImageUriResolver.h"
#include "PlaylistPage.xaml.h"
#include "ArtistPage.xaml.h"
#include "Shell.xaml.h"
#include <Api/CoverCache.h>

void Tidal::PlaylistResumeItemVM::Go()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(PlaylistPage::typeid, Uuid);
	}
}

void Tidal::PlaylistResumeItemVM::GoToArtist()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell && ArtistId != 0) {
		shell->Frame->Navigate(ArtistPage::typeid, ArtistId);
	}
}

Tidal::PlaylistResumeItemVM::PlaylistResumeItemVM(const api::PlaylistResume & info, bool offline)
{
	ArtistId = info.creator.id;
	NumberOfTracks = info.numberOfTracks;
	Uuid = tools::strings::toWindowsString(info.uuid);
	if (offline) {
		ImageUrl = api::getPlaylistOfflineCoverUrl(info.uuid, 320, 214);
		SearchImageUrl = api::getPlaylistOfflineCoverUrl(info.uuid, 160, 107);
	}
	else {
		ImageUrl = api::resolveImageUri(info.image, 320, 214);
		SearchImageUrl = api::resolveImageUri(info.image, 160, 107);
	}
	Title = tools::strings::toWindowsString(info.title);
	if (info.creator.name.size() == 0) {
		ArtistName = L"TIDAL";
	}
	else {
		ArtistName = tools::strings::toWindowsString(info.creator.name);
	}
}
