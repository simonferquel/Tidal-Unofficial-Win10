#include "pch.h"
#include "FactorConverter.h"

Platform::Object ^ Tidal::FactorConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto box = dynamic_cast<Platform::IBox<double>^>(value);
	if (box) {
		return box->Value * Factor;
	}
	return 0.0;
}

Platform::Object ^ Tidal::FactorConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
}
