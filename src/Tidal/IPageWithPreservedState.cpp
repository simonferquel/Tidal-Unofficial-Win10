#include "pch.h"
#include "IPageWithPreservedState.h"
#include "Shell.xaml.h"

Platform::Object^ Tidal::GetCurrentPagePreservedState() {
	auto shell = dynamic_cast<Shell^>(Windows::UI::Xaml::Window::Current->Content);
	if (!shell) {
		return nullptr;
	}
	return shell->CurrentPageState;
}