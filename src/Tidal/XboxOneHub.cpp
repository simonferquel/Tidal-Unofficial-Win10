//
// XboxOneHub.cpp
// Implementation of the XboxOneHub class.
//

#include "pch.h"
#include "XboxOneHub.h"
#include <vector>
#include <tools/TimeUtils.h>
#include <WindowsNumerics.h>

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

// The Templated Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234235


template<typename TProp, typename TOwner>
_::DependencyProperty^ RegisterDependencyProperty(Platform::String^ name, _::PropertyMetadata^ metadata) {
	return _::DependencyProperty::Register(name, TProp::typeid, TOwner::typeid, metadata);
}
template<typename TProp, typename TOwner>
_::DependencyProperty^ RegisterDependencyProperty(Platform::String^ name, const TProp& value) {
	return RegisterDependencyProperty<TProp, TOwner>(name, ref new PropertyMetadata(value));
}

template<typename TProp, typename TOwner>
_::DependencyProperty^ RegisterAttachedProperty(Platform::String^ name, _::PropertyMetadata^ metadata) {
	return _::DependencyProperty::RegisterAttached(name, TProp::typeid, TOwner::typeid, metadata);
}
template<typename TProp, typename TOwner>
_::DependencyProperty^ RegisterAttachedProperty(Platform::String^ name, const TProp& value) {
	return RegisterAttachedProperty<TProp, TOwner>(name, ref new PropertyMetadata(value));
}



Control^ FindFirstTabStopChild(DependencyObject^ root) {
	
	auto childCount = VisualTreeHelper::GetChildrenCount(root);
	for (auto ix = 0; ix < childCount; ++ix) {
		auto child = VisualTreeHelper::GetChild(root, ix);
		auto childCtl = dynamic_cast<Control^>(child);
		if (childCtl && childCtl->IsTabStop) {
			return childCtl;
		}
	}
	return nullptr;
}


bool IsWithin(DependencyObject^ root, DependencyObject^ child) {
	
	auto parent = VisualTreeHelper::GetParent(child);
	if (parent == root) {
		return true;
	}
	else if (parent == nullptr) {
		return false;
	}
	return IsWithin(root, parent);
}

Control^ FindFirstTabStopDescendant(DependencyObject^ root) {
	std::vector<DependencyObject^> roots{ root };
	std::vector<DependencyObject^> newGen;
	while (!roots.empty()) {
		for (auto& r : roots) {
			auto candidate = FindFirstTabStopChild(r);
			if (candidate) {
				return candidate;
			}

			auto childCount = VisualTreeHelper::GetChildrenCount(r);
			for (auto ix = 0; ix < childCount; ++ix) {
				auto child = VisualTreeHelper::GetChild(r, ix);
				newGen.push_back(child);
			}
		}
		roots = std::move(newGen);
		newGen.clear();
	}
	return nullptr;
}

XboxOneHub::XboxOneHub()
{
	DefaultStyleKey = "Tidal.XboxOneHub";
	SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, &Tidal::XboxOneHub::OnSelectionChanged);
	Loaded += ref new Windows::UI::Xaml::RoutedEventHandler(this, &Tidal::XboxOneHub::OnLoaded);
	auto visual = ElementCompositionPreview::GetElementVisual(this);
	_compositor = visual->Compositor;
	SetValue(CompositionPropertySetProperty, _compositor->CreatePropertySet());
	CompositionPropertySet->InsertScalar(L"TotalExtent", 0.0f);
	CompositionPropertySet->InsertScalar(L"Height", 0.0f);
	CompositionPropertySet->InsertScalar(L"CurrentOffsetX", 0.0f);
	CompositionPropertySet->InsertScalar(L"ViewportWidth", 0.0f);
	SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Tidal::XboxOneHub::OnSizeChanged);
}

DependencyProperty^ XboxOneHub_BodyPresenterProperty = RegisterAttachedProperty<ContentPresenter, XboxOneHub>(L"BodyPresenter", ref new PropertyMetadata(nullptr));
DependencyProperty^ Tidal::XboxOneHub::BodyPresenterProperty::get() {
	return XboxOneHub_BodyPresenterProperty;
}
DependencyProperty^ XboxOneHub_CompositionPropertySetProperty = RegisterDependencyProperty<CompositionPropertySet, XboxOneHub>(L"CompositionPropertySet", ref new PropertyMetadata(nullptr));
DependencyProperty^ XboxOneHub::CompositionPropertySetProperty::get() {
	return XboxOneHub_CompositionPropertySetProperty;
}

DependencyProperty^ XboxOneHub_TotalExtentProperty = RegisterDependencyProperty<double, XboxOneHub>(L"TotalExtent", 0.0);
DependencyProperty^ XboxOneHub::TotalExtentProperty::get() {
	return XboxOneHub_TotalExtentProperty;
}
void Tidal::XboxOneHub::ComputeTotalExtentAndMaterializeBodyPresenters()
{
	if (!_body) {
		return;
	}
	auto totalWidth = ActualWidth;
	auto sectionWidth = _body->ActualWidth;
	auto extent = totalWidth;
	if (Items->Size > 1) {
		extent = totalWidth + (Items->Size - 1)*sectionWidth;
	}
	CompositionPropertySet->InsertScalar(L"Height", ActualHeight);
	CompositionPropertySet->InsertScalar(L"TotalExtent", static_cast<float>(extent));
	CompositionPropertySet->InsertScalar(L"ViewportWidth", static_cast<float>(ActualWidth));
	SetValue(TotalExtentProperty, extent);
	for (auto ix = 0; ix < Items->Size; ++ix) {
		auto container = dynamic_cast<XboxOneHubSection^>(ContainerFromIndex(ix));
		auto presenter = GetBodyPresenter(container);
		if (!presenter) {
			presenter = ref new ContentPresenter();
			presenter->Content = container->Content;
			presenter->ContentTemplate = container->ContentTemplate;
			SetBodyPresenter(container, presenter);
			presenter->Width = sectionWidth;
			presenter->Height = _body->ActualHeight;
			_header->RegisterPropertyChangedCallback(FocusHelper::IsFocusWithinProperty,
				ref new DependencyPropertyChangedCallback([container](Windows::UI::Xaml::DependencyObject^ sender, Windows::UI::Xaml::DependencyProperty^ dp) {
				container->HasHeaderFocus = FocusHelper::GetIsFocusWithin(sender);
			}));
			_body->Children->Append(presenter);
			
		}
		else {
			presenter->Width = sectionWidth;
			presenter->Height = _body->ActualHeight;
		}
		auto visual = ElementCompositionPreview::GetElementVisual(presenter);
		//visual->Offset = Windows::Foundation::Numerics::float3(100 * ix, 0, 0);
		auto offset = sectionWidth * ix;
		auto anim = _compositor->CreateExpressionAnimation(L"Vector3(thisOffset - shared.CurrentOffsetX,0,0)");
		anim->SetReferenceParameter(L"shared", CompositionPropertySet);
		anim->SetScalarParameter(L"thisOffset", offset);
		visual->StartAnimation(L"Offset", anim);
	}
}
bool Tidal::XboxOneHub::IsItemItsOwnContainerOverride(Platform::Object ^ item)
{
	return dynamic_cast<XboxOneHubSection^>(item) != nullptr;
}

Windows::UI::Xaml::DependencyObject ^ Tidal::XboxOneHub::GetContainerForItemOverride()
{
	return ref new XboxOneHubSection();
}

bool ShouldGoLeft(Windows::System::VirtualKey key, DependencyObject^ rootElem) {
	if (key == Windows::System::VirtualKey::GamepadLeftShoulder) {
		return true;
	}
	if (key == Windows::System::VirtualKey::GamepadDPadLeft || key == Windows::System::VirtualKey::GamepadLeftThumbstickLeft) {
		if (rootElem) {
			auto leftFocusCandidate = FocusManager::FindNextFocusableElement(FocusNavigationDirection::Left);
			if (!leftFocusCandidate) {
				return true;
			}
			if (!IsWithin(rootElem, leftFocusCandidate)) {
				return true;
			}
		}
	}
	return false;
}


bool ShouldGoRight(Windows::System::VirtualKey key, DependencyObject^ rootElem) {
	if (key == Windows::System::VirtualKey::GamepadRightShoulder) {
		return true;
	}
	if (key == Windows::System::VirtualKey::GamepadDPadRight || key == Windows::System::VirtualKey::GamepadLeftThumbstickRight) {
		if (rootElem) {
			auto leftFocusCandidate = FocusManager::FindNextFocusableElement(FocusNavigationDirection::Right);
			if (!leftFocusCandidate) {
				return true;
			}
			if (!IsWithin(rootElem, leftFocusCandidate)) {
				return true;
			}
		}
	}
	return false;
}

void Tidal::XboxOneHub::OnKeyDown(_::KeyRoutedEventArgs ^ e)
{
	
	auto selectedContainer = SelectedIndex > -1 ? ContainerFromIndex(SelectedIndex) : nullptr;
	auto selectedRoot = selectedContainer != nullptr ? GetBodyPresenter(selectedContainer) : nullptr;
	if (ShouldGoLeft(e->OriginalKey, selectedRoot)) {
		if (SelectedIndex > 0) {
			e->Handled = true;
			SelectedIndex--;
		}
	}
	else if (ShouldGoRight(e->OriginalKey, selectedRoot)) {
		if (SelectedIndex < Items->Size-1) {
			e->Handled = true;
			SelectedIndex++;
		}

	}
}



Tidal::XboxOneHubSection::XboxOneHubSection()
{
	DefaultStyleKey = "Tidal.XboxOneHubSection";
	UseSystemFocusVisuals = false;
	IsTabStop = false;
	Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &Tidal::XboxOneHubSection::OnTapped);
}


_::DependencyProperty^ XboxOneHubSection_headerProperty = RegisterDependencyProperty<Object, XboxOneHubSection>(L"Header", ref new PropertyMetadata(nullptr));
_::DependencyProperty^ Tidal::XboxOneHubSection::HeaderProperty::get() {

	return XboxOneHubSection_headerProperty;
}
_::DependencyProperty^ XboxOneHubSection_HeaderTemplateProperty = RegisterDependencyProperty<DataTemplate, XboxOneHubSection>(L"HeaderTemplate", ref new PropertyMetadata(nullptr));
_::DependencyProperty^ Tidal::XboxOneHubSection::HeaderTemplateProperty::get() {
	return XboxOneHubSection_HeaderTemplateProperty;
}
_::DependencyProperty^ XboxOneHubSection_HasHeaderFocusProperty = RegisterDependencyProperty<bool, XboxOneHubSection>(L"HasHeaderFocus", false);
_::DependencyProperty^ Tidal::XboxOneHubSection::HasHeaderFocusProperty::get() {
	return XboxOneHubSection_HasHeaderFocusProperty;
}

void Tidal::XboxOneHub::OnSelectionChanged(Platform::Object ^sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs ^e)
{
	if (SelectedItem != nullptr && _body) {

		if (_currentSelectionAnimBatch) {
			_currentSelectionAnimBatch = nullptr;

		}
		auto batch = _compositor->CreateScopedBatch(CompositionBatchTypes::Animation);


		batch->Completed += ref new Windows::Foundation::TypedEventHandler<Platform::Object ^, Windows::UI::Composition::CompositionBatchCompletedEventArgs ^>(this, &Tidal::XboxOneHub::OnCompleted);

		auto sectionWidth = _body->ActualWidth;
		auto targetOffset = sectionWidth * SelectedIndex;
		auto anim = _compositor->CreateScalarKeyFrameAnimation();
		anim->InsertKeyFrame(1, targetOffset, _compositor->CreateCubicBezierEasingFunction(float2(0,0),float2(0.0f,1)));
		
		anim->Duration = tools::time::ToWindowsTimeSpan(std::chrono::milliseconds(500));

		CompositionPropertySet->StartAnimation(L"CurrentOffsetX", anim);
		batch->End();
		
	}
}


void Tidal::XboxOneHub::OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e)
{
	_body = dynamic_cast<_::Canvas^>(GetTemplateChild(L"PART_Body"));
	_bodyContainer = dynamic_cast<_::Control^>(GetTemplateChild(L"PART_BodyContainer"));
	_header = dynamic_cast<_::Control^>(GetTemplateChild(L"PART_Header"));
	if (_header) {
		_header->IsTabStop = true;
		FocusHelper::SetIsBindableFocusEnabled(_header, true);
	}
	if (SelectedIndex == -1 && Items->Size > 0) {
		SelectedIndex = 0;
	}
	ComputeTotalExtentAndMaterializeBodyPresenters();
}



void Tidal::XboxOneHub::OnSizeChanged(Platform::Object ^sender, Windows::UI::Xaml::SizeChangedEventArgs ^e)
{
	ComputeTotalExtentAndMaterializeBodyPresenters();
}


void Tidal::XboxOneHub::OnCompleted(Platform::Object ^sender, Windows::UI::Composition::CompositionBatchCompletedEventArgs ^args)
{
	auto container = ContainerFromIndex(SelectedIndex);
	auto presenter = GetBodyPresenter(container);
	if (!presenter) {
		return;
	}
	if (dynamic_cast<Control^>(FocusManager::GetFocusedElement()) == _header) {
		return;
	}
	auto focusable = FindFirstTabStopDescendant(presenter);
	if (focusable) {
		focusable->Focus(_::FocusState::Keyboard);
	}
}

DependencyProperty^ FocusHelper_IsBindableFocusEnabledProperty = RegisterAttachedProperty<bool, FocusHelper>(L"IsBindableFocusEnabled",
	ref new PropertyMetadata(false, ref new PropertyChangedCallback(
		[](_::Object^ sender, DependencyPropertyChangedEventArgs^ args) {
	auto ctl = dynamic_cast<Control^>(sender);
	if (!ctl) {
		return;
	}
	ctl->GotFocus += ref new RoutedEventHandler([](_::Object^ sender, RoutedEventArgs^ args) {
		FocusHelper::SetIsFocusWithin(static_cast<_::DependencyObject^>(sender), true);
	});

	ctl->LostFocus += ref new RoutedEventHandler([](_::Object^ sender, RoutedEventArgs^ args) {
		FocusHelper::SetIsFocusWithin(static_cast<_::DependencyObject^>(sender), false);
	});
})));
_::DependencyProperty^ FocusHelper::IsBindableFocusEnabledProperty::get() {
	return FocusHelper_IsBindableFocusEnabledProperty;
}

DependencyProperty^ FocusHelper_IsFocusWithinProperty = RegisterAttachedProperty<bool, FocusHelper>(L"IsFocusWithin", false);
_::DependencyProperty^ FocusHelper::IsFocusWithinProperty::get() {
	return FocusHelper_IsFocusWithinProperty;
}

Platform::Object ^ Tidal::BoolToVisibilityConverter::Convert(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	auto b = dynamic_cast<_::IBox<bool>^>(value);
	if (!b) {
		return Visibility::Collapsed;
	}
	return b->Value ? Visibility::Visible : Visibility::Collapsed;
	// TODO: insert return statement here
}

Platform::Object ^ Tidal::BoolToVisibilityConverter::ConvertBack(Platform::Object ^value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object ^parameter, Platform::String ^language)
{
	throw ref new Platform::NotImplementedException();
	// TODO: insert return statement here
}


void Tidal::XboxOneHubSection::OnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	auto hub = dynamic_cast<XboxOneHub^>( ItemsControl::ItemsControlFromItemContainer(this));
	if (hub) {
		hub->SelectedIndex = hub->IndexFromContainer(this);
	}
}
