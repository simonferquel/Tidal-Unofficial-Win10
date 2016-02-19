//
// AccountPage.xaml.h
// Declaration of the AccountPage class
//

#pragma once
#include "AuthenticationVisualStateTrigger.h"
#include "AccountPage.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AccountPage sealed
	{
	public:
		AccountPage();
	private:
		void OnLogoutClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
