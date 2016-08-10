#include "pch.h"
#include "AlbumResumeItemVM.h"
#include "Api/AlbumResume.h"
#include "tools/StringUtils.h"
#include "Api/ImageUriResolver.h"
#include "AlbumPage.xaml.h"
#include "Shell.xaml.h"
#include "ArtistPage.xaml.h"
#include <Api/CoverCache.h>
#include <XboxUI/XboxShell.xaml.h>

void Tidal::AlbumResumeItemVM::Go()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(AlbumPage::typeid, Id.ToString());
	}
	else {
		auto xbshell = dynamic_cast<XboxShell^>(Windows::UI::Xaml::Window::Current->Content);
		if (xbshell) {
			xbshell->Frame->Navigate(AlbumPage::typeid, Id.ToString());
		}
	}
}

void Tidal::AlbumResumeItemVM::GoToArtist()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell && ArtistId != 0) {
		shell->Frame->Navigate(ArtistPage::typeid, ArtistId);
	}
}

Tidal::AlbumResumeItemVM::AlbumResumeItemVM(const api::AlbumResume & info, bool offline)
{
	ArtistId = info.artist.id;
	Id = info.id;
	if (!offline) {
		if (info.cover.size() == 0) {
			ImageUrl = L"https://resources.wimpmusic.com/images/0dfd3368/3aa1/49a3/935f/10ffb39803c0/160x160.jpg";
			SearchImageUrl = L"https://resources.wimpmusic.com/images/0dfd3368/3aa1/49a3/935f/10ffb39803c0/80x80.jpg";
		}
		else {
			ImageUrl = api::resolveImageUri(info.cover, 160, 160);
			SearchImageUrl = api::resolveImageUri(info.cover, 80, 80);
		}
	}
	else {
		ImageUrl = api::getOfflineCoverUrl(info.id, 160, 160);
		SearchImageUrl = api::getOfflineCoverUrl(info.id, 80, 80);
	}
	Title = tools::strings::toWindowsString(info.title);
	if (info.artist.name.size() > 0) {
		ArtistName = tools::strings::toWindowsString(info.artist.name);
	}
	else if (info.artists.size() > 0) {

		ArtistName = tools::strings::toWindowsString(info.artists[0].name);
	}
}
