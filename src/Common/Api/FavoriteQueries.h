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

		GetFavoriteAlbumsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
	public:
		static std::shared_ptr<GetFavoriteAlbumsQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode) {
			return std::shared_ptr<GetFavoriteAlbumsQuery>(new GetFavoriteAlbumsQuery(userId, sessionId, countryCode));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<AlbumResume>>>> executeAsync(concurrency::cancellation_token cancelToken);
	};

	class GetFavoriteArtistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
		GetFavoriteArtistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
	public:
		static std::shared_ptr<GetFavoriteArtistsQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode) {
			return std::shared_ptr<GetFavoriteArtistsQuery>(new GetFavoriteArtistsQuery(userId, sessionId, countryCode));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<ArtistInfo>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoriteArtistQuery : public QueryBase {
	private:
		std::int64_t _userId;
		AddFavoriteArtistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:
		static std::shared_ptr<AddFavoriteArtistQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<AddFavoriteArtistQuery>(new AddFavoriteArtistQuery(userId, sessionId, countryCode, id));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteArtisttQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
		RemoveFavoriteArtisttQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:
		static std::shared_ptr<RemoveFavoriteArtisttQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<RemoveFavoriteArtisttQuery>(new RemoveFavoriteArtisttQuery(userId, sessionId, countryCode, id));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class AddFavoriteAlbumQuery : public QueryBase {
	private:
		std::int64_t _userId;
		AddFavoriteAlbumQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:
		static std::shared_ptr<AddFavoriteAlbumQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<AddFavoriteAlbumQuery>(new AddFavoriteAlbumQuery(userId, sessionId, countryCode, id));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteAlbumQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
		RemoveFavoriteAlbumQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:


		static std::shared_ptr<RemoveFavoriteAlbumQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<RemoveFavoriteAlbumQuery>(new RemoveFavoriteAlbumQuery(userId, sessionId, countryCode, id));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoriteTrackQuery : public QueryBase {
	private:
		std::int64_t _userId;
		AddFavoriteTrackQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:
		static std::shared_ptr<AddFavoriteTrackQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<AddFavoriteTrackQuery>(new AddFavoriteTrackQuery(userId, sessionId, countryCode, id));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoriteTrackQuery : public QueryBase {
	private:
		std::int64_t _userId;
		std::int64_t _id;
		RemoveFavoriteTrackQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id);
	public:
		static std::shared_ptr<RemoveFavoriteTrackQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, std::int64_t id) {
			return std::shared_ptr<RemoveFavoriteTrackQuery>(new RemoveFavoriteTrackQuery(userId, sessionId, countryCode, id));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class GetFavoriteTracksQuery : public QueryBase {
	private:
		std::int64_t _userId;
		GetFavoriteTracksQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
	public:
		static std::shared_ptr<GetFavoriteTracksQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode) {
			return std::shared_ptr<GetFavoriteTracksQuery>(new GetFavoriteTracksQuery(userId, sessionId, countryCode));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<TrackInfo>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class GetFavoritePlaylistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
		GetFavoritePlaylistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
	public:
		static std::shared_ptr<GetFavoritePlaylistsQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode) {
			return std::shared_ptr<GetFavoritePlaylistsQuery>(new GetFavoritePlaylistsQuery(userId, sessionId, countryCode));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<TimestampedEntity<PlaylistResume>>>> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class AddFavoritePlaylistQuery : public QueryBase {
	private:
		std::int64_t _userId;
		AddFavoritePlaylistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId);
	public:
		static std::shared_ptr<AddFavoritePlaylistQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId) {
			return std::shared_ptr<AddFavoritePlaylistQuery>(new AddFavoritePlaylistQuery(userId, sessionId, countryCode, playlistId));
		}

		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};

	class RemoveFavoritePlaylistQuery : public QueryBase {
	private:
		std::int64_t _userId;
		Platform::String^ _playlistId;
		RemoveFavoritePlaylistQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId);
	public:
		static std::shared_ptr<RemoveFavoritePlaylistQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode, Platform::String^ playlistId) {
			return std::shared_ptr<RemoveFavoritePlaylistQuery>(new RemoveFavoritePlaylistQuery(userId, sessionId, countryCode, playlistId));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<void> executeAsync(concurrency::cancellation_token cancelToken);

	};


	class GetMyPlaylistsQuery : public QueryBase {
	private:
		std::int64_t _userId;
		GetMyPlaylistsQuery(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode);
	public:
		static std::shared_ptr<GetMyPlaylistsQuery> Make(std::int64_t userId, Platform::String^ sessionId, Platform::String^ countryCode) {
			return std::shared_ptr<GetMyPlaylistsQuery>(new GetMyPlaylistsQuery(userId, sessionId, countryCode));
		}
		// Inherited via QueryBase
		virtual std::wstring url() const override;

		concurrency::task<std::shared_ptr<PaginatedList<PlaylistResume>>> executeAsync(concurrency::cancellation_token cancelToken);

	};
}