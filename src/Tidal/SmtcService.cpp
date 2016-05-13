#include "pch.h"
#include "SmtcService.h"
#include "Mediators.h"
#include "AudioService.h"
#include <Api/TrackInfo.h>
#include <tools/StringUtils.h>
#include <Api/CoverCache.h>
void SmtcService::OnCurrentTrackChanged()
{
	if (!_dispatcher->HasThreadAccess) {
		_dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
			OnCurrentTrackChanged();
		}));
		return;
	}
	auto smtc = Windows::Media::SystemMediaTransportControls::GetForCurrentView();
	auto player = getAudioService().player();
	if (player->CurrentState == Windows::Media::Playback::MediaPlayerState::Playing
		|| player->CurrentState == Windows::Media::Playback::MediaPlayerState::Opening
		|| player->CurrentState == Windows::Media::Playback::MediaPlayerState::Buffering) {
		smtc->IsPlayEnabled = false;
		smtc->IsPauseEnabled = true;
		smtc->PlaybackStatus = Windows::Media::MediaPlaybackStatus::Playing;
	}
	else {
		smtc->IsPauseEnabled = false;
		smtc->IsPlayEnabled = true;

		smtc->PlaybackStatus = Windows::Media::MediaPlaybackStatus::Paused;
	}

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;

	bool canGoNext = false;
	bool canGoBack = false;
	if (settingsValues->HasKey(L"CurrentPlaybackTrackCanGoBack")) {
		canGoBack = dynamic_cast<Platform::IBox<bool>^>(settingsValues->Lookup(L"CurrentPlaybackTrackCanGoBack"))->Value;
	}
	if (settingsValues->HasKey(L"CurrentPlaybackTrackCanGoNext")) {
		canGoNext = dynamic_cast<Platform::IBox<bool>^>(settingsValues->Lookup(L"CurrentPlaybackTrackCanGoNext"))->Value;
	}

	if (settingsValues->HasKey(L"CurrentPlaybackTrack")) {
		auto json = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"CurrentPlaybackTrack"));
		tools::strings::WindowsWIStream stream(json);
		auto jval = web::json::value::parse(stream);
		api::TrackInfo trackInfo(jval);
		auto coverUri = ref new Windows::Foundation::Uri(api::getOfflineCoverUrl(trackInfo.album.id));

		smtc->DisplayUpdater->ClearAll();
		smtc->DisplayUpdater->Type = Windows::Media::MediaPlaybackType::Music;
		smtc->DisplayUpdater->MusicProperties->Artist = tools::strings::toWindowsString(trackInfo.artists[0].name);
		smtc->DisplayUpdater->MusicProperties->AlbumTitle = tools::strings::toWindowsString(trackInfo.album.title);
		smtc->DisplayUpdater->MusicProperties->Title = tools::strings::toWindowsString(trackInfo.title);
		smtc->DisplayUpdater->Thumbnail = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(coverUri);

		smtc->DisplayUpdater->AppMediaId = trackInfo.id.ToString();
		smtc->DisplayUpdater->Update();
	}

	smtc->IsNextEnabled = canGoNext;
	smtc->IsPreviousEnabled = canGoBack;

}

SmtcService::SmtcService(Windows::UI::Core::CoreDispatcher^ dispatcher) : _dispatcher(dispatcher)
{
	_mediatorTokens.push_back(getCurrentPlaybackTrackIdMediator().registerCallback([this](std::int64_t id) {this->OnCurrentTrackChanged(); }));
	auto player = getAudioService().player();
	auto token = player->CurrentStateChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlayer ^, Platform::Object ^>([this](Windows::Media::Playback::MediaPlayer ^ player, Platform::Object ^) {
		this->OnCurrentTrackChanged();
	});
	_eventRegistrations.push_back(tools::makeScopedEventRegistration(token, [player](const Windows::Foundation::EventRegistrationToken& token) {
		player->CurrentStateChanged -= token;
	}));

	auto smtc = Windows::Media::SystemMediaTransportControls::GetForCurrentView();
	auto buttonToken = smtc->ButtonPressed += ref new Windows::Foundation::TypedEventHandler<Windows::Media::SystemMediaTransportControls ^, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^>(&OnSmtcButtonPressed);

	_eventRegistrations.push_back(tools::makeScopedEventRegistration(buttonToken, [smtc](const Windows::Foundation::EventRegistrationToken& token) {
		smtc->ButtonPressed -= token;
	}));
	OnCurrentTrackChanged();
}


void SmtcService::OnSmtcButtonPressed(Windows::Media::SystemMediaTransportControls ^sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs ^args)
{
	switch (args->Button)
	{
	case Windows::Media::SystemMediaTransportControlsButton::Play:
		getAudioService().resumeAsync();
		break;
	case Windows::Media::SystemMediaTransportControlsButton::Pause:
		getAudioService().player()->Pause();
		break;
	case Windows::Media::SystemMediaTransportControlsButton::Next:
		getAudioService().nextAsync();
		break;
	case Windows::Media::SystemMediaTransportControlsButton::Previous:
		getAudioService().previousAsync();
		break;
	default:
		break;
	}
}
