// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct simulation simulation_t;  // opaque record

simulation_t* simulation_create();
int simulation_run(simulation_t* simulation, int argc, char* argv[]);
void simulation_destroy(simulation_t* simulation);

#endif  // SIMULATION_H
