//
// SublistFilterControl.xaml.cpp
// Implementation of the SublistFilterControl class
//

#include "pch.h"
#include "SublistFilterControl.xaml.h"

using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SublistFilterControl::SublistFilterControl()
{
	InitializeComponent();
}

DependencyProperty^ SublistFilterControl::_SublistSourceProperty = DependencyProperty::Register(L"SublistSource", Windows::Foundation::Collections::IVector<SublistItemVM^>::typeid, SublistFilterControl::typeid, ref new PropertyMetadata(ref new Platform::Collections::Vector<SublistItemVM^>()));


void Tidal::SublistFilterControl::OnSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto item = dynamic_cast<SublistItemVM^>(lv->SelectedItem);
	if (item) {
		SelectedSublistChanged(this, item);
	}
}
