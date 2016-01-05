#pragma once
#include <cstdint>
#include <Api/TrackInfo.h>

namespace Tidal {
	public ref class TrackItemVM sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged {
	private:
		api::TrackInfo _trackInfo;
		Windows::UI::Xaml::Visibility _playButtonVisibility;

	public:
		property std::int64_t Id;
		property std::int64_t AlbumId;
		property Platform::String^ ImageUrl;
		property Platform::String^ Title;
		property Platform::String^ AlbumTitle;
		property Platform::String^ ArtistName;
		property Platform::String^ DurationText;
		property double Opacity;
		property Windows::UI::Xaml::Visibility PlayButtonVisibility {Windows::UI::Xaml::Visibility get(); void set(Windows::UI::Xaml::Visibility value); }
		property Windows::UI::Xaml::Visibility PauseButtonVisibility {Windows::UI::Xaml::Visibility get() {
			return _playButtonVisibility == Windows::UI::Xaml::Visibility::Visible ?
				Windows::UI::Xaml::Visibility::Collapsed : Windows::UI::Xaml::Visibility::Visible;
		} }
		property Windows::UI::Text::FontWeight TitleFontWeight { Windows::UI::Text::FontWeight get() {
			return _playButtonVisibility == Windows::UI::Xaml::Visibility::Visible ?
				Windows::UI::Text::FontWeights::Normal : Windows::UI::Text::FontWeights::Bold;
		}}
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;

		void GoToArtist();
		
		void GoToAlbum();
	internal:
		const api::TrackInfo& trackInfo() {
			return _trackInfo;
		}
		TrackItemVM(const api::TrackInfo& info, bool includeTrackNumberInTitle = false);
		void RefreshPlayingState(std::int64_t trackId, Windows::Media::Playback::MediaPlayerState state);

	};
}