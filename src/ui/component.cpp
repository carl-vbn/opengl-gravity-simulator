#include <GL/glew.h>
#include "component.h"

namespace ui {
	Component::Component(int verticalCellSize) {
		this->verticalCellSize = verticalCellSize;
	}

	int Component::GetVerticalCellSize() {
		return verticalCellSize;
	}
}