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
#include "Cell.h"


using namespace glm;



class ParticleSystem{
    
    public:
        const static int MAX_PARTICLES = 100;
        const float RADIUS = 4.0f;
        const float MASS = 0.05f;
        const float REST_DENSITY = 1000;
        const float STIFFNESS = 100;

    
        void initParticles();
        void clean();
        int updateParticles(double delta);
        void initBufferData();
        void updateCellIndex(Particle& p);
        void initGrid();
        void updateGrid();
        void calculatePressure();
        void calculateDensity(int index);
        void calculateForces(int index);
        float kernel(vec2 p);
        vec2 gradKernel(Particle& p);
        float laplaceKernel(Particle &p);
    
        Cell grid[Box::COLS*Box::ROWS];
        Particle ParticlesContainer[MAX_PARTICLES];
        GLuint particles_position_buffer;
        GLuint particles_color_buffer;
        GLuint billboard_vertex_buffer;
        GLfloat* g_particule_position_size_data;
    
    
    
    
    
};

#endif /* defined(__Ducky_Water__ParticleSystem__) */
