#include "label.h"
#include "font_renderer.h"

namespace ui {
	LabelComponent::LabelComponent(std::string text, Color color) {
		this->text = text;
		this->color = color;
	}

	void LabelComponent::draw(Rectangle rect) {
		fontRendering::drawText(this->text, (rect.xMin + rect.xMax) / 2, (rect.yMin + rect.yMax) / 2, rect.GetWidth() * 0.4f, this->color, true, true);
	}

	void LabelComponent::onMouseDown(float mouseX, float mouseY, int button) {
	}
}