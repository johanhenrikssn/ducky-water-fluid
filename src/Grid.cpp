//
//  Grid.cpp
//  Ducky-Water
//
//  Created by Johan Henriksson on 2015-02-24.
//
//

#include "Grid.h"

void Grid::initCell(int i){
    index = i;
    
    //add neighbours here
    
}

void Grid::addParticle(Particle& p){
    particles.push_back(&p);
}

std::vector<Particle*> Grid::getParticles(){
    return particles;
}

void Grid::clearParticles(){
    particles.clear();
}

int Grid::getIndex(){
    return index;
}

void Grid::initGrid(){
    for (int i=0; i < GRIDWIDTH*GRIDHEIGHT; i++) {
        cells[i].initCell(i);
    }
}