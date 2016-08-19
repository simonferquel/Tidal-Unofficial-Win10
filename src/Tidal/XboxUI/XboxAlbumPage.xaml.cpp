//
// XboxAlbumPage.xaml.cpp
// Implementation of the XboxAlbumPage class
//

#include "pch.h"
#include "XboxAlbumPage.xaml.h"
#include "../FavoritesService.h"
#include "../AlbumDataSources.h"
#include <tools/TimeUtils.h>
#include "../IncrementalDataSources.h"
#include "XboxShell.xaml.h"
#include "../XboxOneHub.h"
#include <Api/CoverCache.h>
#include "FocusHelper.h"
#include "../DownloadService.h"
#include <Api/ImageUriResolver.h>
#include "../ArtistDataSources.h"
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

concurrency::task<void> Tidal::XboxAlbumPage::LoadAsync()
{
	try {
		loader->LoadingState = Tidal::LoadingState::Loading;
		if (getFavoriteService().isAlbumFavorite(_id)) {
			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else {

			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

		}
		auto albumInfo = co_await albums::getAlbumResumeAsync(_id, concurrency::cancellation_token::none());
		_artistId = albumInfo->artist.id;
		title->Text = tools::strings::toWindowsString(albumInfo->title);
		

		std::wstring txtTracksAndDuration;
		txtTracksAndDuration.append(std::to_wstring(albumInfo->numberOfTracks));
		txtTracksAndDuration.append(L" tracks (");
		txtTracksAndDuration.append(tools::time::toStringMMSS(albumInfo->duration));
		txtTracksAndDuration.append(L")");
		auto artistInfo = co_await artists::getArtistInfoAsync(albumInfo->artist.id, concurrency::cancellation_token::none());
		auto artistAlbumsSource = getArtistAlbumsDataSource(albumInfo->artist.id);
		albumsGV->ItemsSource = artistAlbumsSource;
		auto artistImgUrl = api::resolveImageUri(artistInfo->picture, 480, 480);
		artistImg->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(artistImgUrl));
		artistNameBtn->Content = tools::strings::toWindowsString(albumInfo->artist.name);
		auto tracks = co_await albums::getAlbumTracksAsync(_id, albumInfo->numberOfTracks, concurrency::cancellation_token::none());
		auto tracksDataSource = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
		for (auto&& t : tracks->items) {
			auto ti = ref new Tidal::TrackItemVM(t, true);
			tracksDataSource->Append(ti);
			ti->AttachTo(tracksDataSource);
		}
		_tracks = tracksDataSource;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(tracksDataSource, Dispatcher);
		lv->ItemsSource = tracksDataSource;
		if (albumInfo->cover.size() > 0) {
			auto bgImageUrl = co_await api::GetCoverUriAndFallbackToWebAsync(_id, tools::strings::toWindowsString(albumInfo->cover), 1080, 1080, concurrency::cancellation_token::none());
			albumImg->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(bgImageUrl));
		}

		loader->LoadingState = Tidal::LoadingState::Loaded;
	}
	catch (...) {
		loader->LoadingState = Tidal::LoadingState::Error;
	}
}


XboxAlbumPage::XboxAlbumPage()
{
	InitializeComponent();
}

void Tidal::XboxAlbumPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto idStr = dynamic_cast<String^>(e->Parameter);
	wchar_t* _;
	std::int64_t id = std::wcstoll(idStr->Data(), &_, 10);
	_id = id;
	LoadAsync();
}


void Tidal::XboxAlbumPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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


void Tidal::XboxAlbumPage::OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_tracks && _tracks->Size >0) {
		_tracks->GetAt(0)->PlayCommand->Execute(nullptr);
	}
}


void Tidal::XboxAlbumPage::OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (getFavoriteService().isAlbumFavorite(_id)) {
		getFavoriteService().removeAlbumAsync(_id);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else {
		getFavoriteService().addAlbumAsync(_id);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
}


void Tidal::XboxAlbumPage::OnImportClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getDownloadService().StartDownloadAlbumAsync(_id);
}


void Tidal::XboxAlbumPage::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go)go->Go();
}


void Tidal::XboxAlbumPage::OnGoToArtist(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}
