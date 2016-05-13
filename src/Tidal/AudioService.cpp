#include "pch.h"
#include "AudioService.h"
#include <mutex>
#include <tools/AsyncHelpers.h>
#include <tools/TimeUtils.h>
#include <tools/StringUtils.h>
#include <random>
#include <map>
#include <set>
#include <Api/CoverCache.h>
#include "Background\BackgroundDownloader.h"
#include "Background\MusicPlayer.h"
struct AudioService::BackgroundState {
	std::shared_ptr<BackgroundDownloader> _downloader;
	std::shared_ptr<MusicPlayer> _player;
	void Initialize() {
		_downloader = std::make_shared<BackgroundDownloader>();
		_player = std::make_shared<MusicPlayer>();
		_downloader->initialize();
		_player->initialize();
	}
	void Shutdown() {
		_downloader->shutdown();
		_player->shutdown();
	}
};
AudioService::AudioService()
{
	if (!_bgState) {
		_bgState = std::make_unique<BackgroundState>();
		_bgState->Initialize();
	}
}

AudioService::~AudioService()
{
	if (_bgState) {
		_bgState->Shutdown();
		_bgState = nullptr;
	}
}

concurrency::task<void> AudioService::resetPlaylistAndPlay(const std::vector<api::TrackInfo>& tracks, int startIndex, concurrency::cancellation_token cancelToken)
{
	std::set<std::int64_t> albumIds;
	
	
	auto json = web::json::value::array();
	for (auto& track : tracks) {
		json[json.size()] = track.toJson();
	}
	auto jsonText = tools::strings::toWindowsString( json.serialize());
	auto file = co_await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::ReplaceExisting), cancelToken);
	co_await concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(file, jsonText, Windows::Storage::Streams::UnicodeEncoding::Utf8), cancelToken);
	_bgState->_player->resetPlayqueueAndPlay(startIndex);

	
}

concurrency::task<void> AudioService::moveToIndex(int startIndex, concurrency::cancellation_token cancelToken)
{
	_bgState->_player->playAtIndex(startIndex);
	return concurrency::task_from_result();
}

concurrency::task<std::shared_ptr<std::vector<api::TrackInfo>>> AudioService::getCurrentPlaylistAsync()
{
	auto file = co_await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::OpenIfExists));
	auto text = co_await concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(file));
	auto result = std::make_shared<std::vector<api::TrackInfo>>();
	if (text->Length() > 0) {	
		tools::strings::WindowsWIStream stream(text);
		auto jsonVal = web::json::value::parse(stream);
		for (auto&& item : jsonVal.as_array()) {
			result->push_back(api::TrackInfo(item));
		}
	}
	return result;
}

std::int64_t AudioService::getCurrentPlaybackTrackId() const
{

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	if (!settingsValues->HasKey(L"CurrentPlaybackTrackId")) {
		return -1;
	}
	auto box = dynamic_cast<Platform::IBox<std::int64_t>^> (settingsValues->Lookup(L"CurrentPlaybackTrackId"));
	if (!box) {
		return -1;
	}
	return box->Value;
}

void AudioService::onSuspending()
{
}

void AudioService::onResuming()
{
	
}


concurrency::task<void> AudioService::wakeupDownloaderAsync(concurrency::cancellation_token cancelToken)
{
	_bgState->_downloader->wakeup();
	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::playAllLocalMusicAsync()
{
	std::set<std::int64_t> albumIds;

	_bgState->_player->playAllLocalMusic();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::resumeAsync()
{
	_bgState->_player->resume();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::pauseAsync()
{
	_bgState->_player->pause();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::nextAsync()
{
	_bgState->_player->moveNext();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::previousAsync()
{
	_bgState->_player->movePrevious();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::shuffleChangeAsync()
{
	_bgState->_player->onShuffleModeChanged();

	return concurrency::task_from_result();
}

concurrency::task<void> AudioService::repeatChangeAsync()
{
	_bgState->_player->onRepeatModeChanged();

	return concurrency::task_from_result();
}

Windows::Media::Playback::MediaPlayer ^ AudioService::player()
{
	return _bgState->_player->Wmp();
}

AudioService & getAudioService()
{
	static std::unique_ptr<AudioService> instance;
	if (!instance) {
		instance.reset(new AudioService());
	}
	return *instance;
}
