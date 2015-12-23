#include "pch.h"
#include "MusicPlayer.h"
#include <tools/StringUtils.h>
#include <tools/TimeUtils.h>
#include <tools/AsyncHelpers.h>
#include <Api/TrackInfo.h>
#include <CachedStreamProvider.h>
#include <Api/CoverCache.h>

using namespace Windows::Media::Playback;
using namespace Windows::Media;
using namespace Windows::Media::Core;
using namespace Windows::Foundation;
using namespace Platform;
using namespace std;
using namespace std::chrono;
using namespace concurrency;

using rec_lockguard = lock_guard<recursive_mutex>;
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

concurrency::task<String^> loadPlaylistJsonAsync() {
	auto file = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFileAsync(L"current_playlist.json", Windows::Storage::CreationCollisionOption::OpenIfExists));
	auto json = await concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(file));
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

					deferral->Complete();
				});
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

void showTrackDisplayInfo(const api::TrackInfo& trackInfo, bool canGoNext, bool canGoBack, SystemMediaTransportControls^ smtc) {
	smtc->IsEnabled = true;
	smtc->IsPauseEnabled = true;
	smtc->IsPlayEnabled = true;
	smtc->IsNextEnabled = canGoNext;
	smtc->IsPreviousEnabled = canGoBack;
	auto coverUri = ref new Uri(api::EnsureCoverInCacheAsync(trackInfo.album.id, tools::strings::toWindowsString(trackInfo.album.cover), concurrency::cancellation_token::none()).get());
	smtc->DisplayUpdater->Type = MediaPlaybackType::Music;
	smtc->DisplayUpdater->MusicProperties->Artist = tools::strings::toWindowsString(trackInfo.artists[0].name);
	smtc->DisplayUpdater->MusicProperties->AlbumTitle = tools::strings::toWindowsString(trackInfo.album.title);
	smtc->DisplayUpdater->MusicProperties->Title = tools::strings::toWindowsString(trackInfo.title);
	smtc->DisplayUpdater->Thumbnail = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(coverUri);
	smtc->DisplayUpdater->Update();

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	settingsValues->Insert(L"CurrentPlaybackTrackId", trackInfo.id);
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
	showTrackDisplayInfo(info, playList->CurrentItemIndex < playList->Items->Size - 1, playList->CurrentItemIndex>0, BackgroundMediaPlayer::Current->SystemMediaTransportControls);

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
	auto cancelToken = _currentCts.get_token();
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
	});

}

void MusicPlayer::resetPlayqueueAndPlay(int index)
{
	rec_lockguard _(_mutex);
	_currentCts.cancel();
	_currentCts = cancellation_token_source();
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
			that->_currentPlayList = playList;
			that->_wmp->Source = playList;
		}
	});
}

void MusicPlayer::playAtIndex(int index)
{
	rec_lockguard _(_mutex);
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
		_currentPlayList->MoveNext();
	}
}

void MusicPlayer::movePrevious()
{
	if (_currentPlayList) {
		_currentPlayList->MovePrevious();
	}
}

void MusicPlayer::resume()
{
	auto p = _wmp;
	if (p) {
		p->Play();
	}
}

void MusicPlayer::pause()
{
	auto p = _wmp;
	if (p) {
		p->Pause();
	}
}

MusicPlayer::~MusicPlayer()
{
}
