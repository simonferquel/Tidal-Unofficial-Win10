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

ref class HomePreservedState sealed{
private:
public:
	property Platform::Object^ SelectionGVItemsSource;
	property Windows::Foundation::Collections::IVector<Tidal::TrackItemVM^> ^ TracksItemsSource;
	property Windows::Foundation::Collections::IVector<Tidal::SublistItemVM^>^ VideosSubLists;
	property Windows::Foundation::Collections::IVector<Tidal::SublistItemVM^>^ AlbumsSubLists;
	property Windows::Foundation::Collections::IVector<Tidal::SublistItemVM^>^ PlaylistsSubLists;
	property Windows::Foundation::Collections::IVector<Tidal::SublistItemVM^>^ TracksSubLists;
	property int SelectedPivotItemIndex;
	property int SelectedVideosSubList;
	property int SelectedAlbumsSubList;
	property int SelectedPlaylistsSubList;
	property int SelectedTracksSubList;
};

Platform::Object ^ Tidal::Home::GetStateToPreserve()
{
	auto result = ref new HomePreservedState();
	result->SelectionGVItemsSource = selectionGV->ItemsSource;
	result->TracksItemsSource = dynamic_cast<Platform::Collections::Vector<Tidal::TrackItemVM^> ^>(tracksLV->ItemsSource);
	result->VideosSubLists = dynamic_cast<Platform::Collections::Vector<Tidal::SublistItemVM^>^>(videosFilter->SublistSource);
	result->AlbumsSubLists = dynamic_cast<Platform::Collections::Vector<Tidal::SublistItemVM^>^>(albumsFilter->SublistSource);
	result->PlaylistsSubLists = dynamic_cast<Platform::Collections::Vector<Tidal::SublistItemVM^>^>(playlistsFilter->SublistSource);
	result->TracksSubLists = dynamic_cast<Platform::Collections::Vector<Tidal::SublistItemVM^>^>(tracksFilter->SublistSource);
	result->SelectedPivotItemIndex = pivot->SelectedIndex;
	result->SelectedVideosSubList = videosFilter->SelectedIndex;
	result->SelectedAlbumsSubList = albumsFilter->SelectedIndex;
	result->SelectedPlaylistsSubList = playlistsFilter->SelectedIndex;
	result->SelectedTracksSubList = tracksFilter->SelectedIndex;
	return result;
}




void Tidal::Home::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e) {
	if (e->NavigationMode == Windows::UI::Xaml::Navigation::NavigationMode::Back) {
		_isRestoringState = true;
		auto stateObj = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content)->CurrentPageState;
		auto state = dynamic_cast<HomePreservedState^>(stateObj);
		if (!state) {
			_isRestoringState = false;
			LoadTracksAsync();
			return;
		}
		selectionGV->ItemsSource = state->SelectionGVItemsSource;
		tracksLV->ItemsSource = state->TracksItemsSource;

		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(state->TracksItemsSource, Dispatcher);
		_isRestoringState = false;
		videosFilter->SetSourceAndSelectedIndex(state->VideosSubLists, state->SelectedVideosSubList);
		albumsFilter->SetSourceAndSelectedIndex(state->AlbumsSubLists, state->SelectedAlbumsSubList);
		playlistsFilter->SetSourceAndSelectedIndex(state->PlaylistsSubLists, state->SelectedPlaylistsSubList);
		tracksFilter->SetSourceAndSelectedIndex(state->TracksSubLists, state->SelectedTracksSubList);
		pivot->SelectedIndex = state->SelectedPivotItemIndex;
	}
	else {

		LoadTracksAsync();
	}
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
	if (_isRestoringState) {
		return;
	}
	selectionGV->ItemsSource = getNewsPromotionsDataSource();
	
	try {
		auto tracks = co_await getNewsTrackItemsAsync(concurrency::cancellation_token::none());
		tracksLV->ItemsSource = tracks;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(tracks, Dispatcher);
		auto videoSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto albumSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto playlistSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();
		auto trackSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();

		auto allLists = co_await getSublistsAsync(concurrency::cancellation_token::none());
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
	if (_isRestoringState) {
		return;
	}
	videosGV->ItemsSource = getNewsVideosDataSource(L"featured", e->Path);
}


void Tidal::Home::OnPlaylistFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	if (_isRestoringState) {
		return;
	}
	playlistsGV->ItemsSource = getNewsPlaylistsDataSource(L"featured", e->Path);
}


void Tidal::Home::OnAlbumFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	if (_isRestoringState) {
		return;
	}
	albumsGV->ItemsSource = getNewsAlbumsDataSource(L"featured", e->Path);

}


void Tidal::Home::OnTrackFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{
	if (_isRestoringState) {
		return;
	}
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
