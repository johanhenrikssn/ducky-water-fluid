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
    
    int newParticles = 100;
    int row_counter = 0;
    double step = 1.2/200;
    
    for(int particleIndex=0; particleIndex<newParticles; particleIndex++){
        
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
        
        //Give particles mass
        ParticlesContainer[particleIndex].mass = PARTICLE_MASS;
        ParticlesContainer[particleIndex].density = 1;
        ParticlesContainer[particleIndex].force = vec2(0,0);
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

void ParticleSystem::render(){
    g_particule_position_size_data = new GLfloat[ParticleSystem::MAX_PARTICLES * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[ParticleSystem::MAX_PARTICLES * 4];
    
    //Render particles
    for (int i = 0; i < MAX_PARTICLES; i++){
        g_particule_position_size_data[4*i+0] = ParticlesContainer[i].pos.x;
        g_particule_position_size_data[4*i+1] = ParticlesContainer[i].pos.y;
        g_particule_position_size_data[4*i+3] = ParticlesContainer[i].size;
        
        g_particule_color_data[4*i+0] = ParticlesContainer[i].r;
        g_particule_color_data[4*i+1] = ParticlesContainer[i].g;
        g_particule_color_data[4*i+2] = ParticlesContainer[i].b;
        g_particule_color_data[4*i+3] = ParticlesContainer[i].a;
    
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, i * sizeof(GLfloat) * 4, g_particule_position_size_data);
        
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, ParticleSystem::MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, i * sizeof(GLubyte) * 4, g_particule_color_data);
        
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
    }
}

void ParticleSystem::updateParticles(float delta){
    updateCellIndex();
    calculateDensity();
    calculatePressure();
    integrationStep(delta);
    collisionHandling();
    render();
    updateGrid();
}

void ParticleSystem::clean(){
    delete[] g_particule_position_size_data;
    
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
}

void ParticleSystem::updateCellIndex() {
    for (int i = 0; i < MAX_PARTICLES; i++){
        
        float positionX = floor((ParticlesContainer[i].pos.x * Box::COLS)/Box::BOX_SIZE);
        float positionY = floor((ParticlesContainer[i].pos.y * Box::ROWS)/Box::BOX_SIZE);
        
        positionX = positionX + Box::COLS/2;
        positionY = Box::ROWS/2 - positionY;
        
        ParticlesContainer[i].cellIndex = positionY*Box::COLS+positionX;
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
    
    for(int i = 0; i < 10; i++){
        tempIndex = ParticlesContainer[i].cellIndex;
        grid[tempIndex].addParticle(ParticlesContainer[i]);
    }
}
float ParticleSystem::kernel(vec2 p, float h) {
    float r2 = powf(p.x, 2) + powf(p.y, 2);
    float h2 = powf(h, 2);
    
    if(r2 < 0 || r2 > h2) return 0.0f;
    
    return 315.0f / (64.0f* 3.14f * pow(RADIUS, 9) * powf(h2-r2, 3));
}

vec2 ParticleSystem::gradKernel(vec2 p, float h) {
    float r = sqrt(powf(p.x, 2)+powf(p.y, 2));
    if(r == 0.0f) return vec2(0.0f, 0.0f);
    
    float t1 = -45.0f / (3.14f * powf(h, 6));
    vec2 t2 = vec2(p.x , p.y)/r;
    float t3 = powf(h-r, 2);
    
    return t1*t2*t3;
    
}

float ParticleSystem::laplaceKernel(vec2  p, float h) {
    float r = powf(p.x, 2) + powf(p.y, 2);
    return 45.0f / (M_PI * pow(h, 6)) * (h - r);
}

void ParticleSystem::calculateDensity() {
    float densitySum = 0;
    
    //For each particle
    for (int i = 0; i < MAX_PARTICLES; i++){
        
        //Vector with neightbouring cells to current particle
        int index = ParticlesContainer[i].cellIndex;
        std::vector<int> neighbourCells = grid[index].getNeighbours();
        
            //For each neighbouring cell
            for(int j = 0; j < neighbourCells.size(); j++) {
            
                //Vector with particles from current neightbour cell
                std::vector<Particle*> neighbourParticles = grid[neighbourCells.at(j)].getParticles();

                //For every particle in the neighbouring cell, calculate contribution to density
                for(int c = 0; c < neighbourParticles.size(); c++) {
                    vec2 deltaRadius = ParticlesContainer[i].pos - neighbourParticles[c]->pos;
                    densitySum += ParticlesContainer[j].mass * kernel(deltaRadius, KERNEL_RANGE);
                }
            }
        ParticlesContainer[i].density = densitySum;
    }
}

void ParticleSystem::calculatePressure() {
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        ParticlesContainer[i].pressure = max(STIFFNESS * (ParticlesContainer[i].density - REST_DENSITY), 0.0f);
    }
}

void ParticleSystem::integrationStep(float delta) {
    
    for(int i = 0; i < MAX_PARTICLES; i++) {
        ParticlesContainer[i].speed += delta * ParticlesContainer[i].force; /// ParticlesContainer[i].density;
        ParticlesContainer[i].pos += delta * ParticlesContainer[i].speed;
    }
}

void ParticleSystem::collisionHandling() {
    
    for(int i = 0; i < MAX_PARTICLES; i++) {
        if(ParticlesContainer[i].pos.x  < -0.8f)
        {
            ParticlesContainer[i].pos.x = -0.8f;
            ParticlesContainer[i].speed.x = -0.5f * ParticlesContainer[i].speed.x;
        }
        else if(ParticlesContainer[i].pos.x > 0.8f)
        {
            ParticlesContainer[i].pos.x = 0.8f;
            ParticlesContainer[i].speed.x = -0.5f * ParticlesContainer[i].speed.x;
        }
        
        if(ParticlesContainer[i].pos.y < -0.8f)
        {
            ParticlesContainer[i].pos.y = -0.8f;
            ParticlesContainer[i].speed.y = -0.5f * ParticlesContainer[i].speed.y;
        }
        else if(ParticlesContainer[i].pos.y > 0.8f)
        {
            ParticlesContainer[i].pos.y = 0.8f;
            ParticlesContainer[i].speed.y = -0.5f * ParticlesContainer[i].speed.y;
        }
    }
}
