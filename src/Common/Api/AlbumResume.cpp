#include "pch.h"
#include "AlbumResume.h"
#include "JsonHelpers.h"
using namespace api;
using namespace date;
api::AlbumResume::AlbumResume()
	: releaseDate{ 1970_y / 1 / 1 }
{
}
api::AlbumResume::AlbumResume(const web::json::value & json)
	: releaseDate{1970_y/1/1}
{
	parseJson(&json, L"allowStreaming", allowStreaming);
	parseJson(&json, L"artist", artist);
	const web::json::value*  artistsJson;
	if (parseJson(&json, L"artists", artistsJson)) {
		if (artistsJson->is_array()) {
			for (auto& itemJson : artistsJson->as_array()) {
				artists.emplace_back(itemJson);
			}
		}
	}
	parseJson(&json, L"copyright", copyright);
	parseJson(&json, L"cover", cover);
	parseJson(&json, L"duration", duration);
	parseJson(&json, L"explicit", is_explicit);
	parseJson(&json, L"id", id);
	parseJson(&json, L"numberOfTracks", numberOfTracks);
	parseJson(&json, L"numberOfVolumes", numberOfVolumes);
	parseJson(&json, L"premiumStreamingOnly", premiumStreamingOnly);
	parseJson(&json, L"releaseDate", releaseDate);
	parseJson(&json, L"streamReady", streamReady);
	parseJson(&json, L"streamStartDate", streamStartDate);
	parseJson(&json, L"title", title);
	parseJson(&json, L"type", type);
	parseJson(&json, L"url", url);
	parseJson(&json, L"version", version);

}

web::json::value api::AlbumResume::toJson() const
{
	auto jobj = web::json::value::object();
	jobj[L"allowStreaming"] = web::json::value(allowStreaming);
	jobj[L"artist"] = artist.toJson();
	auto jarr = web::json::value::array();
	for (auto& a : artists) {
		jarr[jarr.size()] = a.toJson();
	}
	jobj[L"artists"] = jarr;
	jobj[L"copyright"] = web::json::value(copyright);
	jobj[L"cover"] = web::json::value(cover);
	jobj[L"duration"] = web::json::value(duration.count());
	jobj[L"explicit"] = web::json::value(is_explicit);
	jobj[L"id"] = web::json::value(id);
	jobj[L"numberOfTracks"] = web::json::value(numberOfTracks);
	jobj[L"numberOfVolumes"] = web::json::value(numberOfVolumes);
	jobj[L"premiumStreamingOnly"] = web::json::value(premiumStreamingOnly);
	jobj[L"releaseDate"] = web::json::value(api::toJson(releaseDate));
	jobj[L"streamReady"] = web::json::value(streamReady);
	jobj[L"streamStartDate"] = web::json::value(api::toJson(streamStartDate));
	jobj[L"title"] = web::json::value(title);
	jobj[L"type"] = web::json::value(type);
	jobj[L"url"] = web::json::value(url);
	jobj[L"version"] = web::json::value(version);
	return jobj;

}