//
// ArtistPage.xaml.cpp
// Implementation of the ArtistPage class
//

#include "pch.h"
#include "ArtistPage.xaml.h"
#include "AnimationHelpers.h"
#include "tools/TimeUtils.h"
#include <WindowsNumerics.h>
#include "ArtistDataSources.h"
#include <Api/ImageUriResolver.h>
#include <GroupedAlbums.h>
#include "IncrementalDataSources.h"
#include <Api/ApiErrors.h>
#include <Shell.xaml.h>
#include "AlbumPage.xaml.h"
#include "FavoritesService.h"
#include "VideoItemVM.h"
#include <tools/AsyncHelpers.h>
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

ArtistPage::ArtistPage()
{
	InitializeComponent();
}

void parseBio(TextBlock^ txtBlock, std::wstring bioText) {
	txtBlock->Inlines->Clear();
	auto pos = bioText.find(L"<br/>", 0);
	while (pos != bioText.npos) {
		bioText.replace(pos, 5, L"\n");
		pos = bioText.find(L"<br/>", 0);
	}
	pos = bioText.find(L"  ", 0);
	while (pos != bioText.npos) {
		bioText.replace(pos, 5, L" ");
		pos = bioText.find(L"  ", 0);
	}
	pos = 0;
	auto wimplinkPos = bioText.find(L"[wimpLink", pos);
	while (wimplinkPos != bioText.npos) {
		auto beforeLinkRun = ref new Windows::UI::Xaml::Documents::Run();
		beforeLinkRun->Text = ref new String(&bioText[pos], wimplinkPos - pos);
		txtBlock->Inlines->Append(beforeLinkRun);
		
		auto textStart = bioText.find(L']', wimplinkPos)+1;
		auto textEnd = bioText.find(L"[/wimpLink]", textStart);
		
		auto link = ref new Windows::UI::Xaml::Documents::Hyperlink();
		auto linkText = ref new Windows::UI::Xaml::Documents::Run();
		linkText->Text = ref new String(&bioText[textStart], textEnd - textStart);
		link->Inlines->Append(linkText);

		if (bioText.find(L"artistId=\"", wimplinkPos+10) == wimplinkPos + 10) {
			wchar_t* unused;
			std::int64_t id = std::wcstoll(&bioText[wimplinkPos + 10 + 10], &unused, 10);
			link->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^>([id](Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^) {
				auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
				if (shell) {
					shell->Frame->Navigate(ArtistPage::typeid, id);
				}
			});
		}
		else if (bioText.find(L"albumId=\"", wimplinkPos + 10) == wimplinkPos + 10) {
			auto indexOfIdEnd = bioText.find(L'\"', wimplinkPos + 10 + 9);
			auto id = ref new String(&bioText[wimplinkPos + 10 + 9], indexOfIdEnd - (wimplinkPos + 10 + 9));
			link->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^>([id](Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^) {
				auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
				if (shell) {
					shell->Frame->Navigate(AlbumPage::typeid, id);
				}
			});
		}
		txtBlock->Inlines->Append(link);

		pos = textEnd + 11;
		wimplinkPos = bioText.find(L"[wimpLink", pos);
	}

	if (pos != bioText.size() && pos != bioText.npos) {
		auto lastRun = ref new Windows::UI::Xaml::Documents::Run();
		lastRun->Text = ref new String(&bioText[pos]);
		txtBlock->Inlines->Append(lastRun);
	}

}

ref class ArtistPageState sealed {
public:
	property int SelectedPivotIndex;
	ArtistPageState(int selectedPivotIndex) {
		SelectedPivotIndex = selectedPivotIndex;
	}
};

Platform::Object ^ Tidal::ArtistPage::GetStateToPreserve()
{
	return ref new ArtistPageState(pivot->SelectedIndex);
}

concurrency::task<void> Tidal::ArtistPage::LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ args)
{
	auto preservedState = GetCurrentPagePreservedState<ArtistPageState>();
	auto cancelToken = _cts.get_token();
	try {
		loadingView->LoadingState = Tidal::LoadingState::Loading;
		auto id = dynamic_cast<IBox<std::int64_t>^>(args->Parameter)->Value;
		_artistId = id;
		if (getFavoriteService().isArtistFavorite(id)) {
			addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else {
			removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		auto info = co_await artists::getArtistInfoAsync(id, cancelToken);
		videosGV->ItemsSource = getArtistVideosDataSource(id);
		similarArtistsGV->ItemsSource = getSimilarArtistsDataSource(id);
		auto popularTracksTask = tools::async::swallowCancellationException( artists::getArtistPopularTracksAsync(id, 10, cancelToken));
		auto albumsTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, cancelToken));
		auto singlesTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, cancelToken, L"EPSANDSINGLES"));
		auto compilationsTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, cancelToken, L"COMPILATIONS"));
		auto bioTask = tools::async::swallowCancellationException(artists::getArtistBioAsync(id, cancelToken));
		if (info->picture.size() > 0) {
			loadImageAsync(api::resolveImageUri(info->picture, 1024, 256));
		}
		pageHeader->Text = tools::strings::toWindowsString(info->name);
		headerArtistName->Text = tools::strings::toWindowsString(info->name);
		headerImageRound->ImageSource = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(api::resolveImageUri(info->picture, 160, 160)));
		auto tracks = co_await popularTracksTask;
		if (tracks.cancelled) {
			concurrency::cancel_current_task();
		}
		auto trackSource = ref new Platform::Collections::Vector<TrackItemVM^>();
		for (auto&& t : tracks.result->items) {
			auto ti = ref new TrackItemVM(t);
			trackSource->Append(ti);
			ti->AttachTo(trackSource);
		}
		_popularTracks = trackSource;
		_tracksPlaybackManager = std::make_shared<TracksPlaybackStateManager>();
		_tracksPlaybackManager->initialize(trackSource, Dispatcher);
		popularTracksLV->ItemsSource = trackSource;
		auto albumGroups = ref new Platform::Collections::Vector<GroupedAlbums^>();
		{
			auto albums = ref new GroupedAlbums();
			albums->Title = L"ALBUMS";
			albums->Albums = ref new Platform::Collections::Vector<AlbumResumeItemVM^>();
			auto albumsSource = co_await albumsTask;
			if (albumsSource.cancelled) {
				concurrency::cancel_current_task();
			}
			for (auto&& a : albumsSource.result->items) {
				albums->Albums->Append(ref new AlbumResumeItemVM(a));
			}
			albumGroups->Append(albums);
		}

		{
			auto albums = ref new GroupedAlbums();
			albums->Title = L"SINGLES";
			albums->Albums = ref new Platform::Collections::Vector<AlbumResumeItemVM^>();
			auto albumsSource = co_await singlesTask;
			if (albumsSource.cancelled) {
				concurrency::cancel_current_task();
			}
			for (auto&& a : albumsSource.result->items) {
				albums->Albums->Append(ref new AlbumResumeItemVM(a));
			}
			albumGroups->Append(albums);
		}
		{
			auto albums = ref new GroupedAlbums();
			albums->Title = L"COMPILATIONS";
			albums->Albums = ref new Platform::Collections::Vector<AlbumResumeItemVM^>();
			auto albumsSource = co_await compilationsTask;
			if (albumsSource.cancelled) {
				concurrency::cancel_current_task();
			}
			for (auto&& a : albumsSource.result->items) {
				albums->Albums->Append(ref new AlbumResumeItemVM(a));
			}
			albumGroups->Append(albums);
		}

		auto viewSource = ref new CollectionViewSource();
		viewSource->Source = albumGroups;
		viewSource->IsSourceGrouped = true;
		viewSource->ItemsPath = ref new PropertyPath(L"Albums");
		discoGridView->ItemsSource = viewSource->View;
		try {
			auto bio = co_await bioTask;
			if (bio.cancelled) {
				concurrency::cancel_current_task();
			}
			parseBio(bioTxt, bio.result->text);
			bioSource->Text = L"Source: " + tools::strings::toWindowsString(bio.result->source);
		}
		catch (api::statuscode_exception& ex) {
			if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::NotFound) {
				unsigned int index;
				if (pivot->Items->IndexOf(bioPivotItem, &index)) {
					pivot->Items->RemoveAt(index);
				}
			}
			else {
				throw;
			}
		}
		if (preservedState) {
			pivot->SelectedIndex = preservedState->SelectedPivotIndex;
		}
		loadingView->LoadingState = Tidal::LoadingState::Loaded;
	}
	catch (...) {

		loadingView->LoadingState = Tidal::LoadingState::Error;
	}
}



concurrency::task<void> Tidal::ArtistPage::loadImageAsync(Platform::String ^ url)
{
	headerImage->CustomDevice = Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice();
	auto bmp = co_await Microsoft::Graphics::Canvas::CanvasBitmap::LoadAsync(Microsoft::Graphics::Canvas::CanvasDevice::GetSharedDevice(), ref new Uri(url));

	_albumBmp = bmp;
	co_await Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
		headerImage->Opacity = 0;
		headerImage->Invalidate();
		AnimateTo(headerImage, L"Opacity", 1.0, tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(150)));
	}));
}

void Tidal::ArtistPage::OnDrawHeaderImage(Microsoft::Graphics::Canvas::UI::Xaml::ICanvasAnimatedControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasAnimatedDrawEventArgs^ args)
{
	if (_albumBmp) {
		auto blur = ref new Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect();
		blur->BlurAmount = 10;
		blur->Source = _albumBmp;
		blur->BorderMode = Microsoft::Graphics::Canvas::Effects::EffectBorderMode::Hard;

		
		auto session = args->DrawingSession;
		session->Clear(Windows::UI::Colors::Black);
		auto targetSize = sender->Size;
		auto srcSize = _albumBmp->GetBounds(session);
		auto targetAspectRatio = targetSize.Width / targetSize.Height;
		auto srcAspectRatio = srcSize.Width / srcSize.Height;
		auto opacityBrush = ref new Microsoft::Graphics::Canvas::Brushes::CanvasLinearGradientBrush(sender, Windows::UI::ColorHelper::FromArgb(192, 0, 0, 0), Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0) );
		opacityBrush->StartPoint = Windows::Foundation::Numerics::float2(0, 0);
		opacityBrush->EndPoint = Windows::Foundation::Numerics::float2(0, targetSize.Height);
		if (targetAspectRatio > srcAspectRatio) {
			auto h = srcSize.Width / targetAspectRatio;
			auto blurBrush = ref new Microsoft::Graphics::Canvas::Brushes::CanvasImageBrush(sender, blur);
			
			blurBrush->SourceRectangle = Rect(0, (srcSize.Height - h)*.5f, srcSize.Width, h);

			auto scale = targetSize.Width / blurBrush->SourceRectangle->Value.Width;
			blurBrush->Transform = Windows::Foundation::Numerics::make_float3x2_scale(scale);
			session->FillRectangle(Rect(0, 0, targetSize.Width, targetSize.Height), blurBrush, opacityBrush);
		}
		else {
			auto w = srcSize.Height*targetAspectRatio;

			auto blurBrush = ref new Microsoft::Graphics::Canvas::Brushes::CanvasImageBrush(sender, blur);
			blurBrush->SourceRectangle = Rect((srcSize.Width - w)*.5f, 0, w, srcSize.Height);

			auto scale = targetSize.Width / blurBrush->SourceRectangle->Value.Width;
			blurBrush->Transform = Windows::Foundation::Numerics::make_float3x2_scale(scale);
			session->FillRectangle(Rect(0, 0, targetSize.Width, targetSize.Height), blurBrush, opacityBrush);
		}
	}
}




void Tidal::ArtistPage::OnAlbumClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<AlbumResumeItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::ArtistPage::OnVideoClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<VideoItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::ArtistPage::OnSimilarArtistClicked(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto item = dynamic_cast<ArtistItemVM^>(e->ClickedItem);
	if (item) {
		item->Go();
	}
}


void Tidal::ArtistPage::OnAddFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().addArtistAsync(_artistId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
}


void Tidal::ArtistPage::OnRemoveFavoriteClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	getFavoriteService().removeArtistAsync(_artistId).then([](concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (...) {}
	});
	addFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Visible;
	removeFavoriteButton->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
}

concurrency::task<void> PlayRadio(std::int64_t artistId) {
	auto tracks = co_await tools::async::swallowCancellationException(artists::getArtistRadioTracksAsync(artistId, 100, concurrency::cancellation_token::none()));
	auto trackSource = ref new Platform::Collections::Vector<TrackItemVM^>();
	if (tracks.cancelled) {
		return;
	}
	for (auto&& t : tracks.result->items) {
		auto ti = ref new TrackItemVM(t);
		trackSource->Append(ti);
		ti->AttachTo(trackSource);
	}
	if (trackSource->Size > 0) {
		trackSource->GetAt(0)->PlayCommand->Execute(nullptr);
	}
}

void Tidal::ArtistPage::OnPlayPopularTracks(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_popularTracks && _popularTracks->Size > 0) {
		_popularTracks->GetAt(0)->PlayCommand->Execute(nullptr);
	}
}


void Tidal::ArtistPage::OnRadioClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	PlayRadio(this->_artistId);
}

