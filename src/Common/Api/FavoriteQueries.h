#pragma once
#include "QueryBase.h"
#include "AlbumResume.h"
#include "ArtistInfo.h"
#include "TrackInfo.h"
#include "PlaylistResume.h"
#include "PaginatedList.h"

namespace api {
	class GetFavoriteAlbumsQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		GetFavoriteAlbumsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
		
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<AlbumResume>>>> executeAsync(concurrency::cancellation_token cancelToken);
	};

	class GetFavoriteArtistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		GetFavoriteArtistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<ArtistInfo>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoriteArtistQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		AddFavoriteArtistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteArtisttQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
	public:
		RemoveFavoriteArtisttQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class AddFavoriteAlbumQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		AddFavoriteAlbumQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteAlbumQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
	public:
		RemoveFavoriteAlbumQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoriteTrackQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		AddFavoriteTrackQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteTrackQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
	public:
		RemoveFavoriteTrackQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class GetFavoriteTracksQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		GetFavoriteTracksQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<TrackInfo>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class GetFavoritePlaylistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		GetFavoritePlaylistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<PlaylistResume>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoritePlaylistQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		AddFavoritePlaylistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoritePlaylistQuery : public QueryBase {
	private:
		std::int64_t _userId;
		Platform::String^ _playlistId;
	public:
		RemoveFavoritePlaylistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class GetMyPlaylistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
	public:
		GetMyPlaylistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<PlaylistResume>>> executeAsync(concurrency::cancellation_token cancelToken);

	};
}