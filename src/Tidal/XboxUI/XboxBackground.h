//
// XboxBackground.h
// Declaration of the XboxBackground class.
//

#pragma once

#include <memory>
#include <ppltasks.h>
namespace Tidal
{
	class BackgroundTiles;
	public ref class XboxBackground sealed : public Windows::UI::Xaml::Controls::Control
	{
	private:
		concurrency::cancellation_token_source _cts;
		std::shared_ptr<BackgroundTiles> _tiles;
	public:
		XboxBackground();
		virtual ~XboxBackground();
		void OnLoaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void OnUnloaded(Platform::Object ^sender, Windows::UI::Xaml::RoutedEventArgs ^e);
		void SetParallaxAmountBinding(Windows::UI::Composition::ExpressionAnimation^ anim);
	};
}
