//
// XboxAlbumPage.xaml.h
// Declaration of the XboxAlbumPage class
//

#pragma once

#include "XboxUI\XboxAlbumPage.g.h"
#include <memory>
#include <TracksPlaybackStateManager.h>
#include <collection.h>
#include "../TrackItemVM.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxAlbumPage sealed
	{
		std::int64_t _id = -1;
		std::int64_t _artistId = -1;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;

		Platform::Collections::Vector<TrackItemVM^>^ _tracks = nullptr;
		concurrency::task<void> LoadAsync();
	public:
		XboxAlbumPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnImportClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnGoToArtist(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
