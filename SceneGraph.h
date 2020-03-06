#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

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

// abstract node class
class Node {
public:
	float minX, minY, minZ, maxX, maxY, maxZ;

	virtual void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc) = 0;
	virtual void update() = 0;
	virtual void setMoving(bool moving) = 0;
};

// concrete classes: Transform, Geometry, Group

class Transform : public Node {
private:
	std::vector<Node*> children; // children pointers

	float animationSpeed = 0.2f;

	glm::vec3 rotationVector;
	float maxA, minA;
	bool animated = false;
	bool moving = false;
	int dir = 1;

	float currentX, currentY, currentZ, currentScale, currentAngle;

public:
	glm::mat4 M; // transformation matrix
	Transform(glm::mat4 M);
	float minX, minY, minZ, maxX, maxY, maxZ;

	void addChild(Node* child);
	void removeChild();
	void setChildren(std::vector<Node*>* children);

	void addAnimation(glm::vec3 rotation, float maxAngle, float minAngle);

	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	void scale(float s);

	glm::mat4 getModel() { return M; }
	void setModel(glm::mat4 newModel) { M = newModel; }

	void setMoving(bool moving);

	// overwrite node methods
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc);
	void update();
};

class Geometry : public Node {
private:
	glm::mat4 initModel;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> faces;

	GLuint vao, vbo[2], ebo;

public:
	glm::mat4 currModel;

	float minX, maxX, minY, maxY, minZ, maxZ;

	Geometry();
	~Geometry();
	void init(std::string objFilename, int objFormat);

	// overwrite node methods
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc);
	void update() {};
	void setMoving(bool moving) {};
};

class Robot : public Transform {
private:
	Transform* eye;
public:
	Robot(glm::mat4 M);
	~Robot();

	glm::vec3 getEyePosition() { 
		glm::mat4 globalPosition = M * eye->getModel();
		return glm::vec3(globalPosition[3].x, globalPosition[3].y, globalPosition[3].z); 
	};
};

#endif
