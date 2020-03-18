#include "SceneGraph.h"

#include <algorithm>
#include <stack>

using namespace std;

Geometry::Geometry() {}

// load file
void Geometry::init(string objFilename, int objFormat, ObjMaterial material) {

	int type;
	float x, y, z, r, g, b;
	float maxX = -10000.0f, maxY = -10000.0f, maxZ = -10000.0f;
	float minX = 10000.0f, minY = 10000.0f, minZ = 10000.0f;

	this->material = material;

	vector<glm::vec3> input_vertices, input_normals, input_textures;
	vector<int> vertex_indices, normal_indices, texture_indices;

	std::cout << "Loading " << objFilename << "..." << endl;

	FILE* fp = fopen(("models/" + objFilename + ".obj").c_str(), "rb");
	if (!fp) {
		cout << "error loading file" << endl;
		exit(-1);
	}

	// parses object file
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

			else if (type == 't') {
				fscanf(fp, "%f %f %f", &x, &y, &z);
				input_textures.push_back(glm::vec3(x, y, z));
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

					texture_indices.push_back(vt1 - 1);
					texture_indices.push_back(vt2 - 1);
					texture_indices.push_back(vt3 - 1);
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

	//std::cout << "input lengths: " << input_vertices.size() << ", " << input_normals.size() << endl;

	fclose(fp);

	if (objFormat == 0) {
		for (unsigned int i = 0; i < vertex_indices.size(); i++) {
			vertices.push_back(input_vertices[vertex_indices[i]]);
			normals.push_back(input_normals[normal_indices[i]]);
			textures.push_back(input_textures[texture_indices[i]]);
			faces.push_back(i);
		}
	}

	else if (objFormat == 1) {
		vertices = input_vertices;
		normals = input_normals;
	}

	//std::cout << "vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << faces.size() << std::endl;

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

Geometry::~Geometry() {
	// Delete the VAO, VBO, and EBO
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

// draw object
void Geometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) {

	currModel = C * initModel;
	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // unbind from ebo

	glUniform3fv(ambientLoc, 1, glm::value_ptr(material.ambient));
	glUniform3fv(diffuseLoc, 1, glm::value_ptr(material.diffuse));
	glUniform3fv(specularLoc, 1, glm::value_ptr(material.specular));
	glUniform1f(shininessLoc, material.shininess);
	glUniform1f(glGetUniformLocation(shaderProgram, "mode"), 2);

	//glUseProgram(shaderProgram);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currModel));
	
	// draws triangles
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind from ebo

}

TerrainGeometry::TerrainGeometry(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec3> textures, std::vector<unsigned int> faces) {

	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->faces = faces;

	this->material = ObjMaterial();
	this->material.ambient = glm::vec3(0.2f, 0.3f, 0.1f);
	this->material.diffuse = glm::vec3(0.5f, 0.5f, 0.2f);
	this->material.specular = glm::vec3(0.0f, 0.0f, 0.0f);
	this->material.shininess = 100.0f;

	//std::cout << "vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << faces.size() << std::endl;

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

TerrainGeometry::~TerrainGeometry() {
	// Delete the VAO, VBO, and EBO
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void TerrainGeometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) {
	//printf("Calling Function Yes\n");
	currModel = C * initModel;

	// For Texture
	glBindTexture(GL_TEXTURE_2D, normalMap);
    glEnable(GL_TEXTURE_2D);
	glVertexPointer(3, GL_FLOAT, sizeof(vertices), &vertices[0]);
    glEnableClientState(GL_VERTEX_ARRAY);


	//Set up texture environment to do (tex0 dot tex1)*color
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);

    glActiveTextureARB(GL_TEXTURE1_ARB);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);

    glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	
	//Bind normalisation cube map to texture unit 1
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo[0]);
	glEnable(GL_ELEMENT_ARRAY_BUFFER_ARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	//Set vertex arrays for torus
	glVertexPointer(3, GL_FLOAT, sizeof(vertices), &vertices[0]);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // unbind from ebo

	glUniform1f(glGetUniformLocation(shaderProgram, "mode"), 1);
	glUniform3fv(glGetUniformLocation(shaderProgram, "materialAmbient"), 1, glm::value_ptr(material.ambient));
	glUniform3fv(glGetUniformLocation(shaderProgram, "materialDiffuse"), 1, glm::value_ptr(material.diffuse));
	glUniform3fv(glGetUniformLocation(shaderProgram, "materialSpecular"), 1, glm::value_ptr(material.specular));
	glUniform1f(glGetUniformLocation(shaderProgram, "materialShininess"), material.shininess);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currModel));

	// draws triangles
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
	
	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind from ebo
}


PlantGeometry::PlantGeometry(glm::vec3 startingPosition) {
	LSystemString = "X";

	this->position = startingPosition;

	int levels = (2 * rand() / RAND_MAX) + 4;
	for (int i = 0; i < levels; i++) {
		addLevel();
	}

	//std::cout << LSystemString << std::endl;

	// generates lines based on l system
	generatePoints();

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo[0]);

	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lines.size(), lines.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PlantGeometry::addLevel() {
	float num = (4 * rand()) / RAND_MAX;

	std::string ch = "";

	for (int i = 0; i < LSystemString.length(); i++) {
		ch = LSystemString.at(i);

		if (ch.compare("F") == 0) {
		}

		if (ch.compare("X") == 0) {

			if (num == 0) {
				LSystemString.replace(i, 1, "F[+FX]F[&+FX]F[^FX][&F]");
				i += 21;
			}
			else if (num == 1) {
				LSystemString.replace(i, 1, "FF[^-FX][X]FF[^+FX]F");
				i += 20;
			}
			else if (num == 2) {
				LSystemString.replace(i, 1, "F[^&FX]F[-FX]");
				i += 12;
			}
			else {
				LSystemString.replace(i, 1, "FF[^FX][&FX]F[+FX]");
				i += 18;
			}
		}
	}
}

void PlantGeometry::generatePoints() {

	glm::vec3 currentPoint = position;
	glm::vec3 dir = glm::vec3(0, this->segmentLength, 0);
	

	stack<glm::vec3> savedPosition;
	stack<glm::vec3> savedDirection;

	std::string ch = "";
	for (int i = 0; i < LSystemString.length(); i++) {

		float angleOffset = ((float)rand()) / RAND_MAX * 12 - 3; // +- 5 angles
		float lengthOffset = ((float)rand()) / RAND_MAX;
		ch = LSystemString.at(i);
		
		// Forward by distance
		if (ch.compare("F") == 0 || ch.compare("X") == 0) {
			lines.push_back(currentPoint.x);
			lines.push_back(currentPoint.y);
			lines.push_back(currentPoint.z);
			lines.push_back(currentPoint.x + dir.x + lengthOffset);
			lines.push_back(currentPoint.y + dir.y + lengthOffset);
			lines.push_back(currentPoint.z + dir.z + lengthOffset);
			currentPoint += (dir + lengthOffset);
		}

		// push current state
		else if (ch.compare("[") == 0) {
			savedPosition.push(currentPoint);
			savedDirection.push(dir);
		}

		// pop current state
		else if (ch.compare("]") == 0) {
			currentPoint = savedPosition.top();
			savedPosition.pop();
			dir = savedDirection.top();
			savedDirection.pop();
		}

		// end and draw leaf
		else if (ch.compare("L") == 0) {
		}

		// turn left
		else if (ch.compare("+") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(0, 0, 1)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}

		// turn right
		else if (ch.compare("-") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(0, 0, -1)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}

		// pitch down
		else if (ch.compare("&") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(1, 0, 0)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}

		// pitch up
		else if (ch.compare("^") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(-1, 0, 0)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}

		// roll left
		else if (ch.compare("<") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(0, 1, 0)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}

		// roll right
		else if (ch.compare(">") == 0) {
			glm::vec4 newDir = glm::rotate(glm::mat4(1), glm::radians(angle + angleOffset), glm::vec3(0, -1, 0)) * glm::vec4(dir, 1.0f);
			dir = glm::vec3(newDir.x, newDir.y, newDir.z);
		}
	}
}

void PlantGeometry::draw(glm::mat4 C, unsigned int shaderProgram, unsigned int modelLoc, unsigned int ambientLoc, unsigned int diffuseLoc, unsigned int specularLoc, unsigned int shininessLoc) {
	
	// Bind to the VAO.
	glBindVertexArray(vao);

	glLineWidth(lineWidth);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(C));
	glUniform1f(glGetUniformLocation(shaderProgram, "mode"), 2);
	
	for (int i = 0; i < this->lines.size(); i += 6) {
		glDrawArrays(GL_LINES, i, 6);
	}
	
	// Unbind from the VAO.
	glBindVertexArray(0);
}
