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
