//
// XboxPlaylistPage.xaml.h
// Declaration of the XboxPlaylistPage class
//

#pragma once

#include "XboxUI\XboxPlaylistPage.g.h"
#include <string>
#include <memory>
#include "../TracksPlaybackStateManager.h"
#include "TrackItemVM.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxPlaylistPage sealed
	{
	private:

		Platform::Collections::Vector<TrackItemVM^>^ _tracks;
		std::wstring _playlistId;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;

		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			LoadAsync(e);
		}

	public:
		XboxPlaylistPage();
	private:
		void OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnImportClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
