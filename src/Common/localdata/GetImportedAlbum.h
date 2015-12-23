#pragma once
#include "Entities.h"
namespace localdata {
	concurrency::task<std::shared_ptr<imported_album>> getImportedAlbumIfExistsAsync(std::int64_t id, concurrency::cancellation_token cancelToken);
}