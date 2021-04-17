#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../rendering/renderModel.h"
#include "mass_body.h"

#define MAX_OCCLUDERS 4

class Universe
{
private:
	glm::vec3 lightPosition;
	std::vector<MassBody*> bodies;

	void updateBodies(double deltaTime); // Update velocities and positions of all bodies in the universe
public:
	Universe();
	~Universe();

	struct RaycastHit {
		bool hit;
		MassBody* hitBody;
		unsigned int hitBodyIndex;
		glm::vec3 hitPosition;

		RaycastHit(bool hit, MassBody* hitBody, unsigned int hitBodyIndex, glm::vec3 hitPosition);
	};

	float timeScale;
	float gConstant;

	std::vector<MassBody*>* GetBodies();
	void AddBody(MassBody* body);
	RaycastHit Raycast(glm::vec3 startPos, glm::vec3 dir);
	
	void tick(double deltaTime);
};

