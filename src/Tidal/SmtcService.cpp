#include "pch.h"
#include "SmtcService.h"
#include "Mediators.h"
#include "AudioService.h"
#include <Api/TrackInfo.h>
#include <tools/StringUtils.h>
#include <Api/CoverCache.h>
void SmtcService::OnCurrentTrackChanged()
{
	

}

SmtcService::SmtcService(Windows::UI::Core::CoreDispatcher^ dispatcher) : _dispatcher(dispatcher)
{
	_mediatorTokens.push_back(getCurrentPlaybackTrackIdMediator().registerCallback([this](std::int64_t id) {this->OnCurrentTrackChanged(); }));


	
	OnCurrentTrackChanged();
}


void SmtcService::OnSmtcButtonPressed(Windows::Media::SystemMediaTransportControls ^sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^args)
{
	
}
