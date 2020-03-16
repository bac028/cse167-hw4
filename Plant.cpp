#include "Plant.h"
#include <GL\freeglut_std.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <Timeapi.h>;
#include <time.h>

const float PI = 3.14, DEPTH = 3; // 6
// Start and end of camera movement
const int ENDX = 10, STARTX = -500;
// Angle of branches, and depth of tree
float ANGLE = 20, depth = 0;
std::vector<std::string>* trees = new std::vector<std::string>();


double lastTime = 0, elapsedTime = 0, lastElapsedTime = 0;

bool cam = false;

float eyeX, eyeY, eyeZ, lookX, lookY, lookZ,
upX, upY, upZ, fieldOfView, length = 0.001, num = 0,
incr = 0.1;

float lineWidth = 20;
// L-System
std::string str = "X";

void Plant::push() {
	glPushMatrix();
	if (lineWidth > 0)
		lineWidth -= 1;


}

void Plant::pop() {
	glPopMatrix();
	lineWidth += 1;

}

void Plant::rotL() {
	glRotatef(ANGLE, 1, 0, 0);
	glRotatef(ANGLE * 4, 0, 1, 0);
	glRotatef(ANGLE, 0, 0, 1);
}
void Plant::rotR() {
	glRotatef(-ANGLE, 1, 0, 0);
	glRotatef(ANGLE * 4, 0, 1, 0);
	glRotatef(-ANGLE, 0, 0, 1);
}
void Plant::leaf() {
	glPushAttrib(GL_LIGHTING_BIT);//saves current lighting stuff
		//glColor3f(0.50, 1.0, 0.0);
	GLfloat ambient[4] = { 0.50, 1.0, 0.0 };    // ambient reflection
	GLfloat specular[4] = { 0.55, 1.0, 0.0 };   // specular reflection
	GLfloat diffuse[4] = { 0.50, 0.9, 0.0 };   // diffuse reflection


	// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	// set the specular reflection for the object      
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	// set the size of the specular highlights
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);

	//glutSolidCube(depth+1);
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.2, 0, 0.3);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, -0.3);
	glVertex3f(0, 1, 0);
	glEnd();
	glPopAttrib();
}

void Plant::drawLine() {
	glPushAttrib(GL_LIGHTING_BIT);//saves current lighting stuff


	//glColor3f(0.55, 0.27, 0.07);
	GLfloat ambient[4] = { 0.55, 0.27, 0.07 };    // ambient reflection
	GLfloat specular[4] = { 0.55, 0.27, 0.07 };   // specular reflection
	GLfloat diffuse[4] = { 0.55, 0.27, 0.07 };   // diffuse reflection

	// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	// set the specular reflection for the object      
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);
	glEnd();

	glTranslatef(0, length, 0);
	glPopAttrib();
}


void Plant::draw() {

	std::string ch = "";
	std::string LSystem = trees->at(depth);
	for (int i = 0; i < LSystem.length(); i++) {
		ch = LSystem.at(i);

		if (ch.compare("D") == 0 || ch.compare("X") == 0) {
			drawLine();
		}
		else if (ch.compare("[") == 0) {
			push();
		}
		else if (ch.compare("]") == 0) {
			pop();
		}
		else if (ch.compare("V") == 0) {
			leaf();
		}
		else if (ch.compare("R") == 0) {
			rotR();
		}
		else if (ch.compare("L") == 0) {
			rotL();
		}
	}
}

void Plant::expand(float num) {
	std::string ch = "";

	for (int i = 0; i < str.length(); i++) {
		ch = str.at(i);

		if (ch.compare("D") == 0) {
			str.replace(i, 1, "DD");
			i = i + 1;
		}
		else if (ch.compare("X") == 0) {

			if (num < 0.4) {
				//LSystem.replace(i, 1, "D[LX]D[RX]LX");
				str.replace(i, 1, "D[LXV]D[RXV]LX");
			}
			else if (num < 0.6) {
				//LSystem.replace(i, 1, "D[LX]D[RX]LX");
				str.replace(i, 1, "D[LX]D[RX]DX");
			}
			else if (num < 0.8) {
				//LSystem.replace(i, 1, "D[LX]D[RX]LX");
				str.replace(i, 1, "D[RX]D[LX]DX");
			}
			else {
				//LSystem.replace(i, 1, "D[RX]D[LX]RX");
				str.replace(i, 1, "D[RXV]D[LXV]RX");
			}
			i = i + 13;	//13
			//printf("?");
		}

	}
	trees->push_back(str);
}

void Plant::display() {
	// start by clearing the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(fieldOfView, 1.0, 1, 2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eyeX, eyeY, eyeZ, lookX, lookY, lookZ, 0, 1, 0);

	glPushMatrix();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushAttrib(GL_LIGHTING_BIT); //saves current lighting stuff
	GLfloat ambient[4] = { 0.82, 0.41, 0.12 };    // ambient reflection
	GLfloat diffuse[4] = { 0.82, 0.41, 0.12 };   // diffuse reflection    
		// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

	//glBegin(GL_TRIANGLES);
	//glVertex3f(-20, 0, -20);
	//glVertex3f(20, 0, -20);
	//glVertex3f(20, 0, 20);
	//glVertex3f(-20, 0, 20);
	//glVertex3f(-20, 0, -20);
	//glVertex3f(20, 0, 20);

	//glEnd();
	glPopMatrix();
	glPopAttrib();


	draw();
	//glutSwapBuffers();
	//glutPostRedisplay();
}

void Plant::animate() {
	if (lastTime == 0)
		lastTime = timeGetTime();

	elapsedTime = timeGetTime() - lastTime;

	// Change the angle to make it blow in the wind
	float numR = (float)rand() / RAND_MAX;


	if (ANGLE > 21.5) {
		if (numR < 0.5) {
			incr = -0.15;
		}
		else {
			incr = -0.1;
		}
	}
	else if (ANGLE < 18.5) {
		if (numR > 0.5) {
			incr = 0.15;
		}
		else {
			incr = 0.1;
		}
	}
	// Turn on to make plant blow in wind
	//ANGLE += incr;

	if (depth < DEPTH)
		length += 0.001;

	if (elapsedTime - lastElapsedTime > 2000 && depth < DEPTH) {
		depth++;
		lastElapsedTime = elapsedTime;
		std::cout << "a ";

	}
	elapsedTime = elapsedTime / 5000;
	float t = (sin((elapsedTime * PI - PI / 2)) + 1) / 2;
	float p = (1 - t) * STARTX + t * ENDX;

	if (cam)
		eyeX = p;
	//glutPostRedisplay();
}

Plant::Plant() {
	model = glm::mat4(1);
	fieldOfView = 45;
	eyeX = 250;
	eyeY = 100;
	eyeZ = 100;
	lookX = 0;
	lookY = 50;
	lookZ = 0;
	srand(time(NULL));
	num = (float)rand() / RAND_MAX;
	// set the lighting
	glShadeModel(GL_SMOOTH);
	GLfloat lightP[4] = { 0.0, 800.0, 0.0,1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightP);

	// set the ambient light colour    
	GLfloat lightA[4] = { 0.0,0.9,0.9,1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightA);

	// set the specular light colour         
	GLfloat lightS[4] = { 0.9,0.9,0.9,1.0 };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightS);

	// set the diffuse light colour
	GLfloat lightD[4] = { 0.9,0.9,0.9,1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightD);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);
	
	//display();
	//animate();
	//glutDisplayFunc(this->display);
	//glutIdleFunc(this->animate);
	trees = new std::vector<std::string>();
	for (int i = 0; i <= DEPTH; i++) {
		expand(num);
	}
	//glutMainLoop();
}