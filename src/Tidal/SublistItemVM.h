#pragma once
#include <Api/SublistInfo.h>
namespace Tidal {
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class SublistItemVM sealed{
	private:
		api::SublistInfo _sublistInfo;
	public:
		property Platform::String^ Name;
		property Platform::String^ Path;
		property Platform::String^ ImageUrl;
		property Platform::String^ HeadingUrl;
		property Platform::String^ GenreImageUrl;
	internal:
		SublistItemVM(const api::SublistInfo& sublistInfo);
		const api::SublistInfo& sublistInfo() {
			return _sublistInfo;
		}
	};
}