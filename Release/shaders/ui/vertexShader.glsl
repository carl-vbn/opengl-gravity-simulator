#version 330 core

// UI VERTEX SHADER

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 textureCoords;

// Output data ; will be interpolated for each fragment.
out vec4 interpolatedVertexColor;
out vec2 uv;

uniform bool UseTexture;

void main() {
	gl_Position = vec4(vertexPos, 1.0f);

	interpolatedVertexColor = vertexColor;
	uv = textureCoords;
}