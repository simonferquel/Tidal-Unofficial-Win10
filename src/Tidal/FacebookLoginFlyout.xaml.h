//
// FacebookLoginFlyout.xaml.h
// Declaration of the FacebookLoginFlyout class
//

#pragma once

#include "FacebookLoginFlyout.g.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class FacebookLoginFlyout sealed
	{
	public:
		FacebookLoginFlyout();
	private:
		void OnWebViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnNavigating(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ args);
	};
}
