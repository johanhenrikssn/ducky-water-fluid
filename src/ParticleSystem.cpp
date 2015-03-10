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
    double step = 1.2/20;
    
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

int ParticleSystem::updateParticles(float delta){
    
    g_particule_position_size_data = new GLfloat[ParticleSystem::MAX_PARTICLES * 4];
    static GLubyte* g_particule_color_data         = new GLubyte[ParticleSystem::MAX_PARTICLES * 4];
    
    int ParticlesCount = 0;
    
    
    for(int i=0; i<ParticleSystem::MAX_PARTICLES; i++){
        
        Particle& p = ParticlesContainer[i]; // shortcut
            updateCellIndex(p);
            calculateDensity(p.cellIndex);
            calculatePressure();
            
            
            std::cout << p.pressure << std::endl;
            
            //p.speed = vec2(0.0f,-0.00981f) * (float)delta * 0.5f;
            p.speed = vec2(0.0f,grid[p.cellIndex].getGravity());
        
                       
            //implement euler
            p.pos += (p.speed) * delta;
        
            collisionHandling();
        
            //p.pos -= vec2(0.0f,0.50f) * (float)delta;
            g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
            g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
            
            g_particule_position_size_data[4*ParticlesCount+3] = p.size;
            
            g_particule_color_data[4*ParticlesCount+0] = p.r;
            g_particule_color_data[4*ParticlesCount+1] = p.g;
            g_particule_color_data[4*ParticlesCount+2] = p.b;
            g_particule_color_data[4*ParticlesCount+3] = p.a;
            
            ParticlesCount++;
            
            
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
    
    float positionX = floor((p.pos.x * Box::COLS)/Box::BOX_SIZE);
    float positionY = floor((p.pos.y * Box::ROWS)/Box::BOX_SIZE);
    
    positionX = positionX + Box::COLS/2;
    positionY = Box::ROWS/2 - positionY;
    
    p.cellIndex = positionY*Box::COLS+positionX;
    
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
float ParticleSystem::kernel(vec2 p) {
    float r2 = powf(p.x, 2) + powf(p.y, 2);
    float h2 = powf(RADIUS, 2);
    
    if(r2 < 0 || r2 > h2) return 0.0f;
    
    return 315.0f / (64.0f* 3.14f * pow(RADIUS, 9) * powf(h2-r2, 3));
}

vec2 ParticleSystem::gradKernel(Particle &p) {
    float r = sqrt(powf(p.pos.x, 2)+powf(p.pos.y, 2));
    if(r == 0.0f) return vec2(0.0f, 0.0f);
    
    float t1 = -45.0f / (3.14f * powf(RADIUS, 6));
    vec2 t2 = vec2(p.pos.x , p.pos.y)/r;
    float t3 = powf(RADIUS-r, 2);
    
    return t1*t2*t3;
    
}

float ParticleSystem::laplaceKernel(Particle &p) {
    float r = powf(p.pos.x, 2) + powf(p.pos.y, 2);
    return 45.0f / (M_PI * pow(RADIUS, 6)) * (RADIUS - r);
}

void ParticleSystem::calculateDensity(int index) {
    
    
    float density = 0;
    
    std::vector<Particle*> cellParticles = grid[index].getParticles();
    
    for (int i=0; i < cellParticles.size(); i++){
        std::vector<int> neighbourCells = grid[index].getNeighbours();
        
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
        
        grid[index].density = density;
        
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
        ParticlesContainer[i].speed += delta * ParticlesContainer[i].force / ParticlesContainer[i].density;
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

