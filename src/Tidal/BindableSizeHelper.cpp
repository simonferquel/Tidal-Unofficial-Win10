#include "pch.h"
#include "BindableSizeHelper.h"

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Tidal;

void OnSizeChangedForBindableSize(Object^ source, SizeChangedEventArgs^ args) {
	auto fe = dynamic_cast<FrameworkElement^>(source);
	Tidal::BindableSizeHelper::SetBindableWidth(fe, fe->ActualWidth);
	Tidal::BindableSizeHelper::SetBindableHeight(fe, fe->ActualHeight);
}

DependencyProperty^ Tidal::BindableSizeHelper::_EnabledProperty = 
	DependencyProperty::RegisterAttached(
		L"Enabled", 
		bool::typeid, 
		BindableSizeHelper::typeid,
		ref new PropertyMetadata(false, ref new PropertyChangedCallback([](DependencyObject^ source, DependencyPropertyChangedEventArgs^ args) {
	auto fe = dynamic_cast<FrameworkElement^>(source);
	if (!fe) {
		return;
	}
	if (GetEnabled(fe)) {
		SetEventTokenRegistration(fe, fe->SizeChanged += ref new SizeChangedEventHandler(&OnSizeChangedForBindableSize));
		SetBindableWidth(fe, fe->ActualWidth);
		SetBindableHeight(fe, fe->ActualHeight);
	}
	else {
		fe->SizeChanged -= GetEventTokenRegistration(fe);
	}
})));


DependencyProperty^ Tidal::BindableSizeHelper::_EventTokenRegistrationProperty =
	DependencyProperty::RegisterAttached(
		L"EventTokenRegistration",
		EventRegistrationToken::typeid,
		BindableSizeHelper::typeid,
		ref new PropertyMetadata(EventRegistrationToken{}));


DependencyProperty^ Tidal::BindableSizeHelper::_BindableWidthProperty =
DependencyProperty::RegisterAttached(
	L"BindableWidth",
	double::typeid,
	BindableSizeHelper::typeid,
	ref new PropertyMetadata(0.0));
DependencyProperty^ Tidal::BindableSizeHelper::_BindableHeightProperty =
	DependencyProperty::RegisterAttached(
		L"BindableHeight",
		double::typeid,
		BindableSizeHelper::typeid,
		ref new PropertyMetadata(0.0));
