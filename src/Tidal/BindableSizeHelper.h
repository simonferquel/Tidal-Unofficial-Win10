#pragma once
namespace Tidal {
	public ref class BindableSizeHelper sealed : public Windows::UI::Xaml::DependencyObject {
	private:
		static Windows::UI::Xaml::DependencyProperty^ _EnabledProperty;
		static Windows::UI::Xaml::DependencyProperty^ _EventTokenRegistrationProperty;
		static Windows::UI::Xaml::DependencyProperty^ _BindableWidthProperty;
		static Windows::UI::Xaml::DependencyProperty^ _BindableHeightProperty;
	public:
		static property Windows::UI::Xaml::DependencyProperty^ EnabledProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _EnabledProperty; }}
		static property Windows::UI::Xaml::DependencyProperty^ EventTokenRegistrationProperty { Windows::UI::Xaml::DependencyProperty^ get(){return _EventTokenRegistrationProperty;}}
		static property Windows::UI::Xaml::DependencyProperty^ BindableWidthProperty { Windows::UI::Xaml::DependencyProperty^ get(){return _BindableWidthProperty;}}
		static property Windows::UI::Xaml::DependencyProperty^ BindableHeightProperty { Windows::UI::Xaml::DependencyProperty^ get(){return _BindableHeightProperty;}}

		static bool GetEnabled(Windows::UI::Xaml::FrameworkElement^ obj) {
			return static_cast<Platform::IBox<bool>^>(obj->GetValue(EnabledProperty))->Value;
		}
		static void SetEnabled(Windows::UI::Xaml::FrameworkElement^ obj, bool value) {
			obj->SetValue(EnabledProperty, value);
		}
		static Windows::Foundation::EventRegistrationToken GetEventTokenRegistration(Windows::UI::Xaml::FrameworkElement^ obj) {
			return static_cast<Platform::IBox<Windows::Foundation::EventRegistrationToken>^>(obj->GetValue(EventTokenRegistrationProperty))->Value;
		}
		static void SetEventTokenRegistration(Windows::UI::Xaml::FrameworkElement^ obj, Windows::Foundation::EventRegistrationToken value) {
			obj->SetValue(EventTokenRegistrationProperty, value);
		}
		static double GetBindableWidth(Windows::UI::Xaml::FrameworkElement^ obj) {
			return static_cast<Platform::IBox<double>^>(obj->GetValue(BindableWidthProperty))->Value;
		}
		static void SetBindableWidth(Windows::UI::Xaml::FrameworkElement^ obj, double value) {
			obj->SetValue(BindableWidthProperty, value);
		}
		static double GetBindableHeight(Windows::UI::Xaml::FrameworkElement^ obj) {
			return static_cast<Platform::IBox<double>^>(obj->GetValue(BindableHeightProperty))->Value;
		}
		static void SetBindableHeight(Windows::UI::Xaml::FrameworkElement^ obj, double value) {
			obj->SetValue(BindableHeightProperty, value);
		}
	};
}