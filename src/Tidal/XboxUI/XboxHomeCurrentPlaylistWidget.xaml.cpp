//
// XboxHomeCurrentPlaylistWidget.xaml.cpp
// Implementation of the XboxHomeCurrentPlaylistWidget class
//

#include "pch.h"
#include "XboxHomeCurrentPlaylistWidget.xaml.h"
#include "AudioService.h"
#include "Mediators.h"
#include <Api/CoverCache.h>
#include <tools/StringUtils.h>
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
using namespace Platform::Collections;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

XboxHomeCurrentPlaylistWidget::XboxHomeCurrentPlaylistWidget()
{
	InitializeComponent();
}


void Tidal::XboxHomeCurrentPlaylistWidget::OnCurrentTrackPlaybackChanged()
{
	if (!Dispatcher->HasThreadAccess) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
			OnCurrentTrackPlaybackChanged();
		}));
		return;
	}

	LoadCurrentTrackInfoAsync();
	
}

void Tidal::XboxHomeCurrentPlaylistWidget::OnCurrentPlaylistChanged()
{
	if (!Dispatcher->HasThreadAccess) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
			OnCurrentPlaylistChanged();
		}));
		return;
	}
	LoadAsync(_loadCts.get_token());
}

void Tidal::XboxHomeCurrentPlaylistWidget::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.push_back(getCurrentPlaybackTrackIdMediator().registerCallbackNoArg<XboxHomeCurrentPlaylistWidget>(this, &XboxHomeCurrentPlaylistWidget::OnCurrentTrackPlaybackChanged));
	_mediatorTokens.push_back(getCurrentPlaylistMediator().registerCallbackNoArg<XboxHomeCurrentPlaylistWidget>(this, &XboxHomeCurrentPlaylistWidget::OnCurrentPlaylistChanged));
	_loadCts = concurrency::cancellation_token_source();
	LoadAsync(_loadCts.get_token());
	LoadCurrentTrackInfoAsync();
}


void Tidal::XboxHomeCurrentPlaylistWidget::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_loadCts.cancel();
	_mediatorTokens.clear();
	_tracksPlaybackStateMgr = nullptr;
}

concurrency::task<void> Tidal::XboxHomeCurrentPlaylistWidget::LoadAsync(concurrency::cancellation_token cancelToken)
{
	auto& audio = getAudioService();
	auto playlist = co_await audio.getCurrentPlaylistAsync();
	if (cancelToken.is_canceled()) {
		return;
	}

	if (!playlist || playlist->size() == 0) {
		VisualStateManager::GoToState(this, L"PlaylistEmpty", false);
	}
	else {
		auto vms = ref new Vector<TrackItemVM^>();
		for (const auto& item : *playlist) {
			auto itemVM = ref new TrackItemVM(item);
			vms->Append(itemVM);
			itemVM->AttachTo(vms);
		}
		_tracksPlaybackStateMgr = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackStateMgr->initialize(vms, Dispatcher);
		lv->ItemsSource = vms;
		VisualStateManager::GoToState(this, L"PlaylistNotEmpty", false);
	}
	loadingView->LoadingState = LoadingState::Loaded;
}

concurrency::task<void> Tidal::XboxHomeCurrentPlaylistWidget::LoadCurrentTrackInfoAsync()
{
	auto& audio = getAudioService();
	auto currentTrackId = audio.getCurrentPlaybackTrackId();
	if (currentTrackId == -1) {
		return;
	}

	auto currentTrack = audio.getCurrentPlaybackTrack();
	auto cover = await api::EnsureCoverInCacheAsync(currentTrack.album.id, tools::strings::toWindowsString(currentTrack.album.cover), concurrency::cancellation_token::none());

	auto coverUri = ref new Uri(cover);
	currentArtworkImg->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(coverUri);
	currentTrackTitle->Text = tools::strings::toWindowsString(currentTrack.title);
	currentTrackArtist->Text = tools::strings::toWindowsString(currentTrack.artists[0].name);
}


void Tidal::XboxHomeCurrentPlaylistWidget::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto trk = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	auto player = getAudioService().player();
	if (trk == nullptr) {
		return;
	}
	unsigned int ix = 0;
	lv->Items->IndexOf(e->ClickedItem, &ix);
	getAudioService().moveToIndex(ix, concurrency::cancellation_token::none());
	getAudioService().resumeAsync();


}
