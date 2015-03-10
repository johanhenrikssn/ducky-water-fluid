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
#include <vector>


using namespace glm;



struct Particle{
    vec2 pos, speed, force;
    unsigned char r,g,b,a; // Color
    float size, pressure, density, mass;
    int cellIndex;
    
};


class Cell {
    
    public:
        void CreateCell(int cellIndex);
        void addParticle(Particle &_particle);
        std::vector<Particle*> getParticles();
        void clearParticles();
        const std::vector<int> &getNeighbours() const;
        void setNeighbours();
    
    private:
        std::vector<Particle*> particles;
        std::vector<int> neighbours;
        int cellIndex;
    

    
    
};
#endif /* defined(__Ducky_Water__Cell__) */
