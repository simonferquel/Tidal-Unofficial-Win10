//
// VideoPlayer.xaml.h
// Declaration of the VideoPlayer class
//

#pragma once

#include "VideoPlayer.g.h"
#include <ppltasks.h>
#include <Hat.h>
namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class VideoPlayer sealed
	{
	private:
		concurrency::cancellation_token_source _cts;

	protected:
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override {
			_cts.cancel();
			_cts = concurrency::cancellation_token_source();
		}
	private:
		concurrency::task<void> launchVideo(Hat<Platform::String> id);
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		
	public:
		VideoPlayer();
	private:
		void OnMediaFailed(Platform::Object^ sender, Windows::UI::Xaml::ExceptionRoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
