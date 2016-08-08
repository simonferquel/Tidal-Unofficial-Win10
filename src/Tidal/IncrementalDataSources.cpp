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

template<typename TQuery>
struct incremental_loding_traits {
	using result_task_type = decltype(((TQuery*)nullptr)->executeAsync(concurrency::cancellation_token::none()));
	using result_type = decltype(((result_task_type*)nullptr)->get());

	using build_query_type = std::function<std::shared_ptr<TQuery>(size_t toSkip)>;
	using handle_results_type = std::function<bool(Vector<Object^>^ toFill, const result_type& result)>;
};

template <typename TQuery>
class IncrementalLoadingCallback {
public:
	using result_task_type = typename incremental_loding_traits<TQuery>::result_task_type;
	using result_type =  typename incremental_loding_traits<TQuery>::result_type;
protected:
	typename incremental_loding_traits<TQuery>::build_query_type _buildQuery;
	typename incremental_loding_traits<TQuery>::handle_results_type _handleResults;
public:
	IncrementalLoadingCallback(const typename incremental_loding_traits<TQuery>::build_query_type& buildQuery, const typename incremental_loding_traits<TQuery>::handle_results_type& handleResults) :
		_buildQuery(buildQuery),
		_handleResults(handleResults)
	{

	}
	task<bool> operator() (Vector<Object^>^ toFill, unsigned int desiredCount, cancellation_token cancelToken) const {
		auto query = _buildQuery(toFill->Size);
		return query->executeAsync(cancelToken)
			.then([handleResults = _handleResults, toFill, cancelToken](result_task_type resultTask) {
			try {
				return task_from_result(handleResults(toFill, resultTask.get()));
			}
			catch (...) {
				return tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken).then([]() {return true; });
			}
		}, task_continuation_context::get_current_winrt_context());
	}
};

template<typename TQuery>
struct IncrementalLoadingCollectionBuilder1 {
	typename incremental_loding_traits<TQuery>::build_query_type _buildQuery;
	IncrementalLoadingCollectionBuilder1(const typename incremental_loding_traits<TQuery>::build_query_type& buildQuery) : _buildQuery(buildQuery) {

	}
	Tidal::IncrementalLoadingCollection ^ with_handle_results(const typename incremental_loding_traits<TQuery>::handle_results_type& handleResults) {
		return ref new Tidal::IncrementalLoadingCollection(IncrementalLoadingCallback<TQuery>(_buildQuery, handleResults));
	}

};

template<typename TQuery>
struct IncrementalLoadingCollectionBuilder0 {
	IncrementalLoadingCollectionBuilder1<TQuery> with_build_query(const typename incremental_loding_traits<TQuery>::build_query_type& buildQuery) {
		return IncrementalLoadingCollectionBuilder1<TQuery>(buildQuery);
	}
};

template<typename TQuery>
IncrementalLoadingCollectionBuilder0<TQuery> BuildIncrementalLoadingCollection() {
	return IncrementalLoadingCollectionBuilder0<TQuery>();
}

Tidal::IncrementalLoadingCollection ^ getNewsPromotionsDataSource(Platform::String^ list)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return BuildIncrementalLoadingCollection<api::GetPromotionsQuery>()
			.with_build_query([list](auto toSkip) {
			return std::make_shared<api::GetPromotionsQuery>(40, toSkip, list, ref new String(L"US"));
		})
			.with_handle_results([](Vector<Object^>^ toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::PromotionItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});
	}
	else {
		return BuildIncrementalLoadingCollection<api::GetPromotionsQuery>()
			.with_build_query([list, subType = authSvc.authenticationState().subscribtionType(), userId = authSvc.authenticationState().userId(), sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			return std::make_shared<api::GetPromotionsQuery>(40, toSkip, list, subType, userId, sessionId, countryCode);
		})
			.with_handle_results([](Vector<Object^>^ toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::PromotionItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});



	}
}

Tidal::IncrementalLoadingCollection ^ getNewsVideosDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return BuildIncrementalLoadingCollection<api::GetNewVideosQuery>()
			.with_build_query(
				[list, group](auto toSkip) {
					auto retval = std::make_shared<api::GetNewVideosQuery>(40, toSkip, ref new String(L"US"));
					retval->withCustomListGroup(list, group);
					return retval;
				}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
					for (auto& article : result->items) {
						toFill->Append(ref new Tidal::VideoItemVM(article));
					}
					return toFill->Size < result->totalNumberOfItems;
				}
		);



	}
	else {

		return BuildIncrementalLoadingCollection<api::GetNewVideosQuery>()
			.with_build_query(
				[list, group, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			auto retval = std::make_shared<api::GetNewVideosQuery>(40, toSkip, sessionId, countryCode);
			retval->withCustomListGroup(list, group);
			return retval;
		}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::VideoItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		}
		);


	}
}

Tidal::IncrementalLoadingCollection ^ getNewsAlbumsDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {

		return BuildIncrementalLoadingCollection<api::GetNewAlbumsQuery>()
			.with_build_query(
				[list, group](auto toSkip) {
					auto query = std::make_shared<api::GetNewAlbumsQuery>(40, toSkip, ref new String(L"US"));
					query->withCustomListGroup(list, group);
					return query;
				}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
					for (auto& article : result->items) {
						toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
					}
					return toFill->Size < result->totalNumberOfItems;
				}
			);

		
	}
	else {

		return BuildIncrementalLoadingCollection<api::GetNewAlbumsQuery>()
			.with_build_query(
				[list, group, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
					auto query = std::make_shared<api::GetNewAlbumsQuery>(40, toSkip, sessionId, countryCode);
					query->withCustomListGroup(list, group);
					return query;
				}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		}
		);



	}
}

Tidal::IncrementalLoadingCollection ^ getArtistAlbumsDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return BuildIncrementalLoadingCollection<api::GetArtistAlbumsQuery>()
			.with_build_query(
				[artistId](auto toSkip) {
			return std::make_shared<api::GetArtistAlbumsQuery>(artistId, 40, toSkip, ref new String(L"US"));
		}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		}
		);


	}
	else {

		return BuildIncrementalLoadingCollection<api::GetArtistAlbumsQuery>()
			.with_build_query(
				[artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			return std::make_shared<api::GetArtistAlbumsQuery>(artistId, 40, toSkip, sessionId, countryCode);
		}
			)
			.with_handle_results(
				[](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::AlbumResumeItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		}
		);

	}
}

Tidal::IncrementalLoadingCollection ^ getArtistVideosDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {

		return BuildIncrementalLoadingCollection<api::GetArtistVideosQuery>()
			.with_build_query([artistId](auto toSkip) {
			return std::make_shared<api::GetArtistVideosQuery>(artistId, 40, toSkip, ref new String(L"US"));
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::VideoItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});

	}
	else {
		return BuildIncrementalLoadingCollection<api::GetArtistVideosQuery>()
			.with_build_query([artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			return std::make_shared<api::GetArtistVideosQuery>(artistId, 40, toSkip, sessionId, countryCode);
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::VideoItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});
		
	}
}

Tidal::IncrementalLoadingCollection ^ getSimilarArtistsDataSource(std::int64_t artistId)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {
		return BuildIncrementalLoadingCollection<api::GetSimilarArtistsQuery>()
			.with_build_query([artistId](auto toSkip) {
			return std::make_shared<api::GetSimilarArtistsQuery>(artistId, 40, toSkip, ref new String( L"US"));
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::ArtistItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});
		
	}
	else {
		return BuildIncrementalLoadingCollection<api::GetSimilarArtistsQuery>()
			.with_build_query([artistId, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			return std::make_shared<api::GetSimilarArtistsQuery>(artistId, 40, toSkip, sessionId, countryCode);
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::ArtistItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});

		
	}
}

Tidal::IncrementalLoadingCollection ^ getNewsPlaylistsDataSource(Platform::String^ list, Platform::String^ group)
{
	auto& authSvc = getAuthenticationService();
	if (!authSvc.authenticationState().isAuthenticated()) {

		return BuildIncrementalLoadingCollection<api::GetNewPlaylistsQuery>()
			.with_build_query([list, group](auto toSkip) {
			auto query = std::make_shared<api::GetNewPlaylistsQuery>(40, toSkip, ref new String(L"US"));
			query->withCustomListGroup(list, group);
			return query;
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::PlaylistResumeItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});
		
	}
	else {

		return BuildIncrementalLoadingCollection<api::GetNewPlaylistsQuery>()
			.with_build_query([list, group, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			auto query = std::make_shared<api::GetNewPlaylistsQuery>(40, toSkip, sessionId, countryCode);
			query->withCustomListGroup(list, group);
			return query;
		})
			.with_handle_results([](auto toFill, const auto& result) {
			for (auto& article : result->items) {
				toFill->Append(ref new Tidal::PlaylistResumeItemVM(article));
			}
			return toFill->Size < result->totalNumberOfItems;
		});

		
	}
}

concurrency::task<Platform::Collections::Vector<Tidal::TrackItemVM^>^> getNewsTrackItemsAsync(concurrency::cancellation_token cancelToken, Hat<Platform::String> list, Hat<Platform::String> group)
{
	auto& authSvc = getAuthenticationService();
	auto retval = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	if (authSvc.authenticationState().isAuthenticated()) {
		auto query = std::make_shared<api::GetNewTracksQuery>(25, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		query->withCustomListGroup(list.get(), group.get());
		auto result = co_await query->executeAsync(cancelToken);
		for (auto& article : result->items) {
			auto item = ref new Tidal::TrackItemVM(article);
			retval->Append(item);
			item->AttachTo(retval);
		}
		return retval;
	}
	else {
		auto query = std::make_shared<api::GetNewTracksQuery>(25, 0, ref new String(L"US"));
		query->withCustomListGroup(list.get(), group.get());
		auto result = co_await query->executeAsync(cancelToken);
		for (auto& article : result->items) {
			auto item = ref new Tidal::TrackItemVM(article);
			retval->Append(item);
			item->AttachTo(retval);
		}
		return retval;
	}
}

concurrency::task<std::shared_ptr<std::vector<api::SublistInfo>>> getSublistsAsync(concurrency::cancellation_token cancelToken, Hat<Platform::String> list)
{
	auto& authSvc = getAuthenticationService();
	auto retval = ref new Platform::Collections::Vector<Tidal::TrackItemVM^>();
	if (authSvc.authenticationState().isAuthenticated()) {
		api::GetListSublistsQuery query(list.get(), authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());

		return co_await query.executeAsync(cancelToken);
	}
	else {
		api::GetListSublistsQuery query(list.get(), L"US");

		return co_await query.executeAsync(cancelToken);
	}
}

concurrency::task<Windows::UI::Xaml::Data::ICollectionView^> searchAllAsync(Hat<Platform::String> query, concurrency::cancellation_token cancelToken)
{
	auto& authSvc = getAuthenticationService();
	std::shared_ptr<api::SearchResults> results;
	if (authSvc.authenticationState().isAuthenticated()) {
		api::SearchQuery query(query.get(), L"ARTISTS,ALBUMS,TRACKS,VIDEOS,PLAYLISTS", 3, 0, authSvc.authenticationState().sessionId(), authSvc.authenticationState().countryCode());
		results = co_await query.executeAsync(cancelToken);
	}
	else {
		api::SearchQuery query(query.get(), L"ARTISTS,ALBUMS,TRACKS,VIDEOS,PLAYLISTS", 3, 0, L"US");
		results = co_await query.executeAsync(cancelToken);
	}

	auto groups = ref new Platform::Collections::Vector<Tidal::SearchResultGroup^>();
	{
		auto gp = ref new Tidal::SearchResultGroup();
		gp->Name = L"Artists";
		gp->Icon = L"J";
		gp->Filter = L"ARTISTS";
		gp->Query = query.get();
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
		gp->Query = query.get();
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
		gp->Query = query.get();
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
		gp->Query = query.get();
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
		gp->Query = query.get();
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

		return BuildIncrementalLoadingCollection<api::SearchQuery>()
			.with_build_query([query, filter](auto toSkip) {
			return std::make_shared<api::SearchQuery>(query, filter, 40, toSkip, ref new String(L"US"));
		})
			.with_handle_results([](auto toFill, const auto& result) {
			auto beforeCount = toFill->Size;
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
		});

	}
	else {
		return BuildIncrementalLoadingCollection<api::SearchQuery>()
			.with_build_query([query, filter, sessionId = authSvc.authenticationState().sessionId(), countryCode = authSvc.authenticationState().countryCode()](auto toSkip) {
			return std::make_shared<api::SearchQuery>(query, filter, 40, toSkip, sessionId, countryCode);
		})
			.with_handle_results([](auto toFill, const auto& result) {
			auto beforeCount = toFill->Size;
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
		});
		
	}
}

Tidal::IncrementalLoadingCollection ^ getLocalAlbumsDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Hat<Vector<Object^>> toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = co_await localdata::getImportedAlbumsAsync(toFill->Size, desiredCount, cancelToken);

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
		co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}

Tidal::IncrementalLoadingCollection ^ getLocalPlaylistsDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Hat<Vector<Object^>> toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = co_await localdata::getImportedPlaylistsAsync(toFill->Size, desiredCount, cancelToken);

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
		co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}

Tidal::IncrementalLoadingCollection ^ getLocalTracksDataSource()
{
	return ref new Tidal::IncrementalLoadingCollection([](Hat<Vector<Object^>> toFill, unsigned int desiredCount, cancellation_token cancelToken) ->task<bool> {
		try {
			auto data = co_await localdata::getImportedTracksAsync(toFill->Size, desiredCount, cancelToken);

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
		co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);
		return true;

	});
}
