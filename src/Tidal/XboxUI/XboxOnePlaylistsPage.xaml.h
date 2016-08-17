//
// XboxOnePlaylistsPage.xaml.h
// Declaration of the XboxOnePlaylistsPage class
//

#pragma once

#include "XboxUI\XboxOnePlaylistsPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxOnePlaylistsPage sealed
	{
	public:
		XboxOnePlaylistsPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		void OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
