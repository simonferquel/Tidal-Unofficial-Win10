//
// XboxSettingsPage.xaml.h
// Declaration of the XboxSettingsPage class
//

#pragma once

#include "XboxUI\XboxSettingsPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxSettingsPage sealed
	{
	public:
		XboxSettingsPage();
	private:

		concurrency::task<void> loadStorageStatisticsAsync();
		void OnStreamingQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnImportQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnClearCache_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnRemoveOldCacheTracks_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnClearImports_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnRemoveOldImportedTracks_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
