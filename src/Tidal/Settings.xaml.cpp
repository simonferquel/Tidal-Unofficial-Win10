//
// Settings.xaml.cpp
// Implementation of the Settings class
//

#include "pch.h"
#include "Settings.xaml.h"

using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Settings::Settings()
{
	InitializeComponent();
}


void Tidal::Settings::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto streamingQuality = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"streamingQuality"));
	if (!streamingQuality) {
		streamingQuality = L"HIGH";
		settingsValues->Insert(L"streamingQuality", streamingQuality);
	}

	auto importQuality = dynamic_cast<Platform::String^>(settingsValues->Lookup(L"importQuality"));
	if (!importQuality) {
		importQuality = L"LOSSLESS";
		settingsValues->Insert(L"importQuality", importQuality);
	}

	if (streamingQuality == L"LOW") {
		streamLow->IsChecked = true;
	}
	else if (streamingQuality == L"HIGH") {
		streamHigh->IsChecked = true;
	}
	else if (streamingQuality == L"LOSSLESS") {
		streamLossless->IsChecked = true;
	}



	if (importQuality == L"LOW") {
		importLow->IsChecked = true;
	}
	else if (importQuality == L"HIGH") {
		importHigh->IsChecked = true;
	}
	else if (importQuality == L"LOSSLESS") {
		importLossless->IsChecked = true;
	}
}


void Tidal::Settings::OnStreamingQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	if (fe) {
		auto quality = dynamic_cast<String^>(fe->Tag);
		if (quality) {

			settingsValues->Insert(L"streamingQuality", quality);
		}
	}

}


void Tidal::Settings::OnImportQualityChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto settingsValues = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
	auto fe = dynamic_cast<FrameworkElement^>(sender);
	if (fe) {
		auto quality = dynamic_cast<String^>(fe->Tag);
		if (quality) {

			settingsValues->Insert(L"importQuality", quality);
		}
	}
}
