#include "SceneGraph.h"

#include <algorithm>

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

	std::cout << "input lengths: " << input_vertices.size() << ", " << input_normals.size() << endl;

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

	std::cout << "vertices: " << vertices.size() << ", normals: " << normals.size() << ", faces: " << faces.size() << std::endl;

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

	currModel = C * initModel;

	// Bind to the VAO.
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // unbind from ebo

	glUniform1f(glGetUniformLocation(shaderProgram, "mode"), 0);
	/*glUniform1f(glGetUniformLocation(shaderProgram, "tex0"), glm::value_ptr(tex0));
	glUniform1f(glGetUniformLocation(shaderProgram, "tex1"), glm::value_ptr(tex1));
	glUniform1f(glGetUniformLocation(shaderProgram, "tex2"), glm::value_ptr(tex2));
	glUniform1f(glGetUniformLocation(shaderProgram, "tex3"), glm::value_ptr(tex3));*/

	//glUseProgram(shaderProgram);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currModel));

	// draws triangles
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);

	// Unbind from the VAO.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind from ebo
}