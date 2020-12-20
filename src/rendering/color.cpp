#include "color.h"

Color::Color(float red, float green, float blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}

Color::Color() {
	red = 1.0f;
	green = 1.0f;
	blue = 1.0f;
}