#ifndef TERRAIN_H
#define TERRAIN_H

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
#include "TerrainShading.h"
#include "SOIL.h"

struct VertexStr
{
	glm::vec3 position, normal, texture;
};

struct MaterialStr
{
	MaterialStr(void) { GLuint i; for (i = 0; i < 4; i++) { Specular[i] = 0.0f; Diffuse[i] = 0.0f; Ambient[i] = 0.0f; } Shininess[0] = 1.0f; }
	GLfloat Specular[4];
	GLfloat Diffuse[4];
	GLfloat Ambient[4];
	GLfloat Shininess[1];
};

struct SubsetStr
{
	SubsetStr() { NumTriangles = 0; hVBOIndexBuffer = 0; pIndices = NULL; }
	~SubsetStr() { if (pIndices) { delete[] pIndices; } }

	GLuint NumTriangles;
	MaterialStr Material;
	GLuint* pIndices;

	GLuint hVBOIndexBuffer;
};

class Terrain
{
protected:
	enum VertexAttribute { POSITION, NORMAL, TEXCOORDINATE };

	GLuint m_iWidth, m_iHeight;
	GLfloat m_fHsize, m_fVsize, m_fMedianHeight, m_fTileSize;
	unsigned int texture;
	unsigned int terrainVAO, terrainVBO, terrainEBO;

	GLuint m_uiNumSubsets;
	SubsetStr* m_pSubsets;

	GLuint m_uiNumVertices;
	VertexStr* m_pVertices;
	GLuint m_hVBOVertexBuffer;
	GLenum m_hProgram;
	TerrainShading m_Technique;

	GLuint m_hTex[4];

public:
	Terrain();
	Terrain(GLuint width, GLuint height, GLfloat tilesize, GLuint htexturetiles, GLuint vtexturetiles);
	~Terrain();
	void draw(unsigned int shader);
	void reset(float height = 0.0f);
	void fault(GLuint iterations, float initdisplacement, float dampening);
	void randomNoise(float magnitude);
	void smooth(GLuint iterations, GLuint centerweight);
	glm::vec3 getGridSize() const;
	float getMedianHeight(void) const { return m_fMedianHeight;}
	void render(void);
	
	void NormalGen();
	bool GetVertexAttrib(VertexAttribute attr, GLuint column, GLuint row, glm::vec3& output) const;
	void GenerateVertexBuffer();
	void CalculateMedianHeight();
	void SetNumVertices(GLuint numvertices);
	void SetNumSubsets(GLuint numsubsets);
	void ResizeSubset(GLuint numsubset, GLuint numtriangles);
	void GenerateSubsetIndicesBuffer(GLuint numsubset);
	void DrawSubset(GLuint numsubset) const;
};
#endif