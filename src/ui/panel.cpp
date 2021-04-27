#include "panel.h"
#include "GL/glew.h"
#include "ui_palette.h"
#include "font_renderer.h"

#define CONTAINER_SPACING 0.01

namespace ui {
	const Color headColor = PANEL_HEAD_COLOR;
	const Color backgroundColor = PANEL_BACKGROUND_COLOR;

	Panel::Panel(std::string label, Rectangle bounds) {
		this->label = label;
		this->bounds = bounds;
		this->totalVerticalCellSize = 0;
	}

	void Panel::AddContainer(Container container) {
		containers.push_back(container);
		totalVerticalCellSize += container.GetVerticalCellSize();
	}

	Container* Panel::GetContainerAt(float x, float y, Rectangle* out_containerBounds) {
		float cellHeight = bounds.GetHeight() / (totalVerticalCellSize + 1); // We add 1 to make up for the panel head
		float yOffset = bounds.yMax - cellHeight;

		for (unsigned int i = 0; i < containers.size(); i++) {
			Container* container = &containers.at(i);
			Rectangle containerBounds = Rectangle(bounds.xMin, yOffset - container->GetVerticalCellSize() * cellHeight, bounds.xMax, yOffset);
			if (containerBounds.Contains(x, y)) {
				*out_containerBounds = containerBounds;
				return container;
			}

			yOffset -= containerBounds.GetHeight();
		}

		return nullptr;
	}

	void Panel::draw() {
		float cellHeight = bounds.GetHeight() / (totalVerticalCellSize + 1); // We add 1 to make up for the panel head

		glBegin(GL_TRIANGLES);
		// Background
		glVertexAttrib3f(1, backgroundColor.red, backgroundColor.green, backgroundColor.blue);
		glVertex2f(bounds.xMin, bounds.yMin);
		glVertex2f(bounds.xMax, bounds.yMin);
		glVertex2f(bounds.xMax, bounds.yMax);
		glVertex2f(bounds.xMin, bounds.yMax);
		glVertex2f(bounds.xMin, bounds.yMin);
		glVertex2f(bounds.xMax, bounds.yMax);

		// Head
		glVertexAttrib3f(1, headColor.red, headColor.green, headColor.blue);
		glVertex2f(bounds.xMin, bounds.yMax);
		glVertex2f(bounds.xMin, bounds.yMax - cellHeight);
		glVertex2f(bounds.xMax, bounds.yMax - cellHeight);
		glVertex2f(bounds.xMax, bounds.yMax);
		glVertex2f(bounds.xMin, bounds.yMax);
		glVertex2f(bounds.xMax, bounds.yMax - cellHeight);
		glEnd();

		fontRendering::drawText(label, (bounds.xMin + bounds.xMax) / 2, (2 * bounds.yMax - cellHeight) / 2, bounds.GetWidth() * 0.5F, PANEL_TITLE_COLOR, true, true);

		float yOffset = bounds.yMax - cellHeight;
		for (unsigned int i = 0; i < containers.size(); i++) {
			Container* container = &containers.at(i);
			container->draw(Rectangle(bounds.xMin, yOffset - container->GetVerticalCellSize() * cellHeight, bounds.xMax, yOffset));
			yOffset -= container->GetVerticalCellSize() * cellHeight;
		}
	}

	void Panel::onMouseDown(float mouseX, float mouseY, int button) {
		Rectangle containerBounds;
		Container* container = GetContainerAt(mouseX, mouseY, &containerBounds);
		if (container != nullptr) {
			container->onMouseDown(containerBounds, mouseX, mouseY, button);
		}
	}

	void Panel::onMouseMoved(float mouseX, float mouseY) {
		float cellHeight = bounds.GetHeight() / (totalVerticalCellSize + 1); // We add 1 to make up for the panel head
		float yOffset = bounds.yMax - cellHeight;

		for (unsigned int i = 0; i < containers.size(); i++) {
			Container* container = &containers.at(i);
			Rectangle containerBounds = Rectangle(bounds.xMin, yOffset - container->GetVerticalCellSize() * cellHeight, bounds.xMax, yOffset);
			container->onMouseMoved(containerBounds, mouseX, mouseY);
			
			yOffset -= containerBounds.GetHeight();
		}
	}

	Rectangle Panel::GetBounds() {
		return bounds;
	}

	std::vector<Container>* Panel::GetContainers() {
		return &containers;
	}

	std::string Panel::GetLabel() {
		return label;
	}
	void Panel::SetLabel(std::string newLabel) {
		this->label = newLabel;
	}
}