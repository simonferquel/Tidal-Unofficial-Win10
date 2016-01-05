#include "pch.h"
#include "PlaylistResume.h"
#include "JsonHelpers.h"


api::PlaylistResume::PlaylistResume(const web::json::value & json)
{
	parseJson(&json, L"created", created);
	parseJson(&json, L"creator", creator);
	parseJson(&json, L"description", description);
	parseJson(&json, L"duration", duration);
	parseJson(&json, L"image", image);
	parseJson(&json, L"lastUpdated", lastUpdated);
	parseJson(&json, L"numberOfTracks", numberOfTracks);
	parseJson(&json, L"numberOfVideos", numberOfVideos);
	parseJson(&json, L"publicPlaylist", publicPlaylist);
	parseJson(&json, L"title", title);
	parseJson(&json, L"type", type);
	parseJson(&json, L"url", url);
	parseJson(&json, L"uuid", uuid);
}

web::json::value api::PlaylistResume::toJson() const
{
	auto jobj = web::json::value::object();
	jobj[L"created"] = api::toJson(created);
	jobj[L"creator"] = creator.toJson();
	jobj[L"description"] = web::json::value(description);
	jobj[L"duration"] = web::json::value(duration.count());
	jobj[L"image"] = web::json::value(image);
	jobj[L"lastUpdated"] = api::toJson(lastUpdated);
	jobj[L"numberOfTracks"] = web::json::value(numberOfTracks);
	jobj[L"numberOfVideos"] = web::json::value(numberOfVideos);
	jobj[L"publicPlaylist"] = web::json::value(publicPlaylist);
	jobj[L"title"] = web::json::value(title);
	jobj[L"type"] = web::json::value(type);
	jobj[L"url"] = web::json::value(url);
	jobj[L"uuid"] = web::json::value(uuid);
	return jobj;
}
