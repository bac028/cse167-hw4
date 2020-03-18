#include "SceneGraph.h"

#include <algorithm>

using namespace std;

Robot::Robot(glm::mat4 M) : Transform(M) {

	ObjMaterial material;
	material.shininess = 100.0f;
	material.specular = glm::vec3(0.5f, 0.5f, 1.0f);
	material.diffuse = glm::vec3(0.05f, 0.4f, 0.7f);
	material.ambient = glm::vec3(0.0f, 0.1f, 0.2f);

	ObjMaterial bunnyMaterial;
	bunnyMaterial.shininess = 1000.0f;
	bunnyMaterial.specular = glm::vec3(0.55f, 0.55f, 0.6f);
	bunnyMaterial.diffuse = glm::vec3(0.55f, 0.55f, 0.6f);
	bunnyMaterial.ambient = glm::vec3(0.25f, 0.2f, 0.35f);

	// initializes geometries for body parts
	RobotGeometry* bodyGeometry = new RobotGeometry(), * headGeometry = new RobotGeometry(), * eyeGeometry = new RobotGeometry(), * limbGeometry = new RobotGeometry(), * antennaGeometry = new RobotGeometry();
	limbGeometry->init("limb_s", 0, material);
	eyeGeometry->init("eyeball_s", 0, material);
	headGeometry->init("bunny", 1, bunnyMaterial);
	bodyGeometry->init("body_s", 0, material);
	//antennaGeometry->init("antenna_s", 0);

	// adds each body part
	Transform* body = new Transform(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
	body->addChild(bodyGeometry);
	this->addChild(body);

	//Transform* headGlow = new Transform(glm::scale(glm::translate(glm::mat4(1), glm::vec3(0, 1.75, -0.1)), glm::vec3(1.1f,1.1f,1.1f)));
	//headGlow->addChild(headGeometry);
	//this->addChild(headGlow);
	

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

RobotGeometry::RobotGeometry() : Geometry::Geometry() {}


void RobotGeometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) {
	
	
	
	glClearStencil(0x4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0x0, 0x4);
	glStencilMask(0x4);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glDisable(GL_LIGHTING);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0x4, 0x4);
	glStencilMask(0x4);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
	glEnable(GL_BLEND);
	glColor4f(0.8, 0.8, 0.0, 0.3);
}
