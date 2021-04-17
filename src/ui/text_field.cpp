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
		this->labelVisible = label.size() > 0;
		this->longLabel = labelVisible && label.length() > 7;
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

		Rectangle outerRect = Rectangle((labelVisible ? (longLabel ? 2.3f : 1.7f) : 0.25f) / aspectRatio, 0.15f, 1.0f - 0.25f / aspectRatio, 0.85f, rect);
		Rectangle innerRect = Rectangle((labelVisible ? (longLabel ? 2.35f : 1.75f) : 0.3f) / aspectRatio, 0.20f, 1.0f - 0.30f / aspectRatio, 0.80f, rect);

		glBegin(GL_TRIANGLES);

		if (focused) glVertexAttrib3f(1, 1, 1, 1);
		else glVertexAttrib3f(1, 0.5, 0.5, 0.5);
		glVertex2f(outerRect.xMin, outerRect.yMax);
		glVertex2f(outerRect.xMin, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMin);
		glVertex2f(outerRect.xMax, outerRect.yMax);
		glVertex2f(outerRect.xMin, outerRect.yMax);
		glVertex2f(outerRect.xMax, outerRect.yMin);

		glVertexAttrib3f(1, 0, 0, 0);
		glVertex2f(innerRect.xMin, innerRect.yMax);
		glVertex2f(innerRect.xMin, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMin);
		glVertex2f(innerRect.xMax, innerRect.yMax);
		glVertex2f(innerRect.xMin, innerRect.yMax);
		glVertex2f(innerRect.xMax, innerRect.yMin);

		glEnd();

		if (labelVisible) {
			fontRendering::drawText(label, rect.xMin+rect.GetWidth()*0.075F, (innerRect.yMin + innerRect.yMax) / 2, rect.GetWidth() * 0.4f, COLOR_WHITE, false, true);
		}

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
		} else if (keyCode == GLFW_KEY_DELETE) {
			this->text = "";
		} else if (keyCode == GLFW_KEY_V && glfwGetKey(renderer::window, GLFW_KEY_LEFT_CONTROL)) {
			this->text = glfwGetClipboardString(renderer::window);
		}
	}

	// TODO This is definitely not the cleanest way to do this
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
		case TFF_HEX_NUMBER:
			if (isxdigit(pressedChar)) {
				this->text += tolower(pressedChar);
			}
		}
	}

	void TextFieldComponent::setText(std::string newText) {
		this->text = newText;
	}

	std::string TextFieldComponent::getText() {
		return this->text;
	}
}