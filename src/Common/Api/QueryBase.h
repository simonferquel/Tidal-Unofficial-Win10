#pragma once
#include <ppltasks.h>
#include <memory>
namespace api {
	struct ResponseHolder {
		Windows::Web::Http::HttpResponseMessage^ response;
	};
	class QueryBase
	{
	private:
		Platform::Collections::Map<Platform::String^, Platform::String^>^ _requestContent;
		Platform::Collections::Map<Platform::String^, Platform::String^>^ _queryString;
		Platform::Collections::Map<Platform::String^, Platform::String^>^ _customHeaders;
	protected:
		virtual std::wstring url() const = 0;
		virtual Windows::Foundation::TimeSpan timeout() const;
		void addBodyContent(Platform::String^ key, Platform::String^ value);
		void addQueryStringParameter(Platform::String^ key, Platform::String^ value);
		void addHeader(Platform::String^ key, Platform::String^ value);
	public:
		QueryBase();
		QueryBase(Platform::String^ sessionId, Platform::String^ countryCode);
		concurrency::task<Platform::String^> getAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> = nullptr);
		concurrency::task<Platform::String^> deleteAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> = nullptr);
		concurrency::task<Platform::String^> postAsync(concurrency::cancellation_token cancelToken, std::shared_ptr<ResponseHolder> = nullptr);
		virtual ~QueryBase() = default;
	};

}