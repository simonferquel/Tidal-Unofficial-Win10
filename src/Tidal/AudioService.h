#pragma once
#include <memory>
#include <vector>
#include <Api/TrackInfo.h>
#include "Mediators.h"
class AudioService {
	class BackgroundAudioConnection;
private:
	AudioService();
	std::shared_ptr<BackgroundAudioConnection> _connection;
public:
	~AudioService();

	friend AudioService& getAudioService();
	concurrency::task<void> resetPlaylistAndPlay(const std::vector<api::TrackInfo>& tracks, int startIndex, concurrency::cancellation_token cancelToken);
	concurrency::task<void> moveToIndex( int startIndex, concurrency::cancellation_token cancelToken);
	concurrency::task<std::shared_ptr<std::vector<api::TrackInfo>>> getCurrentPlaylistAsync();
	std::int64_t getCurrentPlaybackTrackId() const;
	void onSuspending();
	void onResuming();
	void onBackgroundAudioFailureDetected();
	concurrency::task<void> wakeupDownloaderAsync(concurrency::cancellation_token cancelToken);
	concurrency::task<void> playAllLocalMusicAsync();
	concurrency::task<void> resumeAsync();
	concurrency::task<void> pauseAsync();
	concurrency::task<void> nextAsync();
	concurrency::task<void> previousAsync();

	concurrency::task<void> shuffleChangeAsync();
	concurrency::task<void> repeatChangeAsync();
};