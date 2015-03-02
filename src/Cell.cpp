//
//  Cell.cpp
//  Ducky-Water
//
//  Created by Petra Öhlin on 2015-03-02.
//
//

#include "Cell.h"



void Cell::initCell(int i){
    index = i;
    
    setNeighbours(index);
    
}

void Cell::addParticle(Particle& p){
    particles.push_back(&p);
}

std::vector<Particle*> Cell::getParticles(){
    return particles;
}

void Cell::clearParticles(){
    particles.clear();
}

int Cell::getIndex() const{
    return index;
}

void Cell::setNeighbours(int index){
    
    // If index isn't at first column
    if(index % Grid::WIDTH != 0)
    {
        neighbours.push_back(index -1);
    }
    
    //If index isn't at first row
    if(index > Grid::WIDTH){
        
        neighbours.push_back(index - Grid::WIDTH);
    }
    
    // If index isn't at last row
    if (index < Grid::WIDTH*Grid::HEIGHT-Grid::HEIGHT)
    {
        neighbours.push_back(index + Grid::WIDTH);
    }
    
    // If index isn't at last column
    if((index+1) % Grid::WIDTH != 0)
    {
        neighbours.push_back(index +1);
    }
    
}