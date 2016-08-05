//
// Shell.xaml.h
// Declaration of the Shell class
//

#pragma once
#include "ShellMenuItem.h"
#include "Shell.g.h"
#include "Mediator.h"
#include <stack>
#include "IPageWithPreservedState.h"
struct PageState {
	Platform::Object^ state;
};
namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Shell sealed
	{
	private:
		IPageWithPreservedState^ _currentPage;
		std::stack<PageState> _persistedPageStates;
		Windows::UI::Core::SystemNavigationManager^ _systemNavManager;
		std::vector<RegistrationToken> _mediatorTokens;
		tools::ScopedEventRegistrations _eventRegistrations;
	public:
		property Windows::UI::Xaml::Controls::Frame^ Frame{
			Windows::UI::Xaml::Controls::Frame^ get() {
				return navFrame;
			}
		}
		property Platform::Object^ CurrentPageState {Platform::Object^ get() {
			if (_persistedPageStates.empty()) {
				return nullptr;
			}
			return _persistedPageStates.top().state;
		}}
	internal:
		Shell(Platform::String^ navState, const std::stack<PageState>& persistedState);
		std::stack<PageState> SavePageStateForBackground();
	private:
		void OnToggleSplitView(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectedMenuItemChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void OnBackRequested(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void OnPhoneBackRequested(Platform::Object^sender, Windows::Phone::UI::Input::BackPressedEventArgs^ e);

		void OnWindowActivated(Platform::Object ^sender, Windows::UI::Core::WindowActivatedEventArgs ^e);

		void OnBackClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSearchButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void OnTrackImportComplete();
		void OnVisibleBoundsChanged(Windows::UI::ViewManagement::ApplicationView ^sender, Platform::Object ^args);
		void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
