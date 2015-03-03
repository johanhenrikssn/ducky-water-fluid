//
//  Box.h
//  Ducky-Water
//
//  Created by Carl Englund on 24/02/15.
//
//

#ifndef __Ducky_Water__Box__
#define __Ducky_Water__Box__

#include <stdio.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

class Box
{
    public:
        void draw();
    
        const static int ROWS = 24;
        const static int COLS = 24;
    
        const static float BOX_POSITIVE;
        const static float BOX_NEGATIVE;
    
        const static float BOX_SIZE;

    
};
#endif /* defined(__Ducky_Water__Box__) */
