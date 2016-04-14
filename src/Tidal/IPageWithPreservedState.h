#pragma once
namespace Tidal {
	public interface class IPageWithPreservedState
	{
		Platform::Object^ GetStateToPreserve();
	};

	Platform::Object^ GetCurrentPagePreservedState();

	template<typename T> 
	T^ GetCurrentPagePreservedState() {
		return dynamic_cast<T^>(GetCurrentPagePreservedState());
	}
}

