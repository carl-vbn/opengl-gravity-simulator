#pragma once

#include <string>

#include "component.h"

#define TFF_ALL_CHARS 0
#define TFF_DECIMAL_NUMBER 1
#define TFF_WHOLE_NUMBER 2
#define TFF_HEX_NUMBER 3

namespace ui {
	class TextFieldComponent : public Component
	{
	private:
		std::string label;
		std::string text;
		unsigned int inputFilter;

		bool labelVisible;
		bool focused;
		bool longLabel;
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

		void setText(std::string newText);
		std::string getText();
	};
}

