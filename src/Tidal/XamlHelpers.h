#pragma once

template <typename TControl>
TControl^ FindOwningControl(Windows::UI::Xaml::DependencyObject^ src) {
	if (!src) {
		return nullptr;
	}
	auto parent = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(src);
	auto parentAsScrollViewer = dynamic_cast<TControl^>(parent);
	if (parentAsScrollViewer) {
		return parentAsScrollViewer;
	}
	return FindOwningControl<TControl>(parent);
}
inline Windows::UI::Xaml::Controls::ScrollViewer^ FindOwningScrollViewer(Windows::UI::Xaml::DependencyObject^ src) {
	if (!src) {
		return nullptr;
	}
	auto parent = Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(src);
	auto parentAsScrollViewer = dynamic_cast<Windows::UI::Xaml::Controls::ScrollViewer^>(parent);
	if (parentAsScrollViewer) {
		return parentAsScrollViewer;
	}
	return FindOwningScrollViewer(parent);
}