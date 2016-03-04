#pragma once
#include <memory>
#include "TrackItemVM.h"
#include <vector>
#include <Mediator.h>
#include <tools/ScopedEventRegistration.h>
#include <ppltasks.h>

class TracksPlaybackStateManager : public std::enable_shared_from_this<TracksPlaybackStateManager> {
private:
	Windows::UI::Core::CoreDispatcher^ _dispatcher;
	Windows::Foundation::Collections::IVector<Tidal::TrackItemVM^>^ _tracks;
	std::vector<RegistrationToken> _mediatorRegistrationTokens;
	tools::ScopedEventRegistrations _eventRegistrations;

	concurrency::task<void> onAppSuspended();
	void onAppResuming();
	void onPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args);
	void reevaluateTracksPlayingStates();
	void registerPlayerEvents();
public:
	~TracksPlaybackStateManager();
	void initialize(Windows::Foundation::Collections::IVector<Tidal::TrackItemVM^>^ tracks, Windows::UI::Core::CoreDispatcher^ dispatcher);
};