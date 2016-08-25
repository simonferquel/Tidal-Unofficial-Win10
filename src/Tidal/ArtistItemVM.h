#pragma once
#include <Api/ArtistInfo.h>
#include "IGo.h"
namespace Tidal {
	public ref class ArtistItemVM sealed : public IGo{
	private:
		api::ArtistInfo _info;
	public:
		property std::int64_t Id;
		property Platform::String^ Name;
		property Platform::String^ ImageUrl;
		property Platform::String^ SearchImageUrl;
		virtual void Go();
	internal:
		ArtistItemVM(api::ArtistInfo& artistInfo);
		const api::ArtistInfo& info() { return _info; }
	};
}