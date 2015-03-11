#include <iostream>
#include "Cell.h"


void Cell::CreateCell(int index) {
    cellIndex = index;
    setNeighbours(index);
}

void Cell::addParticle(Particle &_particle) {
    particles.push_back(&_particle);
}

std::vector<Particle*> Cell::getParticles() {
    return particles;
}

void Cell::clearParticles() {
    particles.clear();
}

const std::vector<int> &Cell::getNeighbours() const {
    return neighbours;
}

void Cell::setNeighbours(int index) {
    
    neighbours.push_back(index);
    
    // If index isn't at first column
    if(index % Box::COLS  != 0)
    {
        neighbours.push_back(index -1);
    }
    
    //If index isn't at first row
    if(index > Box::COLS){
        
        neighbours.push_back(index - Box::COLS);
    }
    
    // If index isn't at last row
    if (index < Box::COLS*Box::ROWS-Box::ROWS)
    {
        neighbours.push_back(index + Box::COLS);
    }
    
    // If index isn't at last column
    if((index+1) % Box::COLS != 0)
    {
        neighbours.push_back(index +1);
    }
    
    
}

