//
// XboxDownloadsPage.xaml.cpp
// Implementation of the XboxDownloadsPage class
//

#include "pch.h"
#include "XboxDownloadsPage.xaml.h"
#include <localdata/db.h>
#include <localdata/GetTrackImportQueueQuery.h>
#include <tools/StringUtils.h>
#include <Api/ImageUriResolver.h>

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

XboxDownloadsPage::XboxDownloadsPage()
{
	InitializeComponent();
}

concurrency::task<void> Tidal::XboxDownloadsPage::LoadAsync()
{
	auto downloads = co_await LocalDB::executeAsync<localdata::GetTrackImportQueueQuery>(localdata::getDb());
	for (localdata::track_import_job& item : *downloads) {
		auto vm = ref new DownloadItemVM();
		vm->Artist = tools::strings::toWindowsString(item.artist);
		vm->Id = item.id;
		vm->ImageUrl = api::resolveImageUri(item.cover, 80, 80);
		vm->LocalSize = item.local_size;
		vm->ProgressVisibility = item.server_size > 0 ? Windows::UI::Xaml::Visibility::Visible : Windows::UI::Xaml::Visibility::Collapsed;
		vm->ServerSize = item.server_size;
		vm->Title = tools::strings::toWindowsString(item.title);
		_downloadQueue->Append(vm);
	}
	downloadsLV->ItemsSource = _downloadQueue;
}

void Tidal::XboxDownloadsPage::OnTrackImportComplete(const std::int64_t & id)
{
	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, id]() {
		for (auto ix = 0; ix < _downloadQueue->Size; ++ix) {
			if (_downloadQueue->GetAt(ix)->Id == id) {
				_downloadQueue->RemoveAt(ix);
				return;
			}
		}
	}));
}

void Tidal::XboxDownloadsPage::OnTrackImportProgress(const ImportProgress & progress)
{
	Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this, progress]() {
		for (auto ix = 0; ix < _downloadQueue->Size; ++ix) {
			if (_downloadQueue->GetAt(ix)->Id == progress.trackId) {
				_downloadQueue->GetAt(ix)->ServerSize = progress.serverSize;
				_downloadQueue->GetAt(ix)->LocalSize = progress.localSize;
				_downloadQueue->GetAt(ix)->ProgressVisibility = Windows::UI::Xaml::Visibility::Visible;
				return;
			}
		}
	}));
}


void Tidal::XboxDownloadsPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	_mediatorTokens.push_back(getTrackImportComplete().registerCallback<XboxDownloadsPage>(this, &XboxDownloadsPage::OnTrackImportComplete));
	_mediatorTokens.push_back(getTrackImportProgress().registerCallback<XboxDownloadsPage>(this, &XboxDownloadsPage::OnTrackImportProgress));
	LoadAsync();
}


void Tidal::XboxDownloadsPage::OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	_mediatorTokens.clear();
}
