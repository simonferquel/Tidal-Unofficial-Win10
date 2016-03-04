//
// Discovery.xaml.cpp
// Implementation of the Discovery class
//

#include "pch.h"
#include "Discovery.xaml.h"
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

Discovery::Discovery()
{
	InitializeComponent();
}


concurrency::task<void> Tidal::Discovery::loadAsync()
{
	selectionGV->ItemsSource = getNewsPromotionsDataSource(L"DISCOVERY");
	albumsGV->ItemsSource = getNewsAlbumsDataSource(L"discovery", L"new");
	try {
		auto tracks = await getNewsTrackItemsAsync(concurrency::cancellation_token::none(), L"discovery", L"new");
		tracksLV->ItemsSource = tracks;
		_playbackStateManager = std::make_shared<TracksPlaybackStateManager>();
		_playbackStateManager->initialize(tracks, Dispatcher);
	}
	catch (...) {
		// connectivity issue
	}
}

void Tidal::Discovery::OnPromotionClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<PromotionItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::Discovery::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<AlbumResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::Discovery::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	loadAsync();
}




void Tidal::Discovery::OnTrackClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	if (item) {
		item->PlayCommand->Execute(nullptr);
	}
}
