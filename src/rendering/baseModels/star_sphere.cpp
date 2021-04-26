#include "star_sphere.h"

#include <iostream>

StarSphere::StarSphere() {
    std::vector<glm::vec3> vertices;

    int resolution = 4;

    for (int i = 0; i < resolution / 2; i++) {
        for (int j = 0; j < resolution; j++) {
            float iRads = (float)i / resolution * glm::two_pi<float>();
            float jRads = (float)j / resolution * glm::two_pi<float>();

            float x = std::sin(jRads) * std::sin(iRads);
            float y = std::cos(iRads);
            float z = std::cos(jRads) * std::sin(iRads);

            vertices.push_back(glm::vec3(x, y, z));
        }
    }

    // Bottom vertex
    vertices.push_back(glm::vec3(0, -1, 0));

    this->vertexCount = vertices.size();

    for (int i = 0; i < resolution * ((resolution - 2) / 2); i++) {
        indices.push_back(i + resolution);
        indices.push_back(i + resolution - 1);
        indices.push_back(i);

        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + resolution);
    }

    // Bottom faces
    for (int i = 0; i < resolution - 1; i++) {
        indices.push_back(vertexCount - 1 - resolution + i);
        indices.push_back(vertexCount - 1 - resolution + i + 1);
        indices.push_back(vertexCount - 1);
    }

    // Last triangle
    indices.push_back(vertexCount - 1);
    indices.push_back(vertexCount - 2);
    indices.push_back(vertexCount - resolution - 1);

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 vertex = vertices[i];
        vertexDataVector.push_back(vertex.x);
        vertexDataVector.push_back(vertex.y);
        vertexDataVector.push_back(vertex.z);
    }

    this->normals = new glm::vec3[vertexCount];

    this->indexCount = indices.size();
    this->vertexData = &vertexDataVector[0];
    this->indexData = &indices[0];
    this->normalData = normals;

    for (int i = 0; i < vertexCount; i++) {
		glm::vec3 normalizedVertexPos = glm::normalize(vertices[i]);
        normals[i] = normalizedVertexPos * glm::vec3(-1, -1, -1); // Pointing inwards

		// Texture mapping
		float u = 0.5 + std::atan2f(normalizedVertexPos.x, normalizedVertexPos.z) / glm::two_pi<float>();
		float v = 0.5 - std::asinf(normalizedVertexPos.y) / glm::pi<float>();
    }

    init();
}

StarSphere::~StarSphere() {
    dispose();
    delete[] normals;
}