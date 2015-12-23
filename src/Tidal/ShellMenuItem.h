#pragma once
namespace Tidal {
	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class ShellMenuItem sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged{
	private:
		Windows::UI::Xaml::Visibility _badgeVisibility = Windows::UI::Xaml::Visibility::Collapsed;
		int _badgeCount = 0;
	public:
		property Windows::UI::Xaml::Controls::IconElement^ Icon;
		property Platform::String^ Label;
		property Windows::UI::Xaml::Interop::TypeName PageType;
		bool Match(Windows::UI::Xaml::Interop::TypeName type) {
			return type.Name == PageType.Name
				&& type.Kind == PageType.Kind;
		}
		property Windows::UI::Xaml::Visibility BadgeVisibility {Windows::UI::Xaml::Visibility get() { return _badgeVisibility; }
		void set(Windows::UI::Xaml::Visibility value) {
			if (value != _badgeVisibility) {
				_badgeVisibility = value;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"BadgeVisibility"));
			}
		}}


		property int BadgeCount {int get() { return _badgeCount; }
		void set(int value) {
			if (value != _badgeCount) {
				_badgeCount = value;
				PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"BadgeCount"));
			}
		}}

		// Inherited via INotifyPropertyChanged
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler ^ PropertyChanged;
	};
}