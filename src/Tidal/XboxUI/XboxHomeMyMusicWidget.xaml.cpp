//
// XboxHomeMyMusicWidgetxaml.xaml.cpp
// Implementation of the XboxHomeMyMusicWidgetxaml class
//

#include "pch.h"
#include "XboxHomeMyMusicWidget.xaml.h"
#include "AudioService.h"
#include "XboxArticleListPage.xaml.h"
#include "XboxArticleListPageParameter.h"
#include "XboxShell.xaml.h"
#include "XboxOnePlaylistsPage.xaml.h"
#include "XboxDownloadsPage.xaml.h"
#include <localdata/db.h>
#include <localdata/GetExistingTrackImportJobQuery.h>
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

Tidal::XboxHomeMyMusicWidget::XboxHomeMyMusicWidget()
{
	InitializeComponent();
}


concurrency::task<void> Tidal::XboxHomeMyMusicWidget::UpdateDownloadsTile()
{
	return LocalDB::executeAsync<localdata::CountExistingTrackImportJobsQuery>(localdata::getDb())
		.then([this](std::int64_t count) {
		Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, count]() {
			btnDownloads->Visibility = count == 0 ? Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
			badgeDownloads->Text = count.ToString();
		}));
	});

}

void Tidal::XboxHomeMyMusicWidget::OnTrackImportComplete(const std::int64_t & id)
{
	UpdateDownloadsTile();

}

void Tidal::XboxHomeMyMusicWidget::OnTrackImportProgress(const ImportProgress & progress)
{
}

void Tidal::XboxHomeMyMusicWidget::OnPlayAll(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	getAudioService().playAllLocalMusicAsync();
}


void Tidal::XboxHomeMyMusicWidget::OnFavorites(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		auto p = ref new XboxArticleListPageParameter();
		p->IsFavorites = true;
		shell->Frame->Navigate(XboxArticleListPage::typeid, p);
	}
}


void Tidal::XboxHomeMyMusicWidget::OnGoToLocal(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		auto p = ref new XboxArticleListPageParameter();
		p->IsLocalMusic = true;
		shell->Frame->Navigate(XboxArticleListPage::typeid, p);
	}
}


void Tidal::XboxHomeMyMusicWidget::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.push_back(getTrackImportComplete().registerCallback<XboxHomeMyMusicWidget>(this, &XboxHomeMyMusicWidget::OnTrackImportComplete));
	_mediatorTokens.push_back(getTrackImportProgress().registerCallback<XboxHomeMyMusicWidget>(this, &XboxHomeMyMusicWidget::OnTrackImportProgress));
	UpdateDownloadsTile();
}


void Tidal::XboxHomeMyMusicWidget::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.clear();
}


void Tidal::XboxHomeMyMusicWidget::OnGoToNyPlaylists(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {
		
		shell->Frame->Navigate(XboxOnePlaylistsPage::typeid);
	}
}


void Tidal::XboxHomeMyMusicWidget::OnGoToDownloads(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell) {

		shell->Frame->Navigate(XboxDownloadsPage::typeid);
	}
}
