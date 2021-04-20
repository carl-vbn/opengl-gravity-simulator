#include "scene_loader.h"
#include <iostream>

#define BODY_CHUNK_SIZE 35 // Size of a body in a scene file (in bytes)

Universe* loadScene(const char* filePath) {
	std::ifstream infile(filePath);

	if (!infile.good()) {
		std::cout << "[ERROR] File '" << filePath << "' does not exist." << std::endl;
		return nullptr;
	}

	//get length of file
	infile.seekg(0, std::ios::end);
	size_t length = infile.tellg();
	infile.seekg(0, std::ios::beg);

	char* buffer = new char[length];

	//read file
	infile.read(buffer, length);

	Universe* universe = new Universe();

	memcpy(&universe->gConstant, buffer, 4);
	memcpy(&universe->timeScale, buffer+4, 4);

	int bodyCount = (length - 8) / BODY_CHUNK_SIZE;
	for (int i = 0; i < bodyCount; i++) {
		const char * bodyOffset = buffer + 8 + i * BODY_CHUNK_SIZE;

		unsigned char colR, colG, colB;
		memcpy(&colR, bodyOffset + 0, 1);
		memcpy(&colG, bodyOffset + 1, 1);
		memcpy(&colB, bodyOffset + 2, 1);

		Color bodyColor(colR / 255.0f, colG / 255.0f, colB / 255.0f);

		MassBody* body = new MassBody(glm::vec3(0,0,0), 0, 1, bodyColor);
		memcpy(&body->mass, bodyOffset + 3, 4);
		memcpy(&body->radius, bodyOffset + 7, 4);
		memcpy(&body->position, bodyOffset + 11, 12);
		memcpy(&body->velocity, bodyOffset + 23, 12);
		universe->AddBody(body);
	}

	delete[] buffer;

	return universe;
}

void saveScene(Universe* universe, const char* filePath) {
	std::ofstream outfile(filePath);

	int bodyCount = universe->GetBodies()->size();
	int length = 8 + BODY_CHUNK_SIZE * bodyCount;
	char* buffer = new char[length];

	memcpy(buffer , &universe->gConstant, 4);
	memcpy(buffer  + 4, &universe->timeScale, 4);

	for (int i = 0; i < bodyCount; i++) {
		char* bodyOffset = buffer + 8 + i * BODY_CHUNK_SIZE;

		MassBody body = *universe->GetBodies()->at(i);

		unsigned char colR = body.color.red * 255;
		unsigned char colG = body.color.green * 255;
		unsigned char colB = body.color.blue * 255;
		memcpy(bodyOffset + 0, &colR, 1);
		memcpy(bodyOffset + 1, &colG, 1);
		memcpy(bodyOffset + 2, &colB, 1);

		memcpy(bodyOffset + 3, &body.mass, 4);
		memcpy(bodyOffset + 7, &body.radius, 4);
		memcpy(bodyOffset + 11, &body.position, 12);
		memcpy(bodyOffset + 23, &body.velocity, 12);
	}

	outfile.write(buffer, length);
	outfile.close();

	delete[] buffer;
}