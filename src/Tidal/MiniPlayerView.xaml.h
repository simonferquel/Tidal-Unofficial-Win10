//
// MiniPlayerView.xaml.h
// Declaration of the MiniPlayerView class
//

#pragma once

#include "MiniPlayerView.g.h"
#include "Mediator.h"
#include <tools/ScopedEventRegistration.h>
#include <Api/TrackInfo.h>

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class MiniPlayerView sealed
	{
	private:
		tools::ScopedEventRegistrations _eventRegistrations;
		std::vector<RegistrationToken> _mediatorTokens;
	public:
		MiniPlayerView();
	private:
		void UpdateState();
		void AttachToPlayerEvents();
		void DettachFromPlayerEvents();
		void OnCurrentTrackChanged();
		void OnAppResuming();
		concurrency::task<void> HandleTrackInfoAsync(const api::TrackInfo& info);
		concurrency::task<void> OnAppSuspending();
		void Image_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnMediaPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args);
		void OnPlayPauseClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnNext(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPrevious(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnShuffleClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnRepeatClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
