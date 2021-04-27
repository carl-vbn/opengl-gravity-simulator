#pragma once
#include "../render_model.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class StarSphere : public RenderModel
{
protected:
	std::vector<GLfloat> vertexDataVector;
	std::vector<GLuint> indices;
	glm::vec3* normals;
public:

	StarSphere();
	~StarSphere();
};

