#pragma once
#include "rectangle.h"
#include "../rendering/color.h"

namespace ui {
	class Component
	{
	private:
		int verticalCellSize; // Used to determine how much of the available vertical space the component will take up. 
	public:
		Component(int verticalCellSize=1);
		virtual ~Component() = default;

		int GetVerticalCellSize();

		virtual void draw(Rectangle rect)=0;

		virtual void onMouseDown(float mouseX, float mouseY, int button) = 0;
		virtual void onMouseEnter() = 0;
		virtual void onMouseExit() = 0;

	};
}

