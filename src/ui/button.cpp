#include "button.h"
#include "font_renderer.h"
#include "ui_palette.h"
#include "../rendering/renderer.h"
#include <GL/glew.h>

namespace ui {
	const Color btnLabelColor = BUTTON_LABEL_COLOR;
	const Color btnHoveredColor = BUTTON_HOVER_COLOR;
	const Color btnBackgroundColor = BUTTON_COLOR;

	ButtonComponent::ButtonComponent(std::string label, std::function<void()> onClickCallback) {
		this->label = label;
		this->onClickCallback = onClickCallback;
		this->hovered = false;
	}

	std::string ButtonComponent::GetLabel() {
		return label;
	}

	void ButtonComponent::SetLabel(std::string label) {
		this->label = label;
	}

	void ButtonComponent::draw(Rectangle rect) {
		float aspectRatio = (float)renderer::windowWidth / renderer::windowHeight / rect.GetHeight() * rect.GetWidth();

		// Create margin and account for aspect ratio
		rect = Rectangle(0.15f / aspectRatio, 0.15f, 1.0f - 0.15f / aspectRatio, 0.85f, rect);

		glBegin(GL_TRIANGLES);
		if (hovered) glVertexAttrib3f(1, btnHoveredColor.red, btnHoveredColor.green, btnHoveredColor.blue);
		else glVertexAttrib3f(1, btnBackgroundColor.red, btnBackgroundColor.green, btnBackgroundColor.blue);
		glVertex2f(rect.xMin, rect.yMax);
		glVertex2f(rect.xMin, rect.yMin);
		glVertex2f(rect.xMax, rect.yMin);

		glVertex2f(rect.xMax, rect.yMax);
		glVertex2f(rect.xMin, rect.yMax);
		glVertex2f(rect.xMax, rect.yMin);
		glEnd();

		fontRendering::drawText(label, (rect.xMin + rect.xMax) / 2, (rect.yMin + rect.yMax) / 2, rect.GetWidth() * 0.35f, btnLabelColor, true, true);
	}

	void ButtonComponent::onMouseDown(float mouseX, float mouseY, int button) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) onClickCallback();
	}

	void ButtonComponent::onMouseEnter() {
		hovered = true;
	}

	void ButtonComponent::onMouseExit() {
		hovered = false;
	}
}