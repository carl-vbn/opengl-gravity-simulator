#pragma once

#include "../render_model.h"

class Cube : public RenderModel
{
protected:
	glm::vec3* normals;
public: 
	Cube();
	~Cube();
};

