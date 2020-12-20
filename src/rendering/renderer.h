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
#include "../ui/panel.h"
#include "../ui/font_renderer.h"
#include "../ui/text_field.h"

namespace renderer {
	// Default shader
	struct Shader {
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
		GLuint LightPosUniformID;
		GLuint ModelColorUniformID;
	};

	// Shader used to render star spheres
	struct StarShader {
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
	};

	// Shader used to render overlays
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

	struct Camera {
		float fov;
		float sensitivity;

		unsigned int focusedBodyIndex;
		glm::vec3 offset, deltaOffset;
		glm::vec2 orbitAngles, deltaOrbitAngles;
		float distance; // The distance between the camera and the focused position

		int windowWidth, windowHeight;

		bool isOrbiting;
		bool isBeingDragged;
		int startMouseX;
		int startMouseY;

		// Stuff that is computed for every frame
		glm::vec3 Position;
		glm::vec3 FocusedPosition;
		glm::mat4 ViewMatrix;
		glm::mat4 StationaryViewMatrix; // View Matrix without any translation

		// Things used to animate the camera going from one point to another
		glm::vec3 previousPosition;
		glm::vec3 previousFocusedPosition;
		float interpolationT;
		bool interpolating = false;

		Camera();
		Camera(glm::vec3 offset, glm::vec2 orbitAngles, float distance, float fov, float sensitivity);

		void Update(int mouseX, int mouseY, bool orbiting, bool dragging, float deltaTime);
		void SetFocusedBody(int focusedBodyIndex);
	};

	int init();
	void renderModel(RenderModel model, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Color color);
	void renderBody(MassBody body, glm::mat4 projectionMatrix);
	void renderStars(glm::mat4 projectionMatrix);
	void renderFocusOverlay(glm::mat4 projectionMatrix);
	void renderUI();
	void renderAll();
	void preRender();
	void postRender(double deltaTime);
	void mouseDown(float mouseX, float mouseY, int button); // mouseX and mouseY are in OpenGL Screen-space [-1;1]
	void loadUniverse(Universe* universe);
	void unloadUniverse();
	void terminate();

	glm::vec3 CreateMouseRay();

	GLFWwindow* getWindow();

	extern Camera camera;
	extern Universe* loadedUniverse;
	extern GLFWwindow* window;
	extern Shader shader;
	extern StarShader starShader;
	extern OverlayShader overlayShader;
	extern UIShader uiShader;

	extern RenderModel* bodyLODModels; // Array of spheres with different resolutions used to render bodies
	extern StarSphere* starSphereModel;

	extern std::vector<ui::Panel> uiPanels;
	extern ui::TextFieldComponent* focusedTextField;

	extern int windowWidth;
	extern int windowHeight;
}