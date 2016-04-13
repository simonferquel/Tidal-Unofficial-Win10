//
// SublistFilterControl.xaml.h
// Declaration of the SublistFilterControl class
//

#pragma once
#include "SublistItemVM.h"
#include "SublistFilterControl.g.h"

namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class SublistFilterControl sealed
	{
	private:
		static  Windows::UI::Xaml::DependencyProperty^ _SublistSourceProperty;
	public:
		static property Windows::UI::Xaml::DependencyProperty^ SublistSourceProperty {Windows::UI::Xaml::DependencyProperty^ get(){ return _SublistSourceProperty; } }
		property Windows::Foundation::Collections::IVector<SublistItemVM^>^ SublistSource { Windows::Foundation::Collections::IVector<SublistItemVM^>^ get() {
			return dynamic_cast<Windows::Foundation::Collections::IVector<SublistItemVM^>^>(GetValue(SublistSourceProperty));
		}
		void set(Windows::Foundation::Collections::IVector<SublistItemVM^>^ value) {
			SetValue(SublistSourceProperty, value);
			lv->ItemsSource = value;
			if (value->Size > 0) {
				lv->SelectedIndex = 0;
			}
		}
		}

		property int SelectedIndex {int get() { return lv->SelectedIndex; } void set(int value) { lv->SelectedIndex = value; }}
		void SetSourceAndSelectedIndex(Windows::Foundation::Collections::IVector<SublistItemVM^>^ value, int selectedIndex) {
			SetValue(SublistSourceProperty, value);
			lv->ItemsSource = value;
			if (value->Size > selectedIndex) {
				lv->SelectedIndex = selectedIndex;
			}
		}
		SublistFilterControl();
		event Windows::Foundation::EventHandler< SublistItemVM^>^ SelectedSublistChanged;
	private:
		void OnSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
	};
}
