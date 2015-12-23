//
// Rising.xaml.cpp
// Implementation of the Rising class
//

#include "pch.h"
#include "Rising.xaml.h"
#include "PromotionItemVM.h"
#include "AlbumResumeItemVM.h"
#include "IncrementalDataSources.h"
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

Rising::Rising()
{
	InitializeComponent();
}


concurrency::task<void> Tidal::Rising::loadAsync()
{
	selectionGV->ItemsSource = getNewsPromotionsDataSource(L"RISING");
	albumsGV->ItemsSource = getNewsAlbumsDataSource(L"rising", L"new");
	try {
		auto tracks = await getNewsTrackItemsAsync(concurrency::cancellation_token::none(), L"rising", L"new");
		tracksLV->ItemsSource = tracks;
	}
	catch (...) {
		// connectivity issue
	}
}

void Tidal::Rising::OnPromotionClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<PromotionItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::Rising::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<AlbumResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::Rising::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	loadAsync();
}


void Tidal::Rising::OnTrackClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	std::vector<api::TrackInfo> tracks;
	tracks.push_back(trackItem->trackInfo());
	getAudioService().resetPlaylistAndPlay(tracks, 0, concurrency::cancellation_token::none());
}
