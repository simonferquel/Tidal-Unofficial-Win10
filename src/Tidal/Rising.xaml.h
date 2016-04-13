//
// Rising.xaml.h
// Declaration of the Rising class
//

#pragma once

#include "Rising.g.h"
#include "TracksPlaybackStateManager.h"
#include "IPageWithPreservedState.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Rising sealed : public IPageWithPreservedState
	{
	private:
		concurrency::cancellation_token_source _cts;
		std::shared_ptr<TracksPlaybackStateManager> _tracksPlaybackManager;
	protected:
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	public:
		Rising();
		virtual	Platform::Object^ GetStateToPreserve();
	private:
		concurrency::task<void> loadAsync();
		void OnPromotionClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTrackClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
