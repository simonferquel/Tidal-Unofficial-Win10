#pragma once
#include <cstdint>
#include <Api/TrackInfo.h>

namespace Tidal {
	public ref class TrackItemVM sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged {
	private:
		api::TrackInfo _trackInfo;
		Windows::UI::Xaml::Visibility _playButtonVisibility;
		Windows::UI::Xaml::Visibility _addFavoriteVisibility;
		Windows::UI::Xaml::Visibility _removeFavoriteVisibility;
		Platform::WeakReference _trackListRef;
		std::wstring _owningPlaylistId;
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

		property Windows::UI::Xaml::Visibility AddFavoriteVisibility {
			Windows::UI::Xaml::Visibility get() {
				return _addFavoriteVisibility;
			}
			void set(Windows::UI::Xaml::Visibility value) {
				if (value != _addFavoriteVisibility) {
					_addFavoriteVisibility = value;
					PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"AddFavoriteVisibility"));
				}
			}
		}

		property Windows::UI::Xaml::Visibility RemoveFavoriteVisibility {
			Windows::UI::Xaml::Visibility get() {
				return _removeFavoriteVisibility;
			}
			void set(Windows::UI::Xaml::Visibility value) {
				if (value != _removeFavoriteVisibility) {
					_removeFavoriteVisibility = value;
					PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"RemoveFavoriteVisibility"));
				}
			}
		}
		property Windows::UI::Xaml::Input::ICommand^ PlayCommand {Windows::UI::Xaml::Input::ICommand^ get();  }
		
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
		void TogglePlayPause();
		void GoToArtist();

		void GoToAlbum();
		void AddFavorite();
		void RemoveFavorite();
		void AddToPlaylist();
		void ShowMenu(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	internal:
		const api::TrackInfo& trackInfo() {
			return _trackInfo;
		}
		void AttachTo(Windows::Foundation::Collections::IIterable<TrackItemVM^>^ trackList) {
			_trackListRef = Platform::WeakReference(trackList);
		}
		TrackItemVM(const api::TrackInfo& info, bool includeTrackNumberInTitle = false);
		void RefreshPlayingState(std::int64_t trackId, Windows::Media::Playback::MediaPlayerState state);
		void attachToOwiningPlaylistId(const std::wstring& id) {
			_owningPlaylistId = id;
		}

	};
}