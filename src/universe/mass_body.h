#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "../rendering/color.h"

struct MassBody
{
	glm::vec3 position;
	glm::vec3 velocity;
	Color color;
	float mass;
	float radius;

	bool affectedByGravity;
	bool affectsOthers;

	std::vector<MassBody*> occluders;

	MassBody(glm::vec3 position, float mass, float radius, Color color);
};

