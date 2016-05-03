#pragma once
#include <memory>
#include <AuthenticationState.h>
#include <Hat.h>
class AuthenticationService 
{
private:
	AuthenticationState _authState;
	AuthenticationService();
public:
	const AuthenticationState& authenticationState()const { return _authState; }
	AuthenticationService(const AuthenticationService&) = delete;
	AuthenticationService(AuthenticationService&&) = delete;
	AuthenticationService& operator=(const AuthenticationService&) = delete;
	AuthenticationService& operator=(AuthenticationService&&) = delete;
	concurrency::task<void> authenticateWithFacebookAsync(Hat<Platform::String> accessToken, concurrency::cancellation_token cancelToken);
	concurrency::task<void> authenticateWithPasswordAsync(Hat<Platform::String> userName, Hat<Platform::String> password, concurrency::cancellation_token cancelToken);
	void logout();
	friend AuthenticationService& getAuthenticationService();
};

