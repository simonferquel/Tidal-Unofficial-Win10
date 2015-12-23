#include "pch.h"
#include "PromotionArticle.h"
#include "JsonHelpers.h"
using namespace api;

api::PromotionArticle::PromotionArticle(const web::json::value & json)
{
	parseJson(&json, L"artifactId", artifactId);
	parseJson(&json, L"created", created);
	parseJson(&json, L"featured", featured);
	parseJson(&json, L"group", group);
	parseJson(&json, L"header", header);
	parseJson(&json, L"imageId", imageId);
	parseJson(&json, L"imageURL", imageURL);
	parseJson(&json, L"shortHeader", shortHeader);
	parseJson(&json, L"shortSubHeader", shortSubHeader);
	parseJson(&json, L"standaloneHeader", standaloneHeader);
	parseJson(&json, L"subHeader", subHeader);
	parseJson(&json, L"text", text);
	parseJson(&json, L"type", type);
}
