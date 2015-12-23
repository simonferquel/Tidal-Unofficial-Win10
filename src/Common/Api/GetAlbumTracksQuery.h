#pragma once
#include "TracksQueryBase.h"
#include "TrackInfo.h"
#include "PaginatedList.h"
namespace api {
	class GetAlbumTracksQuery : public GetTracksQueryBase {
		std::int64_t _albumId;
	public:
		GetAlbumTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode)
		: GetTracksQueryBase(limit, offset, sessionId, countryCode), _albumId(id){}
		GetAlbumTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ countryCode)
			: GetTracksQueryBase(limit, offset, countryCode), _albumId(id) {}

		// Inherited via QueryBase
		virtual std::wstring url() const override {
			std::wstring result = L"albums/";
			result.append(std::to_wstring(_albumId));
			result.append(L"/tracks");
			return result;
		}

	};
}