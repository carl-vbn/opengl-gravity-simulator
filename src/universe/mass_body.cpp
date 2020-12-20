#include "mass_body.h"

MassBody::MassBody(glm::vec3 position, float mass, float size, Color color) {
	this->position = position;
	this->mass = mass;
	this->size = size;
	this->color = color;
	this->velocity = glm::vec3(0);
}