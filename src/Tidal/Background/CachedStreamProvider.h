#pragma once
#include <ppltasks.h>
#include <cstdint>
#include <SoundQuality.h>
class track_unavailable_for_streaming{};
class track_download_timeout {};
struct cached_stream_info {
	Platform::String^ contentType;
	Windows::Storage::Streams::IRandomAccessStream^ stream;
	bool isImport;
	SoundQuality soundQuality;
};

concurrency::task<cached_stream_info> resolveCachedStreamAsync(std::int64_t id, concurrency::cancellation_token cancelToken);