#ifndef TERRAIN_SHADING_H
#define TERRAIN_SHADING_H

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <windows.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <iostream>

class TerrainShading {
protected:
	GLcharARB* m_pVertexShaderSrc;
	GLcharARB* m_pPixelShaderSrc;
	GLint m_iMaxTexUnits;

	GLenum m_hProgram;
	GLenum m_hVertexShader;
	GLenum m_hPixelShader;

	//Pointers to log strings and log length variables
	GLcharARB* m_pVertexShaderLog;
	GLint m_iVSCompileFlag;
	GLcharARB* m_pPixelShaderLog;
	GLint m_iPSCompileFlag;
	GLcharARB* m_pProgramLog;
	GLint m_iProgramLinkFlag;
public:
	TerrainShading();
	~TerrainShading();
	void Set();
	void SetTexture0(GLuint hTex);
	void SetTexture1(GLuint hTex);
	void SetTexture2(GLuint hTex);
	void SetTexture3(GLuint hTex);
	void BindTex2DToTextureUnit(GLuint hTexture, GLenum TextureUnitID);
	void BindTexUnitToSampler(GLint TextureUnitNumber, GLcharARB* SamplerName);
	void SetMedianHeight(GLfloat value);

	char* TextFileRead(char* fn);
};

#endif