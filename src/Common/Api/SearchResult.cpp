#include "pch.h"
#include "SearchResult.h"
#include "JsonHelpers.h"
api::SearchResults::SearchResults(const web::json::value & json)
{
	parseJson(&json, L"albums", albums);
	parseJson(&json, L"artists", artists);
	parseJson(&json, L"playlists", playlists);
	parseJson(&json, L"tracks", tracks);
	parseJson(&json, L"videos", videos);
}
