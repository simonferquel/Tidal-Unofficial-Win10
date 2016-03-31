//
// CurrentPlaylistPage.xaml.cpp
// Implementation of the CurrentPlaylistPage class
//

#include "pch.h"
#include "CurrentPlaylistPage.xaml.h"
#include "AudioService.h"
#include "TrackItemVM.h"
#include "Mediators.h"
#include <Api/CoverCache.h>
#include <tools/StringUtils.h>
#include <robuffer.h>
#include "AnimationHelpers.h"
#include <wrl.h>
#include <tools/TimeUtils.h>

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

CurrentPlaylistPage::CurrentPlaylistPage()
{
	InitializeComponent();
	_mediatorRegistrations.push_back(getAppSuspendingMediator().registerCallbackNoArg<CurrentPlaylistPage>(this, &CurrentPlaylistPage::OnAppSuspended));
	_mediatorRegistrations.push_back(getAppResumingMediator().registerCallbackNoArg<CurrentPlaylistPage>(this, &CurrentPlaylistPage::OnAppResuming));
	_mediatorRegistrations.push_back(getCurrentPlaybackTrackIdMediator().registerCallbackNoArg<CurrentPlaylistPage>(this, &CurrentPlaylistPage::ReevaluateTracksPlayingStates));
	AttachToPlayerEvents();
}

void Tidal::CurrentPlaylistPage::AttachToPlayerEvents()
{
	try {
		auto player = Windows::Media::Playback::BackgroundMediaPlayer::Current;
		auto token = player->CurrentStateChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Media::Playback::MediaPlayer ^, Platform::Object ^>(this, &Tidal::CurrentPlaylistPage::OnPlayerStateChanged);
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

void Tidal::CurrentPlaylistPage::DettachFromPlayerEvents()
{
	_eventRegistrations.clear();
}

void Tidal::CurrentPlaylistPage::ReevaluateTracksPlayingStates()
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
		if (track->Id == trackId) {
			if (_currentAlbumId != track->trackInfo().album.id) {
				_currentAlbumImageCts.cancel();
				_currentAlbumImageCts = concurrency::cancellation_token_source();
				TransitionBackground(track->trackInfo().album.id, track->trackInfo().album.cover, _currentAlbumImageCts.get_token());
			}
		}
	}
}

concurrency::task<void> Tidal::CurrentPlaylistPage::OnAppSuspended()
{
	DettachFromPlayerEvents();
	return concurrency::task_from_result();
}

void Tidal::CurrentPlaylistPage::OnAppResuming()
{
	AttachToPlayerEvents();

	ReevaluateTracksPlayingStates();
}

void Tidal::CurrentPlaylistPage::OnPlayerStateChanged(Windows::Media::Playback::MediaPlayer ^ sender, Platform::Object ^ args)
{
	ReevaluateTracksPlayingStates();
}


concurrency::task<void> Tidal::CurrentPlaylistPage::LoadAsync()
{
	auto items = co_await getAudioService().getCurrentPlaylistAsync();
	_tracks = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	for (auto&& item : *items) {
		_tracks->Append(ref new TrackItemVM(item));
	}
	ReevaluateTracksPlayingStates();
	tracksLV->ItemsSource = _tracks;
}

concurrency::task<void> Tidal::CurrentPlaylistPage::TransitionBackground(std::int64_t albumId, const std::wstring & cover, concurrency::cancellation_token cancelToken)
{
	_currentAlbumId = albumId;
	try {
		auto url = co_await api::GetCoverUriAndFallbackToWebAsync(albumId, tools::strings::toWindowsString(cover), 1080, 1080, cancelToken);
		auto device = Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice();
		auto bmp = co_await concurrency::create_task(Microsoft::Graphics::Canvas::CanvasBitmap::LoadAsync(device, ref new Uri(url), 96), cancelToken);
		auto canvas = ref new Microsoft::Graphics::Canvas::CanvasRenderTarget(device, 1080, 1080, 96);
		{
			auto session = canvas->CreateDrawingSession();
			auto blur = ref new Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect();
			blur->BlurAmount = 10;
			blur->Source = bmp;
			blur->BorderMode = Microsoft::Graphics::Canvas::Effects::EffectBorderMode::Hard;

			auto darken = ref new Microsoft::Graphics::Canvas::Effects::ExposureEffect();
			darken->Exposure = -1.5;
			darken->Source = blur;
			auto targetSize = canvas->Size;
			auto srcSize = bmp->GetBounds(session);
			auto targetAspectRatio = targetSize.Width / targetSize.Height;
			auto srcAspectRatio = srcSize.Width / srcSize.Height;
			if (targetAspectRatio > srcAspectRatio) {
				auto h = srcSize.Width / targetAspectRatio;
				session->DrawImage(darken, Rect(0, 0, targetSize.Width, targetSize.Height), Rect(0, (srcSize.Height - h)*.5f, srcSize.Width, h));
			}
			else {
				auto w = srcSize.Height*targetAspectRatio;
				session->DrawImage(darken, Rect(0, 0, targetSize.Width, targetSize.Height), Rect((srcSize.Width - w)*.5f, 0, w, srcSize.Height));
			}
		
		}
		auto bytes = canvas->GetPixelBytes();
		auto xamlBmp = ref new Windows::UI::Xaml::Media::Imaging::WriteableBitmap(canvas->Description.Width, canvas->Description.Height);
		auto buffer = xamlBmp->PixelBuffer;
		Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferBA;
		reinterpret_cast<IInspectable*>(buffer)->QueryInterface<Windows::Storage::Streams::IBufferByteAccess>(&bufferBA);
		byte* outputBytes;
		bufferBA->Buffer(&outputBytes);
		memcpy_s(outputBytes, buffer->Capacity, bytes->Data, bytes->Length);

		xamlBmp->Invalidate();
		auto img = ref new Image();
		img->Source = xamlBmp;
		img->Stretch = Stretch::UniformToFill;
		img->Opacity = 0;
		imgHolder->Children->Append( img);
		if (imgHolder->Children->Size > 2) {
			imgHolder->Children->RemoveAt(0);
		}
		AnimateTo(img, L"Opacity", 1, tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(200)));
	}
	catch (...) {}
}

void Tidal::CurrentPlaylistPage::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	if (trackItem) {

		auto trackId = getAudioService().getCurrentPlaybackTrackId();
		auto state = Windows::Media::Playback::BackgroundMediaPlayer::Current->CurrentState;
		if (state == Windows::Media::Playback::MediaPlayerState::Paused && trackId == trackItem->Id) {
			Windows::Media::Playback::BackgroundMediaPlayer::Current->Play();
			return;
		}
		else if (state == Windows::Media::Playback::MediaPlayerState::Playing && trackId == trackItem->Id) {

			Windows::Media::Playback::BackgroundMediaPlayer::Current->Pause();
			return;
		}
		if (_tracks) {
			
			unsigned int index = 0;
			_tracks->IndexOf(trackItem, &index);
			getAudioService().moveToIndex( index, concurrency::cancellation_token::none());
		}
	}
}


void Tidal::CurrentPlaylistPage::OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	LoadAsync();
}
