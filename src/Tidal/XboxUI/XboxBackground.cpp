//
// XboxBackground.cpp
// Implementation of the XboxBackground class.
//

#include "pch.h"
#include "XboxBackground.h"
#include <WindowsNumerics.h>
#include <tools/TimeUtils.h>
#include <random>
#include <vector>
#include <pplawait2.h>
#include <tools/AsyncHelpers.h>
#include <Api/GetPromotionsQuery.h>
#include <Hat.h>
#include <Api/PromotionArticle.h>

using namespace Tidal;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Composition;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::UI::Composition;
using namespace Windows::Graphics::DirectX;
using namespace Microsoft::Graphics::Canvas::Effects;

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235


class TileSlot {
private:
	ScalarKeyFrameAnimation^ _opacityAnim;
	Compositor^ _compositor;
	ContainerVisual^ _slotVisual;
public:
	Visual^ visual()const { return _slotVisual; }
	void setContent(Visual^ v) {
		_slotVisual->Children->InsertAtBottom(v);
		auto it = _slotVisual->Children->First();
		if (_slotVisual->Children->Count > 1) {
			auto batch = _compositor->CreateScopedBatch(CompositionBatchTypes::Animation);
			batch->Completed += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Windows::UI::Composition::CompositionBatchCompletedEventArgs ^>([coll = _slotVisual->Children, keep = v](Platform::Object ^, Windows::UI::Composition::CompositionBatchCompletedEventArgs ^){
				while (coll->Count > 1) {
					auto it = coll->First();
					it->MoveNext();
					coll->Remove(it->Current);
				}
			});
			auto cur = _slotVisual->Children->First();
			while (cur->MoveNext())
			{
				cur->Current->StartAnimation(L"Opacity", _opacityAnim);
			}
			batch->End();

		}
		else {
			v->Opacity = 0;
			auto fadeIn = _compositor->CreateScalarKeyFrameAnimation();
			fadeIn->Duration = tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(1000));
			fadeIn->InsertKeyFrame(1, 1, _compositor->CreateLinearEasingFunction());
			v->StartAnimation(L"Opacity", fadeIn);
		}
	}
	TileSlot(Compositor^ compositor) : _compositor(compositor), _slotVisual(compositor->CreateContainerVisual())
	{
		_opacityAnim = _compositor->CreateScalarKeyFrameAnimation();
		_opacityAnim->Duration = tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(1000));
		_opacityAnim->InsertKeyFrame(1, 0, _compositor->CreateLinearEasingFunction());
	}
};

class Tidal::BackgroundTiles {
private:
	std::random_device rd;
	Compositor^ _compositor;
	ContainerVisual^ _globalContainer;
	ContainerVisual^ _rowContainers[3];
	CompositionPropertySet^ _metrics;
	ExpressionAnimation^ cellSizeBinding;
	std::vector<std::unique_ptr<TileSlot>> _emptySlots;
	std::vector<std::unique_ptr<TileSlot>> _activeSlots;
	CompositionEffectBrush^ _blurBrush;
	bool _isOnController0 = true;
public:
	bool hasEmptySlots() const {
		return !_emptySlots.empty();
	}

	void pushContent(Visual^ v) {
		v->StartAnimation(L"Size", cellSizeBinding);
		if (hasEmptySlots()) {
			auto slot = std::move(_emptySlots[0]);
			_emptySlots.erase(_emptySlots.begin());
			slot->setContent(v);
			_activeSlots.push_back(std::move(slot));
		}
		else {
			auto ix = std::uniform_int_distribution<int>(0, _activeSlots.size() - 1)(rd);
			_activeSlots[ix]->setContent(v);
		}
	}
	Compositor^ compositor() const { return _compositor; }
	CompositionEffectBrush^ blurBrush() const { return _blurBrush; }
	ContainerVisual^ composedVisual() const {
		return _globalContainer;
	}
	BackgroundTiles(Visual^ referenceVisual) {
		_compositor = referenceVisual->Compositor;
		_globalContainer = _compositor->CreateContainerVisual();
		_metrics = _compositor->CreatePropertySet();
		_metrics->InsertVector2(L"imgSize", float2(0));
		_metrics->InsertScalar(L"parallaxScale", 0);
		_metrics->InsertScalar(L"parallaxAmount", 0);
		_metrics->InsertScalar(L"parallaxAmountControler0", 0);
		_metrics->InsertScalar(L"parallaxAmountControler1", 0);
		_metrics->InsertScalar(L"parallaxAmountControlerCrossFade", 0);
		{

			auto parallaxAmountAnim = _compositor->CreateExpressionAnimation(L"Lerp(This.Target.parallaxAmountControler0, This.Target.parallaxAmountControler1, This.Target.parallaxAmountControlerCrossFade)");
			_metrics->StartAnimation(L"parallaxAmount", parallaxAmountAnim);
		}

		{
			auto metricsAnim = _compositor->CreateExpressionAnimation(L"Vector2(ref.Size.Y/3 / 0.7272727272727273, ref.Size.Y/3)");
			metricsAnim->SetReferenceParameter(L"ref", referenceVisual);
			_metrics->StartAnimation(L"imgSize", metricsAnim);
			auto parallaxAnim = _compositor->CreateExpressionAnimation(L"-(This.Target.imgSize.X*9 - ref.Size.X)");
			parallaxAnim->SetReferenceParameter(L"ref", referenceVisual);
			_metrics->StartAnimation(L"parallaxScale", parallaxAnim);
		}
		auto sizeBinding = _compositor->CreateExpressionAnimation(L"Vector2(metrics.imgSize.X*9, metrics.ImgSize.Y*3)");
		sizeBinding->SetReferenceParameter(L"metrics", _metrics);
		{
			_globalContainer->StartAnimation(L"Size", sizeBinding);
		}
		auto rowSizeBinding = _compositor->CreateExpressionAnimation(L"Vector2(metrics.imgSize.X*10, metrics.ImgSize.Y)");
		auto offsetBinding = _compositor->CreateExpressionAnimation(L"Vector3(metrics.parallaxScale * metrics.parallaxAmount, metrics.ImgSize.Y * This.Target.rowIndex,0)");

		auto cellOffsetBinding = _compositor->CreateExpressionAnimation(L"Vector3(metrics.ImgSize.X * This.Target.colIndex,0,0)");
		cellSizeBinding = _compositor->CreateExpressionAnimation(L"metrics.imgSize");
		rowSizeBinding->SetReferenceParameter(L"metrics", _metrics);
		offsetBinding->SetReferenceParameter(L"metrics", _metrics);
		cellOffsetBinding->SetReferenceParameter(L"metrics", _metrics);
		cellSizeBinding->SetReferenceParameter(L"metrics", _metrics);
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, 256);

		_metrics->InsertScalar(L"translationAmount", 0);

		auto translateAnim = _compositor->CreateScalarKeyFrameAnimation();
		translateAnim->Duration = tools::time::ToWindowsTimeSpan(std::chrono::seconds(30));
		translateAnim->IterationBehavior = AnimationIterationBehavior::Forever;
		translateAnim->InsertKeyFrame(0, 0, _compositor->CreateLinearEasingFunction());
		translateAnim->InsertKeyFrame(1, 2 * std::_Pi, _compositor->CreateLinearEasingFunction());
		_metrics->StartAnimation(L"translationAmount", translateAnim);
		auto rowTranformAnim = _compositor->CreateExpressionAnimation(L"Matrix4x4(Matrix3x2.CreateTranslation(Vector2(-0.5 * metrics.ImgSize.X + 0.5*metrics.ImgSize.X*Cos(metrics.translationAmount + This.Target.rowIndex), 0)))");
		rowTranformAnim->SetReferenceParameter(L"metrics", _metrics);

		for (auto rowIndex = 0; rowIndex < 3; ++rowIndex) {
			auto rowContainer = _compositor->CreateContainerVisual();
			rowContainer->Properties->InsertScalar(L"rowIndex", rowIndex);
			rowContainer->StartAnimation(L"Size", rowSizeBinding);
			rowContainer->StartAnimation(L"Offset", offsetBinding);
			_globalContainer->Children->InsertAtTop(rowContainer);
			_rowContainers[rowIndex] = rowContainer;

			rowContainer->StartAnimation(L"TransformMatrix", rowTranformAnim);

			for (auto colIndex = 0; colIndex < 10; ++colIndex)
			{
				auto slot = std::make_unique<TileSlot>(_compositor);

				/*auto sprite = _compositor->CreateSpriteVisual();
				sprite->Brush = _compositor->CreateColorBrush(Windows::UI::ColorHelper::FromArgb(255, dist(rd), dist(rd), dist(rd)));*/
				slot->visual()->Properties->InsertScalar(L"colIndex", colIndex);
				slot->visual()->StartAnimation(L"Offset", cellOffsetBinding);
				slot->visual()->StartAnimation(L"Size", cellSizeBinding);
				rowContainer->Children->InsertAtTop(slot->visual());

				_emptySlots.push_back(std::move(slot));
			}
		}
		auto backdropBrush = _compositor->CreateBackdropBrush();

		auto desat = ref new SaturationEffect();
		desat->Name = L"Desat";
		desat->Source = ref new CompositionEffectSourceParameter("source");
		desat->Saturation = 0.3;

		auto darken = ref new ColorSourceEffect();
		darken->Name = L"BlendForeground";
		darken->Color = Windows::UI::ColorHelper::FromArgb(164,12,54,60);
		auto blendEffect = ref new BlendEffect();
		blendEffect->Name = L"Blend";
		blendEffect->Background = desat;
		blendEffect->Foreground = darken;
		blendEffect->Mode = BlendEffectMode::Multiply;

		auto blurEffect = ref new GaussianBlurEffect();
		blurEffect->Name = L"Blur";
		blurEffect->BlurAmount = 10.0f;
		blurEffect->BorderMode = EffectBorderMode::Hard;		
		blurEffect->Optimization = EffectOptimization::Balanced;
		blurEffect->Source = blendEffect;


		
		auto params = ref new Platform::Collections::Vector<String^>();
		params->Append(L"Desat.Saturation");
		params->Append(L"BlendForeground.Color");
		params->Append(L"Blur.BlurAmount");
		auto effectFactory = _compositor->CreateEffectFactory(blurEffect, params);
		
		auto blurBrush = effectFactory->CreateBrush();
		_blurBrush = blurBrush;
		blurBrush->SetSourceParameter(L"source", backdropBrush);
		auto blurVisual = _compositor->CreateSpriteVisual();
		blurVisual->Brush = blurBrush;
		blurVisual->StartAnimation(L"Size", sizeBinding);
		_globalContainer->Children->InsertAtTop(blurVisual);
	}

	void setParallaxAmountBinding(ExpressionAnimation^ anim) {
		if (_isOnController0) {
			_metrics->StartAnimation(L"parallaxAmountControler1", anim);
			auto crossfadeAnim = _compositor->CreateScalarKeyFrameAnimation();
			crossfadeAnim->Duration = tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(250));
			crossfadeAnim->InsertKeyFrame(1, 1);
			_metrics->StartAnimation(L"parallaxAmountControlerCrossFade", crossfadeAnim);
			_isOnController0 = false;
		}
		else {

			_metrics->StartAnimation(L"parallaxAmountControler0", anim);
			auto crossfadeAnim = _compositor->CreateScalarKeyFrameAnimation();
			crossfadeAnim->Duration = tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(250));
			crossfadeAnim->InsertKeyFrame(1, 0);
			_metrics->StartAnimation(L"parallaxAmountControlerCrossFade", crossfadeAnim);
			_isOnController0 = true;
		}
	}
};

double XboxBackground::Saturation::get() {
	if (!_tiles) {
		return 1;
	}
	float val = 1;
	_tiles->blurBrush()->Properties->TryGetScalar(L"Desat.Saturation", &val);
	return val;
}
void XboxBackground::Saturation::set(double value) {
	if (!_tiles) {
		return;
	}
	_tiles->blurBrush()->Properties->InsertScalar(L"Desat.Saturation", value);
}
double XboxBackground::BlurAmount::get() {
	if (!_tiles) {
		return 5;
	}
	float val = 5;
	_tiles->blurBrush()->Properties->TryGetScalar(L"Blur.BlurAmount", &val);
	return val;
}
void XboxBackground::BlurAmount::set(double value) {
	if (!_tiles) {
		return;
	}
	_tiles->blurBrush()->Properties->InsertScalar(L"Blur.BlurAmount", value);
}
double XboxBackground::BlendAlpha::get() {
	if (!_tiles) {
		return 0;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	return val.A;
}


void XboxBackground::BlendAlpha::set(double value) {
	if (!_tiles) {
		return;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	val.A = value;
	_tiles->blurBrush()->Properties->InsertColor(L"BlendForeground.Color", val);
}

double XboxBackground::BlendRed::get() {
	if (!_tiles) {
		return 0;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	return val.R;
}

void XboxBackground::BlendRed::set(double value) {
	if (!_tiles) {
		return;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	val.R = value;
	_tiles->blurBrush()->Properties->InsertColor(L"BlendForeground.Color", val);
}

double XboxBackground::BlendGreen::get() {
	if (!_tiles) {
		return 0;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	return val.G;
}
void XboxBackground::BlendGreen::set(double value) {
	if (!_tiles) {
		return;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	val.G = value;
	_tiles->blurBrush()->Properties->InsertColor(L"BlendForeground.Color", val);
}

double XboxBackground::BlendBlue::get() {
	if (!_tiles) {
		return 0;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	return val.B;
}

void XboxBackground::BlendBlue::set(double value) {
	if (!_tiles) {
		return;
	}
	Windows::UI::Color val = Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0);
	_tiles->blurBrush()->Properties->TryGetColor(L"BlendForeground.Color", &val);
	val.B = value;
	_tiles->blurBrush()->Properties->InsertColor(L"BlendForeground.Color", val);
}

XboxBackground::XboxBackground()
{
	DefaultStyleKey = "Tidal.XboxBackground";
	IsTabStop = false;
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Tidal::XboxBackground::OnLoaded);
	Unloaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Tidal::XboxBackground::OnUnloaded);
}

Tidal::XboxBackground::~XboxBackground()
{
}

concurrency::task<CompositionSurfaceBrush^> LoadBitmapAsync(Hat<Compositor> compositor, Hat<Platform::String> url) {
	auto canvasDevice = ref new CanvasDevice();
	auto canvasBmp = co_await CanvasBitmap::LoadAsync(canvasDevice, ref new Uri(url.get()));
	auto compositionDevice = CanvasComposition::CreateCompositionGraphicsDevice(compositor.get(), canvasDevice);
	auto surface = compositionDevice->CreateDrawingSurface(Windows::Foundation::Size(canvasBmp->SizeInPixels.Width, canvasBmp->SizeInPixels.Height), DirectXPixelFormat::B8G8R8A8UIntNormalized, DirectXAlphaMode::Premultiplied);
	{
		auto ds = CanvasComposition::CreateDrawingSession(surface);
		ds->Clear(Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0));
		ds->DrawImage(canvasBmp, Rect(0, 0, canvasBmp->SizeInPixels.Width, canvasBmp->SizeInPixels.Height), Rect(0, 0, canvasBmp->SizeInPixels.Width, canvasBmp->SizeInPixels.Height));
	}
	return compositor->CreateSurfaceBrush(surface);
}

concurrency::task<void> animateBackground(std::shared_ptr<BackgroundTiles> tiles, concurrency::cancellation_token cancelToken) {
	api::GetPromotionsQuery query(1000, 0, L"NEWS", L"US");
	try {
		auto news = co_await query.executeAsync(cancelToken);
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, news->items.size() - 1);
		while (!cancelToken.is_canceled()) {
			while (tiles->hasEmptySlots() && !cancelToken.is_canceled()) {
				auto visual = tiles->compositor()->CreateSpriteVisual();
				auto url = tools::strings::toWindowsString(news->items.at(dist(rd)).imageURL);
				visual->Brush = co_await LoadBitmapAsync(tiles->compositor(), url);
				tiles->pushContent(visual);
			}
			if (cancelToken.is_canceled())
			{
				return;
			}
			co_await tools::async::WaitFor(tools::time::ToWindowsTimeSpan(std::chrono::seconds(3)), cancelToken);
			{
				auto visual = tiles->compositor()->CreateSpriteVisual();
				auto url = tools::strings::toWindowsString(news->items.at(dist(rd)).imageURL);
				visual->Brush = co_await LoadBitmapAsync(tiles->compositor(), url);
				tiles->pushContent(visual);
			}
		}
	}
	catch (...) {}
}


void Tidal::XboxBackground::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	if (_tiles)
	{
		return;
	}
	auto referenceVisual = ElementCompositionPreview::GetElementVisual(this);
	_tiles = std::make_shared<BackgroundTiles>(referenceVisual);

	ElementCompositionPreview::SetElementChildVisual(this, _tiles->composedVisual());
	animateBackground(_tiles, _cts.get_token());
}


void Tidal::XboxBackground::OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	_cts.cancel();
	_tiles = nullptr;
}

void Tidal::XboxBackground::SetParallaxAmountBinding(Windows::UI::Composition::ExpressionAnimation ^ anim)
{
	if (!_tiles) {
		auto referenceVisual = ElementCompositionPreview::GetElementVisual(this);
		_tiles = std::make_shared<BackgroundTiles>(referenceVisual);

		ElementCompositionPreview::SetElementChildVisual(this, _tiles->composedVisual());
		animateBackground(_tiles, _cts.get_token());
	}
	_tiles->setParallaxAmountBinding(anim);
}
