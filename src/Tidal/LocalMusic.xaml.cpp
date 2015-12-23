//
// LocalMusic.xaml.cpp
// Implementation of the LocalMusic class
//

#include "pch.h"
#include "LocalMusic.xaml.h"
#include "IGo.h"
#include "IncrementalDataSources.h"

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

LocalMusic::LocalMusic()
{
	InitializeComponent();
	albumsGV->ItemsSource = getLocalAlbumsDataSource();
}


void Tidal::LocalMusic::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}
