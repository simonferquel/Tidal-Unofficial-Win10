#include "pch.h"
#include "WrapPanel.h"
#include <limits>

Windows::Foundation::Size Tidal::WrapPanel::MeasureOverride(Windows::Foundation::Size availableSize)
{
	float currentWidth = 0;
	float currentHeight = 0;
	float previousRowsWidth = 0;
	float previousRowsHeight = 0;
	float maxRowWidth = availableSize.Width;
	if (maxRowWidth == std::numeric_limits<float>::infinity()) {
		maxRowWidth = std::numeric_limits<float>::max();
	}
	for (auto&& child : Children) {
		child->Measure(Windows::Foundation::Size(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
		auto childSize = child->DesiredSize;
		if (childSize.Width + currentWidth > maxRowWidth) {
			// commit row
			if (currentWidth > previousRowsWidth) {
				previousRowsWidth = currentWidth;
			}
			previousRowsHeight += currentHeight;

			currentWidth = 0;
			currentHeight = 0;
			currentWidth = childSize.Width;
			currentHeight = childSize.Height;
		}
		else {
			currentWidth += childSize.Width;
			if (childSize.Height > currentHeight) {
				currentHeight = childSize.Height;
			}
		}
	}
	if (currentWidth > previousRowsWidth)
	{
		previousRowsWidth = currentWidth;
	}
	return Windows::Foundation::Size(previousRowsWidth, previousRowsHeight + currentHeight);
}

Windows::Foundation::Size Tidal::WrapPanel::ArrangeOverride(Windows::Foundation::Size finalSize)
{
	float currentWidth = 0;
	float currentHeight = 0;
	float previousRowsWidth = 0;
	float previousRowsHeight = 0;

	float maxRowWidth = finalSize.Width;
	for (auto&& child : Children) {
		
		auto childSize = child->DesiredSize;
		if (childSize.Width + currentWidth > maxRowWidth) {
			// commit row
			if (currentWidth > previousRowsWidth) {
				previousRowsWidth = currentWidth;
			}
			previousRowsHeight += currentHeight;

			currentWidth = 0;
			currentHeight = 0;
			Windows::Foundation::Rect arrangeRect(currentWidth, previousRowsHeight, childSize.Width, childSize.Height);
			child->Arrange(arrangeRect);
			currentWidth = childSize.Width;
			currentHeight = childSize.Height;
		}
		else {
			Windows::Foundation::Rect arrangeRect(currentWidth, previousRowsHeight, childSize.Width, childSize.Height);
			child->Arrange(arrangeRect);
			currentWidth += childSize.Width;
			if (childSize.Height > currentHeight) {
				currentHeight = childSize.Height;
			}
		}
	}
	if (currentWidth > previousRowsWidth)
	{
		previousRowsWidth = currentWidth;
	}
	return Windows::Foundation::Size(previousRowsWidth, previousRowsHeight + currentHeight);
}
