//
// LocalMusic.xaml.cpp
// Implementation of the LocalMusic class
//

#include "pch.h"
#include "LocalMusic.xaml.h"
#include "IGo.h"
#include "IncrementalDataSources.h"
#include "AudioService.h"
#include <localdata/db.h>
#include <localdata/GetTrackImportQueueQuery.h>
#include <tools/StringUtils.h>
#include <api/ImageUriResolver.h>
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

LocalMusic::LocalMusic()
{
	_downloadQueue = ref new Platform::Collections::Vector<DownloadItemVM^>();
	InitializeComponent();
	albumsGV->ItemsSource = getLocalAlbumsDataSource();
	tracksLV->ItemsSource = getLocalTracksDataSource();
	playlistsGV->ItemsSource = getLocalPlaylistsDataSource();
	_mediatorTokens.push_back( getTrackImportComplete().registerCallback<LocalMusic>(this, &LocalMusic::OnTrackImportComplete));
	_mediatorTokens.push_back(getTrackImportProgress().registerCallback<LocalMusic>(this, &LocalMusic::OnTrackImportProgress));
}


concurrency::task<void> Tidal::LocalMusic::LoadAsync()
{
	auto downloads = co_await LocalDB::executeAsync<localdata::GetTrackImportQueueQuery>(localdata::getDb());
	for (localdata::track_import_job& item : *downloads) {
		auto vm = ref new DownloadItemVM();
		vm->Artist = tools::strings::toWindowsString( item.artist);
		vm->Id = item.id;
		vm->ImageUrl = api::resolveImageUri(item.cover, 80, 80);
		vm->LocalSize = item.local_size;
		vm->ProgressVisibility = item.server_size > 0 ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
		vm->ServerSize = item.server_size;
		vm->Title = tools::strings::toWindowsString(item.title);
		_downloadQueue->Append(vm);
	}
}

void Tidal::LocalMusic::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::LocalMusic::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	std::vector<api::TrackInfo> tracks;
	tracks.push_back(trackItem->trackInfo());
	getAudioService().resetPlaylistAndPlay(tracks, 0, concurrency::cancellation_token::none());
}


void Tidal::LocalMusic::PlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().playAllLocalMusicAsync();
}


void Tidal::LocalMusic::OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::LocalMusic::OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	LoadAsync();
}

void Tidal::LocalMusic::OnTrackImportComplete(const std::int64_t & id)
{
	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, id]() {
		for (auto ix = 0; ix < _downloadQueue->Size; ++ix) {
			if (_downloadQueue->GetAt(ix)->Id == id) {
				_downloadQueue->RemoveAt(ix);
				return;
			}
		}
	}));
}

void Tidal::LocalMusic::OnTrackImportProgress(const ImportProgress & progress)
{
	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, progress]() {
		for (auto ix = 0; ix < _downloadQueue->Size; ++ix) {
			if (_downloadQueue->GetAt(ix)->Id == progress.trackId) {
				_downloadQueue->GetAt(ix)->ServerSize = progress.serverSize;
				_downloadQueue->GetAt(ix)->LocalSize = progress.localSize;
				_downloadQueue->GetAt(ix)->ProgressVisibility = Windows::UI::Xaml::Visibility::Visible;
				return;
			}
		}
	}));
}
