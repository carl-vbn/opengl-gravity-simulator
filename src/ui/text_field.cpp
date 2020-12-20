#include "text_field.h"
#include "../rendering/renderer.h"
#include "font_renderer.h"
#include <ctype.h>

namespace ui {
	TextFieldComponent::TextFieldComponent(std::string label, std::string text, unsigned short inputFilter) {
		this->label = label;
		this->text = text;
		this->inputFilter = inputFilter;
		this->focused = false;
	}

	void TextFieldComponent::setFocused(bool focused) {
		this->focused = focused;

		if (focused) {
			renderer::focusedTextField = this;
		}
		else if (renderer::focusedTextField == this) {
			renderer::focusedTextField = nullptr;
		}
	}

	void TextFieldComponent::draw(Rectangle rect) {
		float aspectRatio = (float)renderer::windowWidth / renderer::windowHeight / rect.GetHeight() * rect.GetWidth();

		Rectangle outerRect = Rectangle(0.25f / aspectRatio, 0.15f, 1.0f - 0.25f / aspectRatio, 0.85f, rect);
		Rectangle innerRect = Rectangle(0.30f / aspectRatio, 0.20f, 1.0f - 0.30f / aspectRatio, 0.80f, rect);

		glBegin(GL_QUADS);

		if (focused) glVertexAttrib3f(1, 1, 1, 1);
		else glVertexAttrib3f(1, 0.5, 0.5, 0.5);
		glVertex2f(outerRect.xMin, outerRect.yMax);
		glVertex2f(outerRect.xMin, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMax);

		glVertexAttrib3f(1, 0, 0, 0);
		glVertex2f(innerRect.xMin, innerRect.yMax);
		glVertex2f(innerRect.xMin, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMax);

		glEnd();

		fontRendering::drawText(text, (innerRect.xMin + innerRect.xMax) / 2, (innerRect.yMin + innerRect.yMax) / 2, rect.GetWidth() * 0.4f, COLOR_WHITE,  true, true);
	}

	void TextFieldComponent::onMouseDown(float mouseX, float mouseY, int button) {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			setFocused(true);
	}

	void TextFieldComponent::onKeyPressed(int keyCode) {
		if (keyCode == GLFW_KEY_BACKSPACE) {
			int txtLen = this->text.length();
			if (this->text.length() > 0) this->text = this->text.substr(0, txtLen - 1);
		}
	}

	void TextFieldComponent::onCharKeyPressed(char pressedChar) {
		switch (inputFilter) {
		case TFF_ALL_CHARS:
			this->text += pressedChar; 
			break;
		case TFF_DECIMAL_NUMBER:
			if (isdigit(pressedChar) || (pressedChar == '.' && text.find('.') == std::string::npos)) {
				this->text += pressedChar;
			}
			break;
		case TFF_WHOLE_NUMBER:
			if (isdigit(pressedChar)) {
				this->text += pressedChar;
			}
			break;
		}
	}
}