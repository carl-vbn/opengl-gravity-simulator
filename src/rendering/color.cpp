#include "color.h"

Color::Color(float red, float green, float blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}

Color::Color(unsigned int hex) {
	int red, green, blue;

	red = hex >> 16;

	green = (hex & 0x00ff00) >> 8;

	blue = (hex & 0x0000ff);

	this->red = red / 255.0f;
	this->green = green / 255.0f;
	this->blue = blue / 255.0f;
}

Color::Color() {
	red = 1.0f;
	green = 1.0f;
	blue = 1.0f;
}

int Color::toHex() {
	return ((int)(red * 255) << 16 | (int)(green * 255) << 8 | (int)(blue * 255));
}