#pragma once
#include "localdata/Entities.h"
#include <localdb/DBContext.h>
namespace cache {
	concurrency::task<localdata::cached_track> getOrCreateCachedTrackInfoAsync( LocalDB::DBContext ctx, std::int64_t id, concurrency::cancellation_token cancelToken);
	concurrency::task<void> updateCachedTrackInfoAsync(LocalDB::DBContext ctx, const localdata::cached_track& trackIndo, concurrency::cancellation_token cancelToken);
}