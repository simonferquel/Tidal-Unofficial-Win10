#include "pch.h"
#include "BackgroundDownloader.h"
#include <localdata/db.h>
#include <localdata/GetNextTrackImportJobQuery.h>
#include <tools/AsyncHelpers.h>
#include <tools/TimeUtils.h>
#include <tools/StringUtils.h>
#include <soundquality.h>
#include <Api/GetTrackStreamUrlQuery.h>
#include <Api/UrlInfo.h>
#include <Api/ApiErrors.h>
#include "WebStream.h"

concurrency::task<void> handleJobAsync(localdata::track_import_job job, concurrency::cancellation_token cancelToken) {

	await localdata::deleteImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
	if (job.local_size == job.server_size && job.server_size != 0) {
		await localdata::transformTrackImportJobToImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
	}
	else {
		auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
		auto importQualityTxt = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"importQuality"));
		if (!importQualityTxt) {
			importQualityTxt = L"LOSSLESS";
		}
		auto importQuality = parseSoundQuality(importQualityTxt);
		bool unauthorized = false;
		api::GetTrackStreamUrlQuery q(dynamic_cast<Platform::String^>(settingsValues->Lookup(L"SessionId")), dynamic_cast<Platform::String^>(settingsValues->Lookup(L"CountryCode")), job.id, importQualityTxt, true);
		try {
			auto urlInfo = await q.executeAsync(cancelToken);

			//auto streamReference = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(ref new Windows::Foundation::Uri(tools::strings::toWindowsString(urlInfo->url)));
			auto webStream = await WebStream::CreateWebStreamAsync(tools::strings::toWindowsString(urlInfo->url), cancelToken);
			if (webStream->Size != job.server_size || job.quality != static_cast<std::int32_t>(importQuality) || webStream->Modified.UniversalTime != job.server_timestamp) {
				job.local_size = 0;
				job.server_size = webStream->Size;
				job.server_timestamp = webStream->Modified.UniversalTime;
				job.quality = static_cast<std::int32_t>(importQuality);
				await LocalDB::executeAsyncWithCancel<localdata::track_import_jobUpdateDbQuery>(localdata::getDb(), cancelToken, job);
			}
			auto folder = await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"imports", Windows::Storage::CreationCollisionOption::OpenIfExists));
			auto file = await concurrency::create_task(folder->CreateFileAsync(job.id.ToString(), Windows::Storage::CreationCollisionOption::OpenIfExists));
			auto fileStream = await concurrency::create_task(file->OpenAsync(Windows::Storage::FileAccessMode::ReadWrite));
			if (fileStream->Position != job.local_size) {
				fileStream->Seek(job.local_size);
			}
			if (webStream->Position != job.local_size) {
				webStream->Seek(job.local_size);
			}

			auto buffer = ref new Windows::Storage::Streams::Buffer(128 * 1024);
			while (job.local_size < job.server_size) {
				buffer->Length = 0;
				unsigned int count = 128 * 1024;
				if (job.server_size - job.local_size < count) {
					count = job.server_size - job.local_size;
				}
				auto readBuffer = await concurrency::create_task(webStream->ReadAsync(buffer, count, Windows::Storage::Streams::InputStreamOptions::None), cancelToken);
				await concurrency::create_task(fileStream->WriteAsync(readBuffer));
				await concurrency::create_task(fileStream->FlushAsync());
				job.local_size += readBuffer->Length;
				await LocalDB::executeAsync<localdata::track_import_jobUpdateDbQuery>(localdata::getDb(), job);

				auto progressSet = ref new Windows::Foundation::Collections::ValueSet();
				progressSet->Insert(L"request", L"track_import_progress");
				progressSet->Insert(L"track_id", job.id);
				progressSet->Insert(L"local_size", job.local_size);
				progressSet->Insert(L"server_size", job.server_size);
				Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToForeground(progressSet);

			}

			await localdata::transformTrackImportJobToImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
			
		}
		catch (api::statuscode_exception& ex) {
			if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::Unauthorized) {
				unauthorized = true;
			}
		}
		if (unauthorized) {
			await localdata::cancelTrackImportJobAsync(localdata::getDb(), job.id, cancelToken);
		}
	}

	auto ackSet = ref new Windows::Foundation::Collections::ValueSet();
	ackSet->Insert(L"request", L"track_import_finished");
	ackSet->Insert(L"track_id", job.id);
	Windows::Media::Playback::BackgroundMediaPlayer::SendMessageToForeground(ackSet);
}
concurrency::task<bool> handleNextJobAsync(concurrency::cancellation_token cancelToken) {
	auto job = await LocalDB::executeAsyncWithCancel<localdata::GetNextTrackImportJobQuery>(localdata::getDb(), cancelToken);
	if (job->size() == 0) {
		return false;
	}
	// do nothing for now 
	await tools::async::retryWithDelays([cancelToken]() {
		return LocalDB::executeAsyncWithCancel<localdata::GetNextTrackImportJobQuery>(localdata::getDb(), cancelToken)
			.then([cancelToken](std::shared_ptr<std::vector<localdata::track_import_job>> job) {

			return handleJobAsync(job->at(0), cancelToken);
		});
	}, tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);

	return true;
}
concurrency::task<void> BackgroundDownloader::startDownloadLoopAsync(concurrency::cancellation_token cancelToken)
{
	while (!cancelToken.is_canceled()) {
		concurrency::task<void> wakeupTask;
		{

			std::lock_guard<std::mutex> lg(_mutex);
			_wakeupTce = concurrency::task_completion_event<void>();
			wakeupTask = concurrency::create_task(_wakeupTce);
		}

		while (!cancelToken.is_canceled()) {
			bool treatedJob = await handleNextJobAsync(cancelToken);
			if (!treatedJob) {
				break;
			}
		}
		auto delayTask = tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(60)), cancelToken);
		std::vector<concurrency::task<void>> waiters{ wakeupTask, delayTask };
		await concurrency::when_any(waiters.begin(), waiters.end());

	}
}

void BackgroundDownloader::initialize()
{
	_downloadTask = startDownloadLoopAsync(_cts.get_token());
}

void BackgroundDownloader::shutdown()
{
	_cts.cancel();
	try {
		_downloadTask.get();
	}
	catch (concurrency::task_canceled&) {}
	catch (Platform::OperationCanceledException^) {}
}

void BackgroundDownloader::wakeup()
{
	std::lock_guard<std::mutex> lg(_mutex);
	_wakeupTce.set();
}
