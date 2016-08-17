//
// XboxSearchPage.xaml.h
// Declaration of the XboxSearchPage class
//

#pragma once

#include "XboxUI\XboxSearchPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxSearchPage sealed
	{
	public:
		XboxSearchPage();
	private:
		void OnKeydown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
