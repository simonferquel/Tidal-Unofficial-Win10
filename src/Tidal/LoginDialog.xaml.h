//
// LoginDialog.xaml.h
// Declaration of the LoginDialog class
//

#pragma once

#include "LoginDialog.g.h"

namespace Tidal
{
	public enum class LoginDialogResult {
		LoggedInWithPassword,
		FacebookLoginRequested,
		Canceled

	};
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class LoginDialog sealed
	{
	private:
		LoginDialogResult _loginResult = LoginDialogResult::Canceled;
	public:
		LoginDialog();
		property LoginDialogResult DialogResult {LoginDialogResult get() { return _loginResult; }}
	private:
		concurrency::task<void> loginAsync();
		void OnCloseTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);

		
		void OnFBClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnLoginClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPasswordKeydown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}
