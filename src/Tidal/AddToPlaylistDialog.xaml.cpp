//
// AddToPlaylistDialog.xaml.cpp
// Implementation of the AddToPlaylistDialog class
//

#include "pch.h"
#include "AddToPlaylistDialog.xaml.h"
#include "AuthenticationService.h"
#include "UnauthenticatedDialog.h"
#include <Api/FavoriteQueries.h>
#include <Api/PlaylistsQueries.h>
#include "LoadingView.h"
#include "PlaylistResumeItemVM.h"
#include <Api/GetPlaylistTracksQuery.h>
#include <Hat.h>

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

// The Content Dialog item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

concurrency::task<void> Tidal::AddToPlaylistDialog::loadPlaylistsAsync()
{
	loadingView->LoadingState = LoadingState::Loading;
	try {
		auto& authState = getAuthenticationService().authenticationState();
		auto query = api::GetMyPlaylistsQuery::Make(authState.userId(), authState.sessionId(), authState.countryCode());

		auto playlists = co_await query->executeAsync(concurrency::cancellation_token::none());
		auto playlistSrc = ref new Platform::Collections::Vector<PlaylistResumeItemVM^>();
		for (auto&& pl : playlists->items) {
			auto item = ref new PlaylistResumeItemVM(pl);
			playlistSrc->Append(item);
		}
		playlistsLV->ItemsSource = playlistSrc;

		loadingView->LoadingState = LoadingState::Loaded;
	}
	catch(...){
		loadingView->LoadingState = LoadingState::Error;
	}
}

concurrency::task<void> Tidal::AddToPlaylistDialog::addToExistingPlaylistAsync(Hat<PlaylistResumeItemVM> playlist)
{
	loadingView->LoadingState = LoadingState::Loading;
	try {
		auto& authState = getAuthenticationService().authenticationState();
		api::GetPlaylistTracksQuery existingTracksQuery(tools::strings::toStdString(playlist->Uuid), 0, 0, authState.sessionId(), authState.countryCode());
		auto tracksResult = co_await existingTracksQuery.executeAsync(concurrency::cancellation_token::none());
		api::AddTracksToPlaylistQuery query(tools::strings::toStdString( playlist->Uuid), tools::strings::toWindowsString(tracksResult->etag),  authState.sessionId(), authState.countryCode(), _trackIds, playlist->NumberOfTracks);

		co_await query.executeAsync(concurrency::cancellation_token::none());

		loadingView->LoadingState = LoadingState::Loaded;
		this->Hide();
	}
	catch (...) {
		loadingView->LoadingState = LoadingState::Error;
	}
}

concurrency::task<void> Tidal::AddToPlaylistDialog::addToNewPlaylistAsync()
{
	loadingView->LoadingState = LoadingState::Loading;
	try {
		auto& authState = getAuthenticationService().authenticationState();
		api::CreatePlaylistQuery createQuery(authState.userId(), authState.sessionId(), authState.countryCode(), txtTitle->Text, txtDesctiption->Text);
		auto playlist = co_await createQuery.executeAsync(concurrency::cancellation_token::none());
		api::GetPlaylistTracksQuery existingTracksQuery(playlist->uuid, 0, 0, authState.sessionId(), authState.countryCode());
		auto tracksResult = co_await existingTracksQuery.executeAsync(concurrency::cancellation_token::none());
		api::AddTracksToPlaylistQuery query(playlist->uuid, tools::strings::toWindowsString(tracksResult->etag), authState.sessionId(), authState.countryCode(), _trackIds, 0);

		co_await query.executeAsync(concurrency::cancellation_token::none());

		loadingView->LoadingState = LoadingState::Loaded;
		this->Hide();
	}
	catch (...) {
		loadingView->LoadingState = LoadingState::Error;
	}
}

Tidal::AddToPlaylistDialog::AddToPlaylistDialog(const std::vector<std::int64_t>& trackIds)
	:_trackIds(trackIds)
{
	InitializeComponent();
	loadPlaylistsAsync();
}

Tidal::AddToPlaylistDialog::AddToPlaylistDialog(std::vector<std::int64_t>&& trackIds)
	:_trackIds(std::move(trackIds))
{
	
	InitializeComponent();
	loadPlaylistsAsync();
}

void Tidal::ShowAddToPlaylistDialog(const std::vector<std::int64_t>& trackIds)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	auto dlg = ref new AddToPlaylistDialog(trackIds);
	dlg->ShowAsync();
}

void Tidal::ShowAddToPlaylistDialog(std::vector<std::int64_t>&& trackIds)
{
	if (!getAuthenticationService().authenticationState().isAuthenticated()) {
		showUnauthenticatedDialog();
		return;
	}
	auto dlg = ref new AddToPlaylistDialog(std::move(trackIds));
	dlg->ShowAsync();
}


void Tidal::AddToPlaylistDialog::OnCancel(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args)
{
	this->Hide();
}


void Tidal::AddToPlaylistDialog::OnPrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args)
{
	args->Cancel = true;
	if (!_isNewPlaylistMode) {
		/* <Setter Target="playlistLV.Visibility" Value="Collapsed" />
                    <Setter Target = "newPlaylistForm.Visibility" Value = "Visible" / >
						<Setter Target = "root.PrimaryButtonText" Value = "Ok" / >*/
		playlistsLV->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		newPlaylistForm->Visibility = Windows::UI::Xaml::Visibility::Visible;
		PrimaryButtonText = L"Ok";
		_isNewPlaylistMode = true;
		return;
	}

	else {
		addToNewPlaylistAsync();
	}
}


void Tidal::AddToPlaylistDialog::OnExistingPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e)
{

	auto playlist = dynamic_cast<PlaylistResumeItemVM^>(e->ClickedItem);
	if (playlist) {
		addToExistingPlaylistAsync(playlist);
	}
}
