#pragma once
#include <cstdint>
#include <ppltasks.h>
#include <memory>
namespace localdata {
	struct storage_statistics {
		std::int64_t totalCacheSize;
		std::int64_t cachedNotPlayedFor8DaysSize;
		std::int64_t totalImportSize;
		std::int64_t importNotPlayedFor8DaysSize;
	};

	concurrency::task<std::shared_ptr<storage_statistics>> getStorageStatisticsAsync(concurrency::cancellation_token cancelToken);
}