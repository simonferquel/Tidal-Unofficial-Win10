#include "pch.h"
#include "SubscribtionInfo.h"
#include "JsonHelpers.h"

api::SubscribtionInfo::SubscribtionInfo(const web::json::value & json)
{
	parseJson(&json, L"validUntil", validUntil);
	parseJson(&json, L"status", status);
	parseJson(&json, L"highestSoundQuality", highestSoundQuality);
	parseJson(&json, L"premiumAccess", premiumAccess);
	parseJson(&json, L"canGetTrial", canGetTrial);
	const web::json::value* subObj = nullptr;
	parseJson(&json, L"subscription", subObj);
	parseJson(subObj, L"type", type);
	parseJson(subObj, L"offlineGracePeriod", offlineGracePeriod);
	
}
