//
// XboxSearchResults.xaml.h
// Declaration of the XboxSearchResults class
//

#pragma once

#include "XboxUI\XboxSearchResults.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxSearchResults sealed
	{
	private:
		concurrency::task<void> LoadAsync(Platform::String^ query);
	public:
		XboxSearchResults();
	protected:

		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
