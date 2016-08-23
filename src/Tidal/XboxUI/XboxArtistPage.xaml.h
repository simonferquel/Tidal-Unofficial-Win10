//
// XboxArtistPage.xaml.h
// Declaration of the XboxArtistPage class
//

#pragma once

#include "XboxUI\XboxArtistPage.g.h"
#include "../TrackItemVM.h"
#include "../TracksPlaybackStateManager.h"
#include <memory>

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxArtistPage sealed
	{
	private:
		std::int64_t _artistId;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;
		Platform::Collections::Vector<TrackItemVM^>^ _popularTracks;

		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			LoadAsync(e);
		}
	public:
		XboxArtistPage();
	private:
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlayRadio(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
