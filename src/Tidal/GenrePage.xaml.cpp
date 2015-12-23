//
// GenrePage.xaml.cpp
// Implementation of the GenrePage class
//

#include "pch.h"
#include "GenrePage.xaml.h"
#include "IncrementalDataSources.h"
#include "TrackItemVM.h"
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

concurrency::task<void> Tidal::GenrePage::LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto p = dynamic_cast<String^>(e->Parameter);
	auto decoder = ref new WwwFormUrlDecoder(p);
	genreTitleTxt->Text = decoder->GetFirstValueByName(L"name");
	bandImg->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(decoder->GetFirstValueByName(L"headingUrl")));
	bandTxt->Text = decoder->GetFirstValueByName(L"name");

	auto path = decoder->GetFirstValueByName(L"path");
	if (decoder->GetFirstValueByName(L"hasPlaylists") == L"true") {
		playlistsGV->ItemsSource = getNewsPlaylistsDataSource(L"genres", path);
	}
	else {
		unsigned int index;
		if (pivot->Items->IndexOf(playlistsPI, &index)) {
			pivot->Items->RemoveAt(index);
		}
	}

	if (decoder->GetFirstValueByName(L"hasVideos") == L"true") {
		videosGV->ItemsSource = getNewsVideosDataSource(L"genres", path);
	}
	else {
		unsigned int index;
		if (pivot->Items->IndexOf(videosPI, &index)) {
			pivot->Items->RemoveAt(index);
		}
	}
	if (decoder->GetFirstValueByName(L"hasAlbums") == L"true") {
		albumsGV->ItemsSource = getNewsAlbumsDataSource(L"genres", path);
	}
	else {
		unsigned int index;
		if (pivot->Items->IndexOf(albumsPI, &index)) {
			pivot->Items->RemoveAt(index);
		}
	}

	if (decoder->GetFirstValueByName(L"hasTracks") == L"true") {
		tracksLV->ItemsSource = await getNewsTrackItemsAsync(concurrency::cancellation_token::none(), L"genres", path);
	}
	else {
		unsigned int index;
		if (pivot->Items->IndexOf(tracksPI, &index)) {
			pivot->Items->RemoveAt(index);
		}
	}
}

void Tidal::GenrePage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	LoadAsync(e);
}

GenrePage::GenrePage()
{
	InitializeComponent();
}


void Tidal::GenrePage::OnItemClick(IGo ^ item)
{
	if (item) {
		item->Go();
	}
}

void Tidal::GenrePage::OnPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	OnItemClick(dynamic_cast<IGo^>(e->ClickedItem));
}


void Tidal::GenrePage::OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	OnItemClick(dynamic_cast<IGo^>(e->ClickedItem));

}


void Tidal::GenrePage::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	OnItemClick(dynamic_cast<IGo^>(e->ClickedItem));
}


void Tidal::GenrePage::OnTrackClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	std::vector<api::TrackInfo> tracks;
	tracks.push_back(trackItem->trackInfo());
	getAudioService().resetPlaylistAndPlay(tracks, 0, concurrency::cancellation_token::none());
}
