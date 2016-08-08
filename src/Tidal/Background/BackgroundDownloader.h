#pragma once
#include <memory>
#include <ppltasks.h>
#include <mutex>
#include <thread>
class BackgroundDownloader : public std::enable_shared_from_this<BackgroundDownloader> {
private:
	std::mutex _mutex;
	concurrency::cancellation_token_source _cts;
	concurrency::task<void> _downloadTask;
	concurrency::task_completion_event<void> _wakeupTce;
	concurrency::task<void> startDownloadLoopAsync(concurrency::cancellation_token cancelToken);
public:
	void initialize();
	void shutdown();
	void wakeup();
};