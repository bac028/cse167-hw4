#ifndef TERRAIN_H
#define TERRAIN_H

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <iostream>
#include "SceneGraph/SceneGraph.h"

class Terrain
{
public:
	Terrain();
	~Terrain();
	void draw(unsigned int shader);

protected:
	unsigned int texture;
	unsigned int terrainVAO, terrainVBO, terrainEBO;
	void Terrain::init(std::string objFilename, int objFormat);
};
#endif