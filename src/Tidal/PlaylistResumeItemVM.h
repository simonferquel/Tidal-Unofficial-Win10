#pragma once
#include <cstdint>
#include "IGo.h"
namespace api {
	struct PlaylistResume;
}
namespace Tidal {
	public ref class PlaylistResumeItemVM sealed : public IGo {
	public:
		property std::int64_t ArtistId;
		property Platform::String^ Uuid;
		property Platform::String^ ImageUrl;
		property Platform::String^ SearchImageUrl;
		property Platform::String^ Title;
		property Platform::String^ ArtistName;
		virtual void Go();
		void GoToArtist();
	internal:
		PlaylistResumeItemVM(const api::PlaylistResume& info);
	};
}