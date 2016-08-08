//
// PlaylistPage.xaml.cpp
// Implementation of the PlaylistPage class
//

#include "pch.h"
#include "PlaylistPage.xaml.h"
#include "PlaylistsDataSources.h"
#include <tools/StringUtils.h>
#include <tools/TimeUtils.h>
#include <Api/ImageUriResolver.h>
#include "TrackItemVM.h"
#include "MenuFlyouts.h"
#include <Api/CoverCache.h>
#include "AudioService.h"
#include "FavoritesService.h"
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

PlaylistPage::PlaylistPage()
{
	InitializeComponent();
	_mediatorTokens.push_back(getItemRemovedFromPlaylistMediator().registerCallback<PlaylistPage>(this, &PlaylistPage::OnTrackRemovedFromPlaylist));
}



void Tidal::PlaylistPage::OnTrackRemovedFromPlaylist(const ItemRemovedFromPlaylist & ev)
{
	if (_playlistId == ev.playlistId&& _tracks != nullptr) {
		for (auto ix = 0; ix < _tracks->Size; ++ix) {
			if (_tracks->GetAt(ix)->Id == ev.trackId) {
				_tracks->RemoveAt(ix);
				return;
			}
		}
	}
}

concurrency::task<void> Tidal::PlaylistPage::LoadAsync(Hat<Windows::UI::Xaml::Navigation::NavigationEventArgs> args)
{
	try {
		auto idPlat = dynamic_cast<String^>(args->Parameter);
		auto id = tools::strings::toStdString(idPlat);
		if (getFavoriteService().isPlaylistFavorite(id)) {
			addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else {
			removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		_playlistId = id;
		auto playlistInfo = co_await  playlists::getPlaylistAsync(id, concurrency::cancellation_token::none());
		pageHeader->Text = tools::strings::toWindowsString(playlistInfo->title);
		headeTitle->Text = tools::strings::toWindowsString(playlistInfo->title);
		headerArtist->Text = playlistInfo->creator.name.size() > 0 ? tools::strings::toWindowsString(playlistInfo->creator.name) : L"TIDAL";
		headerDescription->Text = tools::strings::toWindowsString(playlistInfo->description);
		auto coverId = tools::strings::toWindowsString(playlistInfo->image);
		auto urlTask = api::GetPlaylistCoverUriAndFallbackToWebAsync(playlistInfo->uuid, coverId, 1080, 720, concurrency::cancellation_token::none());
		co_await urlTask.then([this, playlistInfo, id](Platform::String^ url) {
		headerImage->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(url));
		std::wstring tracksAndDurations = std::to_wstring(playlistInfo->numberOfTracks);
		tracksAndDurations.append(L" tracks (");
		tracksAndDurations.append(tools::time::toStringMMSS(playlistInfo->duration));
		tracksAndDurations.append(L")");
		headerTracksAndDuration->Text = tools::strings::toWindowsString(tracksAndDurations);
		return playlists::getPlaylistTracksAsync(id, playlistInfo->numberOfTracks, concurrency::cancellation_token::none()).then([this, id, playlistInfo](const std::shared_ptr<api::PaginatedList<api::TrackInfo>> & tracks) {
			auto tracksVM = ref new Platform::Collections::Vector<TrackItemVM^>();
			for (auto&& t : tracks->items) {
				auto ti = ref new TrackItemVM(t);
				tracksVM->Append(ti);
				ti->AttachTo(tracksVM);
				if (playlistInfo->type == L"USER" && playlistInfo->creator.id == getAuthenticationService().authenticationState().userId()) {
					ti->attachToOwiningPlaylistId(playlistInfo->uuid);
				}
			}
			_tracks = tracksVM;
			_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
			_tracksPlaybackManager->initialize(_tracks, Dispatcher);
			tracksLV->ItemsSource = _tracks;
		}, concurrency::task_continuation_context::get_current_winrt_context());

		}, concurrency::task_continuation_context::get_current_winrt_context());
	}
	catch (...) {}
}


void Tidal::PlaylistPage::OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_tracks && _tracks->Size>0) {
		_tracks->GetAt(0)->PlayCommand->Execute(nullptr);
	}
}


void Tidal::PlaylistPage::OnContextMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto flyout = getPlaylistMenuFlyout(_playlistId);
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	flyout->ShowAt(fe, Point(0, fe->ActualHeight));
}


void Tidal::PlaylistPage::AddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().addPlaylistAsync(_playlistId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void Tidal::PlaylistPage::RemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().removePlaylistAsync(_playlistId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}
