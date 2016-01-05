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
class AudioService::BackgroundAudioConnection : public std::enable_shared_from_this<AudioService::BackgroundAudioConnection> {
private:
	std::mt19937 _randomGenerator;
	std::mutex _awaitersMutex;
	std::vector<concurrency::task_completion_event<void>> _pongAwaiters;
	std::map<int, concurrency::task_completion_event<void>> _ackAwaiters;
	Windows::Foundation::EventRegistrationToken _messageReceivedRegistration;
	void OnMessageReceived(Windows::Foundation::Collections::ValueSet^ data) {
		try {
			auto request = dynamic_cast<Platform::String^>(data->Lookup(L"request"));
			if (request == L"pong") {
				std::lock_guard<std::mutex> _(_awaitersMutex);
				for (auto& tce : _pongAwaiters) {
					tce.set();
				}
				_pongAwaiters.clear();
			}
			else if (request == L"ack") {
				auto requestId = dynamic_cast<Platform::IBox<int>^>(data->Lookup(L"request_id"))->Value;
				std::lock_guard<std::mutex> _(_awaitersMutex);
				auto it = _ackAwaiters.find(requestId);
				if (it != _ackAwaiters.end()) {
					it->second.set();
					_ackAwaiters.erase(it);
				}
			}
			/*	currentPlaybackValues->Insert(L"request", L"current_playback_item_changed");
	currentPlaybackValues->Insert(L"track_id", it->second.id);*/
			else if (request == L"current_playback_item_changed") {
				auto id = dynamic_cast<Platform::IBox<std::int64_t>^>(data->Lookup(L"track_id"))->Value;
				getCurrentPlaybackTrackIdMediator().raise(id);
			}
			else if (request == L"track_import_finished") {
				auto id = dynamic_cast<Platform::IBox<std::int64_t>^>(data->Lookup(L"track_id"))->Value;
				getTrackImportComplete().raise(id);
			}
			else if (request == L"track_import_progress") {
				auto id = dynamic_cast<Platform::IBox<std::int64_t>^>(data->Lookup(L"track_id"))->Value;
				auto localSize = dynamic_cast<Platform::IBox<std::int64_t>^>(data->Lookup(L"local_size"))->Value;
				auto serverSize = dynamic_cast<Platform::IBox<std::int64_t>^>(data->Lookup(L"server_size"))->Value;
				ImportProgress progress;
				progress.localSize = localSize;
				progress.serverSize = serverSize;
				progress.trackId = id;
				getTrackImportProgress().raise(progress);
			}
		}
		catch (...) {
			// connection lost
			ResetEventHandler();
		}
	}

	void ResetEventHandler() {
		Windows::Media::Playback::BackgroundMediaPlayer::Shutdown();
		Initialize();
	}
public:
	void Reset() {
		ResetEventHandler();
	}
	void Initialize() {
		try {
			_randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			std::weak_ptr<AudioService::BackgroundAudioConnection> weakThis(shared_from_this());
			_messageReceivedRegistration = Windows::Media::Playback::BackgroundMediaPlayer::MessageReceivedFromBackground += ref new Windows::Foundation::EventHandler<Windows::Media::Playback::MediaPlayerDataReceivedEventArgs ^>(
				[weakThis](Platform::Object^, Windows::Media::Playback::MediaPlayerDataReceivedEventArgs ^ args) {
				auto that = weakThis.lock();
				if (!that) {
					return;
				}
				that->OnMessageReceived(args->Data);
			});
		}
		catch (...) {
			ResetEventHandler();
		}
	}
	~BackgroundAudioConnection() {
		try {
			Windows::Media::Playback::BackgroundMediaPlayer::MessageReceivedFromBackground -= _messageReceivedRegistration;
		}
		catch (...) {

		}
	}
	concurrency::task<void> ensureConnectionActiveAsync(concurrency::cancellation_token cancelToken) {
		concurrency::task_completion_event<void> tce;
		{
			std::lock_guard<std::mutex> _(_awaitersMutex);
			_pongAwaiters.push_back(tce);
		}
		auto resultTask = concurrency::create_task(tce, cancelToken);
		for (;;) {
			try {
				auto pingValueSet = ref new Windows::Foundation::Collections::ValueSet();
				pingValueSet->Insert(L"request", L"ping");
				Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToBackground(pingValueSet);
				await (resultTask || tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(100)), cancelToken));
				if (resultTask.is_done()) {
					return;
				}
			}
			catch (concurrency::task_canceled&) {
				throw;
			}
			catch (...) {
				// connection lost
				ResetEventHandler();
			}
		}
	}

	concurrency::task<void> sendAndWaitResponseAsync(Windows::Foundation::Collections::ValueSet^ values) {
		int requestId = _randomGenerator();
		concurrency::task_completion_event<void> tce;
		{
			std::lock_guard<std::mutex> _(_awaitersMutex);
			_ackAwaiters[requestId] = tce;
		}
		values->Insert(L"request_id", requestId);

		Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToBackground(values);
		return concurrency::create_task(tce);
	}
};
AudioService::AudioService()
{
	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
}

AudioService::~AudioService()
{
}

concurrency::task<void> AudioService::resetPlaylistAndPlay(const std::vector<api::TrackInfo>& tracks, int startIndex, concurrency::cancellation_token cancelToken)
{
	std::set<std::int64_t> albumIds;
	
	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
	auto json = web::json::value::array();
	for (auto& track : tracks) {
		json[json.size()] = track.toJson();
	}
	auto jsonText = tools::strings::toWindowsString( json.serialize());
	auto file = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::OpenIfExists), cancelToken);
	await concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(file, jsonText), cancelToken);
	await _connection->ensureConnectionActiveAsync(cancelToken);
	auto resetPlayListValues = ref new Windows::Foundation::Collections::ValueSet();
	resetPlayListValues->Insert(L"request", L"reset_playlist");
	resetPlayListValues->Insert(L"index", startIndex);
	resetPlayListValues->Insert(L"play", true);
	await _connection->sendAndWaitResponseAsync(resetPlayListValues);
	
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
	_connection = nullptr;
}

void AudioService::onResuming()
{
	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
}

void AudioService::onBackgroundAudioFailureDetected()
{
	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
	else {
		_connection->Reset();
	}
}

concurrency::task<void> AudioService::wakeupDownloaderAsync(concurrency::cancellation_token cancelToken)
{
	
	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
	
	await _connection->ensureConnectionActiveAsync(cancelToken);
	auto values = ref new Windows::Foundation::Collections::ValueSet();
	values->Insert(L"request", L"wakeup_downloader");
	await _connection->sendAndWaitResponseAsync(values);
}

concurrency::task<void> AudioService::playAllLocalMusicAsync()
{
	std::set<std::int64_t> albumIds;

	if (!_connection) {
		_connection = std::make_shared<BackgroundAudioConnection>();
		_connection->Initialize();
	}
	
	await _connection->ensureConnectionActiveAsync(Concurrency::cancellation_token::none());
	auto resetPlayListValues = ref new Windows::Foundation::Collections::ValueSet();
	resetPlayListValues->Insert(L"request", L"play_all_local");
	await _connection->sendAndWaitResponseAsync(resetPlayListValues);
}

AudioService & getAudioService()
{
	static AudioService instance;
	return instance;
}
