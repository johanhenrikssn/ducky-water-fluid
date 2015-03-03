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


#include "ParticleSystem.h"
#include "Box.h"

using namespace glm;

class Cell{

    public:
    
        Cell(int i);
        int& operator[](int i);
        int operator[](int i) const;
        Cell operator()(int i);
    
        void addParticle(Particle& p);
        std::vector<Particle*> getParticles();
        void clearParticles();
        void setNeighbours(int index);
        double getGravity;
    
        double gravity;
    
    private:
        int index;
        std::vector<Particle *> particles;
        std::vector<int> neighbours;
    
    
    
};


#endif /* defined(__Ducky_Water__Cell__) */
