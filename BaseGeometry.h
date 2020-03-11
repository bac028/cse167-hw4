#ifndef BASEGEOMETRY_H
#define BASEGEOMETRY_H

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

struct MaterialStr
{
	MaterialStr(void) {GLuint i; for(i=0;i<4;i++) {Specular[i]=0.0f; Emissive[i]=0.0f; Diffuse[i]=0.0f; Ambient[i]=0.0f;} Shininess[0]=1.0f;}
	GLfloat Specular[4];
	GLfloat Emissive[4];
	GLfloat Diffuse[4];
	GLfloat Ambient[4];
	GLfloat Shininess[1];
};

#pragma pack(push, 1)
struct VertexStr
{
	glm::vec3 Pos, Norm, Tex;
};
#pragma pack(pop)

struct SubsetStr
{
	SubsetStr() {NumTriangles=0; hVBOIndexBuffer=0; pIndices=NULL;}
	~SubsetStr() {if(pIndices) {delete[] pIndices;}}

	GLuint NumTriangles;
	MaterialStr Material;
	GLuint * pIndices;
	
	GLuint hVBOIndexBuffer; 
};

class BaseGeometry
{
public:
protected:
	BaseGeometry(void);
	~BaseGeometry(void);
	void SetNumVertices(GLuint numvertices);
	void GenerateVertexBuffer(void);
	void SetNumSubsets(GLuint numsubsets);
	void ResizeSubset(GLuint numsubset, GLuint numtriangles);
	void GenerateSubsetIndicesBuffer(GLuint numsubset);
	void DrawSubset(GLuint numsubset) const;
	virtual void Render()=0;

	GLuint m_uiNumSubsets;
	SubsetStr * m_pSubsets;

	GLuint m_uiNumVertices;
	VertexStr * m_pVertices;
	GLuint m_hVBOVertexBuffer;

	//OpenGL extensions function pointers
	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
	PFNGLGENBUFFERSARBPROC glGenBuffersARB;					// VBO Name Generation Procedure
	PFNGLBINDBUFFERARBPROC glBindBufferARB;					// VBO Bind Procedure
	PFNGLBUFFERDATAARBPROC glBufferDataARB;					// VBO Data Loading Procedure
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;			// VBO Deletion Procedure
};
/*
struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct SubsetStr
{
	SubsetStr() {numTriangles=0; hVBOIndexBuffer=0; pIndices=NULL;}
	~SubsetStr() {if(pIndices) {delete[] pIndices;}}

	GLuint numTriangles;
	Material Material;
	GLuint * pIndices;
	
	GLuint hVBOIndexBuffer; 
};

class BaseGeometry
{
public:
protected:
	BaseGeometry();
	~BaseGeometry();
	void SetNumVertices(GLuint numVertices);
	void GenerateVertexBuffer();
	void SetNumSubsets(GLuint numSubsets);
	void ResizeSubset(GLuint numSubset, GLuint numTriangles);
	void GenerateSubsetIndicesBuffer(GLuint numSubset);
	void DrawSubset(GLuint numSubset) const;
	virtual void Render()=0;

    GLuint numSubsetsBaseInt;
	SubsetStr * numSubsetsBase;

	GLuint numVerticesInt;
	glm::vec4 numVerticesBase;
	GLuint VBO;
};
*/
#endif