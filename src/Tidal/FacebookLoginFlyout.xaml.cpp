//
// FacebookLoginFlyout.xaml.cpp
// Implementation of the FacebookLoginFlyout class
//

#include "pch.h"
#include "FacebookLoginFlyout.xaml.h"
#include "tools/StringUtils.h"
#include <string>
#include "AuthenticationService.h"

using namespace std;

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

FacebookLoginFlyout::FacebookLoginFlyout()
{
	InitializeComponent();
}


void Tidal::FacebookLoginFlyout::OnWebViewLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	wv->Navigate(ref new Windows::Foundation::Uri(L"https://www.facebook.com/dialog/oauth?app_id=185717988231456&redirect_uri=http://listen.tidal.com&response_type=token,signed_request&display=iframe"));
}


void Tidal::FacebookLoginFlyout::OnNavigating(Windows::UI::Xaml::Controls::WebView^ sender, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs^ args)
{
	if (tools::strings::startsWith(args->Uri->AbsoluteUri, L"http://listen.tidal.com")) {
		args->Cancel = true;
		auto fragment = std::wstring(args->Uri->Fragment->Begin() + 1);
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
			if (kvp.size() == 2) {
				getAuthenticationService().authenticateWithFacebookAsync(tools::strings::toWindowsString(kvp[1]), concurrency::cancellation_token::none());
			}
		}
		Hide();
	}
}
