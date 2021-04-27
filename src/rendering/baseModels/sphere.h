#pragma once
#include "../render_model.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Sphere : public RenderModel
{
protected:
	std::vector<GLfloat> vertexDataVector;
	std::vector<GLuint> indices;
	glm::vec3* normals;
public:
	Sphere(int resolution, float radius);
	~Sphere();
};

