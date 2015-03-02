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
#include "Grid.h"

using namespace glm;

class Cell{
    private:
        int index;
        std::vector<Particle *> particles;
        std::vector<int> neighbours;

    public:

        void initCell(int i);

        void addParticle(Particle& p);
        std::vector<Particle*> getParticles();
        void clearParticles();
        int getIndex() const;
        void setNeighbours(int index);
    
    
};


#endif /* defined(__Ducky_Water__Cell__) */
