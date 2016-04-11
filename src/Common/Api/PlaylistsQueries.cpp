#include "pch.h"
#include "PlaylistsQueries.h"
#include <tools/StringUtils.h>

api::CreatePlaylistQuery::CreatePlaylistQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, Platform::String ^ title, Platform::String ^ description)
	: QueryBase(sessionId, countryCode),
	_userId(userId)
{
	addBodyContent(L"title", title);
	addBodyContent(L"description", description);
}

std::wstring api::CreatePlaylistQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/playlists");
	return result;
}

concurrency::task<std::shared_ptr<api::PlaylistResume>> api::CreatePlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return postAsync(cancelToken).then([](Platform::String^ jsonString) {
		tools::strings::WindowsWIStream stream(jsonString);
		auto jsonVal = web::json::value::parse(stream);
		return std::make_shared<api::PlaylistResume>(jsonVal);
	});
}

api::AddTracksToPlaylistQuery::AddTracksToPlaylistQuery(const std::wstring & playlistUuid , Platform::String^ etag, Platform::String ^ sessionId, Platform::String ^ countryCode, const std::vector<std::int64_t>& trackIds, int toIndex)
	: QueryBase(sessionId, countryCode),
	_uuid(playlistUuid)
{
	addHeader(L"If-None-Match", etag);
	addBodyContent(L"toIndex", toIndex.ToString());
	std::wstring ids;
	for (auto& id : trackIds) {
		if (ids.size() > 0) {
			ids.push_back(L',');
		}
		ids.append(std::to_wstring(id));
	}
	addBodyContent(L"trackIds", tools::strings::toWindowsString(ids));
}

std::wstring api::AddTracksToPlaylistQuery::url() const
{
	std::wstring result = L"playlists/";
	result.append(_uuid);
	result.append(L"/tracks");
	return result;
}

concurrency::task<void> api::AddTracksToPlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	co_await postAsync(cancelToken).then([](Platform::String^) {});
}

api::RemoveItemFromPlaylistQuery::RemoveItemFromPlaylistQuery(const std::wstring & playlistUuid, Platform::String ^ etag, Platform::String ^ sessionId, Platform::String ^ countryCode, int index)
	: QueryBase(sessionId, countryCode),
	_uuid(playlistUuid), _index(index)
{
	addHeader(L"If-None-Match", etag);
	
}

std::wstring api::RemoveItemFromPlaylistQuery::url() const
{
	std::wstring result = L"playlists/";
	result.append(_uuid);
	result.append(L"/tracks/");
	result.append(std::to_wstring(_index));
	return result;
}

concurrency::task<void> api::RemoveItemFromPlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	return deleteAsync(cancelToken).then([](Platform::String^) {});
}
