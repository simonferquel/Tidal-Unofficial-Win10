//
// MyMusic.xaml.cpp
// Implementation of the MyMusic class
//

#include "pch.h"
#include "MyMusic.xaml.h"
#include "IGo.h"
#include "AudioService.h"
#include "TrackItemVM.h"
#include "FavoritesService.h"
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

MyMusic::MyMusic()
{
	InitializeComponent();
}

ref class MyMusicPageState sealed {
public:
	property int SelectedPivotItemIndex;
	MyMusicPageState(int selectedPivotItemIndex) {
		SelectedPivotItemIndex = selectedPivotItemIndex;
	}
};

void Tidal::MyMusic::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	LoadAsync();
	if (e->NavigationMode == NavigationMode::Back) {
		auto state = GetCurrentPagePreservedState<MyMusicPageState>();
		if (state) {
			pivot->SelectedIndex = state->SelectedPivotItemIndex;
		}
	}
}

Platform::Object ^ Tidal::MyMusic::GetStateToPreserve()
{
	return ref new MyMusicPageState(pivot->SelectedIndex);
}

concurrency::task<void> Tidal::MyMusic::LoadAsync()
{
	co_await getFavoriteService().refreshAsync(concurrency::cancellation_token::none());
	this->albumsGV->ItemsSource = getFavoriteService().Albums();
	this->playlistsGV->ItemsSource = getFavoriteService().Playlists();
	this->myPlaylistsGV->ItemsSource = getFavoriteService().MyPlaylists();
	this->artistsGV->ItemsSource = getFavoriteService().Artists();
	auto tracks = getFavoriteService().Tracks();
	this->tracksLV->ItemsSource = tracks;
	_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
	_tracksPlaybackManager->initialize(tracks, Dispatcher);
	
}

void Tidal::MyMusic::OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::MyMusic::OnArtistClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::MyMusic::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::MyMusic::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	trackItem->PlayCommand->Execute(nullptr);
}


void Tidal::MyMusic::OnViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}

