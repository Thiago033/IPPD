#ifndef NBODY_H
#define NBODY_H

#include <vector>

struct Body {
    double x, y, z; // Posições
    double vx, vy, vz; // Velocidades
    double mass; // Massa
};

void calculateForces(std::vector<Body>& bodies, double dt);
void updatePositions(std::vector<Body>& bodies, double dt);

#endif // NBODY_H
