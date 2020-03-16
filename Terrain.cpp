#include "Terrain.h"
#include <iostream>
#include <algorithm>

using namespace std;

Terrain::Terrain(GLuint width, GLuint height, GLfloat tilesize, GLuint htexturetiles, GLuint vtexturetiles)
{
	printf("Terrain ctor begin --\n");
	m_hProgram = glCreateProgramObjectARB();

	//Define width and height
	if (width < 1) m_iWidth = 2;
	else m_iWidth = width + 1;

	if (height < 1) m_iHeight = 2;
	else m_iHeight = height + 1;

	m_fTileSize = tilesize;
	m_fHsize = (float(m_iWidth - 1)) * m_fTileSize;
	m_fVsize = (float(m_iHeight - 1)) * m_fTileSize;

	SetNumVertices(m_iWidth * m_iHeight);
	SetNumSubsets(1);
	ResizeSubset(0, (2 * (m_iWidth - 1) * (m_iHeight - 1)));

	//Generete base geometry
	GLuint i, j;
	for (i = 0;i < m_iHeight;i++)
	{
		for (j = 0;j < m_iWidth;j++)
		{
			m_pVertices[j + i * m_iWidth].position.x = (float(j) - (float(m_iHeight - 1) * 0.5f)) * m_fTileSize;
			m_pVertices[j + i * m_iWidth].position.y = 0.0f;
			m_pVertices[j + i * m_iWidth].position.z = ((float(m_iHeight - 1) * 0.5f) - float(i)) * m_fTileSize;

			m_pVertices[j + i * m_iWidth].normal.x = 0.0f;
			m_pVertices[j + i * m_iWidth].normal.y = 1.0f;
			m_pVertices[j + i * m_iWidth].normal.z = 0.0f;

			m_pVertices[j + i * m_iWidth].texture.x = float(htexturetiles) * float(j) / float(m_iWidth - 1);
			m_pVertices[j + i * m_iWidth].texture.y = float(vtexturetiles) * float(i) / float(m_iHeight - 1);
			m_pVertices[j + i * m_iWidth].texture.z = 0.0f;
		}
	}

	GenerateVertexBuffer();

	m_fMedianHeight = 0.0f;

	//Generate indices
	GLuint state = 0;
	GLuint ctr = 0;
	for (i = 0;i < (m_iHeight - 1);i++)
	{
		for (j = 0;j < (m_iWidth - 1);j++)
		{
			if (state == 0)
			{
				m_pSubsets[0].pIndices[ctr] = j + (i + 1) * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = j + i * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = (j + 1) + (i + 1) * m_iWidth;
				ctr++;

				m_pSubsets[0].pIndices[ctr] = (j + 1) + i * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = (j + 1) + (i + 1) * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = j + i * m_iWidth;
				ctr++;
			}
			else
			{
				m_pSubsets[0].pIndices[ctr] = (j + 1) + i * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = j + (i + 1) * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = j + i * m_iWidth;
				ctr++;

				m_pSubsets[0].pIndices[ctr] = j + (i + 1) * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = (j + 1) + i * m_iWidth;
				ctr++;
				m_pSubsets[0].pIndices[ctr] = (j + 1) + (i + 1) * m_iWidth;
				ctr++;
			}
			state = (state + 1) % 2;
		}
		if ((m_iWidth % 2) == 1)
		{
			state = (state + 1) % 2;
		}
	}

	GenerateSubsetIndicesBuffer(0);

	//Generate material properties
	m_pSubsets[0].Material.Specular[0] = 0.0f;
	m_pSubsets[0].Material.Specular[1] = 0.0f;
	m_pSubsets[0].Material.Specular[2] = 0.0f;
	m_pSubsets[0].Material.Specular[3] = 1.0f;
	m_pSubsets[0].Material.Diffuse[0] = 0.6f;
	m_pSubsets[0].Material.Diffuse[1] = 0.6f;
	m_pSubsets[0].Material.Diffuse[2] = 0.6f;
	m_pSubsets[0].Material.Diffuse[3] = 1.0f;
	m_pSubsets[0].Material.Ambient[0] = 0.4f;
	m_pSubsets[0].Material.Ambient[1] = 0.4f;
	m_pSubsets[0].Material.Ambient[2] = 0.4f;
	m_pSubsets[0].Material.Ambient[3] = 1.0f;
	m_pSubsets[0].Material.Shininess[0] = 80.0f;

	m_hTex[0] = SOIL_load_OGL_texture("Textures\\Rock.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
	m_hTex[1] = SOIL_load_OGL_texture("Textures\\Sand.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
	m_hTex[2] = SOIL_load_OGL_texture("Textures\\Grass.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
	m_hTex[3] = SOIL_load_OGL_texture("Textures\\Snow.dds", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_TEXTURE_REPEATS);
	printf("Terrain ctor end --\n");
}

Terrain::~Terrain()
{
	glDeleteBuffers(1, &terrainVBO);
	glDeleteBuffers(1, &terrainEBO);
	glDeleteVertexArrays(1, &terrainVAO);
}

void Terrain::draw(unsigned int shader)
{

	// skybox cube
	glBindVertexArray(terrainVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
	
	glBindTexture(GL_TEXTURE_2D, 0); // unbind texture
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind from ebo
	glBindVertexArray(0);

	printf("-- end Terrain::draw() ---");
}

void Terrain::reset(float height)
{
	GLuint i, j;
	for (i = 0;i < m_iHeight;i++)
	{
		for (j = 0;j < m_iWidth;j++)
		{
			m_pVertices[j + i * m_iWidth].position.x = (float(j) - (float(m_iHeight - 1) * 0.5f)) * m_fTileSize;
			m_pVertices[j + i * m_iWidth].position.y = height;
			m_pVertices[j + i * m_iWidth].position.z = ((float(m_iHeight - 1) * 0.5f) - float(i)) * m_fTileSize;
		}
	}
	NormalGen();
	m_fMedianHeight = height;
}

void Terrain::fault(GLuint iterations, float initdisplacement, float dampening)
{
	GLuint it, i, j;
	float disp = initdisplacement;
	glm::vec3 p1, p2, fault;
	glm::vec3 proj;
	for (it = 0;it < iterations;it++)
	{
		p1.x = m_fTileSize * float(m_iWidth) * ((float(rand()) / float(RAND_MAX)) - 0.5f);
		p1.y = 0.0f;
		p1.z = m_fTileSize * float(m_iWidth) * (0.5f - (float(rand()) / float(RAND_MAX)));
		p2.x = m_fTileSize * float(m_iWidth) * ((float(rand()) / float(RAND_MAX)) - 0.5f);
		p2.y = 0.0f;
		p2.z = m_fTileSize * float(m_iWidth) * (0.5f - (float(rand()) / float(RAND_MAX)));
		fault = p2 - p1;
		for (i = 0;i < (m_iHeight);i++)
		{
			for (j = 0;j < (m_iWidth);j++)
			{
				proj = m_pVertices[j + i * m_iWidth].position;
				proj.y = 0.0f;
				proj = proj - p1;
				proj = glm::cross(proj, fault);
				proj = glm::normalize(proj);
				m_pVertices[j + i * m_iWidth].position.y += disp * proj.y;
			}
		}
		if (dampening > 0.0f && dampening < 1.0f)
		{
			disp = disp * dampening;
		}
		else
		{
			disp = disp * (float(iterations - it) / float(iterations));
		}
	}
	NormalGen();
	CalculateMedianHeight();
}

void Terrain::randomNoise(float magnitude)
{
	GLuint i, j;
	for (i = 0;i < m_iHeight;i++)
	{
		for (j = 0;j < m_iWidth;j++)
		{
			m_pVertices[j + i * m_iWidth].position.y += magnitude * ((float(rand()) / float(RAND_MAX)) - 0.5f);
		}
	}
	NormalGen();
	CalculateMedianHeight();
}

void Terrain::smooth(GLuint iterations, GLuint centerweight)
{
	glm::vec3 vert, * temp;
	bool res;
	temp = new glm::vec3[m_uiNumVertices];
	GLuint i, j, it, ctr;
	for (it = 0; it < iterations;it++)
	{
		for (i = 0; i < (m_iHeight);i++)
		{
			for (j = 0; j < (m_iWidth);j++)
			{
				ctr = 0;
				temp[j + i * m_iWidth].x = 0.0f;
				temp[j + i * m_iWidth].y = 0.0f;
				temp[j + i * m_iWidth].z = 0.0f;

				res = GetVertexAttrib(POSITION, j, i, vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j - 1), i, vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j - 1), (i - 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, j, (i - 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j + 1), (i - 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j + 1), i, vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j + 1), (i + 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, j, (i + 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				res = GetVertexAttrib(POSITION, (j - 1), (i + 1), vert);
				if (res)
				{
					temp[j + i * m_iWidth] += float(centerweight) * vert;
					ctr += centerweight;
				}
				temp[j + i * m_iWidth].x = temp[j + i * m_iWidth].x / float(ctr);
				temp[j + i * m_iWidth].y = temp[j + i * m_iWidth].y / float(ctr);
				temp[j + i * m_iWidth].z = temp[j + i * m_iWidth].z / float(ctr);
			}
		}
		for (i = 0;i < m_uiNumVertices;i++)
		{
			m_pVertices[i].position = temp[i];
		}
	}
	delete[] temp;
	NormalGen();
	CalculateMedianHeight();
}

glm::vec3 Terrain::getGridSize() const
{
	glm::vec3 size(m_fHsize, 0.0f, m_fVsize);
	return size;
}

void Terrain::render(void)
{

	//printf("-- start Terrain::render() ---");
	
	m_Technique.Set();
	m_Technique.SetTexture0(m_hTex[0]);
	m_Technique.SetTexture1(m_hTex[1]);
	m_Technique.SetTexture2(m_hTex[2]);
	m_Technique.SetTexture3(m_hTex[3]);
	m_Technique.SetMedianHeight(m_fMedianHeight);
	DrawSubset(0);

	//printf("-- start Terrain::render() ---");
}


void Terrain::NormalGen()
{
	GLuint i, j;
	glm::vec3 VertexPos, Neighbor1Pos, Neighbor2Pos, TotalNorm, Norm, v1, v2;
	bool r1, r2;
	for (i = 0;i < m_iHeight;i++)
	{
		for (j = 0;j < m_iWidth;j++)
		{
			TotalNorm.x = 0.0f;
			TotalNorm.y = 0.0f;
			TotalNorm.z = 0.0f;
			GetVertexAttrib(POSITION, j, i, VertexPos);
			if ((i + j) % 2 == 0)
			{
				r1 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j + 1), (i - 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j + 1), (i - 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j + 1), (i + 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j + 1), (i + 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j - 1), (i + 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j - 1), (i + 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j - 1), (i - 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j - 1), (i - 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
			}
			else
			{
				r1 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j + 1), i, Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, j, (i + 1), Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
				r1 = GetVertexAttrib(POSITION, (j - 1), i, Neighbor1Pos);
				r2 = GetVertexAttrib(POSITION, j, (i - 1), Neighbor2Pos);
				if (r1 && r2)
				{
					v1 = Neighbor1Pos - VertexPos;
					v2 = Neighbor2Pos - VertexPos;
					Norm = glm::cross(v1, v2);
					Norm = glm::normalize(Norm);
					TotalNorm += Norm;
				}
			}
			TotalNorm = glm::normalize(TotalNorm);
			m_pVertices[j + i * m_iWidth].normal = TotalNorm;
		}
	}
	GenerateVertexBuffer();
}

bool Terrain::GetVertexAttrib(VertexAttribute attr, GLuint column, GLuint row, glm::vec3& output) const
{
	bool succeded = false;
	if (column < m_iWidth)
	{
		if (row < m_iHeight)
		{
			switch (attr)
			{
			case POSITION:
				output = m_pVertices[column + (row)*m_iWidth].position;
				succeded = true;
				break;
			case NORMAL:
				output = m_pVertices[column + (row)*m_iWidth].normal;
				succeded = true;
				break;
			case TEXCOORDINATE:
				output = m_pVertices[column + (row)*m_iWidth].texture;
				succeded = true;
				break;
			}
		}
	}
	return succeded;
}

void Terrain::GenerateVertexBuffer()
{
	glDeleteBuffersARB(1, &m_hVBOVertexBuffer);
	glGenBuffersARB(1, &m_hVBOVertexBuffer);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_hVBOVertexBuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_uiNumVertices * sizeof(VertexStr), m_pVertices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void Terrain::CalculateMedianHeight()
{
	GLuint i;
	m_fMedianHeight = 0.0f;
	for (i = 0;i < m_uiNumVertices;i++)
	{
		m_fMedianHeight += m_pVertices[i].position.y;
	}
	m_fMedianHeight = m_fMedianHeight / float(m_uiNumVertices);
}

void Terrain::SetNumVertices(GLuint numvertices)
{
	if (m_pVertices != NULL)
	{
		delete[] m_pVertices;
	}
	m_uiNumVertices = numvertices;
	m_pVertices = new VertexStr[m_uiNumVertices];
}

void Terrain::SetNumSubsets(GLuint numsubsets)
{
	GLuint i;
	if (m_pSubsets != NULL)
	{
		for (i = 0;i < m_uiNumSubsets;i++)
		{
			glDeleteBuffersARB(1, &m_pSubsets[i].hVBOIndexBuffer);
		}
		delete[] m_pSubsets;
	}
	m_uiNumSubsets = numsubsets;
	m_pSubsets = new SubsetStr[m_uiNumSubsets];
}

void Terrain::ResizeSubset(GLuint numsubset, GLuint numtriangles)
{
	if (numsubset < m_uiNumSubsets)
	{
		if (m_pSubsets[numsubset].pIndices != NULL)
		{
			glDeleteBuffersARB(1, &m_pSubsets[numsubset].hVBOIndexBuffer);
			delete[] m_pSubsets[numsubset].pIndices;
		}
		m_pSubsets[numsubset].NumTriangles = numtriangles;
		m_pSubsets[numsubset].pIndices = new GLuint[numtriangles * 3];
	}
}

void Terrain::GenerateSubsetIndicesBuffer(GLuint numsubset)
{
	if (numsubset < m_uiNumSubsets)
	{
		glDeleteBuffersARB(1, &m_pSubsets[numsubset].hVBOIndexBuffer);
		glGenBuffersARB(1, &m_pSubsets[numsubset].hVBOIndexBuffer);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_pSubsets[numsubset].hVBOIndexBuffer);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 3 * m_pSubsets[numsubset].NumTriangles * sizeof(GLuint), m_pSubsets[numsubset].pIndices, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}
}

void Terrain::DrawSubset(GLuint numsubset) const
{
	if (numsubset < m_uiNumSubsets)
	{
		//Set material properties
		glMaterialfv(GL_FRONT, GL_SPECULAR, m_pSubsets[numsubset].Material.Specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_pSubsets[numsubset].Material.Diffuse);
		glMaterialfv(GL_FRONT, GL_AMBIENT, m_pSubsets[numsubset].Material.Ambient);
		glMaterialfv(GL_FRONT, GL_SHININESS, m_pSubsets[numsubset].Material.Shininess);

		//Setup array usage states
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//Setup arrays
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_hVBOVertexBuffer);
		glVertexPointer(4, GL_FLOAT, sizeof(VertexStr), 0);
		glNormalPointer(GL_FLOAT, sizeof(VertexStr), (GLvoid*)(sizeof(glm::vec3)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(VertexStr), (GLvoid*)(2 * sizeof(glm::vec3)));
		//Draw		
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_pSubsets[numsubset].hVBOIndexBuffer);
		glDrawElements(GL_TRIANGLES, 3 * m_pSubsets[numsubset].NumTriangles, GL_UNSIGNED_INT, 0);
		//Restore previous state
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		// bind with 0, so, switch back to normal pointer operation
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}
}