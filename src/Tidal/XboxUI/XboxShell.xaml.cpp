//
// XboxShell.xaml.cpp
// Implementation of the XboxShell class
//

#include "pch.h"
#include "XboxShell.xaml.h"
#include "FocusHelper.h"
#include "../Mediators.h"
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

XboxShell::XboxShell()
{
	InitializeComponent();
}

void Tidal::XboxShell::OnImportStateChanged()
{
	LocalDB::executeAsync<localdata::CountExistingTrackImportJobsQuery>(localdata::getDb())
		.then([this](std::int64_t count) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, count]() {
			importsIndicator->Text = L"downloading " + count.ToString() + L" tracks";
			importsIndicator->Visibility = count == 0 ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
		}));
	});
}

void Tidal::XboxShell::OnKeyDown(Platform::Object ^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs ^ e)
{
	auto k = e->Key;
	if (k == Windows::System::VirtualKey::Escape && !e->Handled) {
		if (Frame->CanGoBack) {
			e->Handled = true;
			Frame->GoBack();
		}
	}
}




void Tidal::XboxShell::OnNavigated(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationEventArgs^ e)
{
	
}


void Tidal::XboxShell::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.clear();
}


void Tidal::XboxShell::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.push_back(getTrackImportComplete().registerCallbackNoArg<XboxShell>(this, &XboxShell::OnImportStateChanged));
	_mediatorTokens.push_back(getTrackImportLaunchedMediator().registerCallbackNoArg<XboxShell>(this, &XboxShell::OnImportStateChanged));
	OnImportStateChanged();
}
