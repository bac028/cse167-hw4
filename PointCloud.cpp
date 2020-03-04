#include "PointCloud.h"
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

PointCloud::PointCloud(std::string objFilename, Material objMaterial, glm::vec3 spinMatrix) {
	int type;
	float x, y, z, r, g, b;
	unsigned int t1, t2, t3, t1r, t2r, t3r;
	float maxX = -10000.0f, maxY = -10000.0f, maxZ = -10000.0f;
	float minX = 10000.0f, minY = 10000.0f, minZ = 10000.0f;

	material = objMaterial;

	FILE* fp = fopen((objFilename + ".obj").c_str(), "rb");
	if (!fp) {
		cout << "error loading file" << endl;
		exit(-1);
	}

	// scans file
	while ((type = fgetc(fp)) != EOF) {
		// v and vn types
		if (type == 'v') {
			type = fgetc(fp);

			// vector type
			if (type == ' ') {
				fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				points.push_back(glm::vec3(x, y, z));

				// gets max/min
				if (x > maxX) maxX = x;
				if (x < minX) minX = x;
				if (y > maxY) maxY = y;
				if (y < minY) minY = y;
				if (z > maxZ) maxZ = z;
				if (z < minZ) minZ = z;
			}

			// vector normal
			else if (type == 'n') {
				fscanf(fp, "%f %f %f", &x, &y, &z);
				normals.push_back(glm::vec3(x, y, z));
			}
		}

		else if (type == 'f') {
			type = fgetc(fp);

			// faces
			if (type == ' ') {
				fscanf(fp, "%d//%d %d//%d %d//%d", &t1, &t1r, &t2, &t2r, &t3, &t3r);
				faces.push_back(glm::ivec3(t1 - 1, t2 - 1, t3 - 1));
			}
		}
	}

	fclose(fp);

	// calculate center
	float centerX = (float)((double)maxX + minX) / 2.0;
	float centerY = (float)((double)maxY + minY) / 2.0;
	float centerZ = (float)((double)maxZ + minZ) / 2.0;

	// centers points
	//for (int i = 0; i < points.size(); i++) {
	//	points.at(i) = glm::vec3(points.at(i).x - centerX, points.at(i).y - centerY, points.at(i).z - centerZ);
	//}

	// scales points
	//float maxDist = max(max(centerX - minX, centerY - minY), centerZ - minZ);
	//scaleMatrix = glm::vec3(1.0f / maxDist, 1.0f / maxDist, 1.0f / maxDist);
	//model = glm::scale(glm::mat4(1.0f), scaleMatrix);

	model = glm::mat4(1.0f);

	// initial position values
	xOffset = 0;
	currentX = 0;
	currentY = 0;
	currentZ = 0;
	currentScale = 1;
	currentAngle = 0;
	rotationMatrix = glm::vec3(0, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	// VERTICES:
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * faces.size(), faces.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// COLOR:
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

PointCloud::~PointCloud()
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void PointCloud::draw() {

	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// draws PointCloud
	glDrawElements(GL_TRIANGLES, 3 * faces.size(), GL_UNSIGNED_INT, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PointCloud::update() {
	if (spinning)
		spin(0.1f);
}

void PointCloud::spin(float deg) {
	model = glm::rotate(model, glm::radians(deg), rotationMatrix);
	currentAngle += deg;
}

void PointCloud::setSpinDirection(float x, float y, float z) {
	rotationMatrix = glm::vec3(x, y, z);
	spinning = x != 0 && y != 0 && z != 0;
}

void PointCloud::translate(float x, float y, float z) {
	model = glm::translate(glm::mat4(1), glm::vec3(x, y, z)) * model;
	currentX += x;
	currentY += y;
	currentZ += z;
}

void PointCloud::rotate(float x, float y, float z) {
	model = glm::rotate(model, glm::radians(1.0f), glm::vec3(x, y, z));
}

void PointCloud::scale(float s) {
	model = glm::scale(model, glm::vec3(s, s, s));
	currentScale *= s;
}

void PointCloud::resetPosition() {
	model = glm::translate(glm::mat4(1), glm::vec3(-1 * currentX, -1 * currentY, -1 * currentZ)) * model;
	currentX = 0;
	currentY = 0;
	currentZ = 0;
}

void PointCloud::resetRotation() {
	model = glm::rotate(model, glm::radians(-1 * currentAngle), rotationMatrix);
	currentAngle = 0;
}

void PointCloud::resetScale() {
	model = glm::scale(model, glm::vec3(1 / currentScale, 1 / currentScale, 1 / currentScale));
	currentScale = 1;
}
