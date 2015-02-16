//
//  Particle.h
//  Ducky-Water
//
//  Created by Mattias Palmgren on 2015-02-15.
//
//

#ifndef __Ducky_Water__Particle__
#define __Ducky_Water__Particle__

#include <stdio.h>

// Include GLM
#include <glm/glm.hpp>

using namespace glm;


#endif /* defined(__Ducky_Water__Particle__) */

class Particle {
    
    public:
        Particle();
        Particle(vec3 p);
    
        void setPosition(vec3 p);
        vec3 getPosition();
        void setVelocity(vec3 v);
        vec3 getVelocity();
    
    protected:
        vec3 position;
        vec3 velocity;
    
};