#include "pch.h"
#include "GetVideoUrlQuery.h"
#include "../tools/StringUtils.h"
using namespace api;
api::GetVideoUrlQuery::GetVideoUrlQuery(Platform::String ^ sessionId, Platform::String ^ countryCode, Platform::String ^ videoId)
	: QueryBase(sessionId, countryCode), _videoId(videoId)
{
}

std::wstring api::GetVideoUrlQuery::url() const
{
	std::wstring url = L"videos/";
	url.append(_videoId->Data());
	url.append(L"/streamurl");
	return url;
}

concurrency::task<std::shared_ptr<UrlInfo>> api::GetVideoUrlQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<UrlInfo>(jsonVal);
}
