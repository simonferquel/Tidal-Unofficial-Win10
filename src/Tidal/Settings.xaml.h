//
// Settings.xaml.h
// Declaration of the Settings class
//

#pragma once

#include "Settings.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Settings sealed
	{
	public:
		Settings();
	private:
		concurrency::task<void> loadStorageStatisticsAsync();
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnStreamingQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnImportQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
