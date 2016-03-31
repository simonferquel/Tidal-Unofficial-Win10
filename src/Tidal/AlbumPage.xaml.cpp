//
// AlbumPage.xaml.cpp
// Implementation of the AlbumPage class
//

#include "pch.h"
#include "AlbumPage.xaml.h"
#include "AnimationHelpers.h"
#include "tools/TimeUtils.h"
#include "AlbumDataSources.h"
#include "Api/ImageUriResolver.h"
#include "tools/StringUtils.h"
#include "AudioService.h"
#include "IncrementalDataSources.h"
#include "ArtistPage.xaml.h"
#include "MenuFlyouts.h"
#include <Api/CoverCache.h>
#include "FavoritesService.h"
#include "LoadingView.h"
#include "PlayCommand.h"
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
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

AlbumPage::AlbumPage()
{
	InitializeComponent();
	
}

Tidal::AlbumPage::~AlbumPage()
{
}




concurrency::task<void> Tidal::AlbumPage::loadImageAsync(Platform::String^ url,concurrency::cancellation_token cancelToken)
{
	win2DCtl->CustomDevice = Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice();
	auto bmp = co_await concurrency::create_task( Microsoft::Graphics::Canvas::CanvasBitmap::LoadAsync(Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice(), ref new Uri(url)), cancelToken);

	_albumBmp = bmp;
	co_await Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
		win2DCtl->Opacity = 0;
		win2DCtl->Invalidate();
		AnimateTo(win2DCtl, L"Opacity", 1.0, tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(150)));
	}));

}




void Tidal::AlbumPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
	auto idStr = dynamic_cast<String^>(e->Parameter);
	wchar_t* _;
	std::int64_t id = std::wcstoll(idStr->Data(), &_, 10);
	_albumId = id;
	LoadAsync(id);

}


concurrency::task<void> Tidal::AlbumPage::LoadAsync(std::int64_t id)
{
	auto cancelToken = _cts.get_token();
	try {
		loadingView->LoadingState = Tidal::LoadingState::Loading;
		if (getFavoriteService().isAlbumFavorite(id)) {
			addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else {
			addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
			removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

		}
		auto albumInfo = co_await albums::getAlbumResumeAsync(id, cancelToken);
		_artistId = albumInfo->artist.id;
		pageHeader->Text = tools::strings::toWindowsString(albumInfo->title);
		auto headImageUrl = co_await api::GetCoverUriAndFallbackToWebAsync(id, tools::strings::toWindowsString(albumInfo->cover), 320, 320, cancelToken);

		headerCover->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(headImageUrl));

		headerTitle->Text = pageHeader->Text;
		headerArtist->Text = tools::strings::toWindowsString(albumInfo->artist.name);

		std::wstring txtTracksAndDuration;
		txtTracksAndDuration.append(std::to_wstring(albumInfo->numberOfTracks));
		txtTracksAndDuration.append(L" tracks (");
		txtTracksAndDuration.append(tools::time::toStringMMSS(albumInfo->duration));
		txtTracksAndDuration.append(L")");
		headerTracksAndDuration->Text = tools::strings::toWindowsString(txtTracksAndDuration);
		albumsGV->ItemsSource = getArtistAlbumsDataSource(albumInfo->artist.id);
		auto tracks = co_await albums::getAlbumTracksAsync(id, albumInfo->numberOfTracks, cancelToken);
		auto tracksDataSource = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
		for (auto&& t : tracks->items) {
			auto ti = ref new Tidal::TrackItemVM(t, true);
			tracksDataSource->Append(ti);
			ti->AttachTo(tracksDataSource);
		}
		tracksLV->ItemsSource = tracksDataSource;
		_tracks = tracksDataSource;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(_tracks, Dispatcher);
		if (albumInfo->cover.size() > 0) {
			auto bgImageUrl = co_await api::GetCoverUriAndFallbackToWebAsync(id, tools::strings::toWindowsString(albumInfo->cover), 1080, 1080, cancelToken);

			co_await loadImageAsync(bgImageUrl, cancelToken);
		}
		loadingView->LoadingState = Tidal::LoadingState::Loaded;
	}
	catch (...) {
		loadingView->LoadingState = Tidal::LoadingState::Error;
	}
}

void Tidal::AlbumPage::OnWin2DDrawing(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	if (_albumBmp) {
		auto blur = ref new Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect();
		blur->BlurAmount = 10;
		blur->Source = _albumBmp;
		blur->BorderMode = Microsoft::Graphics::Canvas::Effects::EffectBorderMode::Hard;

		auto darken = ref new Microsoft::Graphics::Canvas::Effects::ExposureEffect();
		darken->Exposure = -1.5;
		darken->Source = blur;
		auto session = args->DrawingSession;
		auto targetSize = sender->Size;
		auto srcSize = _albumBmp->GetBounds(session);
		auto targetAspectRatio = targetSize.Width / targetSize.Height;
		auto srcAspectRatio = srcSize.Width / srcSize.Height;
		if (targetAspectRatio > srcAspectRatio) {
			auto h = srcSize.Width / targetAspectRatio;
			session->DrawImage(darken, Rect(0, 0, targetSize.Width, targetSize.Height), Rect(0, (srcSize.Height - h)*.5f, srcSize.Width, h));
		}
		else {
			auto w = srcSize.Height*targetAspectRatio;
			session->DrawImage(darken, Rect(0, 0, targetSize.Width, targetSize.Height), Rect((srcSize.Width - w)*.5f, 0, w, srcSize.Height));
		}
	}
}


void Tidal::AlbumPage::OnWin2DCtlLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
}


void Tidal::AlbumPage::OnPlayAlbumClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_tracks && _tracks->Size >0) {
		auto cmd = ref new PlayCommand(_tracks->GetAt(0), _tracks);
		cmd->Execute(nullptr);
	}
}


void Tidal::AlbumPage::OnAlbumClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<AlbumResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::AlbumPage::OnGoToArtist(Windows::UI::Xaml::Documents::Hyperlink^ sender, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs^ args)
{
	Frame->Navigate(ArtistPage::typeid, _artistId);
}


void Tidal::AlbumPage::OnMenuClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto flyout = getAlbumMenuFlyout(_albumId);
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	flyout->ShowAt(fe, Point(0, fe->ActualHeight));
}


void Tidal::AlbumPage::OnAddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().addAlbumAsync(_albumId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void Tidal::AlbumPage::OnRemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().removeAlbumAsync(_albumId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}
