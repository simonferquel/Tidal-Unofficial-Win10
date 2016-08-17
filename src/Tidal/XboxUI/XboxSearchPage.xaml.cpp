//
// XboxSearchPage.xaml.cpp
// Implementation of the XboxSearchPage class
//

#include "pch.h"
#include "XboxSearchPage.xaml.h"
#include "XboxSearchResults.xaml.h"
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

XboxSearchPage::XboxSearchPage()
{
	InitializeComponent();
}


void Tidal::XboxSearchPage::OnKeydown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Accept || e->Key == Windows::System::VirtualKey::Enter) {
		e->Handled = true;
		auto text = searchBox->Text;
		Frame->Navigate(XboxSearchResults::typeid, text);
	}
}


void Tidal::XboxSearchPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto found = FindFirstTabStopDescendant(this);
	if (found) {
		found->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}
}
