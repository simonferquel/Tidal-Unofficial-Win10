#include "pch.h"
#include "WebStream.h"
#include <string>
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace concurrency;
using namespace Windows::Storage::Streams;

concurrency::task<Windows::Storage::Streams::IInputStream^> WebStream::EnsureInputStreamAsync(concurrency::cancellation_token cancelToken)
{
	if (_currentStream) {
		return concurrency::task_from_result(_currentStream);
	}
	else {
		auto message = ref new HttpRequestMessage(HttpMethod::Get, _uri);
		if (_position != 0) {
			std::wstring headerValue = L"bytes=";
			headerValue.append(std::to_wstring(_position));
			headerValue.append(L"-");
			message->Headers->Append(L"Range", ref new Platform::String(headerValue.c_str()));
		}
		auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
		filter->AllowUI = false;
		auto client = ref new Windows::Web::Http::HttpClient(filter);
		client->DefaultRequestHeaders->UserAgent->Clear();
		client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));
		return  create_task(client->SendRequestAsync(message, HttpCompletionOption::ResponseHeadersRead), cancelToken)
			.then([this, cancelToken](HttpResponseMessage^ response) {
			return create_task(response->Content->ReadAsInputStreamAsync(), cancelToken)
				.then([this](IInputStream^ result) {
				_currentStream = result;
				return result;
			});
		});
	}
}

concurrency::task<Windows::Storage::Streams::IBuffer^> WebStream::DoReadAsync(Windows::Storage::Streams::IBuffer ^ buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options, progress_reporter<unsigned int> reporter, concurrency::cancellation_token cancelToken)
{
	return EnsureInputStreamAsync(cancelToken).then([this, cancelToken, buffer, count, options, reporter](IInputStream^ stream) {
		auto asyncOp = _currentStream->ReadAsync(buffer, count, options);
		asyncOp->Progress = ref new Windows::Foundation::AsyncOperationProgressHandler<IBuffer^, unsigned int>([reporter](Windows::Foundation::IAsyncOperationWithProgress<IBuffer^, unsigned int>^ asyncInfo, unsigned int progressInfo) {
			reporter.report(progressInfo);
		});
		return create_task(asyncOp, cancelToken).then([this](concurrency::task<IBuffer^> resultTask) {
			try {
				auto result = resultTask.get();
				_position += result->Length;
				return result;
			}
			catch (...) {
				_currentStream = nullptr;
				throw;
			}
		});
	});
	
}

Windows::Foundation::IAsyncOperationWithProgress<Windows::Storage::Streams::IBuffer ^, unsigned int> ^ WebStream::ReadAsync(Windows::Storage::Streams::IBuffer ^buffer, unsigned int count, Windows::Storage::Streams::InputStreamOptions options)
{
	return create_async([this, buffer, count, options](progress_reporter<unsigned int> reporter, concurrency::cancellation_token cancelToken) {
		return DoReadAsync(buffer, count, options, reporter, cancelToken);
	});
}

Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> ^ WebStream::WriteAsync(Windows::Storage::Streams::IBuffer ^buffer)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

Windows::Foundation::IAsyncOperation<bool> ^ WebStream::FlushAsync()
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

Windows::Storage::Streams::IInputStream ^ WebStream::GetInputStreamAt(unsigned long long position)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

Windows::Storage::Streams::IOutputStream ^ WebStream::GetOutputStreamAt(unsigned long long position)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

void WebStream::Seek(unsigned long long position)
{
	_position = position;
	_currentStream = nullptr;
}

Windows::Storage::Streams::IRandomAccessStream ^ WebStream::CloneStream()
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}

concurrency::task<WebStream^> WebStream::CreateWebStreamAsync(Hat<Platform::String> url, concurrency::cancellation_token cancelToken)
{
	auto uri = ref new Uri(url.get());
	auto request = ref new HttpRequestMessage(HttpMethod::Head, uri);
	auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
	filter->AllowUI = false;
	auto client = ref new Windows::Web::Http::HttpClient(filter);
	client->DefaultRequestHeaders->UserAgent->Clear();
	client->DefaultRequestHeaders->UserAgent->Append(ref new Windows::Web::Http::Headers::HttpProductInfoHeaderValue(L"Tidal-Unofficial.Windows10", Windows::System::Profile::AnalyticsInfo::VersionInfo->DeviceFamily));
	auto response = co_await create_task(client->SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead), cancelToken);
	auto modified = response->Content->Headers->LastModified->Value;
	auto size = response->Content->Headers->ContentLength->Value;
	return ref new WebStream(uri, size, modified);
}
