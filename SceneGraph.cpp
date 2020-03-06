#include "SceneGraph.h"

#include <algorithm>

using namespace std;

Transform::Transform(glm::mat4 M) {
	this->M = M;

	maxA = 0;
	minA = 0;
	minX = 0;
	minY = 0;
	minZ = 0;
	maxX = 0;
	maxY = 0;
	maxZ = 0;
	rotationVector = glm::vec3(0, 0, 0);
	currentX = 0;
	currentY = 0;
	currentZ = 0;
	currentScale = 1;
	currentAngle = 0;
}

void Transform::setChildren(vector<Node*>* children) {
	this->children = *children;
}

void Transform::addChild(Node* child) {
	children.push_back(child);
	if (child->maxX > maxX) maxX = child->maxX;
	if (child->maxY > maxY) maxY = child->maxY;
	if (child->maxZ > maxZ) maxZ = child->maxZ;
	if (child->minX < minX) minX = child->minX;
	if (child->minY < minY) minY = child->minY;
	if (child->minZ < minZ) minZ = child->minZ;
}

void Transform::removeChild() {
	children.pop_back();
}

void Transform::addAnimation(glm::vec3 rotation, float maxAngle, float minAngle) {
	animated = true;
	rotationVector = rotation;
	maxA = maxAngle;
	minA = minAngle;
}

void Transform::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc) {
	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(C*M, shaderProgram, modelLoc);
	}
}

void Transform::update() {
	for (int i = 0; i < children.size(); i++) {
		children[i]->update();
	}

	if (animated) {
		if (currentAngle > maxA) dir *= -1;
		if (currentAngle < minA) dir *= -1;
		M = glm::rotate(M, glm::radians(dir * 0.1f), rotationVector);
		currentAngle += dir * 0.1f;
	}
}

void Transform::translate(float x, float y, float z) {
	M = glm::translate(glm::mat4(1), glm::vec3(x, y, z)) * M;
	currentX += x;
	currentY += y;
	currentZ += z;
}

void Transform::scale(float s) {
	M = glm::scale(M, glm::vec3(s, s, s));
}

Robot::Robot(glm::mat4 M) : Transform(M) {
	
	// initializes geometries for body parts
	Geometry* bodyGeometry = new Geometry(), * headGeometry = new Geometry(), * eyeGeometry = new Geometry(), * limbGeometry = new Geometry(), * antennaGeometry = new Geometry();
	bodyGeometry->init("body_s", 0);
	headGeometry->init("bunny", 1);
	eyeGeometry->init("eyeball_s", 0);
	limbGeometry->init("limb_s", 0);
	//antennaGeometry->init("antenna_s", 0);

	// adds each body part
	Transform* body = new Transform(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
	body->addChild(bodyGeometry);
	this->addChild(body);

	Transform* head = new Transform(glm::translate(glm::mat4(1), glm::vec3(0, 1.75, 0)));
	head->addChild(headGeometry);
	this->addChild(head);

	Transform* eye = new Transform(glm::translate(glm::mat4(1), glm::vec3(0, 0.75, 1.0)));
	eye->addChild(eyeGeometry);
	this->addChild(eye);
	this->eye = eye;

	Transform* rightArm = new Transform(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(1.2, 0.25, 0)), glm::radians(33.0f), glm::vec3(0, 0, 1)));
	rightArm->addChild(limbGeometry);
	rightArm->addAnimation(glm::vec3(-1, 0, 0), 45.0f, -45.0f);
	this->addChild(rightArm);

	Transform* leftArm = new Transform(glm::rotate(glm::translate(glm::mat4(1), glm::vec3(-1.2, 0.25, 0)), glm::radians(-33.0f), glm::vec3(0, 0, 1)));
	leftArm->addChild(limbGeometry);
	leftArm->addAnimation(glm::vec3(1, 0, 0), 45.0f, -45.0f);
	this->addChild(leftArm);

	Transform* leftLeg = new Transform(glm::translate(glm::mat4(1), glm::vec3(-0.5, -1.2, 0)));
	leftLeg->addChild(limbGeometry);
	leftLeg->addAnimation(glm::vec3(-1, 0, 0), -45.0f, 45.0f);
	this->addChild(leftLeg);

	Transform* rightLeg = new Transform(glm::translate(glm::mat4(1), glm::vec3(0.5, -1.2, 0)));
	rightLeg->addChild(limbGeometry);
	rightLeg->addAnimation(glm::vec3(1, 0, 0), 45.0f, -45.0f);
	this->addChild(rightLeg);
}

Geometry::Geometry() {}

Geometry::~Geometry() {
	// Delete the VAO, VBO, and EBO
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

// load file
void Geometry::init(string objFilename, int objFormat) {

	int type;
	float x, y, z, r, g, b;
	float maxX = -10000.0f, maxY = -10000.0f, maxZ = -10000.0f;
	float minX = 10000.0f, minY = 10000.0f, minZ = 10000.0f;

	vector<glm::vec3> input_vertices, input_normals;
	vector<int> vertex_indices, normal_indices;

	std::cout << "Loading " << objFilename << "..." << endl;

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
				input_vertices.push_back(glm::vec3(x, y, z));

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
				input_normals.push_back(glm::vec3(x, y, z));
			}
		}

		else if (type == 'f') {
			type = fgetc(fp);

			// faces
			if (type == ' ') {

				if (objFormat == 0) {
					int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
					fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

					vertex_indices.push_back(v1 - 1);
					vertex_indices.push_back(v2 - 1);
					vertex_indices.push_back(v3 - 1);

					normal_indices.push_back(vn1 - 1);
					normal_indices.push_back(vn2 - 1);
					normal_indices.push_back(vn3 - 1);
				}

				else if (objFormat == 1) {
					unsigned int t1, t2, t3, t1r, t2r, t3r;
					fscanf(fp, "%d//%d %d//%d %d//%d", &t1, &t1r, &t2, &t2r, &t3, &t3r);
					faces.push_back(t1 - 1);
					faces.push_back(t2 - 1);
					faces.push_back(t3 - 1);
				}
			}
		}
	}

	std::cout << "input lengths: " << input_vertices.size() << ", " << input_normals.size() << endl;

	fclose(fp);

	if (objFormat == 0) {
		for (unsigned int i = 0; i < vertex_indices.size(); i++) {
			vertices.push_back(input_vertices[vertex_indices[i]]);
			normals.push_back(input_normals[normal_indices[i]]);
			faces.push_back(i);
		}
	}

	else if (objFormat == 1) {
		vertices = input_vertices;
		normals = input_normals;
	}

	std::cout << "vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << faces.size() << std::endl;

	float centerX = (float)((double)maxX + minX) / 2.0;
	float centerY = (float)((double)maxY + minY) / 2.0;
	float centerZ = (float)((double)maxZ + minZ) / 2.0;

	this->minX = minX;
	this->minY = minY;
	this->minZ = minZ;
	this->maxX = maxX;
	this->maxY = maxY;
	this->maxZ = maxZ;

	float maxDist = max(max(centerX - minX, centerY - minY), centerZ - minZ);

	// center points
	for (int i = 0; i < vertices.size(); i++) {
		vertices.at(i) = glm::vec3(vertices.at(i).x - centerX, vertices.at(i).y - centerY, vertices.at(i).z - centerZ);
	}

	// Set the model matrix to an identity matrix. 
	initModel = glm::mat4(1);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	// FACES:
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
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

// draw object
void Geometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc) {

	currModel = C * initModel;

	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // unbind from ebo

	//glUseProgram(shaderProgram);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currModel));

	// draws triangles
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind from ebo
}

void Geometry::update() {

}

