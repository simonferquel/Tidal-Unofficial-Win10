//
// CurrentPlaylistPage.xaml.h
// Declaration of the CurrentPlaylistPage class
//

#pragma once

#include <Mediator.h>
#include <tools/ScopedEventRegistration.h>
#include "CurrentPlaylistPage.g.h"
#include "TrackItemVM.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CurrentPlaylistPage sealed
	{
	public:
		CurrentPlaylistPage();
	private:
		std::int64_t _currentAlbumId = -1;
		concurrency::cancellation_token_source _currentAlbumImageCts;

		std::vector<RegistrationToken> _mediatorRegistrations;
		tools::ScopedEventRegistrations _eventRegistrations;


		Platform::Collections::Vector<Tidal::TrackItemVM^>^ _tracks;

		void AttachToPlayerEvents();
		void DettachFromPlayerEvents();
		void ReevaluateTracksPlayingStates();

		concurrency::task<void> OnAppSuspended();
		void OnAppResuming();
		void OnPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args);

		concurrency::task<void> LoadAsync();
		concurrency::task<void> TransitionBackground(std::int64_t albumId, const std::wstring& cover, concurrency::cancellation_token cancelToken);
		void OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
