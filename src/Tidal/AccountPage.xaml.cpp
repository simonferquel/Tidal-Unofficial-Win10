//
// AccountPage.xaml.cpp
// Implementation of the AccountPage class
//

#include "pch.h"
#include "AccountPage.xaml.h"
#include "AuthenticationService.h"
using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

AccountPage::AccountPage()
{
	InitializeComponent();
}



void Tidal::AccountPage::OnLogoutClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAuthenticationService().logout();
}
