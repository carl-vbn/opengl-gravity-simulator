#pragma once

#include <string>
#include "component.h"

namespace ui {
	class LabelComponent : public Component
	{
	private:
		std::string text;
		Color color;
	public:
		LabelComponent(std::string text, Color color=COLOR_WHITE);

		void draw(Rectangle rect) override;
		void onMouseDown(float mouseX, float mouseY, int button);

		void onMouseEnter() override {};
		void onMouseExit() override {};
	};
}