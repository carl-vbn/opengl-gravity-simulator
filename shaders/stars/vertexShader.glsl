#version 330 core
#define PI 3.1415926538

// STAR SPHERE VERTEX SHADER

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexTextureCoordinate;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec2 uv;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;

void main() {

	// Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

	// Normal of the the vertex, in camera space
	Normal_cameraspace = (V * vec4(vertexNormal_modelspace, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	vec3 d = normalize(vertexPosition_modelspace);
	uv = vec2(0.5 + atan(d.x, d.z)/(2*PI), 0.5 - asin(d.y)/PI);
}