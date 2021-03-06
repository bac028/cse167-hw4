﻿#include "Window.h"
#include <tgmath.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

namespace {
	int width, height;
	string windowTitle("MinecraftBlox");

	Robot* playerObject;
	Transform* objects;
	//Terrain* terrain;

	vec3 playerCenter(0, 0, 0);
	vec3 eye(0, 1, 1.2);		// Camera position.
	vec3 center(0, 0, 10);		// The point we are looking at.
	vec3 up(0, 1, 0);			// The up direction of the camera.

	// movement flags
	bool holdingW = false, holdingA = false, holdingS = false, holdingD = false, sprinting = false;

	// for camera rotation
	float objYaw = -90.0f;
	float objPitch = 0.0f;

	float fovy = 60;
	float near_a = 1;
	float far_a = 2000;
	mat4 view = lookAt(eye, center, up);  // View matrix, defined by eye, center and up.
	mat4 projection;							// Projection matrix.

	GLuint program;
	GLuint skyboxProgram;
	GLuint terrainProgram;

	GLuint projectionLoc;	// Location of projection in shader.
	GLuint viewLoc;			// Location of view in shader.
	GLuint modelLoc;		// Location of model in shader.
	GLuint colorLoc;		// Location of color in shader.
	GLuint sphereColorLoc;
	GLuint modeLoc;
	GLuint dirEnabledLoc;
	GLuint pointEnabledLoc;
	GLuint ambientLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
	GLuint shininessLoc;

	GLuint pointLightPosLoc;
	GLuint pointLightColorLoc;
	GLuint directionLightDirLoc;
	GLuint directionLightColorLoc;
	GLuint viewPosLoc;

	// lights
	vec3 lightDir;
	vec3 pointLightPos;

	// mouse control
	bool mousePressed_L = false;
	bool noLastPoint = true;
	bool mousePressed_R = false;

	enum CameraType {
		FIRST_PERSON,
		THIRD_PERSON,
		FREE_CAMERA
	};
	CameraType cameraType = FIRST_PERSON;

	vec3 lastPoint;

	enum ShaderMode {
		NORMAL,
		PHONG,
		TOON
	};
	ShaderMode mode = TOON; // shading mode

	bool cameraControl = true; // camera trackball mapping

	// skybox files
	vector<string> faces = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg"
	};

	float skyboxVertices[] = {
		// positions          
			-100.0f,  100.0f, -100.0f,
			-100.0f, -100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,
			100.0f,  100.0f, -100.0f,
			-100.0f,  100.0f, -100.0f,

			-100.0f, -100.0f,  100.0f,
			-100.0f, -100.0f, -100.0f,
			-100.0f,  100.0f, -100.0f,
			-100.0f,  100.0f, -100.0f,
			-100.0f,  100.0f,  100.0f,
			-100.0f, -100.0f,  100.0f,

			100.0f, -100.0f, -100.0f,
			100.0f, -100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,
			100.0f,  100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,

			-100.0f, -100.0f,  100.0f,
			-100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,
			100.0f, -100.0f,  100.0f,
			-100.0f, -100.0f,  100.0f,

			-100.0f,  100.0f, -100.0f,
			100.0f,  100.0f, -100.0f,
			100.0f,  100.0f,  100.0f,
			100.0f,  100.0f,  100.0f,
			-100.0f,  100.0f,  100.0f,
			-100.0f,  100.0f, -100.0f,
			-100.0f, -100.0f, -100.0f,
			-100.0f, -100.0f,  100.0f,
			100.0f, -100.0f, -100.0f,
			100.0f, -100.0f, -100.0f,
			-100.0f, -100.0f,  100.0f,
			100.0f, -100.0f,  100.0f
	};

	unsigned int cubemapTexture;

	GLuint skybox_projectionLoc;	// Location of projection in shader.
	GLuint skybox_viewLoc;			// Location of view in shader.
	GLuint skybox_skyboxLoc;		// Location of view in shader.
	unsigned int skyboxVAO, skyboxVBO;
};

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	skyboxProgram = LoadShaders("shaders/skybox_shader.vert", "shaders/skybox_shader.frag");
	terrainProgram = LoadShaders("shaders/texture_shader.vert", "shaders/texture_shader.frag");

	// Check the shader programs.
	if (!program)
	{
		cerr << "Failed to initialize shader program" << endl;
		return false;
	}

	// Activate the shader program.
	glUseProgram(program);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "normal");
	modeLoc = glGetUniformLocation(program, "mode");
	sphereColorLoc = glGetUniformLocation(program, "color");

	ambientLoc = glGetUniformLocation(program, "materialAmbient");
	diffuseLoc = glGetUniformLocation(program, "materialDiffuse");
	specularLoc = glGetUniformLocation(program, "materialSpecular");
	shininessLoc = glGetUniformLocation(program, "materialShininess");

	pointLightPosLoc = glGetUniformLocation(program, "pointLightPosition");
	pointLightColorLoc = glGetUniformLocation(program, "pointLightColor");
	directionLightDirLoc = glGetUniformLocation(program, "dirLightDirection");
	directionLightColorLoc = glGetUniformLocation(program, "dirLightColor");

	viewPosLoc = glGetUniformLocation(program, "viewPos");

	// skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// only see inside of box
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);

	// load skybox faces
	cubemapTexture = loadCubemap(faces);

	skybox_projectionLoc = glGetUniformLocation(skyboxProgram, "projection");
	skybox_viewLoc = glGetUniformLocation(skyboxProgram, "view");
	skybox_skyboxLoc = glGetUniformLocation(skyboxProgram, "skybox");

	return true;
}

bool Window::initializeObjects() {

	playerObject = new Robot(translate(mat4(1), playerCenter));

	objects = new Transform(mat4(1));
	objects->addChild(playerObject);
	//terrain = new Terrain();

	return true;
}

void Window::cleanUp() {

	delete(objects);
	//delete(terrain);

	// Delete the shader programs.
	glDeleteProgram(program);
	glDeleteProgram(skyboxProgram);
}

GLFWwindow* Window::createWindow(int width, int height) {
	// Initialize GLFW.
	if (!glfwInit())
	{
		cerr << "Failed to initialize GLFW" << endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		cerr << "Failed to open GLFW window." << endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		cerr << "Failed to initialize GLEW" << endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int w, int h) {
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
	width = w;
	height = h;

	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = perspective(radians(fovy),
		(float)width / (float)height, near_a, far_a);
}

void Window::idleCallback() {
	objects->update();
}

void Window::displayCallback(GLFWwindow* window) {

	handleMovement();

	// ------------ SKYBOX -------------
	glUseProgram(skyboxProgram);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glUniformMatrix4fv(skybox_projectionLoc, 1, GL_FALSE, value_ptr(projection));
	glUniformMatrix4fv(skybox_viewLoc, 1, GL_FALSE, value_ptr(view));

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// no padded bytes
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// bilinear interpolation
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// clamp to edge to hide skybox edges
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default


	// ----------- TERRAIN -------------
	//glUseProgram(terrainProgram);
	
	//unsigned int terrain_projectionLoc = glGetUniformLocation(terrainProgram, "projection");
	//unsigned int terrain_viewLoc = glGetUniformLocation(terrainProgram, "view");
	//unsigned int terrain_textureLoc = glGetUniformLocation(terrainProgram, "skybox");
	
	//glUniformMatrix4fv(terrain_projectionLoc, 1, GL_FALSE, value_ptr(projection));
	//glUniformMatrix4fv(terrain_viewLoc, 1, GL_FALSE, value_ptr(view));

	//terrain->draw(terrainProgram);

	// ----------- OBJECTS -------------
	glUseProgram(program);

	//mat4 model = currentObj->getModel();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniform1f(modeLoc, (float) mode);

	// direction lighting
	glUniform3fv(directionLightDirLoc, 1, glm::value_ptr(glm::vec3(0.0f, -1.0f, -15.0f)));
	glUniform3fv(directionLightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

	glUniform3fv(viewPosLoc, 1, glm::value_ptr(eye));

	glUniform1f(dirEnabledLoc, 1);
	glUniform1f(pointEnabledLoc, 0);
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

	objects->draw(mat4(1), program, modelLoc, ambientLoc, diffuseLoc, specularLoc, shininessLoc);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// check for key press or release
	switch (key) {
		case GLFW_KEY_W:
			holdingW = action != GLFW_RELEASE;
			break;
		case GLFW_KEY_A:
			holdingA = action != GLFW_RELEASE;
			break;

		case GLFW_KEY_S:
			holdingS = action != GLFW_RELEASE;
			break;

		case GLFW_KEY_D:
			holdingD = action != GLFW_RELEASE;
			break;

		case GLFW_KEY_LEFT_CONTROL:
			sprinting = action != GLFW_RELEASE;
			break;
	}

	playerObject->setMoving((holdingW || holdingA || holdingS || holdingD) && cameraType != FREE_CAMERA);
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		// Uppercase key presses (shift held down + key press)
		if (mods == GLFW_MOD_SHIFT) {
			switch (key) {

			default:
				break;
			}
		}

		else {
			// Deals with lowercase key presses
			switch (key)
			{
				case GLFW_KEY_0:
					cameraControl = cameraControl == 0.0f ? 1.0f : 0.0f;
					break;

				case GLFW_KEY_F:
					cameraType = FREE_CAMERA;
					break;

				case GLFW_KEY_1:
					cameraType = FIRST_PERSON;
					break;

				case GLFW_KEY_3:
					cameraType = THIRD_PERSON;
					break;

				case GLFW_KEY_ESCAPE:
					glfwSetWindowShouldClose(window, GL_TRUE);
					break;

				default:
					break;
			}
		}
	}
}

vec3 Window::trackballMapping(double x, double y) {
	vec3 v;
	double d;

	v.x = (2 * x - width) / width;
	v.y = (height - 2 * y) / height;
	v.z = 0;

	d = length(v);
	d = d < 1.0 ? d : 1.0;
	v.z = sqrtf(1.001 - d * d);

	v = normalize(v);
	return v;
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) { 

	// for rotation 
	vec3 currPoint = trackballMapping(xpos, ypos);
	if (noLastPoint) {
		lastPoint = currPoint;
		noLastPoint = false;
		return;
	}

	vec3 pointDirection = currPoint - lastPoint;
	float velocity = length(vec3(pointDirection.x, 0, 0));
	if (velocity > 0.0001) {
		vec3 rotAxis = cross(lastPoint, currPoint);
		float rot_angle = velocity * 100.0f;
			
		if (!cameraControl && mousePressed_L) {
			mat4 prevModel = objects->getModel();
			mat4 newModel = rotate(mat4(1), radians(rot_angle), vec3(rotAxis.x, rotAxis.y, rotAxis.z));
			newModel = prevModel * newModel;
			objects->setModel(newModel);
		}

		if (cameraControl) {
			objYaw += pointDirection.x * 100;
			objPitch += pointDirection.y * 100;

			if (objPitch > 89.9) objPitch = 89.9;
			if (objPitch < -89.9) objPitch = -89.9;

			if (cameraType == FIRST_PERSON || cameraType == THIRD_PERSON) {
				objPitch = objPitch > 44.9 ? 44.9 : objPitch;
				objPitch = objPitch < -44.9 ? -44.9 : objPitch;
			}

			vec3 directionCamera;
			directionCamera.x = cos(radians(objYaw)) * cos(radians(objPitch));
			directionCamera.y = sin(radians(objPitch));
			directionCamera.z = sin(radians(objYaw)) * cos(radians(objPitch));
			directionCamera = normalize(directionCamera);

			up = normalize(cross(normalize(cross(directionCamera, vec3(0, 1, 0))), directionCamera));

			if (cameraType == FREE_CAMERA) center = eye + directionCamera;
			
			if (cameraType == FIRST_PERSON || cameraType == THIRD_PERSON) {
				mat4 prevModel = playerObject->getModel();
				mat4 newModel = rotate(mat4(1), radians(-rot_angle), vec3(0, rotAxis.y, 0));
				newModel = prevModel * newModel;
				playerObject->setModel(newModel);
				eye = playerObject->getEyePosition();
				center = eye + vec3(-directionCamera.x, directionCamera.y, -directionCamera.z);
				if (cameraType == THIRD_PERSON) {
					vec4 newEye = translate(mat4(1.0f), 15.0f * vec3(directionCamera.x, -directionCamera.y, directionCamera.z)) * vec4(eye, 1.0f);
					eye = vec3(newEye.x, newEye.y, newEye.z);
				}
			}
			
			view = lookAt(eye, center, up);
		}
	}
	glfwSetCursorPos(window, height/2, width/2);
	noLastPoint = 1;
	lastPoint = vec3(0, 0, 0); //currPoint;

	// for translation in xy plane
	if (mousePressed_R) {
		vec3 currPoint = vec3(xpos / 15, -1 * ypos / 15, 0);
		if (noLastPoint) {
			lastPoint = currPoint;
			noLastPoint = false;
			return;
		}

		vec3 difference = currPoint - lastPoint;

		objects->translate(difference.x, difference.y, 0);

		lastPoint = currPoint;
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		mousePressed_R = true;
		noLastPoint = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) mousePressed_R = false;
	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mousePressed_L = true;
		noLastPoint = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT) mousePressed_L = false;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}

unsigned int Window::loadCubemap(vector<string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			cout << "Cubemap tex failed to load at path: " << faces[i] << endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

vec3 Window::checkInSkybox(vec4 point) {
	if (cameraType == FREE_CAMERA) {
		if (point.x > 95 || point.x < -95) {
			point.x = eye.x;
		}
		if (point.y > 95 || point.y < -95) {
			point.y = eye.y;
		}
		if (point.z > 95 || point.z < -95) {
			point.z = eye.z;
		}
	} 

	return vec3(point.x, point.y, point.z);
}

bool checkCollision(vec3 objOne, vec3 objTwo){
	if(playerObject->maxX >= 100 && playerObject->minX <= -100 &&
	playerObject->maxY >= 100 && playerObject->minY <= -100 &&
	playerObject->maxZ >= 100 && playerObject->minZ <= -100 ){
		//currentPosition = previews
	}
	return false;
}

void Window::handleMovement() {

	float speedModifier = sprinting ? 3.00f : 1.0f;
	vec3 direction = 0.01f * speedModifier * normalize((vec3(center.x, 0, center.z) - vec3(eye.x, 0, eye.z)));
	vec4 newCenter;
	vec4 newEye;

	// forward
	if (holdingW) {
		vec3 translation = direction;
		newEye = translate(mat4(1), direction) * vec4(eye, 1);
		eye = checkInSkybox(newEye);
		newCenter = translate(mat4(1), direction) * vec4(center, 1);
		center = vec3(newCenter.x, newCenter.y, newCenter.z);
		if (cameraType != FREE_CAMERA)
			playerObject->translate(translation.x, translation.y, translation.z);
		view = lookAt(eye, center, up);
	}

	// backward
	else if (holdingS) {
		vec3 translation = -direction;
		newEye = translate(mat4(1), -direction) * vec4(eye, 1);
		eye = checkInSkybox(newEye);
		newCenter = translate(mat4(1), -direction) * vec4(center, 1);
		center = vec3(newCenter.x, newCenter.y, newCenter.z);
		if (cameraType != FREE_CAMERA)
			playerObject->translate(translation.x, translation.y, translation.z);
		view = lookAt(eye, center, up);
	}

	// right
	if (holdingA) {
		vec3 translation = cross(-direction, up);
		newEye = translate(mat4(1), cross(-direction, up)) * vec4(eye, 1);
		eye = checkInSkybox(newEye);
		newCenter = translate(mat4(1), cross(-direction, up)) * vec4(center, 1);
		center = vec3(newCenter.x, newCenter.y, newCenter.z);
		if (cameraType != FREE_CAMERA)
			playerObject->translate(translation.x, translation.y, translation.z);
		view = lookAt(eye, center, up);
	}

	// left
	else if (holdingD) {
		vec3 translation = cross(direction, up);
		newEye = translate(mat4(1), cross(direction, up)) * vec4(eye, 1);
		eye = checkInSkybox(newEye);
		newCenter = translate(mat4(1), cross(direction, up)) * vec4(center, 1);
		center = vec3(newCenter.x, newCenter.y, newCenter.z);
		if (cameraType != FREE_CAMERA)
			playerObject->translate(translation.x, translation.y, translation.z);
		view = lookAt(eye, center, up);
	}
}