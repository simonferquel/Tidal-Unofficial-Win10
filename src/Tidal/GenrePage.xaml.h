//
// GenrePage.xaml.h
// Declaration of the GenrePage class
//

#pragma once

#include "GenrePage.g.h"
#include "IGo.h"
#include "TracksPlaybackStateManager.h"
namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class GenrePage sealed
	{
	private:
		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		concurrency::cancellation_token_source _cts;
		std::shared_ptr<TracksPlaybackStateManager> _playbackStateManager;
	protected:

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
	public:
		GenrePage();
	private:
		void OnItemClick(IGo^ item);
		void OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
