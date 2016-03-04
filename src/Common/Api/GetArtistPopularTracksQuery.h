#pragma once
#include "TracksQueryBase.h"
namespace api {
	class GetArtistPopularTracksQuery : public GetTracksQueryBase {
	private:
		std::int64_t _id;
	public:
		GetArtistPopularTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode) :
			GetTracksQueryBase(limit, offset, sessionId, countryCode), _id(id) {}
		GetArtistPopularTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ countryCode):
			GetTracksQueryBase(limit, offset, countryCode), _id(id) {}

		virtual std::wstring url() const override {
			std::wstring result = L"artists/";
			result.append(std::to_wstring(_id));
			result.append(L"/toptracks");
			return result;
		}
	};

	class GetArtistRadioTracksQuery : public GetTracksQueryBase {
	private:
		std::int64_t _id;
	public:
		GetArtistRadioTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ sessionId, Platform::String^ countryCode) :
			GetTracksQueryBase(limit, offset, sessionId, countryCode), _id(id) {}
		GetArtistRadioTracksQuery(std::int64_t id, int limit, int offset, Platform::String^ countryCode) :
			GetTracksQueryBase(limit, offset, countryCode), _id(id) {}

		virtual std::wstring url() const override {
			std::wstring result = L"artists/";
			result.append(std::to_wstring(_id));
			result.append(L"/radio");
			return result;
		}
	};
}
