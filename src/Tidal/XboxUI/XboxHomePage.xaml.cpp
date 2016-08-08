//
// XboxHomePage.xaml.cpp
// Implementation of the XboxHomePage class
//

#include "pch.h"
#include "XboxHomePage.xaml.h"
#include <WindowsNumerics.h>
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
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

XboxHomePage::XboxHomePage()
{
	InitializeComponent();
	auto bgVisual = ElementCompositionPreview::GetElementVisual(bg);
	auto compositor = bgVisual->Compositor;
	auto bluringVisual = compositor->CreateSpriteVisual();
	auto backdrop = compositor->CreateBackdropBrush();
	auto blurDef = ref new Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect();
	blurDef->Name = L"blur";
	blurDef->BlurAmount = 15.0f;
	blurDef->BorderMode = Microsoft::Graphics::Canvas::Effects::EffectBorderMode::Hard;
	blurDef->Optimization = Microsoft::Graphics::Canvas::Effects::EffectOptimization::Balanced;
	blurDef->Source = ref new CompositionEffectSourceParameter(L"source");
	auto blendColorSource = ref new Microsoft::Graphics::Canvas::Effects::ColorSourceEffect();
	blendColorSource->Name = L"color";
	blendColorSource->Color = Windows::UI::ColorHelper::FromArgb(255, 100, 170, 170);
	auto blendDef = ref new Microsoft::Graphics::Canvas::Effects::BlendEffect();
	blendDef->Name = L"blend";
	blendDef->Foreground = blendColorSource;
	blendDef->Background = blurDef;
	//blendDef->Background = ref new CompositionEffectSourceParameter(L"source");
	auto factory = compositor->CreateEffectFactory(blendDef);
	auto brush = factory->CreateBrush();
	brush->SetSourceParameter(L"source", backdrop);
	bluringVisual->Brush = brush;
	ElementCompositionPreview::SetElementChildVisual(bgBlur, bluringVisual);
	CompositionPropertySet^ props = hub->CompositionPropertySet;
	_homePagePropertySet = compositor->CreatePropertySet();
	_homePagePropertySet->InsertScalar(L"HomeWidth", 0);
	auto offsetAnim = compositor->CreateExpressionAnimation(L"Vector3(-(0.4*hub.TotalExtent - home.HomeWidth)*(hub.CurrentOffsetX / (hub.TotalExtent - hub.ViewportWidth)) , 0,0)");
	
	offsetAnim->SetReferenceParameter(L"hub", props);
	offsetAnim->SetReferenceParameter(L"home", _homePagePropertySet);
	bgVisual->StartAnimation(L"Offset", offsetAnim);
	bgAnim->Begin();
}


void Tidal::XboxHomePage::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{

	_homePagePropertySet->InsertScalar(L"HomeWidth", ActualWidth);
	auto visual = ElementCompositionPreview::GetElementChildVisual(bgBlur);
	visual->Size = Windows::Foundation::Numerics::float2(ActualWidth, ActualHeight);
}
