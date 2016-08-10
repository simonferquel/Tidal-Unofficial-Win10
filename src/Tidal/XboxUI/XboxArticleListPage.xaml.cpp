//
// XboxArticleListPage.xaml.cpp
// Implementation of the XboxArticleListPage class
//

#include "pch.h"
#include "XboxArticleListPage.xaml.h"
#include "XboxArticleListPageParameter.h"
#include "FocusHelper.h"
#include "../IncrementalDataSources.h"
#include "../IGo.h"
#include "../XboxOneHub.h"
#include <AudioService.h>
#include "XboxShell.xaml.h"
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

XboxArticleListPage::XboxArticleListPage()
{
	InitializeComponent();
}

void Tidal::XboxArticleListPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ args)
{
	auto p = dynamic_cast<XboxArticleListPageParameter^>(args->Parameter);
	if (!p) {
		return;
	}
	title->Text = p->Title;
	selectionGV->ItemsSource = getNewsPromotionsDataSource(p->PromotionListName);
	LoadAsync(p->ListName);
}


concurrency::task<void> Tidal::XboxArticleListPage::LoadAsync(Platform::String ^ listName)
{
	videosGV->ItemsSource = getNewsVideosDataSource(listName);
	playlistsGV->ItemsSource = getNewsPlaylistsDataSource(listName);
	albumsGV->ItemsSource = getNewsAlbumsDataSource(listName);

	auto tracks = co_await getNewsTrackItemsAsync(concurrency::cancellation_token::none(),
		listName);
	tracksLV->ItemsSource = tracks;
	_tpsm = std::make_shared<TracksPlaybackStateManager>();
	_tpsm->initialize(tracks, Dispatcher);

	auto subLists = co_await getSublistsAsync(concurrency::cancellation_token::none(), listName);
	if (!subLists->at(0).hasAlbums) {
		unsigned int idx;
		hub->Items->IndexOf(secAlbums, &idx);
		hub->Items->RemoveAt(idx);
	}
	if (!subLists->at(0).hasPlaylists) {
		unsigned int idx;
		hub->Items->IndexOf(secPlaylists, &idx);
		hub->Items->RemoveAt(idx);
	}
	if (!subLists->at(0).hasTracks) {
		unsigned int idx;
		hub->Items->IndexOf(secTracks, &idx);
		hub->Items->RemoveAt(idx);
	}
	if (!subLists->at(0).hasVideos) {
		unsigned int idx;
		hub->Items->IndexOf(secVideos, &idx);
		hub->Items->RemoveAt(idx);
	}
}

void Tidal::XboxArticleListPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto found = FindFirstTabStopDescendant(this);
	if (found) {
		found->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}

	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell)
	{
		auto anim = hub->CompositionPropertySet->Compositor->CreateExpressionAnimation(L"hub.NormalizedOffsetX");
		anim->SetReferenceParameter(L"hub", hub->CompositionPropertySet);
		shell->AnimatedBackground->SetParallaxAmountBinding(anim);
	}
}


void Tidal::XboxArticleListPage::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<IGo^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::XboxArticleListPage::OnPause(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().pauseAsync();
}


void Tidal::XboxArticleListPage::OnTogglePlayPause(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	if (!fe)return;
	auto trackItem = dynamic_cast<TrackItemVM^>(fe->DataContext);
	if (!trackItem) {
		return;
	}
	if (trackItem->PlayButtonVisibility == Windows::UI::Xaml::Visibility::Visible) {
		trackItem->PlayCommand->Execute(nullptr);
	}
	else {
		getAudioService().pauseAsync();

	}
}
