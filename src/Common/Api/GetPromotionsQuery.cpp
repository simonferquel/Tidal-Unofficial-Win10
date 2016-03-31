#include "pch.h"
#include "GetPromotionsQuery.h"
#include "PromotionArticle.h"
using namespace api;

api::GetPromotionsQuery::GetPromotionsQuery(int limit, int offset, Platform::String ^ group, Platform::String ^ subscribtionType, std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode)
	: QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"group", group);
	addQueryStringParameter(L"subscribtionType", subscribtionType);
	addQueryStringParameter(L"userId", userId.ToString());
	addQueryStringParameter(L"clientType", L"BROWSER");
}

api::GetPromotionsQuery::GetPromotionsQuery(int limit, int offset, Platform::String ^ group, Platform::String ^ countryCode)
	:QueryBase()
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"group", group);
	addQueryStringParameter(L"countryCode", countryCode);
	addQueryStringParameter(L"clientType", L"BROWSER");
}

std::wstring api::GetPromotionsQuery::url() const
{
	return L"promotions";
}

concurrency::task<std::shared_ptr<PaginatedList<PromotionArticle>>> api::GetPromotionsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	co_return std::make_shared<api::PaginatedList<PromotionArticle>>(jsonVal);
}
