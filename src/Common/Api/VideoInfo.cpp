#include "pch.h"
#include "VideoInfo.h"
#include "JsonHelpers.h"

api::VideoInfo::VideoInfo(const web::json::value & json)
{
	parseJson(&json, L"artist", artist);
	const web::json::value*  artistsJson;
	if (parseJson(&json, L"artists", artistsJson)) {
		if (artistsJson->is_array()) {
			for (auto& itemJson : artistsJson->as_array()) {
				artists.emplace_back(itemJson);
			}
		}
	}
	parseJson(&json, L"duration", duration);
	parseJson(&json, L"id", id);
	parseJson(&json, L"imageId", imageId);
	parseJson(&json, L"imagePath", imagePath);
	parseJson(&json, L"quality", quality);
	parseJson(&json, L"releasedDate", releasedDate);
	parseJson(&json, L"streamReady", streamReady);
	parseJson(&json, L"streamStartDate", streamStartDate);
	parseJson(&json, L"title", title);

}
