//
// XboxHomePage.xaml.h
// Declaration of the XboxHomePage class
//

#pragma once
#include "../XboxOneHub.h"
#include "XboxUI\XboxHomePage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxHomePage sealed
	{
	private:
		Windows::UI::Composition::CompositionPropertySet^ _homePagePropertySet;
	public:
		XboxHomePage();
	private:
		void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
