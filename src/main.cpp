// Include standard headers
#include <stdio.h>
#include <stdlib.h>

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

#include <common/Init.h>
#include <common/Shader.h>
#include "Box.h"
#include "ParticleSystem.h"



GLFWwindow* window;

ParticleSystem particleSystem;

using namespace glm;



int main( void )
{
    
    //Create init object
    Init init = Init();
    
    //Initialize glfw
    init.glfw(4,1);
    
    //Open a window
    window = init.window();
    
    //Window title
    glfwSetWindowTitle(window, "Ducky Water");
    
    //Print window info
    init.printWindowInfo(window);
    
    //Make opened window current context
    glfwMakeContextCurrent(window);
    
    init.glew();
    
    // Dark blue background
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    //Load in shaders
    static ShaderProgram prog("../../Particle.vertexshader", "../../Particle.fragmentshader");
    //GLuint programID = LoadShaders("../../vertShader.vert", "../../fragShader.frag" );
    
    GLuint CameraRight_worldspace_ID  = glGetUniformLocation(prog, "CameraRight_worldspace");
    GLuint CameraUp_worldspace_ID  = glGetUniformLocation(prog, "CameraUp_worldspace");
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(prog, "MVP");
    
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 ViewMatrix       = glm::lookAt(
                                       glm::vec3(0,0,-3), // Camera is at (0    ,0,-3), in World Space
                                       glm::vec3(0,0,0), // and looks at the origin
                                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                       );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = ProjectionMatrix * ViewMatrix * Model; // Remember, matrix multiplication is the other way around

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    
    
    particleSystem.initBufferData();
    
    double lastTime = glfwGetTime();
    
    particleSystem.initParticles();
   
    
    
    do{
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;
        
       

        int ParticlesCount = particleSystem.updateParticles(delta);
        
    
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use our shader
        glUseProgram(prog);
        
        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
      
        
        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
        glDrawArraysInstanced(GL_POINTS, 0, 1, ParticlesCount);
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisableVertexAttribArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( !glfwWindowShouldClose(window) );
    
    // Close OpenGL window and terminate GLFW
    
    particleSystem.clean();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    glDeleteVertexArrays(1, &VertexArrayID);
    
    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &VertexArrayID);
    exit(EXIT_SUCCESS);
}
