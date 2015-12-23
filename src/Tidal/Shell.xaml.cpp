//
// Shell.xaml.cpp
// Implementation of the Shell class
//

#include "pch.h"
#include "Shell.xaml.h"
#include <algorithm>
#include "Home.xaml.h"
#include "Mediators.h"
#include <localdata/GetExistingTrackImportJobQuery.h>
#include <localdata/db.h>

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
using namespace Windows::ApplicationModel::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Shell::Shell()
{
	InitializeComponent();
	_mediatorTokens.push_back(getTrackImportComplete().registerCallbackNoArg<Shell>(this, &Shell::OnTrackImportComplete));
	_mediatorTokens.push_back(getTrackImportLaunchedMediator().registerCallbackNoArg<Shell>(this, &Shell::OnTrackImportComplete));
	OnTrackImportComplete();
	auto coreAppView = CoreApplication::GetCurrentView();
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(360, 100));
	coreAppView->TitleBar->ExtendViewIntoTitleBar = true;
	Window::Current->SetTitleBar(titleBar);
	_systemNavManager = Windows::UI::Core::SystemNavigationManager::GetForCurrentView();
	_systemNavManager->BackRequested += ref new EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &Shell::OnBackRequested);
	if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Phone.PhoneContract", 1)) {
		Windows::Phone::UI::Input::HardwareButtons::BackPressed += ref new EventHandler<Windows::Phone::UI::Input::BackPressedEventArgs^>(this, &Shell::OnPhoneBackRequested);
	}
	else {
		searchOverlay->Margin = Thickness(0, 49, 0, 0);
		searchBtn->Height = 46;
	}
	if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
	{
		auto statusBar = Windows::UI::ViewManagement::StatusBar::GetForCurrentView();
		statusBar->BackgroundColor = Windows::UI::ColorHelper::FromArgb(0xFF, 0x17, 0x17, 0x17);
		statusBar->BackgroundOpacity = 1;
	}
	navFrame->Navigate(Home::typeid);
	Window::Current->Activated += ref new Windows::UI::Xaml::WindowActivatedEventHandler(this, &Tidal::Shell::OnWindowActivated);
}


void Tidal::Shell::OnToggleSplitView(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	splitView->IsPaneOpen = !splitView->IsPaneOpen;
}


void Tidal::Shell::OnSelectedMenuItemChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto menuItem = dynamic_cast<ShellMenuItem^>(menuEntriesLV->SelectedItem);
	if (menuItem && menuItem->PageType.Name != navFrame->CurrentSourcePageType.Name) {
		navFrame->Navigate(menuItem->PageType);
	}
	if (splitView->DisplayMode != SplitViewDisplayMode::Inline) {
		splitView->IsPaneOpen = false;
	}
}


void Tidal::Shell::OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	auto itemIt = std::find_if(begin(menuEntriesLV->Items), end(menuEntriesLV->Items), [type = e->SourcePageType](Platform::Object^ item) {
		return static_cast<ShellMenuItem^>(item)->Match(type);
	});
	if (itemIt == end(menuEntriesLV->Items)) {
		menuEntriesLV->SelectedItem = nullptr;
	}
	else {
		menuEntriesLV->SelectedItem = *itemIt;
	}
	_systemNavManager->AppViewBackButtonVisibility = navFrame->CanGoBack ? Windows::UI::Core::AppViewBackButtonVisibility::Visible : Windows::UI::Core::AppViewBackButtonVisibility::Collapsed;
	auto backVisibility = Windows::UI::Xaml::Visibility::Collapsed;
	if (navFrame->CanGoBack) {
		if (!Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Phone.PhoneContract", 1)) {
			backVisibility = Windows::UI::Xaml::Visibility::Visible;
		}
	}
	backButton->Visibility = backVisibility;
}

void Tidal::Shell::OnBackRequested(Platform::Object ^ sender, Windows::UI::Core::BackRequestedEventArgs ^ e)
{
	if (navFrame->CanGoBack) {
		navFrame->GoBack();
		e->Handled = true;
	}
}

void Tidal::Shell::OnPhoneBackRequested(Platform::Object ^ sender, Windows::Phone::UI::Input::BackPressedEventArgs ^ e)
{
	if (!e->Handled) {
		if (navFrame->CanGoBack) {
			navFrame->GoBack();
			e->Handled = true;
		}
	}
}




void Tidal::Shell::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Phone.PhoneContract", 1)) {
		auto occludedRect = Windows::UI::ViewManagement::StatusBar::GetForCurrentView()->OccludedRect;
		auto newSize = e->NewSize;
		if (occludedRect.Width == newSize.Width) {
			Margin = Thickness(0, occludedRect.Height, 0, 0);
		}
		else if (occludedRect.Height == newSize.Height) {
			Margin = Thickness(occludedRect.Width, 0, 0, 0);
		}
		else {
			Margin = Thickness(0);
		}
	}
}


void Tidal::Shell::OnWindowActivated(Platform::Object ^sender, Windows::UI::Core::WindowActivatedEventArgs ^e)
{
	if (e->WindowActivationState == Windows::UI::Core::CoreWindowActivationState::Deactivated) {
		titleBarContainer->Opacity = 0.5;
	}
	else {
		titleBarContainer->Opacity = 1;
	}
}



void Tidal::Shell::OnBackClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (navFrame->CanGoBack) {
		navFrame->GoBack();
	}

}


void Tidal::Shell::OnSearchButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	searchOverlay->IsPaneOpen = !searchOverlay->IsPaneOpen;
}

void Tidal::Shell::OnTrackImportComplete()
{
	LocalDB::executeAsync<localdata::CountExistingTrackImportJobsQuery>(localdata::getDb())
		.then([this](std::int64_t count) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, count]() {
			localMusicMenuItem->BadgeCount = count;
			localMusicMenuItem->BadgeVisibility = count == 0 ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
		}));
	});
}
