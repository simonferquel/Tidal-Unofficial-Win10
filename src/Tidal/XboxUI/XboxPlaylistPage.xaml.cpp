//
// XboxPlaylistPage.xaml.cpp
// Implementation of the XboxPlaylistPage class
//

#include "pch.h"
#include "XboxPlaylistPage.xaml.h"
#include "../PlaylistsDataSources.h"
#include "../FavoritesService.h"
#include <Api/CoverCache.h>
#include "../DownloadService.h"
#include "../AuthenticationService.h"
#include "../AlbumResumeItemVM.h"
#include <set>
#include "FocusHelper.h"
#include "XboxShell.xaml.h"
#include "../XboxOneHub.h"

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

concurrency::task<void> Tidal::XboxPlaylistPage::LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ args)
{
	try {
		auto idPlat = dynamic_cast<String^>(args->Parameter);
		auto id = tools::strings::toStdString(idPlat);
		if (getFavoriteService().isPlaylistFavorite(id)) {
			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else {
			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		_playlistId = id;

		auto playlistInfo = co_await  playlists::getPlaylistAsync(id, concurrency::cancellation_token::none());
		title->Text = tools::strings::toWindowsString(playlistInfo->title);
		playlistTitle->Text = tools::strings::toWindowsString(playlistInfo->title);
		playlistCreator->Text = playlistInfo->creator.name.size() > 0 ? tools::strings::toWindowsString(playlistInfo->creator.name) : L"TIDAL";
		playlistDescription->Text = tools::strings::toWindowsString(playlistInfo->description);
		auto coverId = tools::strings::toWindowsString(playlistInfo->image);
		auto coverUrl = co_await api::GetPlaylistCoverUriAndFallbackToWebAsync(playlistInfo->uuid, coverId, 1080, 720, concurrency::cancellation_token::none());
		playlistImg->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(coverUrl));
		auto tracks = co_await playlists::getPlaylistTracksAsync(_playlistId, playlistInfo->numberOfTracks, concurrency::cancellation_token::none());
		auto tracksVM = ref new Platform::Collections::Vector<TrackItemVM^>();
		std::set<std::int64_t> knownAlbums;
		auto albums = ref new Platform::Collections::Vector<AlbumResumeItemVM^>();
		for (auto&& t : tracks->items) {
			if (knownAlbums.insert(t.album.id).second) {
				albums->Append(ref new AlbumResumeItemVM(t.album));
			}
			auto ti = ref new TrackItemVM(t);
			tracksVM->Append(ti);
			ti->AttachTo(tracksVM);
			if (playlistInfo->type == L"USER" && playlistInfo->creator.id == getAuthenticationService().authenticationState().userId()) {
				ti->attachToOwiningPlaylistId(playlistInfo->uuid);
			}
		}
		_tracks = tracksVM;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(_tracks, Dispatcher);
		this->lv->ItemsSource = _tracks;
		albumsGV->ItemsSource = albums;



		loadingView->LoadingState = LoadingState::Loaded;
	}
	catch(...){
		loadingView->LoadingState = LoadingState::Error;
	}
}

XboxPlaylistPage::XboxPlaylistPage()
{
	InitializeComponent();
}


void Tidal::XboxPlaylistPage::OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_tracks && _tracks->Size > 0) {
		_tracks->GetAt(0)->TogglePlayPause();
	}
}


void Tidal::XboxPlaylistPage::OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (getFavoriteService().isPlaylistFavorite(_playlistId)) {
		getFavoriteService().removePlaylistAsync(_playlistId);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else {
		getFavoriteService().addPlaylistAsync(_playlistId);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
}


void Tidal::XboxPlaylistPage::OnImportClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getDownloadService().StartDownloadPlaylistAsync(_playlistId);
}


void Tidal::XboxPlaylistPage::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go)go->Go();
}


void Tidal::XboxPlaylistPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto found = FindFirstTabStopDescendant(this);
	if (found) {
		found->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}

	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell)
	{
		auto anim = hub->CompositionPropertySet->Compositor->CreateExpressionAnimation(L"hub.NormalizedOffsetX");
		anim->SetReferenceParameter(L"hub", hub->CompositionPropertySet);
		shell->AnimatedBackground->SetParallaxAmountBinding(anim);
	}
}
