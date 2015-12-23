//
// ArtistPage.xaml.h
// Declaration of the ArtistPage class
//

#pragma once
#include "WrapPanel.h"
#include "TrackItemVM.h"
#include "ArtistItemVM.h"
#include "GroupedAlbums.h"
#include "ArtistPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ArtistPage sealed
	{
	private:

		Microsoft::Graphics::Canvas::CanvasBitmap^ _albumBmp;
	public:
		ArtistPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			LoadAsync(e);
		}
	private:

		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
		concurrency::task<void> loadImageAsync(Platform::String^ url);
		void OnDrawHeaderImage(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args);
		void OnPlayFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPauseFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnAlbumClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnSimilarArtistClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
