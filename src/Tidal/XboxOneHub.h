//
// XboxOneHub.h
// Declaration of the XboxOneHub class.
//

#pragma once

namespace Tidal
{
	namespace _ {
		using namespace Platform;
		using namespace Windows::UI::Xaml;
		using namespace Windows::UI::Xaml::Controls;
		using namespace Windows::UI::Xaml::Controls::Primitives;
		using namespace Windows::UI::Xaml::Input;
		using namespace Windows::UI::Xaml::Data;
		using namespace Windows::UI::Composition;
	}
	public ref class BoolToVisibilityConverter sealed : _::IValueConverter {
	public:
		// Inherited via IValueConverter
		virtual Platform::Object ^ Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
		virtual Platform::Object ^ ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language);
	};
	public ref class FocusHelper sealed : _::DependencyObject {
	public:
		static property _::DependencyProperty^ IsBindableFocusEnabledProperty {_::DependencyProperty^ get(); }
		static property _::DependencyProperty^ IsFocusWithinProperty {_::DependencyProperty^ get(); }
		static bool GetIsBindableFocusEnabled(_::DependencyObject^ ctl) {
			return dynamic_cast<_::IBox<bool>^>(ctl->GetValue(IsBindableFocusEnabledProperty))->Value;
		}
		static void SetIsBindableFocusEnabled(_::DependencyObject^ ctl, bool value) {
			ctl->SetValue(IsBindableFocusEnabledProperty, value);
		}
		static bool GetIsFocusWithin(_::DependencyObject^ ctl) {
			return dynamic_cast<_::IBox<bool>^>(ctl->GetValue(IsFocusWithinProperty))->Value;
		}
		static void SetIsFocusWithin(_::DependencyObject^ ctl, bool value) {
			ctl->SetValue(IsFocusWithinProperty, value);
		}
	};

	public ref class XboxOneHubSection sealed : public _::ListBoxItem {
	private:
	public:
		XboxOneHubSection();
		static property _::DependencyProperty^ HeaderProperty { _::DependencyProperty^ get(); }
		static property _::DependencyProperty^ HasHeaderFocusProperty {_::DependencyProperty^ get(); }
		static property _::DependencyProperty^ HeaderTemplateProperty { _::DependencyProperty^ get(); }

		property _::Object^ Header
		{
			_::Object^ get() {
				return GetValue(HeaderProperty);
			}
			void set(_::Object^ value) {
				SetValue(HeaderProperty, value);
			}
		}

		property _::DataTemplate^ HeaderTemplate
		{
			_::DataTemplate^ get() {
				return dynamic_cast<_::DataTemplate^>(GetValue(HeaderTemplateProperty));
			}
			void set(_::DataTemplate^ value) {
				SetValue(HeaderTemplateProperty, value);
			}
		}
		property bool HasHeaderFocus {
			bool get() {
				return dynamic_cast<_::IBox<bool>^>(GetValue(HasHeaderFocusProperty))->Value;
			}
			void set(bool value) {
				SetValue(HasHeaderFocusProperty, value);
			}
		}
		void OnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e);
	};

	[Windows::UI::Xaml::TemplatePartAttribute(Type = Windows::UI::Xaml::Controls::Canvas::typeid, Name = L"PART_Body")]
	[Windows::UI::Xaml::TemplatePartAttribute(Type = Windows::UI::Xaml::Controls::Control::typeid, Name = L"PART_BodyContainer")]
	[Windows::UI::Xaml::TemplatePartAttribute(Type = Windows::UI::Xaml::Controls::Control::typeid, Name = L"PART_Header")]
	public ref class XboxOneHub sealed : public _::ListBox
	{
	private:
		_::CompositionScopedBatch^ _currentSelectionAnimBatch;
		_::Compositor^ _compositor;
		_::Canvas^ _body;
		_::Control^ _bodyContainer;
		_::Control^ _header;
	public:
		XboxOneHub();
		static property _::DependencyProperty^ BodyPresenterProperty {_::DependencyProperty^ get(); }
		static _::ContentPresenter^ GetBodyPresenter(_::DependencyObject^ container) {
			return dynamic_cast<_::ContentPresenter^>(container->GetValue(BodyPresenterProperty));
		}
		static void SetBodyPresenter(_::DependencyObject^ container, _::ContentPresenter^ value) {
			container->SetValue(BodyPresenterProperty, value);
		}
		static property _::DependencyProperty^ CompositionPropertySetProperty {_::DependencyProperty^ get(); }
		property Windows::UI::Composition::CompositionPropertySet^ CompositionPropertySet {Windows::UI::Composition::CompositionPropertySet^ get() {
			return dynamic_cast<Windows::UI::Composition::CompositionPropertySet^>(GetValue(CompositionPropertySetProperty));
		}}


		static property _::DependencyProperty^ TotalExtentProperty {_::DependencyProperty^ get(); }
		property double TotalExtent {double get() {
			return dynamic_cast<_::IBox<double>^>(GetValue(TotalExtentProperty))->Value;
		}}
		
	private:
		void ComputeTotalExtentAndMaterializeBodyPresenters();
	protected:
		virtual bool IsItemItsOwnContainerOverride(_::Object^ item) override;
		virtual _::DependencyObject^ GetContainerForItemOverride() override;
		virtual void OnKeyDown(_::KeyRoutedEventArgs^ e) override;
		void OnSelectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^e);
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e);

		virtual void OnItemsChanged(_::Object^ e) override {
			_::ListBox::OnItemsChanged(e);
			ComputeTotalExtentAndMaterializeBodyPresenters();
		}
		void OnCompleted(Platform::Object ^sender, Windows::UI::Composition::CompositionBatchCompletedEventArgs ^args);
	};
}
