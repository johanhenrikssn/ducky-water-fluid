//
//  Grid.cpp
//  Ducky-Water
//
//  Created by Johan Henriksson on 2015-02-24.
//
//

#include "Grid.h"

void Grid::initGrid(){
    for (int i=0; i < Box::WIDTH*Box::HEIGHT; i++) {
        cells[i](i);
    }
}
