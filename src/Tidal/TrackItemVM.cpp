#include "pch.h"
#include "TrackItemVM.h"
#include "Api/TrackInfo.h"
#include "tools/StringUtils.h"
#include "tools/TimeUtils.h"
#include "Api/ImageUriResolver.h"
#include "Shell.xaml.h"
#include "ArtistPage.xaml.h"
#include "AlbumPage.xaml.h"
#include "FavoritesService.h"
#include "PlayCommand.h"
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
#include "AddToPlaylistDialog.xaml.h"
#include "MenuFlyouts.h"
using namespace api;
Windows::UI::Xaml::Input::ICommand^ Tidal::TrackItemVM::PlayCommand::get() {
	auto lst = _trackListRef.Resolve<Windows::Foundation::Collections::IIterable<TrackItemVM^>>();
	if (lst) {
		return ref new Tidal::PlayCommand(this, lst);
	}
	else {
		return ref new Tidal::PlayCommand(this);
	}
}
void Tidal::TrackItemVM::GoToArtist()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(ArtistPage::typeid, _trackInfo.artist.id);
	}
}
void Tidal::TrackItemVM::GoToAlbum()
{
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(AlbumPage::typeid, _trackInfo.album.id.ToString());
	}
}
void Tidal::TrackItemVM::AddFavorite()
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().addTrackAsync(Id).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	AddFavoriteVisibility = Windows::UI::Xaml::Visibility::Collapsed;
	RemoveFavoriteVisibility = Windows::UI::Xaml::Visibility::Visible;
}
void Tidal::TrackItemVM::RemoveFavorite()
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().removeTrackAsync(Id).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	AddFavoriteVisibility = Windows::UI::Xaml::Visibility::Visible;
	RemoveFavoriteVisibility = Windows::UI::Xaml::Visibility::Collapsed;
}
void Tidal::TrackItemVM::AddToPlaylist()
{
	ShowAddToPlaylistDialog(Id);
}
void Tidal::TrackItemVM::ShowMenu(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
	auto menu = getTrackMenuFlyout(_trackInfo, _owningPlaylistId);
	auto fe = dynamic_cast<Windows::UI::Xaml::FrameworkElement^>(sender);
	menu->ShowAt(fe, Windows::Foundation::Point(0, fe->ActualHeight));
}
Tidal::TrackItemVM::TrackItemVM(const TrackInfo & info, bool includeTrackNumberInTitle) : _trackInfo(info)
{
	if (getFavoriteService().isTrackFavorite(info.id)) {
		_addFavoriteVisibility = Windows::UI::Xaml::Visibility::Collapsed;
		_removeFavoriteVisibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else {
		_addFavoriteVisibility = Windows::UI::Xaml::Visibility::Visible;
		_removeFavoriteVisibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	Opacity = info.allowStreaming ? 1.0 : 0.5;
	_playButtonVisibility = Windows::UI::Xaml::Visibility::Visible;
	Id = info.id;
	AlbumId = info.album.id;
	if (includeTrackNumberInTitle) {
		std::wstring title;
		if (info.trackNumber < 10) {
			title.append(L" ");
		}
		title.append(info.trackNumber.ToString()->Data());
		title.append(L". ");
		title.append(info.title);
		Title = tools::strings::toWindowsString(title);
	}
	else{
		Title = tools::strings::toWindowsString(info.title);
	}
	AlbumTitle = tools::strings::toWindowsString(info.album.title);
	if (info.artist.name.size()>0) {
		ArtistName = tools::strings::toWindowsString(info.artist.name);
	}
	else if (info.artists.size() > 0) {
		ArtistName = tools::strings::toWindowsString(info.artists[0].name);

	}
	DurationText = tools::strings::toWindowsString(tools::time::toStringMMSS(info.duration));
	ImageUrl = api::resolveImageUri(info.album.cover, 80, 80);
}

void Tidal::TrackItemVM::RefreshPlayingState(std::int64_t trackId, Windows::Media::Playback::MediaPlayerState state)
{
	bool isPlaying = Id == trackId && state == Windows::Media::Playback::MediaPlayerState::Playing;
	PlayButtonVisibility = isPlaying ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
}

Windows::UI::Xaml::Visibility Tidal::TrackItemVM::PlayButtonVisibility::get() {
	return _playButtonVisibility;
}

void Tidal::TrackItemVM::PlayButtonVisibility::set(Windows::UI::Xaml::Visibility value) {
	if (_playButtonVisibility != value) {
		_playButtonVisibility = value;
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PlayButtonVisibility"));
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"PauseButtonVisibility"));
		PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"TitleFontWeight"));
	}
}