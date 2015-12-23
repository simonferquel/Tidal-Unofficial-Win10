#include "pch.h"
#include "MetricsHelper.h"
using namespace Windows::UI::Xaml;
using namespace Tidal;
Windows::UI::Xaml::DependencyProperty^ Tidal::MetricsHelper::_DesiredItemSizeProperty = DependencyProperty::Register(L"DesiredItemSize", double::typeid, Tidal::MetricsHelper::typeid, ref new PropertyMetadata(100.0, ref new PropertyChangedCallback([](DependencyObject^ source, DependencyPropertyChangedEventArgs^ args) {
	static_cast<MetricsHelper^>(source)->UpdateComputedItemSize();
})));
Windows::UI::Xaml::DependencyProperty^ Tidal::MetricsHelper::_ViewportSizeProperty = DependencyProperty::Register(L"ViewportSize", double::typeid, Tidal::MetricsHelper::typeid, ref new PropertyMetadata(124.0, ref new PropertyChangedCallback([](DependencyObject^ source, DependencyPropertyChangedEventArgs^ args) {
	static_cast<MetricsHelper^>(source)->UpdateComputedItemSize();
})));
Windows::UI::Xaml::DependencyProperty^ Tidal::MetricsHelper::_ItemMarginProperty = DependencyProperty::Register(L"ItemMargin", double::typeid, Tidal::MetricsHelper::typeid, ref new PropertyMetadata(12.0, ref new PropertyChangedCallback([](DependencyObject^ source, DependencyPropertyChangedEventArgs^ args) {
	static_cast<MetricsHelper^>(source)->UpdateComputedItemSize();
})));
Windows::UI::Xaml::DependencyProperty^ Tidal::MetricsHelper::_ComputedItemSizeProperty = DependencyProperty::Register(L"ComputedItemSize", double::typeid, Tidal::MetricsHelper::typeid, ref new PropertyMetadata(100.0));

void Tidal::MetricsHelper::UpdateComputedItemSize()
{
	auto totalItemSize = DesiredItemSize + 2 * ItemMargin;
	auto count = (int)std::floor(ViewportSize / totalItemSize);
	if (count < 1) {
		ComputedItemSize = std::floor( std::max(ViewportSize - 2 * ItemMargin,0.0));
	}
	else {
		auto itemWithMarginSize = ViewportSize / count;
		ComputedItemSize = std::floor(std::max(itemWithMarginSize - 2 * ItemMargin,0.0));
	}
}
