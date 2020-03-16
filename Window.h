#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <time.h>

#include "Object.h"
#include "PointCloud.h"
#include "Cloud.h"
#include "shader.h"
#include "Terrain.h"
#include "SceneGraph/SceneGraph.h"

class Window
{
public:
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void idleCallback();
	static void displayCallback(GLFWwindow*);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static glm::vec3 trackballMapping(double x, double y);
	static unsigned int loadCubemap(std::vector<std::string> faces);
	static glm::vec3 checkInSkybox(glm::vec4 point);
	static void handleMovement();

	// generate procedurally generated objects
	static void createScene();

	static void DrawScene();
};

#endif
