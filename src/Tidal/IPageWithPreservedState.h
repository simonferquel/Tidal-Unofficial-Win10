#pragma once
namespace Tidal {
	public interface class IPageWithPreservedState
	{
		Platform::Object^ GetStateToPreserve();
	};
}

