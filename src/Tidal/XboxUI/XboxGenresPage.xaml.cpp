//
// XboxGenresPage.xaml.cpp
// Implementation of the XboxGenresPage class
//

#include "pch.h"
#include "XboxGenresPage.xaml.h"
#include "../IncrementalDataSources.h"
#include "../SublistItemVM.h"
#include "XboxArticleListPage.xaml.h"

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

concurrency::task<void> Tidal::XboxGenresPage::LoadAsync()
{
	try {
		auto moods = co_await getSublistsAsync(concurrency::cancellation_token::none(), L"genres");
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

XboxGenresPage::XboxGenresPage()
{
	InitializeComponent();
}

void Tidal::XboxGenresPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	LoadAsync();
}

void Tidal::XboxGenresPage::OnItemClick(Platform::Object ^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs ^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(e->ClickedItem);
	if (item) {
		Frame->Navigate(XboxArticleListPage::typeid, item);
	}
}
