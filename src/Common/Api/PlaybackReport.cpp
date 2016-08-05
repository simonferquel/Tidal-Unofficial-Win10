#include "pch.h"
#include "PlaybackReport.h"
#include "JsonHelpers.h"

web::json::value api::PlaybackReport::toJson() const
{
	auto jobj = web::json::value::object();
	jobj[L"trackId"] = web::json::value(id);
	jobj[L"quality"] = web::json::value(quality);
	jobj[L"deliveredDuration"] = web::json::value(deliveredDuration);
	jobj[L"playDate"] = api::toJson(playDate);
	if (playlistUuid.size() > 0)
	{
		jobj[L"playlistUuid"] = web::json::value(playlistUuid);
	}
	jobj[L"playbackMode"] = web::json::value(L"ONLINE_CACHED");
	jobj[L"type"] = web::json::value(L"TRACK");
	return jobj;
}
