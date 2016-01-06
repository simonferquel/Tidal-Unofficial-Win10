//
// AlbumPage.xaml.h
// Declaration of the AlbumPage class
//

#pragma once
#include <Mediator.h>
#include "TrackItemVM.h"
#include "AlbumPage.g.h"
#include "TrackItemVM.h"
#include <tools/ScopedEventRegistration.h>
#include "AlbumResumeItemVM.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AlbumPage sealed
	{
	private:
		std::vector<RegistrationToken> _mediatorRegistrations;
		tools::ScopedEventRegistrations _eventRegistrations;
		std::int64_t _artistId;
		std::int64_t _albumId;
	public:
		AlbumPage();
		virtual ~AlbumPage();
	protected:

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void AttachToPlayerEvents();
		void DettachFromPlayerEvents();
		void ReevaluateTracksPlayingStates();
		concurrency::task<void> LoadAsync(std::int64_t id);
		Platform::Collections::Vector<TrackItemVM^>^ _tracks = nullptr;
		Microsoft::Graphics::Canvas::CanvasBitmap^ _albumBmp;
		concurrency::task<void> loadImageAsync(Platform::String^ url);

		void OnWin2DDrawing(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args);
		void OnWin2DCtlLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlayAlbumClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPlayFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		concurrency::task<void> OnAppSuspended();
		void OnAppResuming();
		void OnPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args);
		void OnPauseFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnGoToArtist(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args);
		void OnMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnAddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnRemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
