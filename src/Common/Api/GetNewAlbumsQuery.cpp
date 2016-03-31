#include "pch.h"
#include "GetNewAlbumsQuery.h"
using namespace api;
api::GetNewAlbumsQuery::GetNewAlbumsQuery(int limit, int offset, Platform::String ^ sessionId, Platform::String ^ countryCode) 
	:QueryBase(sessionId, countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
}

api::GetNewAlbumsQuery::GetNewAlbumsQuery(int limit, int offset, Platform::String ^ countryCode)
{
	addQueryStringParameter(L"limit", limit.ToString());
	addQueryStringParameter(L"offset", offset.ToString());
	addQueryStringParameter(L"countryCode", countryCode);
}

std::wstring api::GetNewAlbumsQuery::url() const
{
	std::wstring result = _list->Data();
	result.push_back(L'/');
	result.append(_group->Data());
	result.append(L"/albums");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<AlbumResume>>> api::GetNewAlbumsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{

	auto json = co_await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	co_return std::make_shared<api::PaginatedList<AlbumResume>>(jsonVal);
}
