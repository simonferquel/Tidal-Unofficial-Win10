#include "pch.h"
#include "PlayCommand.h"
#include "UnauthenticatedDialog.h"
#include "AuthenticationService.h"
#include "AudioService.h"

using namespace Windows::UI::Xaml;
using namespace Windows::Foundation::Collections;
DependencyProperty^ Tidal::PlayCommand::_trackProperty = DependencyProperty::Register(L"Track",
	Tidal::TrackItemVM::typeid,
	Tidal::PlayCommand::typeid, ref new PropertyMetadata(nullptr));
DependencyProperty^ Tidal::PlayCommand::_trackListProperty = DependencyProperty::Register(L"TrackList",
	IIterable<Tidal::TrackItemVM^>::typeid,
	Tidal::PlayCommand::typeid, ref new PropertyMetadata(nullptr));

Tidal::PlayCommand::PlayCommand(TrackItemVM ^ singleTrack)
{
	Track = singleTrack;
	TrackList = ref new Platform::Collections::Vector<TrackItemVM^>{ singleTrack };
}

Tidal::PlayCommand::PlayCommand(TrackItemVM ^ startTrack, Windows::Foundation::Collections::IIterable<TrackItemVM^>^ trackList)
{
	Track = startTrack;
	TrackList = trackList;
}

bool Tidal::PlayCommand::CanExecute(Platform::Object ^parameter)
{
	return true;
}

void Tidal::PlayCommand::Execute(Platform::Object ^parameter)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	if (Track == nullptr || TrackList == nullptr) {
		return;
	}
	if (getAudioService().getCurrentPlaybackTrackId() == Track->Id && Windows::Media::Playback::BackgroundMediaPlayer::Current->CurrentState != Windows::Media::Playback::MediaPlayerState::Closed
		&& Windows::Media::Playback::BackgroundMediaPlayer::Current->CurrentState != Windows::Media::Playback::MediaPlayerState::Stopped) {
		try {
			Windows::Media::Playback::BackgroundMediaPlayer::Current->Play();
		}
		catch (...) {}
		return;
	}
	int startIndex = 0;
	std::vector<api::TrackInfo> tracks;
	for (auto&& t : TrackList) {
		if (t == Track) {
			startIndex = tracks.size();
		}
		tracks.push_back(t->trackInfo());
	}
	getAudioService().resetPlaylistAndPlay(tracks, startIndex, concurrency::cancellation_token::none());
}
