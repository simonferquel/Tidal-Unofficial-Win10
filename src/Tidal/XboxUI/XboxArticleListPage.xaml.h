//
// XboxArticleListPage.xaml.h
// Declaration of the XboxArticleListPage class
//

#pragma once

#include "XboxUI\XboxArticleListPage.g.h"
#include "TracksPlaybackStateManager.h"
namespace Tidal
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxArticleListPage sealed
	{
	public:
		XboxArticleListPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		std::shared_ptr<TracksPlaybackStateManager> _tpsm;
		concurrency::task<void> LoadAsync(Platform::String^ listName);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSelectionItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
		void OnPause(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTogglePlayPause(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
