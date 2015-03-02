//
//  Grid.h
//  Ducky-Water
//
//  Created by Johan Henriksson on 2015-02-24.
//
//

#ifndef __Ducky_Water__Grid__
#define __Ducky_Water__Grid__

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
#include "Cell.h"

using namespace glm;

class Grid{    
    public:
        
        void initGrid();
        Cell cells[Box::COLS*Box::ROWS];
    
    
};


#endif /* defined(__Ducky_Water__Grid__) */
