#pragma once

#include <string>
#include <functional>
#include "component.h"

namespace ui {
	class CheckBoxComponent : public Component
	{
	private:
		std::string label;
		bool checked;
		std::function<void(bool)> toggleCallback;
	public:
		CheckBoxComponent(std::string label, bool checked = false);
		void draw(Rectangle rect) override;
		void onMouseDown(float mouseX, float mouseY, int button) override;

		void onMouseEnter() override {};
		void onMouseExit() override {};

		void setChecked(bool newChecked);
		bool isChecked();
		void setToggleCallback(std::function<void(bool)> callback);
	};
}
