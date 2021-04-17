#include "container.h"
#include "GL/glew.h"

#include <iostream>

namespace ui {
	Container::Container(std::string label) {
		this->label = label;
		this->totalVerticalCellSize = 0;
		this->hoveredComponentIndex = -1;
	}

	int Container::GetVerticalCellSize() {
		return totalVerticalCellSize;
	}

	void Container::draw(Rectangle rect) {
		float cellHeight = rect.GetHeight() / totalVerticalCellSize;

		float yOffset = rect.yMin;
		for (int i = components.size() - 1; i >= 0; i--) {
			Component* comp = components.at(i);
			float componentHeight = cellHeight * comp->GetVerticalCellSize();
			comp->draw(Rectangle(rect.xMin, yOffset, rect.xMax, yOffset+componentHeight));

			yOffset += componentHeight;
		}
	}

	void Container::AddComponent(Component* component) {
		components.push_back(component);
		totalVerticalCellSize += component->GetVerticalCellSize();
	}

	std::vector<Component*>* Container::GetComponents() {
		return &components;
	}

	void Container::onMouseDown(Rectangle containerBounds, float mouseX, float mouseY, int button) {
		float cellHeight = containerBounds.GetHeight() / totalVerticalCellSize;

		float yOffset = containerBounds.yMin;
		for (int i = components.size() - 1; i >= 0; i--) {
			Component* comp = components.at(i);
			float componentHeight = cellHeight * comp->GetVerticalCellSize();
			Rectangle componentBounds = Rectangle(containerBounds.xMin, yOffset, containerBounds.xMax, yOffset + componentHeight);
			if (componentBounds.Contains(mouseX, mouseY)) {
				comp->onMouseDown(mouseX, mouseY, button);
			}

			yOffset += componentHeight;
		}
	}

	void Container::onMouseMoved(Rectangle containerBounds, float mouseX, float mouseY) {
		float cellHeight = containerBounds.GetHeight() / totalVerticalCellSize;

		float yOffset = containerBounds.yMin;
		for (int i = components.size() - 1; i >= 0; i--) {
			Component* comp = components.at(i);
			float componentHeight = cellHeight * comp->GetVerticalCellSize();
			Rectangle componentBounds = Rectangle(containerBounds.xMin, yOffset, containerBounds.xMax, yOffset + componentHeight);
			bool containsMouse = componentBounds.Contains(mouseX, mouseY);
			if (containsMouse && hoveredComponentIndex != i) {
				if (hoveredComponentIndex >= 0) {
					components.at(hoveredComponentIndex)->onMouseExit();
				}

				comp->onMouseEnter();
				hoveredComponentIndex = i;
			}
			else if (!containsMouse && hoveredComponentIndex == i) {
				comp->onMouseExit();
				hoveredComponentIndex = -i;
			}

			yOffset += componentHeight;
		}
	}
}