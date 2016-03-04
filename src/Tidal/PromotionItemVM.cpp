#include "pch.h"
#include "PromotionItemVM.h"
#include "Api/PromotionArticle.h"
#include "tools/StringUtils.h"
#include "Shell.xaml.h"
#include "VideoPlayer.xaml.h"
#include "AlbumPage.xaml.h"
#include "PlaylistPage.xaml.h"
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
using namespace tools::strings;
void Tidal::PromotionItemVM::Go()
{

	if (Type == L"EXTURL") {
		Windows::System::Launcher::LaunchUriAsync(ref new Windows::Foundation::Uri(ArtifactId));
	}
	else if (Type == L"VIDEO") {
		if (!getAuthenticationService().authenticationState().isAuthenticated()) {
			showUnauthenticatedDialog();
			return;
		}
		auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
		if (shell) {
			shell->Frame->Navigate(VideoPlayer::typeid, ArtifactId);
		}
	}
	else if (Type == L"ALBUM") {
		auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
		if (shell) {
			shell->Frame->Navigate(AlbumPage::typeid, ArtifactId);
		}
	}
	else if (Type == L"PLAYLIST") {
		auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
		if (shell) {
			shell->Frame->Navigate(PlaylistPage::typeid, ArtifactId);
		}
	}
}
Tidal::PromotionItemVM::PromotionItemVM(const api::PromotionArticle & article)
{
	ArtifactId = toWindowsString(article.artifactId);
	ImageUrl = toWindowsString(article.imageURL);
	ShortHeader = toWindowsString(article.shortHeader);
	ShortSubHeader = toWindowsString(article.shortSubHeader);
	Text = toWindowsString(article.text);
	Type = toWindowsString(article.type);
	if (article.type == L"EXTURL") {
		TypeGlyph = L"^";
	}
	else if (article.type == L"PLAYLIST") {
		TypeGlyph = L"M";
	}
	else if (article.type == L"VIDEO") {
		TypeGlyph = L"9";
	}
	else if (article.type == L"ALBUM") {
		TypeGlyph = L"A";
	}

}
