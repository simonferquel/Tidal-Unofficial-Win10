//
// LoginView.xaml.cpp
// Implementation of the LoginView class
//

#include "pch.h"
#include "LoginView.xaml.h"
#include "Mediators.h"
#include "AuthenticationService.h"
#include "LoginDialog.xaml.h"
#include "FacebookLoginFlyout.xaml.h"
#include <Api/ImageUriResolver.h>
#include <tools/StringUtils.h>
#include "Shell.xaml.h"
#include "AccountPage.xaml.h"
using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

LoginView::LoginView()
{
	InitializeComponent();
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Tidal::LoginView::OnViewLoaded);
}


void Tidal::LoginView::OnViewLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	WeakReference weakThis(this);
	_mediatorToken = getAuthenticationStateMediator().registerCallback([weakThis](const AuthenticationState& state){
		auto that = weakThis.Resolve<LoginView>();
		if (!that) {
			return;
		}
		that->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([that, state]() {
			that->UpdateForAuthenticationState(state, true);
			
		}));
	});
	UpdateForAuthenticationState(getAuthenticationService().authenticationState(), false);

}

void Tidal::LoginView::UpdateForAuthenticationState(const AuthenticationState & authState, bool useTransitions)
{
	if (authState.isAuthenticated()) {

		if (authState.userPicture() && authState.userPicture()->Length() > 0) {
			auto bmp = ref new BitmapImage(ref new Uri(api::resolveImageUri(tools::strings::toStdString( authState.userPicture()), 210,210)));
			auto bmpBrush = ref new ImageBrush();
			bmpBrush->Stretch = Stretch::UniformToFill;
			bmpBrush->ImageSource = bmp;
			userPic->Fill = bmpBrush;
		}
		userName->Text = authState.userFirstName()+ L" "+authState.userLastName();
		subscribtionName->Text = L"TIDAL "+authState.subscribtionType();

		VisualStateManager::GoToState(this, L"LoggedIn", useTransitions);
	}
	else {
		VisualStateManager::GoToState(this, L"LoggedOut", useTransitions);
		userPic->Fill = nullptr;
		userName->Text = L"";
		subscribtionName->Text = L"";
	}
}

concurrency::task<void> Tidal::LoginView::loginAsync()
{
	auto dialog = ref new LoginDialog();
	await dialog->ShowAsync();
	if (dialog->DialogResult == LoginDialogResult::FacebookLoginRequested) {
		auto flyout = ref new FacebookLoginFlyout();
		flyout->ShowAt(dynamic_cast<FrameworkElement^>(Window::Current->Content));
	}
}



void Tidal::LoginView::OnLoginClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	loginAsync();
}


void Tidal::LoginView::OnViewTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (getAuthenticationService().authenticationState().isAuthenticated()) {
		auto shell = dynamic_cast<Tidal::Shell^>(Window::Current->Content);
		if (shell) {
			shell->Frame->Navigate(AccountPage::typeid);
		}
	}
}
