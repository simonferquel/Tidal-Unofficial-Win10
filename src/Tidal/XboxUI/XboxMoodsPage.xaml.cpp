//
// XboxMoodsPage.xaml.cpp
// Implementation of the XboxMoodsPage class
//

#include "pch.h"
#include "XboxMoodsPage.xaml.h"
#include "../IncrementalDataSources.h"
#include "../SublistItemVM.h"
#include "XboxOnePlaylistsPage.xaml.h"


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

XboxMoodsPage::XboxMoodsPage()
{
	InitializeComponent();
}

void Tidal::XboxMoodsPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	LoadAsync();
}

concurrency::task<void> Tidal::XboxMoodsPage::LoadAsync()
{
	try {
		auto moods = co_await getSublistsAsync(concurrency::cancellation_token::none(), L"moods");
		//	auto moods = co_await moodsTask;
		auto moodsSource = ref new Platform::Collections::Vector<SublistItemVM^>();
		for (auto&& info : *moods) {
			moodsSource->Append(ref new SublistItemVM(info));
		}
		moodsGV->ItemsSource = moodsSource;
		loader->LoadingState = LoadingState::Loaded;
	}
	catch (...) {

		loader->LoadingState = LoadingState::Error;
	}
}



void Tidal::XboxMoodsPage::OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(e->ClickedItem);
	if (item) {
		Frame->Navigate(XboxOnePlaylistsPage::typeid, item);
	}
}
