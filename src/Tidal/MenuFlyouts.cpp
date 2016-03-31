#include "pch.h"
#include "MenuFlyouts.h"
#include "DownloadService.h"
#include "AuthenticationService.h"
#include <Api/GetAlbumTracksQuery.h>
#include "AddToPlaylistDialog.xaml.h"
#include "UnauthenticatedDialog.h"
#include "Shell.xaml.h"
#include "AlbumPage.xaml.h"
#include <Api/PlaylistsQueries.h>
#include <Api/GetPlaylistTracksQuery.h>
#include <Api/GetPlaylistQuery.h>
#include "Mediators.h"
using namespace Windows::UI::Xaml::Controls;
Windows::UI::Xaml::Controls::MenuFlyout ^ getAlbumMenuFlyout(std::int64_t id)
{
	MenuFlyout^ result = ref new MenuFlyout();
	auto importItem = ref new MenuFlyoutItem();
	importItem->Text = L"import";
	importItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		getDownloadService().StartDownloadAlbumAsync(id);
	});
	result->Items->Append(importItem);
	auto addToPlaylistItem = ref new MenuFlyoutItem();
	addToPlaylistItem->Text = L"add to playlist";
	addToPlaylistItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		auto& authState = getAuthenticationService().authenticationState();
		if (!authState.isAuthenticated()) {
			showUnauthenticatedDialog();
			return;
		}
		api::GetAlbumTracksQuery query(id, 1000, 0, authState.sessionId(), authState.countryCode());
		query.executeAsync(concurrency::cancellation_token::none())
			.then([](concurrency::task<std::shared_ptr<api::PaginatedList<api::TrackInfo>>> result) {
			try {
				std::vector<std::int64_t> ids;
				auto r = result.get();
				for (auto&& item : r->items) {
					ids.push_back(item.id);
				}
				Tidal::ShowAddToPlaylistDialog(std::move(ids));
			}
			catch (...) {}
		}, concurrency::task_continuation_context::get_current_winrt_context());
	});
	result->Items->Append(addToPlaylistItem);

	return result;
}

Windows::UI::Xaml::Controls::MenuFlyout ^ getPlaylistMenuFlyout(const std::wstring & id)
{
	MenuFlyout^ result = ref new MenuFlyout();
	auto importItem = ref new MenuFlyoutItem();
	importItem->Text = L"import";
	importItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		getDownloadService().StartDownloadPlaylistAsync(id);
	});
	result->Items->Append(importItem);
	return result;
}

concurrency::task<void> removeTrackFromPlaylist(std::int64_t trackId, std::wstring playlistId) {
	auto authState = getAuthenticationService().authenticationState();
	try {
		auto playlist = co_await api::GetPlaylistQuery(playlistId, authState.sessionId(), authState.countryCode()).executeAsync(concurrency::cancellation_token::none());
		auto tracks = co_await api::GetPlaylistTracksQuery(playlistId, playlist->numberOfTracks, 0, authState.sessionId(), authState.countryCode()).executeAsync(concurrency::cancellation_token::none());
		std::int32_t index = -1;
		for (auto ix = 0; ix < tracks->items.size(); ++ix) {
			if (tracks->items[ix].id == trackId) {
				index = ix;
				break;
			}
		}
		if (index != -1) {
			co_await api::RemoveItemFromPlaylistQuery(playlistId, tools::strings::toWindowsString( tracks->etag), authState.sessionId(), authState.countryCode(), index).executeAsync(concurrency::cancellation_token::none());
			ItemRemovedFromPlaylist ev;
			ev.playlistId = playlistId;
			ev.trackId = trackId;
			getItemRemovedFromPlaylistMediator().raise(ev);
		}
	}
	catch (...) {}
}

Windows::UI::Xaml::Controls::MenuFlyout ^ getTrackMenuFlyout(const api::TrackInfo & track, std::wstring ownedPlaylist)
{
	MenuFlyout^ result = ref new MenuFlyout();
	auto trackId = track.id;
	auto albumId = track.album.id;
	auto item = ref new MenuFlyoutItem();
	item->Text = L"import";
	item->Click += ref new Windows::UI::Xaml::RoutedEventHandler([trackId](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		getDownloadService().StartDownloadTracksAsync(std::vector<std::int64_t>{trackId});
	});
	result->Items->Append(item);
	item = ref new MenuFlyoutItem();
	item->Text = L"view album";

	item->Click += ref new Windows::UI::Xaml::RoutedEventHandler([albumId](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
		auto shell = dynamic_cast<Tidal::Shell^>( Windows::UI::Xaml::Window::Current->Content);
		if (shell) {
			shell->Frame->Navigate(Tidal::AlbumPage::typeid, albumId.ToString());
		}
	});
	result->Items->Append(item);
	if (ownedPlaylist.size() > 0) {
		item = ref new MenuFlyoutItem();
		item->Text = L"remove from playlist";

		item->Click += ref new Windows::UI::Xaml::RoutedEventHandler([ownedPlaylist, trackId](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
			auto& authState = getAuthenticationService().authenticationState();
			if (!authState.isAuthenticated()) {
				showUnauthenticatedDialog();
				return;
			}
			removeTrackFromPlaylist(trackId, ownedPlaylist);
			
		});
		result->Items->Append(item);
	}
	return result;

}

