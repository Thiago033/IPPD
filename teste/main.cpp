#include <mpi.h>
#include <vector>
#include <iostream>
#include "nbody.h"
#include <SFML/Graphics.hpp>

void initializeBodies(std::vector<Body>& bodies, int n) {
    for (int i = 0; i < n; ++i) {
        bodies.push_back({rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, 0.0,
                          0.0, 0.0, 0.0, 1.0});
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int numBodies = 1000;
    const double dt = 0.01;
    const int numSteps = 100;

    std::vector<Body> bodies;
    if (rank == 0) {
        initializeBodies(bodies, numBodies);
    }

    int bodiesPerProcess = numBodies / size;
    std::vector<Body> localBodies(bodiesPerProcess);

    MPI_Scatter(bodies.data(), bodiesPerProcess * sizeof(Body), MPI_BYTE, localBodies.data(), bodiesPerProcess * sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

    sf::RenderWindow window(sf::VideoMode(800, 800), "N-Body Simulation");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (int step = 0; step < numSteps; ++step) {
            calculateForces(localBodies, dt);
            updatePositions(localBodies, dt);

            MPI_Allgather(localBodies.data(), bodiesPerProcess * sizeof(Body), MPI_BYTE, bodies.data(), bodiesPerProcess * sizeof(Body), MPI_BYTE, MPI_COMM_WORLD);
        }

        if (rank == 0) {
            window.clear();
            for (const auto& body : bodies) {
                sf::CircleShape shape(2.0f);
                shape.setPosition(body.x * 800, body.y * 800);
                window.draw(shape);
            }
            window.display();
        }
    }

    MPI_Finalize();
    return 0;
}
