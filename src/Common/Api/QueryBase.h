#pragma once
#include <ppltasks.h>
namespace api {
	class QueryBase
	{
	private:
		Platform::Collections::Map<Platform::String^, Platform::String^>^ _requestContent;
		Platform::Collections::Map<Platform::String^, Platform::String^>^ _queryString;
	protected:
		virtual std::wstring url() const = 0;
		virtual Windows::Foundation::TimeSpan timeout() const;
		void addBodyContent(Platform::String^ key, Platform::String^ value);
		void addQueryStringParameter(Platform::String^ key, Platform::String^ value);
	public:
		QueryBase();
		QueryBase(Platform::String^ sessionId, Platform::String^ countryCode);
		concurrency::task<Platform::String^> getAsync(concurrency::cancellation_token cancelToken);
		concurrency::task<Platform::String^> deleteAsync(concurrency::cancellation_token cancelToken);
		concurrency::task<Platform::String^> postAsync(concurrency::cancellation_token cancelToken);
		virtual ~QueryBase() = default;
	};

}