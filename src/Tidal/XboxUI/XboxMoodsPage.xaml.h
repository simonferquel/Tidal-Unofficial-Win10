//
// XboxMoodsPage.xaml.h
// Declaration of the XboxMoodsPage class
//

#pragma once

#include "XboxUI\XboxMoodsPage.g.h"
#include <ppltasks.h>

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxMoodsPage sealed
	{
		concurrency::task<void> LoadAsync();
	public:
		XboxMoodsPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
