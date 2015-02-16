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
#include <iostream>

#include <common/Init.h>
#include <common/Shader.h>
#include "Particle.h"

using namespace glm;

int main( void )
{
  
  //Create init object
  Init init = Init();

  //Initialize glfw
  init.glfw(4,1);

  //Open a window
  GLFWwindow* window = init.window();
  glfwSetWindowTitle(window, "Ducky Water");
  //Print window info
  init.printWindowInfo(window);

  //Make opened window current context
  glfwMakeContextCurrent(window);

  
  init.glew();

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  //Load in shaders
  static ShaderProgram prog("../../vertShader.vert", "../../fragShader.frag");
   
  do{


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    prog();

   
    glfwSwapBuffers(window);
    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
  while( !glfwWindowShouldClose(window) );

  // Close OpenGL window and terminate GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  
}
