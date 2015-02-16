//
//  Particle.cpp
//  Ducky-Water
//
//  Created by Mattias Palmgren on 2015-02-15.
//
//

#include "Particle.h"

using namespace glm;

Particle::Particle(){
    position = vec3(0,0,0);
    velocity = vec3(0,0,0);
}


Particle::Particle(vec3 p){
    position = p;
    velocity = vec3(0,0,0);
}

void Particle::setPosition(vec3 p) {
    position = p;
}

vec3 Particle::getPosition() {
    
    return position;
}

void Particle::setVelocity(vec3 v){
    velocity = v;
}

vec3 Particle::getVelocity(){
    return velocity;
}