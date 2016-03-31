//
// LoginDialog.xaml.cpp
// Implementation of the LoginDialog class
//

#include "pch.h"
#include "LoginDialog.xaml.h"
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

// The Content Dialog item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Tidal::LoginDialog::LoginDialog()
{
	InitializeComponent();
}



concurrency::task<void> Tidal::LoginDialog::loginAsync()
{
	loginTxtBox->IsEnabled = false;
	passwordBox->IsEnabled = false;
	loginBtn->IsEnabled = false;
	loginFBBtn->IsEnabled = false;
	progressBar->IsIndeterminate = true;
	validationMessage->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

	try {
		co_await getAuthenticationService().authenticateWithPasswordAsync(loginTxtBox->Text, passwordBox->Password, concurrency::cancellation_token::none());
		_loginResult = LoginDialogResult::LoggedInWithPassword;
		Hide();
		return;
	}
	catch (...) {
		validationMessage->Visibility = Windows::UI::Xaml::Visibility::Visible;
		loginTxtBox->IsEnabled = true;
		passwordBox->IsEnabled = true;
		loginBtn->IsEnabled = true;
		loginFBBtn->IsEnabled = true;
		progressBar->IsIndeterminate = false;

	}
}

void Tidal::LoginDialog::OnCloseTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	this->Hide();
}


void Tidal::LoginDialog::OnFBClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->_loginResult = LoginDialogResult::FacebookLoginRequested;
	this->Hide();
}


void Tidal::LoginDialog::OnLoginClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	loginAsync();
}


void Tidal::LoginDialog::OnPasswordKeydown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Enter) {
		loginAsync();
	}
}
