//
// Genres.xaml.cpp
// Implementation of the Genres class
//

#include "pch.h"
#include "Genres.xaml.h"
#include "SublistItemVM.h"
#include "IncrementalDataSources.h"
#include <tools/StringUtils.h>
#include "GenrePage.xaml.h"
#include <Api/ImageUriResolver.h>

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

concurrency::task<void> Tidal::Genres::LoadAsync()
{
	auto genres = await getSublistsAsync(concurrency::cancellation_token::none(), L"genres");
	auto genresSources = ref new Platform::Collections::Vector<SublistItemVM^>();
	for (auto&& info : *genres) {
		genresSources->Append(ref new SublistItemVM(info));
	}
	genresGV->ItemsSource = genresSources;
}

Genres::Genres()
{
	InitializeComponent();
}


void Tidal::Genres::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	LoadAsync();
}


void Tidal::Genres::OnGenreClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(e->ClickedItem);
	if (item) {
		std::wstring formdata = L"path=";
		formdata.append(Uri::EscapeComponent(tools::strings::toWindowsString( item->sublistInfo().path))->Data());
		formdata.append(L"&name=");
		formdata.append(Uri::EscapeComponent(tools::strings::toWindowsString(item->sublistInfo().name))->Data());
		formdata.append(L"&hasAlbums=");
		formdata.append(item->sublistInfo().hasAlbums ? L"true" : L"false");
		formdata.append(L"&hasArtists=");
		formdata.append(item->sublistInfo().hasArtists ? L"true" : L"false");
		formdata.append(L"&hasPlaylists=");
		formdata.append(item->sublistInfo().hasPlaylists ? L"true" : L"false");
		formdata.append(L"&hasTracks=");
		formdata.append(item->sublistInfo().hasTracks ? L"true" : L"false");
		formdata.append(L"&hasVideos=");
		formdata.append(item->sublistInfo().hasVideos ? L"true" : L"false");
		formdata.append(L"&headingUrl=");
		formdata.append(Uri::EscapeComponent(api::resolveImageUri(item->sublistInfo().image, 2048,512))->Data());
		Frame->Navigate(GenrePage::typeid, tools::strings::toWindowsString(formdata));
	}
}
