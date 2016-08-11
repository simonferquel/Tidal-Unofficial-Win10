//
// XboxGenresPage.xaml.h
// Declaration of the XboxGenresPage class
//

#pragma once

#include "XboxUI\XboxGenresPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxGenresPage sealed
	{
		concurrency::task<void> LoadAsync();
	public:
		XboxGenresPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
