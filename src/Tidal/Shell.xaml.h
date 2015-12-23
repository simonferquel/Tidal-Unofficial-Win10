//
// Shell.xaml.h
// Declaration of the Shell class
//

#pragma once
#include "ShellMenuItem.h"
#include "Shell.g.h"
#include "Mediator.h"
namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Shell sealed
	{
	private:
		Windows::UI::Core::SystemNavigationManager^ _systemNavManager;
		std::vector<RegistrationToken> _mediatorTokens;
	public:
		Shell();
		property Windows::UI::Xaml::Controls::Frame^ Frame{
			Windows::UI::Xaml::Controls::Frame^ get() {
				return navFrame;
			}
		}
	private:
		void OnToggleSplitView(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectedMenuItemChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
		void OnBackRequested(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ e);
		void OnPhoneBackRequested(Platform::Object^sender, Windows::Phone::UI::Input::BackPressedEventArgs^ e);

		void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnWindowActivated(Platform::Object ^sender, Windows::UI::Core::WindowActivatedEventArgs ^e);

		void OnBackClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSearchButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void OnTrackImportComplete();
	};
}
