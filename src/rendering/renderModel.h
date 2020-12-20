#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class RenderModel
{
protected:
	int vertexCount;
	int indexCount;
	const GLfloat* vertexData;
	const GLuint* indexData;
	glm::vec3* normalData;

	void calculateNormals();

public:
	const int GetVertexCount();
	const int GetIndexCount();
	const int GetVertexDataSize();
	const int GetIndexDataSize();
	const int GetNormalDataSize();
	const GLfloat* GetVertexData();
	const GLuint* GetIndexData();
	glm::vec3* GetNormalData();

	GLuint VertexArrayID;
	GLuint VertexBufferID;
	GLuint IndexBufferID;
	GLuint NormalBufferID;

	void init();
	void dispose();
};

