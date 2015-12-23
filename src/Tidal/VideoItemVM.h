#pragma once
#include <cstdint>
#include "IGo.h"
namespace api {
	struct VideoInfo;
}
namespace Tidal {
	public ref class VideoItemVM sealed : public IGo {
	public:
		property std::int64_t ArtistId;
		property std::int64_t Id;
		property Platform::String^ ImageUrl;
		property Platform::String^ SearchImageUrl;
		property Platform::String^ DurationText;
		property Platform::String^ Title;
		property Platform::String^ ArtistName;

		virtual void Go();
		void GoToArtist();
	internal:
		VideoItemVM(const api::VideoInfo& info);
	};
}