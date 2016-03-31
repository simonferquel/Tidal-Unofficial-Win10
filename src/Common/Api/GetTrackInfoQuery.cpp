#include "pch.h"
#include "GetTrackInfoQuery.h"
#include "../tools/StringUtils.h"
using namespace api;
concurrency::task<std::shared_ptr<TrackInfo>> api::GetTrackInfoQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<TrackInfo>(jsonVal);
}
