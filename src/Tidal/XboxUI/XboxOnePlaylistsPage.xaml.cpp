//
// XboxOnePlaylistsPage.xaml.cpp
// Implementation of the XboxOnePlaylistsPage class
//

#include "pch.h"
#include "XboxOnePlaylistsPage.xaml.h"
#include "SublistItemVM.h"
#include "IncrementalDataSources.h"
#include "../IGo.h"
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

XboxOnePlaylistsPage::XboxOnePlaylistsPage()
{
	InitializeComponent();
}

void Tidal::XboxOnePlaylistsPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(e->Parameter);
	if (item) {
		title->Text = item->Name;
		playlistsGV->ItemsSource =  getNewsPlaylistsDataSource(L"moods", item->Path);
	}
}


void Tidal::XboxOnePlaylistsPage::OnItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go) {
		go->Go();
	}
}
