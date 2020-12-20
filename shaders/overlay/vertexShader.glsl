#version 330 core

// FOCUS OVERLAY VERTEX SHADER
// WIP

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 uv;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // View-Projection matrix, but without the Model
uniform vec3 OverlayPos; // Position of the center of the overlay
uniform vec2 OverlaySize; // Size of the overlay

void main() {
	vec3 particleCenter_wordspace = OverlayPos;

	vec3 vertexPosition_worldspace =
		particleCenter_wordspace
		+ CameraRight_worldspace * vertexPosition_modelspace.x * OverlaySize.x
		+ CameraUp_worldspace * vertexPosition_modelspace.y * OverlaySize.y;


	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	uv = vertexPosition_modelspace.xy * 0.5 + vec2(0.5, 0.5);
}