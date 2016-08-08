#include "pch.h"
#include "PlaybackReportStateMachine.h"
#include <memory>
#include <mutex>
#include <Api/PlaybackReport.h>
#include <tools/StringUtils.h>
#include <localdata\InsertPlaybackReportQuery.h>
#include <Background\BackgroundDownloader.h>

std::once_flag g_playbackReportStateMachineOnce;
std::unique_ptr<PlaybackReportStateMachine> g_playbackReportStateMachine;

void PlaybackReportStateMachine::CommitCurrentPlaybackEntry() {
	if (!_currentEntry) {
		return;
	}
	api::PlaybackReport report;
	report.deliveredDuration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _currentEntry->start).count();
	report.id = _currentEntry->id;
	report.playDate = _currentEntry->start;
	report.quality = tools::strings::toStdString(toString(_currentEntry->quality));
	_currentEntry = nullptr;


	auto json = report.toJson().serialize();
	localdata::AddPlaybackReportAsync(json);
	
}
void PlaybackReportStateMachine::OnActiveTrackChanged(std::int64_t id, SoundQuality quality)
{
	std::lock_guard<std::mutex> lg(_mut);
	CommitCurrentPlaybackEntry();
	_currentTrackId = id;
	_currentQuality = quality;
	if (_isPlaying && id != -1) {
		_currentEntry = std::make_unique<PlaybackEntry>();
		_currentEntry->id = id;
		_currentEntry->quality = quality;
		_currentEntry->start = std::chrono::system_clock::now();
	}
}

void PlaybackReportStateMachine::OnMediaPlayerStateChanged(Windows::Media::Playback::MediaPlayerState state)
{
	std::lock_guard<std::mutex> lg(_mut);
	CommitCurrentPlaybackEntry();
	_isPlaying = state == Windows::Media::Playback::MediaPlayerState::Playing;
	if (_isPlaying && _currentTrackId != -1) {
		_currentEntry = std::make_unique<PlaybackEntry>();
		_currentEntry->id = _currentTrackId;
		_currentEntry->quality = _currentQuality;
		_currentEntry->start = std::chrono::system_clock::now();
	}
}

PlaybackReportStateMachine& PlaybackReportStateMachine::get() {
	std::call_once(g_playbackReportStateMachineOnce, []() {
		g_playbackReportStateMachine = std::unique_ptr<PlaybackReportStateMachine>(new PlaybackReportStateMachine());
	});
	return *g_playbackReportStateMachine;
}
