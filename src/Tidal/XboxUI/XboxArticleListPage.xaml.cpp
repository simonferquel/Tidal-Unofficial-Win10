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
#include "SublistItemVM.h"
#include <tools/AsyncHelpers.h>
#include <tools/TimeUtils.h>
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
	if (p) {
		title->Text = p->Title;
		selectionGV->ItemsSource = getNewsPromotionsDataSource(p->PromotionListName);
		LoadAsync(p->ListName, L"new");
		return;
	}
	auto genre = dynamic_cast<SublistItemVM^>(args->Parameter);
	if (genre) {
		secSelection->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		title->Text = genre->Name;
		LoadAsync(L"genres", genre->Path);
	}
}


concurrency::task<void> Tidal::XboxArticleListPage::LoadAsync(Platform::String^ listName, Platform::String^ group)
{
	
	


	auto subLists = co_await getSublistsAsync(concurrency::cancellation_token::none(), listName);
	while (!subLists->empty() && subLists->at(0).path != group->Data()) {
		subLists->erase(subLists->begin());
	}
	
	if (subLists->empty() || !subLists->at(0).hasAlbums) {
		unsigned int idx;
		hub->Items->IndexOf(secAlbums, &idx);
		hub->Items->RemoveAt(idx);
	}
	else {
		albumsGV->ItemsSource = getNewsAlbumsDataSource(listName, group);
	}
	if (subLists->empty() || !subLists->at(0).hasPlaylists) {
		unsigned int idx;
		hub->Items->IndexOf(secPlaylists, &idx);
		hub->Items->RemoveAt(idx);
	}
	else {
		playlistsGV->ItemsSource = getNewsPlaylistsDataSource(listName, group);
	}
	if (subLists->empty() || !subLists->at(0).hasVideos) {
		unsigned int idx;
		hub->Items->IndexOf(secVideos, &idx);
		hub->Items->RemoveAt(idx);
	}

	else {
		videosGV->ItemsSource = getNewsVideosDataSource(listName, group);
	}
	
	if (subLists->empty() || !subLists->at(0).hasTracks) {
		unsigned int idx;
		hub->Items->IndexOf(secTracks, &idx);
		hub->Items->RemoveAt(idx);
	}
	else {

		auto tracks = co_await getNewsTrackItemsAsync(concurrency::cancellation_token::none(),
			listName, group);
		tracksLV->ItemsSource = tracks;
		_tpsm = std::make_shared<TracksPlaybackStateManager>();
		_tpsm->initialize(tracks, Dispatcher);
	}
	co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(100)), concurrency::cancellation_token::none());
	if (listName == L"genres") {
	//	hub->SelectedIndex = 2;
		hub->Items->RemoveAt(0);
		hub->SelectedIndex = 0;
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
