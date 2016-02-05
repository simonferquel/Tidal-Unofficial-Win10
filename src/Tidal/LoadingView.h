//
// LoadingView.h
// Declaration of the LoadingView class.
//

#pragma once

namespace Tidal
{
	public enum class LoadingState {
		Loaded,
		Loading,
		Error
	};

	[Windows::UI::Xaml::Markup::ContentPropertyAttribute(Name=L"Content")]
	public ref class LoadingView sealed : public Windows::UI::Xaml::Controls::Control
	{
	private:
		static Windows::UI::Xaml::DependencyProperty^ _contentProperty;
		static Windows::UI::Xaml::DependencyProperty^ _loadingStateProperty;
		static Windows::UI::Xaml::DependencyProperty^ _loadingTextProperty;
		static Windows::UI::Xaml::DependencyProperty^ _errorTextProperty;
	public:
		LoadingView();

		static property Windows::UI::Xaml::DependencyProperty^ ContentProperty {
			Windows::UI::Xaml::DependencyProperty^ get() { return _contentProperty; }
		}
		static property Windows::UI::Xaml::DependencyProperty^ LoadingStateProperty {
			Windows::UI::Xaml::DependencyProperty^ get() { return _loadingStateProperty; }
		}
		static property Windows::UI::Xaml::DependencyProperty^ LoadingTextProperty {
			Windows::UI::Xaml::DependencyProperty^ get() { return _loadingTextProperty; }
		}
		static property Windows::UI::Xaml::DependencyProperty^ ErrorTextProperty {
			Windows::UI::Xaml::DependencyProperty^ get() { return _errorTextProperty; }
		}

		property Platform::Object^ Content {
			Platform::Object^ get() { return GetValue(ContentProperty); }
			void set(Platform::Object^ value) { SetValue(ContentProperty, value); }
		}
		property Tidal::LoadingState LoadingState {
			Tidal::LoadingState get() { return dynamic_cast<Platform::IBox<Tidal::LoadingState>^>(GetValue(LoadingStateProperty))->Value; }
			void set(Tidal::LoadingState value) { SetValue(LoadingStateProperty, value); }
		}
		property Platform::String^ LoadingText {
			Platform::String^ get() { return dynamic_cast<Platform::String^>(GetValue(LoadingTextProperty)); }
			void set(Platform::String^ value) { SetValue(LoadingTextProperty, value); }
		}
		property Platform::String^ ErrorText {
			Platform::String^ get() { return dynamic_cast<Platform::String^>(GetValue(ErrorTextProperty)); }
			void set(Platform::String^ value) { SetValue(ErrorTextProperty, value); }
		}
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
	};
}
