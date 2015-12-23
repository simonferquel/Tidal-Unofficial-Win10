#pragma once
#include <localdb/DBContext.h>

namespace localdata {
	concurrency::task<void> initializeDbAsync();
	LocalDB::DBContext getDb();
}