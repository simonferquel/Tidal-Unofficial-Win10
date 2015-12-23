#include "pch.h"
#include "ApiErrors.h"
#include "../tools/StringUtils.h"
api::timeout_exception::timeout_exception(const std::wstring& url) : std::runtime_error(tools::strings::convertToUtf8String(url)+" timeout"),
_url(url){

}

api::statuscode_exception::statuscode_exception(const std::wstring & url, Windows::Web::Http::HttpStatusCode statusCode, Platform::String ^ responseText)
	:
	std::runtime_error(std::string("[")+tools::strings::convertToUtf8String(((int)statusCode).ToString())+"] "+ tools::strings::convertToUtf8String(url)+ " : "+ tools::strings::convertToUtf8String(responseText)),
	_url(url),
	_statusCode(statusCode),
	_responseText(responseText)
{
}