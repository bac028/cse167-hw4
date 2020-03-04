#ifndef _PointCloud_H_
#define _PointCloud_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>

#include "Object.h"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class PointCloud : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> faces;

	Material material;

	GLuint vao, vbo[2], ebo;

	glm::vec3 rotationMatrix;
	glm::vec3 scaleMatrix;

	float currentX, currentY, currentZ, currentScale, currentAngle;
	float xOffset;

	bool spinning = false;

public:
	PointCloud(std::string objFilename, Material material, glm::vec3 spinMatrix);
	~PointCloud();

	void draw();
	void update();

	void setSpinDirection(float x, float y, float z);

	void spin(float deg);
	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	void scale(float scale);

	void resetPosition();
	void resetRotation();
	void resetScale();

	std::vector<glm::vec3> getPoints() { return points; };
	std::vector<glm::vec3> getNormals() { return normals; };
	Material getMaterial() { return material; };
	void setModel(glm::mat4 newModel) { model = newModel; };
};

#endif