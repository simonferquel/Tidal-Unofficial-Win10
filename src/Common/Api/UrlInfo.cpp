#include "pch.h"
#include "UrlInfo.h"
#include "JsonHelpers.h"

api::UrlInfo::UrlInfo(const web::json::value & json)
{
	parseJson(&json, L"url", url);
}
