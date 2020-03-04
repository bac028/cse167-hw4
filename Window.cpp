#include "Window.h"
#include <tgmath.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
	int width, height;
	std::string windowTitle("Minecraft");

	Geometry* testObject;
	Transform* objects;

	glm::vec3 eye(0, 5, 10);	// Camera position.
	glm::vec3 center(0, 5, -1); // The point we are looking at.
	glm::vec3 up(0, 1, 0);		// The up direction of the camera.

	// for camera rotation
	float yaw = -90.0f;
	float pitch = 0.0f;

	float fovy = 60;
	float near = 1;
	float far = 2000;
	glm::mat4 view = glm::lookAt(eye, center, up);  // View matrix, defined by eye, center and up.
	glm::mat4 projection;							// Projection matrix.

	GLuint program;
	GLuint skyboxProgram;

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
	glm::vec3 lightDir;
	glm::vec3 pointLightPos;

	// mouse control
	bool mousePressed_L = false;
	bool noLastPoint = true;
	bool mousePressed_R = false;
	glm::vec3 lastPoint;

	float mode = 0.0f;
	bool cameraControl = true; // camera trackball mapping

	// skybox files
	std::vector<std::string> faces = {
		"PalldioPalace_intern_right.jpg",
		"PalldioPalace_intern_left.jpg",
		"PalldioPalace_intern_top.jpg",
		"PalldioPalace_intern_base.jpg",
		"PalldioPalace_intern_front.jpg",
		"PalldioPalace_intern_back.jpg"
	};

	float skyboxVertices[] = {
		// positions          
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		 1000.0f, -1000.0f, -1000.0f,
		 1000.0f, -1000.0f, -1000.0f,
		 1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		 1000.0f, -1000.0f, -1000.0f,
		 1000.0f, -1000.0f,  1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		 1000.0f,  1000.0f, -1000.0f,
		 1000.0f, -1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		 1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		-1000.0f,  1000.0f, -1000.0f,
		 1000.0f,  1000.0f, -1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		 1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		 1000.0f, -1000.0f, -1000.0f,
		 1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		 1000.0f, -1000.0f,  1000.0f
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

	// Check the shader programs.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
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

	testObject = new Geometry();
	testObject->init("bunny", 1);
	objects = new Transform(glm::mat4(1));
	objects->addChild(testObject);

	return true;
}

void Window::cleanUp() {

	delete(testObject);
	delete(objects);

	// Delete the shader programs.
	glDeleteProgram(program);
	glDeleteProgram(skyboxProgram);
}

GLFWwindow* Window::createWindow(int width, int height) {
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

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
		std::cerr << "Failed to open GLFW window." << std::endl;
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
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

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
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, near, far);
}

void Window::idleCallback() {
	objects->update();
}

void Window::displayCallback(GLFWwindow* window) {
	// ------------ SKYBOX -------------
	glUseProgram(skyboxProgram);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glUniformMatrix4fv(skybox_projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(skybox_viewLoc, 1, GL_FALSE, glm::value_ptr(view));

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

	// ----------- OBJECTS -------------
	glUseProgram(program);

	//glm::mat4 model = currentObj->getModel();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniform1f(modeLoc, 0);

	objects->draw(glm::mat4(1), program, modelLoc);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
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
			// TODO
			case GLFW_KEY_W:
				glm::vec3 direction = center - eye;
				break;

			case GLFW_KEY_A:

				break;

			case GLFW_KEY_S:

				break;

			case GLFW_KEY_D:

				break;

			case GLFW_KEY_0:
				cameraControl = 1.0f ? cameraControl == 0.0f : 0.0f;
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

glm::vec3 Window::trackballMapping(double x, double y) {
	glm::vec3 v;
	float d;

	v.x = (2 * x - width) / width;
	v.y = (height - 2 * y) / height;
	v.z = 0;

	d = glm::length(v);
	d = d < 1.0 ? d : 1.0;
	v.z = sqrtf(1.001 - d * d);

	v = normalize(v);
	return v;
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) { 

	// for rotation 
	glm::vec3 currPoint = trackballMapping(xpos, ypos);
	if (noLastPoint) {
		lastPoint = currPoint;
		noLastPoint = false;
		return;
	}

	glm::vec3 direction = currPoint - lastPoint;
	float velocity = glm::length(direction);
	if (velocity > 0.0001) {
		glm::vec3 rotAxis = glm::cross(lastPoint, currPoint);
		float rot_angle = velocity * 50;
			
			
		if (!cameraControl && mousePressed_L) {
			glm::mat4 prevModel = objects->getModel();
			glm::mat4 newModel = glm::rotate(glm::mat4(1), glm::radians(rot_angle), glm::vec3(rotAxis.x, rotAxis.y, rotAxis.z));
			newModel = prevModel * newModel;
			objects->setModel(newModel);
		}

		if (cameraControl) {
			yaw += direction.x * 100;
			pitch += direction.y * 60;

			if (pitch > 89.9) pitch = 89.9;
			if (pitch < -89.9) pitch = -89.9;

			glm::vec3 directionCamera;
			directionCamera.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			directionCamera.y = sin(glm::radians(pitch));
			directionCamera.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			directionCamera = glm::normalize(directionCamera);

			up = glm::normalize(glm::cross(glm::normalize(glm::cross(directionCamera, glm::vec3(0, 1, 0))), directionCamera));

			center = eye + directionCamera;
				
			view = glm::lookAt(eye, center, up);
		}
	}
	lastPoint = currPoint;

	// for translation in xy plane
	if (mousePressed_R) {
		glm::vec3 currPoint = glm::vec3(xpos / 15, -1 * ypos / 15, 0);
		if (noLastPoint) {
			lastPoint = currPoint;
			noLastPoint = false;
			return;
		}

		glm::vec3 difference = currPoint - lastPoint;

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

unsigned int Window::loadCubemap(std::vector<std::string> faces) {
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
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
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