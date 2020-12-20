#pragma once

#include "../renderModel.h"

class Cube : public RenderModel
{
protected:
	glm::vec3* normals;
public: 
	Cube();
	~Cube();
};

