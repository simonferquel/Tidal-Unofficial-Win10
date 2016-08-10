﻿//
// XboxShell.xaml.cpp
// Implementation of the XboxShell class
//

#include "pch.h"
#include "XboxShell.xaml.h"
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

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

XboxShell::XboxShell()
{
	InitializeComponent();
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
