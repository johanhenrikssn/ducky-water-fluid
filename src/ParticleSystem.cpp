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
    
    int newParticles = 1000;
    int rowCounter = 0;
    float step = 0.05f;
    
    for(int particleIndex=0; particleIndex<newParticles; particleIndex++){
        
        // For 20 per row
        if(-0.6+step*(particleIndex % 21) >= 0.6)
            rowCounter++;
        
        ParticlesContainer[particleIndex].pos = glm::vec2(-0.6+step*(particleIndex % 21), 0.7f-step*rowCounter);
        
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
    updateGrid();
    calculateDensity();
    //calculatePressure();
    calculateTotalForce();
    integrationStep(delta);
    collisionHandling();
    render();
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
        grid[i].CreateCell(i);
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

//used in density-calculations
float ParticleSystem::densKernel(vec2 p, float h) {
    float r2 = powf(p.x, 2) + powf(p.y, 2);
    float h2 = powf(h, 2);
    
    if(r2 < 0 || r2 > h2) return 0.0f;
    
    return 315.0f / (64.0f* 3.14f * pow(RADIUS, 9) * powf(h2-r2, 3));
}

//Pressure-kernel, used in calculatePressureForce
vec2 ParticleSystem::pressforceKernel(vec2 p, float h) {
    float r = sqrt(powf(p.x, 2)+powf(p.y, 2));
    if(r == 0.0f) return vec2(0.0f, 0.0f);
    
    float t1 = -45.0f / (3.14f * powf(h, 6));
    vec2 t2 = vec2(p.x , p.y)/r;
    float t3 = powf(h-r, 2);
    
    return t1*t2*t3;
    
}
//Viskosity-kernel derivated two times, used in calculateViscosityForce
float ParticleSystem::viscforceKernel(vec2  p, float h) {
    float r = powf(p.x, 2) + powf(p.y, 2);
    return 45.0f / (M_PI * pow(h, 6)) * (h - r);
}

void ParticleSystem::calculateDensity() {
    float densitySum = 0;
    /*
    //For each particle
    for (int i = 0; i < MAX_PARTICLES; i++){
        
        //Vector with neightbouring cells to current particle
        int index = ParticlesContainer[i].cellIndex;
        updateCellIndex();

        std::vector<int> neighbourCells = grid[index].getNeighbours();
        
            //For each neighbouring cell
            for(int j = 0; j < neighbourCells.size(); j++) {
            
                //Vector with particles from current neightbour cell
                std::vector<Particle*> neighbourParticles = grid[neighbourCells.at(j)].getParticles();

                //For every particle in the neighbouring cell, calculate contribution to density
                for(int c = 0; c < neighbourParticles.size(); c++) {
                    vec2 deltaRadius = ParticlesContainer[i].pos - neighbourParticles[c]->pos;
                    densitySum += ParticlesContainer[j].mass * densKernel(deltaRadius, KERNEL_RANGE);
                }
            }
        ParticlesContainer[i].density = densitySum;
    }
    
    */
    float density_sum = 0;
    
    for(int i = 0; i < MAX_PARTICLES; i++){
        
        
        int cellIndex = ParticlesContainer[i].cellIndex;
        
        std::vector<int> current_cells = grid[cellIndex].getNeighbours();
        
        //std::cout << current_cells.size() << std::endl;
        
        for(int j = 0; j < current_cells.size(); j++){
            
            // Loop through all neighbouring particles
            std::vector<Particle*> neighbours = grid[current_cells.at(j)].getParticles();
            
            /*
            // Too many neighbours...
            if(neighbours.size() > KERNEL_LIMIT)
                reduceNeighbours(neighbours);
            */
            
            for(int k = 0; k < neighbours.size(); k++){
                
                //std::cout << "SIZE " << neighbours.size() << std::endl;
                
                //Particle *n = neighbours.at(k);
                
                glm::vec2 diffvec = ParticlesContainer[i].pos - neighbours[k]->pos;;
                
                float abs_diffvec = glm::length(diffvec);
                
                    float h = 16.f;
                
                    density_sum += 500*.14f * (315 / (64*M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)),3.0);
                    //cout << "Density: " << PARTICLE_MASS * (315 / (64 * M_PI * glm::pow(h, 9.0))) * glm::pow((glm::pow(h, 2.0) - glm::pow(abs_diffvec, 2.f)), 3.0) << endl;
                
                
            }
            
        }
        ParticlesContainer[i].density = density_sum;
        ParticlesContainer[i].pressure = (STIFFNESS * (ParticlesContainer[i].density - 1000));
    }
    
    
}

void ParticleSystem::calculatePressure() {
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        
    }
}

void ParticleSystem::calculateTotalForce(){
    
    vec2 pressureForce = {0.0f, 0.0f};
    vec2 vicosityForce = {0.0f, 0.0f};
    
    std::vector<Particle*> neighbourParticles;
    
    //For each particle
    for (int i = 0; i < MAX_PARTICLES; i++){
        
        //Vector with neightbouring cells to current particle
        int index = ParticlesContainer[i].cellIndex;
        std::vector<int> neighbourCells = grid[index].getNeighbours();
        
        //For each neighbouring cell
        for(int j = 0; j < neighbourCells.size(); j++) {
            //Vector with particles from current neightbour cell
            neighbourParticles = grid[neighbourCells.at(j)].getParticles();
            
            //For every particle in the neighbouring cell, calculate contribution to density
            for(int c = 0; c < neighbourParticles.size(); c++) {
                
                vec2 deltaRadius = {0.01f, 0.01f};

                
                //BLIR DIVISON MED 0 OM DET INTE ÄR STATISKA VÄRDEN. TOKIG PRESSURE-CALC.
                pressureForce += ParticlesContainer[i].mass * (ParticlesContainer[i].pressure + neighbourParticles[c]->pressure)/(2*neighbourParticles[c]->pressure)*pressforceKernel(deltaRadius, KERNEL_RANGE);
                
                
                vicosityForce += ETA * ParticlesContainer[i].mass * (neighbourParticles[c]->speed - ParticlesContainer[i].speed)/(neighbourParticles[c]->density)*viscforceKernel(deltaRadius, KERNEL_RANGE);
                
            }
            
        }
        
        neighbourParticles.clear();
        ParticlesContainer[i].force = vicosityForce + pressureForce + vec2(0.0f, -982.f);
           }
    
}

void ParticleSystem::integrationStep(float delta) {
    
    vec2 gravity =vec2(0.0f, -9.82f);
    
    for(int i = 0; i < MAX_PARTICLES; i++) {

        ParticlesContainer[i].speed += (((delta +ParticlesContainer[i].force))/ParticlesContainer[i].density)+ gravity;
        ParticlesContainer[i].pos += delta * ParticlesContainer[i].speed;
    }
}

void ParticleSystem::collisionHandling() {
    
    for(int i = 0; i < MAX_PARTICLES; i++) {
        if(ParticlesContainer[i].pos.x  < -0.5f)
        {
            ParticlesContainer[i].pos.x = -0.5f;
            ParticlesContainer[i].speed.x = -0.5f * ParticlesContainer[i].speed.x;
        }
        else if(ParticlesContainer[i].pos.x > 0.5f)
        {
            ParticlesContainer[i].pos.x = 0.5f;
            ParticlesContainer[i].speed.x = -0.5f * ParticlesContainer[i].speed.x;
        }
        
        if(ParticlesContainer[i].pos.y < -0.5f)
        {
            ParticlesContainer[i].pos.y = -0.5f;
            ParticlesContainer[i].speed.y = -0.5f * ParticlesContainer[i].speed.y;
        }
        else if(ParticlesContainer[i].pos.y > 0.5f)
        {
            ParticlesContainer[i].pos.y = 0.5f;
            ParticlesContainer[i].speed.y = -0.5f * ParticlesContainer[i].speed.y;
        }
    }
}
