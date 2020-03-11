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

void Transform::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) {
	for (int i = 0; i < children.size(); i++) {
		children[i]->draw(C*M, shaderProgram, modelLoc, ambientLoc, diffuseLoc, specularLoc, shininessLoc);
	}
}

void Transform::update() {
	for (int i = 0; i < children.size(); i++) {
		children[i]->update();
	}

	if (animated && moving) {
		if (currentAngle > maxA) dir *= -1;
		if (currentAngle < minA) dir *= -1;
		M = glm::rotate(M, glm::radians(dir * animationSpeed), rotationVector);
		currentAngle += dir * animationSpeed;
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

void Transform::setMoving(bool moving) { 
	this->moving = moving;
	for (int i = 0; i < children.size(); i++) {
		children[i]->setMoving(moving);
	}
}

