# particle-simulation

2D particle simulation implemented in C using GLFW and the OpenGL graphics API.

This simulation uses Verlet Integration to compute particle movement.
Collision detection is currently accelerated using a 2D grid structure.

In the future, I might implement a quadtree, but this is currently not planned.


## TODOs

- [ ] Use multithreading to speed up collision computations
