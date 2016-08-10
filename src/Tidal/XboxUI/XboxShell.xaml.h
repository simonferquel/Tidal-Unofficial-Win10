//
// XboxShell.xaml.h
// Declaration of the XboxShell class
//

#pragma once

#include "XboxUI\XboxShell.g.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxShell sealed
	{
	public:
		XboxShell();
		property Windows::UI::Xaml::Controls::Frame^ Frame {
			Windows::UI::Xaml::Controls::Frame^ get() { return frame; }
		}
	private:
		void OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^e);
		void OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
	};
}
