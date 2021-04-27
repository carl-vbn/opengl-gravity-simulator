#include "renderer.h"

Universe* renderer::loadedUniverse;
GLFWwindow* renderer::window;

renderer::Shader renderer::shader;
renderer::StarShader renderer::starShader;
renderer::OverlayShader renderer::overlayShader;
renderer::UIShader renderer::uiShader;
renderer::PostProcessingShader renderer::postProcessingShader;

renderer::Camera renderer::camera;

GLuint renderer::FramebufferID;
GLuint renderer::ColorBufferTextureID;
GLuint renderer::EmissionBufferTextureID;
GLuint renderer::rbo;

int renderer::windowWidth;
int renderer::windowHeight;

std::vector<Sphere*> renderer::bodyLODModels;
StarSphere* renderer::starSphereModel;

std::vector<ui::Panel*> renderer::uiPanels;
ui::TextFieldComponent* renderer::focusedTextField;

double lastTime; // Used for deltaTime calculation

bool leftMouseButtonPressed = false;
bool rightMouseButtonPressed = false;
bool middleMouseButtonPressed = false;
bool spacePressed = false;

// Usually nullptr, but has a value when the user is currently spawning an object and settings its parameters
MassBody* spawnedBody;

// This function creates an OpenGL program from a vertex and a fragment shader and returns its ID
GLuint LoadShaderProgram(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Unable to open %s.\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void abortSpawn() {
	delete spawnedBody;
	spawnedBody = nullptr;
	renderer::camera.SetFocusedBody(renderer::loadedUniverse->GetBodies()->at(0));

	ui::showBodyProperties(renderer::camera.focusedBody);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	renderer::camera.distance -= (float)yoffset * renderer::camera.sensitivity;
	renderer::camera.distance = std::fmaxf(renderer::camera.distance, 2.0f);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (renderer::focusedTextField != nullptr) {
			renderer::focusedTextField->onKeyPressed(key);
		}
		else {
			if (key == GLFW_KEY_SPACE) {
				renderer::loadedUniverse->timeScale = 1 - renderer::loadedUniverse->timeScale;
			}
			else if (key == GLFW_KEY_ESCAPE) {
				if (spawnedBody != nullptr) {
					abortSpawn();
				}
			}
			else if (key == GLFW_KEY_DELETE || key == GLFW_KEY_X) {
				if (spawnedBody == nullptr) {
					if (renderer::loadedUniverse->GetBodies()->size() > 1) {
						renderer::loadedUniverse->DeleteBody(renderer::camera.focusedBody);
						renderer::camera.SetFocusedBody(renderer::loadedUniverse->GetBodies()->at(0));
					}
				}
				else {
					abortSpawn();
				}
			}
		}
	}
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	if (renderer::focusedTextField != nullptr) {
		renderer::focusedTextField->onCharKeyPressed(codepoint);
	}
}

void resizeCallback(GLFWwindow* window, int width, int height) {
	
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	renderer::windowWidth = width;
	renderer::windowHeight = height;

	renderer::camera.windowWidth = width;
	renderer::camera.windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, renderer::ColorBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, renderer::EmissionBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, renderer::rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

int renderer::init() {
	std::cout << "Init start" << std::endl;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "Gravity Simulator 3D", NULL, NULL);
	if (!window) // Has the window been created?
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide the mouse

	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwSetWindowAspectRatio(window, 16, 9);

	glewInit();

	std::cout << "Shader load start" << std::endl;

	shader.ProgramID = LoadShaderProgram("shaders\\default\\vertexShader.glsl", "shaders\\default\\fragmentShader.glsl");
	starShader.ProgramID = LoadShaderProgram("shaders\\stars\\vertexShader.glsl", "shaders\\stars\\fragmentShader.glsl");
	overlayShader.ProgramID = LoadShaderProgram("shaders\\overlay\\vertexShader.glsl", "shaders\\overlay\\fragmentShader.glsl");
	uiShader.ProgramID = LoadShaderProgram("shaders\\ui\\vertexShader.glsl", "shaders\\ui\\fragmentShader.glsl");
	postProcessingShader.ProgramID = LoadShaderProgram("shaders\\postprocessing\\vertexShader.glsl", "shaders\\postprocessing\\fragmentShader.glsl");

	shader.MatrixUniformID = glGetUniformLocation(shader.ProgramID, "MVP");
	shader.ViewMatrixUniformID = glGetUniformLocation(shader.ProgramID, "V");
	shader.ModelMatrixUniformID = glGetUniformLocation(shader.ProgramID, "M");
	shader.UnlitUniformID = glGetUniformLocation(shader.ProgramID, "Unlit");
	shader.EmissiveUniformID = glGetUniformLocation(shader.ProgramID, "Emissive");
	shader.LightPosUniformID = glGetUniformLocation(shader.ProgramID, "LightPosition_worldspace");
	shader.LightColorUniformID = glGetUniformLocation(shader.ProgramID, "LightColor");
	shader.ModelColorUniformID = glGetUniformLocation(shader.ProgramID, "ModelColor");
	shader.OccluderCountUniformID = glGetUniformLocation(shader.ProgramID, "OccluderCount");
	shader.LightRadiusUniformID = glGetUniformLocation(shader.ProgramID, "LightRadius");

	for (int i = 0; i < MAX_OCCLUDERS; i++) {
		std::string indexStr = "[" + std::to_string(i) + "]";

		std::string location1Str = "OccluderPositions" + indexStr;
		std::string location2Str = "OccluderRadiuses" + indexStr;

		shader.OccluderPositionsUniformIDs[i] = glGetUniformLocation(shader.ProgramID, location1Str.c_str());
		shader.OccluderRadiusesUniformIDs[i] = glGetUniformLocation(shader.ProgramID, location2Str.c_str());
	}

	starShader.MatrixUniformID = glGetUniformLocation(starShader.ProgramID, "MVP");
	starShader.ViewMatrixUniformID = glGetUniformLocation(starShader.ProgramID, "V");

	overlayShader.CamRightUniformID = glGetUniformLocation(overlayShader.ProgramID, "CameraRight_worldspace");
	overlayShader.CamUpUniformID = glGetUniformLocation(overlayShader.ProgramID, "CameraUp_worldspace");
	overlayShader.ViewProjectionMatrixUniformID = glGetUniformLocation(overlayShader.ProgramID, "VP");
	overlayShader.OverlayPositionUniformID = glGetUniformLocation(overlayShader.ProgramID, "OverlayPos");
	overlayShader.OverlaySizeUniformID = glGetUniformLocation(overlayShader.ProgramID, "OverlaySize");
	overlayShader.TimeUniformID = glGetUniformLocation(overlayShader.ProgramID, "Time");

	uiShader.UseTextureUniformID = glGetUniformLocation(uiShader.ProgramID, "UseTexture");

	postProcessingShader.ScreenTextureUniformID = glGetUniformLocation(postProcessingShader.ProgramID, "ScreenTexture");
	postProcessingShader.EmissionTextureUniformID = glGetUniformLocation(postProcessingShader.ProgramID, "EmissionTexture");

	std::cout << "Shader load end" << std::endl;

	ui::fontRendering::init();

	std::cout << "Setting render parameters" << std::endl;
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one

	// Optimizing: Don't draw the back of every face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// AA
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	// Framebuffer
	glGenFramebuffers(1, &FramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);

	// Generate a texture for the color (since we will later need to read from it)
	glGenTextures(1, &ColorBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, ColorBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBufferTextureID, 0);

	// Repeat for the emission buffer
	glGenTextures(1, &EmissionBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, EmissionBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, EmissionBufferTextureID, 0);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// OpenGL still needs a depth and stencil buffer but we don't need to access them so we don't have to use a texture and can use a RBO instead
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << "Render parameters set" << std::endl;

	camera = Camera(glm::vec3(0, 0, 0), glm::vec2(0, 0), 5.0f, glm::radians(45.0f), 0.5f);
	camera.windowWidth = windowWidth;
	camera.windowHeight = windowHeight;

	for (int i = 0; i < 4; i++) {
		bodyLODModels.push_back(new Sphere(64 / (i + 1), 1));
	}

	starSphereModel = new StarSphere();

	lastTime = glfwGetTime();

	std::cout << "Init end" << std::endl;

	return 1;
}

GLFWwindow* renderer::getWindow() {
	return window;
}

renderer::Camera::Camera(glm::vec3 offset, glm::vec2 orbitAngles, float distance, float fov, float sensitivity) {
	Camera::offset = offset;
	Camera::deltaOffset = glm::vec3(0);
	Camera::distance = distance;
	Camera::fov = fov;
	Camera::sensitivity = sensitivity;

	Camera::focusedBody = nullptr;
	Camera::isBeingDragged = false;
	Camera::isOrbiting = false;
	Camera::startMouseX = 0;
	Camera::startMouseY = 0;
	Camera::lastMouseX = 0;
	Camera::lastMouseY = 0;

	Camera::windowWidth = 1;
	Camera::windowHeight = 1;

	Camera::focusedPosition = offset + deltaOffset;
	Camera::relativeOrbitPosition = glm::vec3(0, 0, 1.0F);
	Camera::position = focusedPosition + relativeOrbitPosition * distance;
	Camera::previousPosition = position;
	Camera::previousFocusedPosition = focusedPosition;
	Camera::interpolating = false;
	Camera::interpolationT = false;

	Camera::viewMatrix = glm::mat4();
	Camera::stationaryViewMatrix = glm::mat4();
}

renderer::Camera::Camera() : renderer::Camera::Camera(glm::vec3(0), glm::vec2(0), 0.0F, glm::radians(45.0f), 0.5f) {}

// Handles all camera movement. Called every frame.
void renderer::Camera::Update(double mouseX, double mouseY, bool orbiting, bool dragging, float deltaTime) {
	focusedPosition = ((focusedBody != nullptr) ? focusedBody->position : glm::vec3(0)) + offset + deltaOffset;

	glm::vec3 viewDir = glm::normalize(focusedPosition - position);
	glm::vec3 upVector = glm::vec3(0, 1, 0);
	glm::vec3 rightVector = glm::cross(viewDir, upVector);

	// This was adapted mainly from https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
	if (orbiting) {
		if (!Camera::isOrbiting) {
			Camera::isOrbiting = true;

			startMouseX = (int)mouseX;
			startMouseY = (int)mouseY;
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}

		// Calculate the amount of rotation given the mouse movement.
		float deltaAngleX = (glm::two_pi<float>() / windowWidth);
		float deltaAngleY = (glm::two_pi<float>() / windowHeight);
		float xAngle = (float)(lastMouseX - mouseX) * deltaAngleX;
		float yAngle = (float)(lastMouseY - mouseY) * deltaAngleY;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		float cosAngle = glm::dot(viewDir, upVector);
		if (cosAngle * glm::sign(yAngle) > 0.99f)
			yAngle = 0;

		// Rotation on first axis
		glm::mat4 rotationMatrixX(1.0f);
		rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, upVector);
		relativeOrbitPosition = (rotationMatrixX * glm::vec4(relativeOrbitPosition, 1.0f));

		// Rotation on second axis
		glm::mat4 rotationMatrixY(1.0f);
		rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, rightVector);
		relativeOrbitPosition = (rotationMatrixY * glm::vec4(relativeOrbitPosition, 1.0f));

		lastMouseX = mouseX;
		lastMouseY = mouseY;
	}
	else if (Camera::isOrbiting) {
		Camera::isOrbiting = false;
	}

	position = focusedPosition + relativeOrbitPosition * distance;

	if (interpolating) {
		position = glm::mix(previousPosition, position, interpolationT);
		focusedPosition = glm::mix(previousFocusedPosition, focusedPosition, interpolationT);

		interpolationT += deltaTime * 4.0f;
		if (interpolationT >= 1) {
			interpolationT = 1;
			interpolating = false;
		}
	}

	// Calculate view matrices (one with translation, one without that is used to render the background)
	stationaryViewMatrix = glm::lookAt(
		glm::vec3(0),
		focusedPosition - position, // What to look at
		upVector
	);

	viewMatrix = glm::lookAt(
		position, // Camera position
		focusedPosition, // What to look at
		upVector
	);

	if (dragging) {
		if (!Camera::isBeingDragged) {
			Camera::isBeingDragged = true;
			startMouseX = (int)mouseX;
			startMouseY = (int)mouseY;
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}
		else {
			glm::mat4 iV = glm::inverse(viewMatrix);
			glm::vec3 horizontalAxis = iV * glm::vec4(-1, 0, 0, 0);
			glm::vec3 verticalAxis = iV * glm::vec4(0, 1, 0, 0);

			deltaOffset = (horizontalAxis * (float)(mouseX - startMouseX) + verticalAxis * (float)(mouseY - startMouseY)) * sensitivity * 0.01f;
		}
	}
	else if (Camera::isBeingDragged) {
		Camera::isBeingDragged = false;
		offset = offset + deltaOffset;
		deltaOffset = glm::vec3(0);
	}
}

void renderer::Camera::SetFocusedBody(MassBody* focusedBody) {
	this->focusedBody = focusedBody;
	this->previousPosition = camera.position;
	this->previousFocusedPosition = camera.focusedPosition;
	this->interpolating = true;
	this->interpolationT = 0;
	this->offset = glm::vec3(0);

	ui::showBodyProperties(focusedBody);
}

void renderer::preRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);

	GLenum drawbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawbuffers);
	glUniform1i(shader.UnlitUniformID, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);
}

// Both are in OpenGL screenspace [-1; 1]
double previousMouseX = 0;
double previousMouseY = 0;

void renderer::postRender(double deltaTime) {
	// Mouse button events
	int leftBtnState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int rightBtnState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	int middleBtnState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	int spacebarState = glfwGetKey(window, GLFW_KEY_SPACE);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Convert mouse coordinates to OpenGL screenspace [-1; 1]
	mouseX = mouseX / windowWidth * 2 - 1;
	mouseY = -(mouseY / windowHeight * 2 - 1);

	if (mouseX != previousMouseX || mouseY != previousMouseY) {
		for (unsigned int i = 0; i < uiPanels.size(); i++) {
			uiPanels[i]->onMouseMoved((float)mouseX, (float)mouseY);
		}

		previousMouseX = mouseX;
		previousMouseY = mouseY;
	}

	if (leftBtnState && !leftMouseButtonPressed) {
		mouseDown((float)mouseX, (float)mouseY, GLFW_MOUSE_BUTTON_LEFT);
		leftMouseButtonPressed = true;
	}
	else if (!leftBtnState && leftMouseButtonPressed) {
		leftMouseButtonPressed = false;
	}

	if (rightBtnState && !rightMouseButtonPressed) {
		mouseDown((float)mouseX, (float)mouseY, GLFW_MOUSE_BUTTON_RIGHT);
		rightMouseButtonPressed = true;
	}
	else if (!rightBtnState && rightMouseButtonPressed) {
		rightMouseButtonPressed = false;
	}

	if (middleBtnState && !middleMouseButtonPressed) {
		mouseDown((float)mouseX, (float)mouseY, GLFW_MOUSE_BUTTON_MIDDLE);
		middleMouseButtonPressed = true;
	}
	else if (!middleBtnState && middleMouseButtonPressed) {
		middleMouseButtonPressed = false;
	}

	// Universe tick
	if (renderer::loadedUniverse != nullptr)
		renderer::loadedUniverse->tick(deltaTime);
}

void renderer::mouseDown(float mouseX, float mouseY, int button) { // mouseX and mouseY are in OpenGL Screen-space [-1;1]
	bool absorbed = false;

	if (focusedTextField != nullptr) {
		focusedTextField->setFocused(false);
		absorbed = true;
	}

	for (unsigned int i = 0; i < uiPanels.size(); i++) {
		if (uiPanels[i]->GetBounds().Contains(mouseX, mouseY)) {
			uiPanels[i]->onMouseDown(mouseX, mouseY, button);
			absorbed = true;
		}
	}



	if (!absorbed) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (spawnedBody == nullptr) {
				Universe::RaycastHit hitResult = loadedUniverse->Raycast(camera.position, CreateMouseRay());
				if (hitResult.hit) {
					camera.SetFocusedBody(hitResult.hitBody);
				}
			}
			else {
				abortSpawn();
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT) {
			MassBody* focusedBody = camera.focusedBody;
			glm::vec3 mouseRay = CreateMouseRay();
			glm::vec3 planeNormal = glm::vec3(0, 1, 0); //glm::normalize(camera.position - focusedBody->position); (uncomment if you don't want the spawn position to be contrainted to the horizontal plane)
			glm::vec3 planeIntersection = PlaneIntersection(focusedBody->position, planeNormal, camera.position, mouseRay);
			if (spawnedBody == nullptr) {
				spawnedBody = new MassBody(planeIntersection, focusedBody->mass * 0.1F, focusedBody->radius * 0.1F, Color((unsigned int)(std::rand()/(float)RAND_MAX*0xFFFFFF)));
				
				// Discretely focus the new body
				camera.offset = glm::vec3(camera.focusedPosition - spawnedBody->position);
				camera.focusedBody = spawnedBody;
				
				ui::showBodyProperties(spawnedBody, "Spawned body");
			}
			else {
				glm::vec3 spawnVelocity = (planeIntersection - spawnedBody->position) * 0.5F;
				spawnedBody->velocity = spawnVelocity;
				loadedUniverse->AddBody(spawnedBody);
				ui::showBodyProperties(camera.focusedBody);

				spawnedBody = nullptr;
			}
		}
	}
}

void renderer::setUniverse(Universe* universe) {
	renderer::loadedUniverse = universe;
	camera.focusedBody = universe->GetBodies()->at(0);
}

// from https://stackoverflow.com/a/30005258
glm::vec3 renderer::CreateMouseRay() {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
	mouseX = mouseX / (windowWidth * 0.5) - 1.0;
	mouseY = mouseY / (windowHeight * 0.5) - 1.0;

	glm::mat4 proj = glm::perspective(camera.fov, (float)windowWidth / (float)windowHeight, 0.1f, 500.0f);
	glm::mat4 view = camera.stationaryViewMatrix;

	glm::mat4 invVP = glm::inverse(proj * view);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return dir;
}

// Adapted from https://stackoverflow.com/a/52711312/6557067
glm::vec3 renderer::PlaneIntersection(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 linePoint, glm::vec3 lineDirection) {
	if (glm::dot(planeNormal, glm::normalize(lineDirection)) == 0) {
		return planePoint; // In this case, there is no intersection so we just return the given point on the plane to avoid problems. This situation is extremely unlikely in practical circumstances.
	}

	float t = (glm::dot(planeNormal, planePoint) - glm::dot(planeNormal, linePoint)) / glm::dot(planeNormal, glm::normalize(lineDirection));
	return linePoint + (glm::normalize(lineDirection) * t);
}

// Requires the default shader to be bound
void renderer::renderModel(RenderModel model, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Color color) {
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around

	MassBody* emittingBody = loadedUniverse->GetEmissiveBody();

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(shader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(shader.ModelMatrixUniformID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform3f(shader.LightColorUniformID, emittingBody->color.red, emittingBody->color.green, emittingBody->color.blue);
	glUniform3f(shader.ModelColorUniformID, color.red, color.green, color.blue);

	glUniform3f(shader.LightPosUniformID, emittingBody->position.x, emittingBody->position.y, emittingBody->position.z);
	glUniform1f(shader.LightRadiusUniformID, emittingBody->radius);

	glBindVertexArray(model.VertexArrayID);

	// Bind the model index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.IndexBufferID);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,          // mode
		model.GetIndexCount(), // index count
		GL_UNSIGNED_INT,       // type
		(void*)0               // element array buffer offset
	);
}

void renderer::renderBody(MassBody* body, glm::mat4 projectionMatrix) {
	glm::mat4 modelMatrix = glm::mat4(1);

	modelMatrix = glm::translate(modelMatrix, body->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(body->radius));

	bool isEmissive = body == loadedUniverse->GetEmissiveBody();

	if (isEmissive) {
		glUniform1i(shader.OccluderCountUniformID, 0);
		glUniform1i(shader.UnlitUniformID, 1);
		glUniform1i(shader.EmissiveUniformID, 1);
	}
	else {
		glUniform1i(shader.OccluderCountUniformID, body->occluders.size());
		for (unsigned int i = 0; i < body->occluders.size(); i++) {
			MassBody* occluder = body->occluders[i];
			glUniform3f(shader.OccluderPositionsUniformIDs[i], occluder->position.x, occluder->position.y, occluder->position.z);
			glUniform1f(shader.OccluderRadiusesUniformIDs[i], occluder->radius);
		}
	}

	float distanceFromCamera = glm::distance(camera.position, body->position) - body->radius;

	renderModel(*bodyLODModels[(int)fminf(fmaxf(distanceFromCamera / (body->radius * 8.0f), 0.0f), 3.0f)], projectionMatrix, camera.viewMatrix, modelMatrix, body->color); // The weird looking formula is for choosing the right LOD model based on distance from camera and radius. I just tried different configurations out to try to find the right balance.

	if (isEmissive) {
		glUniform1i(shader.UnlitUniformID, 0);
		glUniform1i(shader.EmissiveUniformID, 0);
	}
	else {
		glUniform1i(shader.OccluderCountUniformID, 0);
	}
}

// Requires the starShader
void renderer::renderStars(glm::mat4 projectionMatrix) {
	glm::mat4 viewMatrix = camera.stationaryViewMatrix;

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = projectionMatrix * viewMatrix; // Remember, matrix multiplication is the other way around

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(starShader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(starShader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);

	glBindVertexArray(starSphereModel->VertexArrayID);

	// Bind the model index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starSphereModel->IndexBufferID);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,          // mode
		starSphereModel->GetIndexCount(), // index count
		GL_UNSIGNED_INT,       // type
		(void*)0               // element array buffer offset
	);
}


// Requires the default shader
void renderer::renderGrid(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
	glm::mat4 modelMatrix = glm::mat4(1);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(shader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(shader.ModelMatrixUniformID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);

	glUniform1i(shader.UnlitUniformID, 1);

	glLineWidth(3);
	glUniform3f(shader.ModelColorUniformID, 1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	for (float j = -1000; j <= 1000; j += 100) {
		glVertex3f(j, 0.0f, 0.0f);
	}
	glEnd();

	glUniform3f(shader.ModelColorUniformID, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	for (float j = -1000; j <= 1000; j += 100) {
		glVertex3f(0, 0.0f, j);
	}
	glEnd();


	// TODO Only render grid where the camera is (at the moment a large fixed grid is drawn at the center of the world)
	glLineWidth(1);
	glUniform3f(shader.ModelColorUniformID, 1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	for (float i = -1000; i <= 1000; i += 10) {
		for (float j = -1000; j <= 1000; j += 100) {
			glVertex3f(i, 0.0f, j);
		}
		glPrimitiveRestartNV();

		for (float j = -1000; j <= 1000; j += 100) {
			glVertex3f(j, 0.0f, i);
		}
		glPrimitiveRestartNV();
	}
	glEnd();
}

// Requires the overlayShader
void renderer::renderFocusOverlay(glm::mat4 projectionMatrix) {
	MassBody* focusedBody = camera.focusedBody;
	glm::mat4 viewMatrix = camera.viewMatrix;

	// http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
	glUniform3f(overlayShader.CamRightUniformID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	glUniform3f(overlayShader.CamUpUniformID, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

	glUniform3f(overlayShader.OverlayPositionUniformID, focusedBody->position.x, focusedBody->position.y, focusedBody->position.z);
	glUniform2f(overlayShader.OverlaySizeUniformID, focusedBody->radius * 1.5f, focusedBody->radius * 1.5f);

	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	glUniformMatrix4fv(overlayShader.ViewProjectionMatrixUniformID, 1, GL_FALSE, &viewProjectionMatrix[0][0]);

	glUniform1f(overlayShader.TimeUniformID, (float)glfwGetTime());

	glBegin(GL_TRIANGLES);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
}

// Requires the uiShader
void renderer::renderUI() {
	// Disable depth test as the draw order should determine what fragments are above each other
	glDisable(GL_DEPTH_TEST);

	for (unsigned int i = 0; i < uiPanels.size(); i++) {
		uiPanels[i]->draw();
	}

	glEnable(GL_DEPTH_TEST);
}

void renderer::renderAll() {
	preRender();

	double preRenderTime = glfwGetTime();
	double deltaTime = preRenderTime - lastTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Fill the void

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Projection matrix : Camera Field of View, right aspect ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projectionMatrix = glm::perspective(renderer::camera.fov, (float)windowWidth / (float)windowHeight, 0.1f, 500.0f);

	bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	camera.Update(mouseX, mouseY, !shiftPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE), shiftPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE), (float)deltaTime);

	glEnable(GL_DEPTH_TEST);
	glUseProgram(starShader.ProgramID);
	renderStars(projectionMatrix);
	glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer so that the star sphere will be ignored in depth testing

	glUseProgram(shader.ProgramID);
	glUniform1i(shader.UnlitUniformID, 1);
	renderGrid(projectionMatrix, camera.viewMatrix);

	// Render the path of the spawned object
	if (spawnedBody != nullptr) {
		MassBody* focusedBody = camera.focusedBody;
		glm::vec3 mouseRay = CreateMouseRay();
		glm::vec3 planeNormal = glm::vec3(0, 1, 0); //glm::normalize(camera.position - focusedBody->position); (uncomment if you don't want the spawn position to be contrainted to the horizontal plane)
		glm::vec3 planeIntersection = PlaneIntersection(focusedBody->position, planeNormal, camera.position, mouseRay);

		glm::vec3 spawnVelocity = (planeIntersection - spawnedBody->position) * 0.5F;

		glm::vec3 p = spawnedBody->position;
		glm::vec3 velocity = spawnVelocity;
		auto bodies = loadedUniverse->GetBodies();
		for (int i = 0; i < 50; i++) {
			// Step 1: calculate velocity
			glm::vec3 totalGravitationalForce = glm::vec3(0);
			// Loop through all bodies in the universe to calculate their gravitational pull on the object (TODO: Ignore very far away objects for better performance)
			for (unsigned int j = 0; j < bodies->size(); j++) {
				if (i == j) continue; // No need to calculate the gravitational pull of a body on itself as it will always be 0.
				MassBody* otherBody = bodies->at(j);

				if (!otherBody->affectsOthers) continue; // Ignore this one

				float force = loadedUniverse->gConstant * spawnedBody->mass * otherBody->mass / std::pow(glm::distance(p, otherBody->position), 2);
				glm::vec3 forceDirection = glm::normalize(otherBody->position - p);

				totalGravitationalForce += forceDirection * force;
			}

			// Update the velocity of the object by adding the acceleration, which is the force divided by the object's mass. (and multiply it all by deltaTime)
			velocity += totalGravitationalForce / spawnedBody->mass * 0.5f;

			// Step 2: update position
			p += velocity * 0.5f;

			renderModel(*bodyLODModels[3], projectionMatrix, camera.viewMatrix, glm::scale(glm::translate(glm::mat4(1), p), glm::vec3(0.05F)), COLOR_WHITE);
		}
	}

	std::vector<MassBody*> bodies = *loadedUniverse->GetBodies();
	for (unsigned int i = 0; i < bodies.size(); i++) {
		renderBody(bodies[i], projectionMatrix);
	}

	if (spawnedBody != nullptr) {
		renderBody(spawnedBody, projectionMatrix);
	}

	glDisable(GL_DEPTH_TEST); // No depth test required from here on as we won't be rendering any 3D stuff

	// First, let's draw the contents of the color buffer texture to the screen with post-processing. We do this now as we don't want to post-process UI and overlays
	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw content of color buffer texture to screen
	glUseProgram(postProcessingShader.ProgramID);
	glUniform1i(postProcessingShader.ScreenTextureUniformID, 0);
	glUniform1i(postProcessingShader.EmissionTextureUniformID, 1);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, ColorBufferTextureID);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, EmissionBufferTextureID);
	glActiveTexture(GL_TEXTURE0 + 0);

	// Draw a quad covering the entire screen (TODO GL_QUADS is depreacted and we should be using a VAO for this)
	glBegin(GL_QUADS);
	glVertexAttrib2f(1, 0.0F, 1.0F);
	glVertex2f(-1.0f, 1.0f);
	glVertexAttrib2f(1, 0.0F, 0.0F);
	glVertex2f(-1.0f, -1.0f);
	glVertexAttrib2f(1, 1.0F, 0.0F);
	glVertex2f(1.0f, -1.0f);
	glVertexAttrib2f(1, 1.0F, 1.0F);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	// Let's now draw the rest (focus overlay + ui) on top of that
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(overlayShader.ProgramID);
	renderFocusOverlay(projectionMatrix);

	glUseProgram(uiShader.ProgramID);
	renderUI();

	glDisable(GL_BLEND);

	/* Swap front and back buffers */
	glfwSwapBuffers(window);

	glfwPollEvents();

	lastTime = preRenderTime;

	postRender(deltaTime);
}

void renderer::terminate() {
	delete starSphereModel;

	for (unsigned int i = 0; i < bodyLODModels.size(); i++) {
		delete bodyLODModels[i];
	}

	glDeleteTextures(1, &ColorBufferTextureID);
	glDeleteTextures(1, &EmissionBufferTextureID);
	glDeleteFramebuffers(1, &FramebufferID);

	glfwTerminate();
}