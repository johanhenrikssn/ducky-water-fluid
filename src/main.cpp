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
//#include "shader.hpp"
#include "controls.hpp"
const int MaxParticles = 10000;



struct Particle{
    glm::vec2 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f
    
    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
    
   };
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

int FindUnusedParticle(){
    
    for(int i=LastUsedParticle; i<MaxParticles; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }
    
    for(int i=0; i<LastUsedParticle; i++){
        if (ParticlesContainer[i].life < 0){
            LastUsedParticle = i;
            return i;
        }
    }
    
    return 0; // All particles are taken, override the first one
}


void SortParticles(){
    std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}


using namespace glm;

int main( void )
{
    
    
    //Create init object
    Init init = Init();
    
    //Initialize glfw
    init.glfw(4,1);
    
    //Open a window
    GLFWwindow* window = init.window();
    
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
    GLuint ViewProjMatrixID = glGetUniformLocation(prog, "VP");
    
   


    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    static GLfloat* g_particule_position_size_data = new GLfloat[MaxParticles * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[MaxParticles * 4];
    
    static const GLfloat g_vertex_buffer_data1[] = {
        -0.7f, -0.7f, 0.0f,
        0.7f, -0.7f, 0.0f,
        0.7f, 0.7f, 0.0f,
        -0.7f, 0.7f, 0.0f,
    };
    
    for(int i=0; i<MaxParticles; i++){
        ParticlesContainer[i].life = -1.0f;
        ParticlesContainer[i].cameradistance = -1.0f;
    }

    

    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data1), g_vertex_buffer_data1, GL_STATIC_DRAW);
    
    
    
  
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    
    // The VBO containing the colors of the particles
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    
    double lastTime = glfwGetTime();
    
    do{
        
        
        
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;
        
        
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        
        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp,
        // but this works too.
        glm::vec2 CameraPosition(glm::inverse(ViewMatrix)[3]);
        
        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

     
        
        
        int newparticles = (int)(delta*10000.0);
        if (newparticles > (int)(0.016f*10000.0))
            newparticles = (int)(0.016f*10000.0);
        
        for(int i=0; i<newparticles; i++){
            int particleIndex = FindUnusedParticle();
            ParticlesContainer[particleIndex].life = 20.0f; // This particle will live 5 seconds.
            ParticlesContainer[particleIndex].pos = glm::vec2(0,-0.5f);
            
            float spread = 0.5f;
            glm::vec2 maindir = glm::vec2(0.0f, 0.005f);
            // Very bad way to generate a random direction;
            // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
            // combined with some user-controlled parameters (main direction, spread, etc)
            glm::vec2 randomdir = glm::vec2(
                                            (rand()%2000 - 1000.0f)/10000.0f,
                                            (rand()%2000 - 1000.0f)/10000.0f
                                            );
            
            ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;
            
            
            // Very bad way to generate a random color
            ParticlesContainer[particleIndex].r = rand() % 256;
            ParticlesContainer[particleIndex].g = rand() % 256;
            ParticlesContainer[particleIndex].b = rand() % 256;
            ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
            
            ParticlesContainer[particleIndex].size = 0.1f;
            
        }

        int ParticlesCount = 0;
        for(int i=0; i<MaxParticles; i++){
            
            Particle& p = ParticlesContainer[i]; // shortcut
            
            if(p.life > 0.0f){
                
                // Decrease life
                p.life -= delta;
                if (p.life > 0.0f){
                    
                    // Simulate simple physics : gravity only, no collisions
                    p.speed += glm::vec2(0.0f,-0.00981f) * (float)delta * 0.5f;
                    p.pos += p.speed * (float)delta;
                    p.cameradistance = length( p.pos - CameraPosition );
                    ParticlesContainer[i].pos += glm::vec2(0.0f,0.50f) * (float)delta;
                    
                    // Fill the GPU buffer
                    g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
                    g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
                    
                    
                    g_particule_position_size_data[4*ParticlesCount+3] = p.size;
                    
                    g_particule_color_data[4*ParticlesCount+0] = p.r;
                    g_particule_color_data[4*ParticlesCount+1] = p.g;
                    g_particule_color_data[4*ParticlesCount+2] = p.b;
                    g_particule_color_data[4*ParticlesCount+3] = p.a;
                    
                }else{
                    // Particles that just died will be put at the end of the buffer in SortParticles();
                    p.cameradistance = -1.0f;
                }
                
                ParticlesCount++;
                
            }
        }
        
        SortParticles();
        
        
        printf("%d ",ParticlesCount);
        
       	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);
        
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
 
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Use our shader
        glUseProgram(prog);
        
        // Bind our texture in Texture Unit 0
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
       // glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
        
       glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        
       
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              4,                                // size : x + y + z + size => 4
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glVertexAttribPointer(
                              2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              4,                                // size : r + g + b + a => 4
                              GL_UNSIGNED_BYTE,                 // type
                              GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
        
        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisableVertexAttribArray(0);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( !glfwWindowShouldClose(window) );
    
    // Close OpenGL window and terminate GLFW
    delete[] g_particule_position_size_data;
    glfwDestroyWindow(window);
    glfwTerminate();
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &VertexArrayID);
    exit(EXIT_SUCCESS);
}
