//
//  Grid.cpp
//  Ducky-Water
//
//  Created by Johan Henriksson on 2015-02-24.
//
//

#include "Grid.h"

void Grid::initCells(){
    
    
    
}

void Grid::setNeighbours(int index){
    
    // If index isn't at first column
    if(index % width =! 0)
    {
        neigbours.push_back(index -1);
    }
    
    //If index isn't at first row
    if(index > width){
        
        neigbours.push_back(index - width);
    }
    
    // If index isn't at last row
    if (index < width*height-width)
    {
        neigbours.push_back(index + width);
    }
    
    // If index isn't at last column
    if((index+1) % width =! 0)
    {
        neigbours.push_back(index +1);
    }
    
    
}