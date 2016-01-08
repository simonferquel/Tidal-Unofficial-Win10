//
// PlayerSeekBar.xaml.h
// Declaration of the PlayerSeekBar class
//

#pragma once

#include "PlayerSeekBar.g.h"
#include <chrono>
namespace Tidal
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PlayerSeekBar sealed
	{
	private:
		std::chrono::seconds _lastPosition;
		std::chrono::seconds _lastDuration;
		Windows::UI::Xaml::DispatcherTimer^ _timer;
	public:
		PlayerSeekBar();
	private:
		void OnSliderPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnSliderReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnSliderMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnUnloaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTick(Platform::Object ^sender, Platform::Object ^args);
	};
}
