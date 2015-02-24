//
//  Box.cpp
//  Ducky-Water
//
//  Created by Carl Englund on 24/02/15.
//
//

#include "Box.h"
// Include GLEW
#ifdef __APPLE_CC__
#include <OpenGL/gl3.h>
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


void Box::drawBox()
{
    static const GLfloat Boxvertices[] = {
        
        -0.7f, 0.7f, 0.0f,
        
        -0.7f, -0.7f, 0.0f,
        
        0.7f, -0.7f, 0.0f,
        
        0.7f, 0.7f, 0.0f,
    };
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Boxvertices), Boxvertices, GL_STATIC_DRAW);
    
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    
    glDrawArrays(GL_LINE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &vertexbuffer);
    
}