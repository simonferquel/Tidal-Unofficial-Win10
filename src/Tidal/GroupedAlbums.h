#pragma once
#include "AlbumResumeItemVM.h"
namespace Tidal {
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class GroupedAlbums sealed {
	public:
		property Platform::String^ Title;
		property Windows::Foundation::Collections::IVector<AlbumResumeItemVM^>^ Albums;
	};
}