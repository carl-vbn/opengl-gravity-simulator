#pragma once

#include <string>

#include "component.h"

#define TFF_ALL_CHARS 0
#define TFF_DECIMAL_NUMBER 1
#define TFF_WHOLE_NUMBER 2

namespace ui {
	class TextFieldComponent : public Component
	{
	private:
		std::string label;
		std::string text;
		unsigned int inputFilter;

		bool focused;
	public:
		TextFieldComponent(std::string label, std::string text, unsigned short inputFilter=TFF_ALL_CHARS);

		void setFocused(bool focused);

		void draw(Rectangle rect) override;
		void onMouseDown(float mouseX, float mouseY, int button) override;

		void onMouseEnter() override {};
		void onMouseExit() override {};

		// Specific to this component
		void onKeyPressed(int keycode);
		void onCharKeyPressed(char pressedChar);
	};
}

