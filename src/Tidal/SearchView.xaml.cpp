//
// SearchView.xaml.cpp
// Implementation of the SearchView class
//

#include "pch.h"
#include "SearchView.xaml.h"
#include "IncrementalDataSources.h"
#include "SearchResultGroup.h"
#include <tools/AsyncHelpers.h>
#include <tools/TimeUtils.h>
#include "IGo.h"
#include "TrackItemVM.h"
#include "AudioService.h"
#include "XamlHelpers.h"
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SearchView::SearchView()
{
	InitializeComponent();
}


concurrency::task<void> Tidal::SearchView::DoSearchAsync()
{
	_currentSearchCts.cancel();
	_currentSearchCts = concurrency::cancellation_token_source();
	auto cancelToken = _currentSearchCts.get_token();
	backToResultsLnk->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	try {
		await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(150)), cancelToken);
		auto results = await searchAllAsync(searchBox->Text, cancelToken);
		searchLV->ItemsSource = results;
	}
	catch (...) {}
}

void Tidal::SearchView::OnTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	DoSearchAsync();
}


void Tidal::SearchView::OnBackToSearchResults(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{

	DoSearchAsync();
}


void Tidal::SearchView::OnLVHeaderTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	if (fe) {
		auto gp = dynamic_cast<SearchResultGroup^>(fe->DataContext);
		if (gp) {
			backToResultsLnk->Visibility = Windows::UI::Xaml::Visibility::Visible;
			searchLV->ItemsSource = gp->Drilldown();
		}
	}
}


void Tidal::SearchView::OnSearchResultClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go) {
		go->Go();
		auto splitView = FindOwningControl<SplitView>(this);
		splitView->IsPaneOpen = false;
	}
	else {
		auto track = dynamic_cast<TrackItemVM^>(e->ClickedItem);
		if (track) {

			std::vector<api::TrackInfo> tracks;
			tracks.push_back(track->trackInfo());
			getAudioService().resetPlaylistAndPlay(tracks, 0, concurrency::cancellation_token::none());
		}
	}

}
