//
//  ParticleSystem.h
//  Ducky-Water
//
//  Created by Mattias Palmgren on 2015-02-24.
//
//

#ifndef __Ducky_Water__ParticleSystem__
#define __Ducky_Water__ParticleSystem__

#include <stdio.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Include GLFW
#include <GLFW/glfw3.h>

#include "Box.h"

using namespace glm;

const int MaxParticles = 10000;

struct Particle{
    vec2 pos, speed, force;
    unsigned char r,g,b,a; // Color
    float size, pressure;
    int cellIndex;
    
};


class ParticleSystem{
    
    public:
        Particle ParticlesContainer[MaxParticles];
        GLuint particles_position_buffer;
        GLuint particles_color_buffer;
        GLuint billboard_vertex_buffer;
        GLfloat* g_particule_position_size_data;
    
        void initParticles();
        void clean();
        int updateParticles(double delta);
        void initBufferData();
        void updateCellIndex(Particle& p);
    
    
    
};

#endif /* defined(__Ducky_Water__ParticleSystem__) */
