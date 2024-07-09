#include "nbody.h"
#include <cmath>
#include <omp.h>

void calculateForces(std::vector<Body>& bodies, double dt) {
    const double G = 6.67430e-11; // Constante gravitacional
    int n = bodies.size();

    #pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        double fx = 0, fy = 0, fz = 0;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double distance = std::sqrt(dx*dx + dy*dy + dz*dz);
                double force = (G * bodies[i].mass * bodies[j].mass) / (distance * distance * distance);
                fx += force * dx;
                fy += force * dy;
                fz += force * dz;
            }
        }
        bodies[i].vx += fx / bodies[i].mass * dt;
        bodies[i].vy += fy / bodies[i].mass * dt;
        bodies[i].vz += fz / bodies[i].mass * dt;
    }
}

void updatePositions(std::vector<Body>& bodies, double dt) {
    #pragma omp parallel for
    for (int i = 0; i < bodies.size(); ++i) {
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}
