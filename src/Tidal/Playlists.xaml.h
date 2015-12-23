//
// Playlists.xaml.h
// Declaration of the Playlists class
//

#pragma once

#include "SublistItemVM.h"
#include "Playlists.g.h"

namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Playlists sealed
	{
	public:
		Playlists();
	private:
		concurrency::task<void> LoadAsync();
		concurrency::task<void> LoadMoodAsync(SublistItemVM^ item);
		void OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnAllPlaylistsFilterSelectionChanged(Platform::Object^ sender, Tidal::SublistItemVM^ e);
		void OnSelectedMoodChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void OnPlaylistItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	};
}
