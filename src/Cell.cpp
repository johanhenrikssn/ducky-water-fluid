//
//  Cell.cpp
//  Ducky-Water
//
//  Created by Petra Öhlin on 2015-03-02.
//
//

#include "Cell.h"

Cell::~Cell() {
    particles.clear();
}

Cell::Cell(int i){
    
    index = i;
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


float Cell::getGravity(){
    return gravity;
}

float Cell::getPressure() {
    return pressure;
}

float Cell::getDensity() {
    return density;
}

void Cell::setGravity(float g){
    
    gravity = g;
    
}

Cell Cell::operator()(int i) {
    Cell newCell(i);
        
    return newCell;

}


void Cell::setNeighbours(int index){
    
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

std::vector<int> Cell::getNeighbours() {
    return neighbours;
}