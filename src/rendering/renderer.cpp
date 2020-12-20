#include "renderer.h"

Universe* renderer::loadedUniverse;
GLFWwindow* renderer::window;
renderer::Shader renderer::shader;
renderer::StarShader renderer::starShader;
renderer::OverlayShader renderer::overlayShader;
renderer::UIShader renderer::uiShader;
renderer::Camera renderer::camera;

int renderer::windowWidth;
int renderer::windowHeight;

RenderModel* renderer::bodyLODModels;
StarSphere* renderer::starSphereModel;

std::vector<ui::Panel> renderer::uiPanels;
ui::TextFieldComponent* renderer::focusedTextField;

double lastTime; // Used for deltaTime calculation

bool leftMouseButtonPressed = false;
bool rightMouseButtonPressed = false;
bool middleMouseButtonPressed = false;
bool spacePressed = false;

// This function creates an OpenGL program from a vertex and a fragment shader and returns its ID
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	renderer::camera.distance -= yoffset * renderer::camera.sensitivity;
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
		}
	}
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	if (renderer::focusedTextField != nullptr) {
		renderer::focusedTextField->onCharKeyPressed(codepoint);
	}
}

void resizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	
	renderer::windowWidth = width;
	renderer::windowHeight = height;

	renderer::camera.windowWidth = width;
	renderer::camera.windowHeight = height;
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

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwSetWindowAspectRatio(window, 16, 9);

	glewInit();

	std::cout << "Shader load start" << std::endl;

	shader.ProgramID = LoadShaders("shaders\\default\\vertexShader.glsl", "shaders\\default\\fragmentShader.glsl");
	starShader.ProgramID = LoadShaders("shaders\\stars\\vertexShader.glsl", "shaders\\stars\\fragmentShader.glsl");
	overlayShader.ProgramID = LoadShaders("shaders\\overlay\\vertexShader.glsl", "shaders\\overlay\\fragmentShader.glsl");
	uiShader.ProgramID = LoadShaders("shaders\\ui\\vertexShader.glsl", "shaders\\ui\\fragmentShader.glsl");

	shader.MatrixUniformID = glGetUniformLocation(shader.ProgramID, "MVP");
	shader.ViewMatrixUniformID = glGetUniformLocation(shader.ProgramID, "V");
	shader.ModelMatrixUniformID = glGetUniformLocation(shader.ProgramID, "M");
	shader.LightPosUniformID = glGetUniformLocation(shader.ProgramID, "LightPosition_worldspace");
	shader.ModelColorUniformID = glGetUniformLocation(shader.ProgramID, "ModelColor");

	starShader.MatrixUniformID = glGetUniformLocation(starShader.ProgramID, "MVP");
	starShader.ViewMatrixUniformID = glGetUniformLocation(starShader.ProgramID, "V");

	overlayShader.CamRightUniformID = glGetUniformLocation(overlayShader.ProgramID, "CameraRight_worldspace");
	overlayShader.CamUpUniformID = glGetUniformLocation(overlayShader.ProgramID, "CameraUp_worldspace");
	overlayShader.ViewProjectionMatrixUniformID = glGetUniformLocation(overlayShader.ProgramID, "VP");
	overlayShader.OverlayPositionUniformID = glGetUniformLocation(overlayShader.ProgramID, "OverlayPos");
	overlayShader.OverlaySizeUniformID = glGetUniformLocation(overlayShader.ProgramID, "OverlaySize");
	overlayShader.TimeUniformID = glGetUniformLocation(overlayShader.ProgramID, "Time");

	uiShader.UseTextureUniformID = glGetUniformLocation(uiShader.ProgramID, "UseTexture");

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

	std::cout << "Render parameters set" << std::endl;

	camera = Camera(glm::vec3(0,0,0), glm::vec2(0,0), 5.0f, glm::radians(45.0f), 0.5f);
	camera.windowWidth = windowWidth;
	camera.windowHeight = windowHeight;

	bodyLODModels = new RenderModel[4];
	for (int i = 0; i < 4; i++) {
		*(bodyLODModels + i) = Sphere(32 / (i+1), 1);
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
	Camera::orbitAngles = orbitAngles;
	Camera::deltaOrbitAngles = glm::vec3(0);
	Camera::distance = distance;
	Camera::fov = fov;
	Camera::sensitivity = sensitivity;

	Camera::focusedBodyIndex = 0;
	Camera::isBeingDragged = false;
	Camera::isOrbiting = false;
	Camera::startMouseX = 0;
	Camera::startMouseY = 0;

	Camera::windowWidth = 1;
	Camera::windowHeight = 1;
}

renderer::Camera::Camera() {}

void renderer::Camera::Update(int mouseX, int mouseY, bool orbiting, bool dragging, float deltaTime) {
	// Calculate position
	float phi = orbitAngles.x + deltaOrbitAngles.x;
	float theta = orbitAngles.y + deltaOrbitAngles.y;
	bool upsideDown = false;

	if (fmodf(theta + 1.5708f, 6.2831f) > 3.1415f) {
		upsideDown = true;
	}

	float x = distance * cos(phi) * cos(theta);
	float y = distance * cos(phi) * sin(theta);
	float z = distance * sin(phi);

	glm::vec3 orbitOffset = glm::vec3(x, y, z);

	MassBody* focusedBody = nullptr;
	if (loadedUniverse->GetBodies()->size() > focusedBodyIndex) focusedBody = &loadedUniverse->GetBodies()->at(focusedBodyIndex);

	FocusedPosition = ((focusedBody != nullptr) ? focusedBody->position : glm::vec3(0)) + offset + deltaOffset;
	Position = FocusedPosition + orbitOffset;

	if (interpolating) {
		Position = glm::mix(previousPosition, Position, interpolationT);
		FocusedPosition = glm::mix(previousFocusedPosition, FocusedPosition, interpolationT);

		interpolationT += deltaTime*4.0f;
		if (interpolationT >= 1) {
			interpolationT = 1;
			interpolating = false;
		}
	}

	// Calculate view matrices (one with translation, one without that is used to render the background)
	StationaryViewMatrix = glm::lookAt(
			glm::vec3(0),
		FocusedPosition - Position, // What to look at
			upsideDown ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0)
		);
	
	ViewMatrix = glm::lookAt(
			Position,
			FocusedPosition, // What to look at
			upsideDown ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0)
		);

	// Movement
	if (orbiting) {
		if (!Camera::isOrbiting) {
			Camera::isOrbiting = true;
			startMouseX = mouseX;
			startMouseY = mouseY;
		}
		else {
			deltaOrbitAngles = glm::vec2(mouseX - startMouseX, mouseY - startMouseY) * sensitivity * 0.01f;
		}
	}
	else if (Camera::isOrbiting) {
		Camera::isOrbiting = false;
		orbitAngles = orbitAngles + deltaOrbitAngles;
		deltaOrbitAngles = glm::vec3(0);
	}

	if (dragging) {
		if (!Camera::isBeingDragged) {
			Camera::isBeingDragged = true;
			startMouseX = mouseX;
			startMouseY = mouseY;
		}
		else {
			glm::mat4 iV = glm::inverse(ViewMatrix);
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

void renderer::Camera::SetFocusedBody(int focusedBodyIndex) {
	this->focusedBodyIndex = focusedBodyIndex;
	this->previousPosition = camera.Position;
	this->previousFocusedPosition = camera.FocusedPosition;
	this->interpolating = true;
	this->interpolationT = 0;
	this->offset = glm::vec3(0);
}

void renderer::preRender() {
	
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
		for (int i = 0; i < uiPanels.size(); i++) {
			uiPanels.at(i).onMouseMoved(mouseX, mouseY);
		}

		previousMouseX = mouseX;
		previousMouseY = mouseY;
	}

	if (leftBtnState && !leftMouseButtonPressed) {
		mouseDown(mouseX, mouseY, GLFW_MOUSE_BUTTON_LEFT);
		leftMouseButtonPressed = true;
	}
	else if (!leftBtnState && leftMouseButtonPressed) {
		leftMouseButtonPressed = false;
	}

	if (rightBtnState && !rightMouseButtonPressed) {
		mouseDown(mouseX, mouseY, GLFW_MOUSE_BUTTON_RIGHT);
		rightMouseButtonPressed = true;
	}
	else if (!rightBtnState && rightMouseButtonPressed) {
		rightMouseButtonPressed = false;
	}

	if (middleBtnState && !middleMouseButtonPressed) {
		mouseDown(mouseX, mouseY, GLFW_MOUSE_BUTTON_MIDDLE);
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
	else {
		for (int i = 0; i < uiPanels.size(); i++) {
			if (uiPanels.at(i).GetBounds().Contains(mouseX, mouseY)) {
				uiPanels.at(i).onMouseDown(mouseX, mouseY, button);
				absorbed = true;
			}
		}
	}


	if (!absorbed) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			glm::vec3 rayDir = CreateMouseRay();

			glm::vec3 pos = camera.Position + rayDir * 2.0f;
			loadedUniverse->AddBody(MassBody(pos, 100, 0.1, Color(1.0f, 0.0f, 0.0f)));
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			Universe::RaycastHit hitResult = loadedUniverse->Raycast(camera.Position, CreateMouseRay());
			if (hitResult.hit) {
				camera.SetFocusedBody(hitResult.hitBodyIndex);
			}
		}
	}
}

void renderer::loadUniverse(Universe* universe) {
	renderer::loadedUniverse = universe;
}

void renderer::unloadUniverse() {
	delete renderer::loadedUniverse;
	renderer::loadedUniverse = nullptr;
}

// from https://stackoverflow.com/a/30005258
glm::vec3 renderer::CreateMouseRay() {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
	mouseX = mouseX / (windowWidth * 0.5) - 1.0;
	mouseY = mouseY / (windowHeight * 0.5) - 1.0;

	glm::mat4 proj = glm::perspective(camera.fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.StationaryViewMatrix;

	glm::mat4 invVP = glm::inverse(proj * view);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return dir;
}

void renderer::renderModel(RenderModel model, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Color color) {
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix; // Remember, matrix multiplication is the other way around

	// Use shader
	glUseProgram(shader.ProgramID);

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(shader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(shader.ModelMatrixUniformID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform3f(shader.ModelColorUniformID, color.red, color.green, color.blue);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(shader.LightPosUniformID, lightPos.x, lightPos.y, lightPos.z);

	glBindVertexArray(model.VertexArrayID);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, model.VertexBufferID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, model.NormalBufferID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Bind the model
	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.IndexBufferID);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,          // mode
		model.GetIndexCount(), // index count
		GL_UNSIGNED_INT,       // type
		(void*)0               // element array buffer offset
	);

	glDisableVertexAttribArray(0);
}

void renderer::renderBody(MassBody body, glm::mat4 projectionMatrix) {
	glm::mat4 modelMatrix = glm::mat4(1);

	modelMatrix = glm::translate(modelMatrix, body.position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(body.size));

	renderModel(bodyLODModels[0], projectionMatrix, camera.ViewMatrix, modelMatrix, body.color);
}

void renderer::renderStars(glm::mat4 projectionMatrix) {
	glm::mat4 viewMatrix = camera.StationaryViewMatrix;

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = projectionMatrix * viewMatrix; // Remember, matrix multiplication is the other way around

	// Use shader
	glUseProgram(starShader.ProgramID);

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(starShader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(starShader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);

	glBindVertexArray(starSphereModel->VertexArrayID);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, starSphereModel->VertexBufferID);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, starSphereModel->NormalBufferID);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Bind the model
	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starSphereModel->IndexBufferID);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,          // mode
		starSphereModel->GetIndexCount(), // index count
		GL_UNSIGNED_INT,       // type
		(void*)0               // element array buffer offset
	);

	glDisableVertexAttribArray(0);
}

void renderer::renderFocusOverlay(glm::mat4 projectionMatrix) {
	// Use shader
	glUseProgram(overlayShader.ProgramID);

	MassBody focusedBody = loadedUniverse->GetBodies()->at(camera.focusedBodyIndex);
	glm::mat4 viewMatrix = camera.ViewMatrix;

	// http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
	glUniform3f(overlayShader.CamRightUniformID, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	glUniform3f(overlayShader.CamUpUniformID, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

	glUniform3f(overlayShader.OverlayPositionUniformID, focusedBody.position.x, focusedBody.position.y, focusedBody.position.z);
	glUniform2f(overlayShader.OverlaySizeUniformID, focusedBody.size * 1.5f, focusedBody.size * 1.5f);

	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	glUniformMatrix4fv(overlayShader.ViewProjectionMatrixUniformID, 1, GL_FALSE, &viewProjectionMatrix[0][0]);

	glUniform1f(overlayShader.TimeUniformID, (float)glfwGetTime());

	glBegin(GL_QUADS);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glEnd();
}

void renderer::renderUI() {
	// User shader
	glUseProgram(uiShader.ProgramID);

	// Disable depth test as the draw order should determine what fragments are above each other
	glDisable(GL_DEPTH_TEST);

	for (int i = 0; i < uiPanels.size(); i++) {
		uiPanels.at(i).draw();
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
	glm::mat4 Projection = glm::perspective(renderer::camera.fov, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
	
	bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
	camera.Update(mouseX, mouseY, !shiftPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE), shiftPressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE), deltaTime);

	renderStars(Projection);
	glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer so that the star sphere will be ignored in depth testing

	std::vector<MassBody> bodies = *loadedUniverse->GetBodies();
	for (unsigned int i = 0; i < bodies.size(); i++) {
		renderBody(bodies.at(i), Projection);
	}

	glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer once again so the overlay and the UI are above everything else

	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderFocusOverlay(Projection);
	renderUI();

	glDisable(GL_BLEND);

	/* Swap front and back buffers */
	glfwSwapBuffers(window);

	glfwPollEvents();

	lastTime = preRenderTime;

	postRender(deltaTime);
}

void renderer::terminate() {
	delete[] bodyLODModels;
	delete starSphereModel;

	glfwTerminate();
}