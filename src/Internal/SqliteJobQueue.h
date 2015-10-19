#pragma once
#include <ppltasks.h>
#include <mutex>
#include <functional>
#include <sqlite3.h>
#include <condition_variable>
#include <queue>
namespace LocalDB {
	namespace details {
		struct SqliteJob {
			std::function<void(sqlite3* db)> job;
			concurrency::task_completion_event<void> tce;
		};
		class SqliteJobQueue
		{
		private:
			std::mutex _mut;
			std::condition_variable _cv;
			concurrency::cancellation_token _cancelToken;
			std::queue<SqliteJob> _jobs;
		public:
			SqliteJobQueue(const concurrency::cancellation_token& cancelToken);
			concurrency::task<void> enqueue(std::function<void(sqlite3* db)>&& job);
			void processUntilCancellation(sqlite3* db);
		};
	}
}


