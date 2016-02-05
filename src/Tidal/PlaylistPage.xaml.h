//
// PlaylistPage.xaml.h
// Declaration of the PlaylistPage class
//

#pragma once

#include "PlaylistPage.g.h"
#include "TrackItemVM.h"
#include "Mediator.h"
#include <tools/ScopedEventRegistration.h>

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PlaylistPage sealed
	{
	private:

		std::vector<RegistrationToken> _mediatorRegistrations;
		tools::ScopedEventRegistrations _eventRegistrations;
		Platform::Collections::Vector<TrackItemVM^>^ _tracks;
		std::wstring _playlistId;
		concurrency::cancellation_token_source _cts;
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
		void AttachToPlayerEvents();
		void DettachFromPlayerEvents();
		void ReevaluateTracksPlayingStates();
		concurrency::task<void> OnAppSuspended();
		void OnAppResuming();
		void OnPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args);

		void OnPlayFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPauseFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		concurrency::task<void> LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs^ args);
		void OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnContextMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
