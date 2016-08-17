//
// XboxSearchResults.xaml.cpp
// Implementation of the XboxSearchResults class
//

#include "pch.h"
#include "XboxSearchResults.xaml.h"
#include "../IncrementalDataSources.h"
#include "../IGo.h"
#include "XboxShell.xaml.h"
#include"../XboxOneHub.h"
#include "FocusHelper.h"
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

concurrency::task<void> Tidal::XboxSearchResults::LoadAsync(Platform::String ^ query)
{
	title->Text = L"Search: " + query;
	albumsGV->ItemsSource = getFilteredSearchSource(query, L"ALBUMS");
	artistsGV->ItemsSource = getFilteredSearchSource(query, L"ARTISTS");
	tracksLV->ItemsSource = getFilteredSearchSource(query, L"TRACKS");
	videosGV->ItemsSource = getFilteredSearchSource(query, L"VIDEOS");
	playlistsGV->ItemsSource = getFilteredSearchSource(query, L"PLAYLISTS");
	loadingView->LoadingState = LoadingState::Loaded;
	return concurrency::task<void>();
}

XboxSearchResults::XboxSearchResults()
{
	InitializeComponent();
}

void Tidal::XboxSearchResults::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto query = dynamic_cast<Platform::String^>(e->Parameter);
	if (query) {
		LoadAsync(query);
	}
}


void Tidal::XboxSearchResults::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^>(e->ClickedItem);
	if (go)go->Go();
}


void Tidal::XboxSearchResults::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
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
