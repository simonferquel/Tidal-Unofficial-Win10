#include "pch.h"
#include "MusicPlayer.h"
#include <tools/StringUtils.h>
#include <tools/TimeUtils.h>
#include <tools/AsyncHelpers.h>
#include <Api/TrackInfo.h>
#include <CachedStreamProvider.h>
#include <Api/CoverCache.h>
#include <localdata/GetLocalAlbumsQuery.h>

using namespace Windows::Media::Playback;
using namespace Windows::Media;
using namespace Windows::Media::Core;
using namespace Windows::Foundation;
using namespace Platform;
using namespace std;
using namespace std::chrono;
using namespace concurrency;

using rec_lockguard = lock_guard<recursive_mutex>;

bool getRepeatMode() {
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	if (!settingsValues->HasKey(L"RepeatMode")) {
		return false;
	}
	return dynamic_cast<IBox<bool>^>(settingsValues->Lookup(L"RepeatMode"))->Value;
}

bool getShuffleMode() {
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	if (!settingsValues->HasKey(L"ShuffleMode")) {
		return false;
	}
	return dynamic_cast<IBox<bool>^>(settingsValues->Lookup(L"ShuffleMode"))->Value;
}
void MusicPlayer::onWmpStateChanged()
{
	if (_wmp->CurrentState == MediaPlayerState::Playing) {
		BackgroundMediaPlayer::Current->SystemMediaTransportControls->PlaybackStatus = MediaPlaybackStatus::Playing;
	}
	else {

		BackgroundMediaPlayer::Current->SystemMediaTransportControls->PlaybackStatus = MediaPlaybackStatus::Paused;
	}
}

void MusicPlayer::onWmpMediaOpened()
{
}

void MusicPlayer::onWmpMediaEnded()
{
}

void MusicPlayer::onWmpMediaFailed(Windows::Media::Playback::MediaPlayerFailedEventArgs ^ args)
{
}

void SetMusicStateChanging() {
	auto smtc = Windows::Media::Playback::BackgroundMediaPlayer::Current->SystemMediaTransportControls;
	smtc->PlaybackStatus = Windows::Media::MediaPlaybackStatus::Changing;
}

concurrency::task<String^> loadPlaylistJsonAsync() {
	auto file = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::OpenIfExists));
	auto json = await concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(file));
	return json;
}
concurrency::task<String^> loadAllMusicPlaylistJsonAsync() {
	
	int skip = 0;
	std::vector<std::wstring> toRandomize;
	while (true) {
		auto batch = await localdata::getImportedTracksAsync(skip, 30, concurrency::cancellation_token::none());
		if (batch->size() == 0) {
			break;
		}
		for (auto&& item : *batch) {
			
			toRandomize.push_back(item.json);
		}

		skip += 30;
	}
	std::srand(std::chrono::system_clock::now().time_since_epoch().count());
	std::random_shuffle(toRandomize.begin(), toRandomize.end());
	bool first = true;
	std::wstring result = L"[";
	for (auto& item : toRandomize) {
		if (first) {
			first = false;
		}
		else {
			result.push_back(L',');
		}
		result.append(std::move(item));
	}
	result.push_back(L']');
	auto json = tools::strings::toWindowsString(result);
	auto file = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::OpenIfExists));
	await concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(file, json));
	return json;
}


MediaBinder^ createMediaBinderForTrack(const api::TrackInfo& info) {
	auto binder = ref new MediaBinder();
	binder->Token = tools::strings::toWindowsString(info.toJson().serialize());
	binder->Binding += ref new TypedEventHandler<MediaBinder^, MediaBindingEventArgs^>([](MediaBinder^ binder, MediaBindingEventArgs^e) {

		tools::strings::WindowsWIStream stream(binder->Token);
		auto jval = web::json::value::parse(stream);
		api::TrackInfo info(jval);
		concurrency::cancellation_token_source cts;
		auto deferral = e->GetDeferral();
		e->Canceled += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Core::MediaBindingEventArgs ^, Platform::Object ^>([deferral, cts](auto, auto) {
			cts.cancel();
		});
		auto cancelToken = cts.get_token();
		tools::async::retryWithDelays([deferral, e, info, cancelToken]() {
			return api::EnsureCoverInCacheAsync(info.album.id, tools::strings::toWindowsString(info.album.cover), cancelToken)
				.then([info, deferral, e, cancelToken](String^) {
				return resolveCachedStreamAsync(info.id, cancelToken)
					.then([deferral, e, info](const cached_stream_info& info) {
					e->SetStream(info.stream, info.contentType);
					e->MediaBinder->Source->CustomProperties->Insert(L"info", e->MediaBinder->Token);
					e->MediaBinder->Source->CustomProperties->Insert(L"isImport", info.isImport);
					e->MediaBinder->Source->CustomProperties->Insert(L"quality", static_cast<std::int32_t>(info.soundQuality));

					deferral->Complete();
				});
			}).then([deferral](concurrency::task<void > t) {
				try { t.get(); }
				catch (track_unavailable_for_streaming&) {
					deferral->Complete();
				}
				catch (...) {
					deferral->Complete();
				}
			});

		}, tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(500)), cancelToken)
			.then([deferral](concurrency::task<void> t) {
			try {
				t.get();
			}

			catch (concurrency::task_canceled&) {}
			catch (Platform::OperationCanceledException^) {}
		});
	});

	return binder;
}

void showTrackDisplayInfo(const api::TrackInfo& trackInfo, bool canGoNext, bool canGoBack, SystemMediaTransportControls^ smtc, Windows::Foundation::Collections::ValueSet^ customProperties) {
	
	smtc->IsPauseEnabled = true;
	smtc->IsPlayEnabled = true;
	smtc->IsNextEnabled = canGoNext;
	smtc->IsPreviousEnabled = canGoBack;
	auto coverUri = ref new Uri(api::EnsureCoverInCacheAsync(trackInfo.album.id, tools::strings::toWindowsString(trackInfo.album.cover), concurrency::cancellation_token::none()).get());

	smtc->DisplayUpdater->ClearAll();
	smtc->DisplayUpdater->Type = MediaPlaybackType::Music;
	smtc->DisplayUpdater->MusicProperties->Artist = tools::strings::toWindowsString(trackInfo.artists[0].name);
	smtc->DisplayUpdater->MusicProperties->AlbumTitle = tools::strings::toWindowsString(trackInfo.album.title);
	smtc->DisplayUpdater->MusicProperties->Title = tools::strings::toWindowsString(trackInfo.title);
	smtc->DisplayUpdater->Thumbnail = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(coverUri);
	smtc->DisplayUpdater->AppMediaId = trackInfo.id.ToString();
	smtc->DisplayUpdater->Update();

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	settingsValues->Insert(L"CurrentPlaybackTrackId", trackInfo.id);
	settingsValues->Insert(L"CurrentPlaybackTrack", tools::strings::toWindowsString( trackInfo.toJson().serialize()));

	settingsValues->Insert(L"CurrentPlaybackTrackIsImport", customProperties->Lookup(L"isImport"));
	settingsValues->Insert(L"CurrentPlaybackTrackQuality", customProperties->Lookup(L"quality"));
	settingsValues->Insert(L"CurrentPlaybackTrackCanGoBack", canGoBack);
	settingsValues->Insert(L"CurrentPlaybackTrackCanGoNext", canGoNext);
	auto ackSet = ref new Windows::Foundation::Collections::ValueSet();
	ackSet->Insert(L"request", L"current_playback_item_changed");
	ackSet->Insert(L"track_id", trackInfo.id);
	Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToForeground(ackSet);
}
void OnCurrentPlaylistItemChanged(MediaPlaybackList^ playList, CurrentMediaPlaybackItemChangedEventArgs^ args) {
	if (!args->NewItem) {
		return;
	}

	auto json = dynamic_cast<String^>(args->NewItem->Source->CustomProperties->Lookup(L"info"));
	tools::strings::WindowsWIStream stream(json);
	auto jval = web::json::value::parse(stream);
	api::TrackInfo info(jval);
	unsigned itemIndex = playList->CurrentItemIndex;
	if (playList->ShuffleEnabled) {
		playList->ShuffledItems->IndexOf(playList->CurrentItem, &itemIndex);
	}
	showTrackDisplayInfo(info, itemIndex < playList->Items->Size - 1 || playList->AutoRepeatEnabled, itemIndex>0 || playList->AutoRepeatEnabled, BackgroundMediaPlayer::Current->SystemMediaTransportControls, args->NewItem->Source->CustomProperties);

}

void MusicPlayer::initialize()
{
	_wmp = BackgroundMediaPlayer::Current;
	_wmp->AutoPlay = false;
	_wmpStateChangedToken = _wmp->CurrentStateChanged += ref new TypedEventHandler<MediaPlayer^, Object^>([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](auto sender, auto args){
		auto that = weakThis.lock();
		if (that) {
			that->onWmpStateChanged();
		}
	});
	_wmpMediaOpenedToken = _wmp->MediaOpened += ref new TypedEventHandler<MediaPlayer^, Object^>([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](auto sender, auto args){
		auto that = weakThis.lock();
		if (that) {
			that->onWmpMediaOpened();
		}
	});
	_wmpMediaEndedToken = _wmp->MediaEnded += ref new TypedEventHandler<MediaPlayer^, Object^>([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](auto sender, auto args){
		auto that = weakThis.lock();
		if (that) {
			that->onWmpMediaEnded();
		}
	});
	_wmpMediaFailedToken = _wmp->MediaFailed += ref new TypedEventHandler<MediaPlayer^, MediaPlayerFailedEventArgs^>([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](auto sender, auto args){
		auto that = weakThis.lock();
		if (that) {
			that->onWmpMediaFailed(args);
		}
	});

	rec_lockguard _(_mutex);
	_currentCts.cancel();

	_currentCts = cancellation_token_source();
	/*auto cancelToken = _currentCts.get_token();
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	int64_t currentPlayedId = -1;
	auto currentPlayedIdBox = dynamic_cast<IBox<int64_t>^>(settingsValues->Lookup(L"CurrentPlaybackTrackId"));
	if (currentPlayedIdBox) {
		currentPlayedId = currentPlayedIdBox->Value;
	}
	loadPlaylistJsonAsync()
		.then([weakThis = weak_ptr<MusicPlayer>(shared_from_this()), currentPlayedId](Platform::String^ playlistJson){
		auto playList = ref new Windows::Media::Playback::MediaPlaybackList();
		playList->CurrentItemChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlaybackList ^, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs ^>(&OnCurrentPlaylistItemChanged);
		playList->MaxPrefetchTime = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
		if (playlistJson->Length() > 0) {
			tools::strings::WindowsWIStream stream(playlistJson);
			auto jval = web::json::value::parse(stream);
			auto& jarr = jval.as_array();
			for (auto& jitem : jarr) {

				api::TrackInfo info(jitem);
				auto item = ref new MediaPlaybackItem(MediaSource::CreateFromMediaBinder(createMediaBinderForTrack(info)));
				playList->Items->Append(item);
				if (info.id == currentPlayedId) {
					playList->StartingItem = item;
				}
			}
		}
		auto that = weakThis.lock();
		if (that) {
			that->_currentPlayList = playList;
			that->_wmp->Source = playList;
		}
	});*/

}

void MusicPlayer::resetPlayqueueAndPlay(int index)
{
	rec_lockguard _(_mutex);
	_currentCts.cancel();
	_currentCts = cancellation_token_source();
	_wmp->Source = ref new Windows::Media::Playback::MediaPlaybackList();
	_currentPlayList = nullptr;
	SetMusicStateChanging();
	auto cancelToken = _currentCts.get_token();
	loadPlaylistJsonAsync()
		.then([weakThis = weak_ptr<MusicPlayer>(shared_from_this()), index](Platform::String^ playlistJson){
		auto playList = ref new Windows::Media::Playback::MediaPlaybackList();
		playList->CurrentItemChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlaybackList ^, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs ^>(&OnCurrentPlaylistItemChanged);
		playList->MaxPrefetchTime = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
		if (playlistJson->Length() > 0) {
			tools::strings::WindowsWIStream stream(playlistJson);
			auto jval = web::json::value::parse(stream);
			auto& jarr = jval.as_array();
			for (auto& jitem : jarr) {

				api::TrackInfo info(jitem);
				auto item = ref new MediaPlaybackItem(MediaSource::CreateFromMediaBinder(createMediaBinderForTrack(info)));
				playList->Items->Append(item);
				if (playList->Items->Size == index + 1) {
					playList->StartingItem = item;
				}
			}
		}
		auto that = weakThis.lock();
		if (that) {
			playList->AutoRepeatEnabled = getRepeatMode();
			playList->ShuffleEnabled = getShuffleMode();
			that->_currentPlayList = playList;
			that->_wmp->Source = playList;
			that->_wmp->Play();
		}
	});
}

void MusicPlayer::resetPlayqueue(int index)
{
	rec_lockguard _(_mutex);
	_currentCts.cancel();
	_currentCts = cancellation_token_source();
	_wmp->Source = ref new Windows::Media::Playback::MediaPlaybackList();
	_currentPlayList = nullptr;
	SetMusicStateChanging();
	auto cancelToken = _currentCts.get_token();
	loadPlaylistJsonAsync()
		.then([weakThis = weak_ptr<MusicPlayer>(shared_from_this()), index](Platform::String^ playlistJson){
		auto playList = ref new Windows::Media::Playback::MediaPlaybackList();
		playList->CurrentItemChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlaybackList ^, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs ^>(&OnCurrentPlaylistItemChanged);
		playList->MaxPrefetchTime = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
		if (playlistJson->Length() > 0) {
			tools::strings::WindowsWIStream stream(playlistJson);
			auto jval = web::json::value::parse(stream);
			auto& jarr = jval.as_array();
			for (auto& jitem : jarr) {

				api::TrackInfo info(jitem);
				auto item = ref new MediaPlaybackItem(MediaSource::CreateFromMediaBinder(createMediaBinderForTrack(info)));
				playList->Items->Append(item);
				if (playList->Items->Size == index + 1) {
					playList->StartingItem = item;
				}
			}
		}
		auto that = weakThis.lock();
		if (that) {
			playList->AutoRepeatEnabled = getRepeatMode();
			playList->ShuffleEnabled = getShuffleMode();
			that->_currentPlayList = playList;
			that->_wmp->Source = playList;
		}
	});
}

void MusicPlayer::playAllLocalMusic()
{
	rec_lockguard _(_mutex);
	_currentCts.cancel();
	_wmp->Source = ref new Windows::Media::Playback::MediaPlaybackList();
	_currentPlayList = nullptr;
	SetMusicStateChanging();
	_currentCts = cancellation_token_source();
	auto cancelToken = _currentCts.get_token();
	loadAllMusicPlaylistJsonAsync()
		.then([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](Platform::String^ playlistJson){
		auto playList = ref new Windows::Media::Playback::MediaPlaybackList();
		playList->CurrentItemChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlaybackList ^, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs ^>(&OnCurrentPlaylistItemChanged);
		playList->MaxPrefetchTime = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
		if (playlistJson->Length() > 0) {
			tools::strings::WindowsWIStream stream(playlistJson);
			auto jval = web::json::value::parse(stream);
			auto& jarr = jval.as_array();
			for (auto& jitem : jarr) {

				api::TrackInfo info(jitem);
				auto item = ref new MediaPlaybackItem(MediaSource::CreateFromMediaBinder(createMediaBinderForTrack(info)));
				playList->Items->Append(item);
			}
		}
		auto that = weakThis.lock();
		if (that) {
			playList->AutoRepeatEnabled = getRepeatMode();
			playList->ShuffleEnabled = getShuffleMode();
			that->_currentPlayList = playList;
			that->_wmp->Source = playList;
			that->_wmp->Play();
		}
	});
}

void MusicPlayer::playAtIndex(int index)
{
	rec_lockguard _(_mutex);
	SetMusicStateChanging();
	if (_currentPlayList) {
		if (_currentPlayList->CurrentItemIndex != index) {
			_currentPlayList->MoveTo(index);
		}
		_wmp->Play();
	}
	else {
		resetPlayqueueAndPlay(index);
	}
}

void MusicPlayer::shutdown()
{
	_currentCts.cancel();

	_wmp->CurrentStateChanged -= _wmpStateChangedToken;
	_wmp->MediaOpened -= _wmpMediaOpenedToken;
	_wmp->MediaEnded -= _wmpMediaEndedToken;
	_wmp->MediaFailed -= _wmpMediaFailedToken;
}

void MusicPlayer::moveNext()
{
	if (_currentPlayList) {
		SetMusicStateChanging();
		_currentPlayList->MoveNext();
	}
}

void MusicPlayer::movePrevious()
{
	if (_currentPlayList) {
		SetMusicStateChanging();
		_currentPlayList->MovePrevious();
	}
}

void MusicPlayer::resume()
{
	if (_currentPlayList) {
		auto p = _wmp;
		if (p) {
			p->Play();
		}
	}
	else {
		rec_lockguard _(_mutex);
		_currentCts.cancel();
		_wmp->Source = ref new Windows::Media::Playback::MediaPlaybackList();
		_currentPlayList = nullptr;
		SetMusicStateChanging();
		_currentCts = cancellation_token_source();
		auto cancelToken = _currentCts.get_token();
		loadPlaylistJsonAsync()
			.then([weakThis = weak_ptr<MusicPlayer>(shared_from_this())](Platform::String^ playlistJson){

			auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
			std::int64_t resumeId = -1;
			if (settingsValues->HasKey(L"CurrentPlaybackTrackId")) {
				resumeId = dynamic_cast<IBox<std::int64_t>^>(settingsValues->Lookup(L"CurrentPlaybackTrackId"))->Value;
			}

			auto playList = ref new Windows::Media::Playback::MediaPlaybackList();
			playList->CurrentItemChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlaybackList ^, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs ^>(&OnCurrentPlaylistItemChanged);
			playList->MaxPrefetchTime = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
			if (playlistJson->Length() > 0) {
				tools::strings::WindowsWIStream stream(playlistJson);
				auto jval = web::json::value::parse(stream);
				auto& jarr = jval.as_array();
				for (auto& jitem : jarr) {

					api::TrackInfo info(jitem);
					auto item = ref new MediaPlaybackItem(MediaSource::CreateFromMediaBinder(createMediaBinderForTrack(info)));
					playList->Items->Append(item);
					if (info.id == resumeId) {
						playList->StartingItem = item;
					}
				}
			}
			auto that = weakThis.lock();
			if (that) {

				playList->AutoRepeatEnabled = getRepeatMode();
				playList->ShuffleEnabled = getShuffleMode();
				that->_currentPlayList = playList;
				that->_wmp->Source = playList;
				that->_wmp->Play();
			}
		});
	}
}

void MusicPlayer::pause()
{
	auto p = _wmp;
	if (p) {
		p->Pause();
	}
}

void MusicPlayer::onRepeatModeChanged()
{
	auto playlist = _currentPlayList;
	if (playlist) {
		playlist->AutoRepeatEnabled = getRepeatMode();
	}
}

void MusicPlayer::onShuffleModeChanged()
{
	auto playlist = _currentPlayList;
	if (playlist) {
		playlist->ShuffleEnabled = getShuffleMode();
	}
}

MusicPlayer::~MusicPlayer()
{
}
