#include "render_model.h"
#include <iostream>

const int RenderModel::GetVertexCount() {
	return vertexCount;
}

const int RenderModel::GetIndexCount() {
	return indexCount;
}

const GLfloat* RenderModel::GetVertexData() {
	return vertexData;
}

const GLuint* RenderModel::GetIndexData() {
	return indexData;
}

glm::vec3* RenderModel::GetNormalData() {
	return normalData;
}

void RenderModel::init() {
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &VertexBufferID);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, GetVertexDataSize(), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexDataSize(), indexData, GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
	glBufferData(GL_ARRAY_BUFFER, GetNormalDataSize(), normalData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Now fill the buffer with some data
	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 3rd attribute buffer : normals (2nd attribute is used for textures, but we don't use textures here)
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBufferID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
}

void RenderModel::calculateNormals() {
	for (int triangleOffset = 0; triangleOffset < vertexCount*3; triangleOffset += 9) {
		glm::vec3 a = glm::vec3(
			vertexData[triangleOffset + 3] - vertexData[triangleOffset],
			vertexData[triangleOffset + 4] - vertexData[triangleOffset + 1],
			vertexData[triangleOffset + 5] - vertexData[triangleOffset + 2]
		);

		glm::vec3 b = glm::vec3(
			vertexData[triangleOffset + 6] - vertexData[triangleOffset],
			vertexData[triangleOffset + 7] - vertexData[triangleOffset + 1],
			vertexData[triangleOffset + 8] - vertexData[triangleOffset + 2]
		);

		glm::vec3 triangleNormal = glm::vec3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);

		normalData[triangleOffset / 3 + 0] = triangleNormal;
		normalData[triangleOffset / 3 + 1] = triangleNormal;
		normalData[triangleOffset / 3 + 2] = triangleNormal;
	}
}

void RenderModel::dispose() {
	glDeleteBuffers(1, &VertexBufferID);
	glDeleteBuffers(1, &IndexBufferID);
	glDeleteBuffers(1, &NormalBufferID);
	glDeleteVertexArrays(1, &VertexArrayID);
}

const int RenderModel::GetVertexDataSize() {
	return vertexCount * 3 * sizeof(vertexData[0]);
}

const int RenderModel::GetIndexDataSize() {
	return indexCount * sizeof(indexData[0]);
}

const int RenderModel::GetNormalDataSize() {
	return vertexCount * sizeof(normalData[0]);
}