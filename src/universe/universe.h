#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../rendering/render_model.h"
#include "mass_body.h"

#define MAX_OCCLUDERS 4

class Universe
{
private:
	glm::vec3 lightPosition;
	std::vector<MassBody*> bodies;
	unsigned int emissiveBodyIndex;

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
	int GetBodyIndex(MassBody* body);
	void AssignOccluders();
	void AddBody(MassBody* body);
	void DeleteBody(MassBody* body);
	void SetEmissiveBody(unsigned int emittingBodyIndex);
	RaycastHit Raycast(glm::vec3 startPos, glm::vec3 dir);
	unsigned int GetEmissiveBodyIndex();
	MassBody* GetEmissiveBody();
	
	void tick(double deltaTime);
};

