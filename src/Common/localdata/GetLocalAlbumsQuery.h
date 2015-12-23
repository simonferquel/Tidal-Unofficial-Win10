#pragma once
#include "Entities.h"
namespace localdata {
	concurrency::task<std::shared_ptr<std::vector<localdata::imported_album>>> getImportedAlbumsAsync(int skip, int take, concurrency::cancellation_token cancelToken);
}