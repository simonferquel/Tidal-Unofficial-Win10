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
#include <localdata/GetTrackImportQueueQuery.h>
#include "WebStream.h"
#include <ObfuscateStream.h>
#include "../Mediators.h"
#include <localdata\InsertPlaybackReportQuery.h>
#include <Api/Config.h>
#include "../AuthenticationService.h"

concurrency::task<void> handleJobAsync(localdata::track_import_job job, concurrency::cancellation_token cancelToken) {
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto importQualityTxt = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"importQuality"));
	if (!importQualityTxt) {
		importQualityTxt = L"LOSSLESS"; 
	}
	auto importQuality = parseSoundQuality(importQualityTxt);

	auto existing = co_await localdata::getExistingImportedTrackIfExistsAsync(localdata::getDb(), job.id, cancelToken);
	bool existingValid = false;
	if (existing && existing->quality == static_cast<std::int32_t>(importQuality)) {
		
	}
	else if (existing) {

		co_await localdata::deleteImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
	}
	if (existingValid) {

	}
	else if (job.local_size == job.server_size && job.server_size != 0) {
		co_await localdata::transformTrackImportJobToImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
	}
	else {
		
		bool unauthorized = false;
		api::GetTrackStreamUrlQuery q(dynamic_cast<Platform::String^>(settingsValues->Lookup(L"SessionId")), dynamic_cast<Platform::String^>(settingsValues->Lookup(L"CountryCode")), job.id, importQualityTxt, true);
		try {
			auto urlInfo = co_await q.executeAsync(cancelToken);

			//auto streamReference = Windows::Storage::Streams::RandomAccessStreamReference::CreateFromUri(ref new Windows::Foundation::Uri(tools::strings::toWindowsString(urlInfo->url)));
			auto webStream = co_await WebStream::CreateWebStreamAsync(tools::strings::toWindowsString(urlInfo->url), cancelToken);
			if (webStream->Size != job.server_size || job.quality != static_cast<std::int32_t>(importQuality) || webStream->Modified.UniversalTime != job.server_timestamp) {
				job.local_size = 0;
				job.server_size = webStream->Size;
				job.server_timestamp = webStream->Modified.UniversalTime;
				job.quality = static_cast<std::int32_t>(importQuality);
				co_await LocalDB::executeAsyncWithCancel<localdata::track_import_jobUpdateDbQuery>(localdata::getDb(), cancelToken, job);
			}
			auto folder = co_await concurrency::create_task(Windows::Storage::ApplicationData::Current->LocalFolder->CreateFolderAsync(L"imports", Windows::Storage::CreationCollisionOption::OpenIfExists));
			auto file = co_await concurrency::create_task(folder->CreateFileAsync(job.id.ToString(), Windows::Storage::CreationCollisionOption::OpenIfExists));
			Windows::Storage::Streams::IRandomAccessStream^ fileStream = co_await concurrency::create_task(file->OpenAsync(Windows::Storage::FileAccessMode::ReadWrite));
			if (fileStream->Position != job.local_size) {
				fileStream->Seek(job.local_size);
			}
			if (webStream->Position != job.local_size) {
				webStream->Seek(job.local_size);
			}
			if (job.obuscated) {
				fileStream = ref new ObfuscateStream(fileStream, job.id);
			}
			auto buffer = ref new Windows::Storage::Streams::Buffer(128 * 1024);
			while (job.local_size < job.server_size) {
				buffer->Length = 0;
				unsigned int count = 128 * 1024;
				if (job.server_size - job.local_size < count) {
					count = job.server_size - job.local_size;
				}
				auto readBuffer = co_await concurrency::create_task(webStream->ReadAsync(buffer, count, Windows::Storage::Streams::InputStreamOptions::None), cancelToken);
				co_await concurrency::create_task(fileStream->WriteAsync(readBuffer));
				co_await concurrency::create_task(fileStream->FlushAsync());
				job.local_size += readBuffer->Length;
				co_await LocalDB::executeAsync<localdata::track_import_jobUpdateDbQuery>(localdata::getDb(), job);

				ImportProgress progress;
				progress.localSize = job.local_size;
				progress.serverSize = job.server_size;
				progress.trackId = job.id;
				getTrackImportProgress().raise(progress);

			}

			co_await localdata::transformTrackImportJobToImportedTrackAsync(localdata::getDb(), job.id, cancelToken);
			
		}
		catch (api::statuscode_exception& ex) {
			if (ex.getStatusCode() == Windows::Web::Http::HttpStatusCode::Unauthorized) {
				unauthorized = true;
			}
			else {
				throw;
			}
		}
		if (unauthorized) {
			co_await localdata::cancelTrackImportJobAsync(localdata::getDb(), job.id, cancelToken);
		}
	}

	
	getTrackImportComplete().raise(job.id);
}
concurrency::task<bool> handleNextJobAsync(concurrency::cancellation_token cancelToken) {
	auto job = co_await LocalDB::executeAsyncWithCancel<localdata::GetNextTrackImportJobQuery>(localdata::getDb(), cancelToken);
	if (job->size() == 0) {
		return false;
	}
	// do nothing for now 
	co_await tools::async::retryWithDelays([cancelToken]() {
		return LocalDB::executeAsyncWithCancel<localdata::GetNextTrackImportJobQuery>(localdata::getDb(), cancelToken)
			.then([cancelToken](std::shared_ptr<std::vector<localdata::track_import_job>> job) {

			return handleJobAsync(job->at(0), cancelToken);
		});
	}, tools::time::ToWindowsTimeSpan(std::chrono::seconds(1)), cancelToken);

	return true;
}
concurrency::task<void> SendPlaybackReportsAsync(std::shared_ptr<std::vector<localdata::playback_reports>> reports) {
	using namespace Windows::Web::Http;
	try {
		auto jarr = web::json::value::array();
		std::vector<std::int64_t> ids;
		for (auto& report : *reports) {
			jarr[jarr.size()] = web::json::value::parse(report.json);
			ids.push_back(report.id);
		}
		auto completePayload = jarr.serialize();

		auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
		filter->AllowUI = false;
		auto client = ref new Windows::Web::Http::HttpClient(filter);
		client->DefaultRequestHeaders->UserAgent->Clear();
		client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));
		std::wstring urlBuilder(api::config::apiLocationPrefix()->Data());
		urlBuilder.append(L"report/offlineplays");
		urlBuilder.append(L"?countryCode=");
		if (!getAuthenticationService().authenticationState().isAuthenticated()) {
			return;
		}
		urlBuilder.append(Windows::Foundation::Uri::EscapeComponent(getAuthenticationService().authenticationState().countryCode())->Data());

		auto message = ref new HttpRequestMessage(HttpMethod::Post, ref new Windows::Foundation::Uri(tools::strings::toWindowsString(urlBuilder)));
		message->Headers->Append(L"X-Tidal-SessionId", getAuthenticationService().authenticationState().sessionId());
		message->Content = ref new HttpStringContent(tools::strings::toWindowsString(completePayload), Windows::Storage::Streams::UnicodeEncoding::Utf8, L"application/json");
		auto response = co_await concurrency::create_task(client->SendRequestAsync(message));
		auto result = co_await concurrency::create_task(response->Content->ReadAsStringAsync());
		response->EnsureSuccessStatusCode();
		co_await localdata::DeletePlaybackReportsAsync(ids);
		

	}
	catch(...){}
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
			bool treatedJob = co_await handleNextJobAsync(cancelToken);
			if (!treatedJob) {
				auto reports = co_await localdata::GetPlaybackReportsAsync();
				if (reports&&reports->size() > 0) {
					SendPlaybackReportsAsync(reports);
				}
				break;
			}
		}
		auto delayTask = tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(60)), cancelToken);
		std::vector<concurrency::task<void>> waiters{ wakeupTask, delayTask };
		co_await concurrency::when_any(waiters.begin(), waiters.end());

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
