#pragma once
#include <exception>
namespace api {
	class timeout_exception : public std::runtime_error {
	private:
		std::wstring _url;
	public:
		const std::wstring& url()const { return _url; }
		explicit timeout_exception(const std::wstring& url);
	};

	class statuscode_exception : public std::runtime_error {
	private:
		std::wstring _url;
		Windows::Web::Http::HttpStatusCode _statusCode;
		Platform::String^ _responseText;
	public:
		const std::wstring& url()const { return _url; }
		Windows::Web::Http::HttpStatusCode getStatusCode()const { return _statusCode; }
		Platform::String^ getResponseText()const { return _responseText; }
		statuscode_exception(const std::wstring& url, Windows::Web::Http::HttpStatusCode statusCode, Platform::String^ responseText);
	};
}