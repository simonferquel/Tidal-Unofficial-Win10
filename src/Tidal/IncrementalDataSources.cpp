#include "pch.h"
#include "IncrementalDataSources.h"
#include "Api/PaginatedList.h"
#include "Api/PromotionArticle.h"
#include "PromotionItemVM.h"
#include "Api/GetPromotionsQuery.h"
#include "AuthenticationService.h"
#include "tools/AsyncHelpers.h"
#include "tools/TimeUtils.h"
#include "Api/GetNewVideosQuery.h"
#include "VideoItemVM.h"
#include "Api/GetNewAlbumsQuery.h"
#include "AlbumResumeItemVM.h"
#include "Api/GetNewPlaylistsQuery.h"
#include "PlaylistResumeItemVM.h"
#include "Api/GetNewTracksQuery.h"
#include <Api/GetArtistAlbumsQuery.h>
#include <Api/GetArtistVideosQuery.h>
#include <Api/GetSimilarArtistsQuery.h>
#include "ArtistItemVM.h"
#include <Api/GetListSublistsQuery.h>
#include <Api/SearchQuery.h>
#include "SearchResultGroup.h"
#include <localdata/GetLocalAlbumsQuery.h>
using namespace Platform;
using namespace Platform::Collections;
using namespace concurrency;
Tidal::IncrementalLoadingCollection ^ getNewsPromotionsDataSource(Platform::String^ list)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([list](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetPromotionsQuery query(40, toFill->Size, list, L"US");
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::PromotionItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([list,subType = authSvc.authenticationState().subscribtionType(), userId = authSvc.authenticationState().userId(), sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetPromotionsQuery query(40, toFill->Size, list,subType, userId, sessionId, countryCode);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::PromotionItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getNewsVideosDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([list, group](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetNewVideosQuery query(40, toFill->Size, L"US");
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::VideoItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([list, group, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetNewVideosQuery query(40, toFill->Size, sessionId, countryCode);
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::VideoItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getNewsAlbumsDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([list,group](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetNewAlbumsQuery query(40, toFill->Size, L"US");
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([list, group,sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetNewAlbumsQuery query(40, toFill->Size, sessionId, countryCode);
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getArtistAlbumsDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([artistId](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetArtistAlbumsQuery query(artistId, 40, toFill->Size, L"US");
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetArtistAlbumsQuery query(artistId, 40, toFill->Size, sessionId, countryCode);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getArtistVideosDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([artistId](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetArtistVideosQuery query(artistId, 40, toFill->Size, L"US");
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::VideoItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetArtistVideosQuery query(artistId, 40, toFill->Size, sessionId, countryCode);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::VideoItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getSimilarArtistsDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([artistId](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetSimilarArtistsQuery query(artistId, 40, toFill->Size, L"US");
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::ArtistItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetSimilarArtistsQuery query(artistId, 40, toFill->Size, sessionId, countryCode);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::ArtistItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getNewsPlaylistsDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([list,group](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::GetNewPlaylistsQuery query(40, toFill->Size, L"US");
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::PlaylistResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([list,group,sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::GetNewPlaylistsQuery query(40, toFill->Size, sessionId, countryCode);
				query.withCustomListGroup(list, group);
				auto result = await query.executeAsync(cancelToken);
				for (auto& article : result->items) {
					toFill->Append(ref new Tidal::PlaylistResumeItemVM(article));
				}
				return toFill->Size < result->totalNumberOfItems;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

concurrency::task<Platform::Collections::Vector<Tidal::TrackItemVM^>^> getNewsTrackItemsAsync(concurrency::cancellation_token cancelToken, Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	auto retval = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	if (authSvc.authenticationState().isAuthenticated()) {
		api::GetNewTracksQuery query(25, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		query.withCustomListGroup(list, group);
		auto result = await query.executeAsync(cancelToken);
		for (auto& article : result->items) {
			retval->Append(ref new Tidal::TrackItemVM(article));
		}
		return retval;
	}
	else {
		api::GetNewTracksQuery query(25, 0, L"US");
		query.withCustomListGroup(list, group);
		auto result = await query.executeAsync(cancelToken);
		for (auto& article : result->items) {
			retval->Append(ref new Tidal::TrackItemVM(article));
		}
		return retval;
	}
}

concurrency::task<std::shared_ptr<std::vector<api::SublistInfo>>> getSublistsAsync(concurrency::cancellation_token cancelToken, Platform::String ^ list)
{
	auto& authSvc = getAuthenticationService();
	auto retval = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	if (authSvc.authenticationState().isAuthenticated()) {
		api::GetListSublistsQuery query(list, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		
		return await query.executeAsync(cancelToken);
	}
	else {
		api::GetListSublistsQuery query(list, L"US");

		return await query.executeAsync(cancelToken);
	}
}

concurrency::task<Windows::UI::Xaml::Data::ICollectionView^> searchAllAsync(Platform::String ^ query, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	std::shared_ptr<api::SearchResults> results;
	if (authSvc.authenticationState().isAuthenticated()) {
		api::SearchQuery query(query, L"ARTISTS,ALBUMS,TRACKS,VIDEOS,PLAYLISTS", 3, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		results = await query.executeAsync(cancelToken);
	}
	else {
		api::SearchQuery query(query, L"ARTISTS,ALBUMS,TRACKS,VIDEOS,PLAYLISTS", 3, 0, L"US");
		results = await query.executeAsync(cancelToken);
	}

	auto groups = ref new Platform::Collections::Vector<Tidal::SearchResultGroup^>();
	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Artists";
		gp->Icon = L"J";
		gp->Filter = L"ARTISTS";
		gp->Query = query;
		gp->Items = ref new Platform::Collections::Vector<Object^>();
		groups->Append(gp);

		for (auto&& info : results->artists.items) {
			gp->Items->Append(ref new Tidal::ArtistItemVM(info));
		}
	}

	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Tracks";
		gp->Icon = L"G";
		gp->Filter = L"TRACKS";
		gp->Query = query;
		gp->Items = ref new Platform::Collections::Vector<Object^>();
		groups->Append(gp);

		for (auto&& info : results->tracks.items) {
			gp->Items->Append(ref new Tidal::TrackItemVM(info));
		}
	}
	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Albums";
		gp->Filter = L"ALBUMS";
		gp->Query = query;
		gp->Icon = L"A";
		gp->Items = ref new Platform::Collections::Vector<Object^>();
		groups->Append(gp);

		for (auto&& info : results->albums.items) {
			gp->Items->Append(ref new Tidal::AlbumResumeItemVM(info));
		}
	}
	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Playlists";
		gp->Filter = L"PLAYLISTS";
		gp->Query = query;
		gp->Icon = L"M";
		gp->Items = ref new Platform::Collections::Vector<Object^>();
		groups->Append(gp);

		for (auto&& info : results->playlists.items) {
			gp->Items->Append(ref new Tidal::PlaylistResumeItemVM(info));
		}
	}
	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Videos";
		gp->Filter = L"VIDEOS";
		gp->Query = query;
		gp->Icon = L"9";
		gp->Items = ref new Platform::Collections::Vector<Object^>();
		groups->Append(gp);

		for (auto&& info : results->videos.items) {
			gp->Items->Append(ref new Tidal::VideoItemVM(info));
		}
	}

	auto cvs = ref new Windows::UI::Xaml::Data::CollectionViewSource();
	cvs->IsSourceGrouped = true;
	cvs->ItemsPath = ref new Windows::UI::Xaml::PropertyPath(L"Items");
	cvs->Source = groups;
	return cvs->View;
}

Tidal::IncrementalLoadingCollection ^ getFilteredSearchSource(Platform::String ^ query, Platform::String ^ filter)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return ref new Tidal::IncrementalLoadingCollection([query, filter](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
			try {
				api::SearchQuery query(query, filter, 40, toFill->Size, L"US");
				auto beforeCount = toFill->Size;
				auto result = await query.executeAsync(cancelToken);
				for (auto& info : result->albums.items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(info));
				}
				for (auto& info : result->artists.items) {
					toFill->Append(ref new Tidal::ArtistItemVM(info));
				}
				for (auto& info : result->playlists.items) {
					toFill->Append(ref new Tidal::PlaylistResumeItemVM(info));
				}
				for (auto& info : result->tracks.items) {
					toFill->Append(ref new Tidal::TrackItemVM(info));
				}
				for (auto& info : result->videos.items) {
					toFill->Append(ref new Tidal::VideoItemVM(info));
				}
				return toFill->Size > beforeCount;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
	else {
		return ref new Tidal::IncrementalLoadingCollection([query, filter, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken)->task<bool> {
			try {
				api::SearchQuery query(query, filter, 40, toFill->Size, sessionId, countryCode);
				auto beforeCount = toFill->Size;
				auto result = await query.executeAsync(cancelToken);
				for (auto& info : result->albums.items) {
					toFill->Append(ref new Tidal::AlbumResumeItemVM(info));
				}
				for (auto& info : result->artists.items) {
					toFill->Append(ref new Tidal::ArtistItemVM(info));
				}
				for (auto& info : result->playlists.items) {
					toFill->Append(ref new Tidal::PlaylistResumeItemVM(info));
				}
				for (auto& info : result->tracks.items) {
					toFill->Append(ref new Tidal::TrackItemVM(info));
				}
				for (auto& info : result->videos.items) {
					toFill->Append(ref new Tidal::VideoItemVM(info));
				}
				return toFill->Size > beforeCount;
			}
			catch (...) {
			}

			// something wron append.
			// delay and retry on next scroll event
			await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
			return true;

		});
	}
}

Tidal::IncrementalLoadingCollection ^ getLocalAlbumsDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = await localdata::getImportedAlbumsAsync(toFill->Size, desiredCount, cancelToken);
			
			for (auto& article : *data) {
				api::AlbumResume r(web::json::value::parse(article.json));
				toFill->Append(ref new Tidal::AlbumResumeItemVM(r, true));
			}
			return data->size() > 0;
		}
		catch (...) {
		}

		// something wron append.
		// delay and retry on next scroll event
		await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}

Tidal::IncrementalLoadingCollection ^ getLocalPlaylistsDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = await localdata::getImportedPlaylistsAsync(toFill->Size, desiredCount, cancelToken);

			for (auto& article : *data) {
				api::PlaylistResume r(web::json::value::parse(article.json));
				toFill->Append(ref new Tidal::PlaylistResumeItemVM(r, true));
			}
			return data->size() > 0;
		}
		catch (...) {
		}

		// something wron append.
		// delay and retry on next scroll event
		await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}

Tidal::IncrementalLoadingCollection ^ getLocalTracksDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = await localdata::getImportedTracksAsync(toFill->Size, desiredCount, cancelToken);

			for (auto& article : *data) {
				api::TrackInfo r(web::json::value::parse(article.json));
				toFill->Append(ref new Tidal::TrackItemVM(r, false));
			}
			return data->size() > 0;
		}
		catch (...) {
		}

		// something wron append.
		// delay and retry on next scroll event
		await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}
