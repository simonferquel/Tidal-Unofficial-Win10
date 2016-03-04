//
// PlaylistPage.xaml.h
// Declaration of the PlaylistPage class
//

#pragma once

#include "PlaylistPage.g.h"
#include "TrackItemVM.h"
#include "Mediator.h"
#include <tools/ScopedEventRegistration.h>
#include "TracksPlaybackStateManager.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PlaylistPage sealed
	{
	private:

		Platform::Collections::Vector<TrackItemVM^>^ _tracks;
		std::wstring _playlistId;
		concurrency::cancellation_token_source _cts;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;
	public:
		PlaylistPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			LoadAsync(e);
		}

		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
	private:

		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
		void OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnContextMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
