#include "mass_body.h"

MassBody::MassBody(glm::vec3 position, float mass, float radius, Color color) {
	this->position = position;
	this->mass = mass;
	this->radius = radius;
	this->color = color;
	this->velocity = glm::vec3(0);

	this->affectedByGravity = true;
	this->affectsOthers = true;
}