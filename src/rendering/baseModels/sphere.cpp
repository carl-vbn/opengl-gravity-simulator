#include "sphere.h"

#include <iostream>

Sphere::Sphere(int resolution, float radius) {
    std::vector<glm::vec3> vertices;

    for (int i = 0; i < resolution / 2; i++) {
        for (int j = 0; j < resolution; j++) {
            float iRads = (float)i / resolution * glm::two_pi<float>();
            float jRads = (float)j / resolution * glm::two_pi<float>();

            float x = std::sin(jRads) * std::sin(iRads);
            float y = std::cos(iRads);
            float z = std::cos(jRads) * std::sin(iRads);

            vertices.push_back(glm::vec3(x, y, z) * radius);
        }
    }

    // Bottom vertex
    vertices.push_back(glm::vec3(0, -radius, 0));

    this->vertexCount = vertices.size();

    for (int i = 0; i < resolution * ((resolution - 2) / 2); i++) {
        indices.push_back(i);
        indices.push_back(i + resolution - 1);
        indices.push_back(i + resolution);

        indices.push_back(i + resolution);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    // Bottom faces
    for (int i = 0; i < resolution - 1; i++) {
        indices.push_back(vertexCount - 1);
        indices.push_back(vertexCount - 1 - resolution + i + 1);
        indices.push_back(vertexCount - 1 - resolution + i);
    }

    // Last triangle
    indices.push_back(vertexCount - resolution - 1);
    indices.push_back(vertexCount - 2);
    indices.push_back(vertexCount - 1);

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
        normals[i] = glm::normalize(vertices[i]);
    }

    init();
}

Sphere::~Sphere() {
    dispose();
    delete[] normals;
}
