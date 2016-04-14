//
// MyMusic.xaml.h
// Declaration of the MyMusic class
//

#pragma once

#include "MyMusic.g.h"
#include "TracksPlaybackStateManager.h"
#include "IPageWithPreservedState.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MyMusic sealed : public IPageWithPreservedState
	{
	private:
		concurrency::cancellation_token_source _cts;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;
	public:
		MyMusic();
		// Inherited via IPageWithPreservedState
		virtual Platform::Object ^ GetStateToPreserve();
	protected:

		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:

		concurrency::task<void> LoadAsync();
		void OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnArtistClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
