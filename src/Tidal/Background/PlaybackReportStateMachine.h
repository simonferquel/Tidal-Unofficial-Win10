#pragma once
#include <memory>
#include <mutex>
#include <SoundQuality.h>
#include <chrono>
struct PlaybackEntry {
	std::int64_t id;
	SoundQuality quality;
	std::chrono::system_clock::time_point start;
};

class PlaybackReportStateMachine
{
private:
	bool _isPlaying = false;
	std::int64_t _currentTrackId = -1;
	SoundQuality _currentQuality = SoundQuality::Unknown;
	std::mutex _mut;
	std::unique_ptr<PlaybackEntry> _currentEntry;
	PlaybackReportStateMachine() {}
	void CommitCurrentPlaybackEntry();
public:
	void OnActiveTrackChanged(std::int64_t id, SoundQuality quality);
	void OnMediaPlayerStateChanged(Windows::Media::Playback::MediaPlayerState state);
	static PlaybackReportStateMachine& get();
};

