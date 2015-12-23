//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "tools/StringUtils.h"
#include "AuthenticationService.h"
#include "Api/GetSubscribtionQuery.h"
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
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}



concurrency::task<void> Tidal::MainPage::loginWithFacebookAsync(Platform::String^ accessToken, concurrency::cancellation_token cancelToken)
{
	auto& svc = getAuthenticationService();
	await svc.authenticateWithFacebookAsync(accessToken, cancelToken);
	
}

void Tidal::MainPage::OnNavStarting(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ args)
{
	if (tools::strings::startsWith(args->Uri->AbsoluteUri, L"http://listen.tidal.com")) {
		auto fragment = std::wstring( args->Uri->Fragment->Begin()+1);
		auto parts = tools::strings::split(fragment, L'&');
		auto accessTokenPartIt = std::find_if(parts.begin(), parts.end(), [](const std::wstring& p) {
			static const auto toFind = L"access_token="s;
			if (p.size() < toFind.size()) {
				return false;
			}
			return std::equal(p.begin(), p.begin() + toFind.size(), toFind.begin(), toFind.end());
		});
		if (accessTokenPartIt != parts.end()) {
			auto kvp = tools::strings::split(*accessTokenPartIt, L'=');
			loginWithFacebookAsync(Uri::UnescapeComponent(tools::strings::toWindowsString(kvp[1])), concurrency::cancellation_token::none());
		}
	}
}


void Tidal::MainPage::OnWvLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	auto& svc = getAuthenticationService();
	if (svc.authenticationState().isAuthenticated()) {
		unsigned int wvIndex;
		rootGrid->Children->IndexOf(wv, &wvIndex);
		rootGrid->Children->RemoveAt(wvIndex);
	}
	else {
		wv->Navigate(ref new Windows::Foundation::Uri(L"https://www.facebook.com/v2.2/dialog/oauth?app_id=185717988231456&redirect_uri=http://listen.tidal.com&response_type=token,signed_request"));
	}
}
