#pragma once

#include <vector>
#include <string>

#include "container.h"
#include "rectangle.h"
#include "../rendering/color.h"

namespace ui {
	class Panel
	{
	private:
		std::string label;
		Rectangle bounds;
		std::vector<Container> containers;
		int totalVerticalCellSize;
		
	public:
		Panel(std::string label, Rectangle bounds);

		Rectangle GetBounds();
		void AddContainer(Container container);
		Container* GetContainerAt(float x, float y, Rectangle* out_containerBounds);
		std::vector<Container>* GetContainers();

		std::string GetLabel();
		void SetLabel(std::string newLabel);

		void draw();
		void onMouseDown(float mouseX, float mouseY, int button);
		void onMouseMoved(float mouseX, float mouseY);
	};
}