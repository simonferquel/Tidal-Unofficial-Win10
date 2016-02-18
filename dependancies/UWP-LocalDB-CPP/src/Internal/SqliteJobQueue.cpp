#include "pch.h"
#include "SqliteJobQueue.h"
using namespace LocalDB::details;

LocalDB::details::SqliteJobQueue::SqliteJobQueue(const concurrency::cancellation_token & cancelToken) : _cancelToken(cancelToken)
{
	cancelToken.register_callback([&cv = _cv]() {
		cv.notify_one();
	});
}

concurrency::task<void> LocalDB::details::SqliteJobQueue::enqueue(std::function<void(sqlite3*db)>&& job)
{
	std::unique_lock<std::mutex> lg(_mut);
	SqliteJob j;
	j.job = std::move(job);
	auto task = concurrency::create_task(j.tce);
	_jobs.push(std::move(j));
	_cv.notify_one();
	return task;
}

void LocalDB::details::SqliteJobQueue::processUntilCancellation(sqlite3 * db)
{
	for (;;) {
		SqliteJob toProcess;
		{
			std::unique_lock<std::mutex> lg(_mut);
			while (_jobs.size() == 0) {
				if (_cancelToken.is_canceled()) {
					while (_jobs.size() != 0) {
						_jobs.front().tce.set_exception(concurrency::task_canceled());
						_jobs.pop();
					}
					throw concurrency::task_canceled();
				}
				_cv.wait(lg);
				
			}
			toProcess = _jobs.front();
			_jobs.pop();
		}
		try {
			toProcess.job(db);
			toProcess.tce.set();
		}
		catch (Platform::Exception^ ex) {
			toProcess.tce.set_exception(ex);
		}
		catch (...) {
			toProcess.tce.set_exception(std::current_exception());
		}
		
	}
}
