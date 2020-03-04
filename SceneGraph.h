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
	virtual void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc) = 0;
	virtual void update() = 0;
};

// concrete classes: Transform, Geometry, Group

class Transform : public Node {
private:
	glm::mat4 M; // transformation matrix
	std::vector<Node*> children; // children pointers

	glm::vec3 rotationVector;
	float maxA, minA;
	bool animated = false;
	int dir = 1;

	float currentX, currentY, currentZ, currentScale, currentAngle;

public:
	Transform(glm::mat4 M);

	void addChild(Node* child);
	void removeChild();
	void setChildren(std::vector<Node*>* children);

	void addAnimation(glm::vec3 rotation, float maxAngle, float minAngle);

	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	void scale(float s);

	glm::mat4 getModel() { return M; }
	void setModel(glm::mat4 newModel) { M = newModel; }

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

	//float currentX, currentY, currentZ, currentScale, currentAngle;

public:
	glm::mat4 currModel;

	Geometry();
	~Geometry();
	void init(std::string objFilename, int objFormat);

	// overwrite node methods
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc);
	void update();
};

class BezierCurve : public Geometry {
private:
	float *points;
	std::vector<glm::vec3> curvePoints;
	int size = 4 * 3 * 5;
	int N = 150;

	glm::mat4 control;

	glm::mat4 B = glm::mat4(glm::vec4(-1.0, 3.0, -3.0, 1.0),
		glm::vec4(3.0, -6.0, 3.0, 0),
		glm::vec4(-3.0, 3.0, 0, 0),
		glm::vec4(1, 0, 0, 0));
	GLuint vao, vbo;

public:
	void init(float* points);
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc);
	glm::vec4 getPoint(float t);
	void update() {};
};

#endif
