//
// XboxHomeCurrentPlaylistWidget.xaml.h
// Declaration of the XboxHomeCurrentPlaylistWidget class
//

#pragma once

#include "XboxUI\XboxHomeCurrentPlaylistWidget.g.h"
#include <ppltasks.h>
#include "Mediator.h"
#include <vector>
#include <TrackItemVM.h>
#include <memory>
#include <TracksPlaybackStateManager.h>
namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxHomeCurrentPlaylistWidget sealed
	{
	private:
		concurrency::cancellation_token_source _loadCts;
		std::vector<RegistrationToken> _mediatorTokens;
		std::shared_ptr< TracksPlaybackStateManager> _tracksPlaybackStateMgr;
	public:
		XboxHomeCurrentPlaylistWidget();
	private:
		void OnCurrentTrackPlaybackChanged();
		void OnCurrentPlaylistChanged();
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		concurrency::task<void> LoadAsync(concurrency::cancellation_token cancelToken);
		concurrency::task<void> LoadCurrentTrackInfoAsync();
		void OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
