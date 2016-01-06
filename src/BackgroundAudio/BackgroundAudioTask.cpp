#include "pch.h"
#include "BackgroundAudioTask.h"
#include <tools/StringUtils.h>
#include <tools/TimeUtils.h>
#include <Api/GetTrackStreamUrlQuery.h>
#include <Api/ImageUriResolver.h>
#include <Api/CoverCache.h>
#include <localdata/db.h>
#include <CachedStreamProvider.h>
#include <tools/AsyncHelpers.h>
void BackgroundAudio::BackgroundAudioTask::AttachToEvents()
{
	Windows::Media::Playback::BackgroundMediaPlayer::MessageReceivedFromForeground += ref new Windows::Foundation::EventHandler<Windows::Media::Playback::MediaPlayerDataReceivedEventArgs ^>(this, &BackgroundAudio::BackgroundAudioTask::OnMessageReceivedFromForeground);
	auto transportControls = Windows::Media::Playback::BackgroundMediaPlayer::Current->SystemMediaTransportControls;
	transportControls->ButtonPressed += ref new Windows::Foundation::TypedEventHandler<Windows::Media::SystemMediaTransportControls ^, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^>(this, &BackgroundAudio::BackgroundAudioTask::OnMediaTransportButtonPressed);
	
}

void BackgroundAudio::BackgroundAudioTask::DettachFromEvents()
{
}



void BackgroundAudio::BackgroundAudioTask::Run(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^taskInstance)
{
	localdata::initializeDbAsync().get();
	_taskDeferral = taskInstance->GetDeferral();
	_musicPlayer = std::make_shared<MusicPlayer>();
	_musicPlayer->initialize();
	_backgroundDownloader = std::make_shared<BackgroundDownloader>();
	_backgroundDownloader->initialize();
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	auto smtc = player->SystemMediaTransportControls;
	smtc->IsEnabled = true;
	smtc->IsPauseEnabled = true;
	smtc->IsPlayEnabled = true;
	smtc->IsNextEnabled = true;
	smtc->IsPreviousEnabled = true;

	AttachToEvents();
	taskInstance->Canceled += ref new Windows::ApplicationModel::Background::BackgroundTaskCanceledEventHandler(this, &BackgroundAudio::BackgroundAudioTask::OnCanceled);
}


void BackgroundAudio::BackgroundAudioTask::OnCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance ^sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason reason)
{
	auto r = reason;
	auto deferral = _taskDeferral.Get();
	if (deferral) {
		_musicPlayer->shutdown();
		_musicPlayer = nullptr;
		_backgroundDownloader->shutdown();
		_backgroundDownloader = nullptr;
		DettachFromEvents();
		deferral->Complete();
		_taskDeferral = nullptr;
	}
}


void BackgroundAudio::BackgroundAudioTask::OnMessageReceivedFromForeground(Platform::Object ^sender, Windows::Media::Playback::MediaPlayerDataReceivedEventArgs ^args)
{
	auto request = dynamic_cast<Platform::String^>(args->Data->Lookup(L"request"));
	Platform::IBox<int>^ requestIdBox = nullptr;
	if (args->Data->HasKey(L"request_id")) {
		requestIdBox = dynamic_cast<Platform::IBox<int>^>(args->Data->Lookup(L"request_id"));
	}
	try {
		if (request == L"reset_playlist") {
			auto startIndexBox = dynamic_cast<Platform::IBox<int>^>(args->Data->Lookup(L"index"));
			auto playBox = dynamic_cast<Platform::IBox<bool>^>(args->Data->Lookup(L"play"));
			int startIndex = 0;
			if (startIndexBox) {
				startIndex = startIndexBox->Value;
			}
			if (playBox && playBox->Value) {

				_musicPlayer->resetPlayqueueAndPlay(startIndex);
			}
			else {
				_musicPlayer->resetPlayqueue(startIndex);
			}
		}
		else if (request == L"play_all_local") {
			_musicPlayer->playAllLocalMusic();
		}
		else if (request == L"play") {
			auto startIndexBox = dynamic_cast<Platform::IBox<int>^>(args->Data->Lookup(L"index"));
			int startIndex = 0;
			if (startIndexBox) {
				startIndex = startIndexBox->Value;
			}
			_musicPlayer->playAtIndex(startIndex);
		}
		else if (request == L"pause") {
			_musicPlayer->pause();
		}
		else if (request == L"resume") {
			_musicPlayer->resume();
		}
		else if (request == L"next") {
			_musicPlayer->moveNext();
		}
		else if (request == L"previous") {
			_musicPlayer->movePrevious();
		}
		else if (request == L"shuffle_change") {
			_musicPlayer->onShuffleModeChanged();
		}
		else if (request == L"repeat_change") {
			_musicPlayer->onRepeatModeChanged();
		}
		else if (request == L"ping") {
			auto pongSet = ref new Windows::Foundation::Collections::ValueSet();
			pongSet->Insert(L"request", L"pong");
			Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToForeground(pongSet);
		}
		else if (request == L"wakeup_downloader") {
			_backgroundDownloader->wakeup();
		}
	}
	catch (...) {
		// todo
	}
	if (requestIdBox) {
		auto ackSet = ref new Windows::Foundation::Collections::ValueSet();
		ackSet->Insert(L"request", L"ack");
		ackSet->Insert(L"request_id", requestIdBox->Value);
		Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToForeground(ackSet);

	}
}


void BackgroundAudio::BackgroundAudioTask::OnMediaTransportButtonPressed(Windows::Media::SystemMediaTransportControls ^sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^args)
{
	switch (args->Button)
	{
	case Windows::Media::SystemMediaTransportControlsButton::Next:
		_musicPlayer->moveNext();
		break;

	case Windows::Media::SystemMediaTransportControlsButton::Previous:
		_musicPlayer->movePrevious();
		break;
	case Windows::Media::SystemMediaTransportControlsButton::Pause:
		_musicPlayer->pause();
		break;
	case Windows::Media::SystemMediaTransportControlsButton::Play:
		_musicPlayer->resume();
		break;
	default:
		break;
	}
}



