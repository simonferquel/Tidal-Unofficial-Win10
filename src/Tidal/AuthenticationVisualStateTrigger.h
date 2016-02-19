#pragma once
#include <Mediator.h>
#include "AuthenticationState.h"
namespace Tidal {
	public ref class AuthenticationVisualStateTrigger sealed : public Windows::UI::Xaml::StateTriggerBase
	{
	private:
		RegistrationToken _authRegistrationToken;

		void OnAuthenticationStateChanged(const AuthenticationState& authState);
	public:
		AuthenticationVisualStateTrigger();
	};
}

