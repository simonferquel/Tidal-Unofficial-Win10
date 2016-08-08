//
// Playlists.xaml.cpp
// Implementation of the Playlists class
//

#include "pch.h"
#include "Playlists.xaml.h"
#include <IncrementalDataSources.h>
#include "SublistItemVM.h"
#include "SublistFilterControl.xaml.h"
#include "PlaylistResumeItemVM.h"
#include <tools/TimeUtils.h>
#include <tools/AsyncHelpers.h>
#include "XamlHelpers.h"
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



Playlists::Playlists()
{
	InitializeComponent();
}
ref class PlaylistsPageState sealed{
public:
	property int FeaturedPlaylistSelectedFilterIndex;
	property int SelectedMoodIndex;

};
Platform::Object ^ Tidal::Playlists::GetStateToPreserve()
{
	auto state = ref new PlaylistsPageState();
	state->FeaturedPlaylistSelectedFilterIndex = allPlaylistsFilter->SelectedIndex;
	state->SelectedMoodIndex = moodsGV->SelectedIndex;
	return state;
}
void Tidal::Playlists::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	if (e->NavigationMode == NavigationMode::Back) {
		LoadAsync(true);
	}
	else {
		LoadAsync(false);
	}
}


concurrency::task<void> Tidal::Playlists::LoadAsync(bool loadPreservedState)
{
	try {
		auto moods = co_await getSublistsAsync(concurrency::cancellation_token::none(), L"moods");
		auto featured = co_await getSublistsAsync(concurrency::cancellation_token::none());
		//	auto moods = co_await moodsTask;
		auto moodsSource = ref new Platform::Collections::Vector<SublistItemVM^>();
		for (auto&& info : *moods) {
			moodsSource->Append(ref new SublistItemVM(info));
		}
		moodsGV->ItemsSource = moodsSource;

		auto featuredSource = ref new Platform::Collections::Vector<SublistItemVM^>();
		for (auto&& info : *featured) {
			if (info.hasPlaylists) {
				featuredSource->Append(ref new SublistItemVM(info));
			}
		}
		allPlaylistsFilter->SublistSource = featuredSource;
		if (loadPreservedState) {
			auto state = dynamic_cast<PlaylistsPageState^>( dynamic_cast<Shell^>(Window::Current->Content)->CurrentPageState);
			if (state) {
				if (state->SelectedMoodIndex == -1 && state->FeaturedPlaylistSelectedFilterIndex != -1) {
					allPlaylistsFilter->SelectedIndex = state->FeaturedPlaylistSelectedFilterIndex;
				}
				else if (state->SelectedMoodIndex != 1) {
					moodsGV->SelectedIndex = state->SelectedMoodIndex;
				}
			}
		}
	}
	catch (...) {

	}
}



concurrency::task<void> Tidal::Playlists::LoadMoodAsync(Hat<SublistItemVM> item)
{
	try {
		auto src = getNewsPlaylistsDataSource(L"moods", item->Path);
		auto firstLoadTask = src->waitForNextLoadAsync();
		playlistGV->ItemsSource = src;
		allPlaylistsFilter->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		moodHeaderZone->Visibility = Windows::UI::Xaml::Visibility::Visible;
		moodHeaderImage->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(item->HeadingUrl));
		moodHeaderTxt->Text = item->Name;
		co_await firstLoadTask;
		co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(100)), concurrency::cancellation_token::none());
		auto sv = FindOwningScrollViewer(moodHeaderZone);
		auto ttv = moodHeaderZone->TransformToVisual(sv);
		auto offset = ttv->TransformPoint(Point(0, 0));
		sv->ChangeView(nullptr, sv->VerticalOffset + offset.Y, nullptr, false);
	}
	catch (...) {

	}
}

void Tidal::Playlists::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void Tidal::Playlists::OnAllPlaylistsFilterSelectionChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{

	playlistGV->ItemsSource = getNewsPlaylistsDataSource(L"featured", e->Path);
}

void Tidal::Playlists::OnSelectedMoodChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(moodsGV->SelectedItem);
	if (item) {
		LoadMoodAsync(item);
	}
}


void Tidal::Playlists::OnPlaylistItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<Tidal::PlaylistResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}
