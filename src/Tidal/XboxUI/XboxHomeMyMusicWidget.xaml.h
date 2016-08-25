//
// XboxHomeMyMusicWidgetxaml.xaml.h
// Declaration of the XboxHomeMyMusicWidgetxaml class
//

#pragma once

#include "XboxUI\XboxHomeMyMusicWidget.g.h"
#include "../Mediators.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxHomeMyMusicWidget sealed
	{
	private:

		std::vector<RegistrationToken> _mediatorTokens;
		
	public:
		XboxHomeMyMusicWidget();
	private:
		concurrency::task<void> UpdateDownloadsTile();
		void OnTrackImportComplete(const std::int64_t& id);
		void OnTrackImportProgress(const ImportProgress& progress);
		void OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnFavorites(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnGoToLocal(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnGoToNyPlaylists(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnGoToDownloads(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnGoToSettings(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
