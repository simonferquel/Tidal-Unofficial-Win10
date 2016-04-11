#include "pch.h"
#include "GetTrackInfoQuery.h"
#include "../tools/StringUtils.h"
using namespace api;
concurrency::task<std::shared_ptr<TrackInfo>> api::GetTrackInfoQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return getAsync(cancelToken).then([](Platform::String^ json) {
		tools::strings::WindowsWIStream stream(json);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<TrackInfo>(jsonVal);
	});
}
