#include <iostream>
#include "Cell.h"


void Cell::CreateCell(int index) {
    cellIndex = index;
    setNeighbours();
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

void Cell::setNeighbours() {
    
    neighbours.push_back(cellIndex);
    switch (cellIndex % Box::COLS) {
        case 0:
            if (cellIndex == 0) {
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS + 1);
            }
            else if (cellIndex == Box::ROWS * ( Box::COLS - 1)) {
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS + 1);
            }
            else {
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS + 1);
                neighbours.push_back(cellIndex - Box::COLS + 1);
            }
            break;
        case (Box::COLS - 1):
            if (cellIndex == Box::COLS - 1) {
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS - 1);
            }
            else if (cellIndex == Box::COLS * Box::ROWS - 1) {
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS - 1);
            }
            else {
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS - 1);
                neighbours.push_back(cellIndex - Box::COLS - 1);
            }
            break;
        default:
            if (cellIndex < Box::COLS) {
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex + Box::COLS - 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS + 1);
            }
            else if (cellIndex > Box::ROWS * (Box::COLS- 1) - 1) {
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex - Box::COLS - 1);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS + 1);
            }
            else {
                
                neighbours.push_back(cellIndex - Box::COLS - 1);
                neighbours.push_back(cellIndex - Box::COLS);
                neighbours.push_back(cellIndex - Box::COLS + 1);
                neighbours.push_back(cellIndex - 1);
                neighbours.push_back(cellIndex + 1);
                neighbours.push_back(cellIndex + Box::COLS - 1);
                neighbours.push_back(cellIndex + Box::COLS);
                neighbours.push_back(cellIndex + Box::COLS + 1);
            }
            break;
    }
}

