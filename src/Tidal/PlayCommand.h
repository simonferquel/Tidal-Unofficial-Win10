#pragma once
#include "TrackItemVM.h"
namespace Tidal {
	public ref class PlayCommand sealed : public Windows::UI::Xaml::DependencyObject, public Windows::UI::Xaml::Input::ICommand
	{
	private:
		static Windows::UI::Xaml::DependencyProperty^ _trackProperty;
		static Windows::UI::Xaml::DependencyProperty^ _trackListProperty;
	public:
		PlayCommand() {}
		PlayCommand(TrackItemVM^ singleTrack);
		PlayCommand(TrackItemVM^ startTrack, Windows::Foundation::Collections::IIterable<TrackItemVM^>^ trackList);

		static property Windows::UI::Xaml::DependencyProperty^ TrackProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _trackProperty; } }
		static property Windows::UI::Xaml::DependencyProperty^ TrackListProperty { Windows::UI::Xaml::DependencyProperty^ get() { return _trackListProperty; } }

		property TrackItemVM^ Track {
			TrackItemVM^ get() { return dynamic_cast<TrackItemVM^>(GetValue(TrackProperty)); }
			void set(TrackItemVM^ value) { SetValue(TrackProperty, value); }
		}

		property Windows::Foundation::Collections::IIterable< TrackItemVM^>^ TrackList {
			Windows::Foundation::Collections::IIterable< TrackItemVM^>^ get() { return dynamic_cast<Windows::Foundation::Collections::IIterable< TrackItemVM^>^>(GetValue(TrackListProperty)); }
			void set(Windows::Foundation::Collections::IIterable< TrackItemVM^>^ value) { SetValue(TrackListProperty, value); }
		}
		// Inherited via ICommand
		virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^ CanExecuteChanged;
		virtual bool CanExecute(Platform::Object ^parameter);
		virtual void Execute(Platform::Object ^parameter);
	};
}

