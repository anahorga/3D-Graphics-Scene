//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 5.0f, 40.0f), glm::vec3(0.0f, 5.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.03f;


bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D newModel;
gps::Model3D barca;
gps::Model3D lumina;
gps::Model3D screenQuad;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

gps::Shader depthShader;
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;

//lumina punctiforma1
glm::vec3 pointLight1 = glm::vec3(10.926f, 0.3914f, 21.264f);
GLuint pointLightLoc1;
glm::vec3 pointLightColor1 = glm::vec3(1.0f, 1.0f, 1.0f);
GLuint pointLightColorLoc1;


//lumina punctiforma2
glm::vec3 pointLight2 = glm::vec3(10.631f, 0.29589f, 8.4823f);
GLuint pointLightLoc2;
glm::vec3 pointLightColor2 = glm::vec3(1.0f, 1.0f, 1.0f);
GLuint pointLightColorLoc2;


//lumina punctiforma3
glm::vec3 pointLight3 = glm::vec3(15.673f, 0.73439f, 4.6441f);
GLuint pointLightLoc3;
glm::vec3 pointLightColor3 = glm::vec3(1.0f, 1.0f, 1.0f);
GLuint pointLightColorLoc3;




GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}
float lastX = 0, lastY = 0;
float deltaX;
float deltaY;
float pitch;
float yaw;
bool firstTimeInFrame = true;
float sensitivity = 0.05f;

void clampAngle(float& angle, float lower, float upper)
{
	angle = angle > upper ? upper : angle;
	angle = angle < lower ? lower : angle;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstTimeInFrame)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstTimeInFrame = false;
	}
	deltaX = (float)xpos - lastX;
	deltaY = (float)ypos - lastY;

	lastX = (float)xpos;
	lastY = (float)ypos;

	yaw -= deltaX * sensitivity;
	pitch -= deltaY * sensitivity;

	clampAngle(pitch, -70.0f, 89.0f);

	myCamera.rotate(pitch, yaw);
}
float angle = 0.0f;
float scaleFactor = 1.0f;
glm::vec3 translationVector = glm::vec3(0.0f, 0.0f, 0.0f);
bool isNight = false;

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	// Control scale
	if (pressedKeys[GLFW_KEY_Z]) {
		scaleFactor += 0.01f; // Increase scale
	}

	if (pressedKeys[GLFW_KEY_X]) {
		scaleFactor = glm::max(0.01f, scaleFactor - 0.01f); // Decrease scale, avoid negative or zero scale
	}

	// Control translation
	if (pressedKeys[GLFW_KEY_I]) {
		translationVector.z -= 0.1f; // Move scene forward
	}

	if (pressedKeys[GLFW_KEY_K]) {
		translationVector.z += 0.1f; // Move scene backward
	}

	if (pressedKeys[GLFW_KEY_J]) {
		translationVector.x -= 0.1f; // Move scene left
	}

	if (pressedKeys[GLFW_KEY_L]) {
		translationVector.x += 0.1f; // Move scene right
	}
	if (pressedKeys[GLFW_KEY_R]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	// Move camera down
	if (pressedKeys[GLFW_KEY_F]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_V]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_G]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_H]) {
		lightAngle += 1.0f;
	}
	if (pressedKeys[GLFW_KEY_T]) {
		isNight = !isNight;
		if (isNight) {
			lightColor = glm::vec3(0.0f, 0.0f, 0.0f); // Noapte
		}
		else {
			lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Zi
		}
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}
	

}


bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	newModel.LoadModel("objects/scena.obj", "objects/");
	barca.LoadModel("objects/Barca.obj", "objects/");
	lumina.LoadModel("objects/cube.obj");
	lumina.LoadModel("objects/quad.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
	depthShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));



	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));



	//coord lumina punctiorma
	pointLightLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight1");
	glUniform3fv(pointLightLoc1, 1, glm::value_ptr(pointLight1));

	//set point light color
	pointLightColorLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor1");
	glUniform3fv(pointLightColorLoc1, 1, glm::value_ptr(pointLightColor1));


	//coord lumina punctiorma
	pointLightLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight2");
	glUniform3fv(pointLightLoc2, 1, glm::value_ptr(pointLight2));

	//set point light color
	pointLightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor2");
	glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor2));


	//coord lumina punctiorma
	pointLightLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLight3");
	glUniform3fv(pointLightLoc3, 1, glm::value_ptr(pointLight3));

	//set point light color
	pointLightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor3");
	glUniform3fv(pointLightColorLoc3, 1, glm::value_ptr(pointLightColor3));



	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {

	glm::vec3 rotatedLightDir = glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir;
	glm::mat4 lightView = glm::lookAt(rotatedLightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 6.0f;
	glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;


	return lightSpaceTrMatrix;
}

float delta = 0;
float movementSpeed = 0.5; // units per second 
bool movingForward = true; // variabilă pentru a urmări direcția de mișcare

void updateDelta(double elapsedSeconds) {
	if (movingForward) {
		delta += movementSpeed * elapsedSeconds;
		if (delta > 5) { // dacă delta depășește 10, schimbă direcția
			movingForward = false;
		}
	}
	else {
		delta -= movementSpeed * elapsedSeconds;
		if (delta < -5) { // dacă delta scade sub 0, schimbă direcția
			movingForward = true;
		}
	}
}

double lastTimeStamp = glfwGetTime();


void moveBoat(gps::Shader shader)
{
	modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;

	glm::mat4 model2 = glm::mat4(1.0f); // Model matrix for the boat
	model2 = glm::translate(model2, glm::vec3(0, 0, delta));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	// Draw the boat
	barca.Draw(shader);
}
void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	moveBoat(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	newModel.Draw(shader);
	

}


void renderScene() {


	depthShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		glViewport(0, 0, retina_width, retina_height);

		//glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		myCustomShader.useShaderProgram();

		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		//initialize the model matrix
		model = glm::mat4(1.0f);

		// Apply translation
		model = glm::translate(model, translationVector);

		// Apply rotation
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		// Apply scaling
		model = glm::scale(model, glm::vec3(scaleFactor));

		modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));



		//initialize the view matrix
		glm::mat4 view = myCamera.getViewMatrix();
		//send matrix data to shader
		GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		// Bind the shadow map to texture unit 3
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		// Scene drawing
		drawObjects(myCustomShader, false);

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));


		model = lightRotation;
		model = glm::translate(model, 1.0f * (lightDir + glm::vec3(0.0f, 30.0f, 0.0f)));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		 lumina.Draw(lightShader);

		mySkyBox.Draw(skyboxShader, view, projection);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}
void initSkyBox()
{
	std::vector<const GLchar*> faces;
	
	faces.push_back("mountain/mountain_rt.tga");
	faces.push_back("mountain/mountain_lf.tga");
	faces.push_back("mountain/mountain_up.tga");
	faces.push_back("mountain/mountain_dn.tga");
	faces.push_back("mountain/mountain_bk.tga");
	faces.push_back("mountain/mountain_ft.tga");



	mySkyBox.Load(faces);
}
int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initSkyBox();
	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
