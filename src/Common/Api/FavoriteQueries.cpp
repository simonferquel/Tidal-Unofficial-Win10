#include "pch.h"
#include "FavoriteQueries.h"
using namespace api;
api::GetFavoriteAlbumsQuery::GetFavoriteAlbumsQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode)
	: QueryBase(sessionId, countryCode), _userId(userId)
{
	addQueryStringParameter(L"limit", L"9999");
}

std::wstring api::GetFavoriteAlbumsQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/albums");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<AlbumResume>>>> api::GetFavoriteAlbumsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<TimestampedEntity<AlbumResume>>>(jsonVal);
}

api::GetFavoriteArtistsQuery::GetFavoriteArtistsQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addQueryStringParameter(L"limit", L"9999");
}

std::wstring api::GetFavoriteArtistsQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/artists");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<ArtistInfo>>>> api::GetFavoriteArtistsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<TimestampedEntity<ArtistInfo>>>(jsonVal);
}

api::GetFavoriteTracksQuery::GetFavoriteTracksQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addQueryStringParameter(L"limit", L"9999");
}

std::wstring api::GetFavoriteTracksQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/tracks");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<TrackInfo>>>> api::GetFavoriteTracksQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<TimestampedEntity<TrackInfo>>>(jsonVal);
}

api::GetFavoritePlaylistsQuery::GetFavoritePlaylistsQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addQueryStringParameter(L"limit", L"9999");
}

std::wstring api::GetFavoritePlaylistsQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/playlists");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<PlaylistResume>>>> api::GetFavoritePlaylistsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<TimestampedEntity<PlaylistResume>>>(jsonVal);
}

api::GetMyPlaylistsQuery::GetMyPlaylistsQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addQueryStringParameter(L"limit", L"9999");
}

std::wstring api::GetMyPlaylistsQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/playlists");
	return result;
}

concurrency::task<std::shared_ptr<PaginatedList<PlaylistResume>>> api::GetMyPlaylistsQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto json = await getAsync(cancelToken);
	tools::strings::WindowsWIStream stream(json);
	auto jsonVal = web::json::value::parse(stream);
	return std::make_shared<api::PaginatedList<PlaylistResume>>(jsonVal);
}

api::AddFavoritePlaylistQuery::AddFavoritePlaylistQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, Platform::String ^ playlistId) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addBodyContent(L"uuid", playlistId);
}

std::wstring api::AddFavoritePlaylistQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/playlists");
	return result;
}

concurrency::task<void> api::AddFavoritePlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await postAsync(cancelToken);
}

api::RemoveFavoritePlaylistQuery::RemoveFavoritePlaylistQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, Platform::String ^ playlistId) : QueryBase(sessionId, countryCode), _userId(userId), _playlistId(playlistId)
{
}

std::wstring api::RemoveFavoritePlaylistQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/playlists/");
	result.append(_playlistId->Data());
	return result;
}

concurrency::task<void> api::RemoveFavoritePlaylistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await deleteAsync(cancelToken);

}

api::AddFavoriteArtistQuery::AddFavoriteArtistQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addBodyContent(L"artistId", id.ToString());
}

std::wstring api::AddFavoriteArtistQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/artists");
	return result;
}

concurrency::task<void> api::AddFavoriteArtistQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await postAsync(cancelToken);
}

api::RemoveFavoriteArtisttQuery::RemoveFavoriteArtisttQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId), _id(id)
{
}

std::wstring api::RemoveFavoriteArtisttQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/artists/");
	result.append(std::to_wstring(_id));
	return result;
}

concurrency::task<void> api::RemoveFavoriteArtisttQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await deleteAsync(cancelToken);
}


// albums


api::AddFavoriteAlbumQuery::AddFavoriteAlbumQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addBodyContent(L"albumId", id.ToString());
}

std::wstring api::AddFavoriteAlbumQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/albums");
	return result;
}

concurrency::task<void> api::AddFavoriteAlbumQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await postAsync(cancelToken);
}

api::RemoveFavoriteAlbumQuery::RemoveFavoriteAlbumQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId), _id(id)
{
}

std::wstring api::RemoveFavoriteAlbumQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/albums/");
	result.append(std::to_wstring(_id));
	return result;
}

concurrency::task<void> api::RemoveFavoriteAlbumQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await deleteAsync(cancelToken);
}


//tracks


api::AddFavoriteTrackQuery::AddFavoriteTrackQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId)
{
	addBodyContent(L"trackId", id.ToString());
}

std::wstring api::AddFavoriteTrackQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/tracks");
	return result;
}

concurrency::task<void> api::AddFavoriteTrackQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await postAsync(cancelToken);
}

api::RemoveFavoriteTrackQuery::RemoveFavoriteTrackQuery(std::int64_t userId, Platform::String ^ sessionId, Platform::String ^ countryCode, std::int64_t id) : QueryBase(sessionId, countryCode), _userId(userId), _id(id)
{
}

std::wstring api::RemoveFavoriteTrackQuery::url() const
{
	std::wstring result = L"users/";
	result.append(std::to_wstring(_userId));
	result.append(L"/favorites/tracks/");
	result.append(std::to_wstring(_id));
	return result;
}

concurrency::task<void> api::RemoveFavoriteTrackQuery::executeAsync(concurrency::cancellation_token cancelToken)
{
	auto result = await deleteAsync(cancelToken);
}