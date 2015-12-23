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
	jobj[L"allowStreaming"] = allowStreaming;
	jobj[L"artist"] = artist.toJson();
	jobj[L"copyright"] = web::json::value( copyright);
	jobj[L"duration"] = duration.count();
	jobj[L"explicit"] = is_explicit;
	jobj[L"id"] = id;
	jobj[L"peak"] = peak;
	jobj[L"popularity"] = popularity;
	jobj[L"premiumStreamingOnly"] = premiumStreamingOnly;
	jobj[L"replayGain"] = replayGain;
	jobj[L"streamReady"] = streamReady;
	jobj[L"streamStartDate"] = api::toJson(streamStartDate);
	jobj[L"title"] = web::json::value( title);
	jobj[L"trackNumber"] = trackNumber;
	jobj[L"url"] = web::json::value(url);
	jobj[L"version"] = web::json::value(version);
	jobj[L"volumeNumber"] = volumeNumber;
	auto jarr = web::json::value::array();
	for (auto& a : artists) {
		jarr[jarr.size()] = a.toJson();
	}

	jobj[L"artists"] = jarr;
	return jobj;

}