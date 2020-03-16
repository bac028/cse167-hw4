#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

//using namespace std;

class Plant
{



public:
	glm::mat4 model;
	Plant();
	void push();
	void pop();
	void rotL();
	void rotR();
	void leaf();
	void drawLine();
	void draw();
	void expand(float num);
	void display();
	void animate();
};

