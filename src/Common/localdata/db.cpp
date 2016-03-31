#include "pch.h"
#include "db.h"
#include <mutex>
#include "TidalSchema.h"

std::unique_ptr<LocalDB::DBContext> g_db;

concurrency::task<void> localdata::initializeDbAsync()
{
	g_db = std::make_unique<LocalDB::DBContext>(L"localdata.db");
	auto schema = std::make_shared<TidalSchema>(*g_db);
	co_await schema->initializeAsync(concurrency::cancellation_token::none());
}

LocalDB::DBContext localdata::getDb()
{
	return *g_db;
}
