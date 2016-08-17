#pragma once
#include "IncrementalLoadingCollection.h"
#include "TrackItemVM.h"
#include <ppltasks.h>
#include <Api/SublistInfo.h>
//Platform::String^ list, Platform::String^ group
Tidal::IncrementalLoadingCollection^ getNewsPromotionsDataSource(Platform::String^ list = L"NEWS");
Tidal::IncrementalLoadingCollection^ getNewsVideosDataSource(Platform::String^ list = L"featured", Platform::String^ group = L"new");
Tidal::IncrementalLoadingCollection^ getNewsAlbumsDataSource(Platform::String^ list = L"featured", Platform::String^ group = L"new");
Tidal::IncrementalLoadingCollection^ getArtistAlbumsDataSource(std::int64_t artistId);
Tidal::IncrementalLoadingCollection^ getArtistVideosDataSource(std::int64_t artistId);
Tidal::IncrementalLoadingCollection^ getSimilarArtistsDataSource(std::int64_t artistId);
Tidal::IncrementalLoadingCollection^ getNewsPlaylistsDataSource(Platform::String^ list = L"featured", Platform::String^ group = L"new");
concurrency::task<Platform::Collections::Vector<Tidal::TrackItemVM^>^> getNewsTrackItemsAsync(concurrency::cancellation_token cancelToken, Platform::String^ list = L"featured", Platform::String^ group = L"new");
concurrency::task<std::shared_ptr<std::vector<api::SublistInfo>>> getSublistsAsync(concurrency::cancellation_token cancelToken, Platform::String^ list = L"featured");

concurrency::task<Windows::UI::Xaml::Data::ICollectionView^> searchAllAsync(Platform::String^, concurrency::cancellation_token cancelToken);
Tidal::IncrementalLoadingCollection^ getFilteredSearchSource(Platform::String^ query, Platform::String^ filter);

Tidal::IncrementalLoadingCollection^ getLocalAlbumsDataSource();
Tidal::IncrementalLoadingCollection^ getLocalPlaylistsDataSource();
Tidal::IncrementalLoadingCollection^ getLocalTracksDataSource();