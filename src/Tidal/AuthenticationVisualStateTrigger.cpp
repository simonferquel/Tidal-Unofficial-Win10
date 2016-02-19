#include "pch.h"
#include "AuthenticationVisualStateTrigger.h"
#include "Mediators.h"
#include "AuthenticationService.h"

Tidal::AuthenticationVisualStateTrigger::AuthenticationVisualStateTrigger()
{
	_authRegistrationToken = getAuthenticationStateMediator().registerCallback<AuthenticationVisualStateTrigger>(this, &AuthenticationVisualStateTrigger::OnAuthenticationStateChanged);
	OnAuthenticationStateChanged(getAuthenticationService().authenticationState());
}

void Tidal::AuthenticationVisualStateTrigger::OnAuthenticationStateChanged(const AuthenticationState & authState)
{
	SetActive(authState.isAuthenticated());
}
