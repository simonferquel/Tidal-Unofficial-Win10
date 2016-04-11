#include "pch.h"
#include "GetAlbumResumeQuery.h"
#include "../tools/StringUtils.h"

using namespace api;
api::GetAlbumResumeQuery::GetAlbumResumeQuery(std::int64_t id, Platform::String ^ sessionId, Platform::String ^ countryCode)
	:QueryBase(sessionId, countryCode), _id(id)
{
}

api::GetAlbumResumeQuery::GetAlbumResumeQuery(std::int64_t id, Platform::String ^ countryCode)
	:  _id(id)
{
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetAlbumResumeQuery::url() const
{
	std::wstring result = L"albums/";
	result.append(std::to_wstring(_id));
	return result;
}

concurrency::task<std::shared_ptr<AlbumResume>> api::GetAlbumResumeQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return getAsync(cancelToken).then([](Platform::String^ json) {
		tools::strings::WindowsWIStream stream(json);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<AlbumResume>(jsonVal);
	});
}
