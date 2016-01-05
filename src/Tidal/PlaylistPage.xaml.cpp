//
// PlaylistPage.xaml.cpp
// Implementation of the PlaylistPage class
//

#include "pch.h"
#include "PlaylistPage.xaml.h"
#include "PlaylistsDataSources.h"
#include <tools/StringUtils.h>
#include <tools/TimeUtils.h>
#include <Api/ImageUriResolver.h>
#include "TrackItemVM.h"
#include "MenuFlyouts.h"
#include <Api/CoverCache.h>
#include "AudioService.h"
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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

PlaylistPage::PlaylistPage()
{
	InitializeComponent();
	_mediatorRegistrations.push_back(getAppSuspendingMediator().registerCallbackNoArg<PlaylistPage>(this, &PlaylistPage::OnAppSuspended));
	_mediatorRegistrations.push_back(getAppResumingMediator().registerCallbackNoArg<PlaylistPage>(this, &PlaylistPage::OnAppResuming));
	_mediatorRegistrations.push_back(getCurrentPlaybackTrackIdMediator().registerCallbackNoArg<PlaylistPage>(this, &PlaylistPage::ReevaluateTracksPlayingStates));
	AttachToPlayerEvents();
}

void Tidal::PlaylistPage::AttachToPlayerEvents()
{
	try {
		auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
		auto token = player->CurrentStateChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlayer ^, Platform::Object ^>(this, &Tidal::PlaylistPage::OnPlayerStateChanged);
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

void Tidal::PlaylistPage::DettachFromPlayerEvents()
{
	_eventRegistrations.clear();
}

void Tidal::PlaylistPage::ReevaluateTracksPlayingStates()
{
	if (!Dispatcher->HasThreadAccess) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
			this->ReevaluateTracksPlayingStates();
		}));
		return;
	}
	auto tracks = _tracks;
	if (!tracks) {
		return;
	}
	auto trackId = getAudioService().getCurrentPlaybackTrackId();
	auto state = Windows::Media::Playback::BackgroundMediaPlayer::Current->CurrentState;
	for (auto&& track : tracks) {
		track->RefreshPlayingState(trackId, state);
	}
}

concurrency::task<void> Tidal::PlaylistPage::OnAppSuspended()
{
	DettachFromPlayerEvents();
	return concurrency::task_from_result();
}

void Tidal::PlaylistPage::OnAppResuming()
{
	AttachToPlayerEvents();

	ReevaluateTracksPlayingStates();
}

void Tidal::PlaylistPage::OnPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^ sender, Platform::Object ^ args)
{
	ReevaluateTracksPlayingStates();
}


void Tidal::PlaylistPage::OnPlayFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto trackItem = dynamic_cast<TrackItemVM^>(dynamic_cast<FrameworkElement^>(sender)->DataContext);
	if (trackItem) {

		auto trackId = getAudioService().getCurrentPlaybackTrackId();
		auto state = Windows::Media::Playback::BackgroundMediaPlayer::Current->CurrentState;
		if (state == Windows::Media::Playback::MediaPlayerState::Paused && trackId == trackItem->Id) {
			Windows::Media::Playback::BackgroundMediaPlayer::Current->Play();
			return;
		}
		if (_tracks) {
			std::vector<api::TrackInfo> tracksInfo;
			for (auto&& t : _tracks) {
				tracksInfo.push_back(t->trackInfo());
			}
			unsigned int index = 0;
			_tracks->IndexOf(trackItem, &index);
			getAudioService().resetPlaylistAndPlay(tracksInfo, index, concurrency::cancellation_token::none());
		}
	}
}


void Tidal::PlaylistPage::OnPauseFromTrack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Windows::Media::Playback::BackgroundMediaPlayer::Current->Pause();
}

concurrency::task<void> Tidal::PlaylistPage::LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ args)
{
	auto idPlat = dynamic_cast<String^>(args->Parameter);
	auto id = tools::strings::toStdString(idPlat);
	_playlistId = id;
	auto playlistInfo = await  playlists::getPlaylistAsync(id, concurrency::cancellation_token::none());
	pageHeader->Text = tools::strings::toWindowsString(playlistInfo->title);
	headeTitle->Text = tools::strings::toWindowsString(playlistInfo->title);
	headerArtist->Text = playlistInfo->creator.name.size() > 0 ? tools::strings::toWindowsString(playlistInfo->creator.name) : L"TIDAL";
	headerDescription->Text = tools::strings::toWindowsString(playlistInfo->description);
	auto coverId = tools::strings::toWindowsString(playlistInfo->image);
	auto urlTask = api::GetPlaylistCoverUriAndFallbackToWebAsync(playlistInfo->uuid, coverId, 1080, 720, concurrency::cancellation_token::none());
	await urlTask.then([this, playlistInfo, id](Platform::String^ url) {;
	headerImage->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(url));
	std::wstring tracksAndDurations = std::to_wstring(playlistInfo->numberOfTracks);
	tracksAndDurations.append(L" tracks (");
	tracksAndDurations.append(tools::time::toStringMMSS(playlistInfo->duration));
	tracksAndDurations.append(L")");
	headerTracksAndDuration->Text = tools::strings::toWindowsString(tracksAndDurations);
	return playlists::getPlaylistTracksAsync(id, playlistInfo->numberOfTracks, concurrency::cancellation_token::none()).then([this, id, playlistInfo](const std::shared_ptr<api::PaginatedList<api::TrackInfo>> & tracks) {
		auto tracksVM = ref new Platform::Collections::Vector<TrackItemVM^>();
		for (auto&& t : tracks->items) {
			tracksVM->Append(ref new TrackItemVM(t));
		}
		_tracks = tracksVM;
		ReevaluateTracksPlayingStates();
		tracksLV->ItemsSource = _tracks;
	}, concurrency::task_continuation_context::get_current_winrt_context());
	
	}, concurrency::task_continuation_context::get_current_winrt_context());
}


void Tidal::PlaylistPage::OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_tracks) {
		std::vector<api::TrackInfo> tracksInfo;
		for (auto&& t : _tracks) {
			tracksInfo.push_back(t->trackInfo());
		}
		getAudioService().resetPlaylistAndPlay(tracksInfo, 0, concurrency::cancellation_token::none());
	}
}


void Tidal::PlaylistPage::OnContextMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto flyout = getPlaylistMenuFlyout(_playlistId);
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	flyout->ShowAt(fe, Point(0, fe->ActualHeight));
}
