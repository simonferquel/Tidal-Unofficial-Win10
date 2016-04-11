#include "pch.h"
#include "GetTrackStreamUrlQuery.h"
#include <tools/StringUtils.h>
using namespace api;
api::GetTrackStreamUrlQuery::GetTrackStreamUrlQuery(Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id, Platform::String ^ soundQuality, bool offline) :
	QueryBase(sessionId, countryCode), _trackId(id), _offline(offline)
{
	addQueryStringParameter(L"soundQuality", soundQuality);
}

std::wstring api::GetTrackStreamUrlQuery::url() const
{
	std::wstring url(L"tracks/");
	url.append(_trackId.ToString()->Data());
	if (_offline) {

		url.append(L"/offlineUrl");
	}
	else {
		url.append(L"/streamUrl");
	}
	return url;
}

concurrency::task<std::shared_ptr<UrlInfo>> api::GetTrackStreamUrlQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return getAsync(cancelToken).then([](Platform::String^ json) {
		tools::strings::WindowsWIStream stream(json);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<UrlInfo>(jsonVal);
	});
}
