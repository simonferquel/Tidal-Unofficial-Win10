//
// XboxArtistPage.xaml.cpp
// Implementation of the XboxArtistPage class
//

#include "pch.h"
#include "XboxArtistPage.xaml.h"
#include "XboxShell.xaml.h"
#include "XboxAlbumPage.xaml.h"
#include "XboxPlaylistPage.xaml.h"
#include "../ArtistDataSources.h"
#include "../FavoritesService.h"
#include "../IncrementalDataSources.h"
#include <tools\AsyncHelpers.h>
#include <Api/ImageUriResolver.h>
#include "../GroupedAlbums.h"
#include <Api/ApiErrors.h>
#include "../XboxOneHub.h"
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



void parseBioXbox(TextBlock^ txtBlock, std::wstring bioText) {
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

		auto textStart = bioText.find(L']', wimplinkPos) + 1;
		auto textEnd = bioText.find(L"[/wimpLink]", textStart);

		auto link = ref new Windows::UI::Xaml::Documents::Hyperlink();
		auto linkText = ref new Windows::UI::Xaml::Documents::Run();
		linkText->Text = ref new String(&bioText[textStart], textEnd - textStart);
		link->Inlines->Append(linkText);

		if (bioText.find(L"artistId=\"", wimplinkPos + 10) == wimplinkPos + 10) {
			wchar_t* unused;
			std::int64_t id = std::wcstoll(&bioText[wimplinkPos + 10 + 10], &unused, 10);
			link->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^>([id](Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^) {
				auto shell = dynamic_cast<XboxShell^>(Windows::UI::Xaml::Window::Current->Content);
				if (shell) {
					shell->Frame->Navigate(XboxArtistPage::typeid, id);
				}
			});
		}
		else if (bioText.find(L"albumId=\"", wimplinkPos + 10) == wimplinkPos + 10) {
			auto indexOfIdEnd = bioText.find(L'\"', wimplinkPos + 10 + 9);
			auto id = ref new String(&bioText[wimplinkPos + 10 + 9], indexOfIdEnd - (wimplinkPos + 10 + 9));
			link->Click += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^>([id](Windows::UI::Xaml::Documents::Hyperlink ^, Windows::UI::Xaml::Documents::HyperlinkClickEventArgs ^) {
				auto shell = dynamic_cast<XboxShell^>(Windows::UI::Xaml::Window::Current->Content);
				if (shell) {
					shell->Frame->Navigate(XboxAlbumPage::typeid, id);
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



// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

concurrency::task<void> Tidal::XboxArtistPage::LoadAsync(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ args)
{
	try {
		loadingView->LoadingState = Tidal::LoadingState::Loading;
		auto id = dynamic_cast<IBox<std::int64_t>^>(args->Parameter)->Value;
		_artistId = id;
		if (getFavoriteService().isArtistFavorite(id)) {
			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else {
			btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
			btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		auto info = co_await artists::getArtistInfoAsync(id, concurrency::cancellation_token::none());
		videosGV->ItemsSource = getArtistVideosDataSource(id);
		similarArtistsGV->ItemsSource = getSimilarArtistsDataSource(id);
		auto popularTracksTask = tools::async::swallowCancellationException(artists::getArtistPopularTracksAsync(id, 10, concurrency::cancellation_token::none()));
		auto albumsTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, concurrency::cancellation_token::none()));
		auto singlesTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, concurrency::cancellation_token::none(), L"EPSANDSINGLES"));
		auto compilationsTask = tools::async::swallowCancellationException(artists::getArtistAlbumsAsync(id, concurrency::cancellation_token::none(), L"COMPILATIONS"));
		auto bioTask = tools::async::swallowCancellationException(artists::getArtistBioAsync(id, concurrency::cancellation_token::none()));
		
		title->Text = tools::strings::toWindowsString(info->name);
		//headerArtistName->Text = tools::strings::toWindowsString(info->name);
		artistImg ->Source = ref new Windows::UI::Xaml::Media::Imaging::BitmapImage(ref new Uri(api::resolveImageUri(info->picture, 1080, 720)));
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
		lv->ItemsSource = trackSource;
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
		albumsGV->ItemsSource = viewSource->View;
		try {
			auto bio = co_await bioTask;
			if (bio.cancelled) {
				concurrency::cancel_current_task();
			}
			parseBioXbox(bioTxt, bio.result->text);
			bioSource->Text = L"Source: " + tools::strings::toWindowsString(bio.result->source);
		}
		catch (api::statuscode_exception& ex) {
			if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::NotFound) {
				unsigned int index;
				if (hub->Items->IndexOf(bioSec, &index)) {
					hub->Items->RemoveAt(index);
				}
			}
			else {
				throw;
			}
		}
		
		loadingView->LoadingState = Tidal::LoadingState::Loaded;
	}
	catch (...) {

		loadingView->LoadingState = Tidal::LoadingState::Error;
	}
}

XboxArtistPage::XboxArtistPage()
{
	InitializeComponent();
}


void Tidal::XboxArtistPage::OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{
	auto go = dynamic_cast<IGo^> (e->ClickedItem);
	if (go)go->Go();
}


void Tidal::XboxArtistPage::OnToggleFavoritesClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (getFavoriteService().isArtistFavorite(_artistId)) {
		getFavoriteService().removeArtistAsync(_artistId);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	else {
		getFavoriteService().addArtistAsync(_artistId);
		btnAddToFavorites->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		btnRemoveFromFavorites->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
}


void Tidal::XboxArtistPage::OnPlay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_popularTracks && _popularTracks->Size > 0) {
		_popularTracks->GetAt(0)->TogglePlayPause();
	}
}

concurrency::task<void> PlayRadioXbox(std::int64_t artistId) {
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

void Tidal::XboxArtistPage::OnPlayRadio(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	PlayRadioXbox(_artistId);
}


void Tidal::XboxArtistPage::OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto found = FindFirstTabStopDescendant(this);
	if (found) {
		found->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}

	auto shell = dynamic_cast<XboxShell^>(Window::Current->Content);
	if (shell)
	{
		auto anim = hub->CompositionPropertySet->Compositor->CreateExpressionAnimation(L"hub.NormalizedOffsetX");
		anim->SetReferenceParameter(L"hub", hub->CompositionPropertySet);
		shell->AnimatedBackground->SetParallaxAmountBinding(anim);
	}
}
