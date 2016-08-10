//
// LoginView.xaml.h
// Declaration of the LoginView class
//

#pragma once

#include "LoginView.g.h"
#include "Mediator.h"
class AuthenticationState;
namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class LoginView sealed
	{
	private:
		RegistrationToken _mediatorToken;
		void UpdateForAuthenticationState(const AuthenticationState& authState, bool useTransitions);
		concurrency::task<void> loginAsync();
	public:
		LoginView();
		void OnViewLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	private:
		void OnLoginClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnViewTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void OnKeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}
