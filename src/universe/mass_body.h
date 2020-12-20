#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "../rendering/color.h"

struct MassBody
{
	glm::vec3 position;
	glm::vec3 velocity;
	Color color;
	float mass;
	float size;

	MassBody(glm::vec3 position, float mass, float size, Color color);
};

