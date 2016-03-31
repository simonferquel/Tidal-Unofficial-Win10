//
// MiniPlayerView.xaml.cpp
// Implementation of the MiniPlayerView class
//

#include "pch.h"
#include "MiniPlayerView.xaml.h"
#include "AudioService.h"
#include "Mediators.h"
#include <cpprest/json.h>
#include <tools/StringUtils.h>
#include <Api/CoverCache.h>
#include "Shell.xaml.h"
#include "CurrentPlaylistPage.xaml.h"

using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

MiniPlayerView::MiniPlayerView()
{
	InitializeComponent();
	_mediatorTokens.push_back(getCurrentPlaybackTrackIdMediator().registerCallbackNoArg<MiniPlayerView>(this, &MiniPlayerView::OnCurrentTrackChanged));
	_mediatorTokens.push_back(getAppResumingMediator().registerCallbackNoArg<MiniPlayerView>(this, &MiniPlayerView::OnAppResuming));
	_mediatorTokens.push_back(getAppSuspendingMediator().registerCallbackNoArg<MiniPlayerView>(this, &MiniPlayerView::OnAppSuspending));
	AttachToPlayerEvents();
	UpdateState();
	OnCurrentTrackChanged();
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	if (settingsValues->HasKey(L"RepeatMode")) {
		toggleRepeat->IsChecked = dynamic_cast<IBox<bool>^>(settingsValues->Lookup(L"RepeatMode"));
	}
	if (settingsValues->HasKey(L"ShuffleMode")) {
		toggleRepeat->IsChecked = dynamic_cast<IBox<bool>^>(settingsValues->Lookup(L"ShuffleMode"));
	}
}

void Tidal::MiniPlayerView::UpdateAdaptiveState()
{
	if (ActualWidth >= 700) {
		narrowTimelineStateTrigger->IsActive = false;
		largeStateTrigger->IsActive = false;
		largeTimelineStateTrigger->IsActive = false;
		extraLargeStateTrigger->IsActive = true;
	}
	else if(ActualWidth >= 500){
		narrowTimelineStateTrigger->IsActive = false;
		largeStateTrigger->IsActive = !toggleView->IsChecked->Value;
		largeTimelineStateTrigger->IsActive = toggleView->IsChecked->Value;
		extraLargeStateTrigger->IsActive = false;
	}
	else {

		narrowTimelineStateTrigger->IsActive = toggleView->IsChecked->Value;
		largeStateTrigger->IsActive = false;
		largeTimelineStateTrigger->IsActive = false;
		extraLargeStateTrigger->IsActive = false;
	}
}

void Tidal::MiniPlayerView::UpdateState()
{
	if (!Dispatcher->HasThreadAccess) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {UpdateState(); }));
		return;
	}
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	if (player->CurrentState == Windows::Media::Playback::MediaPlayerState::Playing || player->CurrentState == Windows::Media::Playback::MediaPlayerState::Opening || player->CurrentState == Windows::Media::Playback::MediaPlayerState::Buffering) {
		togglePlay->IsChecked = true;
		playPauseIcon->Glyph = L"b";
	}
	else {
		togglePlay->IsChecked = false;
		playPauseIcon->Glyph = L"a";
	}
}

void Tidal::MiniPlayerView::AttachToPlayerEvents()
{
	try {
		auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
		auto token = player->CurrentStateChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlayer ^, Platform::Object ^>(this, &Tidal::MiniPlayerView::OnMediaPlayerStateChanged);
		_eventRegistrations.push_back(tools::makeScopedEventRegistration(token, [player](const Windows::Foundation::EventRegistrationToken& token) {
			try {
				player->CurrentStateChanged -= token;
			}
			catch (...) {}
		}));
	}
	catch (Platform::COMException^ comEx) {
		if (comEx->HResult == 0x800706BA) {
			getAudioService().onBackgroundAudioFailureDetected();
			AttachToPlayerEvents();
			return;
		}
		throw;
	}
}

void Tidal::MiniPlayerView::DettachFromPlayerEvents()
{
	_eventRegistrations.clear();
}

void Tidal::MiniPlayerView::OnCurrentTrackChanged()
{
	if (!Dispatcher->HasThreadAccess) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {OnCurrentTrackChanged(); }));
		return;
	}


	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	if (!settingsValues->HasKey(L"CurrentPlaybackTrack")) {
		Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		return;
	}
	Visibility = Windows::UI::Xaml::Visibility::Visible;
	auto jsonText = dynamic_cast<String^>( settingsValues->Lookup(L"CurrentPlaybackTrack"));

	tools::strings::WindowsWIStream stream(jsonText);
	auto jval = web::json::value::parse(stream);
	api::TrackInfo info(jval);

	HandleTrackInfoAsync(info);

	if (!settingsValues->HasKey(L"CurrentPlaybackTrackQuality")) {
		return;
	}

	auto qualityInt = dynamic_cast<IBox<std::int32_t>^>(settingsValues->Lookup(L"CurrentPlaybackTrackQuality"));
	qualityIndicator->Text = toString(static_cast<SoundQuality>(qualityInt->Value));

}

void Tidal::MiniPlayerView::OnAppResuming()
{
	AttachToPlayerEvents();
	UpdateState();
	OnCurrentTrackChanged();
}

concurrency::task<void> Tidal::MiniPlayerView::HandleTrackInfoAsync(const api::TrackInfo & infoRef)
{
	auto info = infoRef;
	auto url = co_await api::EnsureCoverInCacheAsync(infoRef.album.id, tools::strings::toWindowsString( infoRef.album.cover), concurrency::cancellation_token::none());
	currentItemImage->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(url));
	trackName->Text = tools::strings::toWindowsString(info.title);
	artistName->Text = tools::strings::toWindowsString(info.artists[0].name);
}

concurrency::task<void> Tidal::MiniPlayerView::OnAppSuspending()
{
	DettachFromPlayerEvents();
	return concurrency::task_from_result();
}


void Tidal::MiniPlayerView::Image_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto shell = dynamic_cast<Shell^>(Window::Current->Content);
	if (shell) {
		shell->Frame->Navigate(CurrentPlaylistPage::typeid);
	}
}


void Tidal::MiniPlayerView::OnMediaPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^sender, Platform::Object ^args)
{
	UpdateState();
}


void Tidal::MiniPlayerView::OnPlayPauseClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
	if (player->CurrentState == Windows::Media::Playback::MediaPlayerState::Playing || player->CurrentState == Windows::Media::Playback::MediaPlayerState::Opening || player->CurrentState == Windows::Media::Playback::MediaPlayerState::Buffering) {
		getAudioService().pauseAsync();
	}
	else {
		getAudioService().resumeAsync();
	}
}


void Tidal::MiniPlayerView::OnNext(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().nextAsync();
}


void Tidal::MiniPlayerView::OnPrevious(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().previousAsync();
}


void Tidal::MiniPlayerView::OnShuffleClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	settingsValues->Insert(L"ShuffleMode", toggleShuffle->IsChecked);
	getAudioService().shuffleChangeAsync();
}


void Tidal::MiniPlayerView::OnRepeatClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto v = toggleRepeat->IsChecked;
	settingsValues->Insert(L"RepeatMode", v);
	getAudioService().repeatChangeAsync();
}


void Tidal::MiniPlayerView::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	UpdateAdaptiveState();
}


void Tidal::MiniPlayerView::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void Tidal::MiniPlayerView::OnToggleChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateAdaptiveState();

}


void Tidal::MiniPlayerView::OnToggleUnchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateAdaptiveState();

}
