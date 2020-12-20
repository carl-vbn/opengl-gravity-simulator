#pragma once

namespace ui {
	struct Rectangle
	{
		float xMin, yMin, xMax, yMax;

		Rectangle();
		Rectangle(float xMin, float yMin, float xMax, float yMax);
		Rectangle(float xMin, float yMin, float xMax, float yMax, Rectangle parent);

		float GetWidth();
		float GetHeight();
		bool Contains(float x, float y);
	};
}

