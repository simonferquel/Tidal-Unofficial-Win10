#include "pch.h"
#include "MenuFlyouts.h"
#include "DownloadService.h"
#include "AuthenticationService.h"
#include <Api/GetAlbumTracksQuery.h>
#include "AddToPlaylistDialog.xaml.h"

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
	if (getAuthenticationService().authenticationState().isAuthenticated()) {
		auto addToPlaylistItem = ref new MenuFlyoutItem();
		addToPlaylistItem->Text = L"add to playlist";
		addToPlaylistItem->Click += ref new Windows::UI::Xaml::RoutedEventHandler([id](Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^) {
			auto& authState = getAuthenticationService().authenticationState();
			api::GetAlbumTracksQuery query(id, 1000,0, authState.sessionId(), authState.countryCode());
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
	}
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
