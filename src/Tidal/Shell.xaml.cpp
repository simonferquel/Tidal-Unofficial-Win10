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



Shell::Shell(Platform::String^  navState, const std::stack<PageState>& persistedState) : _persistedPageStates(persistedState)
{
	InitializeComponent();
	_mediatorTokens.push_back(getTrackImportComplete().registerCallbackNoArg<Shell>(this, &Shell::OnTrackImportComplete));
	_mediatorTokens.push_back(getTrackImportLaunchedMediator().registerCallbackNoArg<Shell>(this, &Shell::OnTrackImportComplete));
	OnTrackImportComplete();
	auto coreAppView = CoreApplication::GetCurrentView();
	Windows::UI::ViewManagement::ApplicationView::GetForCurrentView()->SetPreferredMinSize(Size(360, 100));
	auto appView = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
	_eventRegistrations.push_back(tools::makeScopedEventRegistration(appView->VisibleBoundsChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::ViewManagement::ApplicationView ^, Platform::Object ^>(this, &Tidal::Shell::OnVisibleBoundsChanged),
		[appView](EventRegistrationToken token) {
		appView->VisibleBoundsChanged -= token;
	}));
	appView->SetDesiredBoundsMode(Windows::UI::ViewManagement::ApplicationViewBoundsMode::UseVisible);
	OnVisibleBoundsChanged(appView, nullptr);
	coreAppView->TitleBar->ExtendViewIntoTitleBar = true;
	Window::Current->SetTitleBar(titleBar);
	_systemNavManager = Windows::UI::Core::SystemNavigationManager::GetForCurrentView();
	_eventRegistrations.push_back(tools::makeScopedEventRegistration(_systemNavManager->BackRequested += ref new EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(this, &Shell::OnBackRequested),
		[navMgr = _systemNavManager](EventRegistrationToken token) {
		navMgr->BackRequested -= token;
	}));
	if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Phone.PhoneContract", 1)) {
		_eventRegistrations.push_back(tools::makeScopedEventRegistration(Windows::Phone::UI::Input::HardwareButtons::BackPressed += ref new EventHandler<Windows::Phone::UI::Input::BackPressedEventArgs^>(this, &Shell::OnPhoneBackRequested),
			[](EventRegistrationToken token) {
			Windows::Phone::UI::Input::HardwareButtons::BackPressed -= token;
		}));
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
	if (navState) {
	
		_eventRegistrations.push_back(tools::makeScopedEventRegistration(Loaded += ref new RoutedEventHandler([navState](Platform::Object^ s, RoutedEventArgs^) {
			auto that = dynamic_cast<Shell^>(s);
			that->navFrame->SetNavigationState(navState);
			that->_currentPage = dynamic_cast<IPageWithPreservedState^>(that->navFrame->Content);
			auto backVisibility = Windows::UI::Xaml::Visibility::Collapsed;
			if (that->navFrame->CanGoBack) {
				if (!Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Phone.PhoneContract", 1)) {
					backVisibility = Windows::UI::Xaml::Visibility::Visible;
				}
			}
			that->backButton->Visibility = backVisibility;
		}), [this](EventRegistrationToken token) {
			Loaded -= token;
		}));
	}
	else {
		navFrame->Navigate(Home::typeid);
	}
	_eventRegistrations.push_back(tools::makeScopedEventRegistration( Window::Current->Activated += ref new Windows::UI::Xaml::WindowActivatedEventHandler(this, &Tidal::Shell::OnWindowActivated),
		[](EventRegistrationToken token) {
		Window::Current->Activated -= token;
	}));
	
}


std::stack<PageState> Tidal::Shell::SavePageStateForBackground()
{
	if (_currentPage) {
		_persistedPageStates.top().state = _currentPage->GetStateToPreserve();
	}
	return _persistedPageStates;
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
	if (e->NavigationMode == NavigationMode::Back) {
		if (!_persistedPageStates.empty()) {
			_persistedPageStates.pop();
		}
	}
	else if (e->NavigationMode == NavigationMode::New || e->NavigationMode == NavigationMode::Forward) {
		if (_currentPage) {
			_persistedPageStates.top().state = _currentPage->GetStateToPreserve();
		}
		_persistedPageStates.push(PageState{});
	}

	_currentPage = dynamic_cast<IPageWithPreservedState^>(e->Content);

	auto itemIt = std::find_if(begin(menuEntriesLV->Items), end(menuEntriesLV->Items), [type = e->SourcePageType](Platform::Object^ item) {
		return static_cast<ShellMenuItem^>(item)->Match(type);
	});
	if (itemIt == end(menuEntriesLV->Items)) {
		menuEntriesLV->SelectedItem = nullptr;
	}
	else {
		menuEntriesLV->SelectedItem = *itemIt;
	}
	_systemNavManager->AppViewBackButtonVisibility = Windows::UI::Core::AppViewBackButtonVisibility::Collapsed; //navFrame->CanGoBack ? Windows::UI::Core::AppViewBackButtonVisibility::Visible : Windows::UI::Core::AppViewBackButtonVisibility::Collapsed;
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


void Tidal::Shell::OnVisibleBoundsChanged(Windows::UI::ViewManagement::ApplicationView ^sender, Platform::Object ^args)
{
	auto coreWindow = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	auto margin = ThicknessHelper::FromLengths(sender->VisibleBounds.Left - coreWindow->Bounds.Left, sender->VisibleBounds.Top- coreWindow->Bounds.Top, 0, 0);
	rootGrid->Margin = margin;
	rootGrid->Width = sender->VisibleBounds.Width;
	rootGrid->Height = sender->VisibleBounds.Height;
}


void Tidal::Shell::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->_eventRegistrations.clear();
	this->_mediatorTokens.clear();
}
