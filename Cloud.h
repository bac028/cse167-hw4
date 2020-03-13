//
//  Cloud.hpp
//  HW0_EXample
//
//  Created by osvaldo vazquez on 3/11/20.
//  Copyright © 2020 osvaldo vazquez. All rights reserved.
//

#ifndef Cloud_hpp
#define Cloud_hpp

#include <stdio.h>
#define GLFW_INCLUDE_GLCOREARB
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

class Cloud
{
public:
    // Structures

    glm::vec3 vector;
    glm::mat4 matrix;
    typedef struct { char magic[2]; unsigned int size, reserved, offset, hsize, width, height, colors, compression, image_size, h_res, v_res, palletes, colors2; } bmp_header;

    typedef struct { float x, y, z, r, r2; double px, py; } gamestate;
    typedef struct { unsigned int vao, buffer, vertices, program, textures[256], fb; int depth_test, texcount, P, V, M, tex, time; } entity;
    typedef struct { entity* entities; unsigned int entity_count; gamestate state; } scene;

    entity stackE;

    //Methods
    Cloud();
    ~Cloud();
    unsigned int loadTexture(char* filename);
    unsigned int blankTexture(int w, int h, int format);
    unsigned int makeFramebuffer(unsigned int* renderTexture, unsigned int* depthTexture, int w, int h);
    unsigned int makeBuffer(GLenum target, size_t size, void* data);
    void makeEntity(int texcount, char textures[][40],
        void* data, unsigned int vertices, unsigned int layouts, int is_framebuffer, int depth_test,
        int w, int h);
    void draw();
    void renderEntity(float time);
    void renderScene(int w, int h);
};


#endif /* Cloud_hpp */

