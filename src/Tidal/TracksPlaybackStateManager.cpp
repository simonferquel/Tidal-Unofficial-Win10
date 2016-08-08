#include "pch.h"
#include "TracksPlaybackStateManager.h"
#include "Mediators.h"
#include "AudioService.h"
concurrency::task<void> TracksPlaybackStateManager::onAppSuspended()
{
	_eventRegistrations.clear();
	return concurrency::task_from_result();
}
void TracksPlaybackStateManager::onAppResuming()
{
	registerPlayerEvents();
	reevaluateTracksPlayingStates();
}
void TracksPlaybackStateManager::onPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^ sender, Platform::Object ^ args)
{
	reevaluateTracksPlayingStates();
}
void TracksPlaybackStateManager::reevaluateTracksPlayingStates()
{
	if (!_dispatcher->HasThreadAccess) {
		std::weak_ptr<TracksPlaybackStateManager> weakThis(shared_from_this());
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([weakThis]() {
			auto that = weakThis.lock();
			if (that) {
				that->reevaluateTracksPlayingStates();
			}
		}));
		return;
	}
	auto tracks = _tracks;
	if (!tracks) {
		return;
	}
	auto trackId = getAudioService().getCurrentPlaybackTrackId();
	try {
		auto state = getAudioService().player()->CurrentState;
		for (auto&& track : tracks) {
			track->RefreshPlayingState(trackId, state);
		}
	}
	catch (Platform::COMException^ comEx) {
		if (comEx->HResult == 0x800706BA) {
			_eventRegistrations.clear();
			registerPlayerEvents();
			return;
		}
		throw;
	}
}
void TracksPlaybackStateManager::registerPlayerEvents()
{
	try {
		auto player = getAudioService().player();
		std::weak_ptr<TracksPlaybackStateManager> weakThis(shared_from_this());
		auto token = player->CurrentStateChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlayer ^, Platform::Object ^>([weakThis](Windows::Media::Playback::MediaPlayer ^ sender, Platform::Object ^ args) {
			auto that = weakThis.lock();
			if (that) {
				that->onPlayerStateChanged(sender, args);
			}
		});
		_eventRegistrations.push_back(tools::makeScopedEventRegistration(token, [player](const Windows::Foundation::EventRegistrationToken& token) {
			try {
				player->CurrentStateChanged -= token;
			}
			catch (...) {}
		}));
	}
	catch (Platform::COMException^ comEx) {
		if (comEx->HResult == 0x800706BA) {
			registerPlayerEvents();
			return;
		}
		throw;
	}
}
TracksPlaybackStateManager::~TracksPlaybackStateManager()
{
}
void TracksPlaybackStateManager::initialize(Windows::Foundation::Collections::IVector<Tidal::TrackItemVM^>^ tracks, Windows::UI::Core::CoreDispatcher^ dispatcher)
{
	if (!tracks) {
		return;
	}
	_dispatcher = dispatcher;
	_tracks = tracks;
	if (_mediatorRegistrationTokens.size() > 0) {
		return;
	}


	_mediatorRegistrationTokens.push_back(getAppSuspendingMediator().registerSharedPtrCallback(shared_from_this(), &TracksPlaybackStateManager::onAppSuspended));
	_mediatorRegistrationTokens.push_back(getAppResumingMediator().registerSharedPtrCallback(shared_from_this(), &TracksPlaybackStateManager::onAppResuming));
	_mediatorRegistrationTokens.push_back(getCurrentPlaybackTrackIdMediator().registerSharedPtrCallback(shared_from_this(), &TracksPlaybackStateManager::reevaluateTracksPlayingStates));
	registerPlayerEvents();
	reevaluateTracksPlayingStates();
}
