#pragma once

#include <string>
#include "component.h"

namespace ui {
	class CheckBoxComponent : public Component
	{
	private:
		std::string label;
		bool checked;
	public:
		CheckBoxComponent(std::string label, bool checked = false);
		void draw(Rectangle rect) override;
		void onMouseDown(float mouseX, float mouseY, int button) override;

		void onMouseEnter() override {};
		void onMouseExit() override {};
	};
}
