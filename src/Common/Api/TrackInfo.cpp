#include "pch.h"
#include "TrackInfo.h"
#include "JsonHelpers.h"
api::TrackInfo::TrackInfo(const web::json::value & json)
{
	parseJson(&json, L"album", album);
	parseJson(&json, L"allowStreaming", allowStreaming);
	parseJson(&json, L"artist", artist);
	parseJson(&json, L"copyright", copyright);
	parseJson(&json, L"duration", duration);
	parseJson(&json, L"explicit", is_explicit);
	parseJson(&json, L"id", id);
	parseJson(&json, L"peak", peak);
	parseJson(&json, L"popularity", popularity);
	parseJson(&json, L"premiumStreamingOnly", premiumStreamingOnly);
	parseJson(&json, L"replayGain", replayGain);
	parseJson(&json, L"streamReady", streamReady);
	parseJson(&json, L"streamStartDate", streamStartDate);
	parseJson(&json, L"title", title);
	parseJson(&json, L"trackNumber", trackNumber);
	parseJson(&json, L"url", url);
	parseJson(&json, L"version", version);
	parseJson(&json, L"volumeNumber", volumeNumber);

	const web::json::value*  artistsJson;
	if (parseJson(&json, L"artists", artistsJson)) {
		if (artistsJson->is_array()) {
			for (auto& itemJson : artistsJson->as_array()) {
				artists.emplace_back(itemJson);
			}
		}
	}
}


web::json::value api::TrackInfo::toJson() const
{
	auto jobj = web::json::value::object();
	jobj[L"album"] = album.toJson();
	jobj[L"allowStreaming"] = web::json::value(allowStreaming);
	jobj[L"artist"] = artist.toJson();
	jobj[L"copyright"] = web::json::value( copyright);
	jobj[L"duration"] = web::json::value(duration.count());
	jobj[L"explicit"] = web::json::value(is_explicit);
	jobj[L"id"] = web::json::value(id);
	jobj[L"peak"] = web::json::value(peak);
	jobj[L"popularity"] = web::json::value(popularity);
	jobj[L"premiumStreamingOnly"] = web::json::value(premiumStreamingOnly);
	jobj[L"replayGain"] = web::json::value(replayGain);
	jobj[L"streamReady"] = web::json::value(streamReady);
	jobj[L"streamStartDate"] = api::toJson(streamStartDate);
	jobj[L"title"] = web::json::value( title);
	jobj[L"trackNumber"] = web::json::value(trackNumber);
	jobj[L"url"] = web::json::value(url);
	jobj[L"version"] = web::json::value(version);
	jobj[L"volumeNumber"] = web::json::value(volumeNumber);
	auto jarr = web::json::value::array();
	for (auto& a : artists) {
		jarr[jarr.size()] = a.toJson();
	}

	jobj[L"artists"] = jarr;
	return jobj;

}