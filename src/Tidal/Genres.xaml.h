//
// Genres.xaml.h
// Declaration of the Genres class
//

#pragma once

#include "Genres.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Genres sealed
	{
	private:

		concurrency::task<void> LoadAsync();
	public:
		Genres();
	private:
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnGenreClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
