#include "pch.h"
#include "VideoItemVM.h"
#include "tools/TimeUtils.h"
#include "tools/StringUtils.h"
#include "Api/VideoInfo.h"
#include "Api/ImageUriResolver.h"
#include "Shell.xaml.h"
#include "VideoPlayer.xaml.h"
#include "ArtistPage.xaml.h"
void Tidal::VideoItemVM::Go()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(VideoPlayer::typeid, Id.ToString());
	}
}
void Tidal::VideoItemVM::GoToArtist()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell && ArtistId != 0) {
		shell->Frame->Navigate(ArtistPage::typeid, ArtistId);
	}
}
Tidal::VideoItemVM::VideoItemVM(const api::VideoInfo & info)
{
	Id = info.id;
	ImageUrl = api::resolveImageUri(info.imageId, 320, 214);
	SearchImageUrl = api::resolveImageUri(info.imageId, 160, 107);
	DurationText = tools::strings::toWindowsString( tools::time::toStringMMSS(info.duration));
	Title = tools::strings::toWindowsString(info.title);
	if (info.artist.name.size() > 0) {
		ArtistName = tools::strings::toWindowsString(info.artist.name);
	}
	else if (info.artists.size() > 0) {
		ArtistName = tools::strings::toWindowsString(info.artists[0].name);

	}
	ArtistId = info.artist.id;
}
