#pragma once
#include <ppltasks.h>
namespace api {
	concurrency::task<Platform::String^> EnsureCoverInCacheAsync(std::int64_t id, Platform::String^ imageId, concurrency::cancellation_token cancelToken);
	concurrency::task<Platform::String^> EnsureCoverInCacheAsync(std::int64_t id, Platform::String^ imageId, int width, int height, concurrency::cancellation_token cancelToken);
	concurrency::task<Platform::String^> GetCoverUriAndFallbackToWebAsync(std::int64_t id, Platform::String^ imageId, int width, int height, concurrency::cancellation_token cancelToken);

	Platform::String^ getOfflineCoverUrl(std::int64_t id, int width, int height);


	concurrency::task<Platform::String^> EnsurePlaylistCoverInCacheAsync(const std::wstring& id, Platform::String^ imageId, int width, int height, concurrency::cancellation_token cancelToken);
	concurrency::task<Platform::String^> GetPlaylistCoverUriAndFallbackToWebAsync(const std::wstring& id, Platform::String^ imageId, int width, int height, concurrency::cancellation_token cancelToken);

	Platform::String^ getPlaylistOfflineCoverUrl(const std::wstring& id, int width, int height);
}