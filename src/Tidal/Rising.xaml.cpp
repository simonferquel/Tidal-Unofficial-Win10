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
#include "Shell.xaml.h"

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



ref class RisingPageState {
public:
	property int SelectedPivotItemIndex;
};

Platform::Object ^ Tidal::Rising::GetStateToPreserve()
{
	auto result = ref new RisingPageState();
	result->SelectedPivotItemIndex = pivot->SelectedIndex;
	return result;
}

void Tidal::Rising::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	if (e->NavigationMode == NavigationMode::Back) {
		auto stateObj = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content)->CurrentPageState;
		auto state = dynamic_cast<RisingPageState^>(stateObj);
		if (state) {
			pivot->SelectedIndex = state->SelectedPivotItemIndex;
		}
	}
	loadAsync();
}

concurrency::task<void> Tidal::Rising::loadAsync()
{
	selectionGV->ItemsSource = getNewsPromotionsDataSource(L"RISING");
	albumsGV->ItemsSource = getNewsAlbumsDataSource(L"rising", L"new");
	try {
		auto tracks = co_await getNewsTrackItemsAsync(concurrency::cancellation_token::none(), L"rising", L"new");
		tracksLV->ItemsSource = tracks;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(tracks, Dispatcher);
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
}


void Tidal::Rising::OnTrackClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto trackItem = dynamic_cast<TrackItemVM^>(e->ClickedItem);
	trackItem->PlayCommand->Execute(nullptr);
}
