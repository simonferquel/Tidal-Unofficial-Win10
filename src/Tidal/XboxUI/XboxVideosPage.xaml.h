//
// XboxVideosPage.xaml.h
// Declaration of the XboxVideosPage class
//

#pragma once

#include "XboxUI\XboxVideosPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxVideosPage sealed
	{
	private:
		concurrency::task<void> LoadAsync();
	public:
		XboxVideosPage();
	private:
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
