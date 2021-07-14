#include "universe.h"
#include <iostream>

Universe::Universe() {
	Universe::lightPosition = glm::vec3(4, 4, 4);
	Universe::timeScale = 1.0f;
	Universe::gConstant = 0.0001;
	Universe::emissiveBodyIndex = 0;
}

Universe::~Universe() {
	for (int i = 0; i < Universe::bodies.size(); i++) {
		delete bodies.at(i);
	}
}

std::vector<MassBody*>* Universe::GetBodies() {
	return &bodies;
}

int Universe::GetBodyIndex(MassBody* body) {
	int bodyIndex = -1;
	for (int i = 0; i < bodies.size(); i++) {
		if (bodies.at(i) == body) bodyIndex = i;
	}
	
	return bodyIndex;
}

void Universe::AssignOccluders() {
	for (int i = 0; i < bodies.size(); i++) {
		bodies.at(i)->occluders.clear();

		for (int j = 0; j < bodies.size(); j++) {
			if (i == j || j == emissiveBodyIndex) continue; // A body should never been its own occluder, nor should the light source been an occluder

			bodies.at(i)->occluders.push_back(bodies.at(j));
			if (bodies.at(i)->occluders.size() >= MAX_OCCLUDERS) break;
		}
	}
}

void Universe::AddBody(MassBody* body) {
	bodies.push_back(body);

	// Re-assign occluders
	AssignOccluders();
}

void Universe::DeleteBody(MassBody* body) {
	int bodyIndex = GetBodyIndex(body);

	if (bodyIndex >= 0) bodies.erase(bodies.begin() + bodyIndex);

	delete body;

	// Re-assign occluders
	AssignOccluders();
}

void Universe::SetEmissiveBody(unsigned int index) {
	emissiveBodyIndex = index;

	// Re-assign occluders
	AssignOccluders();
}

unsigned int Universe::GetEmissiveBodyIndex() {
	return emissiveBodyIndex;
}

MassBody* Universe::GetEmissiveBody() {
	return bodies.at(emissiveBodyIndex);
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
		MassBody* body = bodies.at(i);
		float t =  glm::dot(body->position-startPos, dir);
		glm::vec3 p = startPos + dir*t;

		float y = glm::length(body->position-p);
		float radius = body->radius;
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
	if (timeScale > 0 && deltaTime < 0.05F) updateBodies(deltaTime*timeScale); // If deltaTime is too high, don't update the bodies as the large deltaTime will distort orbits
}

void Universe::updateBodies(double deltaTime) {
	// Step 1: update all velocities
	for (unsigned int i = 0; i < bodies.size(); i++) {
		MassBody* body = bodies.at(i);

		if (body->affectedByGravity) {
			glm::vec3 totalGravitationalForce = glm::vec3(0);

			// Loop through all bodies in the universe to calculate their gravitational pull on the object (TODO: Ignore very far away objects for better performance)
			for (unsigned int j = 0; j < bodies.size(); j++) {
				if (i == j) continue; // No need to calculate the gravitational pull of a body on itself as it will always be 0.
				MassBody* otherBody = bodies.at(j);

				if (!otherBody->affectsOthers) continue; // Ignore this one

				float force = gConstant * body->mass * otherBody->mass / std::pow(glm::distance(body->position, otherBody->position), 2);
				glm::vec3 forceDirection = glm::normalize(otherBody->position - body->position);

				totalGravitationalForce += forceDirection * force;
			}

			// Update the velocity of the object by adding the acceleration, which is the force divided by the object's mass. (and multiply it all by deltaTime)
			bodies.at(i)->velocity += totalGravitationalForce / body->mass * (float)deltaTime;
		}
	}

	// Step 2: update all positions
	for (unsigned int i = 0; i < bodies.size(); i++) {
		MassBody* body = bodies.at(i);
		body->position += body->velocity * (float)deltaTime;
	}
}