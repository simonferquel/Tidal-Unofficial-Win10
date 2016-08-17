#pragma once
#include "ArtistItemVM.h"
#include "AlbumResumeItemVM.h"
#include "TrackItemVM.h"
#include "VideoItemVM.h"
#include "PlaylistResumeItemVM.h"

namespace Tidal {
	public ref class SearchResultTemplateSelector sealed : public Windows::UI::Xaml::Controls::DataTemplateSelector {
	public:
		property Windows::UI::Xaml::DataTemplate^ ArtistTemplate;
		property Windows::UI::Xaml::DataTemplate^ AlbumTemplate;
		property Windows::UI::Xaml::DataTemplate^ TrackTemplate;
		property Windows::UI::Xaml::DataTemplate^ VideoTemplate;
		property Windows::UI::Xaml::DataTemplate^ PlaylistTemplate;
	protected:

		virtual Windows::UI::Xaml::DataTemplate^ SelectTemplateCore(Platform::Object^ item) override {
			if (dynamic_cast<ArtistItemVM^>(item)) {
				return ArtistTemplate;
			}
			if (dynamic_cast<AlbumResumeItemVM^>(item)) {
				return AlbumTemplate;
			}
			if (dynamic_cast<TrackItemVM^>(item)) {
				return TrackTemplate;
			}
			if (dynamic_cast<VideoItemVM^>(item)) {
				return VideoTemplate;
			}
			if (dynamic_cast<PlaylistResumeItemVM^>(item)) {
				return PlaylistTemplate;
			}
			return nullptr;
		}
	};
}