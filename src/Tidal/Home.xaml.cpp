//
// Home.xaml.cpp
// Implementation of the Home class
//

#include "pch.h"
#include "Home.xaml.h"
#include "IncrementalDataSources.h"
#include "tools/AsyncHelpers.h"
#include "tools/TimeUtils.h"
#include "AudioService.h"
#include "SublistItemVM.h"
#include "SublistFilterControl.xaml.h"
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

Home::Home()
{
	InitializeComponent();
	
}




void Tidal::Home::OnPromotionClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<PromotionItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::Home::OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<VideoItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}

concurrency::task<void> Tidal::Home::LoadTracksAsync()
{

	selectionGV->ItemsSource = getNewsPromotionsDataSource();
	
	try {
		auto tracks = await getNewsTrackItemsAsync(concurrency::cancellation_token::none());
		tracksLV->ItemsSource = tracks;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(tracks, Dispatcher);
		auto videoSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto albumSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto playlistSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto trackSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();

		auto allLists = await getSublistsAsync(concurrency::cancellation_token::none());
		for (auto&& info : *allLists) {
			auto item = ref new SublistItemVM(info);
			if (info.hasAlbums) {
				albumSublists->Append(item);
			}
			if (info.hasPlaylists) {
				playlistSublists->Append(item);
			}
			if (info.hasTracks) {
				trackSublists->Append(item);
			}
			if (info.hasVideos) {
				videoSublists->Append(item);
			}
		}
		videosFilter->SublistSource = videoSublists;
		albumsFilter->SublistSource = albumSublists;
		playlistsFilter->SublistSource = playlistSublists;
		tracksFilter->SublistSource = trackSublists;
	}
	catch (...) {
		// connectivity issue
	}

	
}


void Tidal::Home::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<AlbumResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}




void Tidal::Home::OnHomeLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	LoadTracksAsync();
}


void Tidal::Home::OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto playlistitem = dynamic_cast<PlaylistResumeItemVM^>(e->ClickedItem);
	if (playlistitem) {
		playlistitem->Go();
	}
}


void Tidal::Home::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	trackItem->PlayCommand->Execute(nullptr);
}


void Tidal::Home::OnVideoFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	videosGV->ItemsSource = getNewsVideosDataSource(L"featured", e->Path);
}


void Tidal::Home::OnPlaylistFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{

	playlistsGV->ItemsSource = getNewsPlaylistsDataSource(L"featured", e->Path);
}


void Tidal::Home::OnAlbumFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	albumsGV->ItemsSource = getNewsAlbumsDataSource(L"featured", e->Path);

}


void Tidal::Home::OnTrackFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	getNewsTrackItemsAsync(concurrency::cancellation_token::none(), L"featured", e->Path)
		.then([this](concurrency::task<Platform::Collections::Vector<Tidal::TrackItemVM^>^> t) {
		try {
			auto tracks = t.get();
			tracksLV->ItemsSource = tracks;

			_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
			_tracksPlaybackManager->initialize(tracks, Dispatcher);
		}
		catch (...) {
			// connectivity issue
		}
	}, concurrency::task_continuation_context::get_current_winrt_context());
	
}
