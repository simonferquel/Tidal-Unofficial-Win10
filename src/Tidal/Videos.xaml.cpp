//
// Videos.xaml.cpp
// Implementation of the Videos class
//

#include "pch.h"
#include "IncrementalDataSources.h"
#include "SublistFilterControl.xaml.h"
#include "Videos.xaml.h"
#include "IGo.h"
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

ref class VideosPageState sealed {
public:
	property int SelectedFilterIndex;
	VideosPageState(int selectedFilterIndex) {
		SelectedFilterIndex = selectedFilterIndex;
	}
};

void Tidal::Videos::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	LoadAsync(e->NavigationMode == NavigationMode::Back);
}

Platform::Object ^ Tidal::Videos::GetStateToPreserve()
{
	return ref new VideosPageState(videosFilter->SelectedIndex);
}

Videos::Videos()
{
	InitializeComponent();
}


concurrency::task<void> Tidal::Videos::LoadAsync(bool reloadPreservedState)
{
	try {
		auto videoSublists = ref new Platform::Collections::Vector<Tidal::SublistItemVM^>();

		auto allLists = co_await getSublistsAsync(concurrency::cancellation_token::none());
		for (auto&& info : *allLists) {
			if (info.hasVideos) {
				auto item = ref new SublistItemVM(info);

				videoSublists->Append(item);
			}
		}
		videosFilter->SublistSource = videoSublists;
		if (reloadPreservedState) {
			auto state = GetCurrentPagePreservedState<VideosPageState>();
			if (state) {
				videosFilter->SelectedIndex = state->SelectedFilterIndex;
			}
		}
	}
	catch (...) {

	}
}

void Tidal::Videos::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void Tidal::Videos::OnVideoFilterChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e)
{

	videosGV->ItemsSource = getNewsVideosDataSource(L"featured", e->Path);
}


void Tidal::Videos::OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}
