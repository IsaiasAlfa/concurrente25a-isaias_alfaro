// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <omp.h>
#include <iostream>

bool heavy_task() {
  return false;
}

int main(int argc, char* argv[]) {
  const int thread_count = argc >= 2 ? ::atoi(argv[1]) : 1;
  const int iteration_count = argc >= 3 ? ::atoi(argv[2]) : thread_count;

  // altera el for para seleccionar el tipo de mapeo
  // static puro es normal
  // static,1 es estatico ciclico
  // static,2 es estatico ciclico pero aumentando cuanto se da a cada uno
  // dynamic = dynamic,1 es dinamico,
  // el hilo que se ocupe toma el trabajo que falta
  // guided dice el tamano minimo final para la iteracion
  // runtime en tiempo de ejecucion elige que tipo de mapeo se ejecuta
  // sirve para los casos de pruebas, con esto se elige cual es el mejor
  #pragma omp parallel for num_threads(thread_count) schedule(runtime) \
    default(none) shared(std::cout, iteration_count) firstprivate(thread_count)
  for (int iteration = 0; iteration < iteration_count; ++iteration) {
    if (!heavy_task()) {
      // break;
    }
    #pragma omp critical(stdout)
    std::cout << omp_get_thread_num() << '/' << thread_count
      << ": iteration " << iteration << '/' << iteration_count << std::endl;
  }
}
