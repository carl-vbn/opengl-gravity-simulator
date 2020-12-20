#include "universe.h"
#include "constants.h"
#include <iostream>

Universe::Universe() {
	std::cout << "Universe construction" << std::endl;

	Universe::lightPosition = glm::vec3(4, 4, 4);
	Universe::timeScale = 1.0f;
}

std::vector<MassBody>* Universe::GetBodies() {
	return &bodies;
}

void Universe::AddBody(MassBody body) {
	bodies.push_back(body);
}

Universe::RaycastHit::RaycastHit(bool hit, MassBody* hitBody, unsigned int hitBodyIndex, glm::vec3 hitPosition) {
	this->hit = hit;
	this->hitBody = hitBody;
	this->hitBodyIndex = hitBodyIndex;
	this->hitPosition = hitPosition;
}

Universe::RaycastHit Universe::Raycast(glm::vec3 startPos, glm::vec3 dir) {
	RaycastHit closestHit(false, nullptr, 0, glm::vec3());

	for (unsigned int i = 0; i < bodies.size(); i++) {
		MassBody* body = &bodies.at(i);
		float t =  glm::dot(body->position-startPos, dir);
		glm::vec3 p = startPos + dir*t;

		float y = glm::length(body->position-p);
		float radius = body->size;
		if (y < radius) {
			float x = std::sqrtf(radius * radius - y * y);
			float t1 = t - x;
			if (t1 > 0) {
				glm::vec3 hitPos = startPos + dir * t1;
				if (!closestHit.hit || glm::distance(closestHit.hitPosition, startPos) > glm::distance(hitPos, startPos)) {
					closestHit = RaycastHit(true, body, i, hitPos);
				}
			}
		}
	}

	return closestHit;
}

void Universe::tick(double deltaTime) {
	if (timeScale > 0) updateBodies(deltaTime*timeScale);
}

void Universe::updateBodies(double deltaTime) {
	// Step 1: update all velocities
	for (unsigned int i = 0; i < bodies.size(); i++) {
		MassBody body = bodies.at(i);

		glm::vec3 totalGravitationalForce = glm::vec3(0);
		// Loop through all bodies in the universe to calculate their gravitational pull on the object (TODO: Ignore very far away objects for better performance)
		for (unsigned int j = 0; j < bodies.size(); j++) {
			if (i == j) continue; // No need to calculate the gravitational pull of a body on itself as it will always be 0.
			MassBody otherBody = bodies.at(j);
			float force = GRAVITATIONAL_CONSTANT * body.mass * otherBody.mass * std::pow(glm::distance(body.position, otherBody.position), 2);
			glm::vec3 forceDirection = glm::normalize(otherBody.position - body.position);
			
			totalGravitationalForce += forceDirection*force;
		}

		// Update the velocity of the object by adding the acceleration, which is the force divided by the object's mass. (and multiply it all by deltaTime)
		(&bodies.at(i))->velocity += totalGravitationalForce / body.mass * (float)deltaTime;
	}

	// Step 2: update all positions
	for (unsigned int i = 0; i < bodies.size(); i++) {
		MassBody* body = &bodies.at(i);
		body->position += body->velocity * (float)deltaTime;
	}
}