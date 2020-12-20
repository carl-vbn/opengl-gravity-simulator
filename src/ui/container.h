#pragma once

#include <vector>
#include <string>
#include "component.h"
#include "../rendering/color.h"

namespace ui {
	class Container
	{
	public:
		Container(std::string label);

		std::string label;

		int GetVerticalCellSize();
		void AddComponent(Component* component);
		std::vector<Component*>* GetComponents();

		void onMouseDown(Rectangle containerBounds, float mouseX, float mouseY, int button);
		void onMouseMoved(Rectangle containerBounds, float mouseX, float mouseY);
		void draw(Rectangle rect);
	private:
		std::vector<Component*> components;
		int totalVerticalCellSize;
		int hoveredComponentIndex;
	};
}

