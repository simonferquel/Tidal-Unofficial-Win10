//
// Videos.xaml.h
// Declaration of the Videos class
//

#pragma once

#include "Videos.g.h"
#include "SublistItemVM.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Videos sealed
	{
	public:
		Videos();
	private:
		concurrency::task<void> LoadAsync();
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnVideoFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e);
		void OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
