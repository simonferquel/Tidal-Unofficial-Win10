#include "pch.h"
#include <DBContext.h>
#include <sqlite3.h>
#include <FileUtils.h>
#include <StringUtils.h>
#include <SqliteException.h>
#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include "Internal\SqliteJobQueue.h"
#include "SqliteSmartPtr.h"
#include <map>

// missing stuff from sqlite header

extern "C" {

	/*
	* The value used with sqlite3_win32_set_directory() to specify that
	* the data directory should be changed.
	*/
#ifndef SQLITE_WIN32_DATA_DIRECTORY_TYPE
#  define SQLITE_WIN32_DATA_DIRECTORY_TYPE (1)
#endif

	/*
	* The value used with sqlite3_win32_set_directory() to specify that
	* the temporary directory should be changed.
	*/
#ifndef SQLITE_WIN32_TEMP_DIRECTORY_TYPE
#  define SQLITE_WIN32_TEMP_DIRECTORY_TYPE (2)
#endif
	SQLITE_API int SQLITE_STDCALL sqlite3_win32_set_directory(DWORD type, LPCWSTR zValue);
}


using namespace LocalDB;

struct sqlite3_destructor {
	void operator()(sqlite3* db) {
		if (db) {
			sqlite3_close(db);
		}
	}
};

using sqlite3ptr = std::unique_ptr<sqlite3, sqlite3_destructor>;

std::once_flag g_dbOnce;
class LocalDB::DBContext::impl {
private:
	std::wstring _filePath;
	sqlite3ptr _ctx;
	concurrency::cancellation_token_source _cts;
	std::shared_ptr<LocalDB::details::SqliteJobQueue> _jobQueue;
	std::thread _jobQueueThread;
	std::map<std::string, std::shared_ptr<sqlite3_stmt>> _statementsCache;
public:
	impl(const std::wstring& filePath) : _filePath(filePath) {
		_jobQueue = std::make_shared<details::SqliteJobQueue>(_cts.get_token());
		std::call_once(g_dbOnce, []() {
			sqlite3_win32_set_directory(SQLITE_WIN32_TEMP_DIRECTORY_TYPE, Windows::Storage::ApplicationData::Current->TemporaryFolder->Path->Data());
			sqlite3_initialize();
			
			sqlite3_config(SQLITE_CONFIG_SERIALIZED);
#ifdef _DEBUG			
			sqlite3_config(SQLITE_CONFIG_LOG, [](void* ,int , const char* text) {
				auto wText = convertToWideString(text);
				OutputDebugString(L"SQLITE: ");
				OutputDebugString(wText.c_str());
				OutputDebugString(L"\n");
			}, NULL);
#endif
		});
		auto utf8Path = convertToUtf8String(filePath);
		sqlite3* ctx;

		auto result = sqlite3_open(utf8Path.c_str(), &ctx);
		_ctx.reset(ctx);
		if (result != SQLITE_OK) {
			if (_ctx == nullptr) {
				throw std::bad_alloc();
			}
			else {
				SqliteException ex(ctx);
				throw ex;
			}
		}


		sqlite3_busy_timeout(_ctx.get(), 2000);
		sqlite3_exec(_ctx.get(), "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

		_jobQueueThread = std::thread([queue = _jobQueue, ctx=_ctx.get()]() {
			try {
				queue->processUntilCancellation(ctx);
			}
			catch (concurrency::task_canceled&) {}
		});

	}

	concurrency::task<void> executeAsync(std::function<void(sqlite3* db)>&& job) {
		return _jobQueue->enqueue(std::move(job));
	}

	~impl() {
		_cts.cancel();
		if (_jobQueueThread.joinable()) {
			_jobQueueThread.join();
		}
		_ctx = nullptr;
	}

	auto_reset_statement_ptr getCachedStatement(const std::string& name) {
		auto it = _statementsCache.find(name);
		if (it == _statementsCache.end()) {
			return auto_reset_statement_ptr(nullptr);
		}
		return it->second;
	}
	void registerCachedStatement(const std::string& name, const std::shared_ptr<sqlite3_stmt>& statement) {
		_statementsCache.insert_or_assign(name, statement);
	}
};



concurrency::task<void> LocalDB::DBContext::executeAsync(std::function<void(sqlite3* db)>&& job) {
	return _impl->executeAsync(std::move(job));
}

LocalDB::DBContext::DBContext(const wchar_t * name) : _impl(std::make_shared<impl>(makeLocalStoragePath(name)))
{
}

LocalDB::DBContext::~DBContext()
{
}



LocalDB::auto_reset_statement_ptr LocalDB::DBContext::getCachedStatement(const std::string& name) {
	return _impl->getCachedStatement(name);
}
void LocalDB::DBContext::registerCachedStatement(const std::string& name, const std::shared_ptr<sqlite3_stmt>& statement) {
	_impl->registerCachedStatement(name, statement);
}