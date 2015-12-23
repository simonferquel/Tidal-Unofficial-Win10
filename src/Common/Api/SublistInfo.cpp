#include "pch.h"
#include "SublistInfo.h"
#include "JsonHelpers.h"
api::SublistInfo::SublistInfo(const web::json::value & json)
{
	parseJson(&json, L"hasAlbums", hasAlbums);
	parseJson(&json, L"hasArtists", hasArtists);
	parseJson(&json, L"hasPlaylists", hasPlaylists);
	parseJson(&json, L"hasTracks", hasTracks);
	parseJson(&json, L"hasVideos", hasVideos);
	parseJson(&json, L"image", image);
	parseJson(&json, L"name", name);
	parseJson(&json, L"path", path);
}
