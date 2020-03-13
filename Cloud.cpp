//
//  Cloud.cpp
//  
//
//  Created by osvaldo vazquez on 3/11/20.
//  Copyright © 2020 osvaldo vazquez. All rights reserved.
//

#include "Cloud.h"

#define GL(line) do { line; assert(glGetError() == GL_NO_ERROR); } while(0)
#define GLSL(str) (const char*)"#version 330\n" #str

// Regular Shaders
// Constants

float floorCoords[] = {
   30.0f, -1.0f, -30.0f, 5.0f, 0.0f, 0.0f,
  -30.0f, -1.0f, -30.0f, 0.0f, 0.0f, 0.0f,
   30.0f, -1.0f,  30.0f, 5.0f, 5.0f, 0.0f,
  -30.0f, -1.0f,  30.0f, 0.0f, 5.0f, 0.0f,
};

Cloud::Cloud() {
   //stackE = new entity();
}

Cloud::~Cloud() {
    //delete stackE;
}

// Math Functions

unsigned int Cloud::loadTexture(char* filename)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    bmp_header h;
    FILE* bmp = fopen(filename, "r");
    fread(&h, sizeof(bmp_header), 1, bmp);
    if (h.magic[0] != 'B' || h.magic[1] != 'M') { return texture; }
    char* buffer;
    fread(&buffer, h.image_size, 1, bmp);
    fclose(bmp);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (int)h.width, (int)h.height, 0, GL_RGB, GL_UNSIGNED_BYTE, &buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return texture;
}

unsigned int Cloud::blankTexture(int w, int h, int format)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, (GLenum)format, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    return texture;
}

unsigned int Cloud::makeFramebuffer(unsigned int* renderTexture, unsigned int* depthTexture, int w, int h)
{
    *renderTexture = blankTexture(w, h, GL_RGBA);
    *depthTexture = blankTexture(w, h, GL_DEPTH_COMPONENT);

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *renderTexture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, *depthTexture, 0);
    GLenum arr[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
    glDrawBuffers(2, arr);
    return framebuffer;
}

unsigned int Cloud::makeBuffer(GLenum target, size_t size, void* data)
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, (long)size, data, GL_STATIC_DRAW);
    return buffer;
}


void Cloud::makeEntity(int texcount, char textures[][40],
    void* data, unsigned int vertices, unsigned int layouts, int is_framebuffer, int depth_test,
    int w, int h)
{
    stackE.vertices = vertices;
    stackE.texcount = texcount;
    stackE.depth_test = depth_test;

    // Create VAO
    glGenVertexArrays(1, &stackE.vao);
    glBindVertexArray(stackE.vao);

    // Create Buffer
    stackE.buffer = makeBuffer(GL_ARRAY_BUFFER, sizeof(float) * vertices * layouts * 3, data);
    glBindBuffer(GL_ARRAY_BUFFER, stackE.buffer);

    // Load Attribute Pointers
    for (unsigned int i = 0; i < layouts; i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, (int)(sizeof(float) * layouts * 3), (void*)(sizeof(float) * i * 3));
    }

    // Load Textures

    if (!is_framebuffer)
        for (int i = 0; i < texcount; i++)
            if (textures[i][0] > 0)
                stackE.textures[i] = loadTexture(textures[i]);

    // Create a framebuffer if applicable
    if (is_framebuffer)
        stackE.fb = makeFramebuffer(&stackE.textures[0], &stackE.textures[1], w, h);
    
    //  return e;
}

void Cloud::renderEntity(float time)
{
    for (int i = 0; i < (stackE.fb ? 2 : stackE.texcount); i++)
        glActiveTexture(GL_TEXTURE0 + (unsigned int)i), glBindTexture(GL_TEXTURE_2D, stackE.textures[i]), glUniform1i(stackE.tex + i, i);
    // glUniformMatrix4fv(stackE.P, 1, GL_FALSE, 800);
     //glUniformMatrix4fv(stackE.V, 1, GL_FALSE, 600);
    glUniform1f(stackE.time, time);

    if (stackE.fb)
        glBindFramebuffer(GL_FRAMEBUFFER, 0), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (stackE.depth_test == 0)
        glDisable(GL_DEPTH_TEST);
    glBindVertexArray(stackE.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)stackE.vertices);
    if (stackE.depth_test == 0)
        glEnable(GL_DEPTH_TEST);
}


void Cloud::draw() {

    //if (!stackE.fb)
       // renderEntity((float)glfwGetTime() * 0.2f - 0.0f);
    if (stackE.fb)
        renderEntity(0.0f);
    //  renderEntity( (float)glfwGetTime() * 0.2f - 0.0f);
      /*
      for(int i = 0; i < (stackE.fb ? 2 : stackE.texcount); i++)
        glActiveTexture(GL_TEXTURE0 + (unsigned int)i), glBindTexture(GL_TEXTURE_2D, stackE.textures[i]), glUniform1i(stackE.tex + i, i);
      glDisable(GL_DEPTH_TEST);
      //matrix p = getProjectionMatrix(800, 600);
      glBindVertexArray(stackE.vao);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, (int)stackE.vertices);
      glEnable(GL_DEPTH_TEST);
     // renderEntity(0.0f);
       */
}
/*
void Cloud::renderScene(scene s, int w, int h)
{
 // matrix p = getProjectionMatrix(w, h);
 // matrix v = getViewMatrix(s.state.x, s.state.y, s.state.z, s.state.r, s.state.r2);
  for (unsigned int i = 0; i < s.entity_count; i++)
    if (!.entities[i].fb)
      renderEntity(s.entities[i], p, v, (float)glfwGetTime() * 0.2f - 0.0f);
  for (unsigned int i = 0; i < s.entity_count; i++)
    if (.entities[i].fb)
      renderEntity(s.entities[i], p, v, 0.0f);
}

*/

