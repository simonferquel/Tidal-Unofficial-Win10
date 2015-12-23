#pragma once
namespace Tidal {
	public ref class MetricsHelper sealed : public Windows::UI::Xaml::DependencyObject {
	private:
		static Windows::UI::Xaml::DependencyProperty^ _DesiredItemSizeProperty;
		static Windows::UI::Xaml::DependencyProperty^ _ViewportSizeProperty;
		static Windows::UI::Xaml::DependencyProperty^ _ItemMarginProperty;
		static Windows::UI::Xaml::DependencyProperty^ _ComputedItemSizeProperty;
	public:
		static property Windows::UI::Xaml::DependencyProperty^ DesiredItemSizeProperty { Windows::UI::Xaml::DependencyProperty^  get() { return _DesiredItemSizeProperty; }}
		static property Windows::UI::Xaml::DependencyProperty^ ViewportSizeProperty { Windows::UI::Xaml::DependencyProperty^  get() { return _ViewportSizeProperty; }}
		static property Windows::UI::Xaml::DependencyProperty^ ItemMarginProperty { Windows::UI::Xaml::DependencyProperty^  get() { return _ItemMarginProperty; }}
		static property Windows::UI::Xaml::DependencyProperty^ ComputedItemSizeProperty { Windows::UI::Xaml::DependencyProperty^  get() { return _ComputedItemSizeProperty; }}

		property double DesiredItemSize {
			double get() { return static_cast<Platform::IBox<double>^>(GetValue(DesiredItemSizeProperty))->Value; }
			void set(double value) { SetValue(DesiredItemSizeProperty, value); }
		}
		property double ViewportSize {
			double get() { return static_cast<Platform::IBox<double>^>(GetValue(ViewportSizeProperty))->Value; }
			void set(double value) { SetValue(ViewportSizeProperty, value); }
		}
		property double ItemMargin {
			double get() { return static_cast<Platform::IBox<double>^>(GetValue(ItemMarginProperty))->Value; }
			void set(double value) { SetValue(ItemMarginProperty, value); }
		}
		property double ComputedItemSize {
			double get() { return static_cast<Platform::IBox<double>^>(GetValue(ComputedItemSizeProperty))->Value; }
			void set(double value) { SetValue(ComputedItemSizeProperty, value); }
		}
		void UpdateComputedItemSize();
	};
}