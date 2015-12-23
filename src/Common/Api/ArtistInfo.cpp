#include "pch.h"
#include "ArtistInfo.h"
#include "JsonHelpers.h"

api::ArtistInfo::ArtistInfo(const web::json::value & json)
{
	parseJson(&json, L"id", id);
	parseJson(&json, L"name", name);
	parseJson(&json, L"type", type);
	parseJson(&json, L"picture", picture);
}

web::json::value api::ArtistInfo::toJson() const
{
	auto jobj = web::json::value::object();
	jobj[L"id"] = web::json::value(id);
	jobj[L"name"] = web::json::value(name);
	jobj[L"type"] = web::json::value(type);
	jobj[L"picture"] = web::json::value(picture);
	return jobj;
}