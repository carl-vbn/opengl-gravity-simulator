#include "rectangle.h"

namespace ui {
	Rectangle::Rectangle() {
		this->xMin = 0;
		this->yMin = 0;
		this->xMax = 1;
		this->yMax = 1;
	}

	Rectangle::Rectangle(float xMin, float yMin, float xMax, float yMax) {
		this->xMin = xMin;
		this->yMin = yMin;
		this->xMax = xMax;
		this->yMax = yMax;
	}

	Rectangle::Rectangle(float xMin, float yMin, float xMax, float yMax, Rectangle parent) {
		this->xMin = parent.xMin + parent.GetWidth() * xMin;
		this->yMin = parent.yMin + parent.GetHeight() * yMin;
		this->xMax = parent.xMin + parent.GetWidth() * xMax;
		this->yMax = parent.yMin + parent.GetHeight() * yMax;
	}

	float Rectangle::GetWidth() {
		return xMax - xMin;
	}

	float Rectangle::GetHeight() {
		return yMax - yMin;
	}

	bool Rectangle::Contains(float x, float y) {
		return x >= xMin && y >= yMin && x <= xMax && y <= yMax;
	}
}