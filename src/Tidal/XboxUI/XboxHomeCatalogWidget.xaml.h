//
// XboxHomeCatalogWidget.xaml.h
// Declaration of the XboxHomeCatalogWidget class
//

#pragma once

#include "XboxUI\XboxHomeCatalogWidget.g.h"
#include "../IncrementalLoadingCollection.h"
#include <pplawait2.h>
#include "../PromotionItemVM.h"
namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class XboxHomeCatalogWidget sealed
	{
	private:
		static property Windows::UI::Xaml::DependencyProperty^ SelectionProperty {Windows::UI::Xaml::DependencyProperty^ get(); };
	internal:
		
	public:
		property Windows::Foundation::Collections::IVector<PromotionItemVM^> ^ Selection{
			Windows::Foundation::Collections::IVector<PromotionItemVM^>^ get() {
			return dynamic_cast<Windows::Foundation::Collections::IVector<PromotionItemVM^>^>(GetValue(SelectionProperty));
		}
		void set(Windows::Foundation::Collections::IVector<PromotionItemVM^>^ value) {
			SetValue(SelectionProperty, value);
		}
	}
		XboxHomeCatalogWidget();
		void GoToNews();
		void GoToRising();
		void GoToDiscovery();
		void GoToPlaylists();
		void GoToGenres();
		void GoToVideos();
		void GoToSearch();
	private:
		concurrency::task<void> LoadAsync();
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
