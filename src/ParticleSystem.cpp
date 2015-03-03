//
//  ParticleSystem.cpp
//  Ducky-Water
//
//  Created by Mattias Palmgren on 2015-02-24.
//
//

#include "ParticleSystem.h"


// Needs to be cleaned up with proper variables
void ParticleSystem::initParticles() {
    
    int newparticles = 100;
    int row_counter = 0;
    double step = 1.2/2000;
    
    for(int particleIndex=0; particleIndex<newparticles; particleIndex++){
        
        // For 20 per row
        if(-0.6+step*(particleIndex % 21) >= 0.6)
            row_counter++;
        
        ParticlesContainer[particleIndex].pos = glm::vec2(-0.6+step*(particleIndex % 21), 0.7f-step*row_counter);
        
        glm::vec2 maindir = glm::vec2(0.0f, 0.005f);
        
        ParticlesContainer[particleIndex].speed = maindir;
        
        // Very bad way to generate a random color
        ParticlesContainer[particleIndex].r = rand() % 256;
        ParticlesContainer[particleIndex].g = rand() % 256;
        ParticlesContainer[particleIndex].b = rand() % 256;
        ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
        
        ParticlesContainer[particleIndex].size = 0.1f;
    }
    initGrid();
}

void ParticleSystem::initBufferData(){

    
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        0.0f, 0.0f, 0.0f,
        
    };
    
    
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    // The VBO containing the positions and sizes of the particles
  
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    
    // The VBO containing the colors of the particles
   
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    
}

int ParticleSystem::updateParticles(double delta){
    
    g_particule_position_size_data = new GLfloat[ParticleSystem::MAX_PARTICLES * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[ParticleSystem::MAX_PARTICLES * 4];
    
    int ParticlesCount = 0;
    
    
    for(int i=0; i<ParticleSystem::MAX_PARTICLES; i++){
        
        Particle& p = ParticlesContainer[i]; // shortcut
        
        // Simulate simple physics : gravity only, no collisions
        
        if(p.pos.y > -0.68f && p.pos.x < 0.68f && p.pos.x > -0.68f)
        {
            updateCellIndex(p);
            calculateDensity(p.cellIndex);
            calculatePressure(p.cellIndex);

            
            std::cout << p.cellIndex << std::endl;
            // Senare: speed hämtas från den aktuella cellen.
            p.speed = vec2(0.0f,grid[p.cellIndex].getGravity()) + vec2(grid[p.cellIndex].getPressure(), grid[p.cellIndex].getPressure()) * (float)delta;
        
            p.pos += p.speed * (float)delta;
            
            
            g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
            g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
            
            
            
            g_particule_position_size_data[4*ParticlesCount+3] = p.size;
            
            g_particule_color_data[4*ParticlesCount+0] = p.r;
            g_particule_color_data[4*ParticlesCount+1] = p.g;
            g_particule_color_data[4*ParticlesCount+2] = p.b;
            g_particule_color_data[4*ParticlesCount+3] = p.a;
            
            ParticlesCount++;
            
            
        }
        else
        {
            p.speed = vec2(0.0f, 0.0f);
            g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
            g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
            
            
            
            g_particule_position_size_data[4*ParticlesCount+3] = p.size;
            
            g_particule_color_data[4*ParticlesCount+0] = p.r;
            g_particule_color_data[4*ParticlesCount+1] = p.g;
            g_particule_color_data[4*ParticlesCount+2] = p.b;
            g_particule_color_data[4*ParticlesCount+3] = p.a;
            ParticlesCount++;
        }
        
    }
    
    updateGrid();

    
    
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
                          0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
                          1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                          4,                                // size : x + y + z + size => 4
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
                          2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                          4,                                // size : r + g + b + a => 4
                          GL_UNSIGNED_BYTE,                 // type
                          GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    return ParticlesCount;
    
}

void ParticleSystem::clean(){
    delete[] g_particule_position_size_data;
    
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
}

void ParticleSystem::updateCellIndex(Particle& p) {
    
    float positionX = (p.pos.x * Box::COLS)/Box::BOX_SIZE;
    positionX = positionX + (Box::COLS)/2;
    
    float positionY = (p.pos.y * Box::ROWS)/Box::BOX_SIZE;
    positionY = positionY + (Box::ROWS)/2;

    if (positionY != 0){
        p.cellIndex = ceil(positionX*positionY);
    }
    else{
        p.cellIndex = ceil(positionX);
    }
    
}

void ParticleSystem::initGrid(){
    
    for (int i=0; i < Box::ROWS*Box::COLS; i++) {
        grid[i](i);
    }
    

    for (int i=0; i < Box::ROWS*Box::COLS; i++) {
        grid[i].setNeighbours(i);
    }
    
}

void ParticleSystem::updateGrid(){
    
    for(int i = 0; i < Box::ROWS * Box::COLS; i++) {
        grid[i].clearParticles();
    }
    
    int tempIndex;
    
    for(int i = 0; i < MAX_PARTICLES; i++){
        tempIndex = ParticlesContainer[i].cellIndex;
        grid[tempIndex].addParticle(ParticlesContainer[i]);
    }
}

void ParticleSystem::calculateDensity(int index) {

    float density = 0;
   
    Cell currentCell = grid[index];

    std::vector<Particle*> cellParticles = currentCell.getParticles();
    
    for (int i=0; i < cellParticles.size(); i++){
                std::vector<int> neighbourCells = currentCell.getNeighbours();

        for(int j = 0; j < neighbourCells.size(); j++) {
            
                std::vector<Particle*> neighbourParticles = grid[neighbourCells.at(j)].getParticles();
                //neighbourParticles.insert(neighbourParticles.end(), cellParticles.begin(), cellParticles.end());
                for(int c = 0; c < neighbourParticles.size(); c++) {
                    
                    Particle *n = neighbourParticles.at(c);
                    vec2 distanceVec = cellParticles.at(i)->pos - n->pos;
                    
                    float distance = length(distanceVec);
                        if(distance < RADIUS) {
                        density += MASS*std::powf((std::powf(RADIUS, 2) - std::powf(distance/100, 2)),3);
                    }
            }
        }
        currentCell.density = density;
        
    }
   // std::cout << density << std::endl;
    
   

    }

void ParticleSystem::calculatePressure(int index) {
    Cell currentCell = grid[index];
    currentCell.pressure = MASS*(IDEAL_DENSITY);
    
}

void ParticleSystem::calculateForces(int index) {
   // vec2 gravity = vec2(f, -9.82f);
    
}


