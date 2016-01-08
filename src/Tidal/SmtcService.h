#pragma once
#include <Mediator.h>
#include <tools/ScopedEventRegistration.h>
class SmtcService {
private:
	Windows::UI::Core::CoreDispatcher^ _dispatcher;
	tools::ScopedEventRegistrations _eventRegistrations;
	std::vector<RegistrationToken> _mediatorTokens;
	void OnCurrentTrackChanged();
public:
	SmtcService(Windows::UI::Core::CoreDispatcher^ dispatcher);
	
	static void OnSmtcButtonPressed(Windows::Media::SystemMediaTransportControls ^sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^args);
};