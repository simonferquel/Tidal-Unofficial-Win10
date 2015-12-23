#include "pch.h"
#include "CachedTrackManager.h"
#include "localdata/GetCachedTrackInfoQuery.h"

concurrency::task<localdata::cached_track> cache::getOrCreateCachedTrackInfoAsync( LocalDB::DBContext ctx, std::int64_t id, concurrency::cancellation_token cancelToken)
{
	localdata::cached_track result;
	await ctx.executeAsync([id, ctx, &result, cancelToken](sqlite3* db) {
		auto localCtx = ctx;
		LocalDB::SynchronousTransactionScope trans(localCtx, db);
		auto existing = LocalDB::executeSynchronouslyWithCancel<localdata::GetCachedTrackInfoQuery>(localCtx, db, cancelToken, id);
		if (existing->size() > 0) {
			result = *existing->begin();
		}
		else {
			result.id = id;
			result.local_size = 0;
			result.quality = 0;
			result.server_size = 0;
			result.server_timestamp = 0;
			LocalDB::executeSynchronouslyWithCancel<localdata::cached_trackInsertDbQuery>(localCtx, db, cancelToken, result);
		}

		trans.commit();
	});
	return result;
}

concurrency::task<void> cache::updateCachedTrackInfoAsync(LocalDB::DBContext ctx, const localdata::cached_track & trackIndo, concurrency::cancellation_token cancelToken)
{
	return ctx.executeAsync([trackIndo, ctx,  cancelToken](sqlite3* db) {
		auto localCtx = ctx;
		LocalDB::SynchronousTransactionScope trans(localCtx, db);

		LocalDB::executeSynchronouslyWithCancel<localdata::cached_trackUpdateDbQuery>(localCtx, db, cancelToken, trackIndo);

		trans.commit();
	});
}
