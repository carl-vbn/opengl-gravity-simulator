#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

#include "../universe/universe.h"
#include "../universe/mass_body.h"
#include "baseModels/sphere.h"
#include "baseModels/star_sphere.h"
#include "../ui/ui_manager.h"
#include "../ui/panel.h"
#include "../ui/text_field.h"
#include "../ui/font_renderer.h"

namespace renderer {
	// Default shader (used to render objects in the world with or without lighting)
	struct Shader {
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
		GLuint UnlitUniformID;
		GLuint EmissiveUniformID;
		GLuint LightPosUniformID;
		GLuint LightColorUniformID;
		GLuint ModelColorUniformID;

		// Shadow marching Uniforms
		GLuint OccluderCountUniformID;
		GLuint OccluderPositionsUniformIDs[MAX_OCCLUDERS];
		GLuint OccluderRadiusesUniformIDs[MAX_OCCLUDERS];
		GLuint LightRadiusUniformID;
	};

	// Shader used to render star spheres
	struct StarShader {
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
	};

	// Shader used to render overlays (currently just the focused-body indicator)
	struct OverlayShader {
		GLuint ProgramID;
		GLuint CamRightUniformID;
		GLuint CamUpUniformID;
		GLuint ViewProjectionMatrixUniformID;
		GLuint OverlayPositionUniformID;
		GLuint OverlaySizeUniformID;
		GLuint TimeUniformID;
	};

	// Shader used to render User Interface
	struct UIShader {
		GLuint ProgramID;
		GLuint UseTextureUniformID;
	};

	// Post processing shader
	struct PostProcessingShader {
		GLuint ProgramID;
		GLuint ScreenTextureUniformID;
		GLuint EmissionTextureUniformID;
	};

	struct Camera {
		float fov;
		float sensitivity;

		MassBody* focusedBody;
		glm::vec3 offset, deltaOffset;
		float distance; // The distance between the camera and the focused position

		int windowWidth, windowHeight;

		bool isOrbiting;
		bool isBeingDragged;
		int startMouseX, startMouseY;
		double lastMouseX, lastMouseY;

		// Stuff that is computed for every frame
		glm::vec3 position;
		glm::vec3 relativeOrbitPosition;
		glm::vec3 focusedPosition;
		glm::mat4 viewMatrix;
		glm::mat4 stationaryViewMatrix; // View Matrix without any translation

		// Things used to animate the camera going from one point to another
		glm::vec3 previousPosition;
		glm::vec3 previousFocusedPosition;
		float interpolationT;
		bool interpolating = false;

		Camera();
		Camera(glm::vec3 offset, glm::vec2 orbitAngles, float distance, float fov, float sensitivity);

		void Update(double mouseX, double mouseY, bool orbiting, bool dragging, float deltaTime);
		void SetFocusedBody(MassBody* focusedBody);
	};

	int init();
	void renderModel(RenderModel model, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Color color);
	void renderBody(MassBody* body, glm::mat4 projectionMatrix);
	void renderStars(glm::mat4 projectionMatrix);
	void renderGrid(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
	void renderFocusOverlay(glm::mat4 projectionMatrix);
	void renderUI();
	void renderAll();
	void preRender();
	void postRender(double deltaTime);
	void mouseDown(float mouseX, float mouseY, int button); // mouseX and mouseY are in OpenGL Screen-space [-1;1]
	void setUniverse(Universe* universe);
	void terminate();

	glm::vec3 CreateMouseRay();
	glm::vec3 PlaneIntersection(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 linePoint, glm::vec3 lineDirection);

	GLFWwindow* getWindow();

	extern Camera camera;
	extern Universe* loadedUniverse;
	extern GLFWwindow* window;

	extern Shader shader;
	extern StarShader starShader;
	extern OverlayShader overlayShader;
	extern UIShader uiShader;
	extern PostProcessingShader postProcessingShader;

	extern std::vector<Sphere*> bodyLODModels; // Array of spheres with different resolutions used to render bodies
	extern StarSphere* starSphereModel;

	extern std::vector<ui::Panel*> uiPanels;
	extern ui::TextFieldComponent* focusedTextField;

	extern GLuint FramebufferID;
	extern GLuint ColorBufferTextureID;
	extern GLuint EmissionBufferTextureID;
	extern GLuint rbo;

	extern int windowWidth;
	extern int windowHeight;
}