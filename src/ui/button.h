#pragma once

#include <string>
#include <functional>
#include "component.h"

namespace ui {
	class ButtonComponent : public Component
	{
	private:
		std::string label;
		std::function<void()> onClickCallback;
		bool hovered;
	public:
		ButtonComponent(std::string label, std::function<void()> onClickCallback);

		std::string GetLabel();
		void SetLabel(std::string label);

		void draw(Rectangle rect) override;
		void onMouseDown(float mouseX, float mouseY, int button) override;
		void onMouseEnter() override;
		void onMouseExit() override;
	};
}