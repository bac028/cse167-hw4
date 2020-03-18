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

struct ObjMaterial {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

// abstract node class
class Node {
public:
	float minX, minY, minZ, maxX, maxY, maxZ;

	virtual void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) = 0;
	virtual void update() = 0;
	virtual void setMoving(bool moving) = 0;
};

// concrete classes: Transform, Geometry, Group

class Transform : public Node {
protected:
	std::vector<Node*> children; // children pointers

	glm::vec3 rotationVector;
	float maxA, minA;
	bool animated = false;
	bool moving = false;
	int dir = 1;
	
public:

	float animationSpeed = 0.2f;

	float currentX, currentY, currentZ, currentScale, currentAngle;
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
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc);
	void update();
};

class Geometry : public Node {
protected:
	glm::mat4 initModel;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textures;
	std::vector<unsigned int> faces;

	ObjMaterial material;

	GLuint vao, vbo[2], ebo;

public:
	glm::mat4 currModel;

	float minX, maxX, minY, maxY, minZ, maxZ;

	Geometry();
	~Geometry();
	void init(std::string objFilename, int objFormat, ObjMaterial material);

	// overwrite node methods
	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc);
	void update() {};
	void setMoving(bool moving) {};
};

class TerrainGeometry : public Geometry {
protected:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textures;
	std::vector<unsigned int> faces;
	GLuint normalMap;
public:
	TerrainGeometry(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec3> textures, std::vector<unsigned int> faces);
	void TerrainGeometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc);
	~TerrainGeometry();
	//addTextures(terrain->m_hTex);
};

class RobotGeometry : public Geometry {
public:
	bool halo = false;
	RobotGeometry();
	~RobotGeometry();

	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc);
};

class Robot : public Transform {
protected:
	Transform* eye;
public:
	bool halo;
	RobotGeometry* headGlowGeometry;
	Robot(glm::mat4 M);
	~Robot();

	glm::vec3 getEyePosition() { 
		glm::mat4 globalPosition = M * eye->getModel();
		return glm::vec3(globalPosition[3].x, globalPosition[3].y, globalPosition[3].z); 
	};
	void SetGlow(bool halo);
	void draw();
};



class PlantGeometry : public Geometry {
protected:
	std::string LSystemString;
	float lineWidth = 5;
	float segmentLength = 2;
	float angle = 30;
	glm::vec3 position = glm::vec3(0, 10, 250);
	std::vector<float> lines;

public:
	PlantGeometry(glm::vec3 startingPosition);
	~PlantGeometry();
	void addLevel();
	void generatePoints();

	void draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc);

};

#endif