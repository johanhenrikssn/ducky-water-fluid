//
//  Cell.cpp
//  Ducky-Water
//
//  Created by Petra Öhlin on 2015-03-02.
//
//

#include "Cell.h"



Cell::Cell(int i){
    index = i;
    
    setNeighbours(index);
    
}


int& Cell::operator[](int i){
    return index;
}

int Cell::operator[](int i) const{
    return index;
}

Cell Cell::operator()(int i) {
    Cell newCell(i);
    
    newCell.setNeighbours(i);
    
    return(newCell);
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


void Cell::setNeighbours(int index){
    
    // If index isn't at first column
    if(index % Box::WIDTH != 0)
    {
        neighbours.push_back(index -1);
    }
    
    //If index isn't at first row
    if(index > Box::WIDTH){
        
        neighbours.push_back(index - Box::WIDTH);
    }
    
    // If index isn't at last row
    if (index < Box::WIDTH*Box::HEIGHT-Box::HEIGHT)
    {
        neighbours.push_back(index + Box::WIDTH);
    }
    
    // If index isn't at last column
    if((index+1) % Box::WIDTH != 0)
    {
        neighbours.push_back(index +1);
    }
    
}