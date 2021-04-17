#include "checkbox.h"
#include "ui_palette.h"
#include "../rendering/renderer.h"
#include "font_renderer.h"

#include <GL/glew.h>

namespace ui {
	const Color uncheckedColor = CHECKBOX_UNCHECKED_COLOR;
	const Color checkedColor = CHECKBOX_CHECKED_COLOR;
	const Color borderColor = CHECKBOX_BORDER_COLOR;

	CheckBoxComponent::CheckBoxComponent(std::string label, bool checked) {
		this->label = label;
		this->checked = checked;
	}

	void CheckBoxComponent::draw(Rectangle rect) {
		float aspectRatio = (float)renderer::windowWidth / renderer::windowHeight / rect.GetHeight() * rect.GetWidth();

		Rectangle outerRect = Rectangle(0.2f / aspectRatio, 0.2f, 0.8f / aspectRatio, 0.8f, rect);
		Rectangle innerRect = Rectangle(0.25f / aspectRatio, 0.25f, 0.75f / aspectRatio, 0.75f, rect);

		glBegin(GL_TRIANGLES);

		glVertexAttrib3f(1, borderColor.red, borderColor.green, borderColor.blue);
		glVertex2f(outerRect.xMin, outerRect.yMax);
		glVertex2f(outerRect.xMin, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMax);
		glVertex2f(outerRect.xMin, outerRect.yMax);
		glVertex2f(outerRect.xMax, outerRect.yMin);

		if (checked) glVertexAttrib3f(1, checkedColor.red, checkedColor.green, checkedColor.blue);
		else glVertexAttrib3f(1, uncheckedColor.red, uncheckedColor.green, uncheckedColor.blue);
		glVertex2f(innerRect.xMin, innerRect.yMax);
		glVertex2f(innerRect.xMin, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMax);
		glVertex2f(innerRect.xMin, innerRect.yMax);
		glVertex2f(innerRect.xMax, innerRect.yMin);

		glEnd();

		fontRendering::drawText(label, (rect.xMin + rect.xMax) / 2, (rect.yMin + rect.yMax) / 2, rect.GetWidth() * 0.4f, CHECKBOX_LABEL_COLOR, true, true);
	}

	void CheckBoxComponent::onMouseDown(float mouseX, float mouseY, int button) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			checked = !checked;

			if (this->toggleCallback != NULL) this->toggleCallback(checked);
		}
	}

	void CheckBoxComponent::setChecked(bool newChecked) {
		this->checked = newChecked;
	}

	bool CheckBoxComponent::isChecked() {
		return this->checked;
	}

	void CheckBoxComponent::setToggleCallback(std::function<void(bool)> callback) {
		this->toggleCallback = callback;
	}
}