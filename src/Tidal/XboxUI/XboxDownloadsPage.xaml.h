//
// XboxDownloadsPage.xaml.h
// Declaration of the XboxDownloadsPage class
//

#pragma once

#include "XboxUI\XboxDownloadsPage.g.h"
#include "../LocalMusic.xaml.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxDownloadsPage sealed
	{
	private:
		std::vector<RegistrationToken> _mediatorTokens;
		Platform::Collections::Vector<DownloadItemVM^>^ _downloadQueue = ref new Platform::Collections::Vector<DownloadItemVM^>();
	public:
		XboxDownloadsPage();
	private:
		concurrency::task<void> LoadAsync();

		void OnTrackImportComplete(const std::int64_t& id);
		void OnTrackImportProgress(const ImportProgress& progress);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
