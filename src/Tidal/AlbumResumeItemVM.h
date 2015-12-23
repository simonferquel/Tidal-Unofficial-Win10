#pragma once
#include <cstdint>
#include "IGo.h"
namespace api {
	struct AlbumResume;
}
namespace Tidal {
	public ref class AlbumResumeItemVM sealed : public IGo{
	public:
		property std::int64_t ArtistId;
		property std::int64_t Id;
		property Platform::String^ ImageUrl;
		property Platform::String^ Title;
		property Platform::String^ ArtistName;
		property Platform::String^ SearchImageUrl;
		virtual void Go();
		void GoToArtist();
	internal:
		AlbumResumeItemVM(const api::AlbumResume& info, bool offline = false);
	};
}