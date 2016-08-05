//
// XboxHomeMyMusicWidgetxaml.xaml.h
// Declaration of the XboxHomeMyMusicWidgetxaml class
//

#pragma once

#include "XboxUI\XboxHomeMyMusicWidget.g.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxHomeMyMusicWidget sealed
	{
	public:
		XboxHomeMyMusicWidget();
	private:
		void OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
