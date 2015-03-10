//
//  Cell.h
//  Ducky-Water
//
//  Created by Petra Öhlin on 2015-03-02.
//
//

#ifndef __Ducky_Water__Cell__
#define __Ducky_Water__Cell__

#include <stdio.h>


// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <common/Init.h>
#include <common/Shader.h>

#include "Box.h"

using namespace glm;


struct Particle{
    vec2 pos, speed, force;
    unsigned char r,g,b,a; // Color
    float size, pressure, density;
    int cellIndex;
    
};

class Cell{

    public:
        Cell(){};
        Cell(int i);
        Cell operator()(int i);
        ~Cell();
    
        void addParticle(Particle& p);
        std::vector<Particle*> getParticles();
        void clearParticles();
        void setNeighbours(int index);

        std::vector<int> getNeighbours();
        float getGravity();
        float getPressure();
        float getDensity();
    
        float pressure, density;
        float gravity = -.982;
        void setGravity(float g);
    
     private:
        int index;
        std::vector<Particle*> particles;
        std::vector<int> neighbours;
    
    
    
};


#endif /* defined(__Ducky_Water__Cell__) */
