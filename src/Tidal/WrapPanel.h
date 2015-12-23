#pragma once
namespace Tidal {
	public ref class WrapPanel sealed : public Windows::UI::Xaml::Controls::Panel {
	protected:
		virtual Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size availableSize) override;
		virtual Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) override;
	public:
		WrapPanel() {}
	};
}