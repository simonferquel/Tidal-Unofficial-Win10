//
// AddToPlaylistDialog.xaml.h
// Declaration of the AddToPlaylistDialog class
//

#pragma once
#include "PlaylistResumeItemVM.h"
#include "AddToPlaylistDialog.g.h"
#include <cstdint>
#include <vector>
#include <ppltasks.h>
namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class AddToPlaylistDialog sealed
	{
	private:
		bool _isNewPlaylistMode = false;
		std::vector<std::int64_t> _trackIds;
		concurrency::task<void> loadPlaylistsAsync();
		concurrency::task<void> addToExistingPlaylistAsync(PlaylistResumeItemVM^ playlist);
		concurrency::task<void> addToNewPlaylistAsync();
	internal:
		AddToPlaylistDialog(const std::vector<std::int64_t>& trackIds);
		AddToPlaylistDialog(std::vector<std::int64_t>&& trackIds);

	private:
		void OnCancel(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void OnPrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void OnExistingPlaylistClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};

	void ShowAddToPlaylistDialog(const std::vector<std::int64_t>& trackIds);
	void ShowAddToPlaylistDialog(std::vector<std::int64_t>&& trackIds);
	inline void ShowAddToPlaylistDialog(std::int64_t trackId) {
		ShowAddToPlaylistDialog(std::vector<int64_t>{ trackId });
	}
}
